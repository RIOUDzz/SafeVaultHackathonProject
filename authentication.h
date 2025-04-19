// This section is used to prevent multiple inclusions of the header file
#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <stdbool.h>
#include <time.h>

// Add these definitions BEFORE they are used in the struct and extern declarations
#define APP_USERNAME_LENGTH 50
#define MAX_USERS 100
#define USERS_FILE "users.dat"

#define SALT_LENGTH 16
#define HASH_LENGTH 32
#define MAX_LOGIN_ATTEMPTS 3
#define LOCKOUT_DURATION 300 // 5 minutes in seconds

// Now we can use APP_USERNAME_LENGTH in the struct definition
typedef struct {
    char username[APP_USERNAME_LENGTH];
    unsigned char hashed_password[HASH_LENGTH];
    unsigned char salt[SALT_LENGTH];
    int failed_attempts;
    time_t last_attempt;
    bool is_locked;
} User;

// And MAX_USERS in the extern declaration
extern User users[MAX_USERS];
extern int user_count;

// Function declarations
void init_auth();
bool register_user(const char *username, const char *password);
bool login_user(const char *username, const char *password);
bool is_password_complex(const char *password);
bool update_user_lockout(const char *username, bool should_lock, int failed_attempts);

#endif
