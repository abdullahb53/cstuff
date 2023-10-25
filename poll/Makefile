CC = gcc

# Define the source file and the output binary
SRC = main.c
OUT = main

# Default target: compile the program
all: $(OUT)

# Compile the program
$(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT)

# Target to run the program
run: $(OUT)
	./$(OUT)

# Clean target: remove the binary
clean:
	rm -f $(OUT)