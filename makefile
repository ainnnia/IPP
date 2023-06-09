CC       = gcc
CFLAGS   = -Wall -Wextra -Wno-implicit-fallthrough -std=c17 -O2
OBJS = game_main.o game.o safe_memory_allocation.o board.o player.o interactive_mode.o

.PHONY: all clean

all: game

game: $(OBJS)
	$(CC) -o $@ $(OBJS)

game_main.o: game_main.c game.h safe_memory_allocation.h interactive_mode.h constants.h
game.o: game.c game.h safe_memory_allocation.h board.h player.h constants.h
safe_memory_allocation.o: safe_memory_allocation.c safe_memory_allocation.h
board.o: board.c board.h safe_memory_allocation.h constants.h
player.o: player.c player.h constants.h
interactive_mode.o: interactive_mode.c interactive_mode.h game.h safe_memory_allocation.h board.h player.h constants.h

clean:
	rm -f *.o game