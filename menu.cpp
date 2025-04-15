#include <iostream>
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
        cout << "ID\tKategori\tNama Barang\tJumlah Stok\tTanggal Kadaluarsa\n";
        for (int i = 0; i < jumlah_data; i++) {
            cout << i + 1 << "\t"  // ID dimulai dari 1
                 << daftar_barang[i].kategori << "\t\t"
                 << daftar_barang[i].nama_barang << "\t\t"
                 << daftar_barang[i].jumlah_stok << "\t\t"
                 << daftar_barang[i].tanggal_kadaluarsa << endl;
        }
    }
    cout << "\nKembali ke menu utama...\n" << endl;
}

// Fungsi untuk mencari barang berdasarkan nama barang
void hapusBarang() {
    if (jumlah_data == 0) {
        cout << "Tidak ada barang yang dapat dihapus.\n";
        return;
    }

    string keyword;
    cout << "Masukkan kata kunci untuk mencari barang: ";
    getline(cin, keyword);

    int found_count = 0;

    // Tampilkan barang yang sesuai dengan kata kunci
    cout << "Barang yang ditemukan:\n";
    cout << "ID\tKategori\tNama Barang\tJumlah Stok\tTanggal Kadaluarsa\n";
    for (int i = 0; i < jumlah_data; i++) {
        if (daftar_barang[i].nama_barang.find(keyword) != string::npos) {
            cout << found_count + 1 << "\t"
                 << daftar_barang[i].kategori << "\t\t"
                 << daftar_barang[i].nama_barang << "\t\t"
                 << daftar_barang[i].jumlah_stok << "\t\t"
                 << daftar_barang[i].tanggal_kadaluarsa << endl;
            found_count++;
        }
    }

    if (found_count == 0) {
        cout << "Tidak ada barang yang cocok dengan kata kunci '" << keyword << "'\n";
        return;
    }

    int id;
    cout << "Pilih ID barang yang ingin dihapus (1 - " << found_count << "): ";
    cin >> id;
    cin.ignore();  // Membersihkan newline

    if (id < 1 || id > found_count) {
        cout << "ID tidak valid.\n";
        return;
    }

    // Barang yang dipilih untuk dihapus
    int index_terhapus = -1;
    found_count = 0;
    for (int i = 0; i < jumlah_data; i++) {
        if (daftar_barang[i].nama_barang.find(keyword) != string::npos) {
            found_count++;
            if (found_count == id) {
                index_terhapus = i;
                break;
            }
        }
    }

    // Tampilkan konfirmasi penghapusan
    cout << "\nAnda akan menghapus barang berikut:\n";
    cout << "Kategori: " << daftar_barang[index_terhapus].kategori << endl;
    cout << "Nama Barang: " << daftar_barang[index_terhapus].nama_barang << endl;
    cout << "Jumlah Stok: " << daftar_barang[index_terhapus].jumlah_stok << endl;
    cout << "Tanggal Kadaluarsa: " << daftar_barang[index_terhapus].tanggal_kadaluarsa << endl;

    char konfirmasi;
    cout << "Apakah Anda yakin ingin menghapus barang ini? (y/n): ";
    cin >> konfirmasi;
    cin.ignore();

    if (konfirmasi == 'y' || konfirmasi == 'Y') {
        // Hapus barang dengan menimpa barang tersebut dengan barang terakhir
        daftar_barang[index_terhapus] = daftar_barang[jumlah_data - 1];
        jumlah_data--;  // Kurangi jumlah data
        cout << "Barang berhasil dihapus.\n";
    } else {
        cout << "Barang tidak jadi dihapus.\n";
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
        cout << "3. Remove Item from Fridge\n";
        cout << "4. Exit\n";
        cout << "Masukkan pilihan Anda: ";
        cin >> choices;
        cin.ignore(); // untuk membersihkan buffer newline

        if (choices == 1) {
            display();
        } else if (choices == 2) {
            input();
        } else if (choices == 3) {
            hapusBarang();
        } else if (choices == 4) {
            cout << "Terima kasih telah menggunakan Smart Fridge!\n";
            break;  // Keluar dari loop dan program
        } else {
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    }

    return 0;
}
