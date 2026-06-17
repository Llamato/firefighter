#ifndef SID_H
    #include <stdint.h>
    #define SID_H
    #define SID_VOICE3FEQLOW_ADDRESS 0xd40e
    #define SID_VOICE3FEQHIGH_ADDRESS 0xd40f
    #define SID_VOICE3CONTROL_ADDRESS 0xd412

    struct VoiceState {
        uint16_t frequency;
        uint8_t controlRegister;
    };

    struct VoiceState getVoice3State();
    void setVoiceState(struct VoiceState state);
    struct VoiceState initNoiseVoiceRnd(uint16_t maxValue);
    
#endif