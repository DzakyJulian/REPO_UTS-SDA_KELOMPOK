#include <iostream>
#include <fstream>
#include <string>
#include <regex>

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
bool validJumlahStok(const string& input, int& jumlah) {
    try {
        size_t pos;
        int val = stoi(input, &pos);
        if (pos != input.length() || val < 0) {
            return false;
        }
        jumlah = val;
        return true;
    } catch (...) {
        return false;
    }
}

// Fungsi untuk format tanggal kadaluarsa dd/mm/yyyy
bool validTanggal(const string& tanggal) {
    regex pola(R"(^\d{2}/\d{2}/\d{4}$)");
    if (!regex_match(tanggal, pola)) {
        return false;
    }
    // Kodingan tambahan untuk range hari dan bulan bisa ditambahkan
    return true;
}
// Fungsi untuk input data dari user
void input() {
    if (jumlah_data >= MAKS_DATA) {
        cout << "Data sudah penuh, tidak bisa menambah lagi." << endl;
        return;
    }

    string kategori;
    do {
        cout << "Masukkan kategori (makanan/minuman): ";
        getline(cin, kategori);
        if (!validKategori(kategori)) {
            cout << "Kategori harus 'makanan' atau 'minuman'. Silakan coba lagi." << endl;
        }
    } while (!validKategori(kategori));

    cout << "Masukkan nama barang: ";
    string nama_barang;
    getline(cin, nama_barang);

    string input_jumlah;
    int jumlah_stok;
    do {
        cout << "Masukkan jumlah stok (angka): ";
        getline(cin, input_jumlah);
        if (!validJumlahStok(input_jumlah, jumlah_stok)) {
            cout << "Jumlah stok harus berupa angka positif. Silakan coba lagi." << endl;
        }
    } while (!validJumlahStok(input_jumlah, jumlah_stok));

    string tanggal_kadaluarsa;
    do {
        cout << "Masukkan tanggal kadaluarsa (dd/mm/yyyy): ";
        getline(cin, tanggal_kadaluarsa);
        if (!validTanggal(tanggal_kadaluarsa)) {
            cout << "Format tanggal salah. Gunakan format dd/mm/yyyy. Silakan coba lagi." << endl;
        }
    } while (!validTanggal(tanggal_kadaluarsa));

    // Simpan data ke array
    daftar_barang[jumlah_data].kategori = kategori;
    daftar_barang[jumlah_data].nama_barang = nama_barang;
    daftar_barang[jumlah_data].jumlah_stok = jumlah_stok;
    daftar_barang[jumlah_data].tanggal_kadaluarsa = tanggal_kadaluarsa;
    jumlah_data++;