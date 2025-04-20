#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "storage.h"
#include "database.h"
#include "encryption.h"

void save_encrypted_message(const char *username, const char *message) {
    MYSQL *conn = get_db_connection();
    if (!conn) {
        printf("Database connection failed\n");
        return;
    }

    // Encrypt the message using Caesar cipher
    char* encrypted_message = caesar_encrypt(message);
    if (!encrypted_message) {
        printf("Encryption failed\n");
        return;
    }

    // Prepare SQL statement
    const char *query = "INSERT INTO messages (user_id, encrypted_message) "
                       "SELECT id, ? FROM users WHERE username = ?";
    
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        printf("Failed to initialize statement\n");
        free(encrypted_message);
        return;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        printf("Statement preparation failed\n");
        mysql_stmt_close(stmt);
        free(encrypted_message);
        return;
    }

    // Bind parameters
    MYSQL_BIND bind[2];
    memset(bind, 0, sizeof(bind));

    // Encrypted message
    unsigned long message_len = strlen(encrypted_message);
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = encrypted_message;
    bind[0].buffer_length = message_len;
    bind[0].length = &message_len;

    // Username
    unsigned long username_len = strlen(username);
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)username;
    bind[1].buffer_length = username_len;
    bind[1].length = &username_len;

    if (mysql_stmt_bind_param(stmt, bind)) {
        printf("Parameter binding failed\n");
        mysql_stmt_close(stmt);
        free(encrypted_message);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        printf("Statement execution failed\n");
    } else {
        printf("Message encrypted and saved successfully!\n");
    }

    mysql_stmt_close(stmt);
    free(encrypted_message);
}

void load_messages_for_user(const char *username) {
    MYSQL *conn = get_db_connection();
    if (!conn) {
        printf("Database connection failed\n");
        return;
    }

    const char *query = "SELECT encrypted_message "
                       "FROM messages m "
                       "JOIN users u ON m.user_id = u.id "
                       "WHERE u.username = ? "
                       "ORDER BY m.timestamp DESC";

    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        printf("Failed to initialize statement\n");
        return;
    }

    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        printf("Statement preparation failed\n");
        mysql_stmt_close(stmt);
        return;
    }

    // Bind username parameter
    MYSQL_BIND param_bind[1];
    memset(param_bind, 0, sizeof(param_bind));
    
    unsigned long username_len = strlen(username);
    param_bind[0].buffer_type = MYSQL_TYPE_STRING;
    param_bind[0].buffer = (char*)username;
    param_bind[0].buffer_length = username_len;
    param_bind[0].length = &username_len;

    if (mysql_stmt_bind_param(stmt, param_bind)) {
        printf("Parameter binding failed\n");
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        printf("Statement execution failed\n");
        mysql_stmt_close(stmt);
        return;
    }

    // Prepare result buffer
    MYSQL_BIND result_bind[1];
    memset(result_bind, 0, sizeof(result_bind));
    
    char encrypted_message[1024];
    unsigned long message_len;

    // Bind encrypted message
    result_bind[0].buffer_type = MYSQL_TYPE_STRING;
    result_bind[0].buffer = encrypted_message;
    result_bind[0].buffer_length = sizeof(encrypted_message);
    result_bind[0].length = &message_len;

    if (mysql_stmt_bind_result(stmt, result_bind)) {
        printf("Result binding failed\n");
        mysql_stmt_close(stmt);
        return;
    }

    int count = 0;
    while (!mysql_stmt_fetch(stmt)) {
        // Decrypt message using Caesar cipher
        char* decrypted_message = caesar_decrypt(encrypted_message);
        if (decrypted_message) {
            printf("Message %d: %s\n", ++count, decrypted_message);
            free(decrypted_message);
        } else {
            printf("Message %d: (Failed to decrypt)\n", ++count);
        }
    }

    if (count == 0) {
        printf("No messages found\n");
    }

    mysql_stmt_close(stmt);
}

bool save_message(const char *username, const char *message) {
    save_encrypted_message(username, message);
    return true;
}
