/*
Simple VGA helloworld
Aidan McEllistrem & Julien de Castelnau
3/30/23
*/

// Using wide chars for the time being..
#define char int

// In-hardware MM addresses
const int *VGA_TEXT_BUFFER = 0xFFFFE000;

void vga_print_plain(int x, int y, const char* msg_data) {
  int* buff_addr = VGA_TEXT_BUFFER + (y << 4) + (y << 6) + x;
  int i = 0;
  int c = 0x1000;
  // assume string is NULL-terminated
  while (*(msg_data + i) != 0) {
    // set text data
    // upper byte = 0x20 => green background, black text
    *(buff_addr + i) = *(msg_data + i) | c;
    i += 1;
	  c += 0x1000;
  }
}

__attribute__((section(".text")))
int main() {
  vga_print_plain(0,0,L"Hello World!");
  while(1) {
    continue;
  }
  return 0;
}
