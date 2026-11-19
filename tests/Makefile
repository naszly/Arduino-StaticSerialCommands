CXX ?= g++
CXXFLAGS ?= -std=gnu++20 -Wall -Wextra -fpermissive -Isupport -I../src

all: run

serial_commands_tests: test_serial_commands.cpp ../src/StaticSerialCommands.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

run: serial_commands_tests
	./serial_commands_tests

clean:
	rm -f serial_commands_tests

.PHONY: all run clean
