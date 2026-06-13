#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "gllm/gllm.h"
#include "graphics.h"

struct BitmapPosition spritePixelPositionToBitmapPosition(const struct Vector2uis position) {
    const uint16_t bitPosition = position.y * SPRITE_BYTES_PER_ROW * BITS_PER_BYTE + position.x;
    struct BitmapPosition bitmapPosition = { bitPosition / BITS_PER_BYTE, bitPosition % 8};
    return bitmapPosition;
}

void positionSprite(const uint8_t spriteNr, const struct Vector2ui posiition) {
    volatile unsigned char* spriteXlowPositionRegisterAddress = ADDRESS_TO_PTR(SPRITE_0_POSITION + spriteNr * 2);
    uint8_t positionXlow = posiition.x & 0xFF;
    bool positionXhigh = posiition.y & 0x100;
    *spriteXlowPositionRegisterAddress = positionXlow;
    *ADDRESS_TO_PTR(SPRITES_X_HIGH) = (SPRITES_X_HIGH & ~(1 << spriteNr) | (positionXhigh << spriteNr));
    volatile unsigned char* spriteYpositionRegisterAddress = spriteXlowPositionRegisterAddress+1;
    *spriteYpositionRegisterAddress = posiition.y;
}

void copySpriteBitmap(volatile unsigned char* to, volatile unsigned char* from) {
	for(uint16_t currentByte = 0; currentByte < SPRITE_SIZE; currentByte++) {
		to[currentByte] = from[currentByte];
	}
}

void setSpritePixel(volatile unsigned char* bitmapPointer, const struct Vector2uis position) {
    struct BitmapPosition bitmapPosition = spritePixelPositionToBitmapPosition(position);
    bitmapPointer[bitmapPosition.byte] |= (1<<((BITS_PER_BYTE - 1) - bitmapPosition.bit)); 
}

void mirrorCircleSegment(volatile unsigned char* bitmapPointer, const struct Vector2uis center, const struct Vector2uis circumfrancePoint) {
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x + circumfrancePoint.x, center.y + circumfrancePoint.y});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x - circumfrancePoint.x, center.y + circumfrancePoint.y});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x + circumfrancePoint.x, center.y - circumfrancePoint.y});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x - circumfrancePoint.x, center.y - circumfrancePoint.y});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x + circumfrancePoint.y, center.y + circumfrancePoint.x});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x - circumfrancePoint.y, center.y + circumfrancePoint.x});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x + circumfrancePoint.y, center.y - circumfrancePoint.x});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x - circumfrancePoint.y, center.y - circumfrancePoint.x});
}

void makeCircleSpriteBresenham(volatile unsigned char* bitmapPointer, const struct Vector2uis center, const uint8_t r) {
    struct Vector2uis circumfrancePoint = {0, r};
    int16_t d = 3 - (2 * r);
    mirrorCircleSegment(bitmapPointer, center, circumfrancePoint);
    while(circumfrancePoint.x < circumfrancePoint.y) {
        if(d > 0) {
            circumfrancePoint.y--;
            d = d + 4 * (circumfrancePoint.x - circumfrancePoint.y) + 10;
        } else {
            d = d + 4 * circumfrancePoint.x + 6;
        }
        circumfrancePoint.x++;
        mirrorCircleSegment(bitmapPointer, center, circumfrancePoint);
    }
}

void makeLineSpriteBresenham(volatile unsigned char* bitmapPointer, const struct Vector2uis origin, const struct Vector2uis destination) {
    int16_t x0 = origin.x;
    int16_t y0 = origin.y;
    int16_t x1 = destination.x;
    int16_t y1 = destination.y;
    
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;
    int16_t x = x0;
    int16_t y = y0;
    
    while(1) {
        if (x >= 0 && x < SPRITE_COLUMNS && y >= 0 && y < SPRITE_ROWS) {
            setSpritePixel(bitmapPointer, (struct Vector2uis) {x, y});
        }
        if (x == x1 && y == y1) break;
        
        int16_t e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void setBorderColor(uint8_t color) {
    *ADDRESS_TO_PTR(0xD020) = color;
}

void switchToHighResBitmapMode() {
    //*ADDRESS_TO_PTR(0xD011) &= ~((1<<4) | (1<<6));
    *ADDRESS_TO_PTR(0xDD00) = (*ADDRESS_TO_PTR(0xDD00) & 0xFC) | 0x03; //Set VIC bank to $C000-$FFFF (bank 3)
    *ADDRESS_TO_PTR(0xD011) |= (1<<5); //Bitmap mode on
    *ADDRESS_TO_PTR(0xD016) &= ~(1<<4); //Multicolor mode off
    *ADDRESS_TO_PTR(0xD018) |= (1<<3); //Set screen position
    *ADDRESS_TO_PTR(0xD021) = 0;
}

struct BitmapPosition rasterPositionToMemoryPosition(const struct Vector2ui rasterPosition) {
    return (struct BitmapPosition) {rasterPosition.x, rasterPosition.y}; //Temp
}

void setHighResBitmapPixel(volatile unsigned char* bitmapPointer, const struct BitmapPosition pixelPosition, const bool state) {
    if(state) {
        *ADDRESS_TO_PTR(bitmapPointer + pixelPosition.byte) |= (1 << pixelPosition.bit);
    } else {
        *ADDRESS_TO_PTR(bitmapPointer + pixelPosition.byte) &= ~(1 << pixelPosition.bit);
    }
}