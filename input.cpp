#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace std;

const int MAX_ITEMS = 100;

struct Item {
    string kategori;       
    string nama_barang;    
    int jumlah_stok;      
    string tanggal_kadaluarsa; 
};

Item items[MAX_ITEMS];
int item_count = 0;

// Fungsi Kategori input
bool isValidKategori(const string& kategori) {
    return kategori == "makanan" || kategori == "minuman";
}

// Fungsi jumlah stok input
bool isValidJumlahStok(const string& input, int& jumlah) {
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