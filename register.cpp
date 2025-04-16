#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

bool registerUser(const string& username, const string& password) {
    fstream file("users.json", ios::in);
    json users = json::array();

    if (file.is_open()) {
        file >> users;
        file.close();
    }

    for (const auto& user : users) {
        if (user["username"] == username) {
            cout << "Username sudah terdaftar.\n";
            return false;
        }
    }

    // Buat ID unik dari waktu sekarang
    string userId = "user_" + to_string(time(nullptr));

    users.push_back({
        {"id", userId},
        {"username", username},
        {"password", password},
        {"fridgeContents", json::array()}
    });

    ofstream out("users.json");
    out << setw(4) << users;
    out.close();

    cout << "Berhasil register. Silakan login.\n";
    return true;
}