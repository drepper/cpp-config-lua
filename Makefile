all: test

CXX = g++
CXXFLAGS = $(STD) $(OPT) $(INCLUDES) $(DEBUG) $(WARN) $(CXXFLAGS-$@)
STD = -std=gnu++2a
OPT = -O
DEBUG = -g
INCLUDES = $(shell pkg-config --cflags $(PACKAGES))
WARN = -Wall -Wextra
LIBS = $(shell pkg-config --libs $(PACKAGES))
PACKAGES = lua

CXXFLAGS-test.o = -DDATADIR=\"$$PWD\"

test: test.o config_lua.o
	$(CXX) -o $@ $(LDFLAGS) $^ $(LIBS)

test.o config_lua.o: %.o: config_lua.hh
