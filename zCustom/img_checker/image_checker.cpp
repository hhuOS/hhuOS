#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace fs = std::filesystem;

bool can_open_image(const std::string& filepath) {
    int width, height, channels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    if (data) {
        stbi_image_free(data);
        return true;
    }
    return false;
}

int main() {
    std::string current_dir = std::filesystem::current_path().string();
    std::string pics_dir = current_dir + "/pics";
    if (!std::filesystem::exists(pics_dir)) {
        std::cerr << "The 'pics' directory does not exist." << std::endl;
        return 1;
    }
    current_dir = pics_dir;
    std::string good_dir = current_dir + "/good";

    // Create 'good' directory if it doesn't exist
    if (!fs::exists(good_dir)) {
        fs::create_directory(good_dir);
    }

    std::vector<std::string> failed_files;

    for (const auto& entry : fs::directory_iterator(current_dir)) {
        if (entry.is_regular_file()) {
            std::string filepath = entry.path().string();
            std::string filename = entry.path().filename().string();
            std::string extension = entry.path().extension().string();

            // Convert extension to lowercase for case-insensitive comparison
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

            // Check if the file has one of the specified extensions
            if (extension == ".jpeg" || extension == ".jpg" || extension == ".png" ||
                extension == ".tga" || extension == ".bmp" || extension == ".psd" ||
                extension == ".gif" || extension == ".hdr" || extension == ".pic" ||
                extension == ".pnm") {
                if (can_open_image(filepath)) {
                    std::string new_path = good_dir + "/" + filename;
                    std::filesystem::rename(filepath, new_path);
                    std::cout << "Moved: " << filename << " to good directory" << std::endl;
                } else {
                    failed_files.push_back(filename);
                }
            }
        }
    }

    if (!failed_files.empty()) {
        std::cout << "\nFiles that couldn't be opened:" << std::endl;
        for (const auto& file : failed_files) {
            std::cout << file << std::endl;
        }
    }

    return 0;
}
