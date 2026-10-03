CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -Iinclude -g -fsanitize=address -pthread
SRC = src/arena.c src/metrics.c src/server.c src/main.c
TARGET = build/sysmon_server

all: $(TARGET)

$(TARGET): $(SRC)
@mkdir -p build
$(CC) $(CFLAGS) $^ -o $@

clean:
rm -rf build

.PHONY: all clean
