CC = gcc
CFLAGS = -Wall -Wextra -pthread

TARGET = chash
SRCS = chash.c hash_table.c linked_list.c output.c rw_lock.c thread_manager.c 
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)