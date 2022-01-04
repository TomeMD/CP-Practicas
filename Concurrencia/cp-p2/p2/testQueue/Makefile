CC=gcc
CFLAGS=-Wall -pthread -g
LIBS=
OBJS= options.o queue.o testQueue.o

PROGS= test

all: $(PROGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

test: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(PROGS) *.o *~

