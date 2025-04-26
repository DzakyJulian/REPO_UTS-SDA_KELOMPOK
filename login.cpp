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

void indexLogin(string& Id, json& fridgeContents) {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin.ignore();  // Bersihkan newline dari buffer
    char ch;
    password = "";
    while ((ch = cin.get()) != '\n') {
        cout << '*';
        password += ch;
    }

    if (login(username, password, fridgeContents, Id)) {
        cout << "\nLogin success!\ndi dalam kulkas ada:\n";
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        // for (const auto& item : fridgeContents) {
        //     cout << "- " << item["nama_barang"] << " ("
        //          << item["jumlah_stok"] << ") [" << item["kategori"] << "]\n";
        // }
    } else {
        cout << "Login failed. password atau username salah.\n";
    }

    // return selectedId;
}