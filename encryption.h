// This section is used to prevent multiple inclusions of the header file
#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <stdbool.h>

#define AES_KEY_SIZE 256
#define IV_SIZE 16

typedef struct {
    unsigned char key[AES_KEY_SIZE/8];
    unsigned char iv[IV_SIZE];
} EncryptionContext;

void init_encryption();
bool encrypt_message(const char *plaintext, char *ciphertext, size_t *ciphertext_len, const EncryptionContext *ctx);
bool decrypt_message(const char *ciphertext, size_t ciphertext_len, char *plaintext, const EncryptionContext *ctx);
void generate_encryption_context(EncryptionContext *ctx);

#endif
