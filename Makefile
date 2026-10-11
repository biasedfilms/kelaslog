CC = cc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude

SOURCES = src/main.c src/input.c src/student.c src/storage.c src/attendance.c

.PHONY: all clean run

all: kelaslog

kelaslog: $(SOURCES) $(wildcard include/*.h)
	$(CC) $(CFLAGS) $(SOURCES) -o $@

run: kelaslog
	./kelaslog

clean:
	rm -f kelaslog *.tmp
