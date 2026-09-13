// #include <errno.h>
// #include <fcntl.h>
// #include <netinet/in.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/epoll.h>
// #include <sys/socket.h>
// #include <unistd.h>
//
// #define MAX_EVENTS 10
// #define PORT 8080
// #define BUFFER_SIZE 512
//
// static int set_nonblocking(int fd) {
//   int flags = fcntl(fd, F_GETFL, 0);
//   if (flags == -1)
//     return -1;
//   return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
// }
//
// static void handle_client(int epollfd, int fd) {
//   char buf[BUFFER_SIZE];
//
//   // Edge-triggered (EPOLLET) requires reading until EAGAIN/EWOULDBLOCK
//   while (1) {
//     ssize_t bytes_read = read(fd, buf, sizeof(buf));
//
//     if (bytes_read == -1) {
//       if (errno == EAGAIN || errno == EWOULDBLOCK) {
//         break; // Drained all incoming data for this event
//       }
//       perror("read error");
//       close(fd);
//       break;
//     }
//
//     if (bytes_read == 0) {
//       // Client closed connection
//       printf("Client on fd %d disconnected\n", fd);
//       epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
//       close(fd);
//       break;
//     }
//
//     // Echo back to client
//     int out = write(fd, buf, bytes_read);
//     printf("%d", out);
//   }
// }
//
// int main(void) {
//   struct epoll_event ev, events[MAX_EVENTS];
//   int listen_sock, conn_sock, nfds, epollfd;
//   struct sockaddr_in addr;
//   socklen_t addrlen = sizeof(addr);
//
//   // 1. Create and bind listening socket
//   listen_sock = socket(AF_INET, SOCK_STREAM, 0);
//   if (listen_sock == -1) {
//     perror("socket");
//     exit(EXIT_FAILURE);
//   }
//
//   int opt = 1;
//   setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//
//   memset(&addr, 0, sizeof(addr));
//   addr.sin_family = AF_INET;
//   addr.sin_addr.s_addr = INADDR_ANY;
//   addr.sin_port = htons(PORT);
//
//   if (bind(listen_sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
//     perror("bind");
//     close(listen_sock);
//     exit(EXIT_FAILURE);
//   }
//
//   if (listen(listen_sock, SOMAXCONN) == -1) {
//     perror("listen");
//     close(listen_sock);
//     exit(EXIT_FAILURE);
//   }
//
//   set_nonblocking(listen_sock);
//
//   // 2. Initialize epoll instance
//   epollfd = epoll_create1(0);
//   if (epollfd == -1) {
//     perror("epoll_create1");
//     close(listen_sock);
//     exit(EXIT_FAILURE);
//   }
//
//   ev.events = EPOLLIN;
//   ev.data.fd = listen_sock;
//   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
//     perror("epoll_ctl: listen_sock");
//     close(listen_sock);
//     close(epollfd);
//     exit(EXIT_FAILURE);
//   }
//
//   printf("Server listening on port %d...\n", PORT);
//
//   // 3. Event loop
//   for (;;) {
//     nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
//     if (nfds == -1) {
//       if (errno == EINTR)
//         continue; // Interrupted by signal
//       perror("epoll_wait");
//       break;
//     }
//
//     for (int n = 0; n < nfds; ++n) {
//       if (events[n].data.fd == listen_sock) {
//         // Accept incoming connection(s)
//         conn_sock = accept(listen_sock, (struct sockaddr *)&addr, &addrlen);
//         if (conn_sock == -1) {
//           perror("accept");
//           continue;
//         }
//
//         if (set_nonblocking(conn_sock) == -1) {
//           perror("set_nonblocking");
//           close(conn_sock);
//           continue;
//         }
//
//         ev.events = EPOLLIN | EPOLLET;
//         ev.data.fd = conn_sock;
//         if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
//           perror("epoll_ctl: conn_sock");
//           close(conn_sock);
//           continue;
//         }
//
//         printf("New connection established: fd %d\n", conn_sock);
//       } else {
//         handle_client(epollfd, events[n].data.fd);
//       }
//     }
//   }
//
//   close(listen_sock);
//   close(epollfd);
//   return 0;
// }
