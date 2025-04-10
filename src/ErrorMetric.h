#ifndef ERRORMETRIC_H
#define ERRORMETRIC_H

// Enum untuk metrik error yang digunakan dalam QuadTree
enum class ErrorMetric {
    VARIANCE,
    MAD,
    MAX_PIXEL_DIFFERENCE,
    ENTROPY,
    SSIM
};

#endif // ERRORMETRIC_H
