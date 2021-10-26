# Submission Makefile
# Run `make` to create the minimal output for this assignment

CFLAGS=-Wall -Wextra -Wpedantic
LDFLAGS=-lpthread
APP=prodcon
OBJ=main.o tands.o job_queue.o logger.o

.PHONY: all debug clean noopt

all: CFLAGS += -O
all: clean $(APP)

debug: CFLAGS += -g
debug: clean $(APP)

noopt: clean $(APP)

$(APP): $(OBJ)
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: src/%.c
	gcc $(CFLAGS) -o $@ -c $< $(LDFLAGS)

clean:
	rm -f $(APP) $(OBJ)
