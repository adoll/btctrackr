#include "db.hpp"
std::string table_name = "bchain";

uint32_t db_getmax(sql::Connection *con) {
    
    sql::Statement *stmt;
    sql::ResultSet *res;
    uint32_t result = 0;
    std::string query;
    
    query = "SELECT MAX(cluster) FROM " + table_name;

    stmt = con->createStatement();
    res = stmt->executeQuery(query);

    while (res->next()) {
        result = res->getUInt("MAX(cluster)");
    }
    
    delete res;
    delete stmt;

    return result;

}

std::unordered_set<std::string>* db_getset(sql::Connection *con, uint32_t cur_no) {

    sql::Statement *stmt;
    sql::ResultSet *res;
    std::unordered_set<std::string>* set = new std::unordered_set<std::string>();
    std::ostringstream q;
    std::string query;
    std::string result;
    
    q << "SELECT * FROM " + table_name + " WHERE cluster='" << cur_no << "'";
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
    
    query = "SELECT cluster FROM " + table_name + " WHERE address='" + address + "'";

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

void db_insert(sql::Connection *con, std::string address, uint32_t cluster) {

    sql::PreparedStatement *stmt;

    stmt = con->prepareStatement("REPLACE INTO " + table_name + "(address, cluster) VALUES (?, ?)");
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
    con = driver->connect("localhost", "root", "privacy");
    stmt = con->createStatement();

    stmt->execute("CREATE DATABASE IF NOT EXISTS " + table_name);
    stmt->execute("USE " + table_name);
   // stmt->execute("CREATE TABLE IF NOT EXISTS " + table_name + "(address VARCHAR(34) PRIMARY KEY, cluster INT)");

    delete stmt;
    return con;
}
/*
int main(int argc, char** argv) {

    sql::Connection *con;
    const char* addr1 = "1BGbGFBhsXYq6kTyjSC9AHRe1dhe76tD6i"; 
    const char* addr2 = "12sW3NCCG3v1hTeSHsDiPtDJxjwHmGn5nt";
    uint32_t max;

    con = db_init_connection();

    db_insert(con, addr1, 5); 
    db_insert(con, addr2, 7);

    max = db_getmax(con);
    std::cout << max << std::endl;

    delete con; 
}
*/
