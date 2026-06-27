#include <stdint.h>
void fillMemory(volatile unsigned char* memoryPtr, uint16_t length, uint8_t fillByte) {
	for(uint16_t currentByte = 0; currentByte < length; currentByte++) {
		memoryPtr[currentByte] = fillByte;
	}
}

void copyMemory(volatile unsigned char* destPtr, volatile unsigned char* srcPtr, uint16_t length) {
	for(uint16_t currentByte = 0; currentByte < length; currentByte++) {
		destPtr[currentByte] = srcPtr[currentByte];
	}
}