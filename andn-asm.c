#include <x86intrin.h>

int main(int argc, char* argv[]) {
    unsigned int a = argv[0][0], b = argv[0][1];
    return __andn_u64(a, b);
}
