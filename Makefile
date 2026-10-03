CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -Iinclude -pthread
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = sysmon

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)
