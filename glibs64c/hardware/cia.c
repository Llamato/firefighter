#include <stdint.h>
#include "../common.h"
#include "../gllm/gllm.h"
#include "cia.h"

uint8_t readJoystick1State() {
    return *ADDRESS_TO_PTR(JOYSTICK_PORT1);
}

uint8_t readJoystick2State() {
    return *ADDRESS_TO_PTR(JOYSTICK_PORT2);
}

struct Vector2is getMovementVectorFromJoystickState(uint8_t joystickState) {
    const bool joystickUpPressed = (joystickState & JOYSTICK_UP_MASK) == JOYSTICK_KEY_IS_DOWN;
    const bool joystickDownPressed = (joystickState & JOYSTICK_DOWN_MASK) == JOYSTICK_KEY_IS_DOWN;
    const bool joystickLeftPressed = (joystickState & JOYSTICK_LEFT_MASK) == JOYSTICK_KEY_IS_DOWN;
    const bool joystickRightPressed = (joystickState & JOYSTICK_RIGHT_MASK) == JOYSTICK_KEY_IS_DOWN;
    const struct Vector2is movementVector = {joystickRightPressed - joystickLeftPressed,joystickDownPressed - joystickUpPressed};
    return movementVector;
}

bool isJoystickFirePressed(uint8_t joystickState){
    const bool joystickFirePressed = (joystickState & JOYSTICK_FIRE_MASK) == JOYSTICK_KEY_IS_DOWN;
    return joystickFirePressed;
}