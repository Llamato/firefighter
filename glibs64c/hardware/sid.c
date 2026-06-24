#include "sid.h"
#include "../common.h"
#include "vic.h"
#include <stdint.h>

uint8_t getRandomNumber() {
    return *ADDRESS_TO_PTR(SID_VOICE3VALUE_ADDRESS);
}

uint8_t getVoice3ControlRegisterState() {
    return *ADDRESS_TO_PTR(SID_VOICE3CONTROL_ADDRESS);
}

void setVoice3ControlRegisterState(uint8_t value) {
    *ADDRESS_TO_PTR(SID_VOICE3CONTROL_ADDRESS) = value;
}

void setVoice3Frequency(uint16_t frequency) {
    *ADDRESS_TO_PTR(SID_VOICE3FEQLOW_ADDRESS) = frequency;
    *ADDRESS_TO_PTR(SID_VOICE3FEQHIGH_ADDRESS) = (frequency & 0xf00) >> BITS_PER_BYTE;
}

void setVoice3DutyCycle(uint16_t dutyCycle) {
    *ADDRESS_TO_PTR(SID_VOICE3DUTYCYCLELOW_ADDRESS) = dutyCycle;
    *ADDRESS_TO_PTR(SID_VOICE3DUTYCYCLEHIGH_ADDRESS) = (dutyCycle & 0xf00) >> BITS_PER_BYTE;
}

void initNoiseVoiceRnd() {
    setVoice3Frequency(255);
    setVoice3DutyCycle(0x800);
    setVoice3ControlRegisterState(SID_VOICE_CONTROL_WAVEFORM_NOISE_MASK);
}