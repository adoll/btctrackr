#include "db.hpp"

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
        std::cout << result << std::endl;
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
        stmt->execute("USE test");
        stmt->execute("DROP TABLE IF EXISTS test");
        stmt->execute("CREATE TABLE test(address VARCHAR(34), cluster INT)");
 
        delete stmt;
        return con;
}
/*
int main(int argc, char** argv) {

    sql::Connection *con;
    const char* addr1 = "1BGbGFBhsXYq6kTyjSC9AHRe1dhe76tD6i"; 
    const char* addr2 = "12sW3NCCG3v1hTeSHsDiPtDJxjwHmGn5nt";
    int result;

    con = db_init_connection();

    if (db_get(con, addr1) == 0)
        db_insert(con, addr1, 5);
    else
        db_update(con, addr1, 19);

    if (db_get(con, addr1) == 0)
        db_insert(con, addr1, 5);
    else
        db_update(con, addr1, 19);

    result = db_get(con, addr2);
    std::cout << result << std::endl; 

    delete con; 
}
*/
