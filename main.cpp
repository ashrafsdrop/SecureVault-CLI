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

string sha256(const string str){
  unsigned char hash[SHA256_DIGEST_LENGTH];

  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, str.c_str(), str.size());
  SHA256_Final(hash, &sha256);

  stringstream ss;

  for(int i = 0; i < SHA256_DIGEST_LENGTH; i++){
    ss << hex << setw(2) << setfill('0') << static_cast<int>( hash[i] );
  }
  return ss.str();
}

class User {
    struct UserDetails {
        int id;
        string name;
        string pass;
    };

    UserDetails userDetails;

public:
    User(string name, string pass) {
        userDetails.name = name;
        userDetails.pass = pass;
    }

    User() {}

    void printUser () const {
        cout << "Name: " << userDetails.name << endl;
        cout << "Password: " << userDetails.pass << endl;
    }

    friend void checkuser();

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
                {"is_admin", false}
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
};

int main(){
    int choice;
    cout << "1. Signup" << endl;
    cout << "2. Login" << endl;
    cout << "Enter your choice: ";
    cin >> choice;

    if (choice == 1) {
        User user;
        user.signup();
    } else if (choice == 2) {
        // Implement login functionality here
    } else {
        cout << "Invalid choice. Please try again." << endl;
    }

    return 0;
}
