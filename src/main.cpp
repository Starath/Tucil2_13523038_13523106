#include "QuadTree.h"
#include "Image.h"
#include "MakeGif.h"
#include "MakeFrame.h"
#include "IOHandler.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <filesystem>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <algorithm>

namespace fs = std::filesystem;

int main() {
    IOHandler ioHandler;

    std::string inputFilename, outputImageFilePath, outputGifFilePath;
    Image queryImg, resultImg;
    ErrorMetric metric;
    float initialThreshold = 0.0f;
    float finalThreshold = 0.0f;
    int minBlockSize = 1;
    float targetCompressionRatio = 0.0f;
    double inputSizeKB = 0.0;
    uintmax_t originalSizeBytes = 0;
    uintmax_t compressedImageSizeBytes = 0;
    uintmax_t finalOutputSizeBytes = 0;
    std::string outputImageExtension = ".png";
    const int jpgQuality = 85;

    double compressedImageSizeKB = 0.0;
    double finalOutputSizeKB = 0.0;

    int treeDepth = -1;
    size_t nodeCount = 0;
    double execTimeMs = 0.0;

    try {
        inputFilename = ioHandler.promptForInputPath();
        ioHandler.displayMessage("File Gambar Input: " + inputFilename);

        try {
             queryImg = Image::loadFromFile(inputFilename);
             ioHandler.displayMessage("File Berhasil di Load!!");
             originalSizeBytes = fs::file_size(inputFilename);
             if (originalSizeBytes == 0) {
                 ioHandler.displayError("Warning: Ukuran file input adalah 0.");
             }
             inputSizeKB = static_cast<double>(originalSizeBytes) / 1024.0;
        } catch (const ImageError& e) {
             ioHandler.displayError("Gagal memuat gambar input: " + std::string(e.what()));
             return 1;
        } catch (const fs::filesystem_error& e) {
             ioHandler.displayError("Gagal mendapatkan ukuran file input: " + std::string(e.what()));
             originalSizeBytes = 0;
             inputSizeKB = 0.0;
        }
         if (queryImg.isEmpty()) {
             ioHandler.displayError("Error: Gambar input gagal dimuat atau kosong.");
             return 1;
         }

        metric = ioHandler.promptForErrorMetric();
        initialThreshold = ioHandler.promptForThreshold();
        finalThreshold = initialThreshold;
        minBlockSize = ioHandler.promptForMinBlockSize();
        targetCompressionRatio = ioHandler.promptForTargetCompressionPercentage();
        outputImageFilePath = ioHandler.promptForOutputPath();

        fs::path outPathObj(outputImageFilePath);
        if (outPathObj.has_extension()) {
            outputImageExtension = outPathObj.extension().string();
            std::transform(outputImageExtension.begin(), outputImageExtension.end(), outputImageExtension.begin(),
                           [](unsigned char c){ return std::tolower(c); });
        } else {
             outputImageExtension = ".png";
             outputImageFilePath += outputImageExtension;
             ioHandler.displayMessage("Ekstensi output tidak ada, menggunakan default " + outputImageExtension);
        }

        outputGifFilePath = ioHandler.promptForGifOutputPath();

        if (targetCompressionRatio > 0.0f) {
            ioHandler.displayMessage("Mode target rasio kompresi aktif (" + std::to_string(targetCompressionRatio) + ").");

            if (!outputGifFilePath.empty()) {
                 ioHandler.displayError("Mode target rasio tidak didukung jika output GIF diminta.");
                 ioHandler.displayMessage("Menggunakan threshold manual: " + std::to_string(initialThreshold));
            } else if (originalSizeBytes == 0) {
                ioHandler.displayError("Ukuran file asli tidak valid. Tidak dapat menggunakan mode target.");
                ioHandler.displayMessage("Menggunakan threshold manual: " + std::to_string(initialThreshold));
            } else {
                double targetSizeRatio = 1.0 - targetCompressionRatio;
                uintmax_t targetSizeBytes = static_cast<uintmax_t>(static_cast<double>(originalSizeBytes) * targetSizeRatio);
                ioHandler.displayMessage("Ukuran file asli: " + std::to_string(originalSizeBytes) + " bytes (" + std::to_string(inputSizeKB) + " KB)");
                ioHandler.displayMessage("Target ukuran file: " + std::to_string(targetSizeBytes) + " bytes (Format: " + outputImageExtension + ")");

                float minTh = 0.0f;
                float maxTh = 50000.0f;
                const int maxIterations = 20;
                const float toleranceRatio = 0.05f;
                float bestTh = initialThreshold;
                uintmax_t closestSize = std::numeric_limits<uintmax_t>::max();
                long long minDiff = std::numeric_limits<long long>::max();
                const std::string tempFilename = "temp_compress_search" + outputImageExtension;

                ioHandler.displayMessage("Memulai pencarian threshold (maks " + std::to_string(maxIterations) + " iterasi) [" + std::to_string(minTh) + "..." + std::to_string(maxTh) + "]");

                for (int iter = 0; iter < maxIterations; ++iter) {
                    float midTh = minTh + (maxTh - minTh) / 2.0f;
                    uintmax_t currentSize = 0;
                    long long currentDiff = 0;

                    ioHandler.displayMessage("Iterasi " + std::to_string(iter + 1) + ": Mencoba threshold = " + std::to_string(midTh));

                    try {
                        Quadtree trialQt(queryImg, metric, midTh, minBlockSize);
                        Image trialImage = trialQt.reconstructImage();
                        trialImage.saveImage(tempFilename, jpgQuality);
                        currentSize = fs::file_size(tempFilename);
                        fs::remove(tempFilename);

                        currentDiff = static_cast<long long>(currentSize) - static_cast<long long>(targetSizeBytes);
                        long long absDiff = std::abs(currentDiff);

                        ioHandler.displayMessage("  Ukuran hasil (temp " + outputImageExtension + "): " + std::to_string(currentSize) + " bytes (Target: " + std::to_string(targetSizeBytes) + ", Selisih: " + std::to_string(currentDiff) + ")");

                        if (absDiff < minDiff) {
                            minDiff = absDiff;
                            closestSize = currentSize;
                            bestTh = midTh;
                            ioHandler.displayMessage("  -> Threshold terbaik sementara: " + std::to_string(bestTh) + " (Selisih Abs: " + std::to_string(minDiff) + ")");
                        }

                        if (absDiff <= static_cast<uintmax_t>(static_cast<double>(targetSizeBytes) * toleranceRatio)) {
                            ioHandler.displayMessage("  Target tercapai dalam toleransi.");
                            bestTh = midTh;
                            break;
                        }

                        if (currentSize > targetSizeBytes) {
                            ioHandler.displayMessage("  -> Ukuran > target, naikkan minThreshold.");
                            minTh = midTh;
                        } else {
                             ioHandler.displayMessage("  -> Ukuran < target, turunkan maxThreshold.");
                            maxTh = midTh;
                        }
                         if ((maxTh - minTh) < 0.01f) {
                             ioHandler.displayMessage("  Rentang pencarian sangat kecil, konvergen.");
                             break;
                         }

                    } catch (const std::exception& e) {
                        ioHandler.displayError("  Error saat kompresi/simpan percobaan: " + std::string(e.what()));
                        ioHandler.displayMessage("  Menghentikan pencarian karena error. Menggunakan threshold terbaik sejauh ini: " + std::to_string(bestTh));
                        break;
                    }
                }

                ioHandler.displayMessage("Pencarian selesai. Threshold final diatur ke: " + std::to_string(bestTh));
                finalThreshold = bestTh;
            }
        } else {
             ioHandler.displayMessage("Mode target rasio kompresi dinonaktifkan. Menggunakan threshold: " + std::to_string(initialThreshold));
        }


        auto startTime = std::chrono::high_resolution_clock::now();

        if (!outputGifFilePath.empty()) {
            ioHandler.displayMessage("Membuat frame untuk GIF...");
            std::string outputFolderPath = "frames_temp";
             if (fs::exists(outputFolderPath)) {
                 ioHandler.displayMessage("Membersihkan folder frame sementara...");
                 MakeFrame::deleteFramesFolder(outputFolderPath);
             }
            const int frameCountForGif = 10;
            MakeFrame::createFrames(inputFilename, outputFolderPath, finalThreshold, minBlockSize, frameCountForGif);

            std::string lastFramePathStr = outputFolderPath + "/frame_" + std::to_string(frameCountForGif) + ".png";
            fs::path lastFramePath(lastFramePathStr);
            if (fs::exists(lastFramePath)) {
                 try {
                     compressedImageSizeBytes = fs::file_size(lastFramePath);
                     compressedImageSizeKB = static_cast<double>(compressedImageSizeBytes) / 1024.0;
                     ioHandler.displayMessage("Ukuran gambar terkompresi (frame terakhir): " + std::to_string(compressedImageSizeKB) + " KB");
                 } catch (const fs::filesystem_error& e) {
                     ioHandler.displayError("Warning: Gagal mengukur ukuran frame terakhir: " + std::string(e.what()));
                     compressedImageSizeBytes = 0;
                     compressedImageSizeKB = 0.0;
                 }
            } else {
                 ioHandler.displayError("Warning: Frame terakhir tidak ditemukan untuk pengukuran ukuran.");
                 compressedImageSizeBytes = 0;
                 compressedImageSizeKB = 0.0;
            }

            ioHandler.displayMessage("Membuat file GIF...");
            MakeGif::create(outputFolderPath, outputGifFilePath);

            try {
                if (fs::exists(outputGifFilePath)) {
                    finalOutputSizeBytes = fs::file_size(lastFramePath);
                    finalOutputSizeKB = static_cast<double>(finalOutputSizeBytes) / 1024.0;
                } else {
                    ioHandler.displayError("Warning: File GIF output tidak ditemukan setelah pembuatan.");
                    finalOutputSizeBytes = 0;
                    finalOutputSizeKB = 0.0;
                }
            } catch (const fs::filesystem_error& e) {
                ioHandler.displayError("Warning: Tidak dapat mengukur ukuran file GIF output: " + std::string(e.what()));
                finalOutputSizeBytes = 0;
                finalOutputSizeKB = 0.0;
            }

            ioHandler.displayMessage("Menghapus frame sementara...");
            MakeFrame::deleteFramesFolder(outputFolderPath);
            resultImg.saveImage(outputImageFilePath, jpgQuality);

            treeDepth = -1;
            nodeCount = 0;

        } else {
             ioHandler.displayMessage("Melakukan kompresi gambar dengan threshold: " + std::to_string(finalThreshold));
            Quadtree finalQt(queryImg, metric, finalThreshold, minBlockSize);
            resultImg = finalQt.reconstructImage();

            treeDepth = finalQt.getDepth();
            nodeCount = finalQt.getNodeCount();

            resultImg.saveImage(outputImageFilePath, jpgQuality);
            ioHandler.displayMessage("Gambar Berhasil Dikompresi ke: " + outputImageFilePath);

            try {
                 if (fs::exists(outputImageFilePath)) {
                    compressedImageSizeBytes = fs::file_size(outputImageFilePath);
                    finalOutputSizeBytes = compressedImageSizeBytes;
                    compressedImageSizeKB = static_cast<double>(compressedImageSizeBytes) / 1024.0;
                    finalOutputSizeKB = compressedImageSizeKB;
                 } else {
                     ioHandler.displayError("Warning: File gambar output tidak ditemukan setelah penyimpanan.");
                     compressedImageSizeBytes = 0;
                     finalOutputSizeBytes = 0;
                     compressedImageSizeKB = 0.0;
                     finalOutputSizeKB = 0.0;
                 }

             } catch (const fs::filesystem_error& e) {
                 ioHandler.displayError("Warning: Tidak dapat membuka file gambar output untuk mengukur ukuran: " + std::string(e.what()));
                 compressedImageSizeBytes = 0;
                 finalOutputSizeBytes = 0;
                 compressedImageSizeKB = 0.0;
                 finalOutputSizeKB = 0.0;
             }
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = endTime - startTime;
        execTimeMs = duration.count();

        ioHandler.displayStatistics(execTimeMs, inputSizeKB, compressedImageSizeKB, finalOutputSizeKB, treeDepth, nodeCount);


    } catch (const ImageError& e) {
        ioHandler.displayError("Error Gambar: " + std::string(e.what()));
        return 1;
    } catch (const std::exception& e) {
        ioHandler.displayError("Error Umum: " + std::string(e.what()));
        return 2;
    } catch (...) {
        ioHandler.displayError("Terjadi error yang tidak diketahui.");
        return 3;
    }

    return 0;
}