int main(int argc, char* argv[]) {
    unsigned int a = argv[0][0], b = argv[0][1];
    auto x = a & ~b;
    return x;
}
