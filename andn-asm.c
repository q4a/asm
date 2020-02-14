#include<stdio.h>
#include <x86intrin.h>

int main(int argc, char* argv[]) {
    unsigned int a = strtol(argv[1], NULL, 10), b = strtol(argv[2], NULL, 10), c;
#ifdef NOBMI
    printf("a=%d, b=%d\n", a, b);
    c = ~a & b;
#else
    c = __andn_u64(a, b);
#endif
    printf("__andn_u64(%d, %d)=%d\n", a, b, c);
    return 1;
}
