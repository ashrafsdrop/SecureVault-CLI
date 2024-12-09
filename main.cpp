#include <iostream>
#include <string>
using namespace std;

class User {
  string username;
  string MasterKey;
public:
  User(string username, string MasterKey) {
    this->username=username;
    this->MasterKey=MasterKey;
  }

  void Display(){
    
    cout << username << endl;
  }
};

int main () {
  User obj("tst", "passwrd");
  obj.Display();
  return 0;
}

