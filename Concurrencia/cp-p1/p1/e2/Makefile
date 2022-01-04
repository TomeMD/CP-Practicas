
CC=gcc
CFLAGS=-Wall -pthread -g
LIBS=
OBJS=swap2.o options.o

PROGS= swap2

all: $(PROGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

swap2: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(PROGS) *.o *~

