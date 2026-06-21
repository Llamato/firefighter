#include "gllm/gllm.h"
#ifndef GRAPHICS_H
    #include <stdint.h>
    #define GRAPHICS_H

    void mirrorCircleSpriteSegment(volatile unsigned char* bitmapPointer, const struct Vector2uis center, const struct Vector2uis circumfrancePoint);
    void makeCircleSpriteBresenham(volatile unsigned char* bitmapPointer, const struct Vector2uis center, const uint8_t radius);
    void makeLineSpriteBresenham(volatile unsigned char* bitmapPointer, const struct Vector2uis origin, const struct Vector2uis destination);
    void mirrorCircleHighResBitmapSegment(volatile unsigned char* bitmapPointer, const struct Vector2ui center, const struct Vector2ui circumfrancePoint);
    void makeCircleHighResBitmapBresenham(volatile unsigned char* bitmapPointer, const struct Vector2ui center, const uint8_t radius);
    void makeLineHighResBitmapBresenham(volatile unsigned char* bitmapPointer, const struct Vector2ui origin, const struct Vector2ui destination);
    void fillCircleHighResBitmapSegment(volatile unsigned char* bitmapPointer, const struct Vector2ui center, const struct Vector2ui circumfrancePoint);
    void makeFilledCircleHighResBitmapBresenham(volatile unsigned char* bitmapPointer, const struct Vector2ui center, const uint8_t radius);
    void colorRectangularHighResBitmapRegion(volatile unsigned char* screenRamPointer, const struct Vector2ui topLeftCorner, const struct Vector2ui bottomRightCorner, const uint8_t foregroundColor, const uint8_t backgroundColor);
    uint16_t getLinePixelCoordinatesBresenham(struct Vector2ui* coordinates, const struct Vector2ui origin, const struct Vector2ui destination);
#endif