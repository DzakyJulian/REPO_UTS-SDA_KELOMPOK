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
    bool ditemukan = false;
    cout << "\nHasil pencarian:\n";
    for (int i = 0; i < jumlah_data; i++)
    {
        if ((ptr + i) -> nama_barang == dicari)
        {
            cout << "\nBarang ditemukan:\n";
            cout << "Nama Barang        : " << (ptr + i)->nama_barang << endl;
            cout << "Kategori           : " << (ptr + i)->kategori << endl;
            cout << "Jumlah Stok        : " << (ptr + i)->jumlah_stok << endl;
            cout << "Tanggal Kadaluarsa : " << (ptr + i)->tanggal_kadaluarsa << endl;
            ditemukan = true;
        }
    }
    
    if(!ditemukan)
    {
        cout << "Barang dengan nama " << dicari << "tidak deitemukan.\n";
    }
    
}