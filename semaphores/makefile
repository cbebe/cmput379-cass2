CC=gcc
CFLAGS=-g -Wall
LDFLAGS=-lpthread
OBJS=shared_memory.o
EXE=writeshmem.exe readshmem.exe destroy_shmem.exe

all: $(EXE)

%.exe: %.o $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJS) $(LDFLAGS) -o $@


%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< $(LDFLAGS) -o $@

clean:
	rm -f $(EXE) *.o

