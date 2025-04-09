#include "GifCompressor.h"
#include "stb_image.h"
#include "stb_image_write.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

void GifCompressor::compressGif(const std::string& inputGifPath, const std::string& outputFolderPath,
                                 ErrorMetric metric, double threshold, int minBlockSize) {
    std::vector<Image> frames;

    // Pastikan folder output ada
    if (!fs::exists(outputFolderPath)) {
        fs::create_directories(outputFolderPath);
    }

    // Load semua frame dari GIF
    loadFramesFromGif(inputGifPath, frames);

    std::vector<Image> compressedFrames;

    // Kompres tiap frame
    for (const Image& frame : frames) {
        Quadtree qt(frame, metric, threshold, minBlockSize);
        Image compressedFrame = qt.reconstructImage();
        compressedFrames.push_back(compressedFrame);
    }

    // Simpan hasil ke folder
    saveFramesAsPng(outputFolderPath, compressedFrames);
}

void GifCompressor::loadFramesFromGif(const std::string& gifPath, std::vector<Image>& frames) {
    int* delays = nullptr;
    int width, height, framesCount, comp;

    std::vector<unsigned char> fileData = readFile(gifPath);

    unsigned char* data = stbi_load_gif_from_memory(
        fileData.data(), fileData.size(), &delays, &width, &height, &framesCount, &comp, 4
    );

    if (!data) {
        throw std::runtime_error("Failed to load GIF frames.");
    }

    size_t frameSize = width * height * 4;
    for (int i = 0; i < framesCount; ++i) {
        Image frame(width, height);
        unsigned char* framePixels = data + i * frameSize;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int idx = (y * width + x) * 4;
                Pixel p(framePixels[idx], framePixels[idx+1], framePixels[idx+2]);
                frame.setPixel(y, x, p);
            }
        }

        frames.push_back(frame);
    }

    stbi_image_free(data);
    if (delays) free(delays);
}

void GifCompressor::saveFramesAsPng(const std::string& outputFolderPath, const std::vector<Image>& frames) {
    if (frames.empty()) {
        throw std::runtime_error("No frames to save.");
    }

    for (size_t i = 0; i < frames.size(); ++i) {
        std::string filename = outputFolderPath + "/frame_" + std::to_string(i) + ".png";
        frames[i].saveImage(filename);
        std::cout << "Saved compressed frame: " << filename << std::endl;
    }
}

std::vector<unsigned char> GifCompressor::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(file), {});
}
