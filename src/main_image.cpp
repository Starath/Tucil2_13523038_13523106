// File: main_image_test.cpp
// Driver program untuk menguji fungsionalitas kelas Image.

#include "Image.h"    // Sertakan header kelas Image yang sudah dibuat
#include <iostream>   // Untuk output konsol (std::cout, std::cerr)
#include <string>     // Untuk nama file
#include <stdexcept>  // Untuk menangkap exceptions (std::out_of_range, std::invalid_argument)
#include <vector>     // Tidak secara langsung digunakan di sini, tapi Image menggunakannya

// Fungsi helper untuk mencetak header tes
void printTestHeader(const std::string& testName) {
    std::cout << "\n--- Testing: " << testName << " ---" << std::endl;
}

// Fungsi helper untuk mencetak informasi piksel
void printPixel(const std::string& label, int x, int y, const Pixel& p) {
    std::cout << label << " (" << x << "," << y << "): "
              << "R=" << static_cast<int>(p.r)
              << ", G=" << static_cast<int>(p.g)
              << ", B=" << static_cast<int>(p.b) << std::endl;
}

int main() {
    std::cout << "Starting Image Class Test Driver..." << std::endl;
    int tests_passed = 0;
    int tests_failed = 0;

    // --- Test Konstruksi ---
    printTestHeader("Construction");
    try {
        // Test default constructor
        Image defaultImg;
        if (defaultImg.isEmpty() && defaultImg.getWidth() == 0 && defaultImg.getHeight() == 0) {
            std::cout << "PASS: Default construction successful." << std::endl; tests_passed++;
        } else {
            std::cout << "FAIL: Default construction state incorrect." << std::endl; tests_failed++;
        }

        // Test dimensioned constructor (valid)
        Image sizedImg(20, 15);
        if (!sizedImg.isEmpty() && sizedImg.getWidth() == 20 && sizedImg.getHeight() == 15 && sizedImg.getPixelCount() == 300) {
             std::cout << "PASS: Dimensioned construction (valid) successful." << std::endl; tests_passed++;
             // Check if pixels are default initialized (should be black {0,0,0})
             if (sizedImg.getPixel(0,0).r == 0 && sizedImg.getPixel(0,0).g == 0 && sizedImg.getPixel(0,0).b == 0) {
                 std::cout << "PASS: Dimensioned construction initializes pixels." << std::endl; tests_passed++;
             } else {
                 std::cout << "FAIL: Dimensioned construction did not initialize pixels correctly." << std::endl; tests_failed++;
             }
        } else {
             std::cout << "FAIL: Dimensioned construction (valid) state incorrect." << std::endl; tests_failed++;
        }

        // Test dimensioned constructor (invalid dimensions)
        try {
            Image invalidImgW(0, 5); // Lebar nol
            std::cout << "FAIL: Construction with zero width did not throw." << std::endl; tests_failed++;
        } catch (const std::invalid_argument& e) {
            std::cout << "PASS: Caught expected exception for zero width: " << e.what() << std::endl; tests_passed++;
        } catch (...) {
             std::cout << "FAIL: Caught unexpected exception type for zero width." << std::endl; tests_failed++;
        }

        try {
            Image invalidImgH(5, -1); // Tinggi negatif
            std::cout << "FAIL: Construction with negative height did not throw." << std::endl; tests_failed++;
        } catch (const std::invalid_argument& e) {
            std::cout << "PASS: Caught expected exception for negative height: " << e.what() << std::endl; tests_passed++;
        } catch (...) {
             std::cout << "FAIL: Caught unexpected exception type for negative height." << std::endl; tests_failed++;
        }

    } catch (const std::exception& e) {
         std::cout << "FAIL: Unexpected exception during construction tests: " << e.what() << std::endl; tests_failed++;
    }

    // --- Test Loading ---
    printTestHeader("Image Loading");
    Image testImg; // Gunakan objek ini untuk tes berikutnya
    const std::string inputFilename = "xiao.png"; // !!! PENTING: Anda harus menyediakan file ini !!!
    bool loadSuccess = false;                      // Untuk skip tes berikutnya jika load gagal

    // Coba load file yang valid (gunakan factory method)
    try {
        testImg = Image::loadFromFile(inputFilename); // Gunakan factory method
        if (!testImg.isEmpty() && testImg.getWidth() > 0 && testImg.getHeight() > 0) {
            std::cout << "PASS: loadFromFile successful for '" << inputFilename << "' (" << testImg.getWidth() << "x" << testImg.getHeight() << ")" << std::endl; tests_passed++;
            loadSuccess = true;

            // Mencetak seluruh data piksel dari gambar
            const std::vector<Pixel>& pixelData = testImg.getPixelData(); // Mengambil data piksel

            for (int y = 0; y < testImg.getHeight(); ++y) {
                for (int x = 0; x < testImg.getWidth(); ++x) {
                    Pixel p = testImg.getPixel(x, y);  // Mendapatkan piksel pada koordinat (x, y)
                    std::cout << "Pixel (" << x << "," << y << "): "
                            << "R=" << static_cast<int>(p.r) << ", "
                            << "G=" << static_cast<int>(p.g) << ", "
                            << "B=" << static_cast<int>(p.b) << std::endl;
                }
            }

        } else {
             std::cout << "FAIL: loadFromFile completed but image state is invalid." << std::endl; tests_failed++;
        }
    } catch (const ImageError& e) {
        std::cout << "FAIL: loadFromFile failed for potentially valid file '" << inputFilename << "': " << e.what() << std::endl; tests_failed++;
        std::cout << "** Pastikan file '" << inputFilename << "' ada di direktori yang sama atau berikan path yang benar. **" << std::endl;
    } catch (const std::exception& e) {
         std::cout << "FAIL: Unexpected exception during loadFromFile: " << e.what() << std::endl; tests_failed++;
    }

    // Coba load file yang tidak ada (gunakan metode anggota loadImage)
    try {
        Image nonExistentImg;
        nonExistentImg.loadImage("non_existent_file.xyz"); // Gunakan metode anggota
        std::cout << "FAIL: loadImage for non-existent file did not throw." << std::endl; tests_failed++;
    } catch (const ImageError& e) {
        std::cout << "PASS: Caught expected exception for non-existent file: " << e.what() << std::endl; tests_passed++;
    } catch (...) {
         std::cout << "FAIL: Caught unexpected exception type for non-existent file." << std::endl; tests_failed++;
    }


    // --- Test Pixel Manipulation (Hanya jika load berhasil) ---
    if (loadSuccess) {
        printTestHeader("Pixel Manipulation");
        try {
            // Test getPixel (valid)
            Pixel originalPixel = testImg.getPixel(0, 0);
            printPixel("Original Pixel", 0, 0, originalPixel);
            tests_passed++; // Anggap getPixel berhasil jika tidak melempar exception

            // Test setPixel (valid)
            Pixel newPixel = {255, 10, 20}; // Warna baru
            testImg.setPixel(1, 1, newPixel); // Ubah piksel di (1, 1)
            std::cout << "PASS: setPixel called without throwing." << std::endl; tests_passed++;

            // Verifikasi setPixel dengan getPixel
            Pixel changedPixel = testImg.getPixel(1, 1);
            printPixel("Changed Pixel", 1, 1, changedPixel);
            if (changedPixel.r == newPixel.r && changedPixel.g == newPixel.g && changedPixel.b == newPixel.b) {
                 std::cout << "PASS: getPixel correctly returned the value set by setPixel." << std::endl; tests_passed++;
            } else {
                 std::cout << "FAIL: getPixel did not return the value set by setPixel." << std::endl; tests_failed++;
            }

            // Test getPixel (out of bounds X)
            try {
                testImg.getPixel(testImg.getWidth(), 0); // X = width (invalid)
                std::cout << "FAIL: getPixel with invalid X did not throw std::out_of_range." << std::endl; tests_failed++;
            } catch (const std::out_of_range& e) {
                 std::cout << "PASS: Caught expected std::out_of_range for getPixel(X OOB): " << e.what() << std::endl; tests_passed++;
            } catch (...) { std::cout << "FAIL: Caught unexpected exception type for getPixel(X OOB)." << std::endl; tests_failed++; }

            // Test getPixel (out of bounds Y)
            try {
                testImg.getPixel(0, testImg.getHeight()); // Y = height (invalid)
                std::cout << "FAIL: getPixel with invalid Y did not throw std::out_of_range." << std::endl; tests_failed++;
            } catch (const std::out_of_range& e) {
                 std::cout << "PASS: Caught expected std::out_of_range for getPixel(Y OOB): " << e.what() << std::endl; tests_passed++;
            } catch (...) { std::cout << "FAIL: Caught unexpected exception type for getPixel(Y OOB)." << std::endl; tests_failed++; }


             // Test setPixel (out of bounds X)
            try {
                testImg.setPixel(-1, 0, {0,0,0}); // X negatif
                std::cout << "FAIL: setPixel with invalid X did not throw std::out_of_range." << std::endl; tests_failed++;
            } catch (const std::out_of_range& e) {
                 std::cout << "PASS: Caught expected std::out_of_range for setPixel(X OOB): " << e.what() << std::endl; tests_passed++;
            } catch (...) { std::cout << "FAIL: Caught unexpected exception type for setPixel(X OOB)." << std::endl; tests_failed++; }

            // Test setPixel (out of bounds Y)
            try {
                testImg.setPixel(0, testImg.getHeight(), {0,0,0}); // Y = height
                std::cout << "FAIL: setPixel with invalid Y did not throw std::out_of_range." << std::endl; tests_failed++;
            } catch (const std::out_of_range& e) {
                 std::cout << "PASS: Caught expected std::out_of_range for setPixel(Y OOB): " << e.what() << std::endl; tests_passed++;
            } catch (...) { std::cout << "FAIL: Caught unexpected exception type for setPixel(Y OOB)." << std::endl; tests_failed++; }


        } catch (const std::exception& e) {
            std::cout << "FAIL: Unexpected exception during pixel manipulation tests: " << e.what() << std::endl; tests_failed++;
        }
    } else {
         std::cout << "\n--- Skipping Pixel Manipulation Tests (Image Load Failed) ---" << std::endl;
    }


    // --- Test Fill and Saving ---
    printTestHeader("Fill and Saving");
    try {
        Image fillImg(100, 50); // Buat gambar baru
        Pixel fillColor = {20, 180, 220}; // Warna cyan terang
        fillImg.fill(fillColor);
        std::cout << "PASS: fill called without throwing." << std::endl; tests_passed++;

        // Verifikasi fill dengan getPixel
        Pixel checkPixel = fillImg.getPixel(fillImg.getWidth() / 2, fillImg.getHeight() / 2); // Ambil piksel tengah
        if (checkPixel.r == fillColor.r && checkPixel.g == fillColor.g && checkPixel.b == fillColor.b) {
             std::cout << "PASS: fill seems to work correctly (checked center pixel)." << std::endl; tests_passed++;
        } else {
             std::cout << "FAIL: fill did not set pixel color correctly." << std::endl; tests_failed++;
        }

        // Test saving ke berbagai format
        const char* formats[] = {"output_filled.png", "output_filled.bmp", "output_filled.jpg", "output_filled.tga"};
        for (const char* fname : formats) {
            try {
                fillImg.saveImage(fname, 92); // Kualitas 92 untuk JPG
                std::cout << "PASS: saveImage potentially successful for " << fname << "." << std::endl; tests_passed++;
                std::cout << "   ** Silakan periksa file '" << fname << "' secara visual. **" << std::endl;
            } catch (const ImageError& e) {
                std::cout << "FAIL: saveImage failed for " << fname << ": " << e.what() << std::endl; tests_failed++;
            }
        }

        // Simpan gambar yang sudah dimodifikasi (jika load berhasil)
        if (loadSuccess) {
             try {
                testImg.saveImage("output_modified.png");
                std::cout << "PASS: saveImage potentially successful for output_modified.png." << std::endl; tests_passed++;
                std::cout << "   ** Silakan periksa file 'output_modified.png' (pixel [1,1] harusnya berubah). **" << std::endl;
             } catch (const ImageError& e) {
                 std::cout << "FAIL: saveImage failed for modified image: " << e.what() << std::endl; tests_failed++;
             }
        }

        // Test save gambar kosong
        try {
            Image emptyImg;
            emptyImg.saveImage("output_empty.png");
            std::cout << "FAIL: saveImage for empty image did not throw." << std::endl; tests_failed++;
        } catch (const ImageError& e) {
            std::cout << "PASS: Caught expected exception for saving empty image: " << e.what() << std::endl; tests_passed++;
        } catch (...) {
             std::cout << "FAIL: Caught unexpected exception type for saving empty image." << std::endl; tests_failed++;
        }

        // Test save dengan ekstensi tidak didukung
        try {
            fillImg.saveImage("output_filled.webp"); // Format tidak didukung STB Write standar
            std::cout << "FAIL: saveImage for unsupported extension did not throw." << std::endl; tests_failed++;
        } catch (const ImageError& e) {
            std::cout << "PASS: Caught expected exception for unsupported extension: " << e.what() << std::endl; tests_passed++;
        } catch (...) {
             std::cout << "FAIL: Caught unexpected exception type for unsupported extension." << std::endl; tests_failed++;
        }

    } catch (const std::exception& e) {
        std::cout << "FAIL: Unexpected exception during fill/saving tests: " << e.what() << std::endl; tests_failed++;
    }


    // --- Ringkasan ---
    printTestHeader("Test Summary");
    std::cout << "Tests Passed: " << tests_passed << std::endl;
    std::cout << "Tests Failed: " << tests_failed << std::endl;
    std::cout << "\nImage Class Test Driver Finished." << std::endl;
    if (tests_failed == 0) {
         std::cout << "Semua tes dasar terlewati atau menangkap exception yang diharapkan." << std::endl;
    } else {
         std::cout << "BEBERAPA TES GAGAL!" << std::endl;
    }
    std::cout << "PENTING: Periksa file output ('output_*.png', '.bmp', '.jpg', '.tga') secara visual untuk verifikasi akhir." << std::endl;

    // Kembalikan 0 jika semua tes berhasil, 1 jika ada yang gagal
    return tests_failed > 0 ? 1 : 0;
}