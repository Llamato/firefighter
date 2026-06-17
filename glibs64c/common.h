#ifndef C64_COMMON_H
    #define C64_COMMON_H
    #define ADDRESS_TO_PTR(ADDR) ((volatile unsigned char*)ADDR)
    
    #include <stdint.h>

    void fillMemory(volatile unsigned char* memoryPtr, uint16_t length, uint8_t fillByte);
#endif