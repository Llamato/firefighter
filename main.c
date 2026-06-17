#include <stdint.h>
#include <stdbool.h>
#include "glibs64c/common.h"
#include "glibs64c/gllm/gllm.h"
#include "glibs64c/graphics.h"
#include "glibs64c/hardware/sid.h"
#include "glibs64c/hardware/timer.h"
#include "glibs64c/hardware/vic.h"

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
#define HOUSE_UPPER_FOREGROUND_COLOR COLOR_LIGHT_RED
#define HOUSE_UPPER_BACKGROUND_COLOR COLOR_GREEN
#define HOUSE_LOWER_FOREGROUND_COLOR COLOR_LIGHT_RED
#define HOUSE_LOWER_INSIDE_BACKGROUND_COLOR COLOR_BROWN
#define HOUSE_LOWER_OUTSIDE_BACKGROUND_COLOR COLOR_GREEN

#define GRASS_FOREGROUND_COLOR COLOR_LIGHT_GREEN
#define GRASS_BACKGROUND_COLOR COLOR_GREEN

#define HARDWARE_PLAYER_SPRITE_INDEX 0
#define FLAME_SPRITE_COUNT HARDWARE_SPRITE_COUNT-1

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

const struct HighResBitmapTile grassTiles[] = {
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

const struct HighResBitmapTile houseTiles[] = {
    {
        (volatile unsigned char*) upperHouseTileTemplate,
        (uint8_t)(HOUSE_UPPER_FOREGROUND_COLOR << BITS_PER_NIBBLE) | HOUSE_UPPER_BACKGROUND_COLOR
    },
    {
        (volatile unsigned char*) upperHouseTileTemplate+BYTES_PER_CHAR_BITMAP,
        (uint8_t)(HOUSE_UPPER_FOREGROUND_COLOR << BITS_PER_NIBBLE) | HOUSE_UPPER_BACKGROUND_COLOR
    },
    {
        (volatile unsigned char*) upperHouseTileTemplate+BYTES_PER_CHAR_BITMAP*2,
        (uint8_t)(HOUSE_UPPER_FOREGROUND_COLOR << BITS_PER_NIBBLE) | HOUSE_UPPER_BACKGROUND_COLOR
    },
    {
        (volatile unsigned char*) lowerHouseTileTemplate,
        (uint8_t)(HOUSE_LOWER_FOREGROUND_COLOR << BITS_PER_NIBBLE) | HOUSE_LOWER_OUTSIDE_BACKGROUND_COLOR
    },
    {
        (volatile unsigned char*) lowerHouseTileTemplate+BYTES_PER_CHAR_BITMAP,
        (uint8_t)(HOUSE_LOWER_FOREGROUND_COLOR << BITS_PER_NIBBLE) | HOUSE_LOWER_INSIDE_BACKGROUND_COLOR
    },
    {
        (volatile unsigned char*) lowerHouseTileTemplate+BYTES_PER_CHAR_BITMAP*2,
        (uint8_t)(HOUSE_LOWER_FOREGROUND_COLOR << BITS_PER_NIBBLE) | HOUSE_LOWER_INSIDE_BACKGROUND_COLOR
    }
};

struct HighResBitmapMultiTile house = {
    (struct HighResBitmapTile*) houseTiles,
    HOUSE_TILE_WIDTH,
    HOUSE_TILE_HEIGHT
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
    {32, 22},
    {29, 0},
    {29, 1},
    {29, 2},
    {29, 3},
    {29, 4},
    {29, 5},
    {29, 6},
    {29, 7},
    {29, 8},
    {29, 9},
    {29, 10},
    {29, 11},
    {29, 12},
    {29, 13},
    {29, 14},
    {29, 15},
    {29, 16},
    {29, 17},
    {29, 18},
    {29, 19},
    {29, 20},
    {29, 21},
    {29, 22},
    {29, 23},
    {29, 24},
    {29, 25}
};

const struct Vector2uis housePositions[] = {
    {10, 5},
    {30, 5},
    {10, 20},
    {30, 16}
};

struct Sprite playerSprite = {
    (struct Vector2ui) {160, 200},
    COLOR_YELLOW,
    true,
    false, 
    false,
    ADDRESS_TO_PTR(SPRITE_BITMAP_ADDRESS(SPRITE_0_BLOCK))
};

volatile unsigned char* flameAnimationFrames[] = {
    ADDRESS_TO_PTR(SPRITE_BITMAP_ADDRESS(SPRITE_0_BLOCK)),
    ADDRESS_TO_PTR(SPRITE_BITMAP_ADDRESS(SPRITE_1_BLOCK))
};

struct AnimatedSprite flameSprites[] = {
    {
        (struct Vector2ui) {0, 0},
        COLOR_ORANGE,
        true,
        false,
        false,
        flameAnimationFrames,
        0,
        sizeof(flameAnimationFrames) / sizeof(volatile unsigned char*)
    },
    {
        (struct Vector2ui) {0, 0},
        COLOR_ORANGE,
        true,
        false,
        false,
        flameAnimationFrames,
        1,
        sizeof(flameAnimationFrames) / sizeof(volatile unsigned char*)
    }
};

const uint8_t spriteBlocks[] = {
    SPRITE_0_BLOCK,
    SPRITE_1_BLOCK,
    SPRITE_2_BLOCK,
    SPRITE_3_BLOCK,
    SPRITE_4_BLOCK,
    SPRITE_5_BLOCK,
    SPRITE_6_BLOCK,
    SPRITE_7_BLOCK
};

void drawLake(const struct Vector2ui center, const uint8_t radius) {
    makeFilledCircleHighResBitmapBresenham(ADDRESS_TO_PTR(BITMAP_RAM), center, radius);
    const struct Vector2ui leftTopCorner = {center.x - radius, center.y - radius};
    const struct Vector2ui rightBottomCorner = {center.x + radius, center.y + radius};
    colorRectangularHighResBitmapRegion(ADDRESS_TO_PTR(SCREEN_RAM),leftTopCorner, rightBottomCorner, COLOR_LIGHT_BLUE, COLOR_GREEN);
}

void placeGrass(const struct Vector2uis* gridPositions, const uint16_t amount) { 
    for(uint16_t currentPatchOfGrass = 0; currentPatchOfGrass < amount; currentPatchOfGrass++) {
        placeHighResBitmapTile(ADDRESS_TO_PTR(BITMAP_RAM), ADDRESS_TO_PTR(SCREEN_RAM), grassTiles[currentPatchOfGrass % (sizeof(grassTiles) / sizeof(struct HighResBitmapTile))], gridPositions[currentPatchOfGrass]);
    }
}

void placeHouses(const struct Vector2uis* gridPositions, const uint8_t amount) {
    for(uint8_t currentHouse = 0; currentHouse < amount; currentHouse++) {
        for(uint8_t currentRow = 0; currentRow < house.height; currentRow++) {
            for(uint8_t currentColumn = 0; currentColumn < house.width; currentColumn++) {
                placeHighResBitmapTile(ADDRESS_TO_PTR(BITMAP_RAM), ADDRESS_TO_PTR(SCREEN_RAM), house.tiles[currentRow * house.width + currentColumn], (struct Vector2uis) {gridPositions[currentHouse].x + currentColumn, gridPositions[currentHouse].y + currentRow});
            }
        }
    }
}

struct AnimatedSprite* activeSprites[FLAME_SPRITE_COUNT];
struct Vector2ui activeTargets[FLAME_SPRITE_COUNT];

bool isTargetActive(const struct Vector2uis target) {
    for(uint16_t currentTarget = 0; currentTarget < FLAME_SPRITE_COUNT; currentTarget++) {
        if(activeTargets[currentTarget].x == target.x && activeTargets[currentTarget].y == target.y) {
            return true;
        }
    }
    return false;
}

uint16_t findTargetForFlame(const struct Vector2ui flamePosition, const struct Vector2uis* targets, const uint16_t targetCount) {
    uint16_t minSum = BITMAP_WIDTH * BITMAP_HEIGHT;
    uint16_t minTarget = 0;
    for(uint16_t currentTarget = 0; currentTarget < targetCount; currentTarget++) {
        const uint16_t currentTargetSum = abs(targets[currentTarget].x - flamePosition.x) + abs(targets[currentTarget].y - flamePosition.y);
        if(currentTargetSum < minSum && !isTargetActive(targets[currentTarget])) {
            minSum = currentTargetSum;
            minTarget = currentTarget;
        }
    }
}

void spawnFlame() {
    for(uint8_t currentSprite = 0; currentSprite < FLAME_SPRITE_COUNT; currentSprite++) {
        if(activeTargets[currentSprite].x == 0 && activeTargets[currentSprite].y == 0) {
            uint16_t target = findTargetForFlame(activeSprites[currentSprite]->position, housePositions, sizeof(housePositions) / sizeof(struct Vector2uis));
            activeSprites[currentSprite]->position = getVoice3State();
        }

    }
}

void despawnFlame(const uint8_t hwSprite) {
    
}

void flickerFlame(const uint8_t sprite) {
    
}

volatile uint16_t framecounter = 0;
int main(void) {
    //Init sid
    initNoiseVoiceRnd(UINT16_MAX);

    //Init timer
    setTimerLatch(ADDRESS_TO_PTR(CIA1_BASE_ADDRESS), TIMERA_INDEX, 0xff);

    //Init screen
    setBorderColor(COLOR_BLACK);
    switchToHighResBitmapMode();
    fillMemory(ADDRESS_TO_PTR(SCREEN_RAM), SCREEN_SIZE, (COLOR_LIGHT_GREEN << BITS_PER_NIBBLE) | COLOR_GREEN);
    fillMemory(ADDRESS_TO_PTR(BITMAP_RAM), BITMAP_SIZE, 0);
    
    //Init bitmap
    drawLake((struct Vector2ui) {BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2}, BITMAP_HEIGHT / 4);
    placeGrass((const struct Vector2uis*) grassPositions, sizeof(grassPositions) / sizeof(struct Vector2uis));
    placeHouses((const struct Vector2uis*) housePositions, sizeof(housePositions) / sizeof(struct Vector2uis));

    //Init sprites
    setSharedMulticolorSpriteColors(COLOR_BROWN, COLOR_LIGHT_GRAY);
    bindSprite(HARDWARE_PLAYER_SPRITE_INDEX, SPRITE_0_BLOCK, &playerSprite);
    copySpriteBitmap(playerSprite.bitmapPtr, (volatile unsigned char*) playerSprite1Template);
    const uint8_t flameTemplateCount = sizeof(flameSprites) / sizeof(struct AnimatedSprite);
    for(uint8_t currentSprite = 0; currentSprite < FLAME_SPRITE_COUNT; currentSprite++) {
        const uint8_t flameIndex = currentSprite % flameTemplateCount;
        activeSprites[currentSprite] = &flameSprites[flameIndex];
        activeTargets[currentSprite] = (struct Vector2ui) {0, 0};
    }

    //Gameloop
    bool gamerunning = true;
    volatile bool flametime = true;
    while (gamerunning) {
        if(flametime) {
            spawnFlame((struct Vector2ui) {framecounter % BITMAP_WIDTH, 0});
        }
        framecounter++;
    }
}