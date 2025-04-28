#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

const int MAKS_DATA = 100;

struct Barang {
    string kategori;
    string nama_barang;
    int jumlah_stok;
    string tanggal_kadaluarsa;
};

// Fungsi untuk validasi kategori
bool validKategori(const string& kategori) {
    return kategori == "makanan" || kategori == "minuman";
}

// Fungsi untuk validasi input jumlah stok (harus integer positif)
int validJumlahStok(const string& input) {
    try {
        int stok = stoi(input);
        return stok > 0 ? stok : -1;
    } catch (const exception&) {
        return -1;
    }
}

// Fungsi untuk validasi format tanggal dd/mm/yyyy
bool validTanggal(const string& tanggal) {
    if (tanggal.length() != 10) return false;
    if (tanggal[2] != '/' || tanggal[5] != '/') return false;

    int day, month, year;
    try {
        day = stoi(tanggal.substr(0, 2));
        month = stoi(tanggal.substr(3, 2));
        year = stoi(tanggal.substr(6, 4));
    } catch (...) {
        return false;
    }

    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    if (year < 1900 || year > 2100) return false;

    // Validasi hari per bulan
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
    if (month == 2) {
        bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        if (day > (leap ? 29 : 28)) return false;
    }

    return true;
}

// Fungsi untuk input data dari user
void input(json& data, const string& userId) {
    if (userId.empty()) {
        cout << "Error: User ID tidak valid. Silakan login terlebih dahulu.\n";
        return;
    }

    // Baca users.json
    ifstream fileIn("users.json");
    if (!fileIn.is_open()) {
        cout << "Error: Tidak dapat membaca file users.json\n";
        return;
    }
    fileIn >> data;
    fileIn.close();

    Barang barang[MAKS_DATA];
    int jumlah_data = 0;

    while (true) {
        cout << "\n=== TAMBAH BARANG KE KULKAS ===\n";

        string kategori, nama_barang, tanggal_kadaluarsa, input_jumlah;
        int jumlah_stok;

        // Input kategori
        cout << "Masukkan kategori (makanan/minuman): ";
        getline(cin, kategori);
        if (!validKategori(kategori)) {
            cout << "Kategori harus 'makanan' atau 'minuman'.\n";
            continue;
        }

        // Input nama barang
        cout << "Masukkan nama barang: ";
        getline(cin, nama_barang);
        if (nama_barang.empty()) {
            cout << "Nama barang tidak boleh kosong.\n";
            continue;
        }

        // Input jumlah stok
        cout << "Masukkan jumlah stok: ";
        getline(cin, input_jumlah);
        jumlah_stok = validJumlahStok(input_jumlah);
        if (jumlah_stok == -1) {
            cout << "Jumlah stok harus berupa angka positif.\n";
            continue;
        }

        // Input tanggal kadaluarsa
        cout << "Masukkan tanggal kadaluarsa (dd/mm/yyyy): ";
        getline(cin, tanggal_kadaluarsa);
        if (!validTanggal(tanggal_kadaluarsa)) {
            cout << "Format tanggal tidak valid.\n";
            continue;
        }

        // Simpan ke array
        barang[jumlah_data++] = {kategori, nama_barang, jumlah_stok, tanggal_kadaluarsa};

        // Tampilkan sementara
        cout << "\nData sementara:\n";
        for (int i = 0; i < jumlah_data; i++) {
            cout << "- " << barang[i].nama_barang << " (" << barang[i].kategori
                 << "), stok: " << barang[i].jumlah_stok
                 << ", exp: " << barang[i].tanggal_kadaluarsa << endl;
        }

        cout << "\nTambah barang lagi?\n1. Iya\n2. Tidak\nPilihan: ";
        string pilihan;
        getline(cin, pilihan);

        if (pilihan != "1") {
            break;
        }
    }

    // Update ke user yang sesuai
    bool userFound = false;

    for (auto& user : data) {
        if (user["id"] == userId) {
            userFound = true;

            if (!user.contains("fridgeContents") || !user["fridgeContents"].is_array()) {
                user["fridgeContents"] = json::array();
            }

            for (int i = 0; i < jumlah_data; i++) {
                bool itemFound = false;
                for (auto& item : user["fridgeContents"]) {
                    if (item["kategori"] == barang[i].kategori &&
                        item["nama_barang"] == barang[i].nama_barang &&
                        item["tanggal_kadaluarsa"] == barang[i].tanggal_kadaluarsa) {
                        item["jumlah_stok"] = item["jumlah_stok"].get<int>() + barang[i].jumlah_stok;
                        itemFound = true;
                        break;
                    }
                }
                if (!itemFound) {
                    user["fridgeContents"].push_back({
                        {"kategori", barang[i].kategori},
                        {"nama_barang", barang[i].nama_barang},
                        {"jumlah_stok", barang[i].jumlah_stok},
                        {"tanggal_kadaluarsa", barang[i].tanggal_kadaluarsa}
                    });
                }
            }
            break;
        }
    }

    if (!userFound) {
        cout << "Error: User tidak ditemukan.\n";
        return;
    }

    // Tulis ulang ke file
    ofstream fileOut("users.json");
    if (!fileOut.is_open()) {
        cout << "Error: Tidak bisa membuka file untuk menulis.\n";
        return;
    }
    fileOut << setw(4) << data << endl;
    fileOut.close();

    cout << "\n✅ Data berhasil disimpan ke users.json!\n";
}
