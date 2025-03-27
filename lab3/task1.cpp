#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

mode_t set_temp_permissions(const char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("Ошибка чтения прав доступа");
        return 0;
    }
    mode_t original_mode = st.st_mode & 0777;
    chmod(path, 0700); 
    return original_mode;
}

void restore_permissions(const char *path, mode_t original_mode) {
    chmod(path, original_mode);
}

int reverse_copy_file(const char *src_path, const char *dst_path, mode_t mode) {
    int src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1) {
        perror("Ошибка открытия исходного файла");
        return -1;
    }


    mode_t original_mode_dst;
    int dst_mode_flag = 0;
    struct stat st;
    if (stat(dst_path, &st) != -1) {
        original_mode_dst = set_temp_permissions(dst_path);
        dst_mode_flag = 1;
    }

    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dst_fd == -1) {
        perror("Ошибка создания целевого файла");
        if (dst_mode_flag) {
            restore_permissions(dst_path, original_mode_dst);
        }
        close(src_fd);
        return -1;
    }

    off_t file_size = lseek(src_fd, 0, SEEK_END);
    if (file_size == -1) {
        perror("Ошибка определения размера файла");
        if (dst_mode_flag) {
            restore_permissions(dst_path, original_mode_dst);
        }
        close(src_fd);
        close(dst_fd);
        return -1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    off_t pos = file_size;
    while (pos > 0) {
        ssize_t read_size = (pos >= BUFFER_SIZE) * BUFFER_SIZE + (pos < BUFFER_SIZE) * pos;
        pos -= read_size;
        lseek(src_fd, pos, SEEK_SET);
        bytes_read = read(src_fd, buffer, BUFFER_SIZE);
        if (bytes_read < 0) {
            perror("Ошибка чтения файла");
            if (dst_mode_flag) {
                restore_permissions(dst_path, original_mode_dst);
            }
            close(src_fd);
            close(dst_fd);
            return -1;
        }
        for (ssize_t i = 0; i < bytes_read / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[bytes_read - 1 - i];
            buffer[bytes_read - 1 - i] = temp;
        }
        write(dst_fd, buffer, bytes_read);
    }
    
    fchmod(dst_fd, mode);
    close(src_fd);
    close(dst_fd);
    return 0;
}

void process_directory(const char *src_dir, const char *dst_dir) {
    struct stat st;
    if (stat(src_dir, &st) == -1) {
        perror("Ошибка чтения параметров каталога");
        return;
    }

    mode_t original_mode_src = set_temp_permissions(src_dir);
    mode_t original_mode_dst;
    int dst_mode_flag = 0;

    if (stat(dst_dir, &st) == -1) {
        if (mkdir(dst_dir, 0700) == -1) {
            perror("Ошибка создания каталога");
            restore_permissions(src_dir, original_mode_src);
            return;
        }
    } else {
        original_mode_dst = set_temp_permissions(dst_dir);
        dst_mode_flag = 1;
    }


    DIR *dir = opendir(src_dir);
    if (dir == NULL) {
        perror("Ошибка открытия каталога");
        restore_permissions(src_dir, original_mode_src);
        if (dst_mode_flag) {
            restore_permissions(dst_dir, original_mode_dst);
        }
        return;
    }
    
    struct dirent *entry = readdir(dir);
    while (entry != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            entry = readdir(dir);
            continue;
        }

        char src_path[BUFFER_SIZE], dst_path[BUFFER_SIZE];
        snprintf(src_path, BUFFER_SIZE, "%s/%s", src_dir, entry->d_name);
        
        char dst_name[strlen(entry->d_name) + 1];
        strcpy(dst_name, entry->d_name);
        reverse_string(dst_name);
        snprintf(dst_path, BUFFER_SIZE, "%s/%s", dst_dir, dst_name);

        if (entry->d_type == DT_REG) {
            mode_t file_mode = set_temp_permissions(src_path);
            reverse_copy_file(src_path, dst_path, file_mode);
            restore_permissions(src_path, file_mode);
        } else if (entry->d_type == DT_DIR) {
            process_directory(src_path, dst_path);
        }
        entry = readdir(dir);
    }
    
    closedir(dir);
    restore_permissions(dst_dir, original_mode_src);
    restore_permissions(src_dir, original_mode_src);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Введите путь к каталогу\n");
        return 1;
    }

    char *src_dir = realpath(argv[1], NULL);
    if (!src_dir) {
        perror("Ошибка обработки пути");
        return 1;
    }

    char rev_dir[strlen(src_dir) + 1];
    strcpy(rev_dir, src_dir);
    char *base_name = strrchr(rev_dir, '/');
    if (base_name != NULL) {
        base_name = base_name + 1;
    } else {
        base_name = rev_dir;
    }
    reverse_string(base_name);

    process_directory(src_dir, rev_dir);
    
    free(src_dir);
    return 0;
}
