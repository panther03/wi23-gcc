// In-hardware MM addresses
const int *SWS  = 0xC001;
int *LEDS = 0xC000;

__attribute__((section(".text")))
int main() { 
	while(1) {
		*(LEDS) = *(SWS);
	}	
	return 0;
}
