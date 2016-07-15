# Makefile
# Author: politronix team

CC=g++
CFLAGS= -Wall

all: main

main: base64.o jsoncpp.o twitter-parser.o 
	$(CC) $(CFLAGS) -lcurl -lrestclient-cpp -o main base64.o jsoncpp.o twitter-parser.o

twitter-parser.o: twitter-parser.cpp 
	$(CC) $(CFLAGS) -c twitter-parser.cpp

jsoncpp.o: jsoncpp.cpp json.h 
	$(CC) $(CFLAGS) -c jsoncpp.cpp

base64.o: base64.cpp base64.h
	$(CC) $(CFLAGS) -c base64.cpp

clean:
	rm *o main 
