int main() {
    int x = 10000;
    int y = 0;
    while (1) {
        while (x) x--;
        x = 10000;
        *((int*)0x6969) = 0;
    }
    
    return 0;
}