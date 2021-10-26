# Development makefile
SRC=main tands job_queue logger
SAMPLE_INPUT=inputexample
OUTPUT=*.log
APP=prodcon
ZIP_NAME=Assignment2.zip

CFLAGS=-Wall -Wextra -Wpedantic
LDFLAGS=-lpthread
BUILD=build
OBJ=$(SRC:%=$(BUILD)/%.o)


all: CFLAGS += -O
all: $(APP) $(SAMPLE_INPUT)

debug: CFLAGS += -g
debug: $(APP)

# example input given in the assignment specifications
$(SAMPLE_INPUT):
	echo "T4\nT2\nT1\nT1\nS9\nT5\nS1\nT1\nT1\nT1" > $@

run: CFLAGS += -g
run: $(APP) $(SAMPLE_INPUT)
	./$< 8 < $(SAMPLE_INPUT)

prodcon: $(OBJ)
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD)/%.o: src/%.c $(BUILD)
	gcc $(CFLAGS) -o $@ -c $< $(LDFLAGS)

$(BUILD):
	mkdir $@

zip:
	zip -r $(ZIP_NAME) src Makefile dev.mk README

clean:
	rm -f $(APP) $(SAMPLE_INPUT) $(OUTPUT)
	rm -rf $(BUILD)

.PHONY: all debug run zip clean