#include <stdlib.h>

int gloabl_inited = 6456;
int global_not_inited[1024];
const int global_const = 56789;
char *global_ptr = "global Hello";

void f_a() {
    int local_inited = 12323;
    int local_not_inited;
    char *local_ptr = "local Hello";
    char local_array[] = "local array Hello";
    static int local_static_inited = 12323323;
    static int local_static_not_inited;
    const int local_const = 123;

}

int main() {
    sleep(3000);
    return EXIT_SUCCESS;
}