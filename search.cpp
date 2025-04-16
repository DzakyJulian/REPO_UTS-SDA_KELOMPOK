#include <iostream>
#include <fstream>
#include <string>
#include "input.cpp"

using namespace std;


void cari()
{
    if (jumlah_data == 0)
    {
        cout << "Tidak ada barang yang kang kamu cari." << endl;
        return; 
    }

    string dicari;
    cout << "Masukkan nama barang yang ingin dicari: ";
    getline(cin, dicari);

    Barang* ptr = daftar_barang;
    int ditemukan = 0;

    cout << "\nHasil pencarian:\n";
    cout << "ID\tKategori\tNama Barang\tJumlah Stok\tTanggal Kadaluarsa\n";
    
    for (int i = 0; i < jumlah_data; i++)
    {
        string nama_barang_lower = (ptr + i)->nama_barang;
        transform(nama_barang_lower.begin(),nama_barang_lower.end(), nama_barang_lower.begin(), ::tolower);
        
        if (nama_barang_lower.find(dicari) != string::npos)
        {
            cout << ditemukan + 1 << "\t"
                 << (ptr + i)->kategori << "\t\t"
                 << (ptr + i)->nama_barang << "\t\t"
                 << (ptr + i)->jumlah_stok << "\t\t"
                 << (ptr + i)->tanggal_kadaluarsa<< endl;
                 ditemukan++;
        }
    }
    
    if(!ditemukan)
    {
        cout << "Barang dengan nama " << dicari << "tidak ditemukan.\n";
    }
    
}