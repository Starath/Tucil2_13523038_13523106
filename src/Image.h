#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include <stdexcept> // Untuk exceptions standar
#include <cstddef>   // Untuk std::size_t

// Definisikan struct Pixel (tidak berubah)
struct Pixel {
    unsigned char r = 0, g = 0, b = 0;
};

// Deklarasi kelas exception kustom (opsional tapi bagus)
class ImageError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};


class Image {
private:
    int width = 0;
    int height = 0;
    std::vector<Pixel> pixels;

    // Helper untuk validasi koordinat (tidak perlu publik)
    bool checkBounds(int x, int y) const noexcept;
    // Helper untuk menghitung indeks 1D (juga tidak perlu publik)
    std::size_t getIndex(int x, int y) const; // Implementasi bisa inline di cpp

public:
    // Konstanta untuk jumlah channel warna
    static constexpr int NumChannels = 3;

    // Konstruktor default: Membuat gambar kosong
    Image() noexcept = default;

    // Konstruktor untuk membuat gambar kosong dengan dimensi tertentu
    // Melempar std::invalid_argument jika w atau h <= 0
    Image(int w, int h);

    // Factory method untuk memuat gambar dari file (lebih idiomatik)
    // Melempar ImageError jika gagal load atau decode
    static Image loadFromFile(const std::string& filePath);

    // Memuat gambar dari file ke objek yang sudah ada
    // Melempar ImageError jika gagal load atau decode
    void loadImage(const std::string& filePath);

    // Menyimpan gambar ke file.
    // Melempar ImageError jika gagal menyimpan atau format tidak didukung.
    // Kualitas hanya relevan untuk JPG (default 85).
    void saveImage(const std::string& filePath, int jpgQuality = 85) const;

    // Mendapatkan piksel pada koordinat (x, y)
    // Melempar std::out_of_range jika x atau y di luar batas.
    Pixel getPixel(int x, int y) const;

    // Mengatur piksel pada koordinat (x, y)
    // Melempar std::out_of_range jika x atau y di luar batas.
    void setPixel(int x, int y, const Pixel& p);

    // Mengisi seluruh gambar dengan satu warna
    void fill(const Pixel& p);

    // Getter untuk dimensi (noexcept karena dijamin tidak melempar)
    int getWidth() const noexcept;
    int getHeight() const noexcept;

    // Mendapatkan jumlah total piksel (noexcept)
    std::size_t getPixelCount() const noexcept;

    // Mendapatkan akses (read-only) ke data piksel mentah (vector 1D) (noexcept)
    const std::vector<Pixel>& getPixelData() const noexcept;

    // Mendapatkan akses (read/write) ke data piksel mentah (vector 1D) (noexcept)
    // Gunakan dengan hati-hati karena bisa membatalkan invariant internal jika ukuran diubah.
    std::vector<Pixel>& getPixelData() noexcept;

    // Memeriksa apakah gambar kosong (noexcept)
    bool isEmpty() const noexcept;

};

#endif // IMAGE_H