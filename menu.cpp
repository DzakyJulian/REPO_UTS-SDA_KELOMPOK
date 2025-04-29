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

void clearTerminal() {
    this_thread::sleep_for(chrono::seconds(1));
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    // ANSI escape codes for additional clearing
    cout << "\033[2J\033[1;1H";
}

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
         << setw(20) << "Tanggal Kadaluwarsa" << "Status\n";

    int i = 1;
    for (Node *curr = head; curr != nullptr; curr = curr->next) {
        string status = isExpired(curr->data.tanggal_kadaluarsa)
                            ? "KADALUWARSA"
                            : "MASIH BAIK";

        cout << left << setw(5) << i++
             << setw(15) << curr->data.kategori
             << setw(20) << curr->data.nama_barang
             << setw(15) << curr->data.jumlah_stok
             << setw(20) << curr->data.tanggal_kadaluarsa
             << status << endl;
    }
}

void hapusBarang(json &data, const string &Id)
{
    int userIndex = findUserIndex(data, Id);
    if (userIndex == -1)
    {
        cout << "User tidak ditemukan.\n";
        return;
    }

    json &fridge = data[userIndex]["fridgeContents"];
    if (fridge.empty())
    {
        cout << "Tidak ada barang yang dapat dihapus.\n";
        return;
    }

    string keyword;
    do {
        cout << "Masukkan kata kunci untuk mencari barang (atau ketik 'exit' untuk kembali): ";
        getline(cin, keyword);

        if (keyword == "exit")
            return;

        if (keyword.empty()) {
            cout << "Kata kunci tidak boleh kosong. Silakan masukkan lagi.\n";
        }
    } while (keyword.empty());

    vector<int> found_indices;

    cout << "\n======== HASIL PENCARIAN ========\n";
    cout << left << setw(5) << "ID"
         << setw(15) << "Kategori"
         << setw(20) << "Nama Barang"
         << setw(15) << "Jumlah Stok"
         << setw(20) << "Tanggal Kadaluarsa"
         << "Status\n";
    cout << string(95, '=') << "\n";

    for (int i = 0; i < fridge.size(); i++)
    {
        if (fridge[i]["nama_barang"].get<string>().find(keyword) != string::npos)
        {
            found_indices.push_back(i);

            string tanggal = fridge[i]["tanggal_kadaluarsa"].get<string>();
            string status = isExpired(tanggal) ? "KADALUARSA" : "MASIH BAIK";

            cout << left << setw(5) << found_indices.size()
                 << setw(15) << fridge[i]["kategori"].get<string>()
                 << setw(20) << fridge[i]["nama_barang"].get<string>()
                 << setw(15) << fridge[i]["jumlah_stok"].get<int>()
                 << setw(20) << tanggal
                 << status << endl;
        }
    }

    if (found_indices.empty())
    {
        cout << "\nTidak ada barang yang cocok.\n";
        return;
    }

    int id;
    do {
        cout << "\nPilih ID barang yang ingin dihapus (1 - " << found_indices.size() << ") atau 0 untuk kembali: ";
        cin >> id;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Input harus berupa angka!\n";
            continue;
        }

        if (id == 0) {
            cout << "Kembali ke menu utama...\n";
            return;
        }

        if (id < 1 || id > found_indices.size()) {
            cout << "ID tidak valid. Silakan coba lagi.\n";
            continue;
        }

        cin.ignore(10000, '\n');
        break;
    } while (true);

    int index_terhapus = found_indices[id - 1];
    string tanggal = fridge[index_terhapus]["tanggal_kadaluarsa"].get<string>();
    string status = isExpired(tanggal) ? "KADALUARSA" : "MASIH BAIK";

    cout << "\nAnda akan menghapus barang berikut:\n";
    cout << "Kategori            : " << fridge[index_terhapus]["kategori"] << endl;
    cout << "Nama Barang         : " << fridge[index_terhapus]["nama_barang"] << endl;
    cout << "Jumlah Stok         : " << fridge[index_terhapus]["jumlah_stok"] << endl;
    cout << "Tanggal Kadaluarsa  : " << tanggal << endl;
    cout << "Status              : " << status << endl;

    char konfirmasi;
    do {
        cout << "\nApakah Anda yakin ingin menghapus barang ini? (y/n atau 'b' untuk kembali): ";
        cin >> konfirmasi;
        cin.ignore(10000, '\n');

        konfirmasi = tolower(konfirmasi);

        if (konfirmasi == 'b') {
            cout << "Kembali ke menu utama...\n";
            return;
        }

        if (konfirmasi != 'y' && konfirmasi != 'n') {
            cout << "Input tidak valid. Masukkan 'y' untuk ya, 'n' untuk tidak, atau 'b' untuk kembali.\n";
            continue;
        }

        break;
    } while (true);

    if (konfirmasi == 'y')
    {
        fridge.erase(fridge.begin() + index_terhapus);

        ofstream file("users.json");
        if (file.is_open())
        {
            file << setw(4) << data << endl;
            file.close();
            cout << "\nBarang berhasil dihapus dan data disimpan ke file.\n";
        }
        else
        {
            cout << "\nGagal menulis ke file JSON.\n";
        }
    }
    else
    {
        cout << "\nBarang tidak jadi dihapus.\n";
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

void updateStok(json &data, const string &Id)
{
    int userIndex = findUserIndex(data, Id);
    if (userIndex == -1)
    {
        cout << "User tidak ditemukan.\n";
        return;
    }

    json &fridge = data[userIndex]["fridgeContents"];
    if (fridge.empty())
    {
        cout << "Kulkas kosong.\n";
        return;
    }

    int kategoriPilihan;
    do
    {
        cout << "\nPilih kategori barang yang ingin diambil:\n";
        cout << "1. Makanan\n";
        cout << "2. Minuman\n";
        cout << "3. Kembali\n";
        cout << "Pilihan: ";
        cin >> kategoriPilihan;
        cin.ignore();

        if (kategoriPilihan == 3)
        {
            cout << "Kembali ke menu utama...\n";
            return;
        }

        string kategoriDipilih = (kategoriPilihan == 1) ? "makanan" : (kategoriPilihan == 2) ? "minuman"
                                                                                             : "";

        if (kategoriDipilih == "")
        {
            cout << "Pilihan tidak valid.\n";
            continue;
        }

        // Filter dan tampilkan list barang sesuai kategori dan status MASIH BAIK
        vector<int> filtered_indices;
        cout << "\nBarang dalam kategori '" << kategoriDipilih << "' dengan status MASIH BAIK:\n";
        cout << left << setw(5) << "ID"
         << setw(20) << "Nama Barang" << setw(15) << "Jumlah Stok"
         << setw(20) << "Tanggal Kadaluarsa" << "Status\n";

        for (int i = 0; i < fridge.size(); ++i)
        {
            string tanggal_kadaluarsa = fridge[i]["tanggal_kadaluarsa"];
            string status = isExpired(tanggal_kadaluarsa) ? "KADALUARSA" : "MASIH BAIK";
            
            // Hanya tampilkan jika kategori sesuai dan status MASIH BAIK
            if (fridge[i]["kategori"] == kategoriDipilih && status == "MASIH BAIK")
            {
                filtered_indices.push_back(i);
                cout << left << setw(5) << filtered_indices.size()
                     << setw(20) << fridge[i]["nama_barang"].get<string>()
                     << setw(15) << fridge[i]["jumlah_stok"].get<int>() 
                     << setw(20) << tanggal_kadaluarsa
                     << status << endl;
            }
        }

        if (filtered_indices.empty())
        {
            cout << "Tidak ada barang dalam kategori tersebut dengan status MASIH BAIK.\n";
            continue;  // Kembali ke menu pilihan kategori
        }

        int pilihan;
        cout << "Pilih ID barang yang ingin diambil: ";
        cin >> pilihan;
        cin.ignore();

        if (pilihan < 1 || pilihan > filtered_indices.size())
        {
            cout << "ID tidak valid.\n";
            continue;  // Kembali ke menu pilihan kategori
        }

        int index_update = filtered_indices[pilihan - 1];
        int current_stock = fridge[index_update]["jumlah_stok"];
        int stok_baru;
        cout << "Masukkan jumlah stok yang ingin diambil: ";
        cin >> stok_baru;
        cin.ignore();
        
        if (stok_baru <= 0)
        {
            cout << "Jumlah stok harus lebih dari 0.\n";
            continue;
        }
        
        if (stok_baru > current_stock)
        {
            cout << "Stok yang ingin dikurangi melebihi jumlah stok saat ini.\n";
            continue;
        }

        fridge[index_update]["jumlah_stok"] = current_stock - stok_baru;

        // Simpan ke file
        ofstream file("users.json");
        if (file.is_open())
        {
            file << setw(4) << data;
            file.close();
            cout << "Stok berhasil diperbarui dan disimpan ke file.\n";
        }
        else
        {
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

    string statusFilter;
    cout << "Filter berdasarkan status (MASIH BAIK/KADALUWARSA/kosongkan untuk semua): ";
    getline(cin, statusFilter);
    
    // Convert to uppercase for case-insensitive comparison
    transform(statusFilter.begin(), statusFilter.end(), statusFilter.begin(), ::toupper);

    cout << "\n======== HASIL PENCARIAN ========\n";
    cout << left << setw(5) << "ID" << setw(15) << "Kategori"
         << setw(20) << "Nama Barang" << setw(15) << "Jumlah Stok"
         << setw(20) << "Tanggal Kadaluwarsa" << "Status\n";
    cout << string(95, '=') << endl;

    int i = 1;
    bool found = false;
    for (Node *curr = head; curr != nullptr; curr = curr->next) {
        string lowerNamaBarang = curr->data.nama_barang;
        transform(lowerNamaBarang.begin(), lowerNamaBarang.end(), lowerNamaBarang.begin(), ::tolower);
        
        string lowerKeyword = keyword;
        transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
        
        bool namaMatches = lowerNamaBarang.find(lowerKeyword) != string::npos;
        bool statusMatches = true;
        
        string status = isExpired(curr->data.tanggal_kadaluarsa) ? "KADALUWARSA" : "MASIH BAIK";
        
        // Apply status filter if specified
        if (!statusFilter.empty()) {
            statusMatches = (status == statusFilter);
        }

        if (namaMatches && statusMatches) {
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
        cout << "Tidak ada barang yang sesuai dengan kriteria pencarian.\n";
        cout << "Kata kunci: '" << keyword << "'";
        if (!statusFilter.empty()) {
            cout << ", Status: '" << statusFilter << "'";
        }
        cout << endl;
    }
    
    cout << string(95, '=') << endl;
}