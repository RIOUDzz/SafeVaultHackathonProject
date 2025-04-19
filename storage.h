// This section is used to prevent multiple inclusions of the header file
#ifndef STORAGE_H
#define STORAGE_H

#include <time.h>
#include <stdbool.h>
typedef struct {
    char id[37];  // UUID string
    char username[50];
    char category[30];
    time_t timestamp;
    size_t message_len;
    char *encrypted_message;
} Message;

void init_storage();
bool save_message(const char *username, const char *encrypted_message, const char *category);
bool delete_message(const char *message_id, const char *username);
Message** search_messages(const char *username, const char *search_term, int *count);
bool edit_message(const char *message_id, const char *username, const char *new_message);
void sort_messages(Message **messages, int count, const char *sort_by);

// Save an encrypted message to file
void save_encrypted_message(const char *username, const char *encrypted_message);

// Load and print encrypted messages for a user
void load_messages_for_user(const char *username);

#endif
