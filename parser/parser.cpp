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
/*
   Display info from the last block in our blockchain at "./blockchain/"
   */
#include "parser.hpp"
#include <future>
#include <limits.h>
#include <algorithm>
#include <boost/assign/list_of.hpp>
using namespace std::placeholders;

typedef std::map<std::string,std::size_t> rank_t; // => order on int
typedef std::map<std::string,std::string> parent_t;
typedef boost::associative_property_map<rank_t> Rank;
typedef boost::associative_property_map<parent_t> Parent;
rank_t rank_map;
parent_t parent_map;
Rank rank_pmap(rank_map);
Parent parent_pmap(parent_map);
boost::disjoint_sets<Rank, Parent> dsets(rank_pmap, parent_pmap);

// construct a parser, updating it to the current point in the blockchain
parser::parser(blockchain* chainPtr, bool update) {
    chain = chainPtr;
    updater = update;
    con = db_init_connection();
    if (updater) {
        cur_cluster = 1;
        auto height_fetched_func = bind(&parser::height_fetched, this, _1, _2);
        chain->fetch_last_height(height_fetched_func); 
    }
    else {
        cur_cluster = db_getmax(con);
        cur_cluster++;
    }
}

void parser::update(const block_type& blk) {
   // iterate through all transactions of the block
   for (auto trans = blk.transactions.begin(); trans != blk.transactions.end();
	trans++) {
      
      uint32_t size =  trans->inputs.size();	 
      // for every input, get the previous transaction hash
      if (!is_coinbase(*trans) && size > 1) {
	 
	 std::set<std::string> *addresses = 
	    new std::set<std::string>();
	 std::vector<transaction_input_type> inputs;
  
	 for (auto input = trans->inputs.begin();
	      input != trans->inputs.end(); input++) {
	    payment_address addr;
	    // if we successfully get address from input script
	    if (extract(addr, input->script)) {
	       size--;
	       // if address wasn't already inserted
	       if (addresses->find(addr.encoded()) == addresses->end()) {  
		  addresses->insert(addr.encoded());
	       }
	    }
	    else {
	       inputs.push_back(*input);
	    }
	 } // end input loop
	 if (size == 0) {
	    if (updater) process_trans(addresses); //
	    else process_transaction(addresses);
	    delete addresses;
	 }
	 else {
	    hash_digest trans_hash = hash_transaction(*trans);
	    common_addresses[trans_hash] = addresses;
	    trans_size_map[trans_hash] = size;
	    for (auto i = inputs.begin(); i != inputs.end(); i++) {
	       auto handle_trans = bind(&parser::handle_trans_fetch,
					this, _1, _2,
					i->previous_output.index,
					hash_transaction(*trans));
	       
	       chain->fetch_transaction(i->previous_output.hash,
					handle_trans);

                }
            }

        } // end coinbase if
    }
}

void parser::height_fetched(const std::error_code& ec, size_t last_height)
{
    if (ec)
    {
        log_error() << "Failed to fetch last height: " << ec.message();
        return;
    }
    // Display the block number.
    //log_info() << "height: " << last_height;
    assert(chain);
    auto handle = bind(&parser::handle_block_fetch, this, _1, _2);
    // Begin fetching the block header.
    for (int i = 0; i <= last_height; i++) {
        fetch_block(*chain, i, handle);
    }
}

void parser::handle_block_fetch(
        const std::error_code& ec,  // Status of operation
        const block_type& blk)       // Block header
{
    if (!ec) this->update(blk);

}

void parser::handle_trans_fetch(
        const std::error_code& ec,  // Status of operation
        const transaction_type& tx, // Transaction
        uint32_t index,             // Index within the current transaction
        hash_digest trans_hash // Hash of the transaction spending outputs
        )
{
   if (ec) {
      log_error() << "trans fetch fail" << ec.message();
   }
   else {
      mtx1.lock();
      uint32_t size = trans_size_map[trans_hash];
      payment_address addr;
      std::set<std::string> *addresses = common_addresses[trans_hash];
      if (addresses == NULL) {
	 mtx1.unlock();
	 return;
      }
      if (extract(addr, (tx.outputs.begin() + index)->script)) {
	 // if address wasn't already inserted
	 if (addresses->find(addr.encoded()) == addresses->end()) {
	    addresses->insert(addr.encoded());
	 }
	 // check for case when addresses are used as inputs twice
	 else {
	    trans_size_map[trans_hash] = size - 1;
	 }
	 
      }
      // the scipt was unsupported, we cannot include in closure
      else {
	 trans_size_map[trans_hash] = size - 1;
      }
      // we have all the addresses we need, process it
      if (addresses->size() == trans_size_map[trans_hash]) {
	 if (updater) process_trans(addresses); //
	 else process_transaction(addresses);
	 trans_size_map.erase(trans_hash);
	 common_addresses.erase(trans_hash);
      }
      mtx1.unlock();
   }
}

// given a list of addresses in the same transaction, updates the
// cluster mapping
void parser::process_transaction(std::set<std::string> *addresses) {
    mtx.lock();
    uint32_t cluster_no = 0;

    std::unordered_set<std::string> cluster;

    // merging all clusters into one cluster
    for (auto addr = addresses->begin();
            addr != addresses->end(); addr++) {

        cluster.insert(*addr);
        uint32_t cur_no = db_get(con, *addr); //
        // 0 is the empty cluster, so if it isn't 0 merge everything and erase old
        if (cur_no != 0) {
            std::unordered_set<std::string> *this_cluster = db_getset(con, cur_no);
            // update cluster no, in this way, we always allocate to smallest id
            if (cur_no < cluster_no || cluster_no == 0) {
                cluster_no = cur_no;
            }
            cluster.insert(this_cluster->begin(), this_cluster->end());
        }
    }
    if (cluster_no == 0) {
        cluster_no = cur_cluster;
        cur_cluster++;
    }
    // make sure all addresses in the cluster have the right number

    for (auto addr = cluster.begin();
            addr != cluster.end(); addr++) {
        db_insert(con, *addr, cluster_no);
    }

    mtx.unlock();
}

void parser::process_trans(std::set<std::string> *addresses) {
   // shouldn't happen
   if (addresses == NULL || addresses->size() < 2) {
      return;
   }
   mtx.lock();
   std::string root = *addresses->begin();
   uint32_t cluster_no = closure_map[root];
   if (cluster_no != 0) {
      root = dsets.find_set(root);
   }
   else {
      closure_map[root] = cur_cluster;
      cur_cluster++;
      dsets.make_set(root);
   }
   mtx.unlock();
   for (auto addr = std::next(addresses->begin()); addr != addresses->end();
	addr++) {
      mtx.lock();
      
      uint32_t cluster = closure_map[*addr];
      if (cluster == 0) {
	 closure_map[*addr] = cur_cluster;
	 cur_cluster++;
	 
	 dsets.make_set(*addr);
      }
      dsets.union_set(root, *addr);
      mtx.unlock();
   }
}

void parser::close() {
    // update db
    if (updater) {
        std::vector<std::string> addresses;
        for (auto i = closure_map.begin(); i != closure_map.end(); i++) {
            addresses.push_back(i->first); 
        }
        dsets.compress_sets(addresses.begin(), addresses.end());
        log_error() << dsets.count_sets(addresses.begin(), addresses.end());

        for (auto i = closure_map.begin(); i != closure_map.end(); i++) {
            //db_insert(con, i->first, closure_map[parent_pmap[i->first]]);
            std::cout << i->first << "," << closure_map[parent_pmap[i->first]] << std::endl;          }
    } 
    delete con; 
}
