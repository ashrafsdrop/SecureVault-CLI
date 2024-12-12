#define OPENSSL_API_COMPAT 0x10100000L
#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp"
#include "sha.h"
#include <iomanip>
#include <sstream>

using namespace std;
using json = nlohmann::json;

string sha256(const string str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);

    stringstream ss;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
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

    void printUser  () const {
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
                {"password", sha256(userDetails.pass)},
            });
        } else {
            json_data = {
                {"Users", {{
                    {"name", userDetails.name},
                    {"password", sha256(userDetails.pass)},
                }}},
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
            User user(name, pass);
            user.savejson();
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
        infile >> json_data;
        infile.close();

        for (int i = 0; i < json_data["Users"].size(); i++) {
            if (name == json_data["Users"][i]["name"] && sha256(pass) == json_data["Users"][i]["password"]) {
                cout << "Login successful!" << endl;
                userDetails.current_user = name;
                userDetails.is_loggedin = true;
                return true; // Successful login
            }
        }
        cout << "Invalid username or password." << endl;
        return false; // Failed login
    }

    string getCurrentUser  () const {
        return userDetails.current_user;
    }

    bool isLoggedIn() const {
        return userDetails.is_loggedin;
    }
};

class Passwds {
    string users;

public:
    Passwds(string user) {
        users = user;
    }

    void save_pass(string app_name, string pass, string user_url) {
        const string file_name = "user_data.json";
        ifstream infile(file_name);
        json json_data;

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
        json_data["Passwords"].push_back({
            {"username", users},
            {"passwd", pass}, // Optionally hash the password
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
};

int main() {
    int choice;
    User user;

    while (true) {
        cout << "1. Signup" << endl;
        cout << "2. Login" << endl;
        cout << "3. Save a password" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            user.signup();
        } else if (choice == 2) {
            user.login();
        } else if (choice == 3) {
            if (!user.isLoggedIn()) {
                cout << "You must be logged in to save a password." << endl;
                continue; // Prompt for login first
            }

            string app_name, user_url, password;
            cout << "Enter the application name: ";
            cin >> app_name;

            cout << "Enter the URL: ";
            cin >> user_url;

            cout << "Enter the password: ";
            cin >> password;

            Passwds passwordManager(user.getCurrentUser ());
            passwordManager.save_pass(app_name, password, user_url);
        } else if (choice == 4) {
            break; // Exit the loop
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
