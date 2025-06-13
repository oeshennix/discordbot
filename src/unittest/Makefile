.PHONY : build

build: test.out test2.out

clean:
	rm -v *.o
	rm -v *.out

discord.o: ../discord.c ../discord.h
	gcc ../discord.c -c -o discord.o

test.out: test.c ../discord.h discord.o
	gcc test.c discord.o -o test.out

test2.out: test2.c
	gcc test2.c -o test2.out
