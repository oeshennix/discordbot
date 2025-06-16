.PHONY: build #unittest

libraries=$(libcjson) $(libcurl)
includes=$(foreach lib,$(libraries),-I $(lib)/include -L$(lib)/lib)

build: request.out

unittest:
	$(MAKE) -C unittest

request.out: main.c communication.o communication.h gateway.o gateway.h 
	gcc $(includes) -lcjson -lcurl -lpthread main.c communication.o gateway.o -o request.out

communication.o: communication.c communication.h
	gcc -I $(libcurl)/include -c communication.c
gateway.o: gateway.c gateway.h
	gcc -I $(libcurl)/include -c gateway.c

all: build unittest
