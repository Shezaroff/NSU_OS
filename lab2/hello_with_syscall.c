#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

int my_write(int fildes, const char* buf, size_t nbyte) {
    return syscall(SYS_write, fildes, buf, nbyte);
}

int my_puts(const char* str) {
    size_t len = strlen(str);
    return my_write(1, str, len) * my_write(1, "\n", 1);
}

int main() {
    if (my_puts("Hello World!") != 0) {
	return -1;
    }
    return 0;
}

