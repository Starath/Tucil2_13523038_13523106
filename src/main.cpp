#include "QuadTree.h"
#include "Image.h"
#include "GifCompressor.h"
#include "MakeGif.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;

int main() {
    string inputFilename, outputfilePath;
    Image queryImg, resultImg;
    int methodChoice;
    double inputSize = 0, resultSize = 0;
    float threshold = 0;
    int minBlockSize = 0;

    // Load Gambar
    cout << "Masukkan Alamat Gambar (ex: image.png / image.jpg / image.gif) : ";
    cin >> inputFilename;
    cout << "File Gambar Yang Akan di Proses : " << inputFilename << endl;

    // Deteksi ekstensi input
    bool isGif = false;
    if (inputFilename.size() >= 4) {
        string ext = inputFilename.substr(inputFilename.size() - 4);
        for (auto& c : ext) c = tolower(c);
        if (ext == ".gif") {
            isGif = true;
        }
    }

    // Input metode kompresi
    cout << "Pilih Metode Perhitungan Error: " << endl;
    cout << "1. Variance" << endl;
    cout << "2. Mean Absolute Deviation (MAD)" << endl;
    cout << "3. Max Pixel Difference" << endl;
    cout << "4. Entropy" << endl;
    cout << "5. SSIM" << endl;
    cout << "Pilih (1/2/3/4/5) : ";
    cin >> methodChoice;

    cout << "Masukkan ambang batas (threshold variansi): ";
    cin >> threshold;
    cout << "Masukkan ukuran minimum Blok: ";
    cin >> minBlockSize;
    cout << "Masukan Alamat Gambar Hasil Kompresi: ";
    cin >> outputfilePath;

    // Mapping metode
    ErrorMetric metric;
    if (methodChoice == 1) {
        metric = ErrorMetric::VARIANCE;
    } else if (methodChoice == 2) {
        metric = ErrorMetric::MAD;
    } else if (methodChoice == 3) {
        metric = ErrorMetric::MAX_PIXEL_DIFFERENCE;
    } else if (methodChoice == 4) {
        metric = ErrorMetric::ENTROPY;
    } else if (methodChoice == 5) {
        metric = ErrorMetric::SSIM;
    } else {
        cout << "Input Pilihan Tidak Valid!" << endl;
        return -1;
    }

    auto startTime = chrono::high_resolution_clock::now();

    if (isGif) {
        try {
            cout << "Input adalah GIF, memproses multi-frame..." << endl;
            GifCompressor::compressGif(inputFilename, outputfilePath, metric, threshold, minBlockSize);
            string outputGif = "hasil_kompres.gif";
            MakeGif::create(outputfilePath, outputGif);
        } catch (const std::exception& e) {
            cerr << "Error saat kompres GIF: " << e.what() << endl;
            return -1;
        }
    } else {
        try {
            queryImg = Image::loadFromFile(inputFilename);
            cout << "File Berhasil di Load!!" << endl;

            Quadtree qt(queryImg, metric, threshold, minBlockSize);
            resultImg = qt.reconstructImage();
            resultImg.saveImage(outputfilePath);

            cout << "Gambar Berhasil Di Kompresi Pada Alamat: " << outputfilePath << endl;

            cout << "Kedalaman Pohon : " << qt.getDepth() << endl;
            cout << "Banyak simpul pada pohon : " << qt.getNodeCount() << endl;
        }
        catch (const ImageError& e) {
            cerr << "Gagal, Gambar tidak valid / tidak ditemukan: " << e.what() << endl;
            return -1;
        }
        catch (const std::exception& e) {
            cerr << "Error: " << e.what() << endl;
            return -1;
        }
    }

    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> execTime = endTime - startTime;

    // Hitung ukuran file
    ifstream fileInputSize(inputFilename, ios::binary | ios::ate);
    inputSize = static_cast<double>(fileInputSize.tellg()) / 1024.0;

    ifstream fileOutputSize(outputfilePath, ios::binary | ios::ate);
    resultSize = static_cast<double>(fileOutputSize.tellg()) / 1024.0;

    // Output statistik
    cout << fixed;
    cout.precision(2);
    cout << "Waktu Eksekusi : " << execTime.count() << " ms" << endl;
    cout << "Ukuran Gambar sebelum : " << inputSize << " KB" << endl;
    cout << "Ukuran Gambar sesudah : " << resultSize << " KB" << endl;
    cout << "Persentase Kompresi : " << 100.0 - ((resultSize / inputSize) * 100.0) << "%" << endl;

    return 0;
}
