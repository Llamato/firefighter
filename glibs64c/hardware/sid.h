#ifndef SID_H
    #include <stdint.h>
    #define SID_H
    #define SID_VOICE3FEQLOW_ADDRESS 0xd40e
    #define SID_VOICE3FEQHIGH_ADDRESS 0xd40f
    #define SID_VOICE3DUTYCYCLELOW_ADDRESS 0xd410
    #define SID_VOICE3DUTYCYCLEHIGH_ADDRESS 0xd411
    #define SID_VOICE3CONTROL_ADDRESS 0xd412
    #define SID_VOICE3VALUE_ADDRESS 0xd41b

    #define SID_VOICE_MAX_VALUE 0x0fff
    #define SID_VOICE_CONTROL_WAVEFORM_TRIANGLE_MASK 16
    #define SID_VOICE_CONTROL_WAVEFORM_SAWTOOTH_MASK 32
    #define SID_VOICE_CONTROL_WAVEFORM_PULSE_MASK 64
    #define SID_VOICE_CONTROL_WAVEFORM_NOISE_MASK 128

    uint8_t getRandomNumber();
    uint8_t getVoice3ControlRegisterState();
    void setVoice3ControlRegisterState(uint8_t value);
    void setVoice3Frequency(uint16_t frequency);
    void setVoice3DutyCycle(uint16_t dutyCycle);
    void initNoiseVoiceRnd();
    
#endif