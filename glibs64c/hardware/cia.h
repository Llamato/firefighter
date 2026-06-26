#ifndef CIA_H
    #include <stdint.h>
    #include <stdbool.h>
    #include "../common.h"
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
    #define REGISTERS_PER_TIMER 4
    #define CIA1_BASE_ADDRESS 0xDC00
    #define CIA2_BASE_ADDRESS 0xDD00
    #define TIMERA_INDEX 0
    #define TIMERB_INDEX 1
    #define TIMERA_LOW_BYTE_OFFSET 0x04
    #define TIMERA_HIGH_BYTE_OFFSET 0x05
    #define TIMERB_LOW_BYTE_OFFSET 0x08
    #define TIMERB_HIGH_BYTE_OFFSET 0x09
    #define TIMERA_CONTROL_REGISTER_OFFSET 0x0e
    #define TIMERB_CONTROL_REGISTER_OFFSET 0x0f
    #define TIMER_INTERRUPT_CONTROL_REGISTER_OFFSET 0x0d
    #define TIMER_CONTROL_REGISTER_RUNSTOP_BIT 0
    #define TIMER_CONTROL_REGISTER_FORCE_LOAD_BIT 3
    #define TIMER_CONTROL_REGISTER_RUN_MODE_BIT 4
    #define PORTA_OFFSET 0
    #define PORTB_OFFSET 1
    #define DDRA_OFFSET 2
    #define DDRB_OFFSET 3
    #define KEYBOARD_W_PRESSED BITS_PER_BYTE * 1 + 1
    #define KEYBOARD_A_PRESSED BITS_PER_BYTE * 1 + 2
    #define KEYBOARD_S_PRESSED BITS_PER_BYTE * 1 + 5
    #define KEYBOARD_D_PRESSED BITS_PER_BYTE * 2 + 2
    /*#define KEYBOARD_W_PRESSED 32
    #define KEYBOARD_A_PRESSED 18
    #define KEYBOARD_S_PRESSED 33
    #define KEYBOARD_D_PRESSED 23*/
    

    uint8_t readJoystick1State();
    uint8_t readJoystick2State();
    struct Vector2is getMovementVectorFromJoystickState(uint8_t joystickState);
    bool isJoystickFirePressed(uint8_t joystickState);
    uint8_t readKeyboardState();
    bool isKeyPressed(char key);
    struct Vector2is getMovementVectorFromKeyboardState(uint8_t keyboardState);
    uint16_t getCurrentTimerValue(volatile unsigned char* ciaBase, uint8_t timer);
    void startTimer(volatile unsigned char* ciaBase, uint8_t timer);
    void stopTimer(volatile unsigned char* ciaBase, uint8_t timer);
    void setTimerLatch(volatile unsigned char* ciaBase, uint8_t timer, uint16_t value);
    void reloadLatchIntoTimer(volatile unsigned char* ciaBase, uint8_t timer);
    void setTimerRunModeOneshot(volatile unsigned char* ciaBase, uint8_t timer);
    void setTimerRunModeContinues(volatile unsigned char* ciaBase, uint8_t timer);
    void enableTimerInterrupt(volatile unsigned char* ciaBase, uint8_t timer);
    void disableTimerinterrupt(volatile unsigned char* ciaBase, uint8_t timer);

#endif

