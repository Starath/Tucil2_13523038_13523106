#include "QuadTree.h"
#include <cmath>        
#include <vector>
#include <numeric>      
#include <limits>       
#include <stdexcept>    
#include <iostream>     
#include <algorithm>    
#include <array>        
#include <unordered_map>

QuadTreeNode::QuadTreeNode(int x, int y, int width, int height, const Image& image)
    : x(x), y(y), width(width), height(height), leaf(true), sourceImage(image), children({}) // Inisialisasi children
{
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("Node dimensions must be positive.");
    }
    try {
        averageColor = calculateAverageColor();
    } catch (const std::exception& e) {
         std::cerr << "Error calculating initial average color for node (" << x << "," << y << "): " << e.what() << std::endl;
         averageColor = Pixel(0, 0, 0);
    }
}

// Menghitung warna rata-rata untuk region node ini
Pixel QuadTreeNode::calculateAverageColor() const {
    if (width <= 0 || height <= 0) {
        return Pixel(0, 0, 0);
    }

    long long sumR = 0, sumG = 0, sumB = 0;
    long long count = 0;

    for (int i = y; i < y + height; ++i) {
        for (int j = x; j < x + width; ++j) {
            try {
                if (i >= 0 && i < sourceImage.getHeight() && j >= 0 && j < sourceImage.getWidth()) {
                    Pixel p = sourceImage.getPixel(i, j);
                    sumR += p.r;
                    sumG += p.g;
                    sumB += p.b;
                    count++;
                } else {
                     std::cerr << "Warning: Skipping pixel outside source image bounds during average calculation (" << j << ", " << i << ")\n";
                }
            } catch (const std::out_of_range& oor) {
                 std::cerr << "Warning: Out of range access in calculateAverageColor at (" << j << ", " << i << "): " << oor.what() << std::endl;
            }
        }
    }

    if (count == 0) {
        std::cerr << "Warning: No valid pixels found to calculate average color for node (" << x << "," << y << " " << width << "x" << height << "). Returning black.\n";
        return Pixel(0, 0, 0);
    }

    // Lakukan pembulatan standar (tambah setengah sebelum pembagian integer)
    unsigned char avgR = static_cast<unsigned char>((sumR + count / 2) / count);
    unsigned char avgG = static_cast<unsigned char>((sumG + count / 2) / count);
    unsigned char avgB = static_cast<unsigned char>((sumB + count / 2) / count);

    return Pixel(avgR, avgG, avgB);
}

double QuadTreeNode::calculateError(ErrorMetric metric) const {
     try {
        switch (metric) {
            case ErrorMetric::VARIANCE:
                return calculateVarianceInternal(sourceImage, x, y, width, height);
            case ErrorMetric::MAD:
                return calculateMADInternal(sourceImage, x, y, width, height);
            case ErrorMetric::MAX_PIXEL_DIFFERENCE:
                return calculateMaxPixelDifferenceInternal(sourceImage, x, y, width, height);
            case ErrorMetric::ENTROPY:
                return calculateEntropyInternal(sourceImage, x, y, width, height);
            case ErrorMetric::SSIM:
                return calculateSSIMInternal(sourceImage, x, y, width, height);
            default:
                throw std::runtime_error("Unsupported error metric selected.");
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception during error calculation for node (" << x << "," << y << " " << width << "x" << height << "): " << e.what() << std::endl;
        return std::numeric_limits<double>::infinity();
    }
}

// Implementasi fungsi perhitungan error internal (statis)
double QuadTreeNode::calculateVarianceInternal(const Image& img, int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return 0.0;
    long long totalPixels = static_cast<long long>(w) * h;
    if (totalPixels == 0) return 0.0;

    double meanR = 0, meanG = 0, meanB = 0;
    // Hitung mean
    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) {
            Pixel p = img.getPixel(i, j);
            meanR += p.r;
            meanG += p.g;
            meanB += p.b;
        }
    }
    meanR /= totalPixels;
    meanG /= totalPixels;
    meanB /= totalPixels;

    double varianceR = 0, varianceG = 0, varianceB = 0;
    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) {
            Pixel p = img.getPixel(i, j);
            varianceR += std::pow(p.r - meanR, 2);
            varianceG += std::pow(p.g - meanG, 2);
            varianceB += std::pow(p.b - meanB, 2);
        }
    }
    return ((varianceR / totalPixels) + (varianceG / totalPixels) + (varianceB / totalPixels)) / 3.0;
}

double QuadTreeNode::calculateMADInternal(const Image& img, int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return 0.0;
    long long totalPixels = static_cast<long long>(w) * h;
     if (totalPixels == 0) return 0.0;

    double meanR = 0, meanG = 0, meanB = 0;
    // Hitung mean
    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) {
             Pixel p = img.getPixel(i, j);
             meanR += p.r;
             meanG += p.g;
             meanB += p.b;
        }
    }
    meanR /= totalPixels;
    meanG /= totalPixels;
    meanB /= totalPixels;

    double madR = 0, madG = 0, madB = 0;
     for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) {
            Pixel p = img.getPixel(i, j);
            madR += std::abs(p.r - meanR);
            madG += std::abs(p.g - meanG);
            madB += std::abs(p.b - meanB);
        }
    }
    return ((madR / totalPixels) + (madG / totalPixels) + (madB / totalPixels)) / 3.0;
}

double QuadTreeNode::calculateMaxPixelDifferenceInternal(const Image& img, int x, int y, int w, int h) {
     if (w <= 0 || h <= 0) return 0.0;

    int minR = 255, minG = 255, minB = 255;
    int maxR = 0, maxG = 0, maxB = 0;
    bool firstPixel = true;

    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) {
            Pixel p = img.getPixel(i, j);
            if (firstPixel) {
                minR = maxR = p.r;
                minG = maxG = p.g;
                minB = maxB = p.b;
                firstPixel = false;
            } else {
                minR = std::min(minR, static_cast<int>(p.r));
                minG = std::min(minG, static_cast<int>(p.g));
                minB = std::min(minB, static_cast<int>(p.b));
                maxR = std::max(maxR, static_cast<int>(p.r));
                maxG = std::max(maxG, static_cast<int>(p.g));
                maxB = std::max(maxB, static_cast<int>(p.b));
            }
        }
    }
    if(firstPixel) return 0.0; 
    return static_cast<double>((maxR - minR) + (maxG - minG) + (maxB - minB)) / 3.0;
}

double QuadTreeNode::calculateEntropyInternal(const Image& img, int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return 0.0;
    long long totalPixels = static_cast<long long>(w) * h;
    if (totalPixels == 0) return 0.0;

    std::array<int, 256> freqR = {0}, freqG = {0}, freqB = {0};

    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) {
            Pixel p = img.getPixel(i, j);
            freqR[p.r]++;
            freqG[p.g]++;
            freqB[p.b]++;
        }
    }

    double entropyR = 0.0, entropyG = 0.0, entropyB = 0.0;
    for (int k = 0; k < 256; ++k) {
        if (freqR[k] > 0) {
            double probR = static_cast<double>(freqR[k]) / totalPixels;
            entropyR -= probR * std::log2(probR); 
        }
         if (freqG[k] > 0) {
            double probG = static_cast<double>(freqG[k]) / totalPixels;
            entropyG -= probG * std::log2(probG);
        }
         if (freqB[k] > 0) {
            double probB = static_cast<double>(freqB[k]) / totalPixels;
            entropyB -= probB * std::log2(probB);
        }
    }

    return (entropyR + entropyG + entropyB) / 3.0;
}

double QuadTreeNode::calculateSSIMInternal(const Image& img, int x, int y, int w, int h) const {
    if (w <= 0 || h <= 0) return 1.0;
    long long totalPixels = static_cast<long long>(w) * h;
    if (totalPixels == 0) return 1.0;

    const double K1 = 0.01;
    const double K2 = 0.03;
    const int L = 255;
    const double C1 = (K1 * L) * (K1 * L);
    const double C2 = (K2 * L) * (K2 * L);

    double meanOrigR = 0, meanOrigG = 0, meanOrigB = 0;
    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) {
            Pixel p = img.getPixel(i, j);
            meanOrigR += p.r;
            meanOrigG += p.g;
            meanOrigB += p.b;
        }
    }
    meanOrigR /= totalPixels;
    meanOrigG /= totalPixels;
    meanOrigB /= totalPixels;

    double meanCompR = averageColor.r;
    double meanCompG = averageColor.g;
    double meanCompB = averageColor.b;

    // Hitung variansi dan kovarians
    double varOrigR = 0, varOrigG = 0, varOrigB = 0;
    double varCompR = 0, varCompG = 0, varCompB = 0;
    double covR = 0, covG = 0, covB = 0;

    for (int i = y; i < y + h; ++i) {
        for (int j = x; j < x + w; ++j) {
            Pixel p = img.getPixel(i, j);

            double diffOrigR = p.r - meanOrigR;
            double diffOrigG = p.g - meanOrigG;
            double diffOrigB = p.b - meanOrigB;

            double diffCompR = averageColor.r - meanCompR;
            double diffCompG = averageColor.g - meanCompG;
            double diffCompB = averageColor.b - meanCompB;

            varOrigR += diffOrigR * diffOrigR;
            varOrigG += diffOrigG * diffOrigG;
            varOrigB += diffOrigB * diffOrigB;

            varCompR += diffCompR * diffCompR;
            varCompG += diffCompG * diffCompG;
            varCompB += diffCompB * diffCompB;

            covR += diffOrigR * diffCompR;
            covG += diffOrigG * diffCompG;
            covB += diffOrigB * diffCompB;
        }
    }

    varOrigR /= totalPixels;
    varOrigG /= totalPixels;
    varOrigB /= totalPixels;

    varCompR /= totalPixels;
    varCompG /= totalPixels;
    varCompB /= totalPixels;

    covR /= totalPixels;
    covG /= totalPixels;
    covB /= totalPixels;

    // Hitung SSIM untuk masing-masing channel
    double ssimR = ((2 * meanOrigR * meanCompR + C1) * (2 * covR + C2)) /
                   ((meanOrigR * meanOrigR + meanCompR * meanCompR + C1) * (varOrigR + varCompR + C2));

    double ssimG = ((2 * meanOrigG * meanCompG + C1) * (2 * covG + C2)) /
                   ((meanOrigG * meanOrigG + meanCompG * meanCompG + C1) * (varOrigG + varCompG + C2));

    double ssimB = ((2 * meanOrigB * meanCompB + C1) * (2 * covB + C2)) /
                   ((meanOrigB * meanOrigB + meanCompB * meanCompB + C1) * (varOrigB + varCompB + C2));

    // Rata-rata SSIM semua channel
    double avgSSIM = (ssimR + ssimG + ssimB) / 3.0;

    return 1.0 - avgSSIM; // error = 1 - SSIM
}



void QuadTreeNode::collectNodes(std::vector<const QuadTreeNode*>& nodes) const {
    nodes.push_back(this);
    if (!leaf) {
        for (int i = 0; i < 4; ++i) {
             if (children[i]) { 
                 children[i]->collectNodes(nodes);
             }
        }
    }
}

void QuadTreeNode::reconstructRegion(Image& targetImage) const {
    if (leaf) {
        // Isi area persegi panjang pada targetImage dengan averageColor
        int endY = std::min(y + height, targetImage.getHeight());
        int endX = std::min(x + width, targetImage.getWidth());
        int startY = std::max(y, 0);
        int startX = std::max(x, 0);

        for (int i = startY; i < endY; ++i) {
            for (int j = startX; j < endX; ++j) {
                 try {
                    targetImage.setPixel(i, j, averageColor);
                 } catch (const std::out_of_range&) {
                     std::cerr << "Warning: Reconstruction coordinate out of target bounds (" << j << ", " << i << ") despite checks.\n";
                 }
            }
        }
    } else {
        for (int i = 0; i < 4; ++i) {
             if (children[i]) { // Periksa apakah pointer valid
                 children[i]->reconstructRegion(targetImage);
             }
        }
    }
}


Quadtree::Quadtree(const Image& image, ErrorMetric metric, double threshold, int minSize)
    : sourceImage(image),
      imageWidth(image.getWidth()),
      imageHeight(image.getHeight()),
      errorMetricChoice(metric),
      errorThreshold(threshold),
      minimumBlockSize(std::max(1, minSize)), // minSize minimal 1
      nodeCount(0), 
      maxDepth(0)     
{
    if (image.isEmpty()) {
        throw std::runtime_error("Cannot create Quadtree from an empty image.");
    }
    if (imageWidth <= 0 || imageHeight <= 0) {
         throw std::runtime_error("Image dimensions must be positive.");
    }

    try {
        rootNode = std::make_unique<QuadTreeNode>(0, 0, imageWidth, imageHeight, sourceImage);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Failed to create root node: ") + e.what());
    }


    if(rootNode) {
        buildRecursive(rootNode.get(), 1);
    }

    if (nodeCount == 0 && rootNode) {
        nodeCount = 1;
        maxDepth = 1;
    }
}

void Quadtree::buildRecursive(QuadTreeNode* node, int currentDepth) {
    if (!node) {
         std::cerr << "Error: buildRecursive called with null node." << std::endl;
         return;
    }

    this->nodeCount++;
    this->maxDepth = std::max(this->maxDepth, currentDepth);

    double error = 0.0;
    bool shouldCheckError = true;
    bool canDividePhysically = (node->width > 1 || node->height > 1);
    long long currentArea = static_cast<long long>(node->width) * node->height;
    if (currentArea < this->minimumBlockSize || !canDividePhysically) {
        shouldCheckError = false;
    }

    if (shouldCheckError) {
         try {
            error = node->calculateError(this->errorMetricChoice);
         } catch (const std::exception& e){
             std::cerr << "Error calculating metric during buildRecursive: " << e.what() << std::endl;
             // Anggap error tinggi agar tidak memproses lebih lanjut jika ada masalah
             error = std::numeric_limits<double>::infinity();
         }
    }

    long long areaAfterDivideRough = (currentArea + 3) / 4;

    if (error <= this->errorThreshold || currentArea <= this->minimumBlockSize || !canDividePhysically || areaAfterDivideRough < this->minimumBlockSize) {
        node->leaf = true;
        return; // Node ini menjadi leaf
    }

    node->leaf = false;

    int halfWidth = node->width / 2;
    int halfHeight = node->height / 2;
    int widthRem = node->width - halfWidth;
    int heightRem = node->height - halfHeight;

    if (halfWidth == 0 && widthRem == 0) widthRem = 1;
    if (halfHeight == 0 && heightRem == 0) heightRem = 1;

    try {
        if (halfWidth > 0 && halfHeight > 0)
            node->children[0] = std::make_unique<QuadTreeNode>(node->x, node->y, halfWidth, halfHeight, node->sourceImage);
        if (widthRem > 0 && halfHeight > 0)
            node->children[1] = std::make_unique<QuadTreeNode>(node->x + halfWidth, node->y, widthRem, halfHeight, node->sourceImage);
        if (halfWidth > 0 && heightRem > 0)
            node->children[2] = std::make_unique<QuadTreeNode>(node->x, node->y + halfHeight, halfWidth, heightRem, node->sourceImage);
        if (widthRem > 0 && heightRem > 0)
            node->children[3] = std::make_unique<QuadTreeNode>(node->x + halfWidth, node->y + halfHeight, widthRem, heightRem, node->sourceImage);

        for (int i = 0; i < 4; ++i) {
            if (node->children[i]) {
                buildRecursive(node->children[i].get(), currentDepth + 1);
            }
        }
    } catch (const std::exception& e) {
         std::cerr << "Error creating child nodes for node (" << node->x << "," << node->y << "): " << e.what() << ". Making it a leaf." << std::endl;
         node->leaf = true;
         node->children = {};
    }
}

Image Quadtree::reconstructImage() const {
    if (!rootNode) {
        std::cerr << "Warning: reconstructImage called on uninitialized Quadtree. Returning empty image." << std::endl;
        return Image(0, 0);
    }
    Image reconstructed(imageWidth, imageHeight);
    rootNode->reconstructRegion(reconstructed);
    return reconstructed;
}

void Quadtree::getAllNodes(std::vector<const QuadTreeNode*>& nodes) const {
    nodes.clear();
     if (rootNode) {
        rootNode->collectNodes(nodes);
     }
}

int Quadtree::getDepth() const { return maxDepth; }
size_t Quadtree::getNodeCount() const { return nodeCount; }