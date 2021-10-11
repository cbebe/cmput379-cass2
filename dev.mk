# Development makefile
SRC=main tands
SAMPLE_INPUT=inputexample
OUTPUT=*.log
APP=prodcon
ZIP_NAME=Assignment2.zip

CFLAGS=-Wall -Wextra -Wpedantic
OBJ=$(SRC:%=build/%.o)
BUILD=build

all: $(APP) $(SAMPLE_INPUT)

# example input given in the assignment specifications
$(SAMPLE_INPUT):
	echo "T4\nT2\nT1\nT1\nS9\nT5\nS1\nT1\nT1\nT1" > $@

run: $(APP)
	./$<

prodcon: $(OBJ)
	gcc $(CFLAGS) -o $@ $^

$(BUILD)/%.o: src/%.c $(BUILD)
	gcc $(CFLAGS) -o $@ -c $<

$(BUILD):
	mkdir $@

zip: clean
	$(MAKE) clean
	zip -r ../$(ZIP_NAME) .

clean:
	rm -f $(APP) $(SAMPLE_INPUT) $(OUTPUT)
	rm -rf $(BUILD)