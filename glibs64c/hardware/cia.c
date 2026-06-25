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

//Potential optimisations: remove multiplications and if need be all pointer arithmetic

uint16_t getCurrentTimerValue(volatile unsigned char* ciaBase, uint8_t timer) {
    return *ADDRESS_TO_PTR(ciaBase + TIMERA_LOW_BYTE_OFFSET + timer * 4);
}

void startTimer(volatile unsigned char* ciaBase, uint8_t timer) {
    *ADDRESS_TO_PTR(ciaBase + TIMERA_CONTROL_REGISTER_OFFSET + timer) |= (1 << TIMER_CONTROL_REGISTER_RUNSTOP_BIT);
}

void stopTimer(volatile unsigned char* ciaBase, uint8_t timer) {
    *ADDRESS_TO_PTR(ciaBase + TIMERA_CONTROL_REGISTER_OFFSET + timer) &= ~(1 << TIMER_CONTROL_REGISTER_RUNSTOP_BIT);
}

void setTimerLatch(volatile unsigned char* ciaBase, uint8_t timer, uint16_t value) {
    *ADDRESS_TO_PTR(ciaBase + TIMERA_LOW_BYTE_OFFSET + REGISTERS_PER_TIMER * timer) = value;
}

void reloadLatchIntoTimer(volatile unsigned char* ciaBase, uint8_t timer) {
    *ADDRESS_TO_PTR(ciaBase + TIMERA_CONTROL_REGISTER_OFFSET + REGISTERS_PER_TIMER * timer) |= (1 << TIMER_CONTROL_REGISTER_FORCE_LOAD_BIT);
}

void setTimerRunModeOneshot(volatile unsigned char* ciaBase, uint8_t timer) {
    *ADDRESS_TO_PTR(ciaBase + TIMERA_CONTROL_REGISTER_OFFSET + REGISTERS_PER_TIMER * timer) |= (1 << TIMER_CONTROL_REGISTER_RUNSTOP_BIT);
}

void setTimerRunModeContinues(volatile unsigned char* ciaBase, uint8_t timer) {
    *ADDRESS_TO_PTR(ciaBase + TIMERA_CONTROL_REGISTER_OFFSET + REGISTERS_PER_TIMER * timer) &= ~(1 << TIMER_CONTROL_REGISTER_RUNSTOP_BIT);
}

void enableTimerInterrupt(volatile unsigned char* ciaBase, uint8_t timer) {
    *ADDRESS_TO_PTR(ciaBase + TIMER_INTERRUPT_CONTROL_REGISTER_OFFSET) |= (1 << timer);
}

void disableTimerInterrupt(volatile unsigned char *ciaBase, uint8_t timer) {
    *ADDRESS_TO_PTR(ciaBase + TIMER_INTERRUPT_CONTROL_REGISTER_OFFSET) &= ~(1 << timer);
}
