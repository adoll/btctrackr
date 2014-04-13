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
#include <bitcoin/bitcoin.hpp>
#include <unordered_set>
#include <future>
#include <mutex>
using namespace bc;
using namespace std;
using namespace std::placeholders;

blockchain* chain = nullptr;
// used in process_transaction
mutex mtx;

// map of transaction to payment address
unordered_map<hash_digest, unordered_set<payment_address>*> common_addresses;

// map of closure id to set of addresses, eventually change to closure,
// each closure is a list of transactions
unordered_map<uint32_t, unordered_set<payment_address>*> closure_map;

// map of addresses to closure id
unordered_map<payment_address, uint32_t> address_map;


// Completion handler for when the blockchain has finished initializing.
void blockchain_started(const std::error_code& ec);

// Fetch the last block now that we have the height.
void height_fetched(const std::error_code& ec, size_t last_height);

// populates data structure given a block
void handle_block_fetch(
    const std::error_code& ec,  // Status of operation
    const block_type& blk);       // Block header

// populates data structure given a transaction. called by block fetch

void handle_trans_fetch(
   const std::error_code& ec,  // Status of operation
   const transaction_type& tx,  // Transaction
   uint32_t index,              // index within the current transaction
   hash_digest trans_hash,            // hash of the spending transaction
   uint32_t size
);
// addresses is list of inputs to a transaction, updates data structure 
// accordingly
void process_transaction(unordered_set<payment_address> *addresses);


void blockchain_started(const std::error_code& ec)
{
    // std::error_code's can be tested like bools, and
    // compared against specific error enums.
    // See <bitcoin/error.hpp> for a full list of them.
    if (ec)
    {
        log_error() << "Blockchain failed to start: " << ec.message();
        return;
    }
    // Blockchain has safely started.
    log_info() << "Blockchain started.";
    // chain should've been set inside main().
    assert(chain);
    // Begin fetching the last height number.
    chain->fetch_last_height(height_fetched);
}

void height_fetched(const std::error_code& ec, size_t last_height)
{
    if (ec)
    {
        log_error() << "Failed to fetch last height: " << ec.message();
        return;
    }
    // Display the block number.
    log_info() << "height: " << last_height;
    assert(chain);
    // Begin fetching the block header.
    for (int i = 0; i <= last_height; i++) {
	 fetch_block(*chain, i, handle_block_fetch);
    }
}

void handle_block_fetch(
    const std::error_code& ec,  // Status of operation
    const block_type& blk)       // Block header
{
   // iterate through all transactions of the block
   for (auto trans = blk.transactions.begin(); trans != blk.transactions.end(); 
	trans++) { 
      // for every input, get the previous transaction hash
      if (!is_coinbase(*trans) && trans->inputs.size() > 1) {
	 hash_digest trans_hash = hash_transaction(*trans);
	 uint32_t inputs_size = trans->inputs.size();
	 // log_info() << "size" << inputs_size;
	 for (auto inputs = trans->inputs.begin();
	      inputs != trans->inputs.end(); inputs++) {
	    
	    auto handle_trans = bind(handle_trans_fetch, _1, _2, 
				     inputs->previous_output.index,
				     hash_transaction(*trans),
				     inputs_size);
	    
	    chain->fetch_transaction(inputs->previous_output.hash, 
				     handle_trans);
	 }
      }
   }
}

void handle_trans_fetch(
   const std::error_code& ec,  // Status of operation
   const transaction_type& tx, // Transaction
   uint32_t index,             // Index within the current transaction
   hash_digest trans_hash, // Hash of the transaction spending outputs
   uint32_t size           // pointer to number of addresses expected (pointer caused race conditions)
   )  
{
   payment_address addr;
   if (extract(addr, (tx.outputs.begin() + index)->script)) { 
      unordered_set<payment_address> *addresses = common_addresses[trans_hash];
      if (addresses == NULL) {
	 addresses = new unordered_set<payment_address>();
	 common_addresses[trans_hash] = addresses;
      }
      // if address wasn't already inserted
      if (addresses->find(addr) == addresses->end()) {
	 addresses->insert(addr);
      }
      // check for case when addresses are used as inputs twice (if used more
      // this fails
      else {
	 size--;
      }
      // we have all the addresses we need, process it
      if (addresses->size() == size) {	 
	 process_transaction(addresses);
	 common_addresses.erase(trans_hash);
      }
   }
   else {
      size--;
   }
}

// given a list of addresses in the same transaction, updates the
// cluster mapping
void process_transaction(unordered_set<payment_address> *addresses) {
   mtx.lock();
   static uint32_t cur_cluster = 1;
   uint32_t cluster_no = address_map[*addresses->begin()];
   
   // address has no cluster
   if (cluster_no == 0) { 
      cluster_no = cur_cluster;
      cur_cluster++;
      address_map[*addresses->begin()] = cluster_no;
   }
   
   unordered_set<payment_address>* cluster = 
      new unordered_set<payment_address>();
   cluster->insert(addresses->begin(), addresses->end());

   // merging all clusters into one cluster
   for (auto addr = addresses->begin();
	addr != addresses->end(); addr++) {
      uint32_t cur_no = address_map[*addr];
      // 0 is the empty cluster, so if it isn't 0 merge everything and erase old
      if (cur_no != 0) {
	 unordered_set<payment_address> *cur_cluster = closure_map[cur_no];
	 
	 if (cur_no != cluster_no) {
	    cluster->insert(cur_cluster->begin(), cur_cluster->end());
	    closure_map.erase(cur_no);
	 }
      }
      address_map[*addr] = cluster_no;
   }
   // make sure all addresses in the cluster have the right number
   for (auto addr = cluster->begin();
	addr != cluster->end(); addr++) {
      address_map[*addr] = cluster_no;
   }
   closure_map[cluster_no] = cluster;
   mtx.unlock();
}
   
int main()
{
    // Define a threadpool with 1 thread.
    threadpool pool(1);
    // Create a LevelDB blockchain.
    leveldb_blockchain ldb_chain(pool);
    // Initialize our global 'chain' pointer from above.
    chain = &ldb_chain;
    // Start the database using its implementation specific method.
    ldb_chain.start("blockchain", blockchain_started);
    // Don't wait after all current operations have completed.
    pool.shutdown();
    
    // Join them one by one.
    pool.join();

    // code to print stuff out
    for (auto addr = address_map.begin();
	 addr != address_map.end(); addr++) {
       uint32_t cluster_no = addr->second;
       log_info() << "Address: " << addr->first.encoded()  << " Cluster: " << cluster_no;
       log_info() << "Rest of Cluster: \n --------------------------------------------------";
       for (auto cluster_addr = closure_map[cluster_no]->begin();
	    cluster_addr != closure_map[cluster_no]->end(); cluster_addr++) {
	  log_info() << "Cluster Address: " << cluster_addr->encoded();
       }
       log_info() << "------------------------------------------------------------------------";
    }
    // Finally stop the blockchain safely now everything has stopped.
    ldb_chain.stop();
    return 0;
}
