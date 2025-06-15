.PHONY: build #unittest

libraries=$(libjson) $(libcurl)
includes=$(foreach lib,$(libraries),-I $(lib)/include -L$(lib)/lib)

build: request.out

unittest:
	$(MAKE) -C unittest

request.out: main.c communication.o communication.h
	gcc $(includes) -lcurl -lpthread main.c communication.c -o request.out

communication.o: communication.c communication.h
	gcc -I $(libcurl)/include -c communication.c

all: build unittest
