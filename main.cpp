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

class User {
    struct UserDetails {
        int id;
        string name;
        string pass;
    };

    UserDetails UserDetails;

public:
    User(string name, string pass) {
        UserDetails.name = name;
        UserDetails.pass = pass;
    }

    void printUserDetails() const {
        cout << "Name: " << UserDetails.name << endl;
        cout << "Password: " << UserDetails.pass << endl;
    }

    friend void checkuser();

    void savejson() {
        const string file_name = "user_data.json";
        ifstream infile(file_name);
        if (infile.good()) {
            cout << "File '" << file_name << "' already exists. Skipping write operation." << endl;
            return; // Replace break with return
        }

        // Create a JSON object
                json json_data = {
                  {"Users", {{
                    {"name", UserDetails.name},
                    {"password", UserDetails.pass},
                  }}},
                  {"is_admin", false}
                };

        // Write JSON data to a file
        ofstream file(file_name);
        if (file.is_open()) {
            file << json_data.dump(4); // Serialize with 4-space indentation
            file.close();
            cout << "JSON file saved successfully!" << endl;
        } else {
            cerr << "Could not open the file for writing!" << endl;
        }
    }
};

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

int main(){
    User obj("test", "password");
    obj.printUserDetails();
    obj.savejson();

    string data = "Hello, world!";
    cout << "SHA-256: " << sha256(data) << endl;
    return 0; // Add return statement
}

