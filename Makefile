NAME = fpn
TARGET = bin/$(NAME)
MANPAGE = $(NAME).1
PREFIX ?= /usr/local
CC ?= gcc
LIBS = -lgmp -lmpfr
.PHONY: all debug release install clean

all: $(TARGET)

bin/%.o: src/%.c $(wildcard src/*.h)
	@mkdir -p bin
	$(CC) $(FLAGS) -std=c99 -Wall -Wextra -Wpedantic -c $< -o $@

$(TARGET): $(patsubst src/%.c, bin/%.o, $(wildcard src/*.c))
	@mkdir -p bin
	$(CC) $(FLAGS) -std=c99 -Wall -Wextra -Wpedantic $^ -o $@ $(LIBS)

debug: FLAGS = -g -O0

debug: $(TARGET)

release: FLAGS = -O3

release: $(TARGET)
	strip -s -R .comment -R .gnu.version $(TARGET)

install: $(TARGET)
	install -D $(TARGET) $(DESTDIR)$(PREFIX)/$(TARGET)
	install -Dm644 $(MANPAGE) $(DESTDIR)$(PREFIX)/share/man/man1/$(MANPAGE)

clean:
	rm -rf bin
