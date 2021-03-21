FILES=main.c
TARGET=pct
FLAGS=-std=gnu99 -Wall -pedantic
L_FLAGS=-lpthread -lm
${TARGET}: ${FILES}
	gcc ${FLAGS} -o ${TARGET} ${FILES} ${L_FLAGS}
.PHONY: clean

clean:
	-rm -f ${TARGET}
