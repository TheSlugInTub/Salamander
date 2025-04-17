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
    ALenum   err, format;
    ALuint   buffer;
    SNDFILE* sndfile;
    SF_INFO  sfinfo;
    short*   membuf;
    ALsizei  num_bytes;

    /* Initialize SF_INFO structure */
    memset(&sfinfo, 0, sizeof(sfinfo));

    /* Open the audio file and check that it's usable */
    sndfile = sf_open(filename, SFM_READ, &sfinfo);
    if (!sndfile)
    {
        printf("Could not open audio in %s\n", filename);
        printf("Error: %s\n", sf_strerror(sndfile));
        return -1; // Return an empty/invalid sound
    }

    /* Get the sound format, and figure out the OpenAL format */
    format = AL_NONE;
    if (sfinfo.channels == 1)
        format = AL_FORMAT_MONO16;
    else if (sfinfo.channels == 2)
        format = AL_FORMAT_STEREO16;
    else
    {
        printf("Unsupported channel count: %d\n", sfinfo.channels);
        sf_close(sndfile);
        return -1; // Return an empty/invalid sound
    }

    /* Calculate the size needed for the buffer */
    sf_count_t samples = sfinfo.frames * sfinfo.channels;
    num_bytes = samples * sizeof(short);

    /* Allocate memory for the audio data */
    membuf = (short*)malloc(num_bytes);
    if (!membuf)
    {
        printf("Failed to allocate memory for audio data\n");
        sf_close(sndfile);
        return -1; // Return an empty/invalid sound
    }

    /* Read the audio data */
    sf_count_t read_frames =
        sf_readf_short(sndfile, membuf, sfinfo.frames);
    if (read_frames < sfinfo.frames)
    {
        printf("Failed to read all frames. Read %ld of %ld\n",
               (long)read_frames, (long)sfinfo.frames);
        // Continue anyway with what we could read
    }

    /* Close the sound file */
    sf_close(sndfile);

    /* Buffer the audio data into a new buffer object */
    buffer = 0;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, membuf, num_bytes,
                 sfinfo.samplerate);

    /* Free the memory buffer */
    free(membuf);

    /* Check if an error occurred, and clean up if so */
    err = alGetError();
    if (err != AL_NO_ERROR)
    {
        printf("OpenAL Error: %s\n", alGetString(err));
        if (buffer && alIsBuffer(buffer))
            alDeleteBuffers(1, &buffer);
        return -1; // Return an empty/invalid sound
    }

    return buffer;
}

void smAudio_InitializeAudio()
{
    sm_audioDevice =
        alcOpenDevice(NULL); // NULL = get default device
    if (!sm_audioDevice)
        printf("failed to get sound device");

    sm_audioContext =
        alcCreateContext(sm_audioDevice, NULL); // create context
    if (!sm_audioContext)
        printf("Failed to set sound context");

    if (!alcMakeContextCurrent(
            sm_audioContext)) // make context current
        printf("failed to make context current");

    const ALCchar* name = NULL;
    if (alcIsExtensionPresent(sm_audioDevice,
                              "ALC_ENUMERATE_ALL_EXT"))
        name =
            alcGetString(sm_audioDevice, ALC_ALL_DEVICES_SPECIFIER);
    if (!name || alcGetError(sm_audioDevice) != AL_NO_ERROR)
        name = alcGetString(sm_audioDevice, ALC_DEVICE_SPECIFIER);
    printf("Opened \"%s\"\n", name);
}

void smAudio_DestroyAudio()
{
    if (!alcMakeContextCurrent(NULL))
        printf("failed to set context to NULL");

    alcDestroyContext(sm_audioContext);

    if (sm_audioContext)
        printf("failed to unset during close");

    if (!alcCloseDevice(sm_audioDevice))
        printf("failed to close sound device");
}

smAudioSource smAudioSource_Create(float pitch, float gain,
                                   vec3 position, vec3 velocity,
                                   bool loopSound)
{
    smAudioSource src;
    src.pitch = pitch;
    src.gain = gain;
    glm_vec3_copy(position, src.position);
    glm_vec3_copy(velocity, src.velocity);
    src.buffer = 0;
    src.loopSound = loopSound;

    alGenSources(1, &src.source);
    alSourcef(src.source, AL_PITCH, src.pitch);
    alSourcef(src.source, AL_GAIN, src.gain);
    alSource3f(src.source, AL_POSITION, src.position[0],
               src.position[1], src.position[2]);
    alSource3f(src.source, AL_VELOCITY, src.velocity[0],
               src.velocity[1], src.velocity[2]);
    alSourcei(src.source, AL_LOOPING, src.loopSound);
    alSourcei(src.source, AL_BUFFER, src.buffer);

    return src;
}

void smAudioSource_PlaySound(smAudioSource* src, smSound sound)
{
    // Stop the current playback
    alSourceStop(src->source);
    
    // Change the buffer if it's different
    if (sound != src->buffer)
    {
        src->buffer = sound;
        alSourcei(src->source, AL_BUFFER, (ALint)src->buffer);
    }
    
    // Make sure the source state is reset
    alSourceRewind(src->source);
    
    // Now play the sound
    alSourcePlay(src->source);
}

void smAudioSource_StopSound(smAudioSource* src, smSound sound)
{
#ifdef SM_DEBUG_LEVEL_1
    if (sound != src->buffer)
    {
        printf("Sound is not within source");
    }
#endif

    alSourceStop(src->source);
}
