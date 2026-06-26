#include <stdint.h>
#include "../common.h"
#include "../gllm/gllm.h"
#include "vic.h"
#include "cia.h"

uint8_t readJoystick1State() {
    return *ADDRESS_TO_PTR(JOYSTICK_PORT1);
}

uint8_t readJoystick2State() {
    return *ADDRESS_TO_PTR(JOYSTICK_PORT2);
}

struct Vector2is getMovementVectorFromJoystickState(uint8_t joystickState) {
    const bool upPressed = (joystickState & JOYSTICK_UP_MASK) == JOYSTICK_KEY_IS_DOWN;
    const bool downPressed = (joystickState & JOYSTICK_DOWN_MASK) == JOYSTICK_KEY_IS_DOWN;
    const bool leftPressed = (joystickState & JOYSTICK_LEFT_MASK) == JOYSTICK_KEY_IS_DOWN;
    const bool rightPressed = (joystickState & JOYSTICK_RIGHT_MASK) == JOYSTICK_KEY_IS_DOWN;
    const struct Vector2is movementVector = {rightPressed - leftPressed,downPressed - upPressed};
    return movementVector;
}

bool isJoystickFirePressed(uint8_t joystickState){
    const bool joystickFirePressed = (joystickState & JOYSTICK_FIRE_MASK) == JOYSTICK_KEY_IS_DOWN;
    return joystickFirePressed;
}

uint8_t readKeyboardState() {
    uint8_t oldDDRA = *ADDRESS_TO_PTR(CIA1_BASE_ADDRESS+DDRA_OFFSET);
    uint8_t oldDDRB = *ADDRESS_TO_PTR(CIA1_BASE_ADDRESS+DDRB_OFFSET);
    *ADDRESS_TO_PTR(CIA1_BASE_ADDRESS+DDRA_OFFSET) = 0xff;
    *ADDRESS_TO_PTR(CIA1_BASE_ADDRESS+DDRB_OFFSET) = 0x00;
    uint8_t currentRow = 0;
    uint8_t rowMask = 1;
    uint8_t pressedKey = 0;
    while(rowMask) {
        *ADDRESS_TO_PTR(CIA1_BASE_ADDRESS+PORTA_OFFSET) = ~rowMask;
        uint8_t rowBytes = ~*ADDRESS_TO_PTR(CIA1_BASE_ADDRESS+PORTB_OFFSET);
        if(rowBytes) {
            uint8_t currentColumn = 0;
            uint8_t columnMask = 1;
            while(columnMask) {
                if(rowBytes & columnMask) {
                    pressedKey = currentRow * BITS_PER_BYTE + currentColumn;
                    goto done;
                }
                currentColumn++;
                columnMask = columnMask << 1;
            }
        }
        currentRow++;
        rowMask = rowMask << 1;
    }
    done:
    *ADDRESS_TO_PTR(CIA1_BASE_ADDRESS+DDRA_OFFSET) = oldDDRA;
    *ADDRESS_TO_PTR(CIA1_BASE_ADDRESS+DDRA_OFFSET) = oldDDRB;
    return pressedKey;
}

struct Vector2is getMovementVectorFromKeyboardState(uint8_t keyboardState) {
    const bool upPressed = keyboardState == KEYBOARD_W_PRESSED;
    const bool downPressed = keyboardState == KEYBOARD_S_PRESSED;
    const bool leftPressed = keyboardState == KEYBOARD_A_PRESSED;
    const bool rightPressed = keyboardState == KEYBOARD_D_PRESSED;
    const struct Vector2is movementVector = {rightPressed - leftPressed,downPressed - upPressed};
    return movementVector;
}

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