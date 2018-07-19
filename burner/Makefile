CC=gcc
LD=gcc
LIBS=$(shell pkg-config --libs libusb-1.0)
INCLUDE=$(shell pkg-config --cflags libusb-1.0)
CFLAGS=-std=gnu99 -g
SOURCES=$(shell find src/ -name "*.c")

all: ifd68

ifd68: $(SOURCES:%.c=%.o)
	$(LD) $(CFLAGS) $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -rf src/*.o ifd68

