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
tm *local_time = localtime(&now);

struct Barang
{
    string kategori;
    string nama_barang;
    int jumlah_stok;
    string tanggal_kadaluarsa;
};

struct Node
{
    Barang data;
    Node *next;
};

Barang daftar_barang[MAKS_DATA];
int jumlah_data = 0;

// Validasi kategori
bool validKategori(const string &kategori)
{
    return kategori == "makanan" || kategori == "minuman";
}

// Validasi jumlah stok
bool validJumlahStok(const string &input, int &jumlah)
{
    try
    {
        size_t pos;
        int val = stoi(input, &pos);
        if (pos != input.length() || val < 0)
        {
            return false;
        }
        jumlah = val;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

// Validasi tanggal kadaluarsa (format dd/mm/yyyy)
bool validTanggal(const string &tanggal)
{
    regex pola(R"(^\d{2}/\d{2}/\d{4}$)");
    return regex_match(tanggal, pola);
}

int findUserIndex(const json& data, const string& userId) {
    for (int i = 0; i < data.size(); ++i) {
        if (data[i]["id"] == userId) {
            return i;
        }
    }
    return -1; // Not found
}

// Fungsi input data barang
void input(json& data, const string& userId) {
    char selesai;
    int userIndex = findUserIndex(data, userId);
    if (userIndex == -1) {
        cout << "User tidak ditemukan!\n";
        return;
    }

    do {
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

        json newItem = {
            {"nama_barang", nama_barang},
            {"jumlah_stok", jumlah_stok},
            {"kategori", kategori},
            {"tanggal_kadaluarsa", tanggal_kadaluarsa}
        };

        data[userIndex]["fridgeContents"].push_back(newItem);

        // Tulis kembali ke file
        ofstream file("users.json");
        if (file.is_open()) {
            file << setw(4) << data;
            file.close();
            cout << "Item berhasil ditambahkan dan disimpan ke file.\n";
        } else {
            cerr << "Gagal menyimpan ke file.\n";
        }

        cout << "Apakah sudah selesai input? (y/n): ";
        cin >> selesai;
        cin.ignore();

    } while (selesai == 'n' || selesai == 'N');

    cout << "\nKembali ke menu utama...\n" << endl;
}

// fungsi untuk mengecek kadaluarsa
bool isExpired(const string &exp_date)
{
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

void parseFridgeToList(const json &fridgeJson, Node *&head)
{
    head = nullptr;
    Node *tail = nullptr;

    for (const auto &item : fridgeJson)
    {
        Node *newNode = new Node;
        newNode->data.nama_barang = item["nama_barang"].get<string>();
        newNode->data.jumlah_stok = item["jumlah_stok"].get<int>();
        newNode->data.kategori = item["kategori"].get<string>();
        newNode->data.tanggal_kadaluarsa = item["tanggal_kadaluarsa"].get<string>();
        newNode->next = nullptr;

        if (!head)
        {
            head = newNode;
            tail = head;
        }
        else
        {
            tail->next = newNode;
            tail = newNode;
        }
    }
}

// Fungsi tampilkan isi kulkas
void display(Node *head)
{
    if (!head)
    {
        cout << "Kulkas kosong.\n";
        return;
    }

    cout << "======== ISI KULKAS ========\n";
    cout << left << setw(5) << "ID" << setw(15) << "Kategori"
         << setw(20) << "Nama Barang" << setw(15) << "Jumlah Stok"
         << setw(20) << "Tanggal Kadaluarsa" << "Status\n";

    int i = 1;
    for (Node *curr = head; curr != nullptr; curr = curr->next)
    {
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

// Fungsi untuk mencari barang berdasarkan nama barang
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
    cout << "Masukkan kata kunci untuk mencari barang: ";
    getline(cin, keyword);

    vector<int> found_indices;
    cout << "Barang yang ditemukan:\n";
    cout << "ID\tKategori\tNama Barang\tJumlah Stok\tTanggal Kadaluarsa\n";

    for (int i = 0; i < fridge.size(); i++)
    {
        if (fridge[i]["nama_barang"].get<string>().find(keyword) != string::npos)
        {
            found_indices.push_back(i);
            cout << found_indices.size() << "\t"
                 << fridge[i]["kategori"] << "\t\t"
                 << fridge[i]["nama_barang"] << "\t\t"
                 << fridge[i]["jumlah_stok"] << "\t\t"
                 << fridge[i]["tanggal_kadaluarsa"] << "\n";
        }
    }

    if (found_indices.empty())
    {
        cout << "Tidak ada barang yang cocok.\n";
        return;
    }

    int id;
    cout << "Pilih ID barang yang ingin dihapus (1 - " << found_indices.size() << "): ";
    cin >> id;
    cin.ignore();

    if (id < 1 || id > found_indices.size())
    {
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

    if (konfirmasi == 'y' || konfirmasi == 'Y')
    {
        fridge.erase(fridge.begin() + index_terhapus);

        ofstream file("users.json");
        if (file.is_open())
        {
            file << setw(4) << data << endl;
            file.close();
            cout << "Barang berhasil dihapus dan data disimpan ke file.\n";
        }
        else
        {
            cout << "Gagal menulis ke file JSON.\n";
        }
    }
    else
    {
        cout << "Barang tidak jadi dihapus.\n";
    }

    cout << "\nKembali ke menu utama...\n"
         << endl;
}

json loadData()
{
    ifstream file("users.json");

    if (!file.is_open())
    {
        cerr << "Error: Failed to open " << "users.json" << endl;
        return {}; // Return empty JSON object
    }

    try
    {
        return json::parse(file);
    }
    catch (const json::parse_error &e)
    {
        cerr << "JSON parse error at byte " << e.byte << ": " << e.what() << endl;
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return {};
}

void clearFridgeList(Node*& head) {
    while (head != nullptr) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}


void indexLogin(string &Id, json &fridgeContents);
// Program utama
int main()
{
    json data = loadData();
    json fridgeContents;
    if (data.empty())
    {
        cout << "No data loaded or file is empty" << endl;
    }

    int choices;
    string Id;
    while (Id.empty())
    {
        indexLogin(Id, fridgeContents);
    }

    Node *fridgeList = nullptr;
    parseFridgeToList(fridgeContents, fridgeList);

    while (true)
    {
        cout << "========== SMART FRIDGE ==========\n";
        cout << "1. Display Fridge Contents\n";
        cout << "2. Add Item to Fridge\n";
        cout << "3. Remove Item from Fridge\n";
        cout << "4. Exit\n";
        cout << "Masukkan pilihan Anda: ";
        cin >> choices;
        cin.ignore(); // untuk membersihkan buffer newline

        switch (choices)
        {
        case 1:
        {
            data = loadData(); // reload data dari file
            int userIndex = findUserIndex(data, Id);
            if (userIndex == -1) {
                cout << "User tidak ditemukan!\n";
                break;
            }

            fridgeContents = data[userIndex]["fridgeContents"];

            // Kosongkan fridgeList lama dulu
            clearFridgeList(fridgeList); // fungsi untuk hapus semua node
            parseFridgeToList(fridgeContents, fridgeList); // parsing ulang

            display(fridgeList);

            cout << "\nTekan Enter untuk melanjutkan...";
            cin.ignore();
            break;
        }
        case 2:
            input(data, Id);
            break;
        case 3:
        {
            hapusBarang(data, Id);
            break;
        }
        case 33: // Jika ingin hidden option
            indexLogin(Id, fridgeContents);
            break;
        case 4:
            cout << "Terima kasih telah menggunakan Smart Fridge!\n";
            return 0; // Langsung return untuk menghentikan program
        default:
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
            // Tidak perlu break karena sudah di akhir switch
        }
    }

    return 0;
}
