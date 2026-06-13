#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "gllm/gllm.h"
#include "graphics.h"

int main(void) {
    setBorderColor(COLOR_GREEN);
    switchToHighResBitmapMode();
    fillMemory(ADDRESS_TO_PTR(SCREEN_RAM), SCREEN_SIZE, 0);
    fillMemory(ADDRESS_TO_PTR(SCREEN_COLOR_RAM), SCREEN_SIZE, 1);

    //Debug!!!
    for(uint16_t currentPixel = 0; currentPixel < SCREEN_SIZE; currentPixel++) {
        bool isOdd = currentPixel & 1;
        setHighResBitmapPixel(ADDRESS_TO_PTR(BITMAP_RAM),(struct BitmapPosition) {currentPixel / BITS_PER_BYTE, currentPixel % BITS_PER_BYTE}, isOdd);
    }

    bool gamerunning = true;
    while (gamerunning) {

    }
}