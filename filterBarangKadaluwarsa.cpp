#include <iostream>
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
    
    if (userIndex == -1) return;
    
    json& fridgeContents = data[userIndex]["fridgeContents"];
    vector<size_t> expiredIndices;
    
    // Find expired items
    cout << "\n=== BARANG KADALUARSA ===\n";
    bool foundExpired = false;
    
    for (size_t i = 0; i < fridgeContents.size(); ++i) {
        if (isExpiredStatus(fridgeContents[i]["tanggal_kadaluarsa"])) {
            cout << "- " << fridgeContents[i]["nama_barang"] 
                 << " (Exp: " << fridgeContents[i]["tanggal_kadaluarsa"] << ")\n";
            expiredIndices.push_back(i);
            foundExpired = true;
        }
    }
    
    if (!foundExpired) {
        cout << "Tidak ada barang yang kadaluarsa.\n";
        return;
    }
    
    // Ask for confirmation to remove
    cout << "\nApakah Anda ingin membuang barang yang kadaluarsa? (Enter untuk lanjut, 'n' untuk batal): ";
    string input;
    getline(cin, input);
    
    if (input == "n" || input == "N") {
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
        cout << "Barang kadaluarsa telah dihapus.\n";
    } else {
        cerr << "Gagal menyimpan perubahan.\n";
    }
}