CC = gcc
LIBS = -lncurses -lm -lpthread

all:
	${CC} -o cenv cenv.c ${LIBS}
clean:
	rm *.o
