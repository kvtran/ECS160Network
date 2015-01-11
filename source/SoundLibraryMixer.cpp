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
#include "SoundLibraryMixer.h"
#include <math.h>
#include <cstring>

#define FRAMES_PER_BUFFER       64
#ifndef M_PI
#define M_PI                    3.141592653589793
#endif

CSoundLibraryMixer::CSoundLibraryMixer(){
    DStream = NULL;
    DSineWave = NULL;
    DNextClipID = 0;
    DNextToneID = 0;
    Pa_Initialize();
    pthread_mutex_init(&DMutex, NULL);
    for(int Index = 0; Index < 128; Index++){
        DFreeClipIDs.push_back(Index);   
    }
    
    DFluidSettings = NULL;
    DFluidSynthesizer = NULL;
    DFluidPlayer = NULL;
    DFluidAudioDriver = NULL;
}

CSoundLibraryMixer::~CSoundLibraryMixer(){
    if(NULL != DStream){
        if(Pa_IsStreamActive(DStream)){
            Pa_StopStream(DStream);
        }
        Pa_CloseStream(DStream);
        DStream = NULL;
    }
    if(NULL != DSineWave){
        delete [] DSineWave;    
    }
    Pa_Terminate();
    pthread_mutex_destroy(&DMutex);
    if(NULL != DFluidAudioDriver){
        delete_fluid_audio_driver(DFluidAudioDriver);
    }
    if(NULL != DFluidAudioDriver){
        delete_fluid_player(DFluidPlayer);
    }
    if(NULL != DFluidAudioDriver){
        delete_fluid_synth(DFluidSynthesizer);
    }
    if(NULL != DFluidAudioDriver){
        delete_fluid_settings(DFluidSettings);
    }
}

int CSoundLibraryMixer::FindClip(const std::string &clipname) const{
    std::map< std::string, int >::const_iterator Iterator = DMapping.find(clipname);
    if(DMapping.end() != Iterator){
        return Iterator->second;
    }
    return -1;
}

int CSoundLibraryMixer::FindSong(const std::string &songname) const{
    std::map< std::string, int >::const_iterator Iterator = DMusicMapping.find(songname);
    if(DMusicMapping.end() != Iterator){
        return Iterator->second;
    }
    return -1;
}

int CSoundLibraryMixer::ClipDurationMS(int index){
    if((0 > index)||(index >= DSoundClips.size())){
        return 0;    
    }
    return (DSoundClips[index].TotalFrames() * 1000) / DSoundClips[index].SampleRate();
}

int CSoundLibraryMixer::TimestepCallback(const void *in, void *out, unsigned long frames, const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags status, void *data){
    CSoundLibraryMixer *Mixer = (CSoundLibraryMixer *)data;
    
    return Mixer->Timestep(out, frames, timeinfo, status);
}

int CSoundLibraryMixer::Timestep(void *out, unsigned long frames, const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags status){
    float *DataPtr = (float *)out;
    
    memset(DataPtr, 0, sizeof(float) * frames * 2);
    pthread_mutex_lock(&DMutex);
    for(std::list< SClipStatus >::iterator ClipIterator = DClipsInProgress.begin(); ClipIterator != DClipsInProgress.end(); ){
        bool Advance = true;
        
        DSoundClips[ClipIterator->DIndex].MixStereoClip(DataPtr, ClipIterator->DOffset, frames, ClipIterator->DVolume, ClipIterator->DRightBias);
        
        ClipIterator->DOffset += frames;
        if(ClipIterator->DOffset >= DSoundClips[ClipIterator->DIndex].TotalFrames()){
            DFreeClipIDs.push_back(ClipIterator->DIdentification);   
            ClipIterator = DClipsInProgress.erase(ClipIterator);
            Advance = false;
        }
        if(Advance){
            ClipIterator++;   
        }
    }
    
    for(std::list< SToneStatus >::iterator ToneIterator = DTonesInProgress.begin(); ToneIterator != DTonesInProgress.end(); ){
        bool Advance = true;
        float *CurOutPtr = DataPtr;
        
        for(int Frame = 0; Frame < frames; Frame++){
            int SineIndex = ToneIterator->DCurrentStep;
            
            if(0.5 <= (ToneIterator->DCurrentStep - SineIndex)){
                SineIndex++;
                if(DSampleRate <= SineIndex){
                    SineIndex = 0;   
                }
            }
            *CurOutPtr++ += ToneIterator->DVolume * (1.0 - ToneIterator->DRightBias) * DSineWave[SineIndex];
            *CurOutPtr++ += ToneIterator->DVolume * (1.0 + ToneIterator->DRightBias) * DSineWave[SineIndex];
            ToneIterator->DCurrentStep += ToneIterator->DCurrentFrequency;
            ToneIterator->DCurrentFrequency += ToneIterator->DFrequencyDecay;
            ToneIterator->DVolume += ToneIterator->DVolumeDecay;
            ToneIterator->DRightBias += ToneIterator->DRightShift;
            if(DSampleRate <= ToneIterator->DCurrentStep){
                ToneIterator->DCurrentStep -= DSampleRate;
            }
            if(0.0 > ToneIterator->DVolume){
                Advance = false;
                break;
            }
            if(20.0 > ToneIterator->DCurrentFrequency){
                Advance = false;
                break;
            }
            if(-1.0 > ToneIterator->DRightBias){
                ToneIterator->DRightBias = -1.0;
                ToneIterator->DRightShift = 0.0;
            }
            if(1.0 < ToneIterator->DRightBias){
                ToneIterator->DRightBias = -1.0;
                ToneIterator->DRightShift = 0.0;                
            }
        }        
        if(Advance){
            ToneIterator++;    
        }
        else{
            DFreeToneIDs.push_back(ToneIterator->DIdentification);   
            ToneIterator = DTonesInProgress.erase(ToneIterator);
        }
    }
    
    pthread_mutex_unlock(&DMutex);    
    frames *= 2;
    for(int Frame = 0; Frame < frames; Frame++){
        if(-1.0 > *DataPtr){
            *DataPtr = -1.0;
        }
        if(1.0 < *DataPtr){
            *DataPtr = 1.0;
        }
        DataPtr++;
    }
    
    return paContinue;
}

bool CSoundLibraryMixer::LoadLibrary(const std::string &filename){
    char *TempBuffer = NULL;
    size_t BufferSize = 0;
    int LastChar, TotalClips, TotalSongs;
    FILE *FilePointer;
    bool ReturnStatus = false;
    std::string SoundFontName;
    
    FilePointer = fopen(filename.c_str(), "r");
    if(NULL == FilePointer){
        return false;   
    }
    if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
        goto LoadLibraryExit1;
    }
    sscanf(TempBuffer,"%d", &TotalClips);
    DSoundClips.resize(TotalClips);
    for(int Index = 0; Index < TotalClips; Index++){
        if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
            goto LoadLibraryExit2;
        }    
        LastChar = strlen(TempBuffer) - 1;
        while((0 <= LastChar)&&(('\r' == TempBuffer[LastChar])||('\n' == TempBuffer[LastChar]))){
            TempBuffer[LastChar] = '\0';
            LastChar--;
        }
        DMapping[std::string(TempBuffer)] = Index;
        if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
            goto LoadLibraryExit2;
        }    
        LastChar = strlen(TempBuffer) - 1;
        while((0 <= LastChar)&&(('\r' == TempBuffer[LastChar])||('\n' == TempBuffer[LastChar]))){
            TempBuffer[LastChar] = '\0';
            LastChar--;
        }
        if(!DSoundClips[Index].Load(TempBuffer)){
            goto LoadLibraryExit2;    
        }
    }
    DSampleRate = DSoundClips[0].SampleRate();
    for(int Index = 1; Index < TotalClips; Index++){
        if(DSoundClips[0].SampleRate() != DSoundClips[Index].SampleRate()){
            goto LoadLibraryExit2;    
        }
    }
    if(NULL != DSineWave){
        delete [] DSineWave;    
    }
    DSineWave = new float[DSampleRate];
    for(int Index = 0; Index < DSampleRate; Index++){
        DSineWave[Index] = sin( ((double)Index/(double)DSampleRate) * M_PI * 2.0 );
    }
    
    if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
        goto LoadLibraryExit2;
    }
    LastChar = strlen(TempBuffer) - 1;
    while((0 <= LastChar)&&(('\r' == TempBuffer[LastChar])||('\n' == TempBuffer[LastChar]))){
        TempBuffer[LastChar] = '\0';
        LastChar--;
    }
    if(!fluid_is_soundfont(TempBuffer)){
        printf("%s not a sound font!\n",TempBuffer);
        goto LoadLibraryExit2;
    }
    SoundFontName = TempBuffer;
    if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
        printf("Failed to read line\n");
        goto LoadLibraryExit2;
    }
    sscanf(TempBuffer,"%d", &TotalSongs);
    DMusicFilenames.resize(TotalSongs);
    for(int Index = 0; Index < TotalSongs; Index++){
        if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
            printf("Failed to read line2\n");
            goto LoadLibraryExit3;
        }    
        LastChar = strlen(TempBuffer) - 1;
        while((0 <= LastChar)&&(('\r' == TempBuffer[LastChar])||('\n' == TempBuffer[LastChar]))){
            TempBuffer[LastChar] = '\0';
            LastChar--;
        }
        DMusicMapping[std::string(TempBuffer)] = Index;
        if(-1 == getline(&TempBuffer, &BufferSize, FilePointer)){
            goto LoadLibraryExit2;
        }    
        LastChar = strlen(TempBuffer) - 1;
        while((0 <= LastChar)&&(('\r' == TempBuffer[LastChar])||('\n' == TempBuffer[LastChar]))){
            TempBuffer[LastChar] = '\0';
            LastChar--;
        }
        if(!fluid_is_midifile(TempBuffer)){
            printf("%s is not a midi file!\n", TempBuffer);
            goto LoadLibraryExit3;    
        }
        DMusicFilenames[Index] = std::string(TempBuffer);
    }
       
    DFluidSettings = new_fluid_settings();
    fluid_settings_setstr(DFluidSettings, "audio.driver", "portaudio");
    DFluidSynthesizer = new_fluid_synth(DFluidSettings);
    DFluidPlayer = new_fluid_player(DFluidSynthesizer);
    DFluidAudioDriver = new_fluid_audio_driver(DFluidSettings, DFluidSynthesizer);

    fluid_synth_sfload(DFluidSynthesizer, SoundFontName.c_str(), 1);

    if(paNoError != Pa_OpenDefaultStream(&DStream, 0, 2, paFloat32, DSampleRate, FRAMES_PER_BUFFER, TimestepCallback, this)){
        goto LoadLibraryExit3;
    }
    if(paNoError != Pa_StartStream(DStream)){
        goto LoadLibraryExit3;
    }
    
    fclose(FilePointer);
    return true;
    
LoadLibraryExit3:
    DMusicFilenames.clear();
LoadLibraryExit2:
    DSoundClips.clear();
    DMapping.clear();
LoadLibraryExit1:
    fclose(FilePointer);
    return ReturnStatus;
}

int CSoundLibraryMixer::PlayClip(int index, float volume, float rightbias){
    SClipStatus TempClipStatus;
    
    if((0 > index)||(DSoundClips.size() <= index)){
        fprintf(stderr,"Invalid Clip %d!!!!!\n",index);
        return -1;
    }
    TempClipStatus.DIndex = index;
    TempClipStatus.DOffset = 0;
    TempClipStatus.DVolume = volume;
    TempClipStatus.DRightBias = rightbias;
    
    pthread_mutex_lock(&DMutex);
    if(DFreeClipIDs.size()){
        TempClipStatus.DIdentification = DFreeClipIDs.front();
        DFreeClipIDs.pop_front();
    }
    else{
        TempClipStatus.DIdentification = DNextClipID++;    
    }
    DClipsInProgress.push_back(TempClipStatus);
    pthread_mutex_unlock(&DMutex);  
    return TempClipStatus.DIdentification;
}

int CSoundLibraryMixer::PlayTone(float freq, float freqdecay, float volume, float volumedecay, float rightbias, float rightshift){
    SToneStatus TempToneStatus;
    

    TempToneStatus.DCurrentFrequency = freq;
    TempToneStatus.DCurrentStep = 0;
    TempToneStatus.DFrequencyDecay = freqdecay / DSampleRate;
    TempToneStatus.DVolume = volume;
    TempToneStatus.DVolumeDecay = volumedecay / DSampleRate;
    TempToneStatus.DRightBias = rightbias;
    TempToneStatus.DRightShift = rightshift / DSampleRate;
    
    pthread_mutex_lock(&DMutex);
    if(DFreeToneIDs.size()){
        TempToneStatus.DIdentification = DFreeToneIDs.front();
        DFreeToneIDs.pop_front();
    }
    else{
        TempToneStatus.DIdentification = DNextToneID++;    
    }
    DTonesInProgress.push_back(TempToneStatus);
    pthread_mutex_unlock(&DMutex);  
    return TempToneStatus.DIdentification;
}

void CSoundLibraryMixer::StopTone(int id){
    std::list< SToneStatus >::iterator ToneIterator;
    
    pthread_mutex_lock(&DMutex);
    ToneIterator = DTonesInProgress.begin();
    while(ToneIterator != DTonesInProgress.end()){
        if(ToneIterator->DIdentification == id){
            DTonesInProgress.erase(ToneIterator);
            DFreeToneIDs.push_back(id);
            break;
        }
        ToneIterator++;
    }
    pthread_mutex_unlock(&DMutex);  
}

bool CSoundLibraryMixer::ClipCompleted(int id){
    std::list< int >::iterator ClipIDIterator;
    bool FoundID = false;
    
    pthread_mutex_lock(&DMutex);
    ClipIDIterator = DFreeClipIDs.begin();
    while(ClipIDIterator != DFreeClipIDs.end()){
        if(*ClipIDIterator == id){
            FoundID = true;
            break;
        }
        ClipIDIterator++;
    }
    pthread_mutex_unlock(&DMutex); 
    return FoundID;
}

void CSoundLibraryMixer::PlaySong(int index, float volume){
    if((0 > index)||(index >= DMusicFilenames.size())){
        return;   
    }
    switch(fluid_player_get_status(DFluidPlayer)){
        case FLUID_PLAYER_PLAYING:  fluid_player_stop(DFluidPlayer);
        case FLUID_PLAYER_READY:    
        default:                    delete_fluid_player(DFluidPlayer);
                                    DFluidPlayer = new_fluid_player(DFluidSynthesizer);
                                    break;
    }
    fluid_synth_set_gain(DFluidSynthesizer, 0.2 * volume);
    fluid_player_add(DFluidPlayer,DMusicFilenames[index].c_str());
    fluid_player_set_loop(DFluidPlayer, -1);
    fluid_player_play(DFluidPlayer);
}

void CSoundLibraryMixer::StopSong(){
    fluid_player_stop(DFluidPlayer);
    fluid_synth_set_gain(DFluidSynthesizer, 0.0);
}

void CSoundLibraryMixer::SongVolume(float volume){
    fluid_synth_set_gain(DFluidSynthesizer, 0.2 * volume);
}

