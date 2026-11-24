#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <limits>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>

using namespace std;

const string FILE_NAME = "users.txt";

const int SALT_SIZE = 16;
const int HASH_SIZE = 32;
const int ITERATIONS = 600000;
const int MAX_LOGIN_ATTEMPTS = 3;

// Convert binary data into hexadecimal
string toHex(const unsigned char data[], int length) {

    stringstream ss;

    for (int i = 0; i < length; i++) {
        ss << hex << setw(2) << setfill('0')
           << static_cast<int>(data[i]);
    }

    return ss.str();
}

// Convert hexadecimal into binary
bool fromHex(const string& hex, unsigned char output[],
             int expectedSize) {

    if (hex.length() != expectedSize * 2)
        return false;

    for (int i = 0; i < expectedSize; i++) {

        string byteString = hex.substr(i * 2, 2);

        try {
            size_t position;
            int value = stoi(byteString, &position, 16);

            if (position != 2 || value < 0 || value > 255)
                return false;

            output[i] = static_cast<unsigned char>(value);
        }
        catch (...) {
            return false;
        }
    }

    return true;
}

// Validate username
bool validUsername(const string& username) {

    if (username.length() < 3 || username.length() > 20)
        return false;

    for (unsigned char ch : username) {

        if (!isalnum(ch) && ch != '_')
            return false;
    }

    return true;
}

// Validate password
bool validPassword(const string& password) {

    return password.length() >= 8 &&
           password.length() <= 128;
}

// Generate a secure random salt
bool generateSalt(unsigned char salt[]) {

    return RAND_bytes(salt, SALT_SIZE) == 1;
}

// Generate password hash using PBKDF2
bool generateHash(const string& password,
                  const unsigned char salt[],
                  unsigned char hash[]) {

    return PKCS5_PBKDF2_HMAC(
        password.c_str(),
        static_cast<int>(password.length()),
        salt,
        SALT_SIZE,
        ITERATIONS,
        EVP_sha256(),
        HASH_SIZE,
        hash
    ) == 1;
}

// Check whether username already exists
bool isUsernameTaken(const string& username) {

    ifstream file(FILE_NAME);

    if (!file.is_open())
        return false;

    string line;

    while (getline(file, line)) {

        string storedUser;
        string salt;
        string hash;
        string extra;

        stringstream ss(line);

        if (getline(ss, storedUser, ':') &&
            getline(ss, salt, ':') &&
            getline(ss, hash, ':') &&
            !(ss >> extra)) {

            if (storedUser == username)
                return true;
        }
    }

    return false;
}

// Register a new user
void registerUser() {

    string username;
    string password;
    string confirmPassword;

    cout << "\n==============================================\n";
    cout << "               USER REGISTRATION\n";
    cout << "==============================================\n";

    cout << "Enter Username: ";
    getline(cin, username);

    if (!validUsername(username)) {

        cout << "[Error] Username must contain 3-20 characters.\n";
        cout << "Only letters, numbers and underscores are allowed.\n";

        return;
    }

    if (isUsernameTaken(username)) {

        cout << "[Error] Username already exists!\n";

        return;
    }

    cout << "Enter Password (8-128 characters): ";
    getline(cin, password);

    if (!validPassword(password)) {

        cout << "[Error] Password must contain 8-128 characters.\n";

        return;
    }

    cout << "Confirm Password: ";
    getline(cin, confirmPassword);

    if (password != confirmPassword) {

        cout << "[Error] Passwords do not match!\n";

        return;
    }

    unsigned char salt[SALT_SIZE];
    unsigned char hash[HASH_SIZE];

    if (!generateSalt(salt)) {

        cout << "[Error] Salt generation failed.\n";

        return;
    }

    if (!generateHash(password, salt, hash)) {

        cout << "[Error] Password hashing failed.\n";

        return;
    }

    // Convert salt and hash into hexadecimal
    string saltHex = toHex(salt, SALT_SIZE);
    string hashHex = toHex(hash, HASH_SIZE);

    // Store credentials in file
    ofstream file(FILE_NAME, ios::app);

    if (!file.is_open()) {

        cout << "[Error] Unable to open user database.\n";

        return;
    }

    file << username << ":" << saltHex
         << ":" << hashHex << "\n";

    file.close();

    if (!file) {

        cout << "[Error] Account could not be saved.\n";

        return;
    }

    cout << "\n[Success] Account registered successfully!\n";

    OPENSSL_cleanse(hash, HASH_SIZE);
}

// Login existing user (with MAX_LOGIN_ATTEMPTS enforcement)
void loginUser() {

    int attempts = 0;

    cout << "\n==============================================\n";
    cout << "                  USER LOGIN\n";
    cout << "==============================================\n";

    while (attempts < MAX_LOGIN_ATTEMPTS) {

        string username;
        string password;

        cout << "\nEnter Username: ";
        getline(cin, username);

        cout << "Enter Password: ";
        getline(cin, password);

        ifstream file(FILE_NAME);

        if (!file.is_open()) {

            cout << "[Error] No registered users found.\n";

            return;
        }

        string line;
        bool loginSuccess = false;

        while (getline(file, line)) {

            string storedUser;
            string saltHex;
            string hashHex;
            string extra;

            stringstream ss(line);

            if (!(getline(ss, storedUser, ':') &&
                  getline(ss, saltHex, ':') &&
                  getline(ss, hashHex, ':')) ||
                  (ss >> extra)) {

                continue;
            }

            if (storedUser != username)
                continue;

            unsigned char salt[SALT_SIZE];
            unsigned char storedHash[HASH_SIZE];
            unsigned char enteredHash[HASH_SIZE];

            if (!fromHex(saltHex, salt, SALT_SIZE) ||
                !fromHex(hashHex, storedHash, HASH_SIZE)) {

                cout << "[Error] Invalid account record.\n";

                file.close();
                return;
            }

            // Hash entered password using stored salt
            if (!generateHash(password, salt, enteredHash)) {

                cout << "[Error] Password verification failed.\n";

                OPENSSL_cleanse(storedHash, HASH_SIZE);
                file.close();
                return;
            }

            // Compare hashes safely
            if (CRYPTO_memcmp(
                    storedHash,
                    enteredHash,
                    HASH_SIZE
                ) == 0) {

                loginSuccess = true;
            }

            OPENSSL_cleanse(storedHash, HASH_SIZE);
            OPENSSL_cleanse(enteredHash, HASH_SIZE);

            break;
        }

        file.close();

        if (loginSuccess) {

            cout << "\n[Success] Welcome back, "
                 << username << "!\n";
            return;

        } else {

            attempts++;
            int remaining = MAX_LOGIN_ATTEMPTS - attempts;

            cout << "\n[Error] Invalid username or password.\n";

            if (remaining > 0) {
                cout << "[Warning] Attempts remaining: "
                     << remaining << "\n";
            } else {
                cout << "[Locked] Maximum login attempts ("
                     << MAX_LOGIN_ATTEMPTS
                     << ") exceeded. Returning to Main Menu.\n";
            }
        }
    }
}

// Main menu
int main() {

    int choice;

    cout << "==============================================\n";
    cout << "       CODEALPHA - AUTHENTICATION SYSTEM\n";
    cout << "==============================================\n";

    // Ensure database file exists
    ofstream database(FILE_NAME, ios::app);

    if (!database.is_open()) {

        cout << "[Error] Unable to initialize user database.\n";

        return 1;
    }

    database.close();

    while (true) {

        cout << "\n==============================================\n";
        cout << "                  MAIN MENU\n";
        cout << "==============================================\n";

        cout << "1. Register New Account\n";
        cout << "2. Login to Account\n";
        cout << "3. Exit\n";

        cout << "----------------------------------------------\n";
        cout << "Select an option (1-3): ";

        string input;
        getline(cin, input);

        stringstream ss(input);

        if (!(ss >> choice) || (ss >> ws && !ss.eof()) ||
            choice < 1 || choice > 3) {

            cout << "[Error] Invalid option. Choose 1, 2 or 3.\n";

            continue;
        }

        switch (choice) {

            case 1:
                registerUser();
                break;

            case 2:
                loginUser();
                break;

            case 3:
                cout << "\nExiting system. Goodbye!\n";
                return 0;
        }
    }

    return 0;
}
