#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"


using json = nlohmann::json;
using namespace std;

bool login(const string& username, const string& password, json& fridgeContents, string& selectedId) {
    ifstream file("users.json");
    if (!file.is_open()) {
        cerr << "Error opening JSON file.\n";
        return false;
    }

    json users;
    file >> users;
 
    for (const auto& user : users) {
        selectedId = user["id"];
        if (user["username"] == username && user["password"] == password) {
            fridgeContents = user["fridgeContents"];
            return true;
        }
    }
     selectedId.clear();
    return false;
}

bool indexLogin(string& Id, json& fridgeContents) {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin.ignore();
    char ch;
    password = "";
    while ((ch = cin.get()) != '\n') {
        cout << '*';
        password += ch;
    }

    return login(username, password, fridgeContents, Id); // Langsung return hasil login
}