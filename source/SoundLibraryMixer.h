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

/**
 * Sound library mixer class.
 */
class CSoundLibraryMixer{
    protected:
	/**
	 * Clip status struct
	 */
        typedef struct{
            int DIdentification; /*!< Identification */
            int DIndex; /*!< Index */
            int DOffset; /*!< Offset */
            float DVolume; /*!< Volume */
            float DRightBias; /*!< Right bias */
        } SClipStatus, *SClipStatusRef;
        
	/**
	 * Tone status struct
	 */
        typedef struct{
            int DIdentification; /*!< Identification */
            float DCurrentFrequency; /*!< Current frequency */
            float DCurrentStep; /*!< Current step */
            float DFrequencyDecay; /*!< Frequency decay */
            float DVolume; /*!< Volume */
            float DVolumeDecay; /*!< Volume decay */
            float DRightBias; /*!< Right bias */
            float DRightShift; /*!< Right shift */
        } SToneStatus, *SToneStatusRef;
        
        std::vector< CSoundClip > DSoundClips; /*!< Sound clips */
        std::map< std::string, int > DMapping; /*!< String to int map */
        std::list< SClipStatus > DClipsInProgress; /*!< Clips in progress */
        std::list< SToneStatus > DTonesInProgress; /*!< Tones in progress */
        std::list< int > DFreeClipIDs; /*!< Free clip IDs */
        std::list< int > DFreeToneIDs; /*!< Free tone IDs */
        std::vector< std::string > DMusicFilenames; /*!< Music file names */
        std::map< std::string, int > DMusicMapping; /*!< Music mapping */
        PaStream *DStream; /*!< Music stream */
        fluid_settings_t* DFluidSettings; /*!< Fluid settings */
        fluid_synth_t* DFluidSynthesizer; /*!< Fluid synthesizer */
        fluid_player_t* DFluidPlayer; /*!< Fluid player */
        fluid_audio_driver_t* DFluidAudioDriver; /*!< Fluid audio driver */
        pthread_mutex_t DMutex; /*!< Mutex */
        float *DSineWave; /*!< Sine wave */
        int DSampleRate; /*!< Sample rate */
        int DNextClipID; /*!< ID of next clip */
        int DNextToneID; /*!< ID of next tone */
        
    public:
	/**
	 * Blank constructor.
	 */
        CSoundLibraryMixer();

	/**
	 * Destructor.
	 */
        ~CSoundLibraryMixer();
        
	/**
	 * Getter function for number of sound clips .
	 */
        int ClipCount() const{
            return DSoundClips.size();  
        };
        
	/**
	 * Getter function for sample rate.
	 */
        int SampleRate() const{
            return DSampleRate;  
        };
        
	/**
	 * Finds clip.
	 *
	 * @param clipname Clip name to find.
	 * @return 
	 */
        int FindClip(const std::string &clipname) const;

	/**
	 * Finds song.
	 *
	 * @param songname Song name to find.
	 * @return Song being searched for.
	 */
        int FindSong(const std::string &songname) const;

	/**
	 * Gets duration of clip in milliseconds.
	 *
	 * @param index Index of sound clip to get
	 * @return Clip duration in milliseconds.
	 */
        int ClipDurationMS(int index);
        
	/**
	 * Callback function to CSoundLibraryMixer::Timestep().
	 *
	 * @param frames Frames
	 * @param timeinfo Time
	 * @param status Status
	 * @param data Song to timestep 
	 * @see Timestep()
	 */
        static int TimestepCallback(const void *in, void *out, unsigned long frames, const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags status, void *data);

	/**
	 * Timestep.
	 *
	 * @param frames Frames
	 * @param timeinfo Time
	 * @param status Status
	 * @return Continue status after performing timestep.
	 */
        int Timestep(void *out, unsigned long frames, const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags status);
        
	/**
	 * Loads music library.
	 *
	 * @param filename Name of file to open
	 * @return True if file was opened and scanned successfully, false otherwise.
	 */
        bool LoadLibrary(const std::string &filename);
        
	/**
	 * Plays clip.
	 *
	 * @param index Index of clip to play
	 * @param volume Volume to set clip volume to
	 * @param rightbias Right bias to set clip right bias to
	 */
        int PlayClip(int index, float volume, float rightbias);

	/**
	 * Plays tone.
	 *
	 * @param freq Current frequency of tone
	 * @param freqdecay Decay of frequency 
	 * @param volume Volume to set tone volume to
	 * @param volumedecay Decay of tone volume
	 * @param rightbias Right bias to set tone right bias to
	 * @param rightshift Right shift
	 */
        int PlayTone(float freq, float freqdecay, float volume, float volumedecay, float rightbias, float rightshift);

	/**
	 * Stop tone.
	 *
	 * @param id ID of tone to stop.
	 */
        void StopTone(int id);

	/**
	 * Checks if clip is completed.
	 *
	 * @param id ID of clip to check status of
	 * @return True if clip was found, false otherwise.
	 */
        bool ClipCompleted(int id);

	/**
	 * Plays song.
	 *
	 * @param index Index of song to play
	 * @param volume Volume to play song at
	 */
        void PlaySong(int index, float volume);

	/**
	 * Stops song.
	 */
        void StopSong();

	/**
	 * Sets song volume.
	 *
	 * @param volume Volume to set song volume to
	 */
        void SongVolume(float volume);
};

#endif
