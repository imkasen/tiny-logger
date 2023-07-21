CC := g++
OUT := logger

default: main.cpp tiny_logger.cpp
	$(CC) main.cpp tiny_logger.cpp -o $(OUT) -pthread -std=c++14

.PHONY: clean
clean:
	@rm -f $(OUT)
