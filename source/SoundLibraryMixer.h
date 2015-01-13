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
         *
         * Holds information about a clip that is being played
         */
        typedef struct{
            int DIdentification; //!< Id assigned from CSoundLibraryMixer::DFreeClipIDs
            int DIndex; //!< Index of clip in CSoundLibraryMixer::DSoundClips
            int DOffset; //!< Current number of frames into the clip
            float DVolume; //!< Volume to play at
            float DRightBias; //!< Right bias for stereo playback
        } SClipStatus, *SClipStatusRef;

        /**
         * Tone status struct
         * Holds information about a tone that is being played
         */
        typedef struct{
            int DIdentification; //!< Id assigned from CSoundLibraryMixer::DFreeToneIDs
            float DCurrentFrequency; //!< Current frequency
            float DCurrentStep; //!< Current step
            float DFrequencyDecay; //!< Frequency decay
            float DVolume; //!< Volume
            float DVolumeDecay; //!< Volume decay
            float DRightBias; //!< Right bias
            float DRightShift; //!< Right shift
        } SToneStatus, *SToneStatusRef;

        std::vector< CSoundClip > DSoundClips; //!< Sound clips in library
        std::map< std::string, int > DMapping; //!< Map of sound clips by name
        std::list< SClipStatus > DClipsInProgress; //!< Clips currently being played
        std::list< SToneStatus > DTonesInProgress; //!< Tones currently being played
        std::list< int > DFreeClipIDs; //!< Free clip IDs
        std::list< int > DFreeToneIDs; //!< Free tone IDs
        std::vector< std::string > DMusicFilenames; //!< Music file names in library
        std::map< std::string, int > DMusicMapping; //!< Map of music by name
        PaStream *DStream; //!< Music stream
        fluid_settings_t* DFluidSettings; //!< Fluid settings
        fluid_synth_t* DFluidSynthesizer; //!< Fluid synthesizer
        fluid_player_t* DFluidPlayer; //!< Fluid player
        fluid_audio_driver_t* DFluidAudioDriver; //!< Fluid audio driver
        pthread_mutex_t DMutex; //!< Mutual exclusion used to allow for synchronised update of songs when timestepping
        float *DSineWave; //!< Sine wave
        int DSampleRate; //!< Sample rate used to calculate frequency and volume decay in tone
        int DNextClipID; //!< Next clip id to give as identification if none free
        int DNextToneID; //!< Next tone id to give as identification if none free

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
         * Getter function for number of sound clips.
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
         * @return id of clip if found otherwise -1
         */
        int FindClip(const std::string &clipname) const;

        /**
         * Finds song.
         *
         * @param songname Song name to find.
         * @return id of song being searched if found otherwise -1
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
         * @return True if clip was completed, false otherwise.
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
