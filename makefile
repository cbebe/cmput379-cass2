CC=gcc
CFLAGS=-g -Wall
OBJS=shared_memory.o
EXE=writeshmem.exe readshmem.exe destroy_shmem.exe

all: $(EXE)

%.exe: %.o $(OBJS)
	$(CC) $(CFLAGS) $< $(OBJS) -o $@


%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXE) *.o

