#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

bool registerUser(const string& username, const string& password) {
    // Buka file dengan mode binary untuk menghindari masalah encoding
    ifstream file("users.json", ios::binary);
    json users = json::array();

    if (file.is_open()) {
        try {
            file >> users;
        } catch (const exception& e) {
            cerr << "Error membaca file: " << e.what() << endl;
            return false;
        }
        file.close();
    }

    // Validasi struktur data
    if (!users.is_array()) {
        cerr << "Format data tidak valid" << endl;
        return false;
    }

    // Cek username sudah ada
    for (const auto& user : users) {
        // Pastikan elemen adalah object dan memiliki field username
        if (user.is_object() && user.contains("username")) {
            if (user["username"] == username) {
                cout << "Username sudah terdaftar.\n";
                return false;
            }
        }
    }

    // Buat ID unik
    string userId = "user_" + to_string(time(nullptr));

    // Buat user baru
    json newUser = {
        {"id", userId},
        {"username", username},
        {"password", password},
        {"fridgeContents", json::array()}
    };

    // Tambahkan user baru
    users.push_back(newUser);

    // Simpan ke file
    ofstream out("users.json", ios::binary);
    if (!out) {
        cerr << "Gagal membuka file untuk penyimpanan" << endl;
        return false;
    }

    try {
        out << setw(4) << users;
        out.close();
    } catch (const exception& e) {
        cerr << "Error menyimpan data: " << e.what() << endl;
        return false;
    }

    cout << "Berhasil register. Silakan login.\n";
    return true;
}