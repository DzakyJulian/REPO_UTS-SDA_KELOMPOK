#include <iostream>
#include <string>
#include <regex>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

const int MAKS_DATA = 100;

time_t now = time(nullptr);
tm *local_time = localtime(&now);

struct Barang {
    string kategori;
    string nama_barang;
    int jumlah_stok;
    string tanggal_kadaluarsa;
};

struct Node {
    Barang data;
    Node *next;
};

// Validasi kategori
bool validKategori(const string &kategori) {
    return kategori == "makanan" || kategori == "minuman";
}

// Validasi jumlah stok (fixed implementation)
int validJumlahStok(const string &input) {
    try {
        size_t pos;
        int val = stoi(input, &pos);
        if (pos != input.length() || val < 0) {
            return -1;
        }
        return val;
    } catch (...) {
        return -1;
    }
}

// Validasi tanggal kadaluarsa (format dd/mm/yyyy)
bool validTanggal(const string &tanggal) {
    regex pola(R"(^\d{2}/\d{2}/\d{4}$)");
    if (!regex_match(tanggal, pola))
        return false;

    int day = stoi(tanggal.substr(0, 2));
    int month = stoi(tanggal.substr(3, 2));
    int year = stoi(tanggal.substr(6, 4));

    if (month < 1 || month > 12)
        return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30,
                        31, 31, 30, 31, 30, 31};

    bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (isLeap && month == 2)
        daysInMonth[1] = 29;

    if (day < 1 || day > daysInMonth[month - 1])
        return false;

    if (year < 2025)
        return false;

    return true;
}

int findUserIndex(const json &data, const string &userId) {
    for (int i = 0; i < data.size(); ++i) {
        if (data[i]["id"] == userId) {
            return i;
        }
    }
    return -1; // Not found
}

// Fungsi input data barang
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
    
    try {
        fileIn >> data;
    } catch (const json::parse_error& e) {
        cout << "Error parsing JSON: " << e.what() << endl;
        fileIn.close();
        return;
    }
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
        if (jumlah_data < MAKS_DATA) {
            barang[jumlah_data++] = {kategori, nama_barang, jumlah_stok, tanggal_kadaluarsa};
        } else {
            cout << "Maaf, kapasitas penyimpanan penuh.\n";
            break;
        }

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

// fungsi untuk mengecek kadaluarsa
bool isExpired(const string &exp_date) {
    // Dapatkan tanggal sekarang
    time_t now = time(nullptr);
    tm *local_time = localtime(&now);
    int current_day = local_time->tm_mday;
    int current_month = local_time->tm_mon + 1;
    int current_year = local_time->tm_year + 1900;

    // Parse tanggal kadaluarsa
    int day, month, year;
    char sep;
    istringstream iss(exp_date);
    iss >> day >> sep >> month >> sep >> year;

    // Bandingkan tanggal
    if (year < current_year)
        return true;
    if (year > current_year)
        return false;
    if (month < current_month)
        return true;
    if (month > current_month)
        return false;
    return day < current_day;
}

void parseFridgeToList(const json &fridgeJson, Node *&head) {
    head = nullptr;
    Node *tail = nullptr;

    for (const auto &item : fridgeJson) {
        try {
            Node *newNode = new Node;
            newNode->data.nama_barang = item["nama_barang"].get<string>();
            newNode->data.jumlah_stok = item["jumlah_stok"].get<int>();
            newNode->data.kategori = item["kategori"].get<string>();
            newNode->data.tanggal_kadaluarsa = item["tanggal_kadaluarsa"].get<string>();
            newNode->next = nullptr;

            if (!head) {
                head = newNode;
                tail = head;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
        } catch (const json::exception& e) {
            cerr << "Error parsing fridge item: " << e.what() << endl;
        }
    }
}

// Fungsi tampilkan isi kulkas
void display(Node *head) {
    if (!head) {
        cout << "Kulkas kosong.\n";
        return;
    }

    cout << "======== ISI KULKAS ========\n";
    cout << left << setw(5) << "ID" << setw(15) << "Kategori"
         << setw(20) << "Nama Barang" << setw(15) << "Jumlah Stok"
         << setw(20) << "Tanggal Kadaluarsa" << "Status\n";

    int i = 1;
    for (Node *curr = head; curr != nullptr; curr = curr->next) {
        string status = isExpired(curr->data.tanggal_kadaluarsa)
                            ? "KADALUARSA"
                            : "MASIH BAIK";

        cout << left << setw(5) << i++
             << setw(15) << curr->data.kategori
             << setw(20) << curr->data.nama_barang
             << setw(15) << curr->data.jumlah_stok
             << setw(20) << curr->data.tanggal_kadaluarsa
             << status << endl;
    }
}

// Fungsi untuk menghapus barang dari kulkas
void hapusBarang(json &data, const string &Id) {
    int userIndex = findUserIndex(data, Id);
    if (userIndex == -1) {
        cout << "User tidak ditemukan.\n";
        return;
    }

    json &fridge = data[userIndex]["fridgeContents"];
    if (fridge.empty()) {
        cout << "Tidak ada barang yang dapat dihapus.\n";
        return;
    }

    string keyword;
    cout << "Masukkan kata kunci untuk mencari barang: ";
    getline(cin, keyword);

    vector<int> found_indices;
    cout << "Barang yang ditemukan:\n";
    cout << "ID\tKategori\tNama Barang\tJumlah Stok\tTanggal Kadaluarsa\n";

    for (int i = 0; i < fridge.size(); i++) {
        try {
            if (fridge[i]["nama_barang"].get<string>().find(keyword) != string::npos) {
                found_indices.push_back(i);
                cout << found_indices.size() << "\t"
                     << fridge[i]["kategori"] << "\t\t"
                     << fridge[i]["nama_barang"] << "\t\t"
                     << fridge[i]["jumlah_stok"] << "\t\t"
                     << fridge[i]["tanggal_kadaluarsa"] << "\n";
            }
        } catch (const json::exception& e) {
            cerr << "Error accessing fridge item: " << e.what() << endl;
        }
    }

    if (found_indices.empty()) {
        cout << "Tidak ada barang yang cocok.\n";
        return;
    }

    int id;
    cout << "Pilih ID barang yang ingin dihapus (1 - " << found_indices.size() << "): ";
    cin >> id;
    cin.ignore();

    if (id < 1 || id > found_indices.size()) {
        cout << "ID tidak valid.\n";
        return;
    }

    int index_terhapus = found_indices[id - 1];

    cout << "\nAnda akan menghapus barang berikut:\n";
    cout << "Kategori: " << fridge[index_terhapus]["kategori"] << endl;
    cout << "Nama Barang: " << fridge[index_terhapus]["nama_barang"] << endl;
    cout << "Jumlah Stok: " << fridge[index_terhapus]["jumlah_stok"] << endl;
    cout << "Tanggal Kadaluarsa: " << fridge[index_terhapus]["tanggal_kadaluarsa"] << endl;

    char konfirmasi;
    cout << "Apakah Anda yakin ingin menghapus barang ini? (y/n): ";
    cin >> konfirmasi;
    cin.ignore();

    if (konfirmasi == 'y' || konfirmasi == 'Y') {
        fridge.erase(fridge.begin() + index_terhapus);

        ofstream file("users.json");
        if (file.is_open()) {
            file << setw(4) << data << endl;
            file.close();
            cout << "Barang berhasil dihapus dan data disimpan ke file.\n";
        } else {
            cout << "Gagal menulis ke file JSON.\n";
        }
    } else {
        cout << "Barang tidak jadi dihapus.\n";
    }

    cout << "\nKembali ke menu utama...\n" << endl;
}

json loadData() {
    ifstream file("users.json");

    if (!file.is_open()) {
        cerr << "Error: Failed to open users.json" << endl;
        return {};
    }

    try {
        return json::parse(file);
    } catch (const json::parse_error &e) {
        cerr << "JSON parse error at byte " << e.byte << ": " << e.what() << endl;
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
    }

    return {};
}

void clearFridgeList(Node *&head) {
    while (head != nullptr) {
        Node *temp = head;
        head = head->next;
        delete temp;
    }
}

void updateStok(json &data, const string &Id) {
    int userIndex = findUserIndex(data, Id);
    if (userIndex == -1) {
        cout << "User tidak ditemukan.\n";
        return;
    }

    json &fridge = data[userIndex]["fridgeContents"];
    if (fridge.empty()) {
        cout << "Kulkas kosong.\n";
        return;
    }

    int kategoriPilihan;
    do {
        cout << "\nPilih kategori barang yang ingin diupdate:\n";
        cout << "1. Makanan\n";
        cout << "2. Minuman\n";
        cout << "3. Kembali\n";
        cout << "Pilihan: ";
        cin >> kategoriPilihan;
        cin.ignore();

        if (kategoriPilihan == 3) {
            cout << "Kembali ke menu utama...\n";
            return;
        }

        string kategoriDipilih = (kategoriPilihan == 1) ? "makanan" : (kategoriPilihan == 2) ? "minuman" : "";

        if (kategoriDipilih.empty()) {
            cout << "Pilihan tidak valid.\n";
            continue;
        }

        // Filter dan tampilkan list barang sesuai kategori
        vector<int> filtered_indices;
        cout << "\nBarang dalam kategori '" << kategoriDipilih << "':\n";
        cout << "ID\tNama Barang\tJumlah Stok\n";

        for (int i = 0; i < fridge.size(); ++i) {
            try {
                if (fridge[i]["kategori"] == kategoriDipilih) {
                    filtered_indices.push_back(i);
                    cout << filtered_indices.size() << "\t"
                         << fridge[i]["nama_barang"] << "\t\t"
                         << fridge[i]["jumlah_stok"] << endl;
                }
            } catch (const json::exception& e) {
                cerr << "Error accessing fridge item: " << e.what() << endl;
            }
        }

        if (filtered_indices.empty()) {
            cout << "Tidak ada barang dalam kategori tersebut.\n";
            return;
        }

        int pilihan;
        cout << "Pilih ID barang yang ingin diupdate: ";
        cin >> pilihan;
        cin.ignore();

        if (pilihan < 1 || pilihan > filtered_indices.size()) {
            cout << "ID tidak valid.\n";
            return;
        }

        int index_update = filtered_indices[pilihan - 1];
        int stok_baru;
        cout << "Masukkan stok baru: ";
        cin >> stok_baru;
        cin.ignore();

        fridge[index_update]["jumlah_stok"] = stok_baru;

        // Simpan ke file
        ofstream file("users.json");
        if (file.is_open()) {
            file << setw(4) << data;
            file.close();
            cout << "Stok berhasil diperbarui dan disimpan ke file.\n";
        } else {
            cerr << "Gagal menyimpan ke file.\n";
        }

        break; // selesai update

    } while (true);
}

//fungsi mencari barang berdasarkan nama
void cari(Node *head) {
    if (!head) {
        cout << "Kulkas kosong, tidak ada barang untuk dicari.\n";
        return;
    }
    string keyword;
    cout << "Masukkan kata kunci untuk mencari barang: ";
    getline(cin, keyword);

    string lowerKeyword = keyword;
    transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);

    cout << "======== HASIL PENCARIAN ========\n";
    cout << left << setw(5) << "ID" << setw(15) << "Kategori"
              << setw(20) << "Nama Barang" << setw(15) << "Jumlah Stok"
              << setw(20) << "Tanggal Kadaluarsa" << "Status\n";

    int i = 1;
    bool found = false;
    for (Node *curr = head; curr != nullptr; curr = curr->next) {
        string lowerNamaBarang = curr->data.nama_barang;
        transform(lowerNamaBarang.begin(), lowerNamaBarang.end(), lowerNamaBarang.begin(), ::tolower);
        if (lowerNamaBarang.find(lowerKeyword) != string::npos) {
            string status = isExpired(curr->data.tanggal_kadaluarsa)
                                   ? "KADALUARSA"
                                   : "MASIH BAIK";

            cout << left << setw(5) << i++
                 << setw(15) << curr->data.kategori
                 << setw(20) << curr->data.nama_barang
                 << setw(15) << curr->data.jumlah_stok
                 << setw(20) << curr->data.tanggal_kadaluarsa
                 << status << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "Tidak ada barang dengan kata kunci '" << keyword << "' ditemukan.\n";
    }
}