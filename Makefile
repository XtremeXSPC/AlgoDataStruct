.PHONY: build run clean test

build:
	cmake --build build

run: build
	./build/Tester

clean:
	rm -rf build

configure:
	cmake -S . -B build

compile_commands:
	ln -sf build/compile_commands.json compile_commands.json

all: configure build run