CC = gcc
LIBS = -lncurses -lm -lpthread

all:
	${CC} -o main main.c ${LIBS}
clean:
	rm *.o
