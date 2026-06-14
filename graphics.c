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

struct Vector2uis rasterPositionToCharGridPosition(const struct Vector2ui rasterPosition) {
    return (struct Vector2uis) {rasterPosition.x / BITS_PER_BYTE, rasterPosition.y / BITS_PER_BYTE};
}

struct BitmapPosition rasterPositionToMemoryPosition(const struct Vector2ui rasterPosition) { //Potentially optimize use smaller uints were possible
    struct Vector2uis charGridPosition = rasterPositionToCharGridPosition(rasterPosition);
    const uint16_t pixelY = rasterPosition.y % BITS_PER_BYTE;
    const uint16_t offset = (charGridPosition.y * TEXT_SCREEN_COLUMNS + charGridPosition.x) * BYTES_PER_CHAR_BITMAP + pixelY;
    const uint16_t pixelX = (BITS_PER_BYTE -1) - rasterPosition.x % BITS_PER_BYTE;
    return (struct BitmapPosition) {offset, pixelX};
}

void setSharedMulticolorSpriteColors(const uint8_t primery, const uint8_t secondary) {
    *ADDRESS_TO_PTR(0xD025) = primery;
    *ADDRESS_TO_PTR(0xD026) = secondary;
}

void setSpriteColor(const uint8_t spriteNr, uint8_t color) {
    *ADDRESS_TO_PTR(SPRITE_0_COLOR + spriteNr) = color;
}

void enableSprite(const uint8_t spriteNr) {
   *ADDRESS_TO_PTR(SPRITES_ENABLE) |= (1<<spriteNr); 
}

void disableSprite(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(SPRITES_ENABLE) &= ~(1<<spriteNr);
}

void enableSpriteMulticolorMode(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(0xD01C) |= (1<<spriteNr);
}

void disableSpriteMulticolorMode(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(0xD01C) &= ~(1<<spriteNr);
}

void enableSpriteDoubleWidth(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(0xD017) |= (1<<spriteNr);
}

void disableSpriteDoubleWidth(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(0xD017) &= ~(1<<spriteNr);
}

void enableSpriteDoubleHeight(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(0xD01D) |= (1<<spriteNr);
}

void disableSpriteDoubleHeight(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(0xD01D) &= ~(1<<spriteNr);
}

void setSpriteBitmapPointer(const uint8_t spriteNr, const uint8_t bitmapBlock) {
    *ADDRESS_TO_PTR(SPRITE_0_PTR + spriteNr) = bitmapBlock;
}

void positionSprite(const uint8_t spriteNr, const struct Vector2ui position) {
    volatile unsigned char* spriteXlowPositionRegisterAddress = ADDRESS_TO_PTR(SPRITE_0_POSITION + spriteNr * 2);
    uint8_t positionXlow = position.x & 0xFF;
    bool positionXhigh = position.y & 0x100;
    *spriteXlowPositionRegisterAddress = positionXlow;
    *ADDRESS_TO_PTR(SPRITES_X_HIGH) = (SPRITES_X_HIGH & ~(1 << spriteNr) | (positionXhigh << spriteNr));
    volatile unsigned char* spriteYpositionRegisterAddress = spriteXlowPositionRegisterAddress+1;
    *spriteYpositionRegisterAddress = position.y;
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

void clearSpritePixel(volatile unsigned char* bitmapPointer, const struct Vector2uis position) {
    struct BitmapPosition bitmapPosition = spritePixelPositionToBitmapPosition(position);
    bitmapPointer[bitmapPosition.byte] &= ~(1<<((BITS_PER_BYTE -1) - bitmapPosition.bit));
}

void mirrorCircleSpriteSegment(volatile unsigned char* bitmapPointer, const struct Vector2uis center, const struct Vector2uis circumfrancePoint) {
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x + circumfrancePoint.x, center.y + circumfrancePoint.y});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x - circumfrancePoint.x, center.y + circumfrancePoint.y});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x + circumfrancePoint.x, center.y - circumfrancePoint.y});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x - circumfrancePoint.x, center.y - circumfrancePoint.y});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x + circumfrancePoint.y, center.y + circumfrancePoint.x});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x - circumfrancePoint.y, center.y + circumfrancePoint.x});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x + circumfrancePoint.y, center.y - circumfrancePoint.x});
    setSpritePixel(bitmapPointer, (struct Vector2uis) {center.x - circumfrancePoint.y, center.y - circumfrancePoint.x});
}

void makeCircleSpriteBresenham(volatile unsigned char* bitmapPointer, const struct Vector2uis center, const uint8_t radius) {
    struct Vector2uis circumfrancePoint = {0, radius};
    int16_t diameter = 3 - (2 * radius);
    mirrorCircleSpriteSegment(bitmapPointer, center, circumfrancePoint);
    while(circumfrancePoint.x < circumfrancePoint.y) {
        if(diameter > 0) {
            circumfrancePoint.y--;
            diameter = diameter + 4 * (circumfrancePoint.x - circumfrancePoint.y) + 10;
        } else {
            diameter = diameter + 4 * circumfrancePoint.x + 6;
        }
        circumfrancePoint.x++;
        mirrorCircleSpriteSegment(bitmapPointer, center, circumfrancePoint);
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

void mirrorCircleHighResBitmapSegment(volatile unsigned char* bitmapPointer, const struct Vector2ui center, const struct Vector2ui circumfrancePoint) {
    setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {center.x + circumfrancePoint.x, center.y + circumfrancePoint.y});
    setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {center.x - circumfrancePoint.x, center.y + circumfrancePoint.y});
    setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {center.x + circumfrancePoint.x, center.y - circumfrancePoint.y});
    setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {center.x - circumfrancePoint.x, center.y - circumfrancePoint.y});
    setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {center.x + circumfrancePoint.y, center.y + circumfrancePoint.x});
    setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {center.x - circumfrancePoint.y, center.y + circumfrancePoint.x});
    setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {center.x + circumfrancePoint.y, center.y - circumfrancePoint.x});
    setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {center.x - circumfrancePoint.y, center.y - circumfrancePoint.x});
}

void makeCircleHighResBitmapBresenham(volatile unsigned char* bitmapPointer, const struct Vector2ui center, const uint8_t radius) {
    struct Vector2ui circumfrancePoint = {0, radius};
    int16_t diameter = 3 - (2 * radius);
    mirrorCircleHighResBitmapSegment(bitmapPointer, center, circumfrancePoint);
    while(circumfrancePoint.x < circumfrancePoint.y) {
        if(diameter > 0) {
            circumfrancePoint.y--;
            diameter = diameter + 4 * (circumfrancePoint.x - circumfrancePoint.y) + 10;
        } else {
            diameter = diameter + 4 * circumfrancePoint.x + 6;
        }
        circumfrancePoint.x++;
        mirrorCircleHighResBitmapSegment(bitmapPointer, center, circumfrancePoint);
    }
}

void makeLineHighResBitmapBresenham(volatile unsigned char* bitmapPointer, const struct Vector2ui origin, const struct Vector2ui destination) {
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
        if (x >= 0 && x < BITMAP_WIDTH && y >= 0 && y < BITMAP_HEIGHT) {
            setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {x, y});
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

void fillCircleHighResBitmapSegment(volatile unsigned char* bitmapPointer, const struct Vector2ui center, const struct Vector2ui circumfrancePoint) {
    const struct Vector2ui leftTopSidePoint = {center.x - circumfrancePoint.x, center.y - circumfrancePoint.y};
    const struct Vector2ui rightTopSidePoint = {center.x + circumfrancePoint.x, center.y - circumfrancePoint.y};
    const struct Vector2ui leftUpperSidePoint = {center.x - circumfrancePoint.y, center.y - circumfrancePoint.x};
    const struct Vector2ui rightUpperSidetPoint = {center.x + circumfrancePoint.y, center.y - circumfrancePoint.x};
    const struct Vector2ui leftLowerSidePoint = {center.x - circumfrancePoint.y, center.y + circumfrancePoint.x};
    const struct Vector2ui rightLowerSidePoint = {center.x + circumfrancePoint.y, center.y + circumfrancePoint.x};
    const struct Vector2ui leftBottomSidePoint = {center.x - circumfrancePoint.x, center.y + circumfrancePoint.y};
    const struct Vector2ui rightBottomSidePoint = {center.x + circumfrancePoint.x , center.y + circumfrancePoint.y};
    for(uint16_t currentPixel = leftTopSidePoint.x; currentPixel < rightTopSidePoint.x; currentPixel++) {
        setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {currentPixel, leftTopSidePoint.y});
    }
    for(uint16_t currentPixel = leftUpperSidePoint.x; currentPixel < rightUpperSidetPoint.x; currentPixel++) {
        setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {currentPixel, leftUpperSidePoint.y});
    }
    for(uint16_t currentPixel = leftLowerSidePoint.x; currentPixel < rightLowerSidePoint.x; currentPixel++) {
        setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {currentPixel, leftLowerSidePoint.y});
    }
    for(uint16_t currentPixel = leftBottomSidePoint.x; currentPixel < rightBottomSidePoint.x; currentPixel++) {
        setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {currentPixel, leftBottomSidePoint.y});
    }
}

void makeFilledCircleHighResBitmapBresenham(volatile unsigned char* bitmapPointer, const struct Vector2ui center, const uint8_t radius) {
    struct Vector2ui circumfrancePoint = {0, radius};
    int16_t diameter = 3 - (2 * radius);
    for(uint16_t currentPixel = center.x - circumfrancePoint.x; currentPixel < center.x + circumfrancePoint.x; currentPixel++) {
        setHighResBitmapPixel(bitmapPointer, (struct Vector2ui) {currentPixel, center.y});
    }
    while(circumfrancePoint.x < circumfrancePoint.y) {
        if(diameter > 0) {
            circumfrancePoint.y--;
            diameter = diameter + 4 * (circumfrancePoint.x - circumfrancePoint.y) + 10;
        } else {
            diameter = diameter + 4 * circumfrancePoint.x + 6;
        }
        circumfrancePoint.x++;
        fillCircleHighResBitmapSegment(bitmapPointer, center, circumfrancePoint);
    }
}

void colorRectangularHighResBitmapRegion(volatile unsigned char* screenRamPointer, const struct Vector2ui topLeftCorner, const struct Vector2ui bottomRightCorner, const uint8_t foregroundColor, const uint8_t backgroundColor) {
    const struct Vector2uis topLeftGridCell = rasterPositionToCharGridPosition(topLeftCorner);
    const struct Vector2uis bottomRightGridCell = rasterPositionToCharGridPosition(bottomRightCorner);
    for(uint8_t currentRow = topLeftGridCell.y; currentRow <= bottomRightGridCell.y; currentRow++) {
        for(uint8_t currentColumn = topLeftGridCell.x; currentColumn <= bottomRightGridCell.x; currentColumn++) {
             screenRamPointer[currentRow * TEXT_SCREEN_COLUMNS + currentColumn] = (foregroundColor << BITS_PER_NIBBLE) | backgroundColor;
        }
    }
}

void placeHighResBitmapTile(volatile unsigned char* bitmapPointer, volatile unsigned char* screenRamPointer, struct HighResBitmapTile tile, const struct Vector2uis gridCell) {
    for(uint8_t currentRow = 0; currentRow < BYTES_PER_CHAR_BITMAP; currentRow++) {
        *ADDRESS_TO_PTR(bitmapPointer + gridCell.y * TEXT_SCREEN_COLUMNS * BYTES_PER_CHAR_BITMAP + gridCell.x * BYTES_PER_CHAR_BITMAP) = tile.bitmapData[currentRow];
    }
    *ADDRESS_TO_PTR(screenRamPointer + gridCell.y * TEXT_SCREEN_COLUMNS + gridCell.x) = tile.colors;
}

void placeHighResBitmapMultiTile(volatile unsigned char *bitmapPointer, volatile unsigned char *screenRamPointer, struct HighResBitmapMultiTile tiles, const struct Vector2uis gridCell) {
    for(uint8_t currentRow = gridCell.x; currentRow < tiles.height; currentRow++) {
        for(uint8_t currentColumn = gridCell.y; currentColumn < tiles.width; currentColumn++) {
            //placeHighResBitmapMultiTile(bitmapPointer + currentRow * TEXT_SCREEN_COLUMNS * BYTES_PER_CHAR_BITMAP + currentColumn * BYTES_PER_CHAR_BITMAP, screenRamPointer)
        }
    }
}

void setBorderColor(uint8_t color) {
    *ADDRESS_TO_PTR(0xD020) = color;
}

void setBackgroundColor(uint8_t color) {
    *ADDRESS_TO_PTR(0xD021) = color;
}

void switchToHighResBitmapMode() {
    //*ADDRESS_TO_PTR(0xD011) &= ~((1<<4) | (1<<6));
    *ADDRESS_TO_PTR(0xDD00) = (*ADDRESS_TO_PTR(0xDD00) & 0xFC) | 0x03; //Set VIC bank to $C000-$FFFF (bank 3)
    *ADDRESS_TO_PTR(0xD011) |= (1<<5); //Bitmap mode on
    *ADDRESS_TO_PTR(0xD016) &= ~(1<<4); //Multicolor mode off
    *ADDRESS_TO_PTR(0xD018) |= (1<<3); //Set screen position to 0x2000
    *ADDRESS_TO_PTR(0xD021) = 0;
}

void setHighResBitmapPixel(volatile unsigned char* bitmapPointer, const struct Vector2ui position) {
    struct BitmapPosition pixelPosition = rasterPositionToMemoryPosition(position);
    *ADDRESS_TO_PTR(bitmapPointer + pixelPosition.byte) |= (1 << pixelPosition.bit);
};

void clearHighResBitmapPixel(volatile unsigned char* bitmapPointer, const struct Vector2ui position) {
    struct BitmapPosition pixelPosition = rasterPositionToMemoryPosition(position);
    *ADDRESS_TO_PTR(bitmapPointer + pixelPosition.byte) &= ~(1 << pixelPosition.bit);
}