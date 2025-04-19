#include "encryption.h"
#include <openssl/rand.h>
#include <string.h>

void init_encryption() {
    OpenSSL_add_all_algorithms();
}

void generate_encryption_context(EncryptionContext *ctx) {
    RAND_bytes(ctx->key, AES_KEY_SIZE/8);
    RAND_bytes(ctx->iv, IV_SIZE);
}

bool encrypt_message(const char *plaintext, char *ciphertext, size_t *ciphertext_len, const EncryptionContext *ctx) {
    EVP_CIPHER_CTX *evp_ctx = EVP_CIPHER_CTX_new();
    if (!evp_ctx) return false;
    
    if (EVP_EncryptInit_ex(evp_ctx, EVP_aes_256_gcm(), NULL, ctx->key, ctx->iv) != 1) {
        EVP_CIPHER_CTX_free(evp_ctx);
        return false;
    }
    
    int len;
    if (EVP_EncryptUpdate(evp_ctx, (unsigned char*)ciphertext, &len, 
                         (const unsigned char*)plaintext, strlen(plaintext)) != 1) {
        EVP_CIPHER_CTX_free(evp_ctx);
        return false;
    }
    
    int final_len;
    if (EVP_EncryptFinal_ex(evp_ctx, (unsigned char*)ciphertext + len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(evp_ctx);
        return false;
    }
    
    *ciphertext_len = len + final_len;
    EVP_CIPHER_CTX_free(evp_ctx);
    return true;
}

bool decrypt_message(const char *ciphertext, size_t ciphertext_len, char *plaintext, const EncryptionContext *ctx) {
    EVP_CIPHER_CTX *evp_ctx = EVP_CIPHER_CTX_new();
    if (!evp_ctx) return false;
    
    if (EVP_DecryptInit_ex(evp_ctx, EVP_aes_256_gcm(), NULL, ctx->key, ctx->iv) != 1) {
        EVP_CIPHER_CTX_free(evp_ctx);
        return false;
    }
    
    int len;
    if (EVP_DecryptUpdate(evp_ctx, (unsigned char*)plaintext, &len,
                         (const unsigned char*)ciphertext, ciphertext_len) != 1) {
        EVP_CIPHER_CTX_free(evp_ctx);
        return false;
    }
    
    int final_len;
    if (EVP_DecryptFinal_ex(evp_ctx, (unsigned char*)plaintext + len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(evp_ctx);
        return false;
    }
    
    plaintext[len + final_len] = '\0';
    EVP_CIPHER_CTX_free(evp_ctx);
    return true;
}