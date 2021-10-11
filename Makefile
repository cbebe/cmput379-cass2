# Submission Makefile
# Run `make` to create the minimal output for this assignment

CFLAGS=-Wall -Wextra -Wpedantic
APP=prodcon
OBJ=main.o tands.o

all: CFLAGS += -O
all: $(APP)

debug: CFLAGS += -g
debug: $(APP)

$(APP): $(OBJ)
	gcc $(CFLAGS) -o $@ $^

%.o: src/%.c
	gcc $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(APP) $(OBJ)
