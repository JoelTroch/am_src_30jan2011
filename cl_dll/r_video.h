#ifndef R_VIDEO_H
#define R_VIDEO_H

#include "fmod.h"
#include "avikit.h"
#include "GL/glut.h"
#include "vfw.h"
#include "com_model.h"

#define MAX_MAP_VIDS 16

typedef struct 
{
	bool used;
	bool usable;
	AVIKit*Avi;
	texture_t*tex;
	int xsize,ysize;
	char*frame_buffer;
	
	float frame;
	int last_uploaded_frame;
	
	int sample_rate;
	int num_channels;
	bool is16bit;

	AVIKitBuffer *buffer ;
	FSOUND_SAMPLE *sample;
	int channel;
	long bytes_per_sample;
	float video_duration;
	int fmod_frequency;
	float time;

}avi_texture_t;

void LoadAviForTexture(texture_t* tex);
void RefreshBspAvis();
void StopAviSound();

#endif