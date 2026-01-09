long global = 42;
main() {
    printf("%p", &global);
    // 0x410010
    ...
    printf("%ld\n", global);
}
