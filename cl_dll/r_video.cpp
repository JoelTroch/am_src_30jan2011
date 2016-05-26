#include "hud.h"
#include "cl_util.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>

#include "com_model.h"

#include "fmod.h"
#include "avikit.h"
#include "r_video.h"

extern globalvars_t *gpGlobals;

avi_texture_t Avis[MAX_MAP_VIDS];

avi_texture_t*AllocAviTexture()
{
	for(int i=0;i<MAX_MAP_VIDS;i++)
	{
		if (!Avis[i].used)
		{
			Avis[i].used=true;
			return &Avis[i];
		}
	}

	gEngfuncs.Con_DPrintf("AllocAviTexture(): no free slots!\n");

	return NULL;
}

bool CheckAviError(AVIKit*avi)
{
	int err;
	char *emsg;
	
	bool ret=false;

	while ((err = avi->getError(&emsg)) != AVIKIT_NOERROR)
	{
		ret=true;
		switch (err)
		{
		case AVIKIT_ERROR_BADFILE: // "No handler found, or file does not exist."
		case AVIKIT_ERROR_BADFORMAT: // "Corrupt AVI or unknown format."
		case AVIKIT_ERROR_MEMERROR: // "Not enough memory to open AVI."
		case AVIKIT_ERROR_DISKERROR: // "Disk error attempting to read AVI."
			
			// errors opening video
		case AVIKIT_ERROR_NOVIDEO: // "No video stream found in specified file."
		case AVIKIT_ERROR_VIDEOERROR: // "VFW failed to read video stream."
			gEngfuncs.Con_DPrintf("LoadAviForTexture() error:%s\n",emsg);
			break;
			
			// errors opening audio
		case AVIKIT_ERROR_ACM: // "ACM failed to open conversion stream."
			gEngfuncs.Con_DPrintf("LoadAviForTexture(): ACM failed to open conversion stream (is the codec installed?)\n");
			break;
			
		case AVIKIT_ERROR_ACMCODEC: // "ACM does not support this audio codec." (WMA).
		default:
			gEngfuncs.Con_DPrintf("LoadAviForTexture() error:%s\n",emsg);
			break;
		}
	}
	return ret;
}

void BufferSync( avi_texture_t *avi_tex, float time)
{
	long playpos = FSOUND_GetCurrentPosition(avi_tex->channel);
	long s_update_pos, s_update_length;

	avi_tex->buffer->makeUpdate(playpos, time, &s_update_pos, &s_update_length);

	if (s_update_length <= 0)
	{
		return;
	}

	char *first, *second;
	unsigned int firstlen, secondlen;

	FSOUND_Sample_Lock(avi_tex->sample, avi_tex->bytes_per_sample*s_update_pos, avi_tex->bytes_per_sample*s_update_length, 
					(void**)&first, (void**)&second, &firstlen, &secondlen);

	avi_tex->buffer->getUpdate(first, s_update_pos, firstlen/avi_tex->bytes_per_sample);
	if (secondlen != 0)
	{
		avi_tex->buffer->getUpdate(second, s_update_pos+firstlen/avi_tex->bytes_per_sample, secondlen/avi_tex->bytes_per_sample);
	}

	FSOUND_Sample_Unlock(avi_tex->sample, first, second, firstlen, secondlen);
}

void LoadSoundStream( avi_texture_t *avi_tex )
{
	FSOUND_SetOutput( -1 );
	FSOUND_SetBufferSize( 200 );
	FSOUND_SetDriver( 0 );

	FSOUND_Init(44100, 32, 0);

	// set up the sound sample and channel
	long channels, frequency;

	bool is16bit;

	if (avi_tex->Avi->getAudioInfo(&frequency, &avi_tex->bytes_per_sample, &channels, &is16bit))
	{
		avi_tex->fmod_frequency = frequency;

		avi_tex->buffer = new AVIKitBuffer(avi_tex->Avi, avi_tex->fmod_frequency);
    
		int flags = FSOUND_2D | FSOUND_LOOP_NORMAL;

		flags |= (is16bit) ? FSOUND_16BITS : FSOUND_8BITS;
		flags |= (channels == 2) ? FSOUND_STEREO : FSOUND_MONO;

		avi_tex->sample = FSOUND_Sample_Alloc(FSOUND_FREE, avi_tex->fmod_frequency, flags, avi_tex->fmod_frequency, 255, 127, 0);

		avi_tex->channel = FSOUND_PlaySound(FSOUND_FREE, avi_tex->sample);

		BufferSync( avi_tex, 0);
	}
}

void LoadAviForTexture(texture_t *tex)
{
	char vidname[8];
	memset( vidname, 0, sizeof( vidname ) );
	sscanf(tex->name,"avi@%s",vidname);

	char pszFullName[512];
	memset( pszFullName, 0, sizeof(pszFullName) );
	sprintf(pszFullName, "%s/media/%s.avi",gEngfuncs.pfnGetGameDirectory(), vidname );

	avi_texture_t*avi_tex=AllocAviTexture();
	avi_tex->tex=tex;

	avi_tex->Avi= new AVIKit( pszFullName );

	if (CheckAviError(avi_tex->Avi)) 
	{
		delete avi_tex->Avi;
		avi_tex->used=false;
		return;
	}

	long xsize,ysize;
	float len;

	avi_tex->Avi->getVideoInfo(&xsize,&ysize,&len);
	avi_tex->frame_buffer=(char*)malloc(xsize*ysize*3);
	avi_tex->xsize=xsize;
	avi_tex->ysize=ysize;

	avi_tex->video_duration = len;

	glBindTexture(GL_TEXTURE_2D,tex->gl_texturenum);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xsize, xsize, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	LoadSoundStream( avi_tex );
	
	long sample_rate,bps,channels;
	bool is16bit;

	avi_tex->Avi->getAudioInfo(&sample_rate,&bps,&channels,&is16bit);

	gEngfuncs.Con_DPrintf("Loading avi %s - %d frames,",pszFullName,avi_tex->Avi->NumVideoFrames());
	gEngfuncs.Con_DPrintf("Audio: %d Hz %d bps  %d channels %s",sample_rate,bps,channels,is16bit ? "16bit" : "8bit");

}

void DBG_DumpFrame(avi_texture_t*tex)
{
	char pszRawName[512];
	memset( pszRawName, 0, sizeof(pszRawName) );
	sprintf(pszRawName, "%s/media/%s.raw",gEngfuncs.pfnGetGameDirectory(), tex->tex->name );

	FILE*fp=fopen( pszRawName,"wb" );
	fwrite(tex->frame_buffer,tex->xsize*tex->ysize*3,1,fp);
	fclose(fp);
}

void RefreshBspAvis()
{
	for(int i=0;i<MAX_MAP_VIDS;i++)
	{
		avi_texture_t*tex=&Avis[i];

		if (!tex->used) continue;

		// use the sound buffers' play position as the timer for syncing
		static long sample_lasttime = FSOUND_GetCurrentPosition( tex->channel);
		long sample_delta = FSOUND_GetCurrentPosition(tex->channel) - sample_lasttime;
		sample_lasttime += sample_delta;

		float delta = (float(sample_delta) / float(tex->fmod_frequency));

		// the buffer might have rolled over. it's long enough, so we can rest
		// assured that it'll only do so once per frame at most
		if (delta < 0)
		{
			delta += 1;
		}

		// update the clock
		tex->time += delta;

		// restart the video after it finishes
		if (tex->time > tex->video_duration + 5)
		{
			tex->time = 0;
		}

		// update the sound buffer
		if (tex->buffer != NULL)
		{
			BufferSync( tex, tex->time );
		}

		// update the current frame, if necessary
		static int of = -1;
		int f = tex->Avi->getVideoFrameNumber(tex->time);

		if (f != of)
		{
			tex->Avi->getVideoFrame(tex->frame_buffer, f);

			glBindTexture(GL_TEXTURE_2D,tex->tex->gl_texturenum);
			glTexSubImage2D(GL_TEXTURE_2D, 0,0, 0, tex->xsize,tex->ysize,GL_RGB, GL_UNSIGNED_BYTE, tex->frame_buffer);

			of = f;
		}
	}
}

void StopAviSound()
{
	FSOUND_Close();
}
