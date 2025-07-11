# Makefile to build the "4each" executable

CC := gcc
CFLAGS := -Wall -Wextra -O2
TARGET := 4each
SRC := foreach.c
OBJS = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean