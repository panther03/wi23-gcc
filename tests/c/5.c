volatile char* SIM_PRINT_LOC = 0xF000;

int write_integer(unsigned int integer, char *buff) {
    char intBuff[16];
    char *indx = intBuff + 15;
    int count = 1;
     
    *indx = (char)(integer % 10) + '0';
    integer = integer / 10;
    while (integer) {
        *--indx = (char)(integer % 10) + '0';
        integer = integer / 10;
        count++;	
    }

    int i;
    for (i = 0; i < count; i++) {
        *buff++ = *(indx + i);
    }

    return count;
}


__attribute__((section(".text"))) 
int main() {

    write_integer(6094, SIM_PRINT_LOC);

    asm volatile (
        "irq 0"
    );

    return 0;
}