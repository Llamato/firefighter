#ifndef VIC_H
    #include <stdint.h>
    #include "../gllm/gllm.h"
    #define VIC_H

    //Memory mappings
    #define VIC_BORDER_COLOR		0xD020
    #define VIC_BACKGROUND_COLOR 	0xD021
    #define SCREEN_RAM      		0x0400
    #define SCREEN_COLOR_RAM		0xD800
    #define BITMAP_RAM              0x2000
    #define SPRITES_ENABLE   		0xD015
    #define SPRITES_MULTICOLOR      0xD01C
    #define SPRITES_DOUBLE_WIDTH    0xD017
    #define SPRITES_DOUBLE_HEIGHT   0xD01D
    #define SPRITE_0_PTR     		0x07f8
    #define SPRITE_1_PTR     		0x07f9
    #define SPRITE_2_PTR     		0x07fa
    #define SPRITE_3_PTR     		0x07fb
    #define SPRITE_4_PTR     		0x07fc
    #define SPRITE_5_PTR     		0x07fd
    #define SPRITE_6_PTR     		0x07fe
    #define SPRITE_7_PTR     		0x07ff
    #define SPRITES_X_HIGH 			0xD010
    #define SPRITE_0_COLOR 			0xD027
    #define SPRITE_1_COLOR 			0xD028
    #define SPRITE_2_COLOR 			0xD029
    #define SPRITE_3_COLOR 			0xD02A
    #define SPRITE_4_COLOR 			0xD02B
    #define SPRITE_5_COLOR 			0xD02C
    #define SPRITE_6_COLOR 			0xD02D
    #define SPRITE_7_COLOR 			0xD02E
    #define SPRITE_0_POSITION       0xD000
    #define SPRITE_1_POSITION       0xD002
    #define SPRITE_2_POSITION       0xD004
    #define SPRITE_3_POSITION       0xD006
    #define SPRITE_4_POSITION       0xD008
    #define SPRITE_5_POSITION       0xD00A
    #define SPRITE_6_POSITION       0xD00C
    #define SPRITE_7_POSITION       0xD00E

    //Hardware limitations
    #define BITS_PER_BYTE 8
    #define LOAD_IN_ADDRESS_BYTES 2
    #define BITS_PER_NIBBLE BITS_PER_BYTE / 2
    #define TEXT_SCREEN_COLUMNS 40
    #define TEXT_SCREEN_ROWS 25
    #define SCREEN_SIZE 1000
    #define BYTES_PER_CHAR_BITMAP 8
    #define BITMAP_WIDTH 320
    #define BITMAP_HEIGHT 200
    #define BITMAP_SIZE 8192
    #define BITMAP_SIZE_BITS 64000
    #define SPRITE_COLUMNS 24
    #define SPRITE_ROWS 21
    #define SPRITE_SIZE 64
    #define SPRITE_SCREEN_LEFT_LIMIT 1
    #define SPRITE_SCREEN_UPPER_LIMIT 1
    #define SPRITE_SCREEN_RIGHT_LIMIT BITMAP_WIDTH + 24
    #define SPRITE_SCREEN_LOWER_LIMIT BITMAP_HEIGHT + 50
    #define HARDWARE_SPRITE_COUNT 8
    #define SPRITE_BYTES_PER_ROW SPRITE_COLUMNS / BITS_PER_BYTE
    #define SPRITE_BITMAP_ADDRESS(SPRITE_BLOCK) (SPRITE_BLOCK * SPRITE_SIZE)
    #define POINT_SPRITE(SPRITE_NR, BLOCK) *ADDRESS_TO_PTR(SPRITE_##SPRITE_NR##_PTR) = BLOCK;
    #define SPRITE_EMBED_PARAMS(INDEX) clang::offset(INDEX * SPRITE_SIZE) limit(SPRITE_SIZE)
    #define TILE_EMBED_PARAMS(INDEX) clang::offset(INDEX * BYTES_PER_CHAR_BITMAP + LOAD_IN_ADDRESS_BYTES) limit(BYTES_PER_CHAR_BITMAP)
    #define COLOR_BLACK 0
    #define COLOR_WHITE 1
    #define COLOR_RED 2
    #define COLOR_CYAN 3
    #define COLOR_VIOLET 4
    #define COLOR_GREEN 5
    #define COLOR_BLUE 6
    #define COLOR_YELLOW 7
    #define COLOR_ORANGE 8
    #define COLOR_BROWN 9
    #define COLOR_LIGHT_RED 10
    #define COLOR_DARK_GRAY 11
    #define COLOR_GRAY 12
    #define COLOR_LIGHT_GREEN 13
    #define COLOR_LIGHT_BLUE 14
    #define COLOR_LIGHT_GRAY 15

    struct Sprite {
        uint8_t color;
        bool isMulticolor;
        bool isDoubleWidth;
        bool isDoubleHeight;
        struct Vector2ui position;
        volatile unsigned char* bitmapPtr;
    };

    struct SpriteTemplate {
        uint8_t color;
        bool isMulticolor;
        bool isDoubleWidth;
        bool isDoubleHeight;
    };

    struct AnimatedSprite {
        uint8_t color;
        bool isMulticolor;
        bool isDoubleWidth;
        bool isDoubleHeight;
        volatile unsigned char** frameBitmaps;
        uint8_t frameCount;
        uint8_t currentFrame;
        struct Vector2ui position;
    };

    struct AnimatedSpriteTemplate {
        uint8_t color;
        bool isMulticolor;
        bool isDoubleWidth;
        bool isDoubleHeight;
        volatile unsigned char** frameBitmaps;
        uint8_t frameCount;
    };

    struct BitmapPosition {
        uint16_t byte;
        uint8_t bit;
    };

    struct HighResBitmapTile {
        volatile unsigned char* bitmapData;
        uint8_t colors;
    };

    struct HighResBitmapMultiTile {
        struct HighResBitmapTile* tiles;
        uint8_t width;
        uint8_t height;
    };

    struct BitmapPosition spritePixelPositionToBitmapPosition(const struct Vector2uis position);
    struct Vector2uis rasterPositionToCharGridPosition(const struct Vector2ui rasterPosition);
    struct BitmapPosition rasterPositionToMemoryPosition(const struct Vector2ui rasterPosition);
    struct Vector2ui rasterPositionToSpritePosition(const struct Vector2uis rasterPosition);
    struct Vector2uis spritePositionToRasterPosition(const struct Vector2ui spritePosition);
    struct Vector2ui charGridPositionToRasterPosition(const struct Vector2ui gridPosition);
    struct Vector2ui charGridPositionToSpritePosition(const struct Vector2uis gridPosition);
    void setSharedMulticolorSpriteColors(const uint8_t primery, const uint8_t secondary);
    void setSpriteColor(const uint8_t spriteNr, uint8_t color);
    void enableSprite(const uint8_t spriteNr);
    void disableSprite(const uint8_t spriteNr);
    bool isSpriteEnabled(const uint8_t spriteNr);
    void enableSpriteMulticolorMode(const uint8_t spriteNr);
    void disableSpriteMulticolorMode(const uint8_t spriteNr);
    bool isSpriteMulticolor(const uint8_t spriteNr);
    void enableSpriteDoubleWidth(const uint8_t spriteNr);
    void disableSpriteDoubleWidth(const uint8_t spriteNr);
    bool isSpriteDoubleWidth(const uint8_t spriteNr);
    void enableSpriteDoubleHeight(const uint8_t spriteNr);
    void disableSpriteDoubleHeight(const uint8_t spriteNr);
    bool isSpriteDoubleHeight(const uint8_t spriteNr);
    uint8_t getSpriteBitmapBlock(const uint8_t spriteNr);
    void setSpriteBitmapBlock(const uint8_t spriteNr, const uint8_t bitmapBlock);
    struct Vector2ui getSpritePosition(const uint8_t spriteNr);
    void setSpritePosition(const uint8_t spriteNr, const struct Vector2ui position);
    struct Vector2ui clampSpritePositionToScreen(struct Vector2ui position);
    void copySpriteBitmap(volatile unsigned char* to, volatile unsigned char* from);
    void setSpritePixel(volatile unsigned char* bitmapPointer, const struct Vector2uis position);
    void clearSpritePixel(volatile unsigned char* bitmapPointer, const struct Vector2uis position);
    void applySpriteTemplate(const uint8_t spriteNr, const uint8_t bitmapBlock, const struct SpriteTemplate* spriteTemplate);
    void colorRectangularHighResBitmapRegion(volatile unsigned char* screenRamPointer, const struct Vector2ui topLeftCorner, const struct Vector2ui bottomRightCorner, const uint8_t foregroundColor, const uint8_t backgroundColor);
    void placeHighResBitmapTile(volatile unsigned char* bitmapPointer, volatile unsigned char* screenRamPointer, struct HighResBitmapTile tile, const struct Vector2uis gridCell);
    void switchToHighResBitmapMode();
    void setHighResBitmapPixel(volatile unsigned char* bitmapPointer, const struct Vector2ui position);
    void clearHighResBitmapPixel(volatile unsigned char* bitmapPointer, const struct Vector2ui position);
    void copyTile(volatile unsigned char* to, volatile unsigned char* from);
    void setBorderColor(const uint8_t color);
    void setBackgroundColor(const uint8_t color);
    
    #endif