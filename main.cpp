#include<IOstream>
#include <fstream>
using namespace std;


class User{

    struct UserDetails {
        int id;
        string name;
        string pass;
    };

    UserDetails UserDetails;

public:
    User(string name, string pass){
        UserDetails.name = name;
        UserDetails.pass = pass;

    }

    void printUserDetails() const {
        cout << "Name: " << UserDetails.name << endl;
        cout << "Password: " << UserDetails.pass << endl;
    }

    friend void checkuser();

    void savejson(){
        string json_data =
        R"(
{
"name": "John Doe",
"age": 30,
"is_admin": true,
"address": {
    "street": "123 Main St",
    "city": "New York",
    "zip": "10001"
    }
}
        )";


        ofstream file("user_data.json");
        if (file.is_open()) {
        // Write JSON string to the file
        file << json_data;

        // Close the file
        file.close();

            cout << "JSON file saved successfully!" << endl;
        } else {
            cerr << "Could not open the file for writing!" << endl;
        }


        cout << json_data << endl;

    }

};


int main(){
    User obj("test", "password");
    obj.printUserDetails();
    obj.savejson();
}



