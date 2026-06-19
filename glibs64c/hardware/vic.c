#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "../common.h"
#include "../gllm/gllm.h"
#include "vic.h"

struct BitmapPosition spritePixelPositionToBitmapPosition(const struct Vector2uis position) {
    const uint16_t bitPosition = position.y * SPRITE_BYTES_PER_ROW * BITS_PER_BYTE + position.x;
    struct BitmapPosition bitmapPosition = { bitPosition / BITS_PER_BYTE, bitPosition % BITS_PER_BYTE};
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

struct Vector2ui rasterPositionToSpritePosition(const struct Vector2uis rasterPosition) {
    return (struct Vector2ui) {rasterPosition.x * 8 + 24, rasterPosition.y * 8 + 50};
}

struct Vector2uis spritePositionToRasterPosition(const struct Vector2ui spritePosition) {
    return (struct Vector2uis) {(spritePosition.x - 24) / 8, (spritePosition.y - 50) / 8};
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

bool isSpriteEnabled(const uint8_t spriteNr) {
    return *ADDRESS_TO_PTR(SPRITES_ENABLE) & (1<<spriteNr);
}

void enableSpriteMulticolorMode(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(SPRITES_MULTICOLOR) |= (1<<spriteNr);
}

void disableSpriteMulticolorMode(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(SPRITES_MULTICOLOR) &= ~(1<<spriteNr);
}

bool isSpriteMulticolor(const uint8_t spriteNr) {
    return *ADDRESS_TO_PTR(SPRITES_MULTICOLOR) & (1<<spriteNr);
}

void enableSpriteDoubleWidth(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(SPRITES_DOUBLE_WIDTH) |= (1<<spriteNr);
}

void disableSpriteDoubleWidth(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(SPRITES_DOUBLE_WIDTH) &= ~(1<<spriteNr);
}

bool isSpriteDoubleWidth(const uint8_t spriteNr) {
    return *ADDRESS_TO_PTR(SPRITES_DOUBLE_WIDTH) & (1<<spriteNr);
}

void enableSpriteDoubleHeight(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(SPRITES_DOUBLE_HEIGHT) |= (1<<spriteNr);
}

void disableSpriteDoubleHeight(const uint8_t spriteNr) {
    *ADDRESS_TO_PTR(SPRITES_DOUBLE_HEIGHT) &= ~(1<<spriteNr);
}

bool isSpriteDoubleHeight(const uint8_t spriteNr) {
    return *ADDRESS_TO_PTR(SPRITES_DOUBLE_HEIGHT) & (1<<spriteNr);
}

void setSpriteBitmapPointer(const uint8_t spriteNr, const uint8_t bitmapBlock) {
    *ADDRESS_TO_PTR(SPRITE_0_PTR + spriteNr) = bitmapBlock;
}

void positionSprite(const uint8_t spriteNr, const struct Vector2ui position) {
    volatile unsigned char* spriteXlowPositionRegisterAddress = ADDRESS_TO_PTR(SPRITE_0_POSITION + spriteNr * 2);
    const uint8_t positionXlow = position.x & 0xff;
    const bool positionXhigh = position.x > UINT8_MAX;
    *spriteXlowPositionRegisterAddress = positionXlow;
    if(positionXhigh) {
        *ADDRESS_TO_PTR(SPRITES_X_HIGH) |= (1 << spriteNr);
    } else {
        *ADDRESS_TO_PTR(SPRITES_X_HIGH) &= ~(1 << spriteNr);
    }
    volatile unsigned char* spriteYpositionRegisterAddress = spriteXlowPositionRegisterAddress + 1;
    *spriteYpositionRegisterAddress = position.y;
}

void copySpriteBitmap(volatile unsigned char* to, volatile unsigned char* from) {
	for(uint16_t currentByte = 0; currentByte < SPRITE_SIZE; currentByte++) {
		to[currentByte] = from[currentByte];
	}
}

void setSpritePixel(volatile unsigned char* bitmapPointer, const struct Vector2uis position) {
    struct BitmapPosition bitmapPosition = spritePixelPositionToBitmapPosition(position);
    bitmapPointer[bitmapPosition.byte] |= (1 << ((BITS_PER_BYTE - 1) - bitmapPosition.bit)); 
}

void clearSpritePixel(volatile unsigned char* bitmapPointer, const struct Vector2uis position) {
    struct BitmapPosition bitmapPosition = spritePixelPositionToBitmapPosition(position);
    bitmapPointer[bitmapPosition.byte] &= ~(1 << ((BITS_PER_BYTE -1) - bitmapPosition.bit));
}

void applySpriteTemplate(const uint8_t spriteNr, const uint8_t bitmapBlock, const struct SpriteTemplate* spriteTemplate) {
    setSpriteColor(spriteNr, spriteTemplate->color);
    setSpriteBitmapPointer(spriteNr, bitmapBlock);
    if(spriteTemplate->isMulticolor) {
        enableSpriteMulticolorMode(spriteNr);
    } else {
        disableSpriteMulticolorMode(spriteNr);
    }
    if(spriteTemplate->isDoubleWidth) {
        enableSpriteDoubleWidth(spriteNr);
    } else {
        disableSpriteDoubleWidth(spriteNr);
    }
    if(spriteTemplate->isDoubleHeight) {
        enableSpriteDoubleHeight(spriteNr);
    } else {
        disableSpriteDoubleHeight(spriteNr);
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
    const uint16_t gridIndex = gridCell.y * TEXT_SCREEN_COLUMNS + gridCell.x;
    for(uint16_t currentRow = 0; currentRow < BYTES_PER_CHAR_BITMAP; currentRow++) {
        bitmapPointer[gridIndex * BYTES_PER_CHAR_BITMAP + currentRow] = tile.bitmapData[currentRow];
    }
     screenRamPointer[gridIndex] = tile.colors;
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