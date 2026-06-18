#include <stdint.h>
#include "timer.h"
#include "../common.h"

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
