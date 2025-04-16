# Secure Message Vault

A secure message storage system that allows users to register, login, and store encrypted messages.

## Features

- User registration and authentication
- Secure password hashing
- Message encryption using Caesar cipher
- Persistent storage of user data and messages
- Simple command-line interface
- Logout functionality
- Message viewing capability

## Project Structure 
├── main.c # Main program and user interface
├── authentication.c # User registration and login logic
├── authentication.h # Authentication function declarations
├── encryption.c # Message encryption/decryption
├── encryption.h # Encryption function declarations
├── storage.c # File storage operations
├── storage.h # Storage function declarations
└── README.md # This documentation


## Data Files

- `users.dat`: Stores user credentials (username and hashed passwords)
- `vault.txt`: Stores encrypted messages

## Security Features

### Password Hashing
Passwords are hashed using a custom algorithm(djb2):
```c
int hash_password(const char *password) {
    if (!password) return 0;
    
    int hash = 5381;  // Initial value
    for (int i = 0; password[i] != '\0'; i++) {
        hash = ((hash << 5) + hash) + password[i];
    }
    return hash;
}
```
- Uses a multiplicative hash function
- Initial value 5381 is a prime number
- Shifts and adds for better distribution
- Never stores plain text passwords

### Message Encryption
Messages are encrypted using a Caesar cipher:
```c
char caesar_encrypt(char ch, int key) {
    if (ch >= 'a' && ch <= 'z') {
        return 'a' + (ch - 'a' + key) % 26;
    } else if (ch >= 'A' && ch <= 'Z') {
        return 'A' + (ch - 'A' + key) % 26;
    }
    return ch;
}
```
- Shifts each letter by a fixed number (key)
- Preserves case (uppercase/lowercase)
- Only encrypts letters, leaves other characters unchanged

## User Flow

1. **Main Menu**
   - Register new user
   - Login existing user
   - Exit program

2. **Registration**
   - Enter username
   - Enter password
   - System hashes password and stores credentials

3. **Login**
   - Enter username
   - Enter password
   - System verifies credentials

4. **Logged-in Menu**
   - Write new message
   - View existing messages
   - Logout
   - Exit program

## Data Structures

### User Structure
```c
typedef struct {
    char username[USERNAME_LENGTH];
    int hashed_password;
} User;
```
- Stores username and hashed password
- Fixed size arrays for security

## File Operations

### User Storage
- Users are stored in binary format
- File operations include:
  - Loading users at startup
  - Saving new users
  - Error handling for file operations

### Message Storage
- Messages are stored in text format
- Each message is associated with a username
- Messages are encrypted before storage

## Error Handling

- Input validation for all user inputs
- File operation error checking
- Buffer overflow protection
- Memory initialization checks
- Username length validation
- Password length validation
- Menu choice validation

## Security Considerations

1. **Password Security**
   - Passwords are never stored in plain text
   - Uses hashing to protect passwords
   - Basic protection against password guessing

2. **Message Security**
   - Messages are encrypted before storage
   - Simple Caesar cipher (for educational purposes)
   - Messages are associated with specific users

3. **Input Validation**
   - Checks for buffer overflows
   - Validates all user inputs
   - Sanitizes file operations
   - Validates username and password lengths

## Constants and Configuration

#define MAX_LEN 255              // Maximum length for strings
#define USERNAME_LENGTH 50       // Maximum username length
#define PASSWORD_LENGTH 50       // Maximum password length
#define ENCRYPTION_KEY 3         // Caesar cipher shift value
#define MAX_USERS 100           // Maximum number of users

## Debug Features

- Debug messages for user input
- Debug messages for authentication
- Debug messages for file operations
- User count tracking
- Login attempt tracking

## Limitations

1. **Security**
   - Basic password hashing (not suitable for production)
   - Simple encryption (Caesar cipher is not secure)
   - No protection against brute force attacks
   - No password complexity requirements
   - No account lockout after failed attempts

2. **Features**
   - No password recovery
   - No message deletion
   - No user account management
   - No message editing
   - No message search
   - No message sorting

## Future Improvements

1. **Security**
   - Implement stronger password hashing (bcrypt/Argon2)
   - Add salt to password hashing
   - Implement stronger encryption (AES)
   - Add password complexity requirements
   - Implement account lockout
   - Add session management

2. **Features**
   - Add password recovery
   - Add message deletion
   - Add user account management
   - Add message search functionality
   - Add message editing
   - Add message sorting
   - Add message categories
   - Add message timestamps

## Usage

1. Compile the program:
   ```bash
   gcc *.c -o safevault
   ```

2. Run the program:
   ```bash
   ./safevault
   ```

3. Follow the on-screen instructions to:
   - Register a new account
   - Login to your account
   - Write and view messages
   - Logout when finished

## Troubleshooting

1. **Login Issues**
   - Verify username and password
   - Check if user is registered
   - Ensure correct case for username

2. **Message Issues**
   - Check message length
   - Verify encryption key
   - Check file permissions

3. **File Issues**
   - Check file permissions
   - Verify file existence
   - Check disk space

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.