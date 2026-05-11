# Solar System Simulation
# Forditas: make
# Tisztitas: make clean

CC      = gcc
TARGET  = solar

# Forrasfajlok
SRCS    = main.c

# Fordito flagek
CFLAGS  = -Wall -Wextra -std=c11 $(shell sdl2-config --cflags)

# Linker flagek
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lGL -lGLU -lm

# Windows (MinGW) alatt hasznalando parancs:
# gcc main.c -o solar.exe -mwindows -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lglu32 -lm

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS) solar.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
