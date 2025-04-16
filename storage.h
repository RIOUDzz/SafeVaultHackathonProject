// This section is used to prevent multiple inclusions of the header file
#ifndef STORAGE_H
#define STORAGE_H

// Save an encrypted message to file
void save_encrypted_message(const char *username, const char *encrypted_message);

// Load and print encrypted messages for a user
void load_messages_for_user(const char *username);

#endif
