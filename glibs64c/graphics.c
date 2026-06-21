#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "gllm/gllm.h"
#include "hardware/vic.h"
#include "graphics.h"

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

uint16_t getLinePixelCoordinatesBresenham(struct Vector2ui* coordinates, const struct Vector2ui origin, const struct Vector2ui destination) {
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
    uint16_t currentPoint = 0;
    while(1) {
        if (x >= 0 && x < BITMAP_WIDTH && y >= 0 && y < BITMAP_HEIGHT) {
            coordinates[currentPoint] = (struct Vector2ui) {x, y};
            currentPoint++;
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
    return currentPoint;
}