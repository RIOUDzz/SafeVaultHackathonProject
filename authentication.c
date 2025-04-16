#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_USERS 100
#define USERNAME_LENGTH 50
#define PASSWORD_LENGTH 50
#define USERS_FILE "users.dat"

typedef struct {
    char username[USERNAME_LENGTH];
    int hashed_password;
} User;

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

int hash_password(const char *password) {
    if (!password) return 0;
    
    int hash = 5381;  // Better initial value
    for (int i = 0; password[i] != '\0'; i++) {
        hash = ((hash << 5) + hash) + password[i];  // Better hashing algorithm
    }
    return hash;
}

bool register_user(const char *username, const char *password) {
    printf("Debug: Registering user '%s'\n", username);
    printf("Debug: Current user count: %d\n", user_count);
    
    // Check if username already exists
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            printf("Debug: Username already exists\n");
            return false;
        }
    }
    
    // Add new user
    strcpy(users[user_count].username, username);
    users[user_count].hashed_password = hash_password(password);
    user_count++;
    save_users(); // Save after registration
    printf("Debug: User registered successfully\n");
    return true;
}

bool login_user(const char *username, const char *password) {
    printf("Debug: Attempting login for user '%s'\n", username);
    printf("Debug: Current user count: %d\n", user_count);
    
    int hashed = hash_password(password);
    printf("Debug: Hash of entered password: %d\n", hashed);
    
    for (int i = 0; i < user_count; i++) {
        printf("Debug: Checking user %d: '%s'\n", i, users[i].username);
        printf("Debug: Stored hash for user %d: %d\n", i, users[i].hashed_password);
        if (strcmp(users[i].username, username) == 0 && users[i].hashed_password == hashed) {
            printf("Debug: Login successful\n");
            return true;
        }
    }
    printf("Debug: Login failed\n");
    return false;
}

// Initialize the authentication system
void init_auth() {
    memset(users, 0, sizeof(users));  // Initialize all users to zero
    user_count = 0;
    load_users();
}
