#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "gllm/gllm.h"
#include "graphics.h"

#define SPRITE_0_BLOCK 252
#define SPRITE_1_BLOCK 254
#define SPRITE_2_BLOCK 237
#define SPRITE_3_BLOCK 239
#define SPRITE_4_BLOCK 248
#define SPRITE_5_BLOCK 216
#define SPRITE_6_BLOCK 224
#define SPRITE_7_BLOCK 250

#define HOUSE_TILE_WIDTH 3
#define HOUSE_TILE_HEIGHT 2

struct Sprite {
    struct Vector2ui position;
    uint8_t color;
    volatile unsigned char* bitmapPtr;
    bool isMulticolor;
    bool isDoubleWidth;
    bool isDoubleHeight;
};

const unsigned char flameSprite1Template[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = { 
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(0)
};

const unsigned char flameSprite2Template[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(1)
};

const unsigned char playerSprite1Template[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(10)
};

const unsigned char playerSprite2Template[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(11)
};

const unsigned char playerSprite3Template[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(12)
};

const unsigned char playerSprite4Template[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(13)
};

const unsigned char playerSprite5Template[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(14)
};

const unsigned char playerSprite6Template[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(15)
};

const unsigned char grassTile1Template[BYTES_PER_CHAR_BITMAP] /*__attribute__((aligned(BYTES_PER_CHAR_BITMAP)))*/ = {
    #embed "assets/background.art" TILE_EMBED_PARAMS(0)
};

const unsigned char grassTile2Template[BYTES_PER_CHAR_BITMAP] /*__attribute__((aligned(BYTES_PER_CHAR_BITMAP)))*/ = {
    #embed "assets/background.art" TILE_EMBED_PARAMS(1)
};

const unsigned char grassTile3Template[BYTES_PER_CHAR_BITMAP] /*__attribute__((aligned(BYTES_PER_CHAR_BITMAP)))*/ = {
    #embed "assets/background.art" TILE_EMBED_PARAMS(2)
};

const unsigned char upperHouseTileTemplate[BYTES_PER_CHAR_BITMAP * HOUSE_TILE_WIDTH] = {
    #embed "assets/background.art" clang::offset((4 * TEXT_SCREEN_COLUMNS + 7) * BYTES_PER_CHAR_BITMAP) limit(4 * BYTES_PER_CHAR_BITMAP)
};

const unsigned char lowerHouseTileTemplate[BYTES_PER_CHAR_BITMAP * HOUSE_TILE_WIDTH] = {
    #embed "assets/background.art" clang::offset((5 * TEXT_SCREEN_COLUMNS + 7) * BYTES_PER_CHAR_BITMAP) limit(4 * BYTES_PER_CHAR_BITMAP)
};

struct HighResBitmapTile grassTiles[] = {
    {
        (volatile unsigned char*)grassTile1Template,
        (uint8_t)(COLOR_LIGHT_GREEN << BITS_PER_NIBBLE) | COLOR_GREEN
    },
    {
        (volatile unsigned char*)grassTile2Template,
        (uint8_t)(COLOR_LIGHT_GREEN << BITS_PER_NIBBLE) | COLOR_GREEN
    },
    {
        (volatile unsigned char*)grassTile3Template,
        (uint8_t)(COLOR_LIGHT_GREEN << BITS_PER_NIBBLE) | COLOR_GREEN
    }
};

const struct Vector2uis grassPositions[] = {
    {13, 1},
    {17, 1},
    {20, 1},
    {4, 2},
    {7, 2},
    {17, 2},
    {22, 2},
    {18, 3},
    {22, 3},
    {36, 3},
    {37, 3},
    {3, 4},
    {15, 4},
    {16,4},
    {36, 4},
    {37, 4},
    {3, 5},
    {4, 5},
    {6, 5},
    {31, 5},
    {36, 6},
    {13, 8},
    {36, 8},
    {37, 8},
    {12, 9},
    {38,9},
    {39, 9},
    {1, 10},
    {31, 10},
    {35, 10},
    {36, 10},
    {6, 11},
    {8, 11},
    {9, 11},
    {11, 11},
    {12, 11},
    {31, 11},
    {32, 11},
    {33, 11},
    {35, 11},
    {36, 11},
    {2, 12},
    {3, 12},
    {1, 13},
    {4, 13},
    {13, 13},
    {1, 14},
    {4, 14},
    {13, 14},
    {7, 15},
    {2, 16},
    {3, 16},
    {11, 16},
    {12, 16},
    {31, 16},
    {37, 16},
    {2, 17},
    {3, 17},
    {8, 17},
    {11, 17},
    {12, 17},
    {35, 17},
    {36, 17},
    {37, 17},
    {4, 18},
    {8, 18},
    {11, 18},
    {12, 18},
    {35, 18},
    {4, 19},
    {8, 19},
    {15, 19},
    {17, 19},
    {10, 20},
    {11, 20},
    {28, 20},
    {29, 20},
    {2, 21},
    {31, 21},
    {32, 21},
    {2, 22},
    {31, 22},
    {32, 22}
};

void bindSprite(const uint8_t spriteNr, const uint8_t bitmapBlock, const struct Sprite* spriteStruct) {
    enableSprite(spriteNr);
    setSpriteColor(spriteNr, spriteStruct->color);
    setSpriteBitmapPointer(spriteNr, bitmapBlock);
    positionSprite(spriteNr, spriteStruct->position);
    if(spriteStruct->isMulticolor) {
        enableSpriteMulticolorMode(spriteNr);
    } else {
        disableSpriteMulticolorMode(spriteNr);
    }
    if(spriteStruct->isDoubleWidth) {
        enableSpriteDoubleWidth(spriteNr);
    } else {
        disableSpriteDoubleWidth(spriteNr);
    }
    if(spriteStruct->isDoubleHeight) {
        enableSpriteDoubleHeight(spriteNr);
    } else {
        disableSpriteDoubleHeight(spriteNr);
    }
}

void drawLake(const struct Vector2ui center, const uint8_t radius) {
    makeFilledCircleHighResBitmapBresenham(ADDRESS_TO_PTR(BITMAP_RAM), center, radius);
    const struct Vector2ui leftTopCorner = {center.x - radius, center.y - radius};
    const struct Vector2ui rightBottomCorner = {center.x + radius, center.y + radius};
    colorRectangularHighResBitmapRegion(ADDRESS_TO_PTR(SCREEN_RAM),leftTopCorner, rightBottomCorner, COLOR_LIGHT_BLUE, COLOR_GREEN);
}

void placeGrass(const struct Vector2uis* gridPositions, const uint16_t amount) { 
    for(uint8_t currentPatchOfGrass = 0; currentPatchOfGrass < amount; currentPatchOfGrass++) {
        placeHighResBitmapTile(ADDRESS_TO_PTR(BITMAP_RAM), ADDRESS_TO_PTR(SCREEN_RAM), grassTiles[currentPatchOfGrass % (sizeof(grassTiles) / sizeof(struct HighResBitmapTile))], gridPositions[currentPatchOfGrass]);
    }
}

int main(void) {
    //Init screen
    setBorderColor(COLOR_BLACK);
    switchToHighResBitmapMode();
    fillMemory(ADDRESS_TO_PTR(SCREEN_RAM), SCREEN_SIZE, (COLOR_RED << BITS_PER_NIBBLE) | COLOR_GREEN);
    fillMemory(ADDRESS_TO_PTR(BITMAP_RAM), BITMAP_SIZE, 0);
    
    //Init bitmap
    drawLake((struct Vector2ui) {BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2}, BITMAP_HEIGHT / 4);
    placeGrass((const struct Vector2uis*) &grassPositions, sizeof(grassPositions) / sizeof(struct Vector2uis));

    //Init sprites
    setSharedMulticolorSpriteColors(COLOR_BROWN, COLOR_LIGHT_GRAY);
    struct Sprite playerSprite = {
        (struct Vector2ui) {160, 200},
        COLOR_YELLOW,
        ADDRESS_TO_PTR(SPRITE_BITMAP_ADDRESS(SPRITE_0_BLOCK)),
        true,
        false, 
        false,
    };
    bindSprite(0, SPRITE_0_BLOCK, &playerSprite);
    copySpriteBitmap(playerSprite.bitmapPtr, (volatile unsigned char*) playerSprite1Template);

    //Gameloop
    bool gamerunning = true;
    while (gamerunning) {
        
    }
}