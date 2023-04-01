/*
WI-23 Simple Frontend Test
Aidan McEllistrem
3/30/23
*/

#include "frontend.h"

// In-hardware MM addresses
const int *VGA_TEXT_BUFFER = 0x00000000;
const int *PS2_KEY         = 0x0000B000;
int *PS2_KEY_AWAIT   = 0x0000B004;

void __mulsi3() {
	return;
}

int main() {
  vga_print_plain(0,0,"Hello World!");
  while(1) {
    char kb_val = poll_kb();
    if (kb_val == 0x20) {
      vga_print_plain(0,1,"SPACEBAR Pressed!");
    }
  }
}

// Grab the keycode from the MM'd address that connects to the KB
char poll_kb() {
  if(*PS2_KEY_AWAIT) {
    // set flag down: we may grab a new char
    *PS2_KEY_AWAIT = 0;
    return *PS2_KEY;
  } else return 0;
}

// Print to the MM'd data
void vga_print(int x, int y, const int* color_data, const char* msg_data) {
  int* buff_addr = VGA_TEXT_BUFFER + (y << 4) + (y << 6) + x;
  int i = 0;
  // assume string is NULL-terminated
  while (*(msg_data + i) != 0) {
    // set low bytes (text data)
    *(buff_addr + i + 1) = *(msg_data + i);
    // set high bytes (fg/bg color data)
    *(buff_addr + i) = *(color_data + i);
    // shift 16 bits next
    i += 2;
  }
}

void vga_print_plain(int x, int y, const char* msg_data) {
  int* buff_addr = VGA_TEXT_BUFFER + (y << 4) + (y << 6) + x;
  int i = 0;
  // assume string is NULL-terminated
  while (*(msg_data + i) != 0) {
    // set low bytes (text data)
    *(buff_addr + i + 1) = *(msg_data + i);
    // set to white
    *(buff_addr + i) = 0x0F;
    // shift 16 bits next
    i += 2;
  }
}
