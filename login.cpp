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
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    json fridgeContents;
    if (login(username, password, fridgeContents)) {
        cout << "\nLogin success!\ndi dalam kulkas ada:\n";

        for (const auto& item : fridgeContents) {
            cout << "- " << item["nama_barang"] << " ("
                 << item["jumlah_stok"] << ") [" << item["kategori"] << "]\n";
        }
    } else {
        cout << "Login failed. password atau username salah.\n";
    }

    return 0;
}
