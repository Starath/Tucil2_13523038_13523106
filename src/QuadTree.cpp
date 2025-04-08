#include "QuadTree.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include <vector>
#include <stdexcept>
#include <unordered_map>
using namespace std;


// Dummy function to calculate variance
float calculateVariance(const Image& image, int x, int y, int width, int height) {
    float meanR = 0, meanG = 0, meanB = 0;
    float varianceR = 0, varianceG = 0, varianceB = 0;

    // Calculate mean for each color channel
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            try {
                Pixel p = image.getPixel(i, j);
                meanR += p.r;
                meanG += p.g;
                meanB += p.b;
            } catch (const std::out_of_range& oor) {
                // Pengaman
                std::cerr << "Error: Out of range access inside calculateVariance at (" << i << ", " << j << "): " << oor.what() << std::endl;
                return std::numeric_limits<float>::infinity(); // Error -> anggap variansi tinggi
            }
        }
    }

    int totalPixels = width * height;
    meanR /= totalPixels;
    meanG /= totalPixels;
    meanB /= totalPixels;

    // Calculate variance for each color channel
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            try {
                Pixel p = image.getPixel(i, j); 
                varianceR += pow(p.r - meanR, 2);
                varianceG += pow(p.g - meanG, 2);
                varianceB += pow(p.b - meanB, 2);
            } catch (const std::out_of_range& oor) {
                // Pengaman
                std::cerr << "Error: Out of range access inside calculateVariance at (" << i << ", " << j << "): " << oor.what() << std::endl;
                return std::numeric_limits<float>::infinity(); // Error -> anggap variansi sangat tinggi
            }
        }
    }

    return ((varianceR / totalPixels + varianceG / totalPixels + varianceB / totalPixels)/3); // Sum of variances for RGB
}

// Dummy function to calculate MAD (Mean Absolute Deviation)
float calculateMAD(const Image& image, int x, int y, int width, int height) {
    float meanR = 0, meanG = 0, meanB = 0;
    float madR = 0, madG = 0, madB = 0;

    // int counter = 0
    // Calculate mean for each color channel
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            try {

                Pixel p = image.getPixel(i, j); 
                meanR += p.r;
                meanG += p.g;
                meanB += p.b;
            } catch (const std::out_of_range& oor) {
                std::cerr << "Error: Out of range access inside calculateVariance at (" << i << ", " << j << "): " << oor.what() << std::endl;
                return std::numeric_limits<float>::infinity();
            }
        }
    }

    int totalPixels = width * height;
    meanR /= totalPixels;
    meanG /= totalPixels;
    meanB /= totalPixels;

    // Calculate MAD for each color channel
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            // int index = i * width + j;
            // madR += abs(pixelData[index].r - meanR);
            // madG += abs(pixelData[index].g - meanG);
            // madB += abs(pixelData[index].b - meanB);
            try {
                Pixel p = image.getPixel(i, j);
                madR += abs(p.r - meanR);
                madG += abs(p.g - meanG);
                madB += abs(p.b - meanB);
            } catch (const std::out_of_range& oor) {
                std::cerr << "Error: Out of range access inside calculateVariance at (" << i << ", " << j << "): " << oor.what() << std::endl;
                return std::numeric_limits<float>::infinity(); // Error -> anggap variansi sangat tinggi
            }
        }
    }

    return ((madR  / totalPixels + madG / totalPixels + madB / totalPixels) / 3); // Sum of MAD for RGB
}

// Dummy function to calculate Max Pixel Difference
float calculateMaxPixelDifference(const Image& image, int x, int y, int width, int height) {
    int maxR = 0, maxG = 0, maxB = 0;
    int minR = 255, minG = 255, minB = 255;

    // Find the max and min values for each color channel
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            // int index = i * width + j;
            // maxR = max(maxR, static_cast<int>(pixelData[index].r));
            // maxG = max(maxG, static_cast<int>(pixelData[index].g));
            // maxB = max(maxB, static_cast<int>(pixelData[index].b));
            
            // minR = min(minR, static_cast<int>(pixelData[index].r));
            // minG = min(minG, static_cast<int>(pixelData[index].g));
            // minB = min(minB, static_cast<int>(pixelData[index].b));
            try {
                Pixel p = image.getPixel(i, j);
                maxR = max(maxR, static_cast<int>(p.r));
                maxG = max(maxG, static_cast<int>(p.g));
                maxB = max(maxB, static_cast<int>(p.b));
                
                minR = min(minR, static_cast<int>(p.r));
                minG = min(minG, static_cast<int>(p.g));
                minB = min(minB, static_cast<int>(p.b));
            } catch (const std::out_of_range& oor) {
                std::cerr << "Error: Out of range access inside calculateVariance at (" << i << ", " << j << "): " << oor.what() << std::endl;
                return std::numeric_limits<float>::infinity();
            }

        }
    }

    return ((maxR - minR) + (maxG - minG) + (maxB - minB) / 3 ); 
}


float calculateEntropy(const Image& image, int x, int y, int width, int height) {
    // Histogram untuk kanal R, G, dan B
    std::unordered_map<int, int> histogramR, histogramG, histogramB;
    int totalPixels = 0;

    // Loop melalui setiap piksel dalam blok
    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            try {
                // Ambil data piksel
                Pixel p = image.getPixel(i, j);
                // Tambahkan nilai ke histogram masing-masing kanal
                histogramR[p.r]++;
                histogramG[p.g]++;
                histogramB[p.b]++;
                totalPixels++;
            } catch (const std::out_of_range& oor) {
                std::cerr << "Error: Out of range access in calculateEntropy at (" << i << ", " << j << "): " << oor.what() << std::endl;
                return std::numeric_limits<float>::infinity();
            }
        }
    }

    // Fungsi untuk menghitung entropi untuk satu kanal
    auto calculateChannelEntropy = [&](const std::unordered_map<int, int>& histogram) -> float {
        float entropy = 0.0f;
        for (const auto& [value, count] : histogram) {
            float probability = static_cast<float>(count) / totalPixels;
            if (probability > 0) {
                entropy -= probability * log2(probability);
            }
        }
        return entropy;
    };

    // Hitung entropi untuk kanal R, G, dan B
    float entropyR = calculateChannelEntropy(histogramR);
    float entropyG = calculateChannelEntropy(histogramG);
    float entropyB = calculateChannelEntropy(histogramB);

    // Rata-rata entropi dari ketiga kanal
    return (entropyR + entropyG + entropyB) / 3.0f;
}

// Fungsi untuk membagi blok menggunakan metode variansi
void divideBlockVariance(const Image& image, QuadTreeNode* node, float threshold, int minBlockSize) {
    float error = calculateVariance(image, node->x, node->y, node->width, node->height);
    
    if (error > threshold && (node->width * node->height) / 4  > minBlockSize) {
        int halfWidth = node->width / 2;
        int halfHeight = node->height / 2;

        node->isLeaf = false;

        node->children[0] = new QuadTreeNode(node->x, node->y, halfWidth, halfHeight);
        node->children[1] = new QuadTreeNode(node->x + halfWidth, node->y, halfWidth, halfHeight);
        node->children[2] = new QuadTreeNode(node->x, node->y + halfHeight, halfWidth, halfHeight);
        node->children[3] = new QuadTreeNode(node->x + halfWidth, node->y + halfHeight, halfWidth, halfHeight);

        divideBlockVariance(image, node->children[0], threshold, minBlockSize);
        divideBlockVariance(image, node->children[1], threshold, minBlockSize);
        divideBlockVariance(image, node->children[2], threshold, minBlockSize);
        divideBlockVariance(image, node->children[3], threshold, minBlockSize);
    }
}

// Fungsi untuk membagi blok menggunakan metode MAD (Mean Absolute Deviation)
void divideBlockMAD(const Image& image, QuadTreeNode* node, float threshold, int minBlockSize) {
    float error = calculateMAD(image, node->x, node->y, node->width, node->height);
    
    if (error > threshold && (node->width * node->height) / 4  > minBlockSize) {
        int halfWidth = node->width / 2;
        int halfHeight = node->height / 2;

        node->isLeaf = false;

        node->children[0] = new QuadTreeNode(node->x, node->y, halfWidth, halfHeight);
        node->children[1] = new QuadTreeNode(node->x + halfWidth, node->y, halfWidth, halfHeight);
        node->children[2] = new QuadTreeNode(node->x, node->y + halfHeight, halfWidth, halfHeight);
        node->children[3] = new QuadTreeNode(node->x + halfWidth, node->y + halfHeight, halfWidth, halfHeight);

        divideBlockMAD(image, node->children[0], threshold, minBlockSize);
        divideBlockMAD(image, node->children[1], threshold, minBlockSize);
        divideBlockMAD(image, node->children[2], threshold, minBlockSize);
        divideBlockMAD(image, node->children[3], threshold, minBlockSize);
    }
}

// Fungsi untuk membagi blok menggunakan metode Max Pixel Difference
void divideBlockMaxPixelDifference(const Image& image, QuadTreeNode* node, float threshold, int minBlockSize) {
    float error = calculateMaxPixelDifference(image, node->x, node->y, node->width, node->height);
    
    if (error > threshold && (node->width * node->height) / 4 > minBlockSize) {
        int halfWidth = node->width / 2;
        int halfHeight = node->height / 2;

        node->isLeaf = false;

        node->children[0] = new QuadTreeNode(node->x, node->y, halfWidth, halfHeight);
        node->children[1] = new QuadTreeNode(node->x + halfWidth, node->y, halfWidth, halfHeight);
        node->children[2] = new QuadTreeNode(node->x, node->y + halfHeight, halfWidth, halfHeight);
        node->children[3] = new QuadTreeNode(node->x + halfWidth, node->y + halfHeight, halfWidth, halfHeight);

        divideBlockMaxPixelDifference(image, node->children[0], threshold, minBlockSize);
        divideBlockMaxPixelDifference(image, node->children[1], threshold, minBlockSize);
        divideBlockMaxPixelDifference(image, node->children[2], threshold, minBlockSize);
        divideBlockMaxPixelDifference(image, node->children[3], threshold, minBlockSize);
    }
}

void divideBlockEntropy(const Image& image, QuadTreeNode* node, float threshold, int minBlockSize){
    float error = calculateEntropy(image, node->x, node->y, node->width, node->height);
    
    if (error > threshold && (node->width * node->height) / 4  > minBlockSize) {
        int halfWidth = node->width / 2;
        int halfHeight = node->height / 2;

        node->isLeaf = false;

        node->children[0] = new QuadTreeNode(node->x, node->y, halfWidth, halfHeight);
        node->children[1] = new QuadTreeNode(node->x + halfWidth, node->y, halfWidth, halfHeight);
        node->children[2] = new QuadTreeNode(node->x, node->y + halfHeight, halfWidth, halfHeight);
        node->children[3] = new QuadTreeNode(node->x + halfWidth, node->y + halfHeight, halfWidth, halfHeight);

        divideBlockEntropy(image, node->children[0], threshold, minBlockSize);
        divideBlockEntropy(image, node->children[1], threshold, minBlockSize);
        divideBlockEntropy(image, node->children[2], threshold, minBlockSize);
        divideBlockEntropy(image, node->children[3], threshold, minBlockSize);
    }
}

// Fungsi utama untuk memilih metode perhitungan error dan membagi blok gambar
void divideBlock(const Image& image, QuadTreeNode* node, float threshold, int minBlockSize, int methodChoice) {
    if (methodChoice == 1) {
        // Menggunakan Variance
        divideBlockVariance(image, node, threshold, minBlockSize);
    } 
    else if (methodChoice == 2) {
        // Menggunakan MAD
        divideBlockMAD(image, node, threshold, minBlockSize);
    } 
    else if (methodChoice == 3) {
        // Menggunakan Max Pixel Difference
        divideBlockMaxPixelDifference(image, node, threshold, minBlockSize);
    }
    else if (methodChoice == 4){
        divideBlockEntropy(image, node, threshold, minBlockSize);
    }
}

// Fungsi untuk mengumpulkan semua node dalam QuadTree
void collectNodes(QuadTreeNode* node, vector<QuadTreeNode*>& nodes) {
    if (node != nullptr) {
        nodes.push_back(node);  // Menyimpan node ke dalam vector
        
        if (!node->isLeaf) {
            for (int i = 0; i < 4; ++i) {
                collectNodes(node->children[i], nodes);  // Rekursi untuk setiap sub-blok
            }
        }
    }
}



