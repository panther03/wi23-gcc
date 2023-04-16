/*
Simple VGA helloworld
Aidan McEllistrem & Julien de Castelnau
3/30/23
*/

// In-hardware MM addresses
unsigned int *VGA_TEXT_BUFFER = 0xFFFFE000;
const unsigned int VGA_TEXT_BUFFER_SIZE = 4800;

const unsigned int *PS2_KEY         = 0x0000C008;

// Grab the keycode from the MM'd address that connects to the KB
int poll_kb() {
  return *PS2_KEY;
}

void vga_print(int x, int y, const char* msg_data, int c) {
  int* buff_addr = VGA_TEXT_BUFFER + (y << 4) + (y << 6) + x;
  int i = 0;
  // assume string is NULL-terminated
  while (*(msg_data + i) != 0) {
    // set text data
    *(buff_addr + i) = *(msg_data + i) | c;
    i += 1;
  }
}

__attribute__((section(".text")))
int main() {
  int input_buf[2];
  input_buf[0] = 0;
  input_buf[1] = 0;
  int cursor_pos = 0;
  vga_print(0,0,"KEYBOARD TEST",0x0F00);
  vga_print(0,1,"Please press a key, any key...",0x0F00);
    while(1) {
        if (input_buf[0] = poll_kb()) {
            *VGA_TEXT_BUFFER = input_buf[0];
        }
    }
  return 0;
}