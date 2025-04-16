#include <string.h>

char caesar_encrypt(char ch, int key) {
    if (ch >= 'a' && ch <= 'z') {
        return 'a' + (ch - 'a' + key) % 26;
    } else if (ch >= 'A' && ch <= 'Z') {
        return 'A' + (ch - 'A' + key) % 26;
    }
    return ch;
}

char caesar_decrypt(char ch, int key) {
    return caesar_encrypt(ch, 26 - key);
}

void encrypt_message(const char *message, char *encrypted, int key) {
    for (int i = 0; message[i] != '\0'; i++) {
        encrypted[i] = caesar_encrypt(message[i], key);
    }
    encrypted[strlen(message)] = '\0';
}

void decrypt_message(const char *encrypted, char *message, int key) {
    for (int i = 0; encrypted[i] != '\0'; i++) {
        message[i] = caesar_decrypt(encrypted[i], key);
    }
    message[strlen(encrypted)] = '\0';
}