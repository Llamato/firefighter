#ifndef CIA_H
    #include <stdint.h>
    #include <stdbool.h>
    #define CIA_H

    //Memory Mappings
    #define JOYSTICK_PORT1 0xDC01
    #define JOYSTICK_PORT2 0xDC00
    #define JOYSTICK_PORT1_NEUTRAL 0xff
    #define JOYSTICK_PORT2_NEUTRAL 0x7f
    #define JOYSTICK_UP_MASK 1
    #define JOYSTICK_DOWN_MASK 2 
    #define JOYSTICK_LEFT_MASK 4
    #define JOYSTICK_RIGHT_MASK 8
    #define JOYSTICK_FIRE_MASK 16
    #define JOYSTICK_KEY_IS_DOWN false;
    #define JOYSTICK_KEY_IS_UP true;

    uint8_t readJoystick1State();
    uint8_t readJoystick2State();
#endif

