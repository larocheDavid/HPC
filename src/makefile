CC = mpicc
CFLAGS = -Wall -Wextra
LDFLAGS =

SRCS = laplace.c
OBJS = $(SRCS:.c=.o)
EXEC = ../laplace.out
GRID = ../laplace.txt

all: clean build

build:
	$(CC) $(CFLAGS) -c laplace.c -o laplace.o
	$(CC) $(LDFLAGS) $(OBJS) -o $(EXEC)
clean:
	rm -f $(EXEC) $(OBJS) $(GRID)

