#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>

void copy_dir(const std::filesystem::path& source, const std::filesystem::path& dest) {
    std::size_t BUFFER_SIZE = 100;
    if (!std::filesystem::exists(dest)) {
        std::filesystem::create_directory(dest);
    }

    char* buffer = new char[BUFFER_SIZE];
    long read_size = 0;
    
    for (const auto& entry : std::filesystem::directory_iterator(source)) {
        if (!entry.is_regular_file()) {
            continue;
        }


        std::string fileName = entry.path().filename().string();
        std::reverse(fileName.begin(), fileName.end());
        std::filesystem::path destPath = dest / fileName;

        std::ifstream oldFile(entry.path(), std::ios::binary | std::ios::ate);
        if (!oldFile) {
            std::cerr << "File opening error: " << entry.path() << std::endl;
            continue;
        }

        std::ofstream newFile(destPath, std::ios::binary);
        if (!newFile) {
            std::cerr << "File creation error: " << destPath << std::endl;
            oldFile.close();
            continue;
        }

        std::streampos pos = oldFile.tellg();

        while (pos > 0) {
            std::size_t readSize = (pos >= BUFFER_SIZE) * BUFFER_SIZE + (pos >= BUFFER_SIZE) * pos;
            pos -= readSize;
            oldFile.seekg(pos);
            oldFile.read(buffer, readSize);
            readSize = oldFile.gcount(); 
            std::reverse(buffer, buffer + readSize);
            newFile.write(buffer, readSize);
        }
        oldFile.close();
        newFile.close();
    }

    delete[] buffer;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Incorrect number of arguments: " << argc - 1 << std::endl;
        return 1;
    }
    
    std::filesystem::path sourcePath = argv[1];
    if (!std::filesystem::exists(sourcePath) || !std::filesystem::is_directory(sourcePath)) {
        std::cerr << "Directory does not exist: " << sourcePath << std::endl;
        return 1;
    }
    
    std::string reversedName = sourcePath.filename().string();
    std::reverse(reversedName.begin(), reversedName.end());
    std::filesystem::path destPath = sourcePath.parent_path() / reversedName;
    
    copy_dir(sourcePath, destPath);    
    return 0;
}
