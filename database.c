#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include "database.h"
#include "db_config.h"

static MYSQL *conn = NULL;

bool db_connect() {
    conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "Error: mysql_init() failed\n");
        return false;
    }

    if (mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, 
                          DB_NAME, DB_PORT, NULL, 0) == NULL) {
        fprintf(stderr, "Error: Failed to connect to database\n");
        fprintf(stderr, "Error message: %s\n", mysql_error(conn));
        mysql_close(conn);
        conn = NULL;
        return false;
    }

    printf("Successfully connected to MySQL database\n");
    return true;
}

void db_disconnect() {
    if (conn) {
        mysql_close(conn);
        conn = NULL;
    }
}

MYSQL* get_db_connection() {
    return conn;
}

bool db_init() {
    if (!db_connect()) {
        return false;
    }

    // Create tables if they don't exist
    const char *create_users_table = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "username VARCHAR(50) UNIQUE NOT NULL,"
        "hashed_password BINARY(32) NOT NULL,"
        "salt BINARY(16) NOT NULL,"
        "failed_attempts INT DEFAULT 0,"
        "last_attempt TIMESTAMP NULL,"
        "is_locked BOOLEAN DEFAULT FALSE"
        ")";

    const char *create_messages_table = 
        "CREATE TABLE IF NOT EXISTS messages ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "user_id INT NOT NULL,"
        "encrypted_message TEXT NOT NULL,"
        "category VARCHAR(30),"
        "timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (user_id) REFERENCES users(id)"
        ")";

    if (mysql_query(conn, create_users_table)) {
        fprintf(stderr, "Error creating users table: %s\n", mysql_error(conn));
        return false;
    }

    if (mysql_query(conn, create_messages_table)) {
        fprintf(stderr, "Error creating messages table: %s\n", mysql_error(conn));
        return false;
    }

    return true;
}
