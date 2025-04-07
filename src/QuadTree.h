#ifndef QUADTREE_H
#define QUADTREE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Image.h"

using namespace std;



// Node untuk QuadTree
struct QuadTreeNode {
    int x, y;                // Koordinat sudut kiri atas area
    int width, height;       // Ukuran area
    Pixel averageColor;      // Rata-rata warna pada blok ini
    bool isLeaf;             // Menentukan apakah node adalah daun atau tidak
    QuadTreeNode* children[4]; // Anak-anak dari node (4 sub-blok)

    QuadTreeNode(int x, int y, int width, int height)
        : x(x), y(y), width(width), height(height), isLeaf(true) {
        fill(begin(children), end(children), nullptr);
    }

    ~QuadTreeNode() {
        for (int i = 0; i < 4; ++i) {
            delete children[i]; // delete nullptr aman
            children[i] = nullptr;
        }
    }
};

// Fungsi untuk menghitung variansi
float calculateVariance(const Image& image, int x, int y, int width, int height);

// Fungsi untuk menghitung MAD (Mean Absolute Deviation)
float calculateMAD(const Image& image, int x, int y, int width, int height);

// Fungsi untuk menghitung Max Pixel Difference
float calculateMaxPixelDifference(const Image& image, int x, int y, int width, int height);

// Fungsi untuk membagi blok menggunakan metode variansi
void divideBlockVariance(const Image& image, QuadTreeNode* node, float threshold, int minBlockSize);

// Fungsi untuk membagi blok menggunakan metode MAD (Mean Absolute Deviation)
void divideBlockMAD(const Image& image, QuadTreeNode* node, float threshold, int minBlockSize);

// Fungsi untuk membagi blok menggunakan metode Max Pixel Difference
void divideBlockMaxPixelDifference(const Image& image, QuadTreeNode* node, float threshold, int minBlockSize);

// Fungsi utama untuk memilih metode perhitungan error dan membagi blok gambar
void divideBlock(const Image& image, QuadTreeNode* node, float threshold, int minBlockSize, int methodChoice);

// Fungsi untuk mengumpulkan semua node dalam QuadTree
void collectNodes(QuadTreeNode* node, std::vector<QuadTreeNode*>& nodes);

#endif // QUADTREE_H
