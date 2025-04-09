#ifndef GIFCOMPRESSOR_H
#define GIFCOMPRESSOR_H

#include "Image.h"
#include "QuadTree.h"
#include <string>
#include <vector>

class GifCompressor {
public:
    // Fungsi untuk mengompresi semua frame GIF
    static void compressGif(const std::string& inputGifPath, const std::string& outputFolderPath,
                             ErrorMetric metric, double threshold, int minBlockSize);

private:
    static void loadFramesFromGif(const std::string& gifPath, std::vector<Image>& frames);
    static void saveFramesAsPng(const std::string& outputFolderPath, const std::vector<Image>& frames);
    static std::vector<unsigned char> readFile(const std::string& filename);
};

#endif // GIFCOMPRESSOR_H
