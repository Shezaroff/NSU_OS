#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int global_inited = 6456;
int global_not_inited[8192];
const int global_const = 56789;
char *global_ptr = "global Hello";

int *f_d() {
    int a = 12;
    printf("local inited variable in f_d: %p\n", &a);
    return &a;
}

void f_e() {
    char* str = malloc(100 * sizeof(char));
    if (str == NULL) {
        return;
    }
    strcpy(str, "useless hello world");
    printf("String before free: %s\n", str);
    free(str);
    printf("String after free: %s\n", str);
    char* new_str = malloc(32 * sizeof(char));
    if (new_str == NULL) {
        return;
    }
    strcpy(new_str, "useless hello world 2: return");
    printf("String before half-free: %s\n", new_str);
    sleep(15);
    char* middle_str = str + 16;
    free(middle_str);
    printf("String after half-free: %s\n", new_str);
    free(str);
    return;
}

void f_h() {
    printf("Environment variable: %s\n", getenv("NSU_OS"));
    setenv("NSU_OS", "useless hello world ?: last chance", 1);
    printf("Environment variable: %s\n", getenv("NSU_OS"));
}

void f_a() {
    int local_inited = 12323;
    int local_not_inited;
    char *local_ptr = "local Hello";
    char local_array[] = "local array Hello";
    static int local_static_inited = 12323323;
    static int local_static_not_inited;
    const int local_const = 123;

    printf("global inited: %p (%d)\n", &global_inited, global_inited);
    printf("global not inited: %p %p\n", global_not_inited, &global_not_inited[8100]);
    printf("global const: %p (%d)\n", &global_const, global_const);
    printf("global ptr to str: %p %p (%s)\n\n", &global_ptr, global_ptr, global_ptr);


    printf("local inited: %p (%d)\n", &local_inited, local_inited);
    printf("local not inited: %p\n", &local_not_inited);
    printf("local pointer to str: %p %p (%s)\n", &local_ptr, local_ptr, local_ptr);
    printf("local array: %p %p (%s)\n", &local_array, local_array, local_array);
    printf("local static inited: %p (%d)\n", &local_static_inited, local_static_inited);
    printf("local static not inited: %p\n", &local_static_not_inited);
    printf("local const: %p (%d)\n", &local_const, local_const);
    return;
}

int main() {
    printf("PID: %d\n", getpid());
    sleep(15);

    f_a();
    // int *ptr = f_d(); 
    // printf("local inited variable from f_d: %p\n", ptr);
    // printf("Value at ptr: %d\n", *ptr);
    // f_e();
    // f_h();

    // sleep(15);

    // sleep(15);
    sleep(3000);
    return EXIT_SUCCESS;
}
