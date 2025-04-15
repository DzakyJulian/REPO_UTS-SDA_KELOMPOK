#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"


using json = nlohmann::json;
using namespace std;

bool login(const string& username, const string& password, json& fridgeContents) {
    ifstream file("users.json");
    if (!file.is_open()) {
        cerr << "Error opening JSON file.\n";
        return false;
    }

    json users;
    file >> users;

    for (const auto& user : users) {
        if (user["username"] == username && user["password"] == password) {
            fridgeContents = user["fridgeContents"];
            return true;
        }
    }

    return false;
}

int main() {
    string input_usn;
    string input_pw;
    bool login_status = false;
    json fridgeContents;

    cout << "================ WELCOME ================\n";
    do {
        cout << "Insert username : "; cin >> input_usn;
        cout << "Insert password : "; cin >> input_pw;

        if (login(input_usn, input_pw, fridgeContents)) {
            cout << "Login Success" << endl;
            login_status = true;
        } else {
            cout << "Login Failed, Check your username or password!" << endl;
        }

    } while (!login_status);

    return 0;
}
