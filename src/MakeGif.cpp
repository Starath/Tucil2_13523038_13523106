#include "MakeGif.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include "gif.h"
#include "stb_image.h"

namespace fs = std::filesystem;

void MakeGif::create(const std::string& folderPath, const std::string& outputGif) {
    std::vector<std::string> frameFiles;

    // Ambil semua file frame_*.png
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.find("frame_") == 0 && filename.substr(filename.size() - 4) == ".png") {
                frameFiles.push_back(entry.path().string());
            }
        }
    }

    if (frameFiles.empty()) {
        std::cerr << "Tidak ada frame_*.png di folder tersebut." << std::endl;
        return;
    }

    // Urutkan berdasarkan angka setelah frame_
    std::sort(frameFiles.begin(), frameFiles.end(), [](const std::string& a, const std::string& b) {
        auto get_number = [](const std::string& filename) -> int {
            size_t pos1 = filename.find("frame_") + 6; // setelah "frame_"
            size_t pos2 = filename.find_last_of('.');
            return std::stoi(filename.substr(pos1, pos2 - pos1));
        };
        return get_number(a) < get_number(b);
    });

    int width, height, channels;
    unsigned char* first_frame = stbi_load(frameFiles[0].c_str(), &width, &height, &channels, 4);
    if (!first_frame) {
        std::cerr << "Gagal membaca gambar pertama: " << frameFiles[0] << std::endl;
        return;
    }
    stbi_image_free(first_frame);

    GifWriter writer = {};
    if (!GifBegin(&writer, outputGif.c_str(), width, height, 10)) {
        std::cerr << "Gagal membuat GIF." << std::endl;
        return;
    }

    for (const std::string& framePath : frameFiles) {
        int w, h, c;
        unsigned char* image = stbi_load(framePath.c_str(), &w, &h, &c, 4);
        if (!image) {
            std::cerr << "Gagal membaca gambar: " << framePath << std::endl;
            continue;
        }
        if (w != width || h != height) {
            std::cerr << "Ukuran gambar tidak konsisten: " << framePath << std::endl;
            stbi_image_free(image);
            continue;
        }
        GifWriteFrame(&writer, image, width, height, 10);
        stbi_image_free(image);
    }

    GifEnd(&writer);
    std::cout << "GIF berhasil dibuat: " << outputGif << std::endl;
}
