/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) 
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PARSER_HPP
#define PARSER_HPP

#include "db.hpp"
#include <bitcoin/bitcoin.hpp>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>
#include <mutex>
#include <boost/property_map/property_map.hpp>
#include <boost/pending/disjoint_sets.hpp>
#include <iterator>

using namespace bc;

class parser
{
public:
   parser(blockchain* chain, bool update);
   void update(const block_type& blk);
   void close();

private:
   // the number of blocks that can be open at once
   // setting this too high results in ram exhaustion
   const uint32_t MAX_OPEN_BLOCKS = 10000;
   blockchain* chain = nullptr;
   // next int id to be assigned to an address
   uint32_t cur_cluster;
   // mutex for the disjoint set and database, ensure no transaction
   // is being processed concurrently
   std::mutex disjoint_mutex;
   // ensure that the transaction datastructures are being used safely
   std::mutex trans_mutex;
   // ensure that the block_counter is not accessed unsafely
   std::mutex counter_mutex;
   // the block we are currently on, starts at MAX_BLOCKS since we open
   // those all at once
   uint32_t block_counter = MAX_OPEN_BLOCKS;
   // max block to open
   uint32_t top;
   // identifies whether parser should start at beginning of blockchain
   // or try to incrementally update
   bool updater;
   // map of transaction hash to the number of unique input addresses
   std::unordered_map<hash_digest, uint32_t> trans_size_map;
   // map of transaction hash to the set of unique input addresses (a closure)
   std::unordered_map<hash_digest, std::set<std::string>*> common_addresses;
   // map of addresses (represented as strings) to a unique integer identifier
   std::unordered_map<std::string, uint32_t> closure_map;
   void process_trans(std::set<std::string> *addresses); 
   void process_transaction(std::set<std::string> *addresses);
   void handle_trans_fetch(      
      const std::error_code& ec,  // Status of operation
      const transaction_type& tx, // Transaction
      uint32_t index,             // Index within the current transaction
      hash_digest trans_hash // Hash of the transaction spending outputs
      );
   void handle_block_fetch(
      const std::error_code& ec,  // Status of operation
      const block_type& blk);       // Block header
   void height_fetched(const std::error_code& ec, size_t last_height);

   sql::Connection *con;

};


#endif

