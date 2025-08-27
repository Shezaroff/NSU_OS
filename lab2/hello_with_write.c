#include <unistd.h>

int main() {
    if (write(1, "Hello world\n", 12) != 12) {
	write(2, "error when outputting to stdout\n", 27);
	return -1;
    }
    return 0;
}
