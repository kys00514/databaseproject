#make file
main: ex4.o
	gcc -o main ex4.o
ex4.o: ex4.c
	gcc -c ex4.c
