#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "../common.h"
#include "../gllm/gllm.h"
#include "vic.h"

struct MemoryPosition spritePixelPositionToBitmapPosition(const struct Vector2uis position) {
    const uint16_t bitPosition = position.y * SPRITE_BYTES_PER_ROW * BITS_PER_BYTE + position.x;
    struct MemoryPosition bitmapPosition = { bitPosition / BITS_PER_BYTE, bitPosition % BITS_PER_BYTE};
    return bitmapPosition;
}

struct Vector2uis bitmapPositionToCharGridPosition(const struct Vector2ui rasterPosition) {
    return (struct Vector2uis) {rasterPosition.x / BITS_PER_BYTE, rasterPosition.y / BITS_PER_BYTE};
}

struct MemoryPosition bitmapPositionToMemoryPosition(const struct Vector2ui rasterPosition) { //Potentially optimize use smaller uints were possible
    struct Vector2uis charGridPosition = bitmapPositionToCharGridPosition(rasterPosition);
    const uint16_t pixelY = rasterPosition.y % BITS_PER_BYTE;
    const uint16_t offset = (charGridPosition.y * TEXT_SCREEN_COLUMNS + charGridPosition.x) * BYTES_PER_CHAR_BITMAP + pixelY;
    const uint16_t pixelX = (BITS_PER_BYTE -1) - rasterPosition.x % BITS_PER_BYTE;
    return (struct MemoryPosition) {offset, pixelX};
}

struct Vector2ui bitmapPositionToSpritePosition(struct Vector2ui bitmapPosition) {
    return (struct Vector2ui) {bitmapPosition.x + BITMAP_SPRITE_X_OFFSET, bitmapPosition.y + BITMAP_SPRITE_Y_OFFSET};
}

struct Vector2ui charGridPositionToSpritePosition(struct Vector2uis gridPosition) {
    return (struct Vector2ui) {gridPosition.x * BITS_PER_BYTE + BITMAP_SPRITE_X_OFFSET, gridPosition.y * BITS_PER_BYTE + BITMAP_SPRITE_Y_OFFSET};
}

struct Vector2uis spritePositionToCharGridPosition(struct Vector2ui spritePosition) {
    return (struct Vector2uis) {(spritePosition.x - BITMAP_SPRITE_X_OFFSET) / BITS_PER_BYTE,(spritePosition.y - BITMAP_SPRITE_Y_OFFSET) / BITS_PER_BYTE};
}

struct Vector2ui charGridPositionToBitmapPosition(const struct Vector2uis gridPosition) {
    return (struct Vector2ui) {gridPosition.x * BITS_PER_BYTE, gridPosition.y * BITS_PER_BYTE};
}

void setSharedMulticolorSpritesPrimeryColor(const uint8_t primery) {
    *ADDRESS_TO_PTR(VIC_SPRITES_MULTICOLOR_PRIMERY) = primery;
}

void setSharedMulticolorSpritesSecondaryColor(const uint8_t secondary) {
    *ADDRESS_TO_PTR(VIC_SPRITES_MULTICOLOR_SECONDARY) = secondary;
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

uint8_t getSpriteBitmapBlock(const uint8_t spriteNr) {
    return *ADDRESS_TO_PTR(SPRITE_0_PTR + spriteNr);
}

void setSpriteBitmapBlock(const uint8_t spriteNr, const uint8_t bitmapBlock) {
    *ADDRESS_TO_PTR(SPRITE_0_PTR + spriteNr) = bitmapBlock;
}

struct Vector2ui getSpritePosition(const uint8_t spriteNr) {
    struct Vector2ui position;
    volatile unsigned char* spriteXlowPositionRegisterAddress = ADDRESS_TO_PTR(SPRITE_0_POSITION + spriteNr * 2);
    volatile unsigned char* spriteYpositionRegisterAddress = spriteXlowPositionRegisterAddress + 1;
    position.x = *spriteXlowPositionRegisterAddress;
    position.x |= *ADDRESS_TO_PTR(SPRITES_X_HIGH) & (1 << spriteNr);
    position.y = *spriteYpositionRegisterAddress;
    return position;
}

void setSpritePosition(const uint8_t spriteNr, const struct Vector2ui position) {
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

struct Vector2ui clampSpritePositionToScreen(struct Vector2ui position) {
    if(position.x < SPRITE_SCREEN_LEFT_LIMIT) position.x = SPRITE_SCREEN_LEFT_LIMIT;
    if(position.y < SPRITE_SCREEN_UPPER_LIMIT) position.y = SPRITE_SCREEN_UPPER_LIMIT;
    if(position.x > SPRITE_SCREEN_RIGHT_LIMIT) position.x = SPRITE_SCREEN_RIGHT_LIMIT;
    if(position.y > SPRITE_SCREEN_LOWER_LIMIT) position.y = SPRITE_SCREEN_LOWER_LIMIT;
    return position;
}

void copySpriteBitmap(volatile unsigned char* to, volatile unsigned char* from) {
	for(uint16_t currentByte = 0; currentByte < SPRITE_SIZE; currentByte++) {
		to[currentByte] = from[currentByte];
	}
}

void setSpritePixel(volatile unsigned char* bitmapPointer, const struct Vector2uis position) {
    struct MemoryPosition bitmapPosition = spritePixelPositionToBitmapPosition(position);
    bitmapPointer[bitmapPosition.byte] |= (1 << ((BITS_PER_BYTE - 1) - bitmapPosition.bit)); 
}

void clearSpritePixel(volatile unsigned char* bitmapPointer, const struct Vector2uis position) {
    struct MemoryPosition bitmapPosition = spritePixelPositionToBitmapPosition(position);
    bitmapPointer[bitmapPosition.byte] &= ~(1 << ((BITS_PER_BYTE -1) - bitmapPosition.bit));
}

void applySpriteTemplate(const uint8_t spriteNr, const uint8_t bitmapBlock, const struct SpriteTemplate* spriteTemplate) {
    setSpriteColor(spriteNr, spriteTemplate->color);
    setSpriteBitmapBlock(spriteNr, bitmapBlock);
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

uint8_t getSpriteSpriteCollisions() {
    return *ADDRESS_TO_PTR(SPRITES_SPRITE_COLLISION);
}

uint8_t getSpriteBackgroundCollisions() {
    return *ADDRESS_TO_PTR(SPRITES_BACKGROUND_COLLISION);
}

void fillBitmapTile(volatile unsigned char* bitmapPointer, struct Vector2uis gridPosition, uint8_t fillend) {
    volatile unsigned char* gridCell = ADDRESS_TO_PTR(bitmapPointer + gridPosition.y * TEXT_SCREEN_COLUMNS * BYTES_PER_CHAR_BITMAP + gridPosition.x * BYTES_PER_CHAR_BITMAP);
    for(uint8_t currentByte = 0; currentByte < BYTES_PER_CHAR_BITMAP; currentByte++) {
        gridCell[currentByte] = fillend;
    }
}

void placeHighResBitmapTile(volatile unsigned char* bitmapPointer, volatile unsigned char* screenRamPointer, struct HighResBitmapTile tile, const struct Vector2uis gridPosition) {
    const uint16_t gridIndex = gridPosition.y * TEXT_SCREEN_COLUMNS + gridPosition.x;
    for(uint16_t currentRow = 0; currentRow < BYTES_PER_CHAR_BITMAP; currentRow++) {
        bitmapPointer[gridIndex * BYTES_PER_CHAR_BITMAP + currentRow] = tile.bitmapData[currentRow];
    }
     screenRamPointer[gridIndex] = tile.colors;
}

uint8_t getHighResBitmapTileColors(volatile unsigned char* screenRamPointer, const struct Vector2uis gridPosition) {
    return screenRamPointer[gridPosition.y * TEXT_SCREEN_COLUMNS + gridPosition.x];
}

void setHighResBitmapTileColors(volatile unsigned char* screenRamPointer, const struct Vector2uis gridPosition, uint8_t colors) {
    screenRamPointer[gridPosition.y * TEXT_SCREEN_COLUMNS + gridPosition.x] = colors;
}

uint8_t getBackgroundColorOfHighResBitmapTile(volatile unsigned char *screenRamPointer, const struct Vector2uis gridPosition) {
    uint8_t colors = screenRamPointer[gridPosition.y * TEXT_SCREEN_COLUMNS + gridPosition.x] = colors;
    return colors & 0x0f;
}

void setBackgroundColorOfHighResBitmapTile(volatile unsigned char *screenRamPointer, const struct Vector2uis gridPosition, uint8_t primeryColor) {
    volatile unsigned char* gridCell = ADDRESS_TO_PTR(screenRamPointer+gridPosition.y * TEXT_SCREEN_COLUMNS + gridPosition.x);
    uint8_t colors = *gridCell;
    colors &= 0x0f;
    colors |= primeryColor << BITS_PER_NIBBLE;
    *gridCell = colors;
}

uint8_t getForegroundColorOfHighResBitmapTile(volatile unsigned char *screenRamPointer, const struct Vector2uis gridPosition) {
    uint8_t colors = screenRamPointer[gridPosition.y * TEXT_SCREEN_COLUMNS + gridPosition.x] = colors;
    return (colors & 0xf0) >> BITS_PER_NIBBLE;
}

void setForegroundColorOfHighResBitmapTile(volatile unsigned char *screenRamPointer, const struct Vector2uis gridPosition, uint8_t secondaryColor) {
    volatile unsigned char* gridCell = ADDRESS_TO_PTR(screenRamPointer+gridPosition.y * TEXT_SCREEN_COLUMNS + gridPosition.x);
    uint8_t colors = *gridCell;
    colors &= 0xf0;
    colors |= secondaryColor;
    *gridCell = colors;
}

void colorRectangularHighResBitmapRegion(volatile unsigned char* screenRamPointer, const struct Rectangle2ui rectangle, const uint8_t foregroundColor, const uint8_t backgroundColor) {
    const struct Vector2uis topLeftGridCell = bitmapPositionToCharGridPosition(rectangle.topLeftCorner);
    const struct Vector2uis bottomRightGridCell = bitmapPositionToCharGridPosition(rectangle.bottomRightCorner);
    for(uint8_t currentRow = topLeftGridCell.y; currentRow <= bottomRightGridCell.y; currentRow++) {
        for(uint8_t currentColumn = topLeftGridCell.x; currentColumn <= bottomRightGridCell.x; currentColumn++) {
             screenRamPointer[currentRow * TEXT_SCREEN_COLUMNS + currentColumn] = (foregroundColor << BITS_PER_NIBBLE) | backgroundColor;
        }
    }
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
    struct MemoryPosition pixelPosition = bitmapPositionToMemoryPosition(position);
    *ADDRESS_TO_PTR(bitmapPointer + pixelPosition.byte) |= (1 << pixelPosition.bit);
};

void clearHighResBitmapPixel(volatile unsigned char* bitmapPointer, const struct Vector2ui position) {
    struct MemoryPosition pixelPosition = bitmapPositionToMemoryPosition(position);
    *ADDRESS_TO_PTR(bitmapPointer + pixelPosition.byte) &= ~(1 << pixelPosition.bit);
}

void setBorderColor(uint8_t color) {
    *ADDRESS_TO_PTR(VIC_BORDER_COLOR) = color;
}

void setBackgroundColor(uint8_t color) {
    *ADDRESS_TO_PTR(VIC_BACKGROUND_COLOR) = color;
}