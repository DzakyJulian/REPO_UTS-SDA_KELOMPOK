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

// Validasi kategori
bool validKategori(const string& kategori) {
    return kategori == "makanan" || kategori == "minuman";
}

// Validasi jumlah stok
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

// Validasi tanggal kadaluarsa (format dd/mm/yyyy)
bool validTanggal(const string& tanggal) {
    regex pola(R"(^\d{2}/\d{2}/\d{4}$)");
    return regex_match(tanggal, pola);
}

// Fungsi input data barang
void input() {
    char selesai;

    do {
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

        daftar_barang[jumlah_data].kategori = kategori;
        daftar_barang[jumlah_data].nama_barang = nama_barang;
        daftar_barang[jumlah_data].jumlah_stok = jumlah_stok;
        daftar_barang[jumlah_data].tanggal_kadaluarsa = tanggal_kadaluarsa;
        jumlah_data++;

        cout << "Apakah sudah selesai input? (y/n): ";
        cin >> selesai;
        cin.ignore();

    } while (selesai == 'n' || selesai == 'N');

    cout << "\nKembali ke menu utama...\n" << endl;
}

// Fungsi tampilkan isi kulkas
void display() {
    cout << "======== ISI KULKAS ========\n";
    if (jumlah_data == 0) {
        cout << "Kulkas kosong.\n";
    } else {
        cout << "Kategori\tNama Barang\tJumlah Stok\tTanggal Kadaluarsa\n";
        for (int i = 0; i < jumlah_data; i++) {
            cout << daftar_barang[i].kategori << "\t\t"
                 << daftar_barang[i].nama_barang << "\t\t"
                 << daftar_barang[i].jumlah_stok << "\t\t"
                 << daftar_barang[i].tanggal_kadaluarsa << endl;
        }
    }
    cout << "\nKembali ke menu utama...\n" << endl;
}

// Program utama
int main() {
    int choices;

    while (true) {
        cout << "========== SMART FRIDGE ==========\n";
        cout << "1. Display Fridge Contents\n";
        cout << "2. Add Item to Fridge\n";
        cout << "3. Remove Item from Fridge (belum tersedia)\n";
        cout << "4. Exit\n";

        cout << "Masukkan pilihan Anda: ";
        cin >> choices;
        cin.ignore(); 

        if (choices == 1) {
            display();
        } else if (choices == 2) {
            input();
        } else if (choices == 3) {
            cout << "Fitur penghapusan belum tersedia.\n\n";
        } else if (choices == 4) {
            cout << "Terima kasih telah menggunakan Smart Fridge!\n";
            break;  // Keluar dari loop dan program
        } else {
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    }

    return 0;
}
