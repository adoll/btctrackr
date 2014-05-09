btctrackr
=========
#Instructions for testing out parser:

There might be some issues with certain features of C++, but hopefully all
dependencies are handled by the libbitcoin setup.

1. Have libbitcoin setup (http://libbitcoin.dyne.org/download.html)
2. Install libmysqlcppconn by running "apt-get install libmysqlcppconn-dev"
3. Run make in parser/
4. Run mkdir blockchain
5. Run ./initchain blockchain
6. Run ./fullnode for some period of time
7. Type stop and hit enter; you will now have a mysql database with addresses and cluster ids.

#Next up:
* Getting new blocks from the network and updating the structure accordingly (Done)
* Adding extra information to clusters like transactions etc
* Database integration (Done)

#Other stuff:

Ended up not using union-find as discussed, instead used a bunch of maps.

This should make integration with the database easier and also handle
some of the difficulties with restarting.

Stopped using maps and used only the mysql database instead.
