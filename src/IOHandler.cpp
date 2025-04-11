#include "IOHandler.h"
#include <iostream>
#include <limits>    
#include <sstream>   
#include <stdexcept> 
#include <iomanip>  
#include <algorithm> 


void IOHandler::clearInputBuffer() const {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string IOHandler::promptForInputPath() {
    std::string filePath;
    // Daftar ekstensi yang didukung (dalam huruf kecil)
    const std::vector<std::string> supportedExtensions = {
        ".jpg", ".jpeg", ".png"
    };

    while (true) {
        std::cout << "Masukkan Alamat Absoulute Gambar Input: ";
        std::getline(std::cin >> std::ws, filePath);
        try {
            fs::path inputPath(filePath);

            if (fs::exists(inputPath) && fs::is_regular_file(inputPath)) {

                if (inputPath.has_extension()) {
                    std::string ext = inputPath.extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(),
                                   [](unsigned char c){ return std::tolower(c); });

                    bool found = false;
                    for(const auto& supportedExt : supportedExtensions) {
                        if (ext == supportedExt) {
                            found = true;
                            break;
                        }
                    }

                    if (found) {
                        return filePath;
                    } else {
                        displayError("Ekstensi file '" + inputPath.extension().string() + "' tidak didukung. Gunakan salah satu dari: .jpg, .jpeg, .png, .bmp, .tga, .psd, .gif, .hdr, .pic, .pnm, .pgm, .ppm");
                    }
                } else {
                     displayError("File tidak memiliki ekstensi yang dikenali.");
                }
            } else {
                displayError("File tidak ditemukan atau bukan file reguler di path: " + filePath);
            }
        } catch (const fs::filesystem_error& e) {
             displayError("Filesystem error: " + std::string(e.what()));
        }
    }
}

std::string IOHandler::promptForOutputPath() {
    std::string filePath;
     const std::vector<std::string> supportedOutputExtensions = {
        ".png", ".jpg", ".jpeg", ".gif"
    };

    while (true) {
        std::cout << "Masukkan Alamat Gambar Hasil Kompresi (direktori harus ada, ekstensi: .png, .bmp, .jpg, .tga, .gif): ";
        std::getline(std::cin >> std::ws, filePath);
        try {
            fs::path outputPath(filePath);

            if (!outputPath.has_filename() || outputPath.filename().empty()) {
                 displayError("Nama file output tidak boleh kosong.");
                 continue;
            }

             if (!outputPath.has_extension()) {
                 displayError("Nama file output harus memiliki ekstensi (contoh: .png, .jpg, .gif).");
                 continue;
             }
             std::string ext = outputPath.extension().string();
             std::transform(ext.begin(), ext.end(), ext.begin(),
                            [](unsigned char c){ return std::tolower(c); });

             bool outputExtSupported = false;
             for(const auto& supportedExt : supportedOutputExtensions) {
                 if (ext == supportedExt) {
                     outputExtSupported = true;
                     break;
                 }
             }
             if (!outputExtSupported) {
                 displayError("Ekstensi file output '" + outputPath.extension().string() + "' tidak didukung. Gunakan: .png, .bmp, .jpg, .jpeg, .tga, .gif");
                 continue;
             }


            if (outputPath.has_parent_path()) {
                if (!fs::exists(outputPath.parent_path())) {
                    displayError("Direktori induk untuk path output tidak ditemukan: " + outputPath.parent_path().string());
                    continue;
                }
                return filePath;
            } else {
                return filePath;
            }
        } catch (const fs::filesystem_error& e) {
            displayError("Filesystem error: " + std::string(e.what()));
        }
    }
}


ErrorMetric IOHandler::promptForErrorMetric() {
    int choice;
    std::cout << "Pilih Metode Perhitungan Error: " << std::endl;
    std::cout << "1. Variance" << std::endl;
    std::cout << "2. Mean Absolute Deviation (MAD)" << std::endl;
    std::cout << "3. Max Pixel Difference" << std::endl;
    std::cout << "4. Entropy" << std::endl;
    std::cout << "5. SSIM" << std::endl;

    while (true) {
        std::cout << "Pilih (1-5) : ";
        if (std::cin >> choice && choice >= 1 && choice <= 5) {
            clearInputBuffer();
            switch (choice) {
                case 1: return ErrorMetric::VARIANCE;
                case 2: return ErrorMetric::MAD;
                case 3: return ErrorMetric::MAX_PIXEL_DIFFERENCE;
                case 4: return ErrorMetric::ENTROPY;
                case 5: return ErrorMetric::SSIM;
                default: break;
            }
        } else {
            displayError("Masukan tidak valid. Harap masukkan angka bulat antara 1 dan 5.");
            std::cin.clear(); 
            clearInputBuffer(); 
        }
    }
}

float IOHandler::promptForThreshold() {
    double value; 
    while (true) {
        std::cout << "Masukkan ambang batas (threshold, >= 0): ";
        if (std::cin >> value && value >= 0.0) {
             clearInputBuffer(); 
            return static_cast<float>(value); 
        } else {
            displayError("Masukan tidak valid. Harap masukkan angka non-negatif.");
            std::cin.clear(); 
            clearInputBuffer(); 
        }
    }
}

int IOHandler::promptForMinBlockSize() {
    int value;
    while (true) {
        std::cout << "Masukkan ukuran minimum Blok (>= 1): ";
        if (std::cin >> value && value >= 1) {
            clearInputBuffer(); 
            return value;
        } else {
            displayError("Masukan tidak valid. Harap masukkan angka bulat >= 1.");
            std::cin.clear(); 
            clearInputBuffer(); 
        }
    }
}

void IOHandler::displayStatistics(double execTime, double inputSize, double resultSize,
                                  int depth, size_t nodeCount, bool isGifMode) const {
    std::cout << std::fixed << std::setprecision(2); 
    std::cout << "------------------------------------------" << std::endl;
    std::cout << "Waktu Eksekusi : " << execTime << " ms" << std::endl;

    if (inputSize > 0) {
        std::cout << "Ukuran Gambar sebelum : " << inputSize << " KB" << std::endl;
    } else {
        std::cout << "Ukuran Gambar sebelum : Tidak dapat diukur." << std::endl;
    }

    if (resultSize > 0) {
        std::cout << "Ukuran Gambar sesudah : " << resultSize << " KB" << std::endl;
        if (inputSize > 0) { 
            double compressionRatio = (resultSize / inputSize);
            double percentage = std::max(0.0, (1.0 - compressionRatio) * 100.0);
            std::cout << "Persentase Kompresi : " << percentage << "%" << std::endl;
        }
    } else {
         std::cout << "Ukuran Gambar sesudah : Tidak dapat diukur." << std::endl;
    }

    if (!isGifMode) {
        if (depth >= 0) {
            std::cout << "Kedalaman Pohon : " << depth << std::endl;
        }
        if (nodeCount > 0) {
            std::cout << "Banyak simpul pada pohon : " << nodeCount << std::endl;
        }
    }
    std::cout << "------------------------------------------" << std::endl;
}

void IOHandler::displayMessage(const std::string& message) const {
    std::cout << message << std::endl;
}

void IOHandler::displayError(const std::string& errorMessage) const {
    std::cerr << "Error: " << errorMessage << std::endl;
}
