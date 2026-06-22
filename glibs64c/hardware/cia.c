#include <stdint.h>
#include "../common.h"
#include "cia.h"

uint8_t readJoystick1State() {
    return *ADDRESS_TO_PTR(JOYSTICK_PORT1);
}

uint8_t readJoystick2State() {
    return *ADDRESS_TO_PTR(JOYSTICK_PORT2);
}