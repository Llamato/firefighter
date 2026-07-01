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

#define SPRITE_GRID_WIDTH 2
#define SPRITE_GRID_HEIGHT 2

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
#define FLAME_LIFE_TIME 0x00000fff
#define PRIMERY_FLAME_COLOR COLOR_ORANGE
#define SECONDARY_FLAME_COLOR COLOR_BROWN
#define GAME_ONFIRE_COLOR COLOR_ORANGE
#define PRIMERY_WATER_COLOR COLOR_BLUE
#define SECONDARY_WATER_COLOR COLOR_LIGHT_BLUE
#define GAME_BORDER_COLOR COLOR_LIGHT_GREEN
#define PLAYER_SECONDARY_COLOR COLOR_LIGHT_GRAY
#define BURNED_POSITION_X 0
#define BURNED_POSITION_Y 0

struct FlameTarget {
    struct Vector2ui position;
    bool burned;
};

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

#define ROMANAS_HOUSE_WIDTH 5
#define ROMANAS_HOUSE_HEIGHT 4
#define ROMANAS_HOUSE_SIZE BYTES_PER_CHAR_BITMAP * ROMANAS_HOUSE_WIDTH * ROMANAS_HOUSE_HEIGHT
/*const unsigned char* romanasHouseTiles[ROMANAS_HOUSE_SIZE] = {
    #embed "RomanasHouse.bitmap" limit(ROMANAS_HOUSE_SIZE)
}*/

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

static struct HighResBitmapMultiTile house = {
    (struct HighResBitmapTile*) houseTiles,
    HOUSE_TILE_WIDTH,
    HOUSE_TILE_HEIGHT
};

static struct Vector2uis flameTargets[] = {
    //Grass positions
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
    {29, 25},

    //House positions
    {10, 5},
    {30, 5},
    {10, 20},
    {30, 16}
};

#define GRASS_COUNT 109
#define HOUSE_COUNT 4
#define TARGET_COUNT GRASS_COUNT + HOUSE_COUNT
const struct Vector2uis* grassPositions = &flameTargets[0];
const struct Vector2uis* housePositions = &flameTargets[GRASS_COUNT];

static struct Sprite playerSprite = {
    COLOR_YELLOW,
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

static struct AnimatedSpriteTemplate flameSpriteTemplates[] = {
    {
        PRIMERY_FLAME_COLOR,
        true,
        false,
        false,
        flameAnimationFrames,
        sizeof(flameAnimationFrames) / sizeof(volatile unsigned char*)
    },
    {
        PRIMERY_FLAME_COLOR,
        true,
        false,
        false,
        flameAnimationFrames,
        sizeof(flameAnimationFrames) / sizeof(volatile unsigned char*)
    }
};

bool areSpritesIntersecting(struct Vector2ui sprite1position, struct Vector2ui sprite2position) {
    return (sprite1position.x < sprite2position.x + SPRITE_COLUMNS) && (sprite2position.x < sprite1position.x + SPRITE_COLUMNS) && 
    (sprite1position.y < sprite2position.y + SPRITE_ROWS) && (sprite2position.y < sprite1position.y + SPRITE_ROWS);
}

static struct Rectangle2ui lakeBoundingBox;
void drawLake(const struct Vector2ui center, const uint8_t radius) {
    makeFilledCircleHighResBitmapBresenham(ADDRESS_TO_PTR(BITMAP_RAM), center, radius);
    lakeBoundingBox.topLeftCorner.x = center.x - radius;
    lakeBoundingBox.topLeftCorner.y = center.y - radius;
    lakeBoundingBox.bottomRightCorner.x = center.x + radius;
    lakeBoundingBox.bottomRightCorner.y = center.y + radius;
    colorRectangularHighResBitmapRegion(ADDRESS_TO_PTR(SCREEN_RAM), lakeBoundingBox, SECONDARY_WATER_COLOR, COLOR_GREEN);
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
static uint16_t selectedFlameTargets[FLAME_SPRITE_COUNT];
static struct Vector2ui currentFlamePositions[FLAME_SPRITE_COUNT];
static uint32_t flameLifetimes[FLAME_SPRITE_COUNT];
static int8_t flameSpawnPointMovementVelocity = 1;
static uint16_t burnedTargetCount = 0;
static uint8_t previousRng = 0;
uint16_t findTargetForFlame(const uint8_t flameNr) {
    uint8_t rng = previousRng;

    //This somehow hangs the entire program. Fine... lets not do it then...
    /*for(uint8_t rngAttempts = 0; rngAttempts < 10; rngAttempts++) {
        setBorderColor(COLOR_BLACK);
        rng = getRandomNumber();
        setBorderColor(GAME_BORDER_COLOR);
        if(rng != previousRng) break;
    }*/

    rng = getRandomNumber();
    previousRng = rng;
    rng += flameNr;
    return rng;
}

void spawnFlame(const uint8_t flameNr, const struct Vector2ui startPosition, const uint32_t lifetime) {
    const uint8_t spriteNr = flameNr +1;
    enableSprite(spriteNr);
    currentFlamePositions[flameNr] = startPosition;
    flameLifetimes[flameNr] = lifetime;
}

bool moveFlame(const uint8_t flameNr) {
    const uint8_t spriteNr = flameNr +1;
    const struct Vector2ui targetFlamePosition = targetFlamePositions[flameNr];
    struct Vector2ui nextFlamePosition = currentFlamePositions[flameNr];
    if(targetFlamePosition.x < nextFlamePosition.x) nextFlamePosition.x--;
    if(targetFlamePosition.x > nextFlamePosition.x) nextFlamePosition.x++;
    if(targetFlamePosition.y < nextFlamePosition.y) nextFlamePosition.y--;
    if(targetFlamePosition.y > nextFlamePosition.y) nextFlamePosition.y++;
    //if(nextFlamePosition.x > SPRITE_X_MAX) nextFlamePosition.x = SPRITE_X_MAX;
    //if(nextFlamePosition.x > SPRITE_Y_MAX) nextFlamePosition.y = SPRITE_Y_MAX;
    bool arrived = nextFlamePosition.x == targetFlamePosition.x && nextFlamePosition.y == targetFlamePosition.y;
    setSpritePosition(spriteNr, nextFlamePosition);
    currentFlamePositions[flameNr] = nextFlamePosition;
    return arrived;
}

void despwanFlame(const uint8_t flameNr) {
    const uint8_t spriteNr = flameNr +1;
    disableSprite(spriteNr);
    const struct Vector2ui zero = {0, 0};
    targetFlamePositions[flameNr] = zero;
    currentFlamePositions[flameNr] = zero;
}

void respawnFlame(const uint8_t flameNr) {
    struct Vector2ui restartingPosition = currentFlamePositions[flameNr];
    if(restartingPosition.x >= BITMAP_WIDTH) flameSpawnPointMovementVelocity = -1;
    restartingPosition.x += flameSpawnPointMovementVelocity;
    spawnFlame(flameNr, restartingPosition, FLAME_LIFE_TIME);
}

void initFlames(){
    const struct Vector2ui zero = {0, 0};
    for(uint8_t flameNr = 0; flameNr < FLAME_SPRITE_COUNT; flameNr++) {
        const uint8_t spriteNr = flameNr +1;
        targetFlamePositions[flameNr] = zero;
        currentFlamePositions[flameNr] = zero;
        flameLifetimes[flameNr] = FLAME_LIFE_TIME;
        selectedFlameTargets[flameNr] = findTargetForFlame(flameNr);
        targetFlamePositions[flameNr] = charGridPositionToSpritePosition(flameTargets[selectedFlameTargets[flameNr]]);
    }
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

void enlargeFlame(uint8_t flameNr) {
    const uint8_t spriteNr = flameNr +1;
    enableSpriteDoubleHeight(spriteNr);
    enableSpriteDoubleWidth(spriteNr);
}

bool isOnLakeShore(struct Vector2ui position) {
    const int16_t dxTL = position.x - lakeBoundingBox.topLeftCorner.x;
    const int16_t dyTL = position.y - lakeBoundingBox.topLeftCorner.y;
    const int16_t dxBR = position.x - lakeBoundingBox.bottomRightCorner.x;
    const int16_t dyBR = position.y - lakeBoundingBox.bottomRightCorner.y;
    return dxTL > -SPRITE_COLUMNS && dyTL > -SPRITE_COLUMNS && dxBR < 0 && dyBR < 0;
}

bool isOnLake(struct Vector2ui position) {
    const int16_t dxTL = position.x - lakeBoundingBox.topLeftCorner.x;
    const int16_t dyTL = position.y - lakeBoundingBox.topLeftCorner.y;
    const int16_t dxBR = position.x - lakeBoundingBox.bottomRightCorner.x;
    const int16_t dyBR = position.y - lakeBoundingBox.bottomRightCorner.y;
    return dxTL > 0 && dyTL > 0 && dxBR < -SPRITE_COLUMNS && dyBR < -SPRITE_ROWS;
}

bool extinglishAreaOfEffect(struct Vector2uis basePosition, struct Vector2uis areaOfEffect) {
    bool extinglished = false;
    struct Vector2uis areaOfEffectBorder = {basePosition.x + areaOfEffect.x, basePosition.y + areaOfEffect.y};
    for(uint8_t currentRow = basePosition.y; currentRow < areaOfEffectBorder.y; currentRow++) {
        if(currentRow >= TEXT_SCREEN_ROWS) continue; 
        for(uint8_t currentColumn = basePosition.x; currentColumn < areaOfEffectBorder.x; currentColumn++) {
            if(currentColumn >= TEXT_SCREEN_COLUMNS) continue;
            const struct Vector2uis currentPosition = {currentColumn, currentRow};
            const uint8_t colors = getHighResBitmapTileColors(ADDRESS_TO_PTR(SCREEN_RAM), currentPosition);
            const uint8_t foregroundColor = (colors & 0xf0) >> BITS_PER_NIBBLE;
            const uint8_t backgroundColor = colors & 0x0f;
            //Potential optimisations: Remove terms that can not be true
            if(foregroundColor == PRIMERY_FLAME_COLOR || foregroundColor == SECONDARY_FLAME_COLOR || backgroundColor == PRIMERY_FLAME_COLOR || backgroundColor == SECONDARY_FLAME_COLOR) {
                setHighResBitmapTileColors(ADDRESS_TO_PTR(SCREEN_RAM), currentPosition, (GRASS_FOREGROUND_COLOR << BITS_PER_NIBBLE) | GRASS_BACKGROUND_COLOR);
                extinglished = true;
            }
        }
    }
    return extinglished;
}

void looseGame() {
    uint8_t borderColor = 0;
    while(true) {
        setBorderColor(borderColor);
        borderColor++;
        if(borderColor == 0){
            flickerFlames();
        }
    }
}

int main(void) {
    //Init sid
    initNoiseVoiceRnd();
    
    //Init screen
    setBorderColor(GAME_BORDER_COLOR);
    switchToHighResBitmapMode();
    fillMemory(ADDRESS_TO_PTR(SCREEN_RAM), SCREEN_SIZE, (GRASS_FOREGROUND_COLOR << BITS_PER_NIBBLE) | GRASS_BACKGROUND_COLOR);
    fillMemory(ADDRESS_TO_PTR(BITMAP_RAM), BITMAP_SIZE, 0);
    
    //Init bitmap
    const struct Vector2ui lakeCenter = {BITMAP_WIDTH / 2, BITMAP_HEIGHT / 2};
    const uint8_t lakeRadius = BITMAP_HEIGHT / 4;
    drawLake(lakeCenter, lakeRadius);
    lakeBoundingBox.topLeftCorner = bitmapPositionToSpritePosition(lakeBoundingBox.topLeftCorner);
    lakeBoundingBox.bottomRightCorner = bitmapPositionToSpritePosition(lakeBoundingBox.bottomRightCorner);
    placeGrass((const struct Vector2uis*) grassPositions, GRASS_COUNT);
    placeHouses((const struct Vector2uis*) housePositions, HOUSE_COUNT);

    //Init sprites
    *ADDRESS_TO_PTR(SPRITES_ENABLE) = 0xff;
    setSharedMulticolorSpritesPrimeryColor(COLOR_BROWN);
    setSharedMulticolorSpritesSecondaryColor(PLAYER_SECONDARY_COLOR);
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
    bool playerIsOnFire = false;
    uint32_t playerEndurence = 0xffff; 
    uint32_t framecounter = 0;

    //Gameloop
    while (gamerunning) {

        //Flames
        const bool flameflickertime = (framecounter & FLAME_SPEED_GATE) == 0;
        if(flameflickertime) flickerFlames();
        for(uint8_t currentFlame = 0; currentFlame < FLAME_SPRITE_COUNT; currentFlame++) {
            const uint8_t currentSprite = currentFlame +1;
            const bool isFlameInGame = isSpriteEnabled(currentSprite);
            bool isFlameOnTarget = false;
            if(isFlameInGame && (isFlameOnTarget = moveFlame(currentFlame))) {
                flameLifetimes[currentFlame]--;
                setForegroundColorOfHighResBitmapTile(ADDRESS_TO_PTR(SCREEN_RAM), flameTargets[selectedFlameTargets[currentFlame]], PRIMERY_FLAME_COLOR);
                
            }
            if(isFlameInGame && flameLifetimes[currentFlame] == 0) {
                //setHighResBitmapTileColors(ADDRESS_TO_PTR(SCREEN_RAM), flameTargets[selectedFlameTargets[currentFlame]], PRIMERY_FLAME_COLOR << BITS_PER_NIBBLE | SECONDARY_FLAME_COLOR);
                //setBackgroundColorOfHighResBitmapTile(ADDRESS_TO_PTR(SCREEN_RAM), flameTargets[selectedFlameTargets[currentFlame]], SECONDARY_FLAME_COLOR);
                burnedTargetCount++;
                const int16_t substituteTargetIndex = sizeof(flameTargets) / sizeof(struct Vector2uis) - burnedTargetCount;
                if(substituteTargetIndex < 0) {
                    looseGame();
                }
                flameTargets[selectedFlameTargets[currentFlame]].x = flameTargets[substituteTargetIndex].x;
                flameTargets[selectedFlameTargets[currentFlame]].y = flameTargets[substituteTargetIndex].y;
                flameTargets[burnedTargetCount].x = BURNED_POSITION_X;
                flameTargets[burnedTargetCount].y = BURNED_POSITION_Y;

                //Avoid duplicate targets
                uint8_t duplicateCheck[FLAME_SPRITE_COUNT];
                uint8_t rollCount = 0; 
                reroll:
                rollCount++;
                for(uint8_t currentEntry = 0; currentEntry < FLAME_SPRITE_COUNT; currentEntry++) {
                    duplicateCheck[currentEntry] = 0;
                }

                selectedFlameTargets[currentFlame] = findTargetForFlame(currentFlame);
                targetFlamePositions[currentFlame] = charGridPositionToSpritePosition(flameTargets[selectedFlameTargets[currentFlame]]);
                
                //Avoid duplicate targets
                for(uint8_t currentEntry = 0; currentEntry < FLAME_SPRITE_COUNT; currentEntry++) {
                    for(uint8_t currentDuplicateCounter = 0; currentDuplicateCounter < FLAME_SPRITE_COUNT; currentDuplicateCounter++) {
                        if(targetFlamePositions[currentEntry].x == targetFlamePositions[currentDuplicateCounter].x && targetFlamePositions[currentEntry].y == targetFlamePositions[currentDuplicateCounter].y) {
                            duplicateCheck[currentDuplicateCounter]++;
                        }
                    }
                }
                if(rollCount < 10) { 
                    for(uint8_t currentEntry = 0; currentEntry < FLAME_SPRITE_COUNT; currentEntry++) {
                        if(duplicateCheck[currentEntry] > 1){
                            goto reroll;
                        }
                    }
                }

                setSpriteColor(currentSprite, PRIMERY_FLAME_COLOR);
                respawnFlame(currentFlame);
            }
        }
        flickerFlames();
        
        //Player
        const uint8_t joystickState = readJoystick1State() & readJoystick2State();
        const uint8_t keyboardState = readKeyboardState();
        const struct Vector2is joystickMovementVector = getMovementVectorFromJoystickState(joystickState);
        const struct Vector2is keyboardMovementVector = getMovementVectorFromKeyboardState(keyboardState);
        const struct Vector2is combinedMovementVector = {joystickMovementVector.x + keyboardMovementVector.x, joystickMovementVector.y + keyboardMovementVector.y};
        struct Vector2ui nextPlayerPositionCandidate = {playerSprite.position.x + combinedMovementVector.x, playerSprite.position.y + combinedMovementVector.y};
        nextPlayerPositionCandidate = clampSpritePositionToScreen(nextPlayerPositionCandidate);
        if(!isOnLake(nextPlayerPositionCandidate)) {
            playerSprite.position = nextPlayerPositionCandidate;
        }
        setSpritePosition(HARDWARE_PLAYER_SPRITE_INDEX, playerSprite.position);

        //Gameplay
        const uint8_t spriteSpriteCollisions = getSpriteSpriteCollisions();
        const uint8_t spriteBackgroundCollisions = getSpriteBackgroundCollisions();
        const bool frameLocked = false;
        const bool playerIsOnLakeShore = isOnLakeShore(playerSprite.position);
        const bool fireButtonIsPressed = isJoystickFirePressed(joystickState);
        const bool isPlayerInFire = spriteSpriteCollisions & (1 << HARDWARE_PLAYER_SPRITE_INDEX);
        if(fireButtonIsPressed && playerIsOnLakeShore) {
                playerIsOnFire = false;
                playerHasWater = true;
        }
        if(isPlayerInFire && playerHasWater && fireButtonIsPressed) {
            //fire extinglished (Temporary as hardware collision checks can not determine witch sprite is colliding with witch)
            for(uint8_t spriteNr = 1; spriteNr < HARDWARE_SPRITE_COUNT; spriteNr++) {
                if(spriteSpriteCollisions & (1 << spriteNr)) {
                    const uint8_t flameNr = spriteNr -1;
                    despwanFlame(flameNr);
                }
            }
            //Water consumed
            playerHasWater = false;
        }else if(playerHasWater && fireButtonIsPressed) {
            if(spriteBackgroundCollisions & (1 << HARDWARE_PLAYER_SPRITE_INDEX)) {
                const struct Vector2uis playerPositionOnGrid = spritePositionToCharGridPosition(playerSprite.position);
                const bool extinglished = extinglishAreaOfEffect(playerPositionOnGrid, (struct Vector2uis) {SPRITE_GRID_WIDTH, SPRITE_GRID_HEIGHT});
                if(extinglished) {
                    playerHasWater = false;
                }
            }  
        }
        /*else if(fireButtonIsPressed && !playerIsOnLakeShore) {
            //Drop water (Bug / inaccuracy in here were water is dropped before flame is extinglished)
            playerHasWater = false;
        }*/
        else if(isPlayerInFire && !playerHasWater) {
            playerIsOnFire = true;
        }
        
        //Apply States
        if(playerIsOnFire) {
            setSharedMulticolorSpritesSecondaryColor(GAME_ONFIRE_COLOR);
        } else if(playerHasWater) {
            setSharedMulticolorSpritesSecondaryColor(PRIMERY_WATER_COLOR);
        } else {
            setSharedMulticolorSpritesSecondaryColor(PLAYER_SECONDARY_COLOR);
        }

        //On to next frame
        framecounter++;
    }
}