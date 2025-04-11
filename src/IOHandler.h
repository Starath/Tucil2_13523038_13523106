#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <string>
#include <filesystem> // (C++17)
#include "QuadTree.h"

namespace fs = std::filesystem;

class IOHandler {
private:
    void clearInputBuffer() const;
public:
    // Meminta dan memvalidasi path file input/output dari pengguna.
    std::string promptForInputPath();
    std::string promptForOutputPath();


    ErrorMetric promptForErrorMetric();

    float promptForThreshold();

    int promptForMinBlockSize();

    void displayStatistics(double execTime, double inputSize, double resultSize,
                           int depth = -1, size_t nodeCount = 0, bool isGifMode = false) const;

    // Menampilkan pesan informasi ke pengguna.
    void displayMessage(const std::string& message) const;

    // Menampilkan pesan error
    void displayError(const std::string& errorMessage) const;

};

#endif
