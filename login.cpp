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
        cerr << "Error: Database pengguna tidak ditemukan.\n";
        return false;
    }

    try {
        json users;
        file >> users;
        
        if (users.empty()) {
            cerr << "Error: Tidak ada pengguna terdaftar.\n";
            return false;
        }

        for (const auto& user : users) {
            if (!user.contains("username") || !user.contains("password")) {
                continue; // Skip invalid user entries
            }

            if (user["username"] == username) {
                if (user["password"] == password) {
                    selectedId = user.value("id", "");
                    fridgeContents = user.value("fridgeContents", json::array());
                    return true;
                } else {
                    cerr << "Error: Password salah.\n";
                    return false;
                }
            }
        }
        
        cerr << "Error: Username tidak ditemukan.\n";
        return false;
    } catch (const json::exception& e) {
        cerr << "Error membaca data: " << e.what() << '\n';
        return false;
    }
}

bool indexLogin(string& Id, json& fridgeContents) {
    string username, password;
    cout << "Enter username: ";
    getline(cin, username);
    
    if (username.empty()) {
        cerr << "Error: Username tidak boleh kosong.\n";
        return false;
    }
    
    password = getHiddenPassword("Enter password: ");
    
    if (password.empty()) {
        cerr << "Error: Password tidak boleh kosong.\n";
        return false;
    }

    return login(username, password, fridgeContents, Id);
}