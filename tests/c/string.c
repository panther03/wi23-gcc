volatile char* SIM_PRINT_LOC = 0xF000;

__attribute__((section(".text")))
int main() {
    const char* hello_world = "Hello World!!!!";
    int ofs = 0;
    while (*(hello_world)) {
        *(SIM_PRINT_LOC + ofs) = *hello_world;
        ofs++;
        hello_world++;
    }
    *(SIM_PRINT_LOC+ofs) = 0;
    asm volatile (
        "irq 0"
    );
}