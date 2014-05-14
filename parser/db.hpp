#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <unordered_set>

#include "mysql_connection.h"
#include "mysql_driver.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h> 

uint32_t db_getmax(sql::Connection *con);
std::unordered_set<std::string>* db_getset(sql::Connection *con, uint32_t no);
uint32_t db_get(sql::Connection *con, std::string address);
void db_insert(sql::Connection *con, std::string address, uint32_t cluster);
sql::Connection *db_init_connection();
