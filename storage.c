#include <stdio.h>
#include <string.h>

void save_encrypted_message(const char *username, const char *encrypted_message) {
    FILE *file = fopen("vault.txt", "a");
    if (file) {
        fprintf(file, "%s:%s\n", username, encrypted_message);
        fclose(file);
    }
}

void load_messages_for_user(const char *username) {
    FILE *file = fopen("vault.txt", "r");
    if (!file) return;
    
    char line[200];
    int message_count = 1;
    
    while (fgets(line, sizeof(line), file)) {
        char *user = strtok(line, ":");
        char *msg = strtok(NULL, "\n");
        if (user && msg && strcmp(user, username) == 0) {
            printf("Message %d: %s\n", message_count++, msg);
        }
    }
    fclose(file);
}
