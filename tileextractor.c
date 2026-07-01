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

int main(int argc, char* argv[]) {
    
}