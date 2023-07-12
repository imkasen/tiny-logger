CC := g++
OUT := test

default: *.cpp
	$(CC) *.cpp -o $(OUT) -std=c++11

.PHONY: clean
clean:
	@rm $(OUT)
