#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include "json.hpp"
#include "login.cpp"
#include "menu.cpp"
#include "register.cpp"
#include "filterBarangKadaluwarsa.cpp"

using namespace std;
using json = nlohmann::json;

void clearTerminal() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    // ANSI escape codes for additional clearing
    cout << "\033[2J\033[1;1H";
}

int main() {
    int pilihan;
    json fridgeContents;
    string userId;

    while (true) {
        cout << "\n=== MENU UTAMA ===\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "3. Keluar\n";
        cout << "Pilih: ";
        cin >> pilihan;
        cin.ignore();  // Membersihkan newline dari buffer

        if (pilihan == 1) {
            if (indexLogin(userId, fridgeContents)) { // Jika login berhasil
                clearTerminal();
                cout << "Login berhasil! Tekan enter untuk lanjut...";
                cin.ignore();
                clearTerminal();

                // Load file JSON user
                ifstream file("users.json");
                json data;
                if (file.is_open()) {
                    file >> data;
                    file.close();
                } else {
                    cerr << "Gagal membuka file users.json.\n";
                    return 1;
                }

                // Menu setelah login
                int menuKulkas;
                do {
                    clearTerminal();  // Clear terminal setiap kali kembali ke menu kulkas
                    cout << "\n=== MENU KULKAS ===\n";
                    cout << "1. Input Barang\n";
                    cout << "2. Tampilkan Isi Kulkas\n";
                    cout << "3. Cari Barang\n";
                    cout << "4. Hapus Barang\n";
                    cout << "5. Update Stok Barang\n";
                    cout << "6. Filter Barang Kadaluarsa\n";  // New option
                    cout << "0. Logout\n";
                    cout << "Pilih: ";
                    cin >> menuKulkas;
                    cin.ignore();

                    Node* head = nullptr;

                    switch (menuKulkas) {
                        case 1:
                            input(data, userId);
                            break;
                        case 2:
                            parseFridgeToList(data[findUserIndex(data, userId)]["fridgeContents"], head);
                            display(head);
                            cout << "\nTekan enter untuk melanjutkan...";
                            cin.ignore();
                            break;
                        case 3:
                            parseFridgeToList(data[findUserIndex(data, userId)]["fridgeContents"], head);
                            cari(head);
                            cout << "\nTekan enter untuk melanjutkan...";
                            cin.ignore();
                            break;
                        case 4:
                            hapusBarang(data, userId);
                            break;
                        case 5:
                            updateStok(data, userId);
                            break;
                        case 6:  // Add this as option 6 in your menu
                            filterExpiredItems(data, userId);
                            cout << "\nTekan enter untuk melanjutkan...";
                            cin.ignore();
                            break;
                        case 0:
                            cout << "Logout berhasil.\n";
                            this_thread::sleep_for(chrono::milliseconds(1000));
                            break;
                        default:
                            cout << "Pilihan tidak valid.\n";
                            this_thread::sleep_for(chrono::milliseconds(1000));
                    }

                    // Bebaskan memori linked list
                    while (head != nullptr) {
                        Node* temp = head;
                        head = head->next;
                        delete temp;
                    }

                } while (menuKulkas != 0);  // keluar dari menu kulkas (logout)
                
            } else {
                cout << "Login gagal. Username atau password salah.\n";
                this_thread::sleep_for(chrono::milliseconds(2000));
                clearTerminal();
            }

        } else if (pilihan == 2) {
            clearTerminal();
            string username, password;
            cout << "=== MENU REGISTRASI ===\n";
            cout << "Masukkan username baru: ";
            getline(cin, username);
            cout << "Masukkan password: ";
            getline(cin, password);

            registerUser(username, password);
            cout << "Registrasi berhasil! Tekan enter untuk melanjutkan...";
            cin.ignore();
            clearTerminal();

        } else if (pilihan == 3) {
            clearTerminal();
            cout << "Keluar dari program.\n";
            break;

        } else {
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
            this_thread::sleep_for(chrono::milliseconds(1000));
            clearTerminal();
        }
    }

    return 0;
}