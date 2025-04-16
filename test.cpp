#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <iomanip>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

struct Barang {
    string kategori;
    string nama_barang;
    int jumlah_stok;
    string tanggal_kadaluarsa;
};

vector<Barang> daftar_barang;
string currentUser;

// Validasi
bool validKategori(const string& kategori) {
    return kategori == "makanan" || kategori == "minuman";
}

bool validJumlahStok(const string& input, int& jumlah) {
    try {
        size_t pos;
        int val = stoi(input, &pos);
        if (pos != input.length() || val < 0) return false;
        jumlah = val;
        return true;
    } catch (...) {
        return false;
    }
}

bool validTanggal(const string& tanggal) {
    regex pola(R"(^\d{2}/\d{2}/\d{4}$)");
    return regex_match(tanggal, pola);
}

// Load & Save
void loadFridgeFromJSON() {
    ifstream file("users.json");
    json users;
    file >> users;

    for (const auto& user : users) {
        if (user["username"] == currentUser) {
            daftar_barang.clear();
            for (const auto& item : user["fridgeContents"]) {
                Barang b;
                b.nama_barang = item["item"];
                b.jumlah_stok = item["quantity"];
                b.kategori = item["type"];
                b.tanggal_kadaluarsa = item["expiry"];
                daftar_barang.push_back(b);
            }
            break;
        }
    }
}

void saveFridgeToJSON() {
    fstream file("users.json", ios::in);
    json users;
    file >> users;
    file.close();

    for (auto& user : users) {
        if (user["username"] == currentUser) {
            user["fridgeContents"] = json::array();
            for (const auto& b : daftar_barang) {
                user["fridgeContents"].push_back({
                    {"item", b.nama_barang},
                    {"quantity", b.jumlah_stok},
                    {"type", b.kategori},
                    {"expiry", b.tanggal_kadaluarsa}
                });
            }
            break;
        }
    }

    ofstream out("users.json");
    out << setw(4) << users;
    out.close();
}

// Input data
void input() {
    char selesai;
    do {
        Barang b;
        string input_jumlah;

        do {
            cout << "Masukkan kategori (makanan/minuman): ";
            getline(cin, b.kategori);
        } while (!validKategori(b.kategori));

        cout << "Masukkan nama barang: ";
        getline(cin, b.nama_barang);

        int jumlah;
        do {
            cout << "Masukkan jumlah stok: ";
            getline(cin, input_jumlah);
        } while (!validJumlahStok(input_jumlah, jumlah));
        b.jumlah_stok = jumlah;

        do {
            cout << "Masukkan tanggal kadaluarsa (dd/mm/yyyy): ";
            getline(cin, b.tanggal_kadaluarsa);
        } while (!validTanggal(b.tanggal_kadaluarsa));

        daftar_barang.push_back(b);

        cout << "Tambah barang lagi? (y/n): ";
        cin >> selesai;
        cin.ignore();
    } while (selesai == 'y' || selesai == 'Y');

    saveFridgeToJSON();
}

// Display
void display() {
    cout << "\n======== ISI KULKAS ========\n";
    if (daftar_barang.empty()) {
        cout << "Kulkas kosong.\n";
    } else {
        cout << "ID\tKategori\tNama\t\tJumlah\tKadaluarsa\n";
        for (size_t i = 0; i < daftar_barang.size(); i++) {
            cout << i + 1 << "\t"
                 << daftar_barang[i].kategori << "\t\t"
                 << daftar_barang[i].nama_barang << "\t\t"
                 << daftar_barang[i].jumlah_stok << "\t"
                 << daftar_barang[i].tanggal_kadaluarsa << "\n";
        }
    }
}

// Hapus barang
void hapusBarang() {
    if (daftar_barang.empty()) {
        cout << "Kulkas kosong.\n";
        return;
    }

    string keyword;
    cout << "Masukkan kata kunci nama barang: ";
    getline(cin, keyword);

    vector<int> foundIndexes;
    cout << "ID\tKategori\tNama\t\tJumlah\tKadaluarsa\n";
    for (size_t i = 0; i < daftar_barang.size(); i++) {
        if (daftar_barang[i].nama_barang.find(keyword) != string::npos) {
            cout << foundIndexes.size() + 1 << "\t"
                 << daftar_barang[i].kategori << "\t\t"
                 << daftar_barang[i].nama_barang << "\t\t"
                 << daftar_barang[i].jumlah_stok << "\t"
                 << daftar_barang[i].tanggal_kadaluarsa << "\n";
            foundIndexes.push_back(i);
        }
    }

    if (foundIndexes.empty()) {
        cout << "Tidak ditemukan.\n";
        return;
    }

    int pilih;
    cout << "Pilih ID untuk dihapus: ";
    cin >> pilih;
    cin.ignore();

    if (pilih < 1 || pilih > (int)foundIndexes.size()) {
        cout << "ID tidak valid.\n";
        return;
    }

    int index = foundIndexes[pilih - 1];
    cout << "Hapus " << daftar_barang[index].nama_barang << "? (y/n): ";
    char konfirmasi;
    cin >> konfirmasi;
    cin.ignore();
    if (konfirmasi == 'y' || konfirmasi == 'Y') {
        daftar_barang.erase(daftar_barang.begin() + index);
        saveFridgeToJSON();
        cout << "Barang dihapus.\n";
    }
}

// Login & Register
bool login(const string& username, const string& password) {
    ifstream file("users.json");
    if (!file.is_open()) return false;

    json users;
    file >> users;

    for (const auto& user : users) {
        if (user["username"] == username && user["password"] == password) {
            currentUser = username;
            loadFridgeFromJSON();
            return true;
        }
    }
    return false;
}

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

    users.push_back({
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

// Main
int main() {
    cout << "===== SMART FRIDGE LOGIN =====\n";
    cout << "1. Login\n2. Register\nPilih: ";
    int pilihan;
    cin >> pilihan;
    cin.ignore();

    string username, password;
    cout << "Username: ";
    getline(cin, username);
    cout << "Password: ";
    getline(cin, password);

    if (pilihan == 1) {
        if (!login(username, password)) {
            cout << "Login gagal.\n";
            return 0;
        }
    } else if (pilihan == 2) {
        if (!registerUser(username, password)) return 0;
        return 0;
    } else {
        cout << "Pilihan tidak valid.\n";
        return 0;
    }

    // Menu utama
    while (true) {
        cout << "\n==== SMART FRIDGE MENU ====\n";
        cout << "1. Tampilkan isi kulkas\n";
        cout << "2. Tambah barang\n";
        cout << "3. Hapus barang\n";
        cout << "4. Keluar\n";
        cout << "Pilih: ";
        int menu;
        cin >> menu;
        cin.ignore();

        switch (menu) {
            case 1: display(); break;
            case 2: input(); break;
            case 3: hapusBarang(); break;
            case 4: cout << "Sampai jumpa!\n"; return 0;
            default: cout << "Pilihan tidak valid.\n";
        }
    }

    return 0;
}
