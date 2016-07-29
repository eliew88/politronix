# Makefile
# Author: politronix team



CC=g++
CFLAGS= -Wall -std=c++11 -Icurlcpp/include

SOURCEDIR=src
BUILDDIR=build

EXECUTABLE= streaming
SOURCES=$(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS=$(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

LINKS= -lcurl -lcurlcpp -loauth -lboost_date_time -Lcurlcpp/build/src 

all: dir $(EXECUTABLE)

dir:
	mkdir -p $(BUILDDIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ $(LINKS) -o $@ 

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -f $(BUILDDIR)/*o $(EXECUTABLE)

