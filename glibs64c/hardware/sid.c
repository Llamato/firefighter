#include "sid.h"
#include "../common.h"
#include <stdint.h>

struct VoiceState getVoice3State() {
    return (struct VoiceState) {*ADDRESS_TO_PTR(SID_VOICE3FEQLOW_ADDRESS), *ADDRESS_TO_PTR(SID_VOICE3CONTROL_ADDRESS)};
}

void setVoiceState(struct VoiceState state) {
    *ADDRESS_TO_PTR(SID_VOICE3FEQLOW_ADDRESS) = state.frequency;
    *ADDRESS_TO_PTR(SID_VOICE3CONTROL_ADDRESS) = state.controlRegister;
}

struct VoiceState initNoiseVoiceRnd(uint16_t maxValue) {
    const struct VoiceState previousState = getVoice3State();
    setVoiceState((struct VoiceState) {maxValue, 0x80});
    return previousState;
}