#include <iostream>
#include <string>
#include "json.hpp"

using namespace std;

const int MAKS_DATA = 100;

struct Barang {
    string kategori;       
    string nama_barang;   
    int jumlah_stok;      
    string tanggal_kadaluarsa; 
};

Barang daftar_barang[MAKS_DATA];
int jumlah_data = 0;

// Fungsi untuk input kategori
bool validKategori(const string& kategori) {
    return kategori == "makanan" || kategori == "minuman";
}

// Fungsi untuk input jumlah stok (integer)
int validJumlahStok(const string& input) {
    return stoi(input);
}

// Fungsi untuk format tanggal kadaluarsa dd/mm/yyyy
bool validTanggal(const string& tanggal) {
    return tanggal.length() == 10;
}

// Fungsi untuk input data dari user
void input() {
    Barang* barang = new Barang[100]; 
    int jumlah_data = 0; 

    while (true) {
        string kategori;
        cout << "Masukkan kategori (makanan/minuman): ";
        getline(cin, kategori);
        if (!validKategori(kategori)) {
            cout << "Kategori harus 'makanan' atau 'minuman'. Silakan coba lagi." << endl;
            continue;
        }

        string nama_barang;
        cout << "Masukkan nama barang: ";
        getline(cin, nama_barang);

        string input_jumlah;
        cout << "Masukkan jumlah stok (angka): ";
        getline(cin, input_jumlah);
        int jumlah_stok = validJumlahStok(input_jumlah);

        string tanggal_kadaluarsa;
        cout << "Masukkan tanggal kadaluarsa (dd/mm/yyyy): ";
        getline(cin, tanggal_kadaluarsa);
        if (!validTanggal(tanggal_kadaluarsa)) {
            cout << "Format tanggal salah. Gunakan format dd/mm/yyyy. Silakan coba lagi." << endl;
            continue;
        }

        // Simpan data ke array dinamis
        barang[jumlah_data].kategori = kategori;
        barang[jumlah_data].nama_barang = nama_barang;
        barang[jumlah_data].jumlah_stok = jumlah_stok;
        barang[jumlah_data].tanggal_kadaluarsa = tanggal_kadaluarsa;
        jumlah_data++;

        cout << "Apakah sudah selesai input? (y/n): ";
        char selesai;
        cin >> selesai;
        cin.ignore();

        if (selesai == 'y' || selesai == 'Y') {
            break;
        }
    }
}

int main() {
    input();
    return 0;
}

    // Simpan ke file JSON
    // ofstream fileJson("users.json");
    // if (!fileJson) {
    //     cout << "Gagal membuka file untuk menyimpan data." << endl;
    //     return;
    // }

    // fileJson << "[" << endl;
    // for (int i = 0; i < jumlah_data; i++) {
    //     fileJson << "  {" << endl;
    //     fileJson << "    \"kategori\": \"" << daftar_barang[i].kategori << "\"," << endl;
    //     fileJson << "    \"nama_barang\": \"" << daftar_barang[i].nama_barang << "\"," << endl;
    //     fileJson << "    \"jumlah_stok\": " << daftar_barang[i].jumlah_stok << "," << endl;
    //     fileJson << "    \"tanggal_kadaluarsa\": \"" << daftar_barang[i].tanggal_kadaluarsa << "\"" << endl;
    //     fileJson << "  }" << (i < jumlah_data - 1 ? "," : "") << endl;
    // }
    // fileJson << "]" << endl;

    // fileJson.close();

    // cout << "Data berhasil disimpan ke file data.json" << endl;
