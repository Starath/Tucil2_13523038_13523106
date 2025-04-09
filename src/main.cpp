#include "QuadTree.h"
#include "Image.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono> 

// 1. [INPUT] alamat absolut gambar yang akan dikompresi.
// 2. [INPUT] metode perhitungan error (gunakan penomoran sebagai input).
// 3. [INPUT] ambang batas (pastikan range nilai sesuai dengan metode yang dipilih).
// 4. [INPUT] ukuran blok minimum.
// 5. [INPUT] Target persentase kompresi (floating number, 1.0 = 100%), beri nilai 0 jika ingin menonaktifkan mode ini, jika mode ini aktif maka nilai threshold bisa menyesuaikan secara otomatis untuk memenuhi target persentase kompresi (bonus).
// 6. [INPUT] alamat absolut gambar hasil kompresi.
// 7. [INPUT] alamat absolut gif (bonus).
// 8. [OUTPUT] waktu eksekusi.
// 9. [OUTPUT] ukuran gambar sebelum.
// 10. [OUTPUT] ukuran gambar setelah.
// 11. [OUTPUT] persentase kompresi.
// 12. [OUTPUT] kedalaman pohon.
// 13. [OUTPUT] banyak simpul pada pohon.
// 14. [OUTPUT] gambar hasil kompresi pada alamat yang sudah ditentukan.
// 15. [OUTPUT] GIF proses kompresi pada alamat yang sudah ditentukan (bonus).
using namespace std;

int main(){
    string inputFilename, outputfilePath;
    Image queryImg, resultImg;
    int methodChoice;
    double inputSize, resultSize;
    float threshold = 0;
    int minBlockSize = 0;

    // Load Gambar
    cout << "Masukkan Alamat Gambar (ex : image.png / image.jpg) : ";
    cin >> inputFilename;
    cout << "File Gambar Yang Akan di Proses : " << inputFilename << endl;
    try {
        queryImg = Image::loadFromFile (inputFilename);
        cout << "File Berhasil di Load!!" << endl;
    }
    catch (const ImageError& e){
        cout << "Gagal, Gambar tidak valid / tidak di temukan "<< endl << e.what() << endl;
        return -1;
    }

    // Input
    cout << "Pilih Metode Perhitungan Error : " << endl;
    cout << "1. Variance" << endl;
    cout << "2. Mean Absolute Deviation (MAD)" << endl;
    cout << "3. Max Pixel Difference" << endl;
    cout << "4. Entropy" << endl;
    cout << "Pilih (1/2/3/4) : ";
    cin >> methodChoice; 

    
    cout << "Masukkan ambang batas (threshold variansi) : ";
    cin >> threshold;
    cout << "Masukkan ukuran minimum Blok: ";
    cin >> minBlockSize;
    cout << "Masukan Alamat Gambar Hasil Kompresi : ";
    cin >> outputfilePath;

    // proses kompresi
    ErrorMetric metric;
    if (methodChoice == 1){
        metric = ErrorMetric::VARIANCE;
    }
    else if(methodChoice == 2){
        metric = ErrorMetric::MAD;
    }
    else if(methodChoice == 3){
        metric = ErrorMetric::MAX_PIXEL_DIFFERENCE;
    }
    else if(methodChoice == 4){
        metric = ErrorMetric::ENTROPY;
    }
    else {
        cout << "Input Pilihan Tidak Valid!" << endl;
        return -1;
    }

    auto startTime = chrono::high_resolution_clock::now();

    Quadtree qt(queryImg, metric, threshold, minBlockSize);
    
    resultImg = qt.reconstructImage();

    resultImg.saveImage(outputfilePath);

    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> execTime = endTime - startTime; // dalam ms

    ifstream fileInputSize(inputFilename, ios::binary | ios::ate);
    inputSize = static_cast<size_t>(fileInputSize.tellg()) / 1024.0;

    ifstream file(outputfilePath, ios::binary | ios::ate);
    resultSize = static_cast<size_t>(file.tellg()) / 1024.0;
    
    // output

    cout << "Waktu Eksekusi : "<< execTime.count() <<" ms" << endl;
    cout << "Ukuran Gambar sebelum : " << inputSize <<" KB" << endl;
    cout << "Ukuran Gambar sesudah : " << resultSize <<" KB" << endl;
    cout << "Persentase Kompresi : " << 100 - ((resultSize / inputSize) * 100) << "%" << endl;
    cout << "Kedalaman Pohon : " << qt.getDepth() << endl;
    cout << "Banyak simpul pada pohon : " << qt.getNodeCount() <<endl;
    cout << "Gambar Berhasil Di Kompresi Pada Alamat : " << outputfilePath << endl;


    return 0;
}