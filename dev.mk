# Development makefile

CFLAGS=-Wall -Wextra -Wpedantic

SRC=main tands
OBJ=$(SRC:%=build/%.o)

BUILD=build

all: prodcon

run: prodcon
	./prodcon

prodcon: $(OBJ)
	gcc $(CFLAGS) -o $@ $^

$(BUILD)/%.o: src/%.c $(BUILD)
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD):
	mkdir $@

zip: clean
	$(MAKE) clean
	zip -r ../Assignment2.zip .

clean:
	rm -f prodcon
	rm -rf $(BUILD)