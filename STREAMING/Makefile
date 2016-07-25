# Makefile
# Author: politronix team



CC=g++
CFLAGS= -Wall -std=c++11

SOURCEDIR=src
BUILDDIR=build

EXECUTABLE= sentiment-continual
SOURCES=$(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS=$(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

LINKS= -lcurl -lrestclient-cpp -lmysqlcppconn -lboost_date_time  

all: dir $(EXECUTABLE)

dir:
	mkdir -p $(BUILDDIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ $(LINKS) -o $@ 

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -f $(BUILDDIR)/*o $(EXECUTABLE)

