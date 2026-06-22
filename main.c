#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "glibs64c/common.h"
#include "glibs64c/gllm/gllm.h"
#include "glibs64c/graphics.h"
#include "glibs64c/hardware/cia.h"
#include "glibs64c/hardware/sid.h"
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
#define FLAME_SPEED_GATE 0xffff
#define FLAME_LIFE_TIME 0xffffff

#define GAME_BORDER_COLOR COLOR_BLACK

const unsigned char flameSprite1Bitmap[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = { 
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(0)
};

const unsigned char flameSprite2Bitmap[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(1)
};

const unsigned char playerSprite1Bitmap[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(10)
};

const unsigned char playerSprite2Bitmap[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(11)
};

const unsigned char playerSprite3Bitmap[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(12)
};

const unsigned char playerSprite4Bitmap[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(13)
};

const unsigned char playerSprite5Bitmap[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(14)
};

const unsigned char playerSprite6Template[SPRITE_SIZE] /*__attribute__((aligned(SPRITE_SIZE)))*/ = {
    #embed "assets/mysprites.prg" SPRITE_EMBED_PARAMS(15)
};

const unsigned char grassTile1Bitmap[BYTES_PER_CHAR_BITMAP] /*__attribute__((aligned(BYTES_PER_CHAR_BITMAP)))*/ = {
    #embed "assets/background.art" TILE_EMBED_PARAMS(0)
};

const unsigned char grassTile2Bitmap[BYTES_PER_CHAR_BITMAP] /*__attribute__((aligned(BYTES_PER_CHAR_BITMAP)))*/ = {
    #embed "assets/background.art" TILE_EMBED_PARAMS(1)
};

const unsigned char grassTile3Bitmap[BYTES_PER_CHAR_BITMAP] /*__attribute__((aligned(BYTES_PER_CHAR_BITMAP)))*/ = {
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
        (volatile unsigned char*)grassTile1Bitmap,
        (uint8_t)(COLOR_LIGHT_GREEN << BITS_PER_NIBBLE) | COLOR_GREEN
    },
    {
        (volatile unsigned char*)grassTile2Bitmap,
        (uint8_t)(COLOR_LIGHT_GREEN << BITS_PER_NIBBLE) | COLOR_GREEN
    },
    {
        (volatile unsigned char*)grassTile3Bitmap,
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
    COLOR_BROWN,
    true,
    false, 
    false,
    (struct Vector2ui) {160, 200},
    ADDRESS_TO_PTR(SPRITE_BITMAP_ADDRESS(SPRITE_0_BLOCK))
};

volatile unsigned char* flameAnimationFrames[] = {
    ADDRESS_TO_PTR(SPRITE_BITMAP_ADDRESS(SPRITE_1_BLOCK)),
    ADDRESS_TO_PTR(SPRITE_BITMAP_ADDRESS(SPRITE_2_BLOCK))
};

struct AnimatedSpriteTemplate flameSpriteTemplates[] = {
    {
        COLOR_ORANGE,
        true,
        false,
        false,
        flameAnimationFrames,
        sizeof(flameAnimationFrames) / sizeof(volatile unsigned char*)
    },
    {
        COLOR_ORANGE,
        true,
        false,
        false,
        flameAnimationFrames,
        sizeof(flameAnimationFrames) / sizeof(volatile unsigned char*)
    }
};
static struct Vector2ui leftTopLakeCorner;
static struct Vector2ui rightBottomLakeCorner;
void drawLake(const struct Vector2ui center, const uint8_t radius) {
    makeFilledCircleHighResBitmapBresenham(ADDRESS_TO_PTR(BITMAP_RAM), center, radius);
    leftTopLakeCorner.x = center.x - radius;
    leftTopLakeCorner.y = center.y - radius;
    rightBottomLakeCorner.x = center.x + radius;
    rightBottomLakeCorner.y = center.y + radius;
    colorRectangularHighResBitmapRegion(ADDRESS_TO_PTR(SCREEN_RAM),leftTopLakeCorner, rightBottomLakeCorner, COLOR_LIGHT_BLUE, COLOR_GREEN);
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

static struct Vector2ui targetFlamePositions[FLAME_SPRITE_COUNT];
static struct Vector2ui currentFlamePositions[FLAME_SPRITE_COUNT];
static uint32_t flameLifetimes[FLAME_SPRITE_COUNT];
static int8_t flameSpawnPointMovementVelocity = 1;
void initFlames(){
    const struct Vector2ui zero = {0, 0};
    for(uint8_t currentFlame = 0; currentFlame < FLAME_SPRITE_COUNT; currentFlame++) {
        targetFlamePositions[currentFlame] = zero;
        currentFlamePositions[currentFlame] = zero;
        flameLifetimes[currentFlame] = 1;
    }
}

void spawnFlame(const uint8_t flameNr, const struct Vector2ui startPosition, const uint32_t lifetime) {
    const uint8_t spriteNr = flameNr +1;
    targetFlamePositions[flameNr] = (struct Vector2ui) {45 * spriteNr, 28 * spriteNr};
    currentFlamePositions[flameNr] = startPosition;
    flameLifetimes[flameNr] = lifetime;
}

bool moveFlame(const uint8_t flameNr) {
    const uint8_t spriteNr = flameNr +1;
    const struct Vector2ui targetFlamePosition = targetFlamePositions[flameNr];
    const struct Vector2ui currentFlamePosition = currentFlamePositions[flameNr];
    struct Vector2ui nextFlamePosition = {0, 0};
    if(targetFlamePosition.x < currentFlamePosition.x) nextFlamePosition.x = currentFlamePosition.x -1;
    if(targetFlamePosition.x > currentFlamePosition.x) nextFlamePosition.x = currentFlamePosition.x +1;
    if(targetFlamePosition.y < currentFlamePosition.y) nextFlamePosition.y = currentFlamePosition.y -1;
    if(targetFlamePosition.y > currentFlamePosition.y) nextFlamePosition.y = currentFlamePosition.y +1;
    setSpritePosition(spriteNr, nextFlamePosition);
    currentFlamePositions[flameNr] = nextFlamePosition;
    bool arrived = nextFlamePosition.x == targetFlamePosition.x && nextFlamePosition.y == targetFlamePosition.y;
    return arrived;
}

void despwanFlame(const uint8_t flameNr) {
    const struct Vector2ui zero = {0, 0};
    const uint8_t spriteNr = flameNr +1;
    targetFlamePositions[flameNr] = zero;
    currentFlamePositions[flameNr] = zero;
}

void respawnFlame(const uint8_t flameNr) {
    struct Vector2ui restartingPosition = currentFlamePositions[flameNr];
    if(restartingPosition.x >= BITMAP_WIDTH) flameSpawnPointMovementVelocity = -1;
    if(restartingPosition.x <= 0) flameSpawnPointMovementVelocity = 1;
    restartingPosition.x += flameSpawnPointMovementVelocity;
    spawnFlame(flameNr, restartingPosition, FLAME_LIFE_TIME);
}

void flickerFlames() {
    const uint8_t temp = *ADDRESS_TO_PTR(SPRITE_1_PTR);
    *ADDRESS_TO_PTR(SPRITE_1_PTR) = *ADDRESS_TO_PTR(SPRITE_2_PTR);
    *ADDRESS_TO_PTR(SPRITE_2_PTR) = *ADDRESS_TO_PTR(SPRITE_3_PTR);
    *ADDRESS_TO_PTR(SPRITE_3_PTR) = *ADDRESS_TO_PTR(SPRITE_4_PTR);
    *ADDRESS_TO_PTR(SPRITE_4_PTR) = *ADDRESS_TO_PTR(SPRITE_5_PTR);
    *ADDRESS_TO_PTR(SPRITE_5_PTR) = *ADDRESS_TO_PTR(SPRITE_6_PTR);
    *ADDRESS_TO_PTR(SPRITE_6_PTR) = *ADDRESS_TO_PTR(SPRITE_7_PTR);
    *ADDRESS_TO_PTR(SPRITE_7_PTR) = temp;
}

struct Vector2ui computeNextPlayerPosition(uint8_t joystickState) {
    const struct Vector2is movementVector = getMovementVectorFromJoystickState(joystickState);
    struct Vector2ui nextPosition = {playerSprite.position.x + movementVector.x, playerSprite.position.y + movementVector.y};
    nextPosition = clampSpritePositionToScreen(nextPosition);
    return nextPosition;
}

bool isOnLakeShore(struct Vector2ui position) {
    const int16_t dxTL = position.x - leftTopLakeCorner.x;
    const int16_t dyTL = position.y - leftTopLakeCorner.y;
    const int16_t dxBR = position.x - rightBottomLakeCorner.x;
    const int16_t dyBR = position.y - rightBottomLakeCorner.y;
    return dxTL > -SPRITE_COLUMNS && dyTL > -SPRITE_COLUMNS && dxBR < 0 && dyBR < 0;
}

bool isOnLake(struct Vector2ui position) {
    const int16_t dxTL = position.x - leftTopLakeCorner.x;
    const int16_t dyTL = position.y - leftTopLakeCorner.y;
    const int16_t dxBR = position.x - rightBottomLakeCorner.x;
    const int16_t dyBR = position.y - rightBottomLakeCorner.y;
    return dxTL > 0 && dyTL > 0 && dxBR < -SPRITE_COLUMNS && dyBR < -SPRITE_ROWS;
}

int main(void) {
    //Init sid
    initNoiseVoiceRnd(UINT16_MAX);
    
    //Init screen
    setBorderColor(GAME_BORDER_COLOR);
    switchToHighResBitmapMode();
    fillMemory(ADDRESS_TO_PTR(SCREEN_RAM), SCREEN_SIZE, (COLOR_LIGHT_GREEN << BITS_PER_NIBBLE) | COLOR_GREEN);
    fillMemory(ADDRESS_TO_PTR(BITMAP_RAM), BITMAP_SIZE, 0);
    
    //Init bitmap
    const struct Vector2ui lakeCenter = {BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2};
    const uint8_t lakeRadius = BITMAP_HEIGHT / 4;
    drawLake(lakeCenter, lakeRadius);
    leftTopLakeCorner = bitmapPositionToSpritePosition(leftTopLakeCorner);
    rightBottomLakeCorner = bitmapPositionToSpritePosition(rightBottomLakeCorner);
    placeGrass((const struct Vector2uis*) grassPositions, sizeof(grassPositions) / sizeof(struct Vector2uis));
    placeHouses((const struct Vector2uis*) housePositions, sizeof(housePositions) / sizeof(struct Vector2uis));

    //Init sprites
    *ADDRESS_TO_PTR(SPRITES_ENABLE) = 0xff;
    setSharedMulticolorSpriteColors(COLOR_BROWN, COLOR_LIGHT_GRAY);
    enableSprite(HARDWARE_PLAYER_SPRITE_INDEX);
    applySpriteTemplate(HARDWARE_PLAYER_SPRITE_INDEX, SPRITE_0_BLOCK, (const struct SpriteTemplate*) &playerSprite);
    copySpriteBitmap(playerSprite.bitmapPtr, (volatile unsigned char*) playerSprite1Bitmap);
    setSpritePosition(HARDWARE_PLAYER_SPRITE_INDEX, playerSprite.position);
    copySpriteBitmap(flameAnimationFrames[0], (volatile unsigned char*) flameSprite1Bitmap);
    copySpriteBitmap(flameAnimationFrames[1], (volatile unsigned char*) flameSprite2Bitmap);
    for(uint8_t currentSprite = 1; currentSprite < HARDWARE_SPRITE_COUNT; currentSprite++) {
        uint8_t odd = currentSprite & 1;
        applySpriteTemplate(currentSprite, odd ? SPRITE_1_BLOCK : SPRITE_2_BLOCK, (struct SpriteTemplate*) &flameSpriteTemplates[odd]);
    }

    //Final gamestate init
    initFlames();
    bool gamerunning = true;
    bool playerHasWater = false;
    uint32_t framecounter = 0;
    //TODO: Solve diagonal movement problem using movement accumulator (and fixed point math?)

    //Gameloop
    while (gamerunning) {

        //Flames
        const bool flameflickertime = (framecounter & FLAME_SPEED_GATE) == 0;
        if(flameflickertime) flickerFlames();
        for(uint8_t currentFlame = 0; currentFlame < FLAME_SPRITE_COUNT; currentFlame++) {
            const bool isFlameOnTarget = moveFlame(currentFlame);
            if(isFlameOnTarget) {
                flameLifetimes[currentFlame]--;
            }
            if(flameLifetimes[currentFlame] == 0) {
                flameLifetimes[currentFlame] = FLAME_LIFE_TIME;
                respawnFlame(currentFlame);
                break;
            }
        }
        
        //Player
        const uint8_t joystickState = readJoystick1State() & readJoystick2State();
        bool playerIsOnLakeShore = isOnLakeShore(playerSprite.position);
        const struct Vector2ui nextPlayerPositionCandidate = computeNextPlayerPosition(joystickState);
        if(!isOnLake(nextPlayerPositionCandidate)) {
            playerSprite.position = nextPlayerPositionCandidate;
        }
        setSpritePosition(HARDWARE_PLAYER_SPRITE_INDEX, playerSprite.position);

        //Gameplay
        if(isJoystickFirePressed(joystickState) && playerIsOnLakeShore) {
            playerHasWater = true;
        }
        setBorderColor(playerHasWater ? COLOR_BLUE : GAME_BORDER_COLOR);
        
        //On to next frame
        framecounter++;
    }
}