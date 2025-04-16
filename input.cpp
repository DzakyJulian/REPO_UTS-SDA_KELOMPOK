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

// Fungsi untuk input data dari user
void input() {
    Barang* barang = new Barang[100]; 
    int jumlah_data = 0; 

    while (true) {
        string kategori;
        string input_jumlah;
        string nama_barang;
        string tanggal_kadaluarsa;

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
        std::cout << "Apakah sudah selesai? (y/n): ";
        char selesai;
        cin >> selesai;
        cin.ignore();

        if (selesai == 'y' || selesai == 'Y') {
            break;
        }
    }
        // Simpan data ke file JSON
        json jsonData = json::array();
        for (int i = 0; i < jumlah_data; i++) {
                json dataBarang;
                dataBarang["kategori"] = daftar_barang[i].kategori;
                dataBarang["nama_barang"] = daftar_barang[i].nama_barang;
                dataBarang["jumlah_stok"] = daftar_barang[i].jumlah_stok;
                dataBarang["tanggal_kadaluarsa"] = daftar_barang[i].tanggal_kadaluarsa;
                jsonData.push_back(dataBarang);
            }
            ofstream fileJson("users.json");
            if (!fileJson) {
                cout << "Gagal membuka file untuk menyimpan data." << endl;
                return;
            }
            fileJson << jsonData.dump(4);
            fileJson.close();
            cout << "Data berhasil disimpan ke file users.json" << endl;
        }