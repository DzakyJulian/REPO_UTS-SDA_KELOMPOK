#include <iostream>
#include <string>
#include <fstream>
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

void saveToJSON(json &data, const string &kategori, const string &nama_barang, int jumlah_stok, const string &tanggal_kadaluarsa) {
    // Cari apakah data yang sama sudah ada di JSON
    bool found = false;
    for (auto &item : data) {
        if (item["kategori"] == kategori &&
            item["nama_barang"] == nama_barang &&
            item["tanggal_kadaluarsa"] == tanggal_kadaluarsa) {
            // Jika data yang sama sudah ada, perbarui jumlah stoknya
            item["jumlah_stok"] = item["jumlah_stok"].get<int>() + jumlah_stok;
            found = true;
            break;
        }
    }

    if (!found) {
        // Simpan data baru ke JSON
        json newItem;
        newItem["kategori"] = kategori;
        newItem["nama_barang"] = nama_barang;
        newItem["jumlah_stok"] = jumlah_stok;
        newItem["tanggal_kadaluarsa"] = tanggal_kadaluarsa;
        data.push_back(newItem);
    }
}

// Fungsi untuk input data dari user
void input() {
    Barang* barang = new Barang[100]; 
    int jumlah_data = 0; 
    string kategori;
    string input_jumlah;
    string nama_barang;
    string tanggal_kadaluarsa;
    int jumlah_stok;

    while (true) {    
        std::cout << "Masukkan kategori (makanan/minuman): ";
        getline(cin, kategori);
        if (!validKategori(kategori)) {
            std::cout << "Kategori harus 'makanan' atau 'minuman'. Silakan coba lagi." << std::endl;
            continue;
        }

        std::cout << "Masukkan nama barang: ";
        getline(cin, nama_barang);

        std::cout << "Masukkan jumlah stok (angka): ";
        getline(cin, input_jumlah);
        int jumlah_stok = validJumlahStok(input_jumlah);

        std::cout << "Masukkan tanggal kadaluarsa (dd/mm/yyyy): ";
        getline(cin, tanggal_kadaluarsa);
        if (!validTanggal(tanggal_kadaluarsa)) {
            std::cout << "Format tanggal salah. Gunakan format dd/mm/yyyy. Silakan coba lagi." << endl;
            continue;
        }

        // Simpan data ke array dinamis
        barang[jumlah_data].kategori = kategori;
        barang[jumlah_data].nama_barang = nama_barang;
        barang[jumlah_data].jumlah_stok = jumlah_stok;
        barang[jumlah_data].tanggal_kadaluarsa = tanggal_kadaluarsa;
        jumlah_data++;

        // Tampilkan data yang telah diisi
        std::cout << "Data yang telah diisi:" << std::endl << std::endl;
        for (int i = 0; i < jumlah_data; i++) {
            std::cout << "Kategori: " << barang[i].kategori << std::endl;
            std::cout << "Nama Barang: " << barang[i].nama_barang << std::endl;
            std::cout << "Jumlah Stok: " << barang[i].jumlah_stok << std::endl;
            std::cout << "Tanggal Kadaluarsa: " << barang[i].tanggal_kadaluarsa << std::endl;
            std::cout << std::endl;
        }

        // Konfirmasi apakah sudah selesai atau belum
        std::cout << "Konfirmasi Data Barang" << std::endl;
        std::cout << "1. Iya" << std::endl;
        std::cout << "2. Tidak" << std::endl;
        std::cout << "Apakah sudah selesai?: ";
        int selesai;
        cin >> selesai;
        cin.ignore();

        if (selesai == 1){
            break;
        }
    }
    json data;

    // Coba baca file JSON
    ifstream file("users.json");
    if (file.is_open()) {
        // Jika file tidak kosong, baca dan parse isi file
        file >> data;
        file.close();
    } else {
        // Jika file kosong, buat data JSON kosong
        data = json::array();
    }

    // Simpan data ke file JSON dengan menggunakan dump()
    ofstream fileOut("users.json");
    fileOut << std::setw(4) << data << std::endl;
}

int main()
{
    input();
    return 0;
}