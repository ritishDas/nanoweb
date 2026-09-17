CC        := gcc
CFLAGS    := -std=c2x -Wall -Wextra -Werror -Iinclude -MMD -MP -O2
LDFLAGS   := -lpq

TARGET    := bin/app

SRCS      := $(shell find src -name "*.c")
OBJS      := $(SRCS:src/%.c=obj/%.o)
DEPS      := $(OBJS:.o=.d)

OBJ_DIRS  := $(sort $(dir $(OBJS))) bin

.PHONY: all clean

all: $(TARGET) 

run: $(TARGET) 
	./bin/app

$(TARGET): $(OBJS) | bin
	@echo "Linking: $@"
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

obj/%.o: src/%.c | $(OBJ_DIRS)
	@echo "Compiling: $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIRS):
	@mkdir -p $@

-include $(DEPS)

clean:
	@rm -rf obj bin
	@echo "Cleaned."
