//========= Copyright © 2009, Destruction Team, All rights reserved. ================//
//																					 //
// Purpose:																			 //
//																					 //
// $NoKeywords: $																	 //
//===================================================================================//

#ifndef AENGINE_H
#define AENGINE_H

#include "fmod.hpp"
#include "fmod_errors.h"

typedef struct mp3_s
{

	char name[256];

} aengine_t;

class CAEngine
{
public:
	void InitFMODEx();
	void CreatePlaylist( void );
	void RandomMusicPlay( void );
	void RemoveCurrent ( void );
	void NextSong ( void );
	void SetPause ( void );
	void UnloadFMODEx();
	void PlayMedia( const char *filename, int playlisttrack );
	void StopMedia();
	void Play3D( const char *filename, float volume);
	void UpdateFMOD();
	void ERRCHECK( FMOD_RESULT result );
private:
	FMOD::System *system;
    FMOD_RESULT result;
	unsigned int version;
	int numdrivers;
	FMOD_SPEAKERMODE speakermode;
	FMOD_CAPS caps;
	char scname[256];
	FMOD::Sound *mediasound;
	FMOD::Sound *samplesound;
	FMOD::Channel *mediachannel;
	FMOD::Channel *samplechannel;
	aengine_t playlist[256];
	int	nbtrack;
	int	currenttrack;
	bool state;
};

extern CAEngine gAudioEngine;
#endif