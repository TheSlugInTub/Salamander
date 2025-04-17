#include <salamander/audio.h>
#include <AL/al.h>
#include <sndfile/sndfile.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <salamander/config.h>

ALCdevice*  sm_audioDevice;
ALCcontext* sm_audioContext;

smSound smAudio_LoadSound(const char* filename)
{
}

void smAudio_InitializeAudio()
{
}

void smAudio_DestroyAudio()
{
}

smAudioSource smAudioSource_Create(float pitch, float gain,
                                   vec3 position, vec3 velocity,
                                   bool loopSound)
{
}

void smAudioSource_PlaySound(smAudioSource* src, smSound sound)
{
}

void smAudioSource_StopSound(smAudioSource* src, smSound sound)
{
}
