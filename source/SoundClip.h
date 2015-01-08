#ifndef SOUNDCLIP_H
#define SOUNDCLIP_H
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
#include <string>

class CSoundClip{
    protected:
        int DChannels;
        int DTotalFrames;
        int DSampleRate;
        float *DData;
        
    public:
        CSoundClip();
        CSoundClip(const CSoundClip &clip);
        ~CSoundClip();
        
        CSoundClip &operator=(const CSoundClip &clip);
        
        int Channels() const{
            return DChannels;  
        };
        
        int TotalFrames() const{
            return DTotalFrames;  
        };
        
        int SampleRate() const{
            return DSampleRate;  
        };
        
        bool Load(const std::string &filename);
            
        void CopyStereoClip(float *data, int offset, int frames);
        void MixStereoClip(float *data, int offset, int frames, float volume = 1.0, float rightbias = 0.0);
        
            
};

#endif
