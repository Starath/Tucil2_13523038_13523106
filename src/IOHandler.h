#ifndef IOHANDLER_H
#define IOHANDLER_H

#include <string>
#include <filesystem>
#include "QuadTree.h"

namespace fs = std::filesystem;

class IOHandler {
private:
    void clearInputBuffer() const;

public:
    std::string promptForInputPath();
    std::string promptForOutputPath();
    std::string promptForGifOutputPath();
    ErrorMetric promptForErrorMetric();
    float promptForThreshold();
    int promptForMinBlockSize();
    float promptForTargetCompressionPercentage();

    void displayStatistics(double execTime,
                           double inputSizeKB,
                           double compressedImageSizeKB,
                           double finalOutputSizeKB,
                           int depth = -1,
                           size_t nodeCount = 0) const;

    void displayMessage(const std::string& message) const;
    void displayError(const std::string& errorMessage) const;
};

#endif