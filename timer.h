#ifndef TIMER_H
    #include <stdint.h>
    #define TIMER_H

    //Memory mappings
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

    void startTimer(volatile unsigned char* ciaBase, uint8_t timer);
    void stopTimer(volatile unsigned char* ciaBase, uint8_t timer);
    uint16_t getCurrentTimerValue(volatile unsigned char* ciaBase, uint8_t timer);
    void setTimerLatch(volatile unsigned char* ciaBase, uint8_t timer, uint16_t value);
    void reloadLatchIntoTimer(volatile unsigned char* ciaBase, uint8_t timer);
    void enableTimerInterrupt(volatile unsigned char* ciaBase, uint8_t timer);
    void disableTimerinterrupt(volatile unsigned char* ciaBase, uint8_t timer);
    
#endif