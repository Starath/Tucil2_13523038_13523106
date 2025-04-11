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

namespace fs = std::filesystem;

int main() {
    IOHandler ioHandler;

    std::string inputFilename, outputfilePath;
    Image queryImg, resultImg;
    ErrorMetric metric;
    float threshold = 0;
    int minBlockSize = 0;
    double inputSize = 0, resultSize = 0;
    int treeDepth = -1;
    size_t nodeCount = 0;

    try {
        // Input dari pengguna 
        inputFilename = ioHandler.promptForInputPath();
        ioHandler.displayMessage("File Gambar Yang Akan di Proses : " + inputFilename);

        metric = ioHandler.promptForErrorMetric();
        threshold = ioHandler.promptForThreshold();
        minBlockSize = ioHandler.promptForMinBlockSize();
        outputfilePath = ioHandler.promptForOutputPath();

        auto startTime = std::chrono::high_resolution_clock::now();

        bool isOutputGif = false;
        fs::path outPathObj(outputfilePath);
         if (outPathObj.has_extension() && outPathObj.extension().string() == ".gif") {
             isOutputGif = true;
        }

        if (isOutputGif) {
            // Alur pembuatan GIF
            ioHandler.displayMessage("Output adalah GIF, membuat frame...");
            std::string outputFolderPath = "frames_temp";
             if (fs::exists(outputFolderPath)) {
                 ioHandler.displayError("Warning: Folder '" + outputFolderPath + "' sudah ada. Menghapusnya...");
                 MakeFrame::deleteFramesFolder(outputFolderPath);
             }
            MakeFrame::createFrames(inputFilename, outputFolderPath, threshold, minBlockSize, 10);
            MakeGif::create(outputFolderPath, outputfilePath);
            MakeFrame::deleteFramesFolder(outputFolderPath);
            ioHandler.displayMessage("GIF berhasil dibuat: " + outputfilePath);

        } else {
            // Alur kompresi gambar
            queryImg = Image::loadFromFile(inputFilename);
            ioHandler.displayMessage("File Berhasil di Load!!");

            Quadtree qt(queryImg, metric, threshold, minBlockSize);
            resultImg = qt.reconstructImage();
            resultImg.saveImage(outputfilePath);

            treeDepth = qt.getDepth();
            nodeCount = qt.getNodeCount();

            ioHandler.displayMessage("Gambar Berhasil Di Kompresi Pada Alamat: " + outputfilePath);
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> execTime = endTime - startTime;

        // Perhitungan ukuran file 
        std::ifstream fileInputSize(inputFilename, std::ios::binary | std::ios::ate);
        if (fileInputSize) {
            inputSize = static_cast<double>(fileInputSize.tellg()) / 1024.0;
        } else {
             ioHandler.displayError("Warning: Tidak dapat membuka file input untuk mengukur ukuran.");
        }

        std::ifstream fileOutputSize(outputfilePath, std::ios::binary | std::ios::ate);
         if (fileOutputSize) {
            resultSize = static_cast<double>(fileOutputSize.tellg()) / 1024.0;
        } else {
            ioHandler.displayError("Warning: Tidak dapat membuka file output untuk mengukur ukuran.");
        }

        ioHandler.displayStatistics(execTime.count(), inputSize, resultSize, treeDepth, nodeCount, isOutputGif);


    } catch (const ImageError& e) {
        ioHandler.displayError("Error Gambar: " + std::string(e.what()));
        return 1; // Kode error: Masalah terkait gambar
    } catch (const std::exception& e) {
        ioHandler.displayError("Error Umum: " + std::string(e.what()));
        return 2; // Kode error: Masalah umum runtime
    } catch (...) {
        ioHandler.displayError("Terjadi error yang tidak diketahui.");
        return 3; // Kode error: Tidak diketahui
    }

    return 0; 
}
