#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define INPUT_FILE_PARAMETER_INDEX 1
#define OUTPUT_BITMAP_FILE_PARAMETER_INDEX 2
#define OUTPUT_COLOR_FILE_PARAMETER_INDEX 3
#define USER_COMMANDS_PARAMETER_INDEX 4
#define TILE_SIZE 8
#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 25
#define FILE_HEADER_SIZE 2
#define TILE_COUNT SCREEN_WIDTH * SCREEN_HEIGHT
#define COLOR_DATA_SIZE TILE_COUNT
#define BITMAP_DATA_SIZE COLOR_DATA_SIZE * TILE_SIZE

uint8_t inputBitmapData[BITMAP_DATA_SIZE];
uint8_t inputColorData[COLOR_DATA_SIZE];
uint8_t outputBitmapData[BITMAP_DATA_SIZE] = { 0 };
uint8_t outputColorData[COLOR_DATA_SIZE] = { 0 };
uint16_t nonEmptyTileCount = 0;

bool isBitmapTileNotExclusively(const uint16_t tileStart, const uint8_t rowValue) {
    for(uint8_t currentByte = 0; currentByte < TILE_SIZE; currentByte++) {
        if(inputBitmapData[tileStart + currentByte] != rowValue) {
            return true;
        }
    }
    return false;
}

void copyBitmapTile(const uint16_t sourceTileStart, const uint16_t destinationTileStart) {
    for(uint8_t currentByte = 0; currentByte < TILE_SIZE; currentByte++) {
        outputBitmapData[sourceTileStart + currentByte] = inputBitmapData[destinationTileStart + currentByte];
    }
}

int main(int argc, char* argv[]) {
    //Preamble
    if(argc < 4) {
        fprintf(stderr, "Please pass the input art file and an output bitmap file aswell as output color file path as program arguments.\n");
        fprintf(stderr, "Pass flip as the last parameter to remove fully forground colored tiles instead of fully background colord tiles.\n");
        return 1;
    }
    uint8_t emptyRowValue = 0x00;
    if(argc >= 5) {
        if(!strcmp(argv[USER_COMMANDS_PARAMETER_INDEX], "flip")) {
            emptyRowValue = 0xff;
        }
    }

    //Input
    FILE* inputFile = fopen(argv[INPUT_FILE_PARAMETER_INDEX], "r");
    fseek(inputFile, FILE_HEADER_SIZE, SEEK_SET);
    const size_t inputBitmapDataSize = fread(inputBitmapData, TILE_SIZE, TILE_COUNT, inputFile);
    const size_t inputColorDataSize = fread(inputColorData, 1, TILE_COUNT, inputFile);
    fclose(inputFile);

    //Processing
    for(uint16_t currentTile = 0; currentTile < TILE_COUNT; currentTile++) {
        const uint16_t currentTileStart = currentTile * TILE_SIZE;
        if(isBitmapTileNotExclusively(currentTileStart, emptyRowValue)) {
            const uint16_t destination = nonEmptyTileCount * TILE_SIZE;
            copyBitmapTile(currentTileStart, destination);
            outputColorData[nonEmptyTileCount] = inputColorData[currentTile];
            nonEmptyTileCount++;
        }
    }

    //Output
    FILE* outputBitmapFile = fopen(argv[OUTPUT_BITMAP_FILE_PARAMETER_INDEX], "w");
    FILE* outputColorFile = fopen(argv[OUTPUT_COLOR_FILE_PARAMETER_INDEX], "w");
    const size_t outputBitmapDataSize = fwrite(outputBitmapData, TILE_SIZE, nonEmptyTileCount, outputBitmapFile);
    const size_t outputColorDataSize = fwrite(outputColorData, 1, nonEmptyTileCount, outputColorFile);
    fclose(outputBitmapFile);
    fclose(outputColorFile);
    printf("Input bitmap tiles read: %Iu\n", inputBitmapDataSize);
    printf("Input color bytes read: %Iu\n", inputColorDataSize);
    printf("\n");
    printf("Output bitmap tiles written: %Iu\n", outputBitmapDataSize);
    printf("Output color bytes written: %Iu\n", outputColorDataSize);
    return 0;
}