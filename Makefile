
CC = gcc

PROGS = error.o my_io.o my_signal.o wrapper.o esame.o cucina cameriere

CLEANFILES = error.o my_io.o my_signal.o wrapper.o esame.o cucina.o cameriere.o


all: ${PROGS}

error.o: error.c
	${CC} -c error.c

my_io.o:  my_io.c
	${CC} -c my_io.c

my_signal.o:  my_signal.c
	${CC} -c my_signal.c

wrapper.o: wrapper.c
	${CC} -c wrapper.c
	
esame.o: esame.c
	${CC} -c esame.c

cameriere:cameriere.o esame.o error.o my_io.o my_signal.o wrapper.o
	${CC} -o $@ $^

cucina: cucina.o esame.o error.o my_io.o my_signal.o wrapper.o
	${CC} -o $@ $^
	
clean:
	rm -f ${CLEANFILES}
	rm -f ${PROGS}
	rm -f *.*~	
	rm -f *~
