#include "Image.h"

// --- STB Implementation (tetap sama, idealnya di file terpisah) ---
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
// --------------------------

#include <iostream>   // Untuk std::cerr dalam kasus tertentu (meski idealnya logging)
#include <vector>
#include <memory>     // Untuk std::unique_ptr
#include <algorithm>  // Untuk std::transform, std::fill
#include <cctype>     // Untuk std::tolower

// Implementasi konstruktor
Image::Image(int w, int h) : width(w), height(h) {
    if (w <= 0 || h <= 0) {
        // Reset state ke kosong dan lempar exception
        width = 0;
        height = 0;
        pixels.clear();
        throw std::invalid_argument("Image dimensions must be positive.");
    }
    // Alokasi vector 1D dan inisialisasi dengan Pixel default (hitam)
    pixels.resize(static_cast<std::size_t>(width) * height);
}

// Implementasi factory method (delegasi ke loadImage)
Image Image::loadFromFile(const std::string& filePath) {
    Image img;
    img.loadImage(filePath); // loadImage akan melempar jika gagal
    return img;
}

// Implementasi loadImage
void Image::loadImage(const std::string& filePath) {
    int w, h, channels_in_file;

    // Menggunakan unique_ptr dengan custom deleter untuk RAII
    auto deleter = [](unsigned char* data){ stbi_image_free(data); };
    std::unique_ptr<unsigned char, decltype(deleter)> rawData(
        stbi_load(filePath.c_str(), &w, &h, &channels_in_file, NumChannels),
        deleter
    );

    if (rawData == nullptr) {
        // Gagal memuat, lempar exception
        throw ImageError("Error loading image '" + filePath + "': " + stbi_failure_reason());
    }

    width = w;
    height = h;
    std::size_t totalPixels = static_cast<std::size_t>(width) * height;
    pixels.resize(totalPixels); // Pastikan ukuran vector sesuai

    // Salin data (loop tetap cara paling aman dan jelas)
    const unsigned char* currentPixelData = rawData.get();
    for (std::size_t i = 0; i < totalPixels; ++i) {
        pixels[i].r = currentPixelData[0];
        pixels[i].g = currentPixelData[1];
        pixels[i].b = currentPixelData[2];
        currentPixelData += NumChannels;
    }

    // unique_ptr akan otomatis memanggil stbi_image_free saat keluar scope
}

// Implementasi saveImage
void Image::saveImage(const std::string& filePath, int jpgQuality) const {
    if (isEmpty()) {
        throw ImageError("Cannot save empty image.");
    }

    // Siapkan data mentah untuk STB write (buffer sementara)
    std::vector<unsigned char> rawOutputData(pixels.size() * NumChannels);
    for (std::size_t i = 0; i < pixels.size(); ++i) {
        rawOutputData[i * NumChannels + 0] = pixels[i].r;
        rawOutputData[i * NumChannels + 1] = pixels[i].g;
        rawOutputData[i * NumChannels + 2] = pixels[i].b;
    }

    // Tentukan format dari ekstensi file (lebih rapi)
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
        // Gagal menyimpan, mungkin karena permission atau path tidak valid
        throw ImageError("Failed to write image to '" + filePath + "'. Check path and permissions.");
    }
}

// Implementasi checkBounds (helper private)
bool Image::checkBounds(int x, int y) const noexcept {
     return x >= 0 && x < width && y >= 0 && y < height;
}

// Implementasi getIndex (helper private)
std::size_t Image::getIndex(int x, int y) const {
    // Asumsi bounds sudah dicek oleh pemanggil (getPixel/setPixel)
    return static_cast<std::size_t>(y) * width + x;
}


// Implementasi getPixel
Pixel Image::getPixel(int x, int y) const {
    if (!checkBounds(x, y)) {
        throw std::out_of_range("Pixel coordinates (" + std::to_string(x) + ", " + std::to_string(y) + ") are out of bounds ["+ std::to_string(width) + "x" + std::to_string(height) +"].");
    }
    return pixels[getIndex(x, y)];
}

// Implementasi setPixel
void Image::setPixel(int x, int y, const Pixel& p) {
    if (!checkBounds(x, y)) {
        throw std::out_of_range("Pixel coordinates (" + std::to_string(x) + ", " + std::to_string(y) + ") are out of bounds ["+ std::to_string(width) + "x" + std::to_string(height) +"].");
    }
    pixels[getIndex(x, y)] = p;
}

// Implementasi fill
void Image::fill(const Pixel& p) {
    std::fill(pixels.begin(), pixels.end(), p);
}

// Implementasi getter sederhana
int Image::getWidth() const noexcept {
    return width;
}

int Image::getHeight() const noexcept {
    return height;
}

std::size_t Image::getPixelCount() const noexcept {
    // Pastikan konsisten, return 0 jika width/height 0
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