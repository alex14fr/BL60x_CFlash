#include <stdint.h>

void sayHello(void) {
	volatile uint32_t *uart=0x4000a000;
	char a=0;
	while(1) {
		*uart=a++;
	}
}
