#include "file_utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "zip_utils.h"
#include "md5_utils.h"

#ifdef _MSC_VER
#include <experimental/filesystem>
namespace std {
    namespace filesystem = experimental::filesystem;
}
#endif
namespace fs = std::filesystem;

bool FileUtils::createDirectory(const std::string& path) {
    try {
        return fs::create_directories(path);
    } catch (const std::exception& e) {
        std::cerr << "[FileUtils] Error creating directory: " << e.what() << std::endl;
        return false;
    }
}

bool FileUtils::copyFile(const std::string& source, const std::string& dest) {
    try {
        fs::copy_file(source, dest, fs::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[FileUtils] Error copying file: " << e.what() << std::endl;
        return false;
    }
}

bool FileUtils::removePath(const std::string& path) {
    try {
        return fs::remove_all(path);
    } catch (const std::exception& e) {
        std::cerr << "[FileUtils] Error removing path: " << e.what() << std::endl;
        return false;
    }
}

bool FileUtils::fileExists(const std::string& path) {
    return fs::exists(path);
}

long long FileUtils::getFileSize(const std::string& path) {
    try {
        return fs::file_size(path);
    } catch (const std::exception& e) {
        std::cerr << "[FileUtils] Error getting file size: " << e.what() << std::endl;
        return -1;
    }
}

bool FileUtils::extractZip(const std::string& zipFile, const std::string& destDir) {
    std::cout << "[FileUtils] Extracting ZIP: " << zipFile << " to " << destDir << std::endl;
    // DONE(2026/9/12 21:26): 实现ZIP解压功能
    return ZipUtils::extractAll(zipFile, destDir);
}

std::string FileUtils::calculateMD5(const std::string& filePath) {
    std::cout << "[FileUtils] Calculating MD5 for: " << filePath << std::endl;
    // DONE(2026/9/12 21:21): 实现MD5计算
    return MD5Utils::calculateMD5(filePath);
}
