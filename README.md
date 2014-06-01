btctrackr
=========

#Parser and database setup
1. Install libbitcoin (http://libbitcoin.dyne.org/download.html)
..* Their dependences are our dependencies, they have instructions in their README
2. Install libmysqlcppconn by running "apt-get install libmysqlcppconn-dev"
3. Run make in parser/
4. Run mkdir blockchain
5. Run ./initchain blockchain
6. Run ./downloader to download the blockchain
6. Run ./updater to process blockchain, currently this will give you a file with the data in CSV format.
7. Import this into the db, and then build indices. (Shaheed- more info)

For more specific details about the parser or website, look in the respective
directory for the README (This may not have been added yet).

## Notes and future work
* Right now our our parser does a good job of going through the entire blockchain, but it is too slow for updating as each block is updated. We thought that updating one block would be fast enough using just database calls and no in-memory structures to help out, unfortunately we were wrong.
* Libbitcoin has a bug: while parsing a transaction, if it cannot extract the address from an input script, it does not add the transaction to the debits of the appropriate address. I attempted to fix this: https://github.com/adoll/libbitcoin/blob/188ebdde4b05337b231b91984abc9e2d6c0f8bfc/src/blockchain/leveldb/leveldb_common.cpp specifically in the add_debit function, but this failed to correct all the problems. This leads to incorrect path calculations.