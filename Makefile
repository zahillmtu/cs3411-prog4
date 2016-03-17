CC = gcc
CCFLAGS = -std=c99 -g -Wall -Wextra -lreadline
EXEC = mtush
OBJS = mtush.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS}

.c.o:
	${CC} ${CCFLAGS} -c $<

convert.o: mtush.c

run: ${EXEC}
	./${EXEC}

clean:
	rm -f ${EXEC} ${OBJS}
