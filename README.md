btctrackr
=========
#Instructions for testing out parser:

There might be some issues with certain features of C++, but hopefully all
dependencies are handled by the libbitcoin setup.

1. Have libbitcoin setup (http://libbitcoin.dyne.org/download.html)
2. Run make in parser/
3. Run ./initchain blockchain
4. Run ./fullnode for some period of time
5. Type stop and hit enter, you'll see a bunch of addresses and their closures

#Next up:
* Getting new blocks from the network and updating the structure accordingly (Done -probably)
* Adding extra information to clusters like transactions etc
* Database integration

#Other stuff:

Ended up not using union-find as discussed, instead used a bunch of maps.

This should make integration with the database easier and also handle
some of the difficulties with restarting.
