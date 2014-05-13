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

   blockchain* chain = nullptr;
   uint32_t cur_cluster;
   // used in process_transaction
   std::mutex mtx;
   std::mutex mtx1;
   bool updater;
   // map of transaction to payment address
   std::unordered_map<hash_digest, uint32_t> trans_size_map;
   std::unordered_map<hash_digest, std::set<std::string>*> common_addresses;
   std::unordered_set<std::string> all_addresses;
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

