#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include <stdexcept>
#include <cstddef>

struct Pixel {
    unsigned char r = 0, g = 0, b = 0;

    Pixel() : r(0), g(0), b(0) {}
    Pixel(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
};

class ImageError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};


class Image {
private:
    int width = 0;
    int height = 0;
    std::vector<Pixel> pixels;

    bool checkBounds(int i, int j) const noexcept;
    // Helper untuk menghitung indeks 1D (juga tidak perlu publik)
    std::size_t getIndex(int i, int j) const;

public:
    static constexpr int NumChannels = 3;

    Image() noexcept = default;

    Image(int w, int h);

    static Image loadFromFile(const std::string& filePath);

    void loadImage(const std::string& filePath);

    void saveImage(const std::string& filePath, int jpgQuality = 85) const;

    Pixel getPixel(int i, int j) const;

    void setPixel(int i, int j, const Pixel& p);

    void fill(const Pixel& p);

    int getWidth() const noexcept;
    int getHeight() const noexcept;

    std::size_t getPixelCount() const noexcept;

    const std::vector<Pixel>& getPixelData() const noexcept;

    std::vector<Pixel>& getPixelData() noexcept;

    bool isEmpty() const noexcept;

};

#endif