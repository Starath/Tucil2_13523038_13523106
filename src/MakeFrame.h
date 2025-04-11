#ifndef MAKEFRAME_H
#define MAKEFRAME_H

#include <string>
#include "Image.h"
#include "QuadTree.h"
// #include "ErrorMetric.h"

class MakeFrame {
public:
    // Fungsi untuk membuat frame berdasarkan progress threshold dan minBlock
    static void createFrames(const std::string& inputImagePath, const std::string& outputFolderPath,
                             float threshold, int minBlockSize, int frameCount);
    static void deleteFramesFolder(const std::string& folderPath);
private:
    // Fungsi untuk menyimpan gambar sebagai frame di folder yang ditentukan
    static void saveFrame(const Image& img, const std::string& outputPath, int frameIndex);
    
};

#endif // MAKEFRAME_H
