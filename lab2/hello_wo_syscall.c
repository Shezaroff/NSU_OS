int main() {
    const char str[] = "Hello world!\n";
    const long len = 13; 
   __asm__ volatile (
        "movq $1, %%rax\n"
        "movq $1, %%rdi\n"
        "leaq %0, %%rsi\n"
        "movq %1, %%rdx\n"
        "syscall\n"       
        :: "m"(str), "r"(len) : "rax", "rdi", "rsi", "rdx"
    );

    return 0;
}
