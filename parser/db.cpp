#include "db.hpp"
#include <sstream>

std::unordered_set<std::string>* db_getset(sql::Connection *con, uint32_t cur_no) {

    sql::Statement *stmt;
    sql::ResultSet *res;
    std::unordered_set<std::string>* set = new std::unordered_set<std::string>();
    std::ostringstream q;
    std::string query;
    std::string result;
    
    q << "SELECT * FROM test WHERE cluster='" << cur_no << "'";
    query = q.str();

    if (con == NULL) { 
        fprintf(stderr, "NULL connection\n");    
    }

    stmt = con->createStatement();
    res = stmt->executeQuery(query);

    while (res->next()) {
        result = res->getString("address");
        set->insert(result);
    }
    
    delete res;
    delete stmt;

    return set;

}

uint32_t db_get(sql::Connection *con, std::string address) {

    sql::Statement *stmt;
    sql::ResultSet *res;
    uint32_t result = 0;
    std::string query;
    
    query = "SELECT cluster FROM test WHERE address LIKE '" + address + "'";

    if (con == NULL) { 
        fprintf(stderr, "NULL connection\n");    
    }

    stmt = con->createStatement();
    res = stmt->executeQuery(query);

    while (res->next()) {
        result = res->getUInt("cluster");
    }
    
    delete res;
    delete stmt;

    return result;

}

void db_update(sql::Connection *con, std::string address, uint32_t cluster) {

    sql::PreparedStatement *stmt;

    stmt = con->prepareStatement("UPDATE test SET address = ?, cluster = ? WHERE address = ?");
    stmt->setString(1, address);
    stmt->setUInt(2, cluster);
    stmt->setString(3, address); 
    stmt->execute();

    delete stmt;
}

void db_insert(sql::Connection *con, std::string address, uint32_t cluster) {

    sql::PreparedStatement *stmt;

    stmt = con->prepareStatement("INSERT INTO test(address, cluster) VALUES (?, ?)");
    stmt->setString(1, address);
    stmt->setUInt(2, cluster);
    
    stmt->execute();

    delete stmt;
}

sql::Connection *db_init_connection() {
   
        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;

        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("localhost", "root", "");
        stmt = con->createStatement();
        
	//stmt->execute("DROP TABLE IF EXISTS test");
	stmt->execute("CREATE DATABASE IF NOT EXISTS test");
	stmt->execute("USE test");
	stmt->execute("CREATE TABLE IF NOT EXISTS test(address VARCHAR(34), cluster INT)");
 
        delete stmt;
        return con;
}
/*
int main(int argc, char** argv) {

    sql::Connection *con;
    const char* addr1 = "1BGbGFBhsXYq6kTyjSC9AHRe1dhe76tD6i"; 
    const char* addr2 = "12sW3NCCG3v1hTeSHsDiPtDJxjwHmGn5nt";
    std::unordered_set<std::string>* set;

    con = db_init_connection();

    db_insert(con, addr1, 5);
    db_insert(con, addr2, 5);

    set = db_getset(con, 5);

    for (auto result = set->begin(); result != set->end(); result++)
        std::cout << *result << std::endl;

    delete con; 
}
*/
