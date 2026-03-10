CC = gcc
DEBUG = 0
BUILD = ./build

ifeq ($(DEBUG), 0)
FLAGS = -std=c11 -Wall -Wextra
else
FLAGS = -g -std=c11 -Wall -Wextra
endif

skester: main.o
	$(CC) $(BUILD)/*.o -o $(BUILD)/skester

main.o: ./src/main.c
	$(CC) $(FLAGS) -c ./src/main.c -o $(BUILD)/main.o

clean:
	rm -f $(BUILD)/*
	@echo "Cleaned Build Files..."

run:
	make
	@echo " "
	$(BUILD)/skester

debug:
	make DEBUG=1
	@echo " "

dummy:
	gcc ./tests/dummy_cases.c -o $(BUILD)/tests
	@echo "compiled tests!"

