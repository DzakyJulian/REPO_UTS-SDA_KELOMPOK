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