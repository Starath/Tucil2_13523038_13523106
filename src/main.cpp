#include "QuadTree.h"
#include "Image.h"
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::cout << "Starting Image Class Test Driver..." << std::endl;

    // Langkah 1: Meminta input metode perhitungan
    int methodChoice;
    std::cout << "Pilih metode kompresi gambar:\n";
    std::cout << "1. Variance\n2. Threshold Variance\n3. Minimum Block Size\n";
    std::cin >> methodChoice;

    // Variabel untuk menyimpan parameter
    float threshold = 0;        // Threshold untuk variansi
    int minBlockSize = 0;      // Ukuran minimum blok

    // Langkah 2: Menanyakan parameter sesuai pilihan metode
    if (methodChoice == 1) {
        // Jika memilih Variance, kita butuh threshold variansi dan ukuran minimum blok
        std::cout << "Masukkan threshold variansi (nilai ambang batas): ";
        std::cin >> threshold;

        std::cout << "Masukkan ukuran minimum blok: ";
        std::cin >> minBlockSize;
    }
    else if (methodChoice == 2) {
        // Jika memilih Threshold Variance, hanya butuh threshold
        std::cout << "Masukkan threshold variansi (nilai ambang batas): ";
        std::cin >> threshold;
    }
    else if (methodChoice == 3) {
        // Jika memilih Ukuran Minimum Blok, hanya butuh ukuran minimum blok
        std::cout << "Masukkan ukuran minimum blok: ";
        std::cin >> minBlockSize;
    }

    // --- Test Memuat Gambar --- 
    std::string inputFilename = "xiao.png";  // Pastikan ada file ini
    Image testImg;
    try {
        testImg = Image::loadFromFile(inputFilename); // Gunakan factory method
        std::cout << "PASS: loadFromFile successful for '" << inputFilename << "' (" << testImg.getWidth() << "x" << testImg.getHeight() << ")" << std::endl;
    } catch (const ImageError& e) {
        std::cout << "FAIL: loadFromFile failed: " << e.what() << std::endl;
        return -1;  // Keluar jika gambar gagal dimuat
    }

    // --- Membuat QuadTree dan Membagi Blok --- 
    int width = testImg.getWidth();
    int height = testImg.getHeight();

    // Membuat root node untuk QuadTree
    QuadTreeNode* rootNode = new QuadTreeNode(0, 0, width, height);

    // Panggil divideBlock untuk membangun QuadTree sesuai dengan pilihan metode
    divideBlock(testImg, rootNode, threshold, minBlockSize, methodChoice);
    
    std::cout << "Proses pembagian blok dengan metode " << methodChoice << " selesai!" << std::endl;

    // Tes dan cetak hasil divideBlock
    std::vector<QuadTreeNode*> nodes;
    collectNodes(rootNode, nodes);  // Fungsi untuk mengumpulkan semua node QuadTree

    std::cout << "Total nodes terbentuk: " << nodes.size() << std::endl;

    // Menampilkan informasi dari beberapa node pertama sebagai tes
    for (size_t i = nodes.size() - 1; i > nodes.size() - 6; --i) {
        std::cout << "Node " << i << " -> Position: (" << nodes[i]->x << ", " << nodes[i]->y 
                  << "), Size: (" << nodes[i]->width << "x" << nodes[i]->height 
                  << "), Leaf: " << (nodes[i]->isLeaf ? "Yes" : "No") << std::endl;
    }

    return 0;
}
