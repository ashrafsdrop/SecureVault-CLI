#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp"
#include "picosha2.h" // Include the picosha2 header file
#include <iomanip>
#include <sstream>

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

        // Debug: Print the entire loaded JSON data
        // cout << "Debug: Loaded JSON data:\n" << json_data.dump(4) << endl;

        for (int i = 0; i < json_data["Users"].size(); ++i) {
            // Access the current user
            auto user = json_data["Users"][i];

            // Compare the entered name with the current user's name
            if (name == user["name"]) {
                // Hash the entered password
                string hashedPassword = sha256(pass);
                if (hashedPassword == user["password"]) {
                    cout << "Login successful!" << endl;
                    userDetails.current_user = name;
                    userDetails.is_loggedin = true;
                    return true;
                } else {
                    cout << "Incorrect password." << endl;
                    return false;
                }
            }
        }

        // If no matching user was found
        cout << "User not found." << endl;
        return false;


    }



    string getCurrentUser() const {
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
        cout << "\n1. Signup" << endl;
        cout << "2. Login" << endl;
        cout << "3. Save a password" << endl;
        cout << "4. Exit" << endl;
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
                continue; // Prompt for login first
            }

            string app_name, user_url, password;
            cout << "Enter the application name: ";
            cin >> app_name;

            cout << "Enter the URL: ";
            cin >> user_url;

            cout << "Enter the password: ";
            cin >> password;

            Passwds passwordManager(user.getCurrentUser());
            passwordManager.save_pass(app_name, password, user_url);
        } else if (choice == 4) {
            cout << "Exiting program. Goodbye!" << endl;
            break;
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
