#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "authentication.h"
#include "encryption.h"
#include "storage.h"
#include "database.h"

#define MAX_LEN 255
#define ENCRYPTION_KEY 3

int main() {
    // Initialize the systems
    init_auth();
    if (!db_init()) {
        printf("Failed to initialize database connection\n");
        return 1;
    }
    
    char username[MAX_LEN];
    char password[MAX_LEN];
    int choice;
    char input[MAX_LEN];

    while (1) {
        printf("1. Register\n2. Login\n3. Exit\nEnter your choice (1, 2, or 3): ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error reading input\n");
            return 1;
        }
        
        if (sscanf(input, "%d", &choice) != 1 || (choice != 1 && choice != 2 && choice != 3)) {
            printf("Invalid choice. Please enter 1, 2, or 3.\n");
            continue;
        }

        if (choice == 3) {
            printf("Goodbye!\n");
            db_disconnect();
            return 0;
        }

        printf("Enter username: ");
        if (fgets(username, sizeof(username), stdin) == NULL) {
            printf("Error reading username\n");
            return 1;
        }
        username[strcspn(username, "\n")] = '\0';
        
        printf("Enter password: ");
        if (fgets(password, sizeof(password), stdin) == NULL) {
            printf("Error reading password\n");
            return 1;
        }
        password[strcspn(password, "\n")] = '\0';

        bool success;
        if (choice == 1) {
            success = register_user(username, password);
            printf("Registration %s\n", success ? "successful" : "failed");
        } else {
            success = login_user(username, password);
            printf("Login %s\n", success ? "successful" : "failed");
        }

        if (!success) {
            printf("Failed to %s user.\n", (choice == 1) ? "register" : "login");
            continue;
        }

        printf("\nWelcome, %s!\n", username);

        while (1) {
            printf("\nMenu:\n");
            printf("1. Write a new message\n");
            printf("2. View my messages\n");
            printf("3. Logout\n");
            printf("4. Exit\n");
            printf("Enter your choice: ");
            
            if (fgets(input, sizeof(input), stdin) == NULL) {
                printf("Error reading input\n");
                return 1;
            }
            
            if (sscanf(input, "%d", &choice) != 1 || choice < 1 || choice > 4) {
                printf("Invalid input. Please enter a number between 1 and 4.\n");
                continue;
            }

            switch (choice) {
                case 1: {
                    char message[MAX_LEN];
                    
                    printf("Write your secret message: ");
                    if (fgets(message, sizeof(message), stdin) == NULL) {
                        printf("Error reading message\n");
                        break;
                    }
                    message[strcspn(message, "\n")] = '\0';

                    if (!save_message(username, message)) {
                        printf("Failed to save message\n");
                        break;
                    }
                    
                    printf("Message encrypted and saved successfully!\n");
                    break;
                }
                case 2:
                    printf("\nYour Messages:\n");
                    load_messages_for_user(username);
                    break;
                case 3:
                    printf("Logged out successfully!\n");
                    goto main_menu;
                case 4:
                    printf("Goodbye!\n");
                    db_disconnect();
                    return 0;
                default:
                    printf("Invalid choice. Please try again.\n");
            }
        }
        
        main_menu: ;  // Label for the goto statement
    }

    return 0;
}
