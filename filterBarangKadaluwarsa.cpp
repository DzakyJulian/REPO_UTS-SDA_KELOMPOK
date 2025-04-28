#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// Function to check if a date is expired
bool isExpiredStatus(const string& expiryDate) {
    // Parse current date
    time_t now = time(0);
    tm* currentDate = localtime(&now);
    
    // Parse expiry date (format: dd/mm/yyyy)
    int day, month, year;
    if (sscanf(expiryDate.c_str(), "%d/%d/%d", &day, &month, &year) != 3) {
        return true; // Consider invalid dates as expired
    }
    
    // Compare dates
    if (year < currentDate->tm_year + 1900) return true;
    if (year == currentDate->tm_year + 1900) {
        if (month < currentDate->tm_mon + 1) return true;
        if (month == currentDate->tm_mon + 1 && day < currentDate->tm_mday) return true;
    }
    return false;
}

// Function to filter and remove expired items
void filterExpiredItems(json& data, const string& userId) {
    // Find user index
    int userIndex = -1;
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i]["id"] == userId) {
            userIndex = i;
            break;
        }
    }
    
    if (userIndex == -1) {
        cout << "User tidak ditemukan.\n";
        return;
    }
    
    json& fridgeContents = data[userIndex]["fridgeContents"];
    vector<size_t> expiredIndices;
    
    // Display header
    cout << "\n======== BARANG KADALUWARSA ========\n";
    
    // Check if fridge is empty
    if (fridgeContents.empty()) {
        cout << "Kulkas kosong.\n";
        cout << string(95, '=') << "\n";
        return;
    }
    
    cout << left << setw(5) << "ID" 
         << setw(15) << "Kategori" 
         << setw(20) << "Nama Barang" 
         << setw(15) << "Jumlah Stok" 
         << setw(20) << "Tanggal Kadaluwarsa" 
         << "Status\n";
    cout << string(95, '=') << "\n";
    
    // Find and display expired items
    bool foundExpired = false;
    int displayId = 1;
    
    for (size_t i = 0; i < fridgeContents.size(); ++i) {
        string expiryDate = fridgeContents[i]["tanggal_kadaluarsa"];
        bool isExpired = isExpiredStatus(expiryDate);
        
        if (isExpired) {
            expiredIndices.push_back(i);
            foundExpired = true;
            
            cout << left << setw(5) << displayId++
                 << setw(15) << fridgeContents[i]["kategori"].get<string>()
                 << setw(20) << fridgeContents[i]["nama_barang"].get<string>()
                 << setw(15) << fridgeContents[i]["jumlah_stok"].get<int>()
                 << setw(20) << expiryDate
                 << "KADALUWARSA" << endl;
        }
    }
    
    if (!foundExpired) {
        cout << "Tidak ada barang yang kadaluwarsa.\n";
        cout << string(95, '=') << "\n";
        return;
    }
    
    cout << string(95, '=') << "\n";
    
    // Ask for confirmation to remove
    cout << "\nApakah Anda ingin membuang " << expiredIndices.size() 
         << " barang yang kadaluwarsa? (y/n): ";
    string input;
    getline(cin, input);
    
    if (input != "y" && input != "Y") {
        cout << "Penghapusan dibatalkan.\n";
        return;
    }
    
    // Remove expired items (starting from the end to preserve indices)
    for (auto it = expiredIndices.rbegin(); it != expiredIndices.rend(); ++it) {
        fridgeContents.erase(fridgeContents.begin() + *it);
    }
    
    // Save changes to file
    ofstream file("users.json");
    if (file.is_open()) {
        file << data.dump(4);
        file.close();
        cout << expiredIndices.size() << " barang kadaluarsa telah dihapus.\n";
    } else {
        cerr << "Gagal menyimpan perubahan.\n";
    }
}