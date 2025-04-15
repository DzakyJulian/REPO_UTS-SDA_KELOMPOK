#include <iostream>
#include <string>
#include <regex>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "json.hpp"
// #include "login.cpp"

using namespace std;
using json = nlohmann::json;

const int MAKS_DATA = 100;

time_t now = time(nullptr);
tm* local_time = localtime(&now);

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

// fungsi untuk mengecek kadaluarsa 
bool isExpired(const string& exp_date) {
    // Dapatkan tanggal sekarang
    time_t now = time(nullptr);
    tm* local_time = localtime(&now);
    int current_day = local_time->tm_mday;
    int current_month = local_time->tm_mon + 1;
    int current_year = local_time->tm_year + 1900;

    // Parse tanggal kadaluarsa
    int day, month, year;
    char sep;
    istringstream iss(exp_date);
    iss >> day >> sep >> month >> sep >> year;

    // Bandingkan tanggal
    if (year < current_year) return true;
    if (year > current_year) return false;
    if (month < current_month) return true;
    if (month > current_month) return false;
    return day < current_day;
}

// Fungsi tampilkan isi kulkas
void display(json& data, string& Id) {
    if (jumlah_data == 0) {
        cout << "Kulkas kosong.\n";
    } else {
        cout << "======== ISI KULKAS ========\n";
        cout << left << setw(5) << "ID" << setw(15) << "Kategori" 
             << setw(20) << "Nama Barang" << setw(15) << "Jumlah Stok"
             << setw(20) << "Tanggal Kadaluarsa" << "Status\n";
        
        // Dapatkan tanggal sekarang
        time_t now = time(nullptr);
        tm* local_time = localtime(&now);
        printf("Tanggal terkini: %02d/%02d/%d\n", 
               local_time->tm_mday, 
               local_time->tm_mon + 1, 
               local_time->tm_year + 1900);
             
        for (int i = 0; i < jumlah_data; i++) {
            string status = isExpired(daftar_barang[i].tanggal_kadaluarsa) 
                          ? "KADALUARSA" : "MASIH BAIK";
            
            cout << left << setw(5) << i + 1
                 << setw(15) << daftar_barang[i].kategori
                 << setw(20) << daftar_barang[i].nama_barang
                 << setw(15) << daftar_barang[i].jumlah_stok
                 << setw(20) << daftar_barang[i].tanggal_kadaluarsa
                 << status << endl;
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

json loadData() {
    ifstream file("users.json");
    
    if (!file.is_open()) {
        cerr << "Error: Failed to open " << "users.json" << endl;
        return {};  // Return empty JSON object
    }

    try {
        return json::parse(file);
    } 
    catch (const json::parse_error& e) {
        cerr << "JSON parse error at byte " << e.byte << ": " << e.what() << endl;
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    return {};
}

void indexLogin(string& Id);
// Program utama
int main() {
    json data = loadData();
    if (data.empty()) {
        cout << "No data loaded or file is empty" << endl;
    }

    int choices;
    string Id;
    while (Id.empty()){
        indexLogin(Id);
    }
    while (true) {
        cout<< Id << endl;
        cout << "========== SMART FRIDGE ==========\n";
        cout << "1. Display Fridge Contents\n";
        cout << "2. Add Item to Fridge\n";
        cout << "3. Remove Item from Fridge\n";
        cout << "4. Exit\n";
        cout << "Masukkan pilihan Anda: ";
        cin >> choices;
        cin.ignore(); // untuk membersihkan buffer newline

        if (choices == 1) {
            display(data,Id);
        } else if (choices == 2) {
            input();
        } else if (choices == 3) {
            hapusBarang();
        } if (choices == 33) {
            indexLogin(Id);
        }else if (choices == 4) {
            cout << "Terima kasih telah menggunakan Smart Fridge!\n";
            break;  // Keluar dari loop dan program
        } else {
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    }

    return 0;
}
