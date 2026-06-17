#include <stdint.h>
void fillMemory(volatile unsigned char* memoryPtr, uint16_t length, uint8_t fillByte) {
	for(uint16_t currentByte = 0; currentByte < length; currentByte++) {
		memoryPtr[currentByte] = fillByte;
	}
}