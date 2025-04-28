#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using json = nlohmann::json;
using namespace std;

// Fungsi untuk membaca password dengan tampilan bintang (cross-platform)
string getHiddenPassword(const char* prompt) {
    cout << prompt;
    string password;

#ifdef _WIN32
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!password.empty()) {
                cout << "\b \b";
                password.pop_back();
            }
        } else {
            password.push_back(ch);
            cout << '*';
        }
    }
#else
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char ch;
    while ((ch = getchar()) != '\n') {
        if (ch == '\b' || ch == 127) {
            if (!password.empty()) {
                cout << "\b \b";
                password.pop_back();
            }
        } else {
            password.push_back(ch);
            cout << '*';
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    cout << endl;
    return password;
}

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
    
    password = getHiddenPassword("Enter password: ");

    return login(username, password, fridgeContents, Id);
}
