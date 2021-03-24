utils.o: utils.c utils.h
	gcc -Wall -c utils.c

opcode.o: opcode.c opcode.h utils.h
	gcc -Wall -c opcode.c

main.o: main.c opcode.h utils.h
	gcc -Wall -c main.c

main: opcode.o utils.o main.o
	gcc -g -o trout opcode.o utils.o main.o -Wall