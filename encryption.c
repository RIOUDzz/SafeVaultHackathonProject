#include "encryption.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* caesar_encrypt(const char* text) {
    if (!text) return NULL;
    
    int len = strlen(text);
    char* result = (char*)malloc(len + 1);
    
    for (int i = 0; i < len; i++) {
        if (isalpha(text[i])) {
            char base = isupper(text[i]) ? 'A' : 'a';
            result[i] = (text[i] - base + SHIFT_KEY) % 26 + base;
        } else {
            result[i] = text[i];  // Keep non-alphabetic characters unchanged
        }
    }
    result[len] = '\0';
    return result;
}

char* caesar_decrypt(const char* text) {
    if (!text) return NULL;
    
    int len = strlen(text);
    char* result = (char*)malloc(len + 1);
    
    for (int i = 0; i < len; i++) {
        if (isalpha(text[i])) {
            char base = isupper(text[i]) ? 'A' : 'a';
            result[i] = (text[i] - base - SHIFT_KEY + 26) % 26 + base;
        } else {
            result[i] = text[i];  // Keep non-alphabetic characters unchanged
        }
    }
    result[len] = '\0';
    return result;
}