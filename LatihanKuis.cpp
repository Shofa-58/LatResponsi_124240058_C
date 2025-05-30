#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>

using namespace std;

struct Video
{
    string Judul;
    int durasi;
    string status = "TERSEDIA";
};

struct ptbNode
{
    Video Data;
    ptbNode *right;
    ptbNode *left;

    // constructor
    ptbNode(const Video &value)
    {
        Data = value;
        left = right = nullptr;
    }
};

ptbNode *root = nullptr;

struct queue
{
    Video playlist;
    queue *next;
};

struct stack
{
    Video riwayat;
    stack *lanjut;
};

stack *awal, *top;
queue *depan, *belakang;
int pilihmenu;
char menu_kembali;

void buatstack()
{
    awal = top = NULL;
}

void buatqueue()
{
    depan = NULL;
    belakang = NULL;
}

struct UndoAction
{
    string aksi; // “TAMBAH”, “HAPUS”, “PLAYLIST”, “TONTON”
    Video dataundo;
    UndoAction *next;
};

UndoAction *undoTop = NULL;

bool stackkosong() // untuk mengecek node/data di dalam riwayat masih kosong atau tidak
{
    return (awal == NULL);
}

bool queuekosong() // untuk mengecek node/data di dalam playlist masih kosong atau tidak
{
    return (depan == NULL);
}

void pushUndo(string aksi, Video dataundo)
{
    UndoAction *newAction = new UndoAction;
    newAction->aksi = aksi;
    newAction->dataundo = dataundo;
    newAction->next = undoTop;
    undoTop = newAction;
}

bool undoKosong()
{
    return (undoTop == NULL);
}

UndoAction *popUndo()
{
    if (undoKosong())
        return NULL;
    UndoAction *actionToReturn = undoTop;
    undoTop = undoTop->next;
    return actionToReturn;
}

void deleteNode(ptbNode *&root, string hapus)
{
    if (root == nullptr)
        return; // ngecek jika kosong

    if (hapus < root->Data.Judul)
        deleteNode(root->left, hapus);
    else if (hapus > root->Data.Judul)
        deleteNode(root->right, hapus);
    else
    {
        ptbNode *bantu = root;
        if (root->left == nullptr)
            root = root->right;
        else if (root->right == nullptr)
            root = root->left;
        else
        {
            ptbNode *pengganti = root->right;
            while (pengganti->left != nullptr)
                pengganti = pengganti->left;
            root->Data.Judul = pengganti->Data.Judul;
            deleteNode(root->right, pengganti->Data.Judul);
            return;
        }
        delete bantu;
        cout << "berhasil dihapus" << endl;
    }
}

void insert(ptbNode *&root, const Video &value)
{
    if (root == nullptr)
    {
        root = new ptbNode(value);
        return;
    }

    ptbNode *bantu = root;
    while (true)
    {
        if (value.Judul < bantu->Data.Judul)
        {
            if (bantu->left == nullptr)
            {
                bantu->left = new ptbNode(value);
                cout << "Video Telah di tambahkan" << endl;
                return;
            }
            bantu = bantu->left;
        }
        else if (value.Judul > bantu->Data.Judul)
        {
            if (bantu->right == nullptr)
            {
                bantu->right = new ptbNode(value);
                cout << "Video Telah di tambahkan" << endl;
                return;
            }
            bantu = bantu->right;
        }
        else
        {
            cout << "Tidak dapat menambahkan \"" << value.Judul << "\" Judul Telah ada\n";
            return;
        }
    }
}

void Undo_Tambah_Video(const Video &temp)
{
    deleteNode(root, temp.Judul);
    cout << "Undo Tambah: Video \"" << temp.Judul << "\" telah dihapus dari daftar.\n";
}

void Tambah_Video()
{
    Video tempstruct;
    cout << "\n=====================================\n";
    cout << "         INPUT DATA PESANAN\n";
    cout << "=====================================\n";

    cout << "Judul Video         : ";
    getline(cin, tempstruct.Judul);
    cout << "Durasi Video (menit): ";
    cin >> tempstruct.durasi;
    cin.ignore();

    insert(root, tempstruct);

    pushUndo("TAMBAH", tempstruct); // aksi untuk undo
}

void urutkan_data(ptbNode *root)
{ // Inorder (Kiri Root → Kanan)
    if (root == nullptr)
        return;
    urutkan_data(root->left); // Telusuri kiri dulu
    cout << left << "| "
         << setw(29) << root->Data.Judul << "| "
         << setw(4) << root->Data.durasi << " menit" << "| "
         << setw(20) << root->Data.status << "|" << endl;
    urutkan_data(root->right); // Telusuri kanan
}

void mencari_data(ptbNode *root, string target)
{
    ptbNode *bantu = root;

    while (bantu)
    { // selama bantu bukan null
        if (target == bantu->Data.Judul)
        {
            cout << target << " ditemukan" << endl;
            cout << "\n| " << bantu->Data.Judul << "| " << bantu->Data.durasi << " menit" << "| " << bantu->Data.status << "|" << endl;
            return; // langsung berhenti setelah ketemu
        }
        bantu = (target < bantu->Data.Judul) ? bantu->left : bantu->right;
    }
    cout << target << " tidak ditemukan" << endl;
}

void Tampilkan_Daftar_Video()
{
    if (!root)
    {
        cout << "Tidak ada Data Video Di Daftar Video";
        return;
    }
    string pilih;
    cout << "\n========================== DAFTAR Video ==========================\n";
    cout << "\n------------------------------------------------------------------\n";
    cout << left << "|"
         << setw(30) << " Nama Video" << "|"
         << setw(11) << " Durasi" << "|"
         << setw(21) << " Status" << "|" << endl;

    urutkan_data(root);
    cout << "\n------------------------------------------------------------------\n";
    cout << endl;
    cout << "Apakah Anda ingin mencari video(y/t)?";
    getline(cin, pilih);
    if (pilih == "y" || pilih == "Y")
    {
        string cari;
        cout << "\nInput Video yang ingin dicari : ";
        getline(cin, cari);
        mencari_data(root, cari);
    }
    else
        return;
}

void Undo_Tambahkan_Ke_Playlist(const Video &temp)
{
    // Kembalikan status di pohon ke TERSEDIA
    ptbNode *bantu = root;
    while (bantu)
    {
        if (temp.Judul == bantu->Data.Judul)
        {
            bantu->Data.status = "TERSEDIA";
            break;
        }
        else if (temp.Judul < bantu->Data.Judul)
            bantu = bantu->left;
        else
            bantu = bantu->right;
    }

    // Hapus video itu dari queue (bisa di depan atau di tengah)
    queue *current = depan, *prev = NULL;
    while (current)
    {
        if (current->playlist.Judul == temp.Judul)
        {
            if (prev == NULL)
            {
                // Hapus di depan
                depan = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            if (current == belakang)
            {
                belakang = prev;
            }
            delete current;
            cout << "Undo Playlist: Video \"" << temp.Judul << "\" telah dikeluarkan dari playlist.\n";
            return;
        }
        prev = current;
        current = current->next;
    }
}

void Tambahkan_Ke_Playlist()
{
    if (!root)
    {
        cout << "Belum ada video yang tersedia." << endl;
        return;
    }

    string judulCari;
    cout << "\nMasukkan judul video yang ingin ditambahkan ke playlist: ";
    getline(cin, judulCari);

    ptbNode *bantu = root;
    while (bantu)
    {
        if (judulCari == bantu->Data.Judul)
        {
            if (bantu->Data.status != "TERSEDIA")
            {
                cout << "Video ini sudah dipilih" << endl;
                return;
            }

            // Simpan data Video (sebelum ubah status) ke Undo
            Video v = bantu->Data;
            pushUndo("PLAYLIST", v);

            // Enqueue ke playlist
            queue *NB = new queue;
            NB->playlist = bantu->Data;
            NB->next = NULL;

            if (depan == NULL)
            {
                depan = NB;
                belakang = NB;
                bantu->Data.status = "SEDANG DIPUTAR";
            }
            else
            {
                belakang->next = NB;
                belakang = NB;
                bantu->Data.status = "DALAM ANTRIAN";
            }

            cout << "Video berhasil ditambahkan ke playlist!" << endl;
            return;
        }
        else if (judulCari < bantu->Data.Judul)
            bantu = bantu->left;
        else
            bantu = bantu->right;
    }

    cout << "Video dengan judul \"" << judulCari << "\" tidak ditemukan." << endl;
}

void Undo_Tonton_Video(const Video &temp)
{
    if (!stackkosong())
    {
        stack *curr = awal, *prev = NULL;

        if (awal == top && awal->riwayat.Judul == temp.Judul)
        {
            delete awal;
            awal = top = NULL;
        }
        else
        {
            while (curr->lanjut != top)
                curr = curr->lanjut;
            delete top;
            top = curr;
            top->lanjut = NULL;
        }
    }

    queue *newQ = new queue;
    newQ->playlist = temp;
    newQ->next = depan;
    depan = newQ;
    if (belakang == NULL)
        belakang = newQ;

    // Kembalikan status di pohon menjadi "SEDANG DIPUTAR"
    ptbNode *bantu = root;
    while (bantu)
    {
        if (temp.Judul == bantu->Data.Judul)
        {
            bantu->Data.status = "SEDANG DIPUTAR";
            break;
        }
        else if (temp.Judul < bantu->Data.Judul)
            bantu = bantu->left;
        else
            bantu = bantu->right;
    }

    cout << "Undo Tonton: Video \"" << temp.Judul << "\" telah dikembalikan ke antrian depan.\n";
}

void Tonton_Video()
{
    if (queuekosong())
    {
        cout << "Playlist Masih Kosong" << endl;
        return;
    }

    ptbNode *bantu = root;
    while (bantu)
    {
        if (depan->playlist.Judul == bantu->Data.Judul &&
            bantu->Data.status == "SEDANG DIPUTAR")
        {
            Video temp = bantu->Data;
            pushUndo("TONTON", temp);

            // Ubah status di Pohon jadi TERSEDIA
            bantu->Data.status = "TERSEDIA";

            // Pindahkan ke stack riwayat
            stack *newNode = new stack;
            newNode->riwayat = bantu->Data;
            newNode->lanjut = NULL;
            if (stackkosong())
                awal = top = newNode;
            else
            {
                top->lanjut = newNode;
                top = newNode;
            }

            // Dequeue dari playlist
            queue *hapus = depan;
            depan = depan->next;
            delete hapus;
            if (depan == NULL)
                belakang = NULL;

            // Update status video berikutnya jadi “SEDANG DIPUTAR”
            if (depan != NULL)
            {
                ptbNode *bantu2 = root;
                while (bantu2)
                {
                    if (depan->playlist.Judul == bantu2->Data.Judul)
                    {
                        bantu2->Data.status = "SEDANG DIPUTAR";
                        break;
                    }
                    else if (depan->playlist.Judul < bantu2->Data.Judul)
                        bantu2 = bantu2->left;
                    else
                        bantu2 = bantu2->right;
                }
            }

            cout << "Video Yang Ditonton Telah Tersimpan Di Daftar Riwayat" << endl;
            return;
        }
        else if (depan->playlist.Judul < bantu->Data.Judul)
            bantu = bantu->left;
        else
            bantu = bantu->right;
    }

    cout << "Tidak ditemukan video dalam playlist" << endl;
}

void Riwayat_Tontonan()
{
    if (stackkosong())
    {
        cout << "data riwayat masih kosong" << endl;
        return;
    }

    stack *bantu = awal;

    cout << "\n========================== DAFTAR Video ==========================\n";
    cout << left << "|"
         << setw(30) << " Nama Video" << "|"
         << setw(11) << " Durasi" << "|"
         << setw(21) << " Status" << "|" << endl;

    while (bantu != NULL)
    {
        cout << left << "| "
             << setw(29) << bantu->riwayat.Judul << "| "
             << setw(4) << bantu->riwayat.durasi << " menit" << "| "
             << setw(20) << bantu->riwayat.status << "|" << endl;
        bantu = bantu->lanjut;
    }
    cout << "\n------------------------------------------------------------------\n";
    cout << endl;
}

void Undo_Hapus_Video(const Video &temp)
{
    insert(root, temp);
    cout << "Undo Hapus: Video \"" << temp.Judul << "\" telah dikembalikan ke daftar.\n";
}

void Hapus_Video()
{
    if (!root)
    {
        cout << "Tidak Ada Video " << endl;
        return;
    }

    // Kasus queue hanya berisi satu node
    if (depan == belakang)
    {
        string pilih;
        cout << "Video yang ingin dihapus [sedang diputar]. Yakin untuk tetap menghapus? (y/t) ";
        getline(cin, pilih);

        if (pilih == "y" || pilih == "Y")
        {
            Video temp = depan->playlist;

            pushUndo("HAPUS", temp);

            queue *hapus = depan;
            depan = belakang = NULL;
            delete hapus;

            deleteNode(root, temp.Judul);

            cout << "Data playlist telah dihapus" << endl;
        }
        return;
    }

    // Kasus queue berisi lebih dari satu node
    queue *bantu = depan;
    while (bantu->next != belakang)
    {
        bantu = bantu->next;
    }

    string pilih;
    cout << "Video yang ingin dihapus [masih dalam antrean]. Yakin untuk tetap menghapus? (y/t) ";
    getline(cin, pilih);

    if (pilih == "y" || pilih == "Y")
    {
        Video temp = belakang->playlist;

        pushUndo("HAPUS", temp);

        queue *hapusQ = belakang;
        belakang = bantu;
        belakang->next = NULL;
        delete hapusQ;

        deleteNode(root, temp.Judul);

        cout << "Data playlist telah dihapus" << endl;
    }
}

void Undo_Tindakan_Terakhir()
{
    if (undoKosong())
    {
        cout << "Tidak ada tindakan yang bisa di-undo.\n";
        return;
    }

    // Pop aksi terakhir
    UndoAction *act = popUndo();
    string t = act->aksi;
    Video v = act->dataundo;
    delete act; // buang node aksi baru dari stack

    if (t == "TAMBAH")
    {
        Undo_Tambah_Video(v);
    }
    else if (t == "HAPUS")
    {
        Undo_Hapus_Video(v);
    }
    else if (t == "PLAYLIST")
    {
        Undo_Tambahkan_Ke_Playlist(v);
    }
    else if (t == "TONTON")
    {
        Undo_Tonton_Video(v);
    }
    else
    {
        cout << "Aksi tidak dikenali untuk undo.\n";
    }
}

int main()
{
    buatqueue();
    buatstack();

    do
    {
        cout << "\nMENU PILIHAN:\n"
             << "1. Tambah Video\n"
             << "2. Tampilkan Daftar Video\n"
             << "3. Tambahkan Ke Playlist\n"
             << "4. Tonton Video\n"
             << "5. Riwayat Tontonan\n"
             << "6. Hapus Video\n"
             << "7. Undo Tindakan Terakhir\n"
             << "8. keluar Dari Program\n"
             << endl;
        cout << "Pilih [1-8]: " << endl;
        cin >> pilihmenu;
        cin.ignore();

        switch (pilihmenu)
        {
        case 1:
        {
            Tambah_Video();
            break;
        }
        case 2:
        {
            Tampilkan_Daftar_Video();
            break;
        }
        case 3:
        {
            Tambahkan_Ke_Playlist();
            break;
        }
        case 4:
        {
            Tonton_Video();
            break;
        }
        case 5:
        {
            Riwayat_Tontonan();
            break;
        }
        case 6:
        {
            Hapus_Video();
            break;
        }
        case 7:
        {
            Undo_Tindakan_Terakhir();
            break;
        }
        case 8:
        {
            cout << "\nTerima kasih!" << endl;
            exit(0);
            break;
        }
        default:
            cout << "\nPilihan tidak valid" << endl;
        }

        cout << "\nKembali ke menu? (y/n): ";
        cin >> menu_kembali;
        cin.ignore();
    } while (menu_kembali == 'y' || menu_kembali == 'Y');

    return 0;
}
