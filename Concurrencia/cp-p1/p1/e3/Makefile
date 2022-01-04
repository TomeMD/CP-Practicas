
CC=gcc
CFLAGS=-Wall -pthread -g
LIBS=
OBJS=swap3.o options.o

PROGS= swap3

all: $(PROGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

swap3: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(PROGS) *.o *~

