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
	$(CXX) -o $@ -c -I/usr/local/include/cppconn -Wl,-Bdynamic $< $(CXXFLAGS)

fullnode.o: fullnode.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

fullnode: fullnode.o parser.o db.o
	$(CXX) -o $@ $< parser.o db.o $(LIBS) -lmysqlcppconn

db.o: db.cpp db.hpp
	$(CXX) -c -I/usr/local/include -I/usr/local/include/cppconn -Wl,-Bdynamic db.cpp	

all: initchain fullnode 

clean:
	rm -f fullnode initchain parser
	rm -f *.o

blockchain:
	rm -rf blockchain
	mkdir blockchain
	./initchain blockchain
