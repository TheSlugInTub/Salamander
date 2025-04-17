#pragma once

#include <salamander/glm.h>
#include <AL/alc.h>
#include <stdbool.h>

typedef unsigned int smSound;

typedef struct
{
    float pitch;
    float gain;
    vec3  position;
    bool  loopSound;
    vec3  velocity;

    unsigned int source;
    unsigned int buffer;
} smAudioSource;

extern ALCdevice*  sm_audioDevice;
extern ALCcontext* sm_audioContext;

void smAudio_InitializeAudio();
void smAudio_DestroyAudio();

smSound smAudio_LoadSound(const char* filename);

smAudioSource smAudioSource_Create(float pitch, float gain,
                                   vec3 position, vec3 velocity,
                                   bool loopSound);
void smAudioSource_PlaySound(smAudioSource* src, smSound sound);
void smAudioSource_StopSound(smAudioSource* src, smSound sound);
