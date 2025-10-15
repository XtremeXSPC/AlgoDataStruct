.PHONY: build run clean test

build:
	cmake --build build

run: build
	./build/Tester

clean:
	rm -rf build

configure:
	cmake -S . -B build

all: configure build run