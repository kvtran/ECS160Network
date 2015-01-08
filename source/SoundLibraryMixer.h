#ifndef SOUNDLIBRARYMIXER_H
#define SOUNDLIBRARYMIXER_H
/*
    Copyright (c) 2015, Christopher Nitta
    All rights reserved.
    
    All source material (source code, images, sounds, etc.) have been provided to 
    University of California, Davis students of course ECS 160 for educational 
    purposes. It may not be distributed beyond those enrolled in the course without 
    prior permission from the copyright holder. 
    
    Some sound files, sound fonts, and midi files have been included that were 
    freely available via internet sources. They have been included in this 
    distribution for educational purposes only and this copyright notice does not 
    attempt to claim any ownership of this material.
*/
#include "SoundClip.h"
#include <portaudio.h>
#include <fluidsynth.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <pthread.h>

class CSoundLibraryMixer{
    protected:
        typedef struct{
            int DIdentification;
            int DIndex;
            int DOffset;
            float DVolume;
            float DRightBias;
        } SClipStatus, *SClipStatusRef;
        
        typedef struct{
            int DIdentification;
            float DCurrentFrequency;
            float DCurrentStep;
            float DFrequencyDecay;
            float DVolume;
            float DVolumeDecay;
            float DRightBias;
            float DRightShift;
        } SToneStatus, *SToneStatusRef;
        
        std::vector< CSoundClip > DSoundClips;
        std::map< std::string, int > DMapping;
        std::list< SClipStatus > DClipsInProgress;
        std::list< SToneStatus > DTonesInProgress;
        std::list< int > DFreeClipIDs;
        std::list< int > DFreeToneIDs;
        std::vector< std::string > DMusicFilenames;
        std::map< std::string, int > DMusicMapping;
        PaStream *DStream;
        fluid_settings_t* DFluidSettings;
        fluid_synth_t* DFluidSynthesizer;
        fluid_player_t* DFluidPlayer;
        fluid_audio_driver_t* DFluidAudioDriver;
        pthread_mutex_t DMutex;
        float *DSineWave;
        int DSampleRate;
        int DNextClipID;
        int DNextToneID;
        
    public:
        CSoundLibraryMixer();
        ~CSoundLibraryMixer();
        
        int ClipCount() const{
            return DSoundClips.size();  
        };
        
        int SampleRate() const{
            return DSampleRate;  
        };
        
        int FindClip(const std::string &clipname) const;
        int FindSong(const std::string &songname) const;
        int ClipDurationMS(int index);
        
        static int TimestepCallback(const void *in, void *out, unsigned long frames, const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags status, void *data);
        int Timestep(void *out, unsigned long frames, const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags status);
        
        bool LoadLibrary(const std::string &filename);
        
        int PlayClip(int index, float volume, float rightbias);
        int PlayTone(float freq, float freqdecay, float volume, float volumedecay, float rightbias, float rightshift);
        void StopTone(int id);
        bool ClipCompleted(int id);
        void PlaySong(int index, float volume);
        void StopSong();
        void SongVolume(float volume);
};

#endif
