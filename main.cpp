#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp"
#include "picosha2.h" // Include the picosha2 header file
#include <iomanip>
#include <sstream>
#include <cstring>
#include "aes.h"


using namespace std;
using json = nlohmann::json;

string sha256(const string& str) {
    return picosha2::hash256_hex_string(str);
}

class User {
    struct UserDetails {
        int id;
        string name;
        string pass;
        string current_user;
        bool is_loggedin = false;
    };

    UserDetails userDetails;

public:
    User(string name, string pass) {
        userDetails.name = name;
        userDetails.pass = pass;
    }

    User() {}

    void printUser() const {
        cout << "Name: " << userDetails.name << endl;
        cout << "Password: " << userDetails.pass << endl;
    }

    void savejson() {
        const string file_name = "user_data.json";
        ifstream infile(file_name);
        json json_data;

        if (infile.good()) {
            infile >> json_data;
            json_data["Users"].push_back({
                {"name", userDetails.name},
                {"password", sha256(userDetails.pass)} // Store hashed password
            });
        } else {
            json_data = {
                {"Users", {{"name", userDetails.name}, {"password", sha256(userDetails.pass)}}},
                {"is_admin", false},
                {"Passwords", json::array()} // Initialize Passwords array
            };
        }

        ofstream file(file_name);
        if (file.is_open()) {
            file << json_data.dump(4);
            file.close();
            cout << "JSON file saved successfully!" << endl;
        } else {
            cerr << "Could not open the file for writing!" << endl;
        }
    }

    void signup() {
        string name, pass, confirmPass;
        cout << "Enter your name: ";
        cin >> name;
        cout << "Enter your password: ";
        cin >> pass;
        cout << "Confirm your password: ";
        cin >> confirmPass;

        if (pass == confirmPass) {
            userDetails.name = name;
            userDetails.pass = pass;
            savejson();
            cout << "Signup successful! You can now log in." << endl;
        } else {
            cout << "Passwords do not match. Please try again." << endl;
        }
    }

    bool login() {
        string name, pass;
        cout << "Enter your name: ";
        cin >> name;
        cout << "Enter your password: ";
        cin >> pass;

        const string file_name = "user_data.json";
        ifstream infile(file_name);
        json json_data;

        if (!infile.good()) {
            cout << "No user data found. Please sign up first." << endl;
            return false;
        }

        infile >> json_data;
        infile.close();

        for (int i = 0; i < json_data["Users"].size(); ++i) {
            auto user = json_data["Users"][i];

            if (name == user["name"]) {
                string hashedPassword = sha256(pass);
                if (hashedPassword == user["password"]) {
                    cout << "Login successful!" << endl;
                    userDetails.current_user = name;
                    userDetails.pass = pass;  // Store the plain password after login
                    userDetails.is_loggedin = true;
                    return true;
                } else {
                    cout << "Incorrect password." << endl;
                    return false;
                }
            }
        }

        cout << "User not found." << endl;
        return false;
    }

    string getCurrentUser() const {
        return userDetails.current_user;
    }

    bool isLoggedIn() const {
        return userDetails.is_loggedin;
    }

    string getPass() const {
        return userDetails.pass;
    }
};

class Passwds {
protected:
    string users;

public:
    Passwds(string user) {
        users = user;
    }
    
    string byte_array_to_hex_string(const uint8_t* byte_array, size_t length) {
        stringstream ss;
        for (size_t i = 0; i < length; ++i) {
            ss << hex << setw(2) << setfill('0') << static_cast<int>(byte_array[i]);
        }
        return ss.str();
    }

    void hex_string_to_byte_array(const string& hex_string, uint8_t* byte_array, size_t byte_array_size) {
        for (size_t i = 0; i < byte_array_size; ++i) {
            string byte_string = hex_string.substr(i * 2, 2);
            byte_array[i] = static_cast<uint8_t>(strtol(byte_string.c_str(), nullptr, 16));
        }
    }
    
    // AES decryption using Tiny AES C
    string aes_decrypt(const string& encrypted_hex, const string& key) {
        uint8_t aes_key[16] = {0};
        uint8_t input[16] = {0};
        uint8_t output[16] = {0};

        // Convert the key to a 16-byte AES key
        size_t key_len = key.size() > 16 ? 16 : key.size();
        memcpy(aes_key, key.c_str(), key_len);

        // Convert the encrypted hex string back to a byte array
        hex_string_to_byte_array(encrypted_hex, input, sizeof(input));

        // Initialize the AES context and decrypt
        struct AES_ctx ctx;
        AES_init_ctx(&ctx, aes_key);
        AES_ECB_decrypt(&ctx, input);

        // Convert the decrypted byte array to a string
        return string(reinterpret_cast<char*>(input), sizeof(input));
    }


    // AES encryption using Tiny AES C
    string aes_encrypt(const string& plaintext, const string& key) {
        uint8_t aes_key[16] = {0};
        uint8_t input[16] = {0};
        uint8_t output[16] = {0};

        // Copy the key to the AES key array, truncating or padding to 16 bytes
        size_t key_len = key.size() > 16 ? 16 : key.size();
        memcpy(aes_key, key.c_str(), key_len);

        // Copy the plaintext to the input array, truncating or padding to 16 bytes
        size_t text_len = plaintext.size() > 16 ? 16 : plaintext.size();
        memcpy(input, plaintext.c_str(), text_len);

        // Initialize the AES context and encrypt
        struct AES_ctx ctx;
        AES_init_ctx(&ctx, aes_key);
        AES_ECB_encrypt(&ctx, input);

        // Convert encrypted bytes to a hex string for storage
        return byte_array_to_hex_string(input, sizeof(input));
    }

    void save_pass(string app_name, string pass, string user_url, string hashedPass) {
        const string file_name = "user_data.json";
        ifstream infile(file_name);
        json json_data;

        cout << "Saving password..." << endl;

        

        if (infile.good()) {
            infile >> json_data;
        } else {
            json_data = {
                {"Users", json::array()},
                {"is_admin", false},
                {"Passwords", json::array()} // Initialize the Passwords array if the file does not exist
            };
        }

        infile.close();

        string encrypted_pass = aes_encrypt(pass, hashedPass);


        json_data["Passwords"].push_back({
            {"username", users},
            {"passwd", encrypted_pass}, // Optionally hash the password
            {"app", app_name},
            {"url", user_url}
        });

        ofstream file(file_name);
        if (file.is_open()) {
            file << json_data.dump(4);
            file.close();
            cout << "Password saved successfully!" << endl;
        } else {
            cout << "Could not open the file for writing!" << endl;
        }
    }


    void show_pass(string hashedPass) {
        const string file_name = "user_data.json";
        ifstream infile(file_name);
        json json_data;

        if (!infile.good()) {
            cout << "No password data found." << endl;
            return;
        }

        infile >> json_data;
        infile.close();

        for (int i = 0; i < json_data["Passwords"].size(); ++i) {
            auto pass = json_data["Passwords"][i];

            if (users == pass["username"]) {
                string encrypted_pass = pass["passwd"];
                string decrypted_pass = aes_decrypt(encrypted_pass, hashedPass);
                cout << "App: " << pass["app"] << endl;
                cout << "URL: " << pass["url"] << endl;
                cout << "Password: " << decrypted_pass << endl;
            }
        }
    }
};


class singlePasswd: public Passwds{
    public:
    singlePasswd(string user): Passwds(user){
    }
    void show_pass_single(string hashedPass, string app_name) {
        const string file_name = "user_data.json";
        ifstream infile(file_name);
        json json_data;

        if (!infile.good()) {
            cout << "No password data found." << endl;
            return;
        }

        infile >> json_data;
        infile.close();

        bool found = false; // Flag to track if a matching password is found

        for (int i = 0; i < json_data["Passwords"].size(); ++i) {
            auto pass = json_data["Passwords"][i];

            if (users == pass["username"]) {
                if (app_name == pass["app"]) {
                    string encrypted_pass = pass["passwd"];
                    string decrypted_pass = aes_decrypt(encrypted_pass, hashedPass);
                    cout << "App: " << pass["app"] << endl;
                    cout << "URL: " << pass["url"] << endl;
                    cout << "Password: " << decrypted_pass << endl;
                    found = true;
                    break; // Exit the loop since the match is found
                }
            }
        }

        if (!found) {
            cout << "No password data found for the specified app." << endl;
        }
    }


};



int main() {
    int choice;
    User user;

    while (true) {
        cout << "\n1. Signup" << endl;
        cout << "2. Login" << endl;
        cout << "3. Save a password" << endl;
        cout << "4. show all the password" << endl;
        cout << "5. Show password for a specific app" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            user.signup();
        } else if (choice == 2) {
            if (user.login()) {
                cout << "Welcome, " << user.getCurrentUser() << "!" << endl;
            }
        } else if (choice == 3) {
            if (!user.isLoggedIn()) {
                cout << "You must be logged in to save a password." << endl;
                continue;
            }

            string app_name, user_url, password, master_password;

            cout << "Enter your master password: ";
            cin >> master_password;

            // Check if the master password entered matches the logged-in password
            if (master_password == user.getPass()) {
                cout << "Master password is correct." << endl;
                cout << user.getPass() << endl;

                string hashedPass = sha256(user.getPass());
                cout << "Enter the application name: ";
                cin >> app_name;

                cout << "Enter the URL: ";
                cin >> user_url;

                cout << "Enter the password: ";
                cin >> password;

                Passwds passwordManager(user.getCurrentUser());
                passwordManager.save_pass(app_name, password, user_url, hashedPass);
            } else {
                cout << "Master password is incorrect. Please try again." << endl;
            }
        } else if (choice == 4) {
            if (!user.isLoggedIn()) {
                cout << "You must be logged in to view passwords." << endl;
                continue;
            }

            string master_password;
            cout << "Enter your master password: ";
            cin >> master_password;

            if (master_password == user.getPass()) {
                cout << "Master password is correct." << endl;
                cout << user.getPass() << endl;

                string hashedPass = sha256(user.getPass());
                Passwds passwordManager(user.getCurrentUser());
                passwordManager.show_pass(hashedPass);
            } else {
                cout << "Master password is incorrect. Please try again." << endl;
            }
        } 
        else if (choice == 5) {
            if (!user.isLoggedIn()) {
                cout << "You must be logged in to view passwords." << endl;
                continue;
            }

            string master_password;
            cout << "Enter your master password: ";
            cin >> master_password;

            if (master_password == user.getPass()) {
                cout << "Master password is correct." << endl;
                cout << user.getPass() << endl;

                string hashedPass = sha256(user.getPass());
                string app_name;
                cout << "Enter the application name: ";
                cin >> app_name;

                singlePasswd passwordManager(user.getCurrentUser());
                passwordManager.show_pass_single(hashedPass, app_name);
            } else {
                cout << "Master password is incorrect. Please try again." << endl;
            }
        }          
        else if (choice == 6) {
            cout << "Exiting program. Goodbye!" << endl;
            break;
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
