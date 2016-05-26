//========= Copyright © 2009, Destruction Team, All rights reserved. ================//
//																					 //
// Purpose:																			 //
//																					 //
// $NoKeywords: $																	 //
//===================================================================================//

// Version for Arrangement
// Created by Richard Rohac
// Copyright (c) Richard Rohac 2009, All rights reserved.
// You are not allowed to use, edit this code without my permission.

//****************************************************//
//FMOD Ex, Firelight Technologies
//****************************************************//


#include "windows.h"
#include "hud.h"
#include "cl_util.h"
#include "r_efx.h"
#include "event_api.h"
#include "audioengine.h"
#include <io.h>

#define ARRAYSIZE(p)		(sizeof(p)/sizeof(p[0]))

/***********************************************************
*
 Enhanced GoldSrc Audio Engine Init FMOD Ex                *
*
***********************************************************/

void CAEngine::InitFMODEx()
{
	nbtrack			= 0;
	currenttrack	= -1;
	state			= false;

    result = FMOD::System_Create(&system);
    ERRCHECK(result);
    
    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  Arrangement requires %08x\n", version, FMOD_VERSION);
        return;
    }
    
    result = system->getNumDrivers(&numdrivers);
    ERRCHECK(result);

    if (numdrivers == 0)
    {
        result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
        ERRCHECK(result);
    }
    else
    {
        result = system->getDriverCaps(0, &caps, 0, 0, &speakermode);
        ERRCHECK(result);

        result = system->setSpeakerMode(speakermode);
        ERRCHECK(result);

        if (caps & FMOD_CAPS_HARDWARE_EMULATED)
        {
			MessageBox(NULL, "Audio Engine warning: You have disabled acceleration on your sound card!\n", "WARNING", MB_OK);
            result = system->setDSPBufferSize(1024, 10);
            ERRCHECK(result);
        }

        result = system->getDriverInfo(0, scname, 256, 0);
        ERRCHECK(result);

        if (strstr(scname, "SigmaTel")) 
        {
            result = system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
            ERRCHECK(result);
        }
    }

    result = system->init(100, FMOD_INIT_NORMAL, 0);
    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
    {
        result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
        ERRCHECK(result);
            
        result = system->init(100, FMOD_INIT_NORMAL, 0);
        ERRCHECK(result);
    }

}

/******************************
**      Create Play List     **
******************************/

void CAEngine::CreatePlaylist( void )
{
	struct		_finddata_t music_file;
	long		hFile;
	char		media[512];
	int			i, j;

	char	*mediatype[]	=	{
									"wav",
									"mp2",
									"mp3",
									"ogg",
									"it",
									"xm",
								};

	i = 0;
	 
	for ( j = 0; j < ARRAYSIZE( mediatype ); j++ )
	{
		sprintf( media, "%s/sound/fmod/*.%s", gEngfuncs.pfnGetGameDirectory(), mediatype[j] );

		if( ( hFile = _findfirst( media, &music_file ) ) == -1L )
		{
		}
		else
		{
			strcpy( playlist[i].name, music_file.name);

			i++;

			if ( i >= ARRAYSIZE( playlist ) )
				break;

			while( _findnext( hFile, &music_file ) == 0 )
			{
				strcpy( playlist[i].name, music_file.name );

				i++;

				if ( i >= ARRAYSIZE( playlist ) )
					break;
			}

			_findclose( hFile );
		}
	}

	nbtrack	= i;
}

void CAEngine :: RandomMusicPlay( void )
{
	int	random;
	random = gEngfuncs.pfnRandomLong( 0, ( nbtrack - 1 ) );

	PlayMedia( playlist[random].name, random );
}

void CAEngine :: RemoveCurrent ( void )
{
	if ( currenttrack < 0 )

		return;

	nbtrack--;

	for ( int i = 0; i <= nbtrack; i++ )
	{
		if ( ( i >= currenttrack ) && ( i < ( ARRAYSIZE( playlist ) - 1 ) ) )

			playlist[i] = playlist[(i+1)];

		else if ( i == ( ARRAYSIZE( playlist ) - 1 ) )

			strcpy( playlist[i].name, "" );
	}

	if ( nbtrack <= 0 )
		CreatePlaylist();
}

void CAEngine :: NextSong ( void )
{
	StopMedia();
	RandomMusicPlay();
}

void CAEngine :: SetPause ( void )
{
	//is this good method??
	switch( gEngfuncs.pfnRandomLong( 0, 1 ) )
	{
	case 0:
		result = mediachannel->setPaused( true );
		ERRCHECK(result);
		break;
	case 1:
		result = mediachannel->setPaused( false );
		ERRCHECK(result);
		break;
	}
}

/***********************************************************
*
 Unload FMOD Ex library and all resources                  *
 Must be called at ~CHUD!                                  *
*
***********************************************************/

void CAEngine::UnloadFMODEx ( void )
{
	result = system->release();
	ERRCHECK( result );
}

/***********************************************************
*
 Enhanced GoldSrc Audio Engine Media Playing               *
*
***********************************************************/

void CAEngine::PlayMedia( const char *filename, int playlisttrack )
{
	if ( !state )
		CreatePlaylist();

	bool bCreatedByOS = false;

	if ( strstr(filename, "scripts/OS") != NULL )//IMPORTANT! this check strings literaly, be aware of typos!
	bCreatedByOS = true;

	//Save real path, so, player will don't need to type full path, only name, and file will be stored in media directory
	char mediadirectory[256];
		
	if( bCreatedByOS )
	sprintf( mediadirectory, "%s/%s", gEngfuncs.pfnGetGameDirectory(), filename );
	else
	sprintf( mediadirectory, "%s/sound/fmod/%s", gEngfuncs.pfnGetGameDirectory(), filename );

	if ( strcmp( filename, "" ) == 0 )
	{
		StopMedia();
		RemoveCurrent();
	}

	//Prepare audio file (file opened by playmedia *.* from console, or, with trigger_ entity
	//Play as 3D sound, we want media files right?

	result = system->createStream(mediadirectory, FMOD_DEFAULT | FMOD_LOOP_NORMAL, 0, &mediasound);
	ERRCHECK(result);

	//Let's play this media file
	result = system->playSound(FMOD_CHANNEL_FREE, mediasound, false, &mediachannel);
	ERRCHECK(result);
	state = true;
	currenttrack = playlisttrack;
}

void CAEngine::StopMedia()
{
	mediachannel->stop();
	state = false;
}

void CAEngine::Play3D( const char *filename, float volume)
{
	char sounddirectory[256];
	sprintf( sounddirectory, "%s/sound/%s", gEngfuncs.pfnGetGameDirectory(), filename );

	result = system->createSound( sounddirectory, FMOD_3D_HEADRELATIVE, 0, &samplesound);

//	samplechannel->set3DAttributes(channel, origin, NULL); 

	ERRCHECK(result);
	result = system->playSound(FMOD_CHANNEL_FREE, samplesound, false, &samplechannel);
	ERRCHECK(result);  
	result = samplechannel->setVolume(volume);
	ERRCHECK(result); 
}

void CAEngine::UpdateFMOD()
{
	result = system->update();

	float volume = CVAR_GET_FLOAT("MP3Volume");
	result = mediachannel->setVolume(volume);

	gEngfuncs.Cvar_SetValue( "volume", 0.8 );

}

void CAEngine::ERRCHECK( FMOD_RESULT result )
{
	if (result != FMOD_OK)
    {
		gEngfuncs.Con_Printf("Audio engine error: (%d) %s\n", result, FMOD_ErrorString(result));
    }
}