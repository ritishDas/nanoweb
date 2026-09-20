#include "server/routeTrie.h"
#include "dataStructure/khash.h"
#include "dataStructure/vector.h"
#include "hashmap.h"
#include "server/server.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

static char *copyChars(const char *data, size_t len) {
  char *copy = malloc(len + 1);
  if (!copy)
    return NULL;

  memcpy(copy, data, len);
  copy[len] = '\0';
  return copy;
}

static int setNodePath(TrieNode *node, const char *data, size_t len) {
  char *copy = copyChars(data, len);
  if (!copy)
    return 0;

  free(node->path.data);
  node->path.data = copy;
  node->path.size = len;
  return 1;
}

static void sendResponse(int receiverFd, int statusCode,
                         const char *contentType, const char *body) {
  char response[1024];
  size_t responseLen = sizeof(response);

  newServerResponse(response, &responseLen, statusCode, contentType, body);

  size_t sent = 0;
  while (sent < responseLen) {
    ssize_t written = send(receiverFd, response + sent, responseLen - sent, 0);
    if (written <= 0)
      return;

    sent += (size_t)written;
  }
}

static _Bool methodMatches(MethodType methodType, const char *method,
                           size_t methodSize) {
  switch (methodType) {
  case GET:
    return methodSize == 3 && memcmp(method, "GET", 3) == 0;
  case POST:
    return methodSize == 4 && memcmp(method, "POST", 4) == 0;
  case PUT:
    return methodSize == 3 && memcmp(method, "PUT", 3) == 0;
  case DELETE:
    return methodSize == 6 && memcmp(method, "DELETE", 6) == 0;
  case PATCH:
    return methodSize == 5 && memcmp(method, "PATCH", 5) == 0;
  case QUERY:
    return methodSize == 5 && memcmp(method, "QUERY", 5) == 0;
  }

  return 0;
}

TrieNode *RouteNodeInit(void) {
  TrieNode *newNode = malloc(sizeof(TrieNode));
  if (!newNode)
    return NULL;

  newNode->path.data = nullptr;
  newNode->path.size = 0;
  newNode->complete = 0;
  newNode->children = newMap();
  newNode->dynamicChildren = nullptr;
  newNode->method = NULL;

  if (!newNode->children ||
      !setNodePath(newNode, "nanoweb", strlen("nanoweb"))) {
    destroyMap(newNode->children);
    free(newNode);
    return NULL;
  }

  return newNode;
}

void RouteNodeFree(TrieNode *tn) {
  if (!tn)
    return;

  for (khiter_t k = kh_begin(tn->children); k != kh_end(tn->children); ++k) {

    if (kh_exist(tn->children, k)) {
      TrieNode *child = kh_value(tn->children, k);
      RouteNodeFree(child);
    }
  }

  if (tn->dynamicChildren) {

    for (khiter_t k = kh_begin(tn->dynamicChildren);
         k != kh_end(tn->dynamicChildren); k++) {

      if (kh_exist(tn->dynamicChildren, k)) {
        TrieNode *child = kh_value(tn->dynamicChildren, k);
        RouteNodeFree(child);
      }
    }
    destroyMap(tn->dynamicChildren);
  }

  destroyMap(tn->children);
  free(tn->method);
  free(tn->path.data);
  free(tn);
}

void pathSeparator(StringVec *vec, String path) {
  size_t start = 0;

  while (start < path.size && path.data[start] == '/') {
    start++;
  }

  for (size_t i = start; i <= path.size; i++) {
    if (i == path.size || path.data[i] == '/') {
      size_t len = i - start;
      if (len > 0) {
        char *segment = malloc(len + 1);
        if (!segment)
          return;

        memcpy(segment, path.data + start, len);
        segment[len] = '\0';
        VECTOR_PUSH(vec, segment);
      }
      start = i + 1;
    }
  }
}

void freeStringVec(StringVec *vec) {

  for (size_t i = 0; i < vec->size; i++) {
    free(vec->data[i]);
  }
}

void addMethod(TrieNode *nanoweb, char *path, Method *method) {
  if (!nanoweb || !path || !method)
    return;

  StringVec pathVec;
  VECTOR_INIT(&pathVec);

  String p = {.data = path, .size = strlen(path)};

  pathSeparator(&pathVec, p);

  TrieNode *temp = nanoweb;

  for (size_t i = 0; i < pathVec.size; i++) {

    if (pathVec.data[i][0] != ':') {

      TrieNode *temp2 = checkKey(temp->children, pathVec.data[i]);
      if (!temp2) {

        TrieNode *newNode = RouteNodeInit();
        if (!newNode ||
            !setNodePath(newNode, pathVec.data[i], strlen(pathVec.data[i]))) {
          RouteNodeFree(newNode);
          break;
        }

        insertMap(temp->children, pathVec.data[i], newNode);
        temp = newNode;
      } else {
        temp = temp2;
      }

    } else {
      if (!temp->dynamicChildren)
        temp->dynamicChildren = newMap();

      TrieNode *temp2 = checkKey(temp->dynamicChildren, pathVec.data[i]);

      if (!temp2) {

        TrieNode *newNode = RouteNodeInit();
        if (!newNode ||
            !setNodePath(newNode, pathVec.data[i], strlen(pathVec.data[i]))) {
          RouteNodeFree(newNode);
          break;
        }

        insertMap(temp->dynamicChildren, pathVec.data[i], newNode);
        temp = newNode;
      } else {
        temp = temp2;
      }
    }

    if (i == pathVec.size - 1) {
      Method *met = malloc(sizeof(Method));
      if (!met)
        break;

      *met = *method;
      free(temp->method);
      temp->method = met;
      temp->complete = 1;
    }
  }

  freeStringVec(&pathVec);
  VECTOR_FREE(&pathVec);
}

static TrieNode *matchRoute(Request *userReq, TrieNode *node,
                            StringVec *pathVec, size_t index) {
  if (!node)
    return NULL;

  if (index == pathVec->size) {
    if (node->complete)
      return node;

    return NULL;
  }

  TrieNode *staticNode = checkKey(node->children, pathVec->data[index]);

  if (staticNode) {
    TrieNode *result = matchRoute(userReq, staticNode, pathVec, index + 1);

    if (result)
      return result;
  }

  if (node->dynamicChildren) {

    khiter_t k = kh_begin(node->dynamicChildren);

    for (; k != kh_end(node->dynamicChildren); ++k) {

      if (!kh_exist(node->dynamicChildren, k))
        continue;

      TrieNode *dynamicNode = kh_value(node->dynamicChildren, k);
      khash_t(2) *previousParams = userReq->params;
      userReq->params = copyMap2(previousParams);
      if (!userReq->params) {
        userReq->params = previousParams;
        return NULL;
      }

      if (dynamicNode->path.size > 1 && dynamicNode->path.data[0] == ':') {
        insertCharMap(userReq->params, dynamicNode->path.data + 1,
                      pathVec->data[index]);
      }

      TrieNode *result = matchRoute(userReq, dynamicNode, pathVec, index + 1);

      if (result) {
        destroyMap2(previousParams);
        return result;
      }

      destroyMap2(userReq->params);
      userReq->params = previousParams;
    }
  }

  return NULL;
}

void routeMatcher(Request *userReq, TrieNode *nanoweb, String p,
                  int receiverFd) {

  StringVec pathVec;
  VECTOR_INIT(&pathVec);

  pathSeparator(&pathVec, p);

  TrieNode *temp = matchRoute(userReq, nanoweb, &pathVec, 0);

  if (!temp) {
    sendResponse(receiverFd, 404, "text/plain", "Not Found");
    freeStringVec(&pathVec);
    VECTOR_FREE(&pathVec);
    return;
  }

  if (!methodMatches(temp->method->type, userReq->method.data,
                     userReq->method.size)) {
    sendResponse(receiverFd, 405, "text/plain", "Method Not Allowed");
    freeStringVec(&pathVec);
    VECTOR_FREE(&pathVec);
    return;
  }

  ControllerRes response = temp->method->handler();
  if (!response.res) {
    sendResponse(receiverFd, 500, "text/plain", "Internal Server Error");
    freeStringVec(&pathVec);
    VECTOR_FREE(&pathVec);
    return;
  }

  size_t sent = 0;
  while (sent < response.reslen) {
    ssize_t written =
        send(receiverFd, response.res + sent, response.reslen - sent, 0);
    if (written <= 0)
      break;

    sent += (size_t)written;
  }

  freeStringVec(&pathVec);
  VECTOR_FREE(&pathVec);
  free(response.res);
}
