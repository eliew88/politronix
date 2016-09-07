# Makefile
# Author: politronix team

CC=g++
LDFLAGS=  -L/usr/local/opt/curl/lib
CPPFLAGS= -Wall -std=c++11 -Icurlcpp/include -I/usr/local/opt/curl/include -I/usr/local/include -D_GLIBCXX_USE_CXX11_ABI=1

SOURCEDIR=src
BUILDDIR=build

EXECUTABLE= train
SOURCES=$(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS=$(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

all: dir $(EXECUTABLE)

dir:
	mkdir -p $(BUILDDIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LINKS) -o $@ 

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@ 

clean:
	rm -f $(BUILDDIR)/*o $(EXECUTABLE)

