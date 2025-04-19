#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <ctype.h>
#include <mysql/mysql.h>
#include "authentication.h"
#include "database.h"

#define MAX_USERS 100

// Define the global variables
User users[MAX_USERS];
int user_count = 0;

// Load users from file
void load_users() {
    // This line of code opens the file specified by USERS_FILE in binary read mode.
    // The "rb" argument to fopen() indicates that the file should be opened for reading in binary mode.
    // This is necessary for reading the file's contents into memory without any formatting or interpretation.
    FILE *file = fopen(USERS_FILE, "rb");
    if (file) {
        user_count = fread(users, sizeof(User), MAX_USERS, file);
        fclose(file);
        printf("Debug: Loaded %d users\n", user_count);
    }
}

// Save users to file
void save_users() {
    FILE *file = fopen(USERS_FILE, "wb");
    if (!file) {
        printf("Error: Could not open users file for writing\n");
        return;
    }
    
    if (fwrite(users, sizeof(User), user_count, file) != user_count) {
        printf("Error: Could not write all users to file\n");
    }
    
    fclose(file);
    printf("Debug: Saved %d users\n", user_count);
}

// Password complexity requirements
bool is_password_complex(const char *password) {
    int len = strlen(password);
    bool has_upper = false, has_lower = false;
    bool has_digit = false, has_special = false;
    
    if (len < 8) return false;
    
    for (int i = 0; i < len; i++) {
        if (isupper(password[i])) has_upper = true;
        if (islower(password[i])) has_lower = true;
        if (isdigit(password[i])) has_digit = true;
        if (!isalnum(password[i])) has_special = true;
    }
    
    return has_upper && has_lower && has_digit && has_special;
}

// Generate random salt
void generate_salt(unsigned char *salt) {
    RAND_bytes(salt, SALT_LENGTH);
}

// Hash password using PBKDF2
void hash_password_with_salt(const char *password, const unsigned char *salt, unsigned char *hash) {
    PKCS5_PBKDF2_HMAC(password, strlen(password),
                      salt, SALT_LENGTH,
                      10000, // iterations
                      EVP_sha256(),
                      HASH_LENGTH,
                      hash);
}

bool register_user(const char *username, const char *password) {
    if (!is_password_complex(password)) {
        printf("Password must be at least 8 characters long and contain uppercase, lowercase, digits, and special characters.\n");
        return false;
    }

    MYSQL *conn = get_db_connection();
    if (!conn) return false;

    // Generate salt and hash password
    unsigned char salt[SALT_LENGTH];
    unsigned char hash[HASH_LENGTH];
    generate_salt(salt);
    hash_password_with_salt(password, salt, hash);

    // Prepare SQL statement
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() failed\n");
        return false;
    }

    const char *query = "INSERT INTO users (username, hashed_password, salt) VALUES (?, ?, ?)";
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "mysql_stmt_prepare() failed\n");
        mysql_stmt_close(stmt);
        return false;
    }

    // Bind parameters
    MYSQL_BIND bind[3];
    memset(bind, 0, sizeof(bind));

    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = (char*)username;
    bind[0].buffer_length = strlen(username);

    bind[1].buffer_type = MYSQL_TYPE_BLOB;
    bind[1].buffer = hash;
    bind[1].buffer_length = HASH_LENGTH;

    bind[2].buffer_type = MYSQL_TYPE_BLOB;
    bind[2].buffer = salt;
    bind[2].buffer_length = SALT_LENGTH;

    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() failed\n");
        mysql_stmt_close(stmt);
        return false;
    }

    // Execute statement
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() failed\n");
        mysql_stmt_close(stmt);
        return false;
    }

    mysql_stmt_close(stmt);
    return true;
}

bool login_user(const char *username, const char *password) {
    MYSQL *conn = get_db_connection();
    if (!conn) return false;

    // Prepare SQL statement
    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() failed\n");
        return false;
    }

    const char *query = "SELECT hashed_password, salt, failed_attempts, is_locked, last_attempt "
                       "FROM users WHERE username = ?";
    
    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "mysql_stmt_prepare() failed\n");
        mysql_stmt_close(stmt);
        return false;
    }

    // Bind username parameter
    MYSQL_BIND bind_param[1];
    memset(bind_param, 0, sizeof(bind_param));
    
    bind_param[0].buffer_type = MYSQL_TYPE_STRING;
    bind_param[0].buffer = (char*)username;
    bind_param[0].buffer_length = strlen(username);

    if (mysql_stmt_bind_param(stmt, bind_param)) {
        fprintf(stderr, "mysql_stmt_bind_param() failed\n");
        mysql_stmt_close(stmt);
        return false;
    }

    // Execute statement
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "mysql_stmt_execute() failed\n");
        mysql_stmt_close(stmt);
        return false;
    }

    // Bind result buffers
    MYSQL_BIND bind_result[5];
    unsigned char db_hash[HASH_LENGTH];
    unsigned char db_salt[SALT_LENGTH];
    int failed_attempts;
    my_bool is_locked;
    MYSQL_TIME last_attempt;
    
    memset(bind_result, 0, sizeof(bind_result));

    bind_result[0].buffer_type = MYSQL_TYPE_BLOB;
    bind_result[0].buffer = db_hash;
    bind_result[0].buffer_length = HASH_LENGTH;

    bind_result[1].buffer_type = MYSQL_TYPE_BLOB;
    bind_result[1].buffer = db_salt;
    bind_result[1].buffer_length = SALT_LENGTH;

    bind_result[2].buffer_type = MYSQL_TYPE_LONG;
    bind_result[2].buffer = &failed_attempts;

    bind_result[3].buffer_type = MYSQL_TYPE_TINY;
    bind_result[3].buffer = &is_locked;

    bind_result[4].buffer_type = MYSQL_TYPE_TIMESTAMP;
    bind_result[4].buffer = &last_attempt;

    if (mysql_stmt_bind_result(stmt, bind_result)) {
        fprintf(stderr, "mysql_stmt_bind_result() failed\n");
        mysql_stmt_close(stmt);
        return false;
    }

    // Fetch result
    if (mysql_stmt_fetch(stmt)) {
        fprintf(stderr, "User not found\n");
        mysql_stmt_close(stmt);
        return false;
    }

    mysql_stmt_close(stmt);

    // Check account lockout
    if (is_locked) {
        time_t current_time = time(NULL);
        struct tm timeinfo = {0};
        timeinfo.tm_year = last_attempt.year - 1900;
        timeinfo.tm_mon = last_attempt.month - 1;
        timeinfo.tm_mday = last_attempt.day;
        timeinfo.tm_hour = last_attempt.hour;
        timeinfo.tm_min = last_attempt.minute;
        timeinfo.tm_sec = last_attempt.second;
        time_t last_attempt_time = mktime(&timeinfo);
        
        if (current_time - last_attempt_time < LOCKOUT_DURATION) {
            printf("Account is locked. Try again in %ld seconds\n", 
                   LOCKOUT_DURATION - (current_time - last_attempt_time));
            return false;
        }
        
        // Reset lockout if duration has passed
        update_user_lockout(username, false, 0);
    }

    // Verify password
    unsigned char hash[HASH_LENGTH];
    hash_password_with_salt(password, db_salt, hash);
    
    if (memcmp(hash, db_hash, HASH_LENGTH) == 0) {
        // Reset failed attempts on successful login
        update_user_lockout(username, false, 0);
        return true;
    }

    // Handle failed attempt
    failed_attempts++;
    bool should_lock = failed_attempts >= MAX_LOGIN_ATTEMPTS;
    update_user_lockout(username, should_lock, failed_attempts);
    
    if (should_lock) {
        printf("Account locked due to too many failed attempts\n");
    }
    
    return false;
}

// Initialize the authentication system
void init_auth() {
    memset(users, 0, sizeof(users));  // Initialize all users to zero
    user_count = 0;
    load_users();
}

bool update_user_lockout(const char *username, bool should_lock, int failed_attempts) {
    MYSQL *conn = get_db_connection();
    if (!conn) return false;

    MYSQL_STMT *stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "mysql_stmt_init() failed\n");
        return false;
    }

    const char *query = "UPDATE users SET is_locked = ?, failed_attempts = ?, last_attempt = NOW() "
                       "WHERE username = ?";

    if (mysql_stmt_prepare(stmt, query, strlen(query))) {
        fprintf(stderr, "mysql_stmt_prepare() failed\n");
        mysql_stmt_close(stmt);
        return false;
    }

    MYSQL_BIND bind[3];
    memset(bind, 0, sizeof(bind));
    
    my_bool is_locked = should_lock;
    bind[0].buffer_type = MYSQL_TYPE_TINY;
    bind[0].buffer = &is_locked;
    bind[0].is_null = 0;

    bind[1].buffer_type = MYSQL_TYPE_LONG;
    bind[1].buffer = &failed_attempts;
    bind[1].is_null = 0;

    bind[2].buffer_type = MYSQL_TYPE_STRING;
    bind[2].buffer = (char*)username;
    bind[2].buffer_length = strlen(username);
    bind[2].is_null = 0;

    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "mysql_stmt_bind_param() failed\n");
        mysql_stmt_close(stmt);
        return false;
    }

    bool success = (mysql_stmt_execute(stmt) == 0);
    mysql_stmt_close(stmt);
    return success;
}
