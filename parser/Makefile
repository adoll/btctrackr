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

updater.o: updater.cpp
	$(CXX) -o $@ -c -I/usr/local/include/cppconn -Wl,-Bdynamic $< $(CXXFLAGS)

updater: updater.o
	$(CXX) -o $@ $< parser.o db.o $(LIBS) -lmysqlcppconn

fullnode.o: fullnode.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

fullnode: fullnode.o parser.o db.o
	$(CXX) -o $@ $< parser.o db.o $(LIBS) -lmysqlcppconn

downloader.o: downloader.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

downloader: downloader.o
	$(CXX) -o $@ $< $(LIBS)


db.o: db.cpp db.hpp
	$(CXX) -c -I/usr/local/include -I/usr/local/include/cppconn -Wl,-Bdynamic db.cpp $(CXXFLAGS)	

all: initchain fullnode updater downloader

clean:
	rm -f fullnode initchain parser downloader updater
	rm -f *.o

restartDB:
	./droptables.sh
restart:
	mkdir blockchain
	./initchain blockchain
