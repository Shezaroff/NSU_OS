#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>

namespace fs = std::filesystem;

void create_directory(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cerr << "Не указан путь директории\n";
        return;
    }
    if (fs::create_directory(args[0])) {
        std::cout << "Создана директория " << args[0] << "\n";
    } else {
        std::cerr << "Не удалось создать директорию\n";
    }
}

void list_directory(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cerr << "Не указан путь к директории\n";
        return;
    }
    try {
        for (const auto& entry : fs::directory_iterator(args[0])) {
            std::cout << entry.path().filename() <<  "\n";
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Ошибка " << e.what() << "\n";
    }
}

void remove_directory(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cerr << "Не указан путь к директории\n";
        return;
    }
    try {
        fs::remove_all(args[0]);
        std::cout << "Удалена директория " << args[0] << "\n";
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Ошибка " << e.what() << "\n";
    }
}

void create_file(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cerr << "Не указан путь к файлу\n";
        return;
    }
    std::ofstream file(args[0]);
    if (file) {
        std::cout << "Создан файл " << args[0] << "\n";
    } else {
        std::cerr << "Не удалось создать файл\n";
    }
}

void read_file(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cerr << "Не указан путь к файлу\n";
        return;
    }
    std::ifstream file(args[0]);
    if (file) {
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << "\n";
        }
    } else {
        std::cerr << "Не удалось прочитать файл\n";
    }
}

void remove_file(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cerr << "Не указан путь к файлу\n";
        return;
    }
    if (fs::remove(args[0])) {
        std::cout << "Удален файл " << args[0] << "\n";
    } else {
        std::cerr << "Не удалось удалить файл\n";
    }
}

void create_symlink(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "Не указаны путь к файлу и имя ссылки\n";
        return;
    }
    try {
        fs::create_symlink(args[0], args[1]);
        std::cout << "Создана символьная ссылка " << args[1] << " -> " << args[0] << "\n";
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Ошибка " << e.what() << "\n";
    }
}

void read_symlink(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cerr << "Не указан путь к символьной ссылке\n";
        return;
    }
    try {
        std::cout << args[0] << " -> " << fs::read_symlink(args[0]) << "\n";
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Ошибка " << e.what() << "\n";
    }
}

void remove_symlink(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cerr << "Не указан путь к символьной ссылке\n";
        return;
    }
    if (fs::remove(args[0])) {
        std::cout << "Символьная ссылка удалена: " << args[0] << "\n";
    } else {
        std::cerr << "Не удалось удалить символьную ссылку\n";
    }
}

void create_hardlink(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "Не указаны путь к файлу и имя ссылки\n";
        return;
    }
    try {
        fs::create_hard_link(args[0], args[1]);
        std::cout << "Создана жесткая ссылка " << args[1] << " -> " << args[0] << "\n";
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Jib,rf " << e.what() << "\n";
    }
}

void remove_hardlink(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cerr << "Не указан путь к жесткой ссылке\n";
        return;
    }
    if (fs::remove(args[0])) {
        std::cout << "Удалена жесткая ссылка " << args[0] << "\n";
    } else {
        std::cerr << "Не удалось удалить жесткую ссылку\n";
    }
}

void print_file_stats(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cerr << "Не указан путь к файлу\n";
        return;
    }
    try {
        auto status = fs::status(args[0]);
        std::cout << "Права доступа: " << static_cast<int>(status.permissions()) << "\n";
        std::cout << "Количество жестких ссылок: " << fs::hard_link_count(args[0]) << "\n";
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Ошибка " << e.what() << "\n";
    }
}

void change_permissions(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "Не указаны путь к файлу и права доступа\n";
        return;
    }
    try {
        int num = std::stoi(args[1], nullptr, 8);
        if (args[1].length() != 3) {
            std::cerr << "Неправильно заданы права\n";
            return;
        }
        fs::permissions(args[0], static_cast<fs::perms>(num));
        std::cout << "Изменены права доступа: " << args[0] << "\n";
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Ошибка " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка " << e.what() << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Не указаны аргументы\n";
        return 1;
    } else if (argc > 3) {
        std::cerr << "Указано слишком много аргументов\n";
        return 1;
    }

    std::string link_name = fs::path(argv[0]).filename().string();

    std::vector<std::string> args(argv + 1, argv + argc);

    if (link_name == "mkdir") {
        create_directory(args);
    } else if (link_name == "ls") {
        list_directory(args);
    } else if (link_name == "rmdir") {
        remove_directory(args);
    } else if (link_name == "touch") {
        create_file(args);
    } else if (link_name == "cat") {
        read_file(args);
    } else if (link_name == "rm") {
        remove_file(args);
    } else if (link_name == "lns") {
        create_symlink(args);
    } else if (link_name == "readlink") {
        read_symlink(args);
    } else if (link_name == "rmlink") {
        remove_symlink(args);
    } else if (link_name == "ln") {
        create_hardlink(args);
    } else if (link_name == "unlink") {
        remove_hardlink(args);
    } else if (link_name == "stat") {
        print_file_stats(args);
    } else if (link_name == "chmod") {
        change_permissions(args);
    } else {
        std::cerr << "Неизвестная команда\n";
        return 1;
    }

    return 0;
}
