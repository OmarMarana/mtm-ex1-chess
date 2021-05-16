CC = gcc
OBJS = chessSystem.o game.o player.o map.o node.o tournament.o
EXEC = chessEXE
DEBUG_FLAG = -pedantic-errors -DNDEBUG
COMP_FLAG = -std=c99 -Wall -Werror


$(EXEC) : $(OBJS)
	$(CC) $(DEBUG_FLAG) $(OBJS) -o $@

chessSystem.o : chessSystem.c chessSystem.h map.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c

player.o : player.c player.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c

game.o : game.c game.h chessSystem.h map.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c

tournament.o : tournament.c tournament.h game.h map.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c

map.o : map.c map.h node.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c

node.o : node.c node.h map.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c

clean:
	rm -f $(OBJS) $(EXEC)