// This section is used to prevent multiple inclusions of the header file
#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stdbool.h>
#include <stddef.h>

#define SHIFT_KEY 3  // Caesar cipher shift value

char* caesar_encrypt(const char* text);
char* caesar_decrypt(const char* text);

#endif
