# Tucil2_13523038_13523106
Repository Tucil 2 for IF2211 "Strategi Algoritma"

# Kompresi Gambar Menggunakan Metode Quadtree

## Daftar Isi
1. [Penjelasan Program](#penjelasan-program)
2. [Requirement Program dan Instalasi](#requirement-program-dan-instalasi)
3. [Cara Kompilasi Program](#cara-kompilasi-program)
4. [Cara Menjalankan Program](#cara-menjalankan-program)
5. [Author](#author)

---

## Penjelasan Program
- **Tujuan:** Program ini mengimplementasikan kompresi gambar menggunakan algoritma **Quadtree** berbasis **divide and conquer**.
- **Deskripsi:**  
  Program membagi gambar ke dalam blok-blok persegi kecil berdasarkan keseragaman warna (dihitung menggunakan metode error seperti Variance, MAD, Max Pixel Difference, Entropy, atau SSIM).  
  Blok akan terus dibagi hingga error dalam blok di bawah **threshold** atau ukuran blok mencapai **minimum block size**.
- **Fitur:**  
  - Input gambar yang dikompresi berdasarkan parameter pengguna.
  - Menyediakan beberapa metode penghitungan error.
  - Menyimpan hasil kompresi sebagai gambar baru.
  - Menampilkan statistik hasil kompresi (ukuran file, persentase kompresi, waktu eksekusi, jumlah node, kedalaman pohon).
  - [Bonus] Membuat **GIF** visualisasi proses pembentukan Quadtree.

---

## Requirement Program dan Instalasi
- **Bahasa Pemrograman:** C++
- **Library eksternal yang digunakan:**
  - **STB Image:** Untuk membaca dan menyimpan file gambar (.png, .jpg, .jpeg).
  - **GIF.h:** Untuk membuat file GIF visualisasi (bonus).
- **Requirement:**
  - Compiler C++17 atau lebih baru (contoh: g++, clang++)
  - Sistem Operasi: Windows, Linux, atau MacOS.
- **Instalasi Tambahan:**
  - Tidak diperlukan instalasi eksternal (semua library menggunakan header-only).
  - Pastikan `stb_image.h`, `stb_image_write.h`, dan `gif.h` tersedia di project folder.

---

## Cara Kompilasi Program
1. Pastikan [Requirement Program](#requirement-program-dan-instalasi) telah terinstal
2. Buka teminal
3. Pastikan berada dalam directory Tucil2_13523038_13523106
4. Jalankan command berikut
```sh
g++ -std=c++17 src/main.cpp src/Image.cpp src/QuadTree.cpp src/IOHandler.cpp src/MakeFrame.cpp src/MakeGif.cpp -o bin/main -lm
```

---

## Cara Menjalankan Program
1. Jalankan program dengan lakukan command berikut 
Untuk Windows:
```sh
./bin/main.exe
```
Untuk Linux:
```sh
./bin/main
```
2. Ikuti alur input program berupa berikut:
  1. Masukkan alamat absolut gambar yang akan dikompresi
  2. Metode perhitungan error (1. Variance, 2. MAD, 3. MPD, 4. Entropy, 5. SSIM).
  3. Threshold (pastikan range nilai sesuai dengan metode yang dipilih).
  4. Ukuran blok minimum.
  5. Mode target persentase kompresi (0.0 hingga 1.0, 0: menonaktifkan)
     - Jika diaktifkan, threshold akan disesuaikan secara otomatis.
  6. Alamat absolut gambar hasil kompresi.
  7. Alamat absolut gif (opsional, kosongkan jika tidak ingin membuat gif).

3. Program memroses input, lalu memberi output statistik dan juga gambar hasil kompres (dan gif jika memasukkan alamatnya) di directory sesuai dengan alamat yang telah anda masukkan.
---

##  Authors:
<div align="center">

<table border="1" cellspacing="0" cellpadding="8"> 
  <tr> <th>NIM</th> <th>Nama</th> </tr> 
  <tr> <td>13523038</td> <td>Abrar Abhirama Widyadhana</td> </tr> 
  </tr> <tr> <td>13523106</td> <td>Athian Nugraha Muarajuang</td> </tr> </table>
</div>
