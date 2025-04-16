// This section is used to prevent multiple inclusions of the header file
#ifndef ENCRYPTION_H
#define ENCRYPTION_H

// Encrypt the message using Caesar cipher
void encrypt_message(const char *message, char *encrypted, int key);

// Decrypt a Caesar cipher encrypted message
void decrypt_message(const char *encrypted, char *message, int key);

#endif
