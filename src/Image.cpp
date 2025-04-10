#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <memory>  
#include <algorithm>
#include <cctype>   
using namespace std;

// Implementasi konstruktor
Image::Image(int w, int h) : width(w), height(h) {
    if (w <= 0 || h <= 0) {
        width = 0;
        height = 0;
        pixels.clear();
        throw std::invalid_argument("Image dimensions must be positive.");
    }
    pixels.resize(static_cast<std::size_t>(width) * height);
}

Image Image::loadFromFile(const std::string& filePath) {
    Image img;
    img.loadImage(filePath);
    return img;
}

// Implementasi loadImage
void Image::loadImage(const std::string& filePath) {
    int w, h, channels_in_file;

    auto deleter = [](unsigned char* data){ stbi_image_free(data); };
    std::unique_ptr<unsigned char, decltype(deleter)> rawData(
        stbi_load(filePath.c_str(), &w, &h, &channels_in_file, NumChannels),
        deleter
    );

    if (rawData == nullptr) {
        throw ImageError("Error loading image '" + filePath + "': " + stbi_failure_reason());
    }

    width = w;
    height = h;
    std::size_t totalPixels = static_cast<std::size_t>(width) * height;
    pixels.resize(totalPixels);

    const unsigned char* currentPixelData = rawData.get();
    for (std::size_t i = 0; i < totalPixels; ++i) {
        pixels[i].r = currentPixelData[0];
        pixels[i].g = currentPixelData[1];
        pixels[i].b = currentPixelData[2];
        currentPixelData += NumChannels;
    }

}

void Image::saveImage(const std::string& filePath, int jpgQuality) const {
    if (isEmpty()) {
        throw ImageError("Cannot save empty image.");
    }

    std::vector<unsigned char> rawOutputData(pixels.size() * NumChannels);
    for (std::size_t i = 0; i < pixels.size(); ++i) {
        rawOutputData[i * NumChannels + 0] = pixels[i].r;
        rawOutputData[i * NumChannels + 1] = pixels[i].g;
        rawOutputData[i * NumChannels + 2] = pixels[i].b;
    }

    std::string ext;
    std::size_t dotPos = filePath.rfind('.');
    if (dotPos != std::string::npos) {
        ext = filePath.substr(dotPos);
        // Ubah ke huruf kecil
        std::transform(ext.begin(), ext.end(), ext.begin(),
                       [](unsigned char c){ return std::tolower(c); });
    }

    int success = 0;
    if (ext == ".png") {
        success = stbi_write_png(filePath.c_str(), width, height, NumChannels, rawOutputData.data(), width * NumChannels);
    } else if (ext == ".bmp") {
        success = stbi_write_bmp(filePath.c_str(), width, height, NumChannels, rawOutputData.data());
    } else if (ext == ".jpg" || ext == ".jpeg") {
        // Pastikan kualitas valid
        if (jpgQuality < 1) jpgQuality = 1;
        if (jpgQuality > 100) jpgQuality = 100;
        success = stbi_write_jpg(filePath.c_str(), width, height, NumChannels, rawOutputData.data(), jpgQuality);
    } else if (ext == ".tga") {
        success = stbi_write_tga(filePath.c_str(), width, height, NumChannels, rawOutputData.data());
    } else {
        throw ImageError("Unsupported file extension '" + ext + "' for saving. Use .png, .bmp, .jpg, or .tga.");
    }

    if (success == 0) {
        throw ImageError("Failed to write image to '" + filePath + "'. Check path and permissions.");
    }
}

bool Image::checkBounds(int i, int j) const noexcept {
     return i < height && i >= 0 && j < width && j >= 0;
}

std::size_t Image::getIndex(int i, int j) const {
    return static_cast<std::size_t>(i) * width + j;
}


// Implementasi getPixel
Pixel Image::getPixel(int i, int j) const {
    if (!checkBounds(i, j)) {
        throw std::out_of_range("Pixel coordinates (" + std::to_string(j) + ", " + std::to_string(i) + ") are out of bounds ["+ std::to_string(width) + "i" + std::to_string(height) +"].");
    }
    return pixels[getIndex(i, j)];
}

// Implementasi setPixel
void Image::setPixel(int i, int j, const Pixel& p) {
    if (!checkBounds(i, j)) {
        throw std::out_of_range("Pixel coordinates (" + std::to_string(j) + ", " + std::to_string(i) + ") are out of bounds ["+ std::to_string(width) + "i" + std::to_string(height) +"].");
    }
    pixels[getIndex(i, j)] = p;
}

void Image::fill(const Pixel& p) {
    std::fill(pixels.begin(), pixels.end(), p);
}

int Image::getWidth() const noexcept {
    return width;
}

int Image::getHeight() const noexcept {
    return height;
}

std::size_t Image::getPixelCount() const noexcept {
    return (width > 0 && height > 0) ? pixels.size() : 0;
}

const std::vector<Pixel>& Image::getPixelData() const noexcept {
    return pixels;
}

std::vector<Pixel>& Image::getPixelData() noexcept {
    return pixels;
}

bool Image::isEmpty() const noexcept {
    return width <= 0 || height <= 0 || pixels.empty();
}