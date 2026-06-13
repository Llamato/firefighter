#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "gllm/gllm.h"
#include "graphics.h"

int main(void) {
    setBorderColor(COLOR_BLACK);
    switchToHighResBitmapMode();
    fillMemory(ADDRESS_TO_PTR(SCREEN_RAM), SCREEN_SIZE, (COLOR_RED << BITS_PER_NIBBLE) | (COLOR_GREEN));
    fillMemory(ADDRESS_TO_PTR(BITMAP_RAM), BITMAP_SIZE, 0);

    uint8_t backgroundBitmap[] = {
        #embed ""
    };

    //makeFilledCircleHighResBitmapBresenham(ADDRESS_TO_PTR(BITMAP_RAM), (struct Vector2ui) {BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2}, BITMAP_HEIGHT / 2);


    bool gamerunning = true;
    while (gamerunning) {

    }
}