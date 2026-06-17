#include "timer.h"
#include "common.h"
#include <stdint.h>

void startTimer(volatile unsigned char *ciaBase, uint8_t timer) {
    *ADDRESS_TO_PTR(ciaBase + TIMERA_CONTROL_REGISTER_OFFSET + timer) |= (1 << TIMER_CONTROL_REGISTER_RUNSTOP_BIT);
}

void stopTimer(volatile unsigned char* ciaBase, uint8_t timer) {
    *ADDRESS_TO_PTR(ciaBase + TIMERA_CONTROL_REGISTER_OFFSET + timer) &= ~(1 << TIMER_CONTROL_REGISTER_RUNSTOP_BIT);
}

uint16_t getCurrentTimerValue(volatile unsigned char* ciaBase, uint8_t timer) {
    return *ADDRESS_TO_PTR(ciaBase + TIMERA_LOW_BYTE_OFFSET + timer * 4);
}

void setTimerLatch(volatile unsigned char *ciaBase, uint8_t timer, uint16_t value) {
    *ADDRESS_TO_PTR(ciaBase + TIMERA_LOW_BYTE_OFFSET + timer * REGISTERS_PER_TIMER) = value;
}

void reloadLatchIntoTimer(volatile unsigned char *ciaBase, uint8_t timer) {
    *ADDRESS_TO_PTR(ciaBase + TIMERA_CONTROL_REGISTER_OFFSET + timer) |= (1 << TIMER_CONTROL_REGISTER_FORCE_LOAD_BIT);
}

void enableTimerInterrupt(volatile unsigned char* ciaBase, uint8_t timer) {
    *ADDRESS_TO_PTR(ciaBase + TIMER_INTERRUPT_CONTROL_REGISTER_OFFSET) |= (1 << timer);
}

void disableTimerInterrupt(volatile unsigned char *ciaBase, uint8_t timer) {
    *ADDRESS_TO_PTR(ciaBase + TIMER_INTERRUPT_CONTROL_REGISTER_OFFSET) &= ~(1 << timer);
}
