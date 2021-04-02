all: GarbageEater

utils.o: utils.c utils.h
	gcc -Wall -c utils.c

opcode.o: opcode.c opcode.h utils.h
	gcc -Wall -c opcode.c

GarbageEater: opcode.o utils.o main.c
	gcc -g -o GarbageEater main.c opcode.o utils.o -Wall

clean:
	rm -f GarbageEater opcode.o utils.o

cleanProgram:
	rm -f programs/*.obj programs/*.sym 