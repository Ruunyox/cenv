CC = gcc
LIBS = -lncurses -lm

all:
	${CC} -o main main.c ${LIBS}
clean:
	rm *.o
