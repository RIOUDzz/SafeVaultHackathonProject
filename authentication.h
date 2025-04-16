// This section is used to prevent multiple inclusions of the header file
#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <stdbool.h>

// Initialize the authentication system
void init_auth();

// Register a new user with a hashed password
bool register_user(const char *username, const char *password);

// Verify user login by comparing hashed password
bool login_user(const char *username, const char *password);

#endif
