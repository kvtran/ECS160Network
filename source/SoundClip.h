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

/**
 * Sound Clip class
 */
class CSoundClip{
    protected:
        int DChannels; /*!< Channels */
        int DTotalFrames; /*!< Total frames */
        int DSampleRate; /*!< Sample rate */
        float *DData; /*!< Data */
        
    public:
	/**
	 * Empty constructor. Initializes all class variables NULL or 0.
	 */
        CSoundClip();

	/**
	 * Class constructor. 
	 *
	 * @param clip Used to initialize class variables
	 */
        CSoundClip(const CSoundClip &clip);

	/**
	 * Destructor.
	 */
        ~CSoundClip();
        
	/**
	 * Operator overload =.
	 */
        CSoundClip &operator=(const CSoundClip &clip);
        
	/**
	 * Getter function for channels.
	 */
        int Channels() const{
            return DChannels;  
        };
        
	/**
	 * Getter function for total frames.
	 */
        int TotalFrames() const{
            return DTotalFrames;  
        };
        
	/**
	 * Getter function for sample rate.
	 */
        int SampleRate() const{
            return DSampleRate;  
        };
        
	/**
	 * Loads sound clip from file.
	 *
	 * @param filename Name of file to load sound clip from.
	 */
        bool Load(const std::string &filename);

	/**
	 * Copies sound clip.
	 *
	 * @param data Destination of sound clip copy
	 * @param offset Frame offset
	 * @param frames Frames
	 */
        void CopyStereoClip(float *data, int offset, int frames);

	/**
	 * Mixes sound clip.
	 *
	 * @param data Destination of clip mix
	 * @param offset Frame offset
	 * @param frames Frames
	 * @param volume Volume of clip
	 * @param rightbias Right bias
	 */
        void MixStereoClip(float *data, int offset, int frames, float volume = 1.0, float rightbias = 0.0);
        
            
};

#endif
