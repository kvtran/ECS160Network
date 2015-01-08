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
#include <sndfile.h>


CSoundClip::CSoundClip(){
    DData = NULL;
    DChannels = 0;
    DTotalFrames = 0;
    DSampleRate = 0;
}

CSoundClip::CSoundClip(const CSoundClip &clip){
    DChannels = clip.DChannels;
    DTotalFrames = clip.DTotalFrames;
    DSampleRate = clip.DSampleRate;
    DData = NULL;
    if(DChannels * DTotalFrames){
        DData = new float[DChannels * DTotalFrames];
        memcpy(DData, clip.DData, sizeof(float) * DTotalFrames * DChannels);
    }
}

CSoundClip::~CSoundClip(){
    if(NULL != DData){
        delete [] DData;   
    }
}

CSoundClip &CSoundClip::operator=(const CSoundClip &clip){
    if(&clip != this){
        DChannels = clip.DChannels;
        DTotalFrames = clip.DTotalFrames;
        DSampleRate = clip.DSampleRate;
        if(NULL != DData){
            delete [] DData;
            DData = NULL;
        }
        if(DChannels * DTotalFrames){
            DData = new float[DChannels * DTotalFrames];
            memcpy(DData, clip.DData, sizeof(float) * DTotalFrames * DChannels);
        }
    }
    return *this;
}

bool CSoundClip::Load(const std::string &filename){
    SNDFILE *SoundFilePtr;
    SF_INFO SoundFileInfo;
    
    SoundFilePtr = sf_open(filename.c_str(), SFM_READ, &SoundFileInfo);
    if(NULL == SoundFilePtr){
        return false;
    }
    if(1 == SoundFileInfo.channels){
        DChannels = 2;
        DTotalFrames = SoundFileInfo.frames;
        DSampleRate = SoundFileInfo.samplerate;
        DData = new float[SoundFileInfo.frames * 2];
        for(int Frame = 0; Frame < DTotalFrames; Frame++){
            sf_readf_float(SoundFilePtr, DData + Frame * 2, 1);   
            DData[Frame * 2 + 1] = DData[Frame * 2];
        }
    }
    else if(2 == SoundFileInfo.channels){
        DChannels = 2;
        DTotalFrames = SoundFileInfo.frames;
        DSampleRate = SoundFileInfo.samplerate;
        DData = new float[SoundFileInfo.frames * SoundFileInfo.channels];
        sf_readf_float(SoundFilePtr, DData, DTotalFrames);
    }
    else{
        sf_close(SoundFilePtr);
        return false;    
    }
    
    sf_close(SoundFilePtr);
    
    return true;
}
    
void CSoundClip::CopyStereoClip(float *data, int offset, int frames){
    if(offset + frames > DTotalFrames){
        int FramesToCopy = DTotalFrames - offset;
        
        if(0 > FramesToCopy){
            FramesToCopy = 0;
        }
        if(FramesToCopy){
            memcpy(data, DData + (offset * 2), sizeof(float) * FramesToCopy * 2);
        }
        memset(data, 0, sizeof(float) * (frames - FramesToCopy) * 2);
    }
    else{
        memcpy(data, DData + (offset * 2), sizeof(float) * frames * 2);
    }
}

void CSoundClip::MixStereoClip(float *data, int offset, int frames, float volume, float rightbias){
    float *DataPointer = DData + (offset * 2);
    int FramesToMix = frames;
    
    if(offset + frames > DTotalFrames){
        FramesToMix = DTotalFrames - offset;
        if(0 > FramesToMix){
            FramesToMix = 0;
        }
    }
    for(int Frame = 0; Frame < FramesToMix; Frame++){
        *data++ += volume * (1.0 - rightbias) * *DataPointer++;
        *data++ += volume * (1.0 + rightbias) * *DataPointer++;
    }
}

