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
#include <bitcoin/bitcoin.hpp>
#include <future>
#include <list>
#include <unordered_set>
#include "db.hpp"
using namespace bc;

blockchain* c = nullptr;
std::list<payment_address> addr_queue;


blockchain::history_list get_history(payment_address payaddr) {
   std::promise<blockchain::history_list> history_promise;
   auto history_fetched1 =
      [&history_promise](const std::error_code& ec,
	                 const blockchain::history_list& history)
      {
	 history_promise.set_value(history);
      };
   c->fetch_history(payaddr, history_fetched1);
   return history_promise.get_future().get();
}

int main(int argc, char** argv)
{
   if (argc != 3)
   {
      log_info() << "Usage: path ADDRESS1 ADDRESS2";
      return -1;
   }
   sql::Connection *con = db_init_connection();
   threadpool pool(1);
   leveldb_blockchain chain(pool);
   c = &chain;
   std::promise<std::error_code> ec_promise;
   auto blockchain_started =
      [&ec_promise](const std::error_code& ec)
      {
	 ec_promise.set_value(ec);
      };
   chain.start("practiceBlockchaind", blockchain_started);
   std::error_code ec = ec_promise.get_future().get();
   if (ec)
   {
      log_error() << "Problem starting blockchain: " << ec.message();
      return 1;
   }


   std::string src = argv[1];
   std::string dst = argv[2];
   uint32_t src_cluster_no = db_get(con, src);
   uint32_t dst_cluster_no = db_get(con, dst);
   std::unordered_map<hash_digest, std::string> addresses;
   std::unordered_set<std::string>* src_cluster = db_getset(con, src_cluster_no);
   if (src_cluster->size() == 0) src_cluster->insert(src);
   std::unordered_set<std::string>* dst_cluster = db_getset(con, dst_cluster_no);
   if (dst_cluster->size() == 0) dst_cluster->insert(dst);
   std::unordered_set<hash_digest> src_out_trans;

   for (auto addr = src_cluster->begin(); addr != src_cluster->end(); addr++) {
      payment_address src_addr;

      if (!src_addr.set_encoded(*addr))
      {
	 log_fatal() << "Invalid address";
	 return 1;
      }

      blockchain::history_list history = get_history(src_addr);
      
      for (const auto& row: history)
      {
	 uint32_t value = row.value;
	 BITCOIN_ASSERT(value >= 0);
	 //log_info() << row.spend.hash;
	 if (row.spend.hash != null_hash) {
	    src_out_trans.insert(row.spend.hash);
	    addresses[row.spend.hash] = *addr;
	 }
      }
   }
    if (src_cluster_no != 0 && src_cluster_no == dst_cluster_no) {
        payment_address dst_addr;
        if (!dst_addr.set_encoded(dst))
        {
	        log_fatal() << "Invalid address";
	        return 1;
        }
        
        blockchain::history_list history = get_history(dst_addr);
      for (const auto& row: history)
      {
        uint32_t value = row.value;
	    BITCOIN_ASSERT(value >= 0);
       // log_info() << row.spend.hash;
        if (src_out_trans.find(row.spend.hash) != src_out_trans.end()) {
	        std::cout << src_cluster_no << "|" << dst_cluster_no << "|" 
		       << addresses[row.spend.hash] << "|" << dst << "|" << row.spend.hash;
	        break;
	    }
      }
     } else {
   for (auto addr = dst_cluster->begin(); addr != dst_cluster->end(); addr++) {
   
      payment_address dst_addr;
      if (!dst_addr.set_encoded(*addr))
      {
	 log_fatal() << "Invalid address";
	 return 1;
      }

      blockchain::history_list history = get_history(dst_addr);
      for (const auto& row: history)
      {
	 uint32_t value = row.value;
	 BITCOIN_ASSERT(value >= 0);
	    if (src_out_trans.find(row.output.hash) != src_out_trans.end()) {
	        std::cout << src_cluster_no << "|" << dst_cluster_no << "|" 
		       << "|" << addresses[row.output.hash] << "|" << *addr << "|" << row.output.hash;
	        break;
	    }
      }
      
   }
    }
   pool.shutdown();
   pool.join();
   chain.stop();
   return 0;
}
