#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
#include "login.cpp"
#include "menu.cpp"
#include "register.cpp"

using namespace std;
using json = nlohmann::json;

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
            indexLogin(userId, fridgeContents);

            if (userId.empty()) {
                continue; // Jika login gagal, kembali ke menu utama
            }

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
                cout << "\n=== MENU KULKAS ===\n";
                cout << "1. Input Barang\n";
                cout << "2. Tampilkan Isi Kulkas\n";
                cout << "3. Hapus Barang\n";
                cout << "4. Update Stok Barang\n";
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
                        break;
                    case 3:
                        hapusBarang(data, userId);
                        break;
                    case 4:
                        updateStok(data, userId);
                        break;
                    case 0:
                        cout << "Logout berhasil.\n";
                        break;
                    default:
                        cout << "Pilihan tidak valid.\n";
                }

                // Bebaskan memori linked list
                while (head != nullptr) {
                    Node* temp = head;
                    head = head->next;
                    delete temp;
                }

            } while (menuKulkas != 0);  // keluar dari menu kulkas (logout)

        } else if (pilihan == 2) {
            string username, password;
            cout << "Masukkan username baru: ";
            getline(cin, username);
            cout << "Masukkan password: ";
            getline(cin, password);

            registerUser(username, password);

        } else if (pilihan == 3) {
            cout << "Keluar dari program.\n";
            break;

        } else {
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    }

    return 0;
}
