#include<stdio.h>
#include <x86intrin.h>

void main(int argc, char* argv[]) {
    unsigned int a = 13, b = 3, c;
#ifdef NOBMI
    c = ~a & b;
#else
    c = __andn_u64(a, b);
#endif
    printf("__andn_u64(13, 3)=%d\n", c);
}
