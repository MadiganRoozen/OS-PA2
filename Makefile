CC = gcc
CFLAGS = -Wall -Wextra -pthread

TARGET = chash.exe
SRCS = chash.c hash_table.c output.c rw_lock.c thread_manager.c 
OBJS = $(SRCS:.c=.o)
INPUT = commands.txt

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET) $(INPUT)