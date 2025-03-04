CC=gcc
FLAGS=-Wall -O3

BUILD_DIR=dist
SRC_DIR=src

run: all
	$(BUILD_DIR)/program

all: $(BUILD_DIR) $(SRC_DIR)/*.c
	$(CC) $(FLAGS) $(SRC_DIR)/*.c -o $(BUILD_DIR)/program


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
