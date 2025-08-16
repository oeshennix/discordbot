.PHONY: build #unittest

libraries=$(libcjson) $(libcurl)
includes=$(foreach lib,$(libraries),-I $(lib)/include -L$(lib)/lib)

#set the compiler
ifndef CC
	CC=gcc;
endif

build: request.out

unittest:
	$(MAKE) -C unittest

request.out: main.c gateway.o gateway.h 
	$(CC) $(includes) -lcjson -lcurl -lpthread main.c gateway.o -o request.out

gateway.o: gateway.c gateway.h gateway_events.c
	$(CC) -I $(libcurl)/include -c gateway.c

gateway_events.c:
	$(MAKE) -C build
	./build/buildgatewayevents


all: build unittest
