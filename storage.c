#include <stdio.h>
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

    // Generate encryption context
    EncryptionContext ctx;
    generate_encryption_context(&ctx);

    // Encrypt the message
    char encrypted_message[1024];
    size_t encrypted_len;
    
    if (!encrypt_message(message, encrypted_message, &encrypted_len, &ctx)) {
        printf("Encryption failed\n");
        return;
    }

    // Prepare SQL statement
    const char *query = "INSERT INTO messages (user_id, encrypted_message, encryption_key, encryption_iv) "
                       "SELECT id, ?, ?, ? FROM users WHERE username = ?";
    
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

    // Bind parameters
    MYSQL_BIND bind[4];
    memset(bind, 0, sizeof(bind));

    // Encrypted message
    bind[0].buffer_type = MYSQL_TYPE_BLOB;
    bind[0].buffer = encrypted_message;
    bind[0].buffer_length = encrypted_len;
    bind[0].length = &encrypted_len;

    // Encryption key
    unsigned long key_len = AES_KEY_SIZE/8;
    bind[1].buffer_type = MYSQL_TYPE_BLOB;
    bind[1].buffer = ctx.key;
    bind[1].buffer_length = key_len;
    bind[1].length = &key_len;

    // Encryption IV
    unsigned long iv_len = IV_SIZE;
    bind[2].buffer_type = MYSQL_TYPE_BLOB;
    bind[2].buffer = ctx.iv;
    bind[2].buffer_length = iv_len;
    bind[2].length = &iv_len;

    // Username
    unsigned long username_len = strlen(username);
    bind[3].buffer_type = MYSQL_TYPE_STRING;
    bind[3].buffer = (char*)username;
    bind[3].buffer_length = username_len;
    bind[3].length = &username_len;

    if (mysql_stmt_bind_param(stmt, bind)) {
        printf("Parameter binding failed\n");
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        printf("Statement execution failed\n");
    } else {
        printf("Message encrypted and saved successfully!\n");
    }

    mysql_stmt_close(stmt);
}

void load_messages_for_user(const char *username) {
    MYSQL *conn = get_db_connection();
    if (!conn) {
        printf("Database connection failed\n");
        return;
    }

    const char *query = "SELECT encrypted_message, encryption_key, encryption_iv "
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

    // Prepare result buffers
    MYSQL_BIND result_bind[3];
    memset(result_bind, 0, sizeof(result_bind));
    
    char encrypted_message[1024];
    unsigned char key[AES_KEY_SIZE/8];
    unsigned char iv[IV_SIZE];
    unsigned long message_len, key_len, iv_len;

    // Bind encrypted message
    result_bind[0].buffer_type = MYSQL_TYPE_BLOB;
    result_bind[0].buffer = encrypted_message;
    result_bind[0].buffer_length = sizeof(encrypted_message);
    result_bind[0].length = &message_len;

    // Bind encryption key
    result_bind[1].buffer_type = MYSQL_TYPE_BLOB;
    result_bind[1].buffer = key;
    result_bind[1].buffer_length = sizeof(key);
    result_bind[1].length = &key_len;

    // Bind encryption IV
    result_bind[2].buffer_type = MYSQL_TYPE_BLOB;
    result_bind[2].buffer = iv;
    result_bind[2].buffer_length = sizeof(iv);
    result_bind[2].length = &iv_len;

    if (mysql_stmt_bind_result(stmt, result_bind)) {
        printf("Result binding failed\n");
        mysql_stmt_close(stmt);
        return;
    }

    int count = 0;
    while (!mysql_stmt_fetch(stmt)) {
        // Create encryption context
        EncryptionContext ctx;
        memcpy(ctx.key, key, AES_KEY_SIZE/8);
        memcpy(ctx.iv, iv, IV_SIZE);

        // Decrypt message
        char decrypted_message[1024];
        if (decrypt_message(encrypted_message, message_len, decrypted_message, &ctx)) {
            printf("Message %d: %s\n", ++count, decrypted_message);
        } else {
            printf("Message %d: (Failed to decrypt)\n", ++count);
        }
    }

    if (count == 0) {
        printf("No messages found\n");
    }

    mysql_stmt_close(stmt);
}

bool save_message(const char *username, const char *encrypted_message, const char *category) {
    MYSQL *conn = get_db_connection();
    if (!conn) return false;

    // Prepare SQL statement
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() failed\n");
        return false;
    }

    const char *query = "INSERT INTO messages (user_id, encrypted_message, category) "
                       "SELECT id, ?, ? FROM users WHERE username = ?";

    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "mysql_stmt_prepare() failed\n");
        mysql_stmt_close(stmt);
        return false;
    }

    // Bind parameters
    MYSQL_BIND bind[3];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (char*)encrypted_message;
    bind[0].buffer_length = strlen(encrypted_message);

    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = (char*)category;
    bind[1].buffer_length = strlen(category);

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (char*)username;
    bind[2].buffer_length = strlen(username);

    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() failed\n");
        mysql_stmt_close(stmt);
        return false;
    }

    bool success = (mysql_stmt_execute(stmt) == 0);
    mysql_stmt_close(stmt);
    return success;
}
