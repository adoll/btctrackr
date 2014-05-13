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
using namespace placeholders;

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

            unordered_set<payment_address> *addresses = 
                new unordered_set<payment_address>();
            vector<transaction_input_type> inputs;

            for (auto input = trans->inputs.begin();
                    input != trans->inputs.end(); input++) {
                payment_address addr;
                // if we successfully get address from input script
                if (extract(addr, input->script)) {
                    size--;
                    // if address wasn't already inserted
                    if (addresses->find(addr) == addresses->end()) {  
                        addresses->insert(addr);
                    }
                }
                else {
                    inputs.push_back(*input);
                }
            } // end input loop
            if (size == 0) {
                //if (updater) process_trans_map(addresses);
                if (updater) process_transaction(addresses); 
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
    log_info() << "height: " << last_height;
    assert(chain);
    auto handle = bind(&parser::handle_block_fetch, this, _1, _2);
    // Begin fetching the block header.
    for (int i = 150000; i <= 151000; i++) {
        fetch_block(*chain, i, handle);
	std::cerr << i << std::endl;
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
        log_info() << "trans fetch fail" << ec.message();
    }
    else {
        mtx1.lock();
        uint32_t size = trans_size_map[trans_hash];
        payment_address addr;
        unordered_set<payment_address> *addresses = common_addresses[trans_hash];
        if (addresses == NULL) {
            mtx1.unlock();
            return;
        }
        if (extract(addr, (tx.outputs.begin() + index)->script)) {
            // if address wasn't already inserted
            if (addresses->find(addr) == addresses->end()) {
                addresses->insert(addr);
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
            //if (updater) process_trans_map(addresses);
            if (updater) process_transaction(addresses);
            else process_transaction(addresses);
            trans_size_map.erase(trans_hash);
            common_addresses.erase(trans_hash);
        }
        mtx1.unlock();
    }
}

// given a list of addresses in the same transaction, updates the
// cluster mapping
void parser::process_transaction(unordered_set<payment_address> *addresses) {
    mtx.lock();
    uint32_t cluster_no = 0;

    unordered_set<string>* cluster = new unordered_set<string>();

    // merging all clusters into one cluster
    for (auto addr = addresses->begin();
            addr != addresses->end(); addr++) {

        cluster->insert(addr->encoded());
        uint32_t cur_no = db_get(con, addr->encoded()); //
        // 0 is the empty cluster, so if it isn't 0 merge everything and erase old
        if (cur_no != 0) {
            unordered_set<string> *this_cluster = db_getset(con, cur_no);
            // update cluster no, in this way, we always allocate to smallest id
            if (cur_no < cluster_no || cluster_no == 0) {
                cluster_no = cur_no;
            }
            cluster->insert(this_cluster->begin(), this_cluster->end());
        }
    }
    if (cluster_no == 0) {
        cluster_no = cur_cluster;
        cur_cluster++;
    }
    // make sure all addresses in the cluster have the right number

    for (auto addr = cluster->begin();
            addr != cluster->end(); addr++) {
        db_insert(con, *addr, cluster_no);
    }

    mtx.unlock();
}

void parser::process_trans_map(unordered_set<payment_address> *addresses) {
    mtx.lock();
    uint32_t cluster_no = 0;

    if (addresses == NULL || addresses->size() == 0) {
        mtx.unlock();
        return;
    }
    unordered_set<payment_address>* cluster = new unordered_set<payment_address>();
    cluster->insert(addresses->begin(), addresses->end());
//unordered_map<string, uint32_t> temp_addr;
    // merging all clusters into one cluster
    uint32_t cur_max_size = 0;
    for (auto addr = addresses->begin();
            addr != addresses->end(); addr++) {
        string addr_string = addr->encoded();
        uint32_t cur_no = address_map[*addr];//db_get(con, addr_string); //address_map[*addr];
        /*if (cur_no != 0)
          temp_addr[addr_string] = cur_no;
          else 
          temp_addr[addr_string] = UINT_MAX;*/
        // 0 is the empty cluster, so if it isn't 0 merge everything and erase old
        if (cur_no != 0) {
            unordered_set<payment_address> *cur_cluster = closure_map[cur_no];
            // might be null if the cluster was already iterated through
            if (cur_cluster) {
                // update cluster no, in this way, we always allocate to smallest id
                uint32_t cluster_size = cur_cluster->size();
                if (cluster_no == 0 || cluster_size > cur_max_size) {
                    cur_max_size = cluster_size;
                    cluster_no = cur_no;
                }
                cluster->insert(cur_cluster->begin(), cur_cluster->end());
                closure_map.erase(cur_no);
            }
        }
    }
    if (cluster_no == 0) {
        cluster_no = cur_cluster;
        cur_cluster++;
    }

    // make sure all addresses in the cluster have the right number
    for (auto addr = cluster->begin();
            addr != cluster->end(); addr++) {
        /*string addr_string = addr->encoded();
        //log_info() << addr_string;
        uint32_t cluster = temp_addr[addr_string];
        if (cluster == UINT_MAX) db_insert(con, addr_string, cluster_no);
        else {

        if (cluster == 0)
        uint32_t cluster = db_get(con, addr_string);
        if (cluster != cluster_no)
        db_update(con, addr_string, cluster_no); */
        address_map[*addr] = cluster_no;
    }

    closure_map[cluster_no] = cluster;
    mtx.unlock();
    }

    void parser::close() {
        // update db
        if (updater) {

            for (auto i = address_map.begin(); i != address_map.end(); i++) {
                db_insert(con, i->first.encoded(), i->second);
            }
        }
        delete con;
    }

    /*int main()
      {
    // Define a threadpool with 1 thread.
    threadpool pool(1);
    // Create a LevelDB blockchain.
    leveldb_blockchain ldb_chain(pool);
    promise<error_code> ec_promise;
    auto blockchain_started =
    [&ec_promise](const std::error_code& ec)
    {
    ec_promise.set_value(ec);
    };
    ldb_chain.start("blockchain", blockchain_started);
    std::error_code ec = ec_promise.get_future().get();
    if (ec)
    {
    log_error() << "Problem starting blockchain: " << ec.message();
    return 1;
    }
    parser parse(&ldb_chain);

    // Don't wait after all current operations have completed.
    pool.shutdown();

    // Join them one by one.
    pool.join();

    // code to print stuff out
    for (auto addr = parse.addressesBegin();
    addr != parse.addressesEnd(); addr++) {
    uint32_t cluster_no = addr->second;
    log_info() << "Address: " << addr->first.encoded()  << " Cluster: " << cluster_no;
    log_info() << "Rest of Cluster: \n --------------------------------------------------";
    for (auto cluster_addr = parse.closure(addr->first)->begin();
    cluster_addr != parse.closure(addr->first)->end(); cluster_addr++) {
    log_info() << "Cluster Address: " << cluster_addr->encoded();
    }
    log_info() << "------------------------------------------------------------------------";
    }

    // Finally stop the blockchain safely now everything has stopped.
    ldb_chain.stop();
    return 0;
    }*/
