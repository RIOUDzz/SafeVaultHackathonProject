#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>
#include <stdbool.h>
#include "authentication.h"

bool db_connect();
void db_disconnect();
bool db_init();
MYSQL* get_db_connection();

#endif
