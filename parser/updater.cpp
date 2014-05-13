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
using namespace std::placeholders;

int main()
  {
  // Define a threadpool with 1 thread.
  threadpool pool(1);
  // Create a LevelDB blockchain.
  leveldb_blockchain ldb_chain(pool);
  std::promise<std::error_code> ec_promise;
  auto blockchain_started =
  [&ec_promise](const std::error_code& ec)
  {
  ec_promise.set_value(ec);
  };
  ldb_chain.start("practiceBlockchaind", blockchain_started);
  std::error_code ec = ec_promise.get_future().get();
  if (ec)
  {
  log_error() << "Problem starting blockchain: " << ec.message();
  return 1;
  }
  parser parse(&ldb_chain, true);

  // Don't wait after all current operations have completed.
  pool.shutdown();

  // Join them one by one.
  pool.join();

  // Finally stop the blockchain safely now everything has stopped.
  ldb_chain.stop();

  parse.close();
  return 0;
  }
