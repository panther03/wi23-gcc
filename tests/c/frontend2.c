/*
Simple VGA helloworld
Aidan McEllistrem & Julien de Castelnau
3/30/23
*/

#define STATE_WELCOME   0
#define STATE_EQUATION  1
int window_state = STATE_WELCOME;

// Using wide chars for the time being..
#define char int

// In-hardware MM addresses
const int *VGA_TEXT_BUFFER = 0xFFFFE000;

const int *PS2_KEY         = 0x0000C008;
const int *PS2_KEY_AWAIT   = 0x0000C009;
const int *PS2_KEY_ENTER   = 0x0000C00A;

const char TOP_BORDER[]          = {0x00, 0xCD};
const char SIDE_BORDER[]         = {0x00, 0xBA};
const char TOP_LEFT_BORDER[]     = {0x00, 0xC9};
const char TOP_RIGHT_BORDER[]    = {0x00, 0xCB};
const char BOTTOM_LEFT_BORDER[]  = {0x00, 0xC8};
const char BOTTOM_RIGHT_BORDER[] = {0x00, 0xBC};

// Grab the keycode from the MM'd address that connects to the KB
char poll_kb() {
  if(*PS2_KEY_AWAIT) {
    // set flag down: we may grab a new char
    //*PS2_KEY_AWAIT = 0;
    return *PS2_KEY;
  } else  {
    return 0;
  }
}

void vga_print(int x, int y, const char* msg_data) {
  int* buff_addr = VGA_TEXT_BUFFER + (y << 4) + (y << 6) + x;
  int i = 0;
  // assume color = white on black background
  int c = 0x0F00;
  // assume string is NULL-terminated
  while (*(msg_data + i) != 0) {
    // set text data
    *(buff_addr + i) = *(msg_data + i) | c;
    i += 1;
  }
}

// fancy VGA text mode boxes
void vga_draw_box(int x1, int y1, int x2, int y2) {

  // THE OFFENDING CODE (presumably)
  // draw top and bottom border
  for(int i = x1; i < x2; i++) {
    vga_print(i, y1, TOP_BORDER);
    vga_print(i, y2, TOP_BORDER);
  }

  // draw left and right side
  for(int i = y1; i < x2; i++) {
    vga_print(x1, i, SIDE_BORDER);
    vga_print(x2, i, SIDE_BORDER);
  }

  // draw top left corner
  vga_print(x1, y1, TOP_LEFT_BORDER);

  // draw top right corner
  vga_print(x2, y1, TOP_RIGHT_BORDER);

  // draw bottom left corner
  vga_print(x1, y2, BOTTOM_LEFT_BORDER);

  // draw bottom right corner
  vga_print(x1, y2, BOTTOM_RIGHT_BORDER);
}


__attribute__((section(".text")))
int main() {
  vga_print(0,0,L"Hello World!");
  char curr_line[79];
  int cursor_pos = 0;
  while(1) {
    switch(window_state) {
      case STATE_WELCOME:
        vga_draw_box(10,20,30,23);
        vga_print(22,21,"WISC-23 GRAPHING CALCULATOR");
        vga_print(22,22,"-PRESS ANY KEY-");
        if (poll_kb())
          window_state = STATE_EQUATION;
        break;
      case STATE_EQUATION:
        vga_print(0,0,">");
        vga_print(1,0,curr_line);
        if(poll_kb() && (cursor_pos >= 0 && cursor_pos <= 79)) {
          curr_line[cursor_pos] = poll_kb();
          cursor_pos++;
        }
        break;
      default: break;
    }
  }
  return 0;
}
