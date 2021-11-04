CC = g++ -lpthread -lmysqlclient

BINARY = gameserver

SRC=*.cpp util/*.cpp
DEPS=*.h util/*.h fwd/*.h
#DEPS=*.h util/*.h handlers/*.h fwd/*.h

build:
	$(CC) -std=c++17 -o $(BINARY) -Wall $(SRC)

all: run clean

run: $(BINARY)
	./$(BINARY)
clean:
	rm $(BINARY)
