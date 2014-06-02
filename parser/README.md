More detail on how our parser works
---------
Most of the stuff is contained in parser.cpp and parser.hpp, with usage examples
in updater.cpp and fullnode.cpp.

updater.cpp is used to parse an already downloaded blockchain from block 0
to the end (downloaded with downloader.cpp).

fullnode.cpp is supposed to update incrementally, and assumes that
the database is up to date for the entire blockchain, running the parser
for each block downloaded. Unfortunately, our parser is slow when using
the database, we are going to try and correct that at some point in the future.

Since the incremental version is too slow and if it eventually works it will
have changed, we will explain the from scratch parser.

Everything is done asynchronously, so order is not guaranteed! Block 1 could be processed
after block 10000!

Everytime we make a libbitcoin call, you must give it a callback function for when
the call completes. In order to send extra parameters, you can make use of std::bind,
which we often do.

In the constructor, the parser calls a function that gets the latest block number.
The callback for the function then fetches 10,000 blocks, calling the update function
for each one. After update is called on a block, it fetches the next block, which once
fetched has update called on it and so on until reaching the last block.

This allows any analysis that doesn't rely on the order the blocks are processed
in to be done by simply changing the update function. If the order is important,
this platform will probably not be the best route to go, although it may be possible.
As we discovered, this should be done in memory, and written to file, and then put
into a database using bulk file inserts.