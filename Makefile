CC=gcc
OBJS=chessSystem.o game.o player.o tournament.o tests/chessSystemTestsExample.o
EXEC=chess
OBJ=chess.o
DEBUG=-g -DNDEBUG# now empty, assign -g for debug
CFLAGS=-std=c99 -Wall -pedantic-errors -Werror $(DEBUG)

$(EXEC) : $(OBJ)
	$(CC) $(CFLAG) $(OBJ) -o $@ -L. -lmap
$(OBJ): $(OBJS)
	ld -r -o $(OBJ) $(OBJS)

chessSystemTestsExample.o: tests/chessSystemTestsExample.c chessSystem.h test_utilities.h
chessSystem.o: chessSystem.c chessSystem.h map.h tournament.h
tournament.o: tournament.c tournament.h map.h
game.o: game.h game.c chessSystem.h map.h
player.o: player.c player.h

clean:
	rm -f $(OBJS) $(OBJ) $(EXEC)