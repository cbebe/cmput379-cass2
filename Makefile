# Submission Makefile
# Run `make` to create the minimal output for this assignment

CFLAGS=-Wall -Wextra -Wpedantic
LDFLAGS=-lpthread
APP=prodcon
OBJ=main.o tands.o queue.o

all: CFLAGS += -O
all: $(APP)

debug: CFLAGS += -g
debug: $(APP)

$(APP): $(OBJ)
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: src/%.c
	gcc $(CFLAGS) -o $@ -c $< $(LDFLAGS)

clean:
	rm -f $(APP) $(OBJ)
