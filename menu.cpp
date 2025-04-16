#include <iostream>
#include <string>
#include <regex>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "json.hpp"

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

int findUserIndex(const json &data, const string &userId)
{
    for (int i = 0; i < data.size(); ++i)
    {
        if (data[i]["id"] == userId)
        {
            return i;
        }
    }
    return -1; // Not found
}

// Fungsi input data barang
void input(json &data, const string &userId)
{
    int selesai;
    int userIndex = findUserIndex(data, userId);
    if (userIndex == -1)
    {
        cout << "User tidak ditemukan!\n";
        return;
    }

    do
    {
        string kategori;
        do
        {
            cout << "Masukkan kategori (makanan/minuman): ";
            getline(cin, kategori);
            if (!validKategori(kategori))
            {
                cout << "Kategori harus 'makanan' atau 'minuman'. Silakan coba lagi." << endl;
            }
        } while (!validKategori(kategori));

        cout << "Masukkan nama barang: ";
        string nama_barang;
        getline(cin, nama_barang);

        string input_jumlah;
        int jumlah_stok;
        do
        {
            cout << "Masukkan jumlah stok (angka): ";
            getline(cin, input_jumlah);
            if (!validJumlahStok(input_jumlah, jumlah_stok))
            {
                cout << "Jumlah stok harus berupa angka positif. Silakan coba lagi." << endl;
            }
        } while (!validJumlahStok(input_jumlah, jumlah_stok));

        string tanggal_kadaluarsa;
        do
        {
            cout << "Masukkan tanggal kadaluarsa (dd/mm/yyyy): ";
            getline(cin, tanggal_kadaluarsa);
            if (!validTanggal(tanggal_kadaluarsa))
            {
                cout << "Format tanggal salah. Gunakan format dd/mm/yyyy. Silakan coba lagi." << endl;
            }
        } while (!validTanggal(tanggal_kadaluarsa));

        json newItem = {
            {"nama_barang", nama_barang},
            {"jumlah_stok", jumlah_stok},
            {"kategori", kategori},
            {"tanggal_kadaluarsa", tanggal_kadaluarsa}};

        data[userIndex]["fridgeContents"].push_back(newItem);

        // Tulis kembali ke file
        ofstream file("users.json");
        if (file.is_open())
        {
            file << setw(4) << data;
            file.close();
            cout << "Item berhasil ditambahkan dan disimpan ke file.\n";
        }
        else
        {
            cerr << "Gagal menyimpan ke file.\n";
        }

        cout << "Konfirmasi Data Barang" << endl;
        cout << "1. Sudah selesai" << endl;
        cout << "2. Tambahkan barang lagi" << endl;
        cout << "Apakah sudah selesai?: ";
        cin >> selesai;
        cin.ignore();
        if (selesai != 1 && selesai != 2)
        {
            cout << "Pilihan tidak valid. Silakan pilih 1 atau 2." << endl;
        }

    } while (selesai == 2);

    cout << "\nKembali ke menu utama...\n"
         << endl;
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

void clearFridgeList(Node *&head)
{
    while (head != nullptr)
    {
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
        cout << "\nPilih kategori barang yang ingin diupdate:\n";
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

        // Filter dan tampilkan list barang sesuai kategori
        vector<int> filtered_indices;
        cout << "\nBarang dalam kategori '" << kategoriDipilih << "':\n";
        cout << "ID\tNama Barang\tJumlah Stok\n";

        for (int i = 0; i < fridge.size(); ++i)
        {
            if (fridge[i]["kategori"] == kategoriDipilih)
            {
                filtered_indices.push_back(i);
                cout << filtered_indices.size() << "\t"
                     << fridge[i]["nama_barang"] << "\t\t"
                     << fridge[i]["jumlah_stok"] << endl;
            }
        }

        if (filtered_indices.empty())
        {
            cout << "Tidak ada barang dalam kategori tersebut.\n";
            return;
        }

        int pilihan;
        cout << "Pilih ID barang yang ingin diupdate: ";
        cin >> pilihan;
        cin.ignore();

        if (pilihan < 1 || pilihan > filtered_indices.size())
        {
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
