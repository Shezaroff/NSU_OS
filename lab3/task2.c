#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

void create_directory(const char *path) {
    if (mkdir(path, 0755) == -1) {
        perror("Ошибка создания каталога");
        exit(1);
    }
}

void list_directory(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Ошибка открытия каталога");
        exit(1);
    }
    struct dirent *entry = readdir(dir);
    while (entry != NULL) {
        printf("%s\n", entry->d_name);
        entry = readdir(dir);
    }
    closedir(dir);
}

void remove_directory(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Ошибка открытия каталога");
        exit(1);
    }
    char fullpath[1024];
    struct dirent *entry = readdir(dir);
    while (entry != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        struct stat st;
        if (lstat(fullpath, &st) == -1) {
            perror("Ошибка получения информации о файле");
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            remove_directory(fullpath);
        } else {
            unlink(fullpath);
        }
        entry = readdir(dir);
    }
    closedir(dir);
    rmdir(path);
}

void create_file(const char *path) {
    int fd = open(path, O_CREAT, 0644);
    if (fd == -1) {
        perror("Ошибка создания файла");
        exit(1);
    }
    close(fd);
}

void remove_file(const char *path) {
    if (unlink(path) == -1) {
        perror("Ошибка удаления файла");
        exit(1);
    }
}

void read_file(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Ошибка открытия файла");
        exit(1);
    }
    char buffer[1024];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    while (bytes_read  > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
        bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    }
    printf("\n");
    close(fd);
}

void create_symlink(const char *target, const char *linkpath) {
    if (symlink(target, linkpath) == -1) {
        perror("Ошибка создания символьной ссылки");
        exit(1);
    }
}

void read_symlink(const char *path) {
    char buffer[1024];
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len == -1) {
        perror("Ошибка чтения символьной ссылки");
        exit(1);
    }
    buffer[len] = '\0';
    printf("%s\n", buffer);
}

void remove_symlink(const char *path) {
    if (unlink(path) == -1) {
        perror("Ошибка удаления символьной ссылки");
        exit(1);
    }
}

void create_hardlink(const char *target, const char *linkpath) {
    if (link(target, linkpath) == -1) {
        perror("Ошибка создания жесткой ссылки");
        exit(1);
    }
}

void remove_hardlink(const char *path) {
    if (unlink(path) == -1) {
        perror("Ошибка удаления жесткой ссылки");
        exit(1);
    }
}

void print_permissions(mode_t mode) {
    char perms[11] = "----------";
    if (S_ISDIR(mode)) {
        perms[0] = 'd';
    } else if (S_ISLNK(mode)) {
        perms[0] = 'l';
    } else if (S_ISREG(mode)) {
        perms[0] = '-';
    }

    if (mode & S_IRUSR) perms[1] = 'r';
    if (mode & S_IWUSR) perms[2] = 'w';
    if (mode & S_IXUSR) perms[3] = 'x';
    if (mode & S_IRGRP) perms[4] = 'r';
    if (mode & S_IWGRP) perms[5] = 'w';
    if (mode & S_IXGRP) perms[6] = 'x';
    if (mode & S_IROTH) perms[7] = 'r';
    if (mode & S_IWOTH) perms[8] = 'w';
    if (mode & S_IXOTH) perms[9] = 'x';

    printf("Права: %s, ", perms);
}

void stat_file(const char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("Ошибка получения информации о файле");
        exit(1);
    }
    print_permissions(st.st_mode);
    printf("Количество жестких ссылок: %lu\n", st.st_nlink);
}

void change_permissions(const char *path, mode_t mode) {
    if (chmod(path, mode) == -1) {
        perror("Ошибка изменения прав доступа");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Укажите аргумент команды\n");
        return 1;
    }

    char *action_path = realpath(argv[0], NULL);
    if (!action_path) {
        perror("Ошибка обработки пути команды");
        return 1;
    }
    char *action = strrchr(action_path, '/');
    if (action != NULL) {
        action = action + 1;
    } else {
        action = argv[0];
    }

    char *file_path = realpath(argv[1], NULL);
    if (!file_path) {
        perror("Ошибка обработки пути файла");
        return 1;
    }
    
    if (strcmp(action, "mkdir") == 0) {
        create_directory(file_path);
    } else if (strcmp(action, "ls") == 0) {
        list_directory(file_path);
    } else if (strcmp(action, "rmdir") == 0) {
        remove_directory(file_path);
    } else if (strcmp(action, "touch") == 0) {
        create_file(file_path);
    } else if (strcmp(action, "cat") == 0) {
        read_file(file_path);
    } else if (strcmp(action, "rm") == 0) {
        remove_file(file_path);
    } else if (strcmp(action, "lns") == 0) {
        create_symlink(file_path, argv[2]);
    } else if (strcmp(action, "readlink") == 0) {
        read_symlink(file_path);
    } else if (strcmp(action, "ln") == 0) {
        create_hardlink(file_path, argv[2]);
    } else if (strcmp(action, "unlink") == 0) {
        remove_hardlink(file_path);
    } else if (strcmp(action, "chmod") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Укажите имя файла и новые права в числовом формате\n");
            exit(1);
        }
        change_permissions(file_path, strtol(argv[2], NULL, 8));
    } else if (strcmp(action, "stat") == 0) {
        stat_file(file_path);
    } else {
        fprintf(stderr, "Неизвестная команда: %s\n", action);
        exit(1);
    }
    return 0;
}
