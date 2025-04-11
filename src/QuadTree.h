#ifndef QUADTREE_H
#define QUADTREE_H

#include "Image.h" 
#include <vector>
#include <memory> 
#include <array>  
#include <string> 
#include <stdexcept> 
#include <limits>    

class Quadtree;

enum class ErrorMetric {
    VARIANCE,
    MAD, 
    MAX_PIXEL_DIFFERENCE,
    ENTROPY,
    SSIM
};

class QuadTreeNode {
private:
    // Data Anggota
    int x, y;                   
    int width, height;          
    Pixel averageColor;         
    bool leaf;                  
    std::array<std::unique_ptr<QuadTreeNode>, 4> children; 
    const Image& sourceImage;   

    Pixel calculateAverageColor() const;              
    double calculateError(ErrorMetric metric) const;  

    static double calculateVarianceInternal(const Image& img, int x, int y, int w, int h);
    static double calculateMADInternal(const Image& img, int x, int y, int w, int h);
    static double calculateMaxPixelDifferenceInternal(const Image& img, int x, int y, int w, int h);
    static double calculateEntropyInternal(const Image& img, int x, int y, int w, int h);
    double calculateSSIMInternal(const Image& img, int x, int y, int w, int h) const;

    friend class Quadtree;

public:
    QuadTreeNode(int x, int y, int width, int height, const Image& image);

    bool isLeaf() const { return leaf; }
    Pixel getAverageColor() const { return averageColor; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    const std::array<std::unique_ptr<QuadTreeNode>, 4>& getChildren() const { return children; }

    void collectNodes(std::vector<const QuadTreeNode*>& nodes) const;

    void reconstructRegion(Image& targetImage) const;
};


class Quadtree {
private:
    // Data Anggota
    std::unique_ptr<QuadTreeNode> rootNode; 
    const Image& sourceImage;               
    int imageWidth;                         
    int imageHeight;
    ErrorMetric errorMetricChoice;          
    double errorThreshold;                  
    int minimumBlockSize;                   

    size_t nodeCount; 
    int maxDepth;    

    void buildRecursive(QuadTreeNode* node, int currentDepth);

public:
    Quadtree(const Image& image, ErrorMetric metric, double threshold, int minSize);

    Image reconstructImage() const;

    int getDepth() const;
    size_t getNodeCount() const;

    const QuadTreeNode* getRoot() const { return rootNode.get(); }
    void getAllNodes(std::vector<const QuadTreeNode*>& nodes) const;

};

#endif