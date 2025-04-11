#include "IOHandler.h"
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <vector>

void IOHandler::clearInputBuffer() const {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string IOHandler::promptForInputPath() {
    std::string filePath;
    const std::vector<std::string> supportedExtensions = {
        ".jpg", ".jpeg", ".png"
    };

    while (true) {
        std::cout << "1. Masukkan Alamat Absolut Gambar Input (contoh: C:/Users/Nama/Pictures/gambar.png): ";
        std::getline(std::cin >> std::ws, filePath);
        try {
            fs::path inputPath(filePath);

            if (!inputPath.is_absolute()) {
                displayError("Path input harus berupa path absolut.");
                continue;
            }


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
                        std::string supportedExtStr;
                        for(size_t i = 0; i < supportedExtensions.size(); ++i) {
                            supportedExtStr += supportedExtensions[i] + (i == supportedExtensions.size() - 1 ? "" : ", ");
                        }
                        displayError("Ekstensi file input '" + inputPath.extension().string() + "' tidak didukung. Gunakan salah satu dari: " + supportedExtStr);
                    }
                } else {
                     displayError("File tidak memiliki ekstensi yang dikenali.");
                }

            } else {
                displayError("File tidak ditemukan atau bukan file reguler di path: " + filePath);
            }
        } catch (const fs::filesystem_error& e) {
             displayError("Filesystem error saat memeriksa path input: " + std::string(e.what()));
        } catch (const std::exception& e) {
             displayError("Error saat memproses path input: " + std::string(e.what()));
        }
    }
}

std::string IOHandler::promptForOutputPath() {
    std::string filePath;
     const std::vector<std::string> supportedOutputExtensions = {
        ".png", ".jpg", ".jpeg"
    };

    while (true) {
        std::cout << "6. Masukkan Alamat Absolut Gambar Hasil Kompresi (ekstensi: .png, .jpg, .jpeg): ";
        std::getline(std::cin >> std::ws, filePath);
        try {
            fs::path outputPath(filePath);

            if (!outputPath.is_absolute()) {
                displayError("Path input harus berupa path absolut.");
                continue;
            }

            if (!outputPath.has_filename() || outputPath.filename().empty() || outputPath.filename() == "." || outputPath.filename() == "..") {
                 displayError("Nama file output tidak valid atau kosong.");
                 continue;
            }

             if (!outputPath.has_extension()) {
                 displayError("Nama file output harus memiliki ekstensi (contoh: .png, .jpg).");
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
                 std::string supportedExtStr;
                 for(size_t i = 0; i < supportedOutputExtensions.size(); ++i) {
                     supportedExtStr += supportedOutputExtensions[i] + (i == supportedOutputExtensions.size() - 1 ? "" : ", ");
                 }
                 displayError("Ekstensi file output '" + outputPath.extension().string() + "' tidak didukung untuk kompresi gambar. Gunakan: " + supportedExtStr);
                 continue;
             }


            if (outputPath.has_parent_path()) {
                 if (!outputPath.parent_path().empty() && !fs::exists(outputPath.parent_path())) {
                    displayError("Direktori induk untuk path output tidak ditemukan: " + outputPath.parent_path().string());
                    continue;
                 } else if (!outputPath.parent_path().empty() && !fs::is_directory(outputPath.parent_path())) {
                    displayError("Path induk untuk output bukanlah direktori: " + outputPath.parent_path().string());
                    continue;
                 }
            }
            return filePath;

        } catch (const fs::filesystem_error& e) {
            displayError("Filesystem error saat memeriksa path output: " + std::string(e.what()));
        } catch (const std::exception& e) {
             displayError("Error saat memproses path output: " + std::string(e.what()));
        }
    }
}

std::string IOHandler::promptForGifOutputPath() {
    std::string filePath;
    const std::string gifExtension = ".gif";

    while (true) {
        std::cout << "7. Masukkan Alamat Absolut Output GIF [Bonus] (opsional, tekan Enter untuk skip): ";
        std::getline(std::cin, filePath);

        if (filePath.empty()) {
            displayMessage("Output GIF dilewati.");
            return "";
        }

        try {
            fs::path outputPath(filePath);

            if (!outputPath.is_absolute()) {
                displayError("Path input harus berupa path absolut.");
                continue;
            }

            if (!outputPath.has_filename() || outputPath.filename().empty() || outputPath.filename() == "." || outputPath.filename() == "..") {
                 displayError("Nama file GIF tidak valid atau kosong.");
                 continue;
            }

            if (!outputPath.has_extension()) {
                 displayError("Nama file GIF harus memiliki ekstensi .gif");
                 continue;
            }
             std::string ext = outputPath.extension().string();
             std::transform(ext.begin(), ext.end(), ext.begin(),
                            [](unsigned char c){ return std::tolower(c); });

            if (ext != gifExtension) {
                 displayError("Ekstensi file output GIF harus .gif");
                 continue;
            }

            if (outputPath.has_parent_path()) {
                 if (!outputPath.parent_path().empty() && !fs::exists(outputPath.parent_path())) {
                    displayError("Direktori induk untuk path output GIF tidak ditemukan: " + outputPath.parent_path().string());
                    continue;
                 } else if (!outputPath.parent_path().empty() && !fs::is_directory(outputPath.parent_path())) {
                    displayError("Path induk untuk output GIF bukanlah direktori: " + outputPath.parent_path().string());
                    continue;
                 }
            }
            return filePath;

        } catch (const fs::filesystem_error& e) {
            displayError("Filesystem error saat memeriksa path output GIF: " + std::string(e.what()));
        } catch (const std::exception& e) {
             displayError("Error saat memproses path output GIF: " + std::string(e.what()));
        }
    }
}

ErrorMetric IOHandler::promptForErrorMetric() {
    int choice;
    std::cout << "2. Pilih Metode Perhitungan Error: " << std::endl;
    std::cout << "   1. Variance" << std::endl;
    std::cout << "   2. Mean Absolute Deviation (MAD)" << std::endl;
    std::cout << "   3. Max Pixel Difference" << std::endl;
    std::cout << "   4. Entropy" << std::endl;
    std::cout << "   5. SSIM (Bonus)" << std::endl;

    while (true) {
        std::cout << "   Pilih (1-5): ";
        if (std::cin >> choice && choice >= 1 && choice <= 5) {
            clearInputBuffer();
            switch (choice) {
                case 1: selectedMetric = ErrorMetric::VARIANCE; break;
                case 2: selectedMetric = ErrorMetric::MAD; break;
                case 3: selectedMetric = ErrorMetric::MAX_PIXEL_DIFFERENCE; break;
                case 4: selectedMetric = ErrorMetric::ENTROPY; break;
                case 5: selectedMetric = ErrorMetric::SSIM; break;
                
            }
            return selectedMetric; 
        } else {
            displayError("   Masukan tidak valid. Harap masukkan angka bulat antara 1 dan 5.");
            std::cin.clear(); 
            clearInputBuffer(); 
        }
    }
}

float IOHandler::promptForThreshold() {
    float value;
    std::string metricName;
    float min_val = 0.0; 
    float max_val = std::numeric_limits<float>::max(); 

    if (selectedMetric == ErrorMetric::VARIANCE) {
        metricName = "Variance";
        min_val = 0.0;
        max_val = 1000.0;
    } else if (selectedMetric == ErrorMetric::MAD) {
        metricName = "MAD";
        min_val = 0.0;
        max_val = 255.0;
    } else if (selectedMetric == ErrorMetric::MAX_PIXEL_DIFFERENCE) {
        metricName = "Max Pixel Difference";
        min_val = 0.0;
        max_val = 255.0;
    } else if (selectedMetric == ErrorMetric::ENTROPY) {
        metricName = "Entropy";
        min_val = 0.0;
        max_val = 8.0; 
    } else if (selectedMetric == ErrorMetric::SSIM) {
        metricName = "SSIM Error (1-SSIM)";
        min_val = 0.0;
        max_val = 1.0; 
    } else {
        metricName = "Unknown Metric";
    }

    std::cout << std::fixed << std::setprecision(4); 
    std::cout << "   (Rentang valid untuk " << metricName << " adalah [" << min_val << ".." << max_val << "])" << std::endl;
    std::cout << std::defaultfloat; 

    while (true) {
        std::cout << "3. Masukkan ambang batas (threshold): ";
        if (std::cin >> value) {
            if (value >= min_val && value <= max_val) {
                clearInputBuffer(); 
                return value;       
            } else {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(4);
                ss << "   Masukan tidak valid. Threshold untuk " << metricName
                   << " harus berada dalam rentang [" << min_val << ".." << max_val << "].";
                displayError(ss.str());
                clearInputBuffer(); 
            }
        } else {
            
            displayError("   Masukan tidak valid. Harap masukkan angka.");
            std::cin.clear(); 
            clearInputBuffer();
        }
    }
}

int IOHandler::promptForMinBlockSize() {
    int value;
    while (true) {
        std::cout << "4. Masukkan ukuran minimum Blok (luas piksel, >= 1): ";
        if (std::cin >> value && value >= 1) {
            clearInputBuffer();
            return value;
        } else {
            displayError("   Masukan tidak valid. Harap masukkan angka bulat >= 1.");
            std::cin.clear();
            clearInputBuffer();
        }
    }
}

float IOHandler::promptForTargetCompressionPercentage() {
    float value;
    while (true) {
        std::cout << "5. Masukkan Target Rasio Kompresi [Bonus] (0 nonaktif, 0 < target < 1.0): ";
        if (std::cin >> value && value >= 0.0f && value < 1.0f) {
            clearInputBuffer();
            return value;
        } else {
            displayError("   Masukan tidak valid. Harap masukkan angka antara 0.0 (inklusif) dan 1.0 (eksklusif).");
            std::cin.clear();
            clearInputBuffer();
        }
    }
}

void IOHandler::displayStatistics(double execTime,
                                  double inputSizeKB,
                                  double compressedImageSizeKB,
                                  double finalOutputSizeKB,
                                  int depth,
                                  size_t nodeCount) const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n---------------- HASIL ---------------" << std::endl;
    std::cout << "8. Waktu Eksekusi       : " << execTime << " ms" << std::endl;

    if (inputSizeKB > 0) {
        std::cout << "9. Ukuran Sebelum       : " << inputSizeKB << " KB (Original)" << std::endl;
    } else {
        std::cout << "9. Ukuran Sebelum       : Tidak dapat diukur." << std::endl;
    }

    if (finalOutputSizeKB > 0) {
        std::cout << "10. Ukuran Output Akhir : " << finalOutputSizeKB << " KB (" << "Gambar" << ")" << std::endl;
    } else {
         std::cout << "10. Ukuran Output Akhir : Tidak dapat diukur." << std::endl;
    }

    if (inputSizeKB > 0 && compressedImageSizeKB > 0) {
        if (inputSizeKB > 0.001) { // Hindari pembagian dengan nol atau nilai yang sangat kecil
            if (inputSizeKB >= compressedImageSizeKB) {
                double compressionRatio = (inputSizeKB - compressedImageSizeKB) / inputSizeKB;
                double percentage = compressionRatio * 100.0;
                std::cout << "11. Persentase Kompresi : " << percentage << "% (Original vs Gambar Kompresi)" << std::endl;
            } else {
                double increaseRatio = (compressedImageSizeKB - inputSizeKB) / inputSizeKB;
                double percentage = increaseRatio * 100.0;
                std::cout << "11. Persentase Kompresi : -" << percentage << "% (Ukuran bertambah)" << std::endl;
            }
        } else {
            std::cout << "11. Persentase Kompresi : Tidak dapat dihitung (ukuran original terlalu kecil)." << std::endl;
        }
    } else {
         std::cout << "11. Persentase Kompresi : Tidak dapat dihitung (ukuran input/gambar kompresi tidak valid)." << std::endl;
    }


    if (depth >= 0) {
        std::cout << "12. Kedalaman Pohon      : " << depth << std::endl;
    } else {
            std::cout << "12. Kedalaman Pohon      : N/A" << std::endl;
    }
    if (nodeCount > 0) {
        std::cout << "13. Jumlah Simpul Pohon  : " << nodeCount << std::endl;
    } else {
            std::cout << "13. Jumlah Simpul Pohon  : N/A" << std::endl;
    }
    std::cout << "--------------------------------------" << std::endl;
}

void IOHandler::displayMessage(const std::string& message) const {
    std::cout << "[INFO] " << message << std::endl;
}

void IOHandler::displayError(const std::string& errorMessage) const {
    std::cerr << "[ERROR] " << errorMessage << std::endl;
}