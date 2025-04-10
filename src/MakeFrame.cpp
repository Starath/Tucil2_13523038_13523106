#include "MakeFrame.h"
#include <iostream>
#include <filesystem>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;

// Fungsi untuk membuat frame berdasarkan threshold dan minBlock yang meningkat
void MakeFrame::createFrames(const std::string& inputImagePath, const std::string& outputFolderPath,
                              float threshold, int minBlockSize, int frameCount) {
    // Memuat gambar asli
    Image inputImage = Image::loadFromFile(inputImagePath);
    std::cout << "Gambar berhasil dimuat!" << std::endl;

    // Membuat folder untuk menyimpan frame
    if (!fs::exists(outputFolderPath)) {
        fs::create_directory(outputFolderPath);
    }

     // Mengecek apakah file frame_0.png sudah ada
     std::string originalFileName = outputFolderPath + "/frame_0.png";
     if (!fs::exists(originalFileName)) {
         // Jika file tidak ada, salin gambar original sebagai frame_0.png
         try {
             fs::copy(inputImagePath, originalFileName);
             std::cout << "Gambar original berhasil disalin sebagai frame_0.png." << std::endl;
         } catch (const std::exception& e) {
             std::cerr << "Error saat menyalin gambar original: " << e.what() << std::endl;
         }
     } else {
         std::cout << "frame_0.png sudah ada di folder, gambar original tidak disalin ulang." << std::endl;
     }

    // Menghitung peningkatan progres untuk threshold dan minBlock
    float thresholdStep = (threshold > 0) ? (threshold / frameCount ) : 1;
    int minBlockStep = (minBlockSize > 0) ? (minBlockSize / frameCount) : 1;

    // Membuat setiap frame
    for (int i = 0; i < frameCount; ++i) {  // Mulai dari frame_1, karena frame_0 adalah gambar original
        // Menentukan threshold dan minBlock untuk frame ini
        float currentThreshold = thresholdStep * (i + 1);
        int currentMinBlockSize = minBlockStep * (i + 1);

        // Menampilkan info frame yang sedang diproses
        std::cout << "Membuat frame " << i + 1 << " dengan threshold: " << currentThreshold
                  << " dan minBlock: " << currentMinBlockSize << std::endl;

        // Proses kompresi gambar dengan nilai threshold dan minBlock yang berbeda-beda
        Quadtree qt(inputImage, ErrorMetric::VARIANCE, currentThreshold, currentMinBlockSize);
        Image resultImage = qt.reconstructImage();

        // Simpan frame
        saveFrame(resultImage, outputFolderPath, i);
    }

    std::cout << "Proses frame selesai!" << std::endl;
}

// Fungsi untuk menyimpan frame sebagai file gambar
void MakeFrame::saveFrame(const Image& img, const std::string& outputPath, int frameIndex) {
    std::stringstream frameName;
    frameName << outputPath << "/frame_" << frameIndex << ".png";

    // Simpan gambar sebagai PNG di folder output dengan nama frame_x.png
    img.saveImage(frameName.str());
}

void MakeFrame::deleteFramesFolder(const std::string& folderPath) {
    try {
        fs::remove_all(folderPath);  // Menghapus folder beserta isinya
        std::cout << "Folder " << folderPath << " berhasil dihapus." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error saat menghapus folder " << folderPath << ": " << e.what() << std::endl;
    }
}