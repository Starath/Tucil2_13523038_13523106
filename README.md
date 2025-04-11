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
  - **STB Image:** Untuk membaca dan menyimpan file gambar (.png, .jpg, .bmp, .tga).
  - **GIF.h:** Untuk membuat file GIF visualisasi (bonus).
- **Requirement:**
  - Compiler C++17 atau lebih baru (contoh: g++, clang++)
  - Sistem Operasi: Windows, Linux, atau MacOS.
- **Instalasi Tambahan:**
  - Tidak diperlukan instalasi eksternal (semua library menggunakan header-only).
  - Pastikan `stb_image.h`, `stb_image_write.h`, dan `gif.h` tersedia di project folder.

---

## Cara Kompilasi Program


---

## Cara Menjalankan Program


---

##  Authors:
<div align="center">

<table border="1" cellspacing="0" cellpadding="8"> 
  <tr> <th>NIM</th> <th>Nama</th> </tr> 
  <tr> <td>13523038</td> <td>Abrar Abhirama Widyadhana</td> </tr> 
  </tr> <tr> <td>13523106</td> <td>Athian Nugraha Muarajuang</td> </tr> </table>
</div>
