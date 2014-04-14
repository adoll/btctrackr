CXXFLAGS=$(shell pkg-config --cflags libbitcoin) -ggdb
LIBS=$(shell pkg-config --libs libbitcoin)

default: all

initchain.o: initchain.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

initchain: initchain.o
	$(CXX) -o $@ $< $(LIBS)

priv.o: priv.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

priv: priv.o
	$(CXX) -o $@ $< $(LIBS)

parser.o: parser.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

fullnode.o: fullnode.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

fullnode: fullnode.o parser.o
	$(CXX) -o $@ $< parser.o $(LIBS)

all: initchain fullnode 

clean:
	rm -f fullnode initchain parser
	rm -f *.o
