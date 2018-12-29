FLAGS = -Wall -g -std=c11
CC = gcc
LIST = LinkedListAPI
LDFLAGS = -L.
PARSER = GEDCOMparser

#UNAME Shell Variable
UNAME_S := $(shell uname -s)

# OS X Dependant Variables
ifeq ($(UNAME_S), Darwin)
    LIBPARSER := ../GEDCOMparser.dylib
# GNU/Linux Depedant Variables
else ifeq ($(UNAME_S), Linux)
    LIBPARSER := ../GEDCOMparser.so
endif

$(LIBPARSER): src/GEDCOMparser.o src/LinkedListAPI.o src/hashTable.o
	$(CC) -shared -o $(LIBPARSER) src/GEDCOMparser.o src/LinkedListAPI.o src/hashTable.o

src/$(LIST).o:include/$(LIST).h src/$(LIST).c
	$(CC) $(FLAGS) -c src/$(LIST).c -Iinclude -o src/$(LIST).o


src/hashTable.o: src/hashTable.c include/hashTable.h include/GEDCOMUtilities.h 
	$(CC) $(FLAGS) -c -Iinclude src/hashTable.c -o src/hashTable.o

src/$(PARSER).o: src/$(PARSER).c include/$(PARSER).h include/GEDCOMUtilities.h 
	$(CC) $(FLAGS) -c -Iinclude src/$(PARSER).c -o src/$(PARSER).o


	
clean:
	rm -rf src/*.o bin/*.a
	rm $(LIBPARSER)