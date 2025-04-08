#ifndef QUADTREE_H
#define QUADTREE_H

#include "Image.h" // Menggunakan definisi Pixel dan Image dari Image.h
#include <vector>
#include <memory> // Untuk std::unique_ptr dan std::make_unique
#include <array>  // Untuk std::array
#include <string> // Untuk std::string
#include <stdexcept> // Untuk std::runtime_error, std::invalid_argument
#include <limits>    // Untuk std::numeric_limits

// Forward declaration untuk kelas Quadtree
class Quadtree;

// Enum class untuk pilihan metode perhitungan error (lebih aman tipe)
enum class ErrorMetric {
    VARIANCE,
    MAD, // Mean Absolute Deviation
    MAX_PIXEL_DIFFERENCE,
    ENTROPY
    // Bisa ditambahkan SSIM jika diperlukan
};

// --- Kelas QuadTreeNode ---
// Merepresentasikan satu node dalam Quadtree (sebuah region gambar)
class QuadTreeNode {
private:
    // Data Anggota
    int x, y;                   // Koordinat pojok kiri atas region
    int width, height;          // Dimensi region
    Pixel averageColor;         // Warna rata-rata region (terutama penting untuk leaf)
    bool leaf;                  // Apakah node ini merupakan leaf (tidak dibagi lagi)?
    std::array<std::unique_ptr<QuadTreeNode>, 4> children; // Anak-anak node (auto-managed memory)
    const Image& sourceImage;   // Referensi ke gambar asli (untuk perhitungan)

    // Helper Methods (Private)
    Pixel calculateAverageColor() const;              // Menghitung warna rata-rata region ini
    double calculateError(ErrorMetric metric) const;  // Menghitung error region ini berdasarkan metrik

    // Fungsi perhitungan error statis internal (dipanggil oleh calculateError)
    // Dibuat statis karena tidak bergantung pada state spesifik objek node, hanya pada region gambar
    static double calculateVarianceInternal(const Image& img, int x, int y, int w, int h);
    static double calculateMADInternal(const Image& img, int x, int y, int w, int h);
    static double calculateMaxPixelDifferenceInternal(const Image& img, int x, int y, int w, int h);
    static double calculateEntropyInternal(const Image& img, int x, int y, int w, int h);

    // Memberikan akses ke Quadtree (khususnya buildRecursive) untuk memodifikasi state internal
    // seperti 'leaf' dan 'children', serta memanggil 'calculateError'.
    friend class Quadtree;

public:
    // Konstruktor
    // Membuat node baru untuk region tertentu dari gambar sumber.
    QuadTreeNode(int x, int y, int width, int height, const Image& image);

    // Destruktor tidak perlu didefinisikan secara eksplisit karena std::unique_ptr
    // akan otomatis menghapus memori anak-anaknya saat node dihancurkan.
    // ~QuadTreeNode() = default; // Bisa ditambahkan jika ingin eksplisit

    // Getters - Menyediakan akses read-only ke properti node
    bool isLeaf() const { return leaf; }
    Pixel getAverageColor() const { return averageColor; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    // Mengembalikan referensi konstan ke array unique_ptr anak
    const std::array<std::unique_ptr<QuadTreeNode>, 4>& getChildren() const { return children; }

    // Method untuk Analisis Tree (dipanggil secara rekursif)
    // Mengumpulkan pointer ke semua node di bawah node ini (termasuk dirinya)
    void collectNodes(std::vector<const QuadTreeNode*>& nodes) const;

    // Method untuk Rekonstruksi Gambar (dipanggil secara rekursif)
    // Menggambar region node ini ke targetImage jika leaf, atau memanggil anak-anaknya
    void reconstructRegion(Image& targetImage) const;
};


// --- Kelas Quadtree ---
// Merepresentasikan keseluruhan struktur Quadtree dan mengelola proses pembuatannya
class Quadtree {
private:
    std::unique_ptr<QuadTreeNode> rootNode; // Node akar pohon (auto-managed memory)
    const Image& sourceImage;               // Referensi ke gambar sumber asli
    int imageWidth;                         // Dimensi gambar sumber
    int imageHeight;
    ErrorMetric errorMetricChoice;          // Metrik error yang digunakan
    double errorThreshold;                  // Ambang batas error untuk subdivisi
    int minimumBlockSize;                   // Ukuran (luas) minimum blok agar tidak dibagi

    // Anggota untuk menyimpan hasil perhitungan agar efisien
    size_t nodeCount; // Jumlah total node dalam pohon
    int maxDepth;    // Kedalaman (level) maksimum pohon

    // Helper rekursif privat untuk membangun pohon dan menghitung metrik
    // Dipanggil oleh konstruktor.
    void buildRecursive(QuadTreeNode* node, int currentDepth);

public:
    // Konstruktor Utama
    // Menerima gambar sumber dan parameter kompresi, lalu membangun Quadtree.
    Quadtree(const Image& image, ErrorMetric metric, double threshold, int minSize);

    // Destruktor tidak perlu didefinisikan secara eksplisit karena std::unique_ptr
    // akan otomatis menghapus rootNode beserta seluruh turunannya.
    // ~Quadtree() = default;

    // Method Utama
    // Membuat dan mengembalikan objek Image baru yang direkonstruksi dari Quadtree.
    Image reconstructImage() const;

    // Method untuk Analisis Tree (Akses Efisien O(1))
    // Mengembalikan kedalaman maksimum pohon yang sudah dihitung.
    int getDepth() const { return maxDepth; }
    // Mengembalikan jumlah total node dalam pohon yang sudah dihitung.
    size_t getNodeCount() const { return nodeCount; }

    // Method Akses Lainnya
    // Mengembalikan pointer konstan ke node akar (bisa null jika tree kosong/gagal).
    const QuadTreeNode* getRoot() const { return rootNode.get(); }
    // Mengisi vektor yang diberikan dengan pointer ke semua node dalam pohon.
    void getAllNodes(std::vector<const QuadTreeNode*>& nodes) const;

}; // Akhir kelas Quadtree

#endif // QUADTREE_H