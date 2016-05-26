#include "hud.h"
#include "cl_util.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
/*
#include <gl\glut.h>
#define GL_GLEXT_PROTOTYPES	
*/
//#include <gl\glaux.h>
//#include <gl/glext.h>									// Header File For The GLu32 Library

#include "blur.h"

#ifndef GL_TEXTURE_RECTANGLE_NV
#define GL_TEXTURE_RECTANGLE_NV 0x84F5
#endif


CTextureBlur::CTextureBlur()
{
}

void CTextureBlur::Init(int width, int height)
{	
		 // create a load of blank pixels to create textures with
     unsigned char* pBlankTex = new unsigned char[width*height*3];
     memset(pBlankTex, 0, width*height*3);

	  // Create the SCREEN-HOLDING TEXTURE
     glGenTextures(1, &g_texture);
     glBindTexture(GL_TEXTURE_RECTANGLE_NV, g_texture);

     glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA8, width, height, 0, GL_RGBA8, GL_UNSIGNED_BYTE, 0);

	   // free the memory
     delete[] pBlankTex;
}

void CTextureBlur::BindTexture(int width, int height)
{ 
     glBindTexture(GL_TEXTURE_RECTANGLE_NV, g_texture);
//	 glCopyTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA8, 0, 0, ScreenWidth, ScreenHeight, 0);
	 glCopyTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA8, 0, 0, ScreenWidth, ScreenHeight, 0);

	 //ConsolePrint("BindTexture\n"); // 30/08/02 November235: Just a debug
}

void CTextureBlur:: Draw(int width, int height)
{
	 // enable some OpenGL stuff
     glEnable(GL_TEXTURE_RECTANGLE_NV);     
     glColor3f(1,1,1);
     glDisable(GL_DEPTH_TEST);

	 glMatrixMode(GL_MODELVIEW);
     glPushMatrix();
     glLoadIdentity();
     
     glMatrixMode(GL_PROJECTION);
     glPushMatrix();
     glLoadIdentity();
     glOrtho(0, 1, 1, 0, 0.1, 100);


	 glBindTexture(GL_TEXTURE_RECTANGLE_NV, g_texture);

	 glColor4f(r,g,b,alpha);

	if ((int)gEngfuncs.pfnGetCvarFloat("r_blur") == 1)
	{
		glBegin(GL_QUADS);
		DrawQuad(ScreenWidth, ScreenHeight,0);

		glEnd();
	}
	else if ((int)gEngfuncs.pfnGetCvarFloat("r_blur") == 2)
	{
		glBegin(GL_QUADS);

		DrawQuad(width, height,of);

		glEnd();
	}

	 glMatrixMode(GL_PROJECTION);
     glPopMatrix();

     glMatrixMode(GL_MODELVIEW);
     glPopMatrix();

     glDisable(GL_TEXTURE_RECTANGLE_NV);
     glEnable(GL_DEPTH_TEST);
}

void CTextureBlur::DrawQuad(int width, int height,int of)
{
	 glTexCoord2f(0-of,0-of);
     glVertex3f(0, 1, -1);
     glTexCoord2f(0-of,height+of);
     glVertex3f(0, 0, -1);
     glTexCoord2f(width+of,height+of);
     glVertex3f(1, 0, -1);
     glTexCoord2f(width+of,0-of);
     glVertex3f(1, 1, -1);
}

bool CBlur::AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = 0.0f;
	float elapsedTime = 0.0;

	// Get current time in seconds  (milliseconds * .001 = seconds)
    float currentTime = GetTickCount() * 0.001f; 

	// Get the elapsed time by subtracting the current time from the last time
	elapsedTime = currentTime - lastTime;

	// Check if the time since we last checked is over (1 second / framesPerSecond)
    if( elapsedTime > (1.0f / desiredFrameRate) )
    {
		// Reset the last time
        lastTime = currentTime;	

		// Return TRUE, to animate the next frame of animation
        return true;
    }

	// We don't animate right now.
	return false;
}

CBlur gBlur;

void CBlur::InitScreen()
{	
	blur_pos = 1;

	m_pTextures.Init(320,240);
//	m_pTextures.Init(ScreenWidth,ScreenHeight);
	m_pTextures.r = 1;//was 0.3
	m_pTextures.g = 1;//was 0.3
	m_pTextures.b = 1;//was 0.3

	gEngfuncs.Con_Printf("Generate SrceenTexture");

	CVAR_CREATE( "r_blur", "0", 0 );
	CVAR_CREATE( "r_blur_rate", "70", 0 );
	CVAR_CREATE( "r_blur_fade", "0.4", 0 );
	CVAR_CREATE( "r_blur_strenght", "1", 0 );

}
int reset = 0;
int alpha = 0;

void CBlur::DrawBlur()
{
	int rate = (CVAR_GET_FLOAT("r_blur_rate"));
	float fade = (CVAR_GET_FLOAT("r_blur_fade"));

	if (CVAR_GET_FLOAT("r_blur") == 1)
	{
		if(rate <= 60)
		{
			//glBlendFunc(GL_ONE_MINUS_SRC_COLOR,GL_DST_COLOR);
			glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);  
			glEnable(GL_BLEND);	
		
			m_pTextures.alpha = fade;//was 0.32
			m_pTextures.r = 1.0;//was 0.3
			m_pTextures.g = 1.0;//was 0.3
			m_pTextures.b = 1.0;//was 0.3
			m_pTextures.Draw(ScreenWidth,ScreenHeight);

			if(AnimateNextFrame(rate))
				m_pTextures.BindTexture(ScreenWidth,ScreenHeight);
			
			glDisable(GL_BLEND);
		}
		else
		{
			glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);  
			//glBlendFunc(GL_SRC_ALPHA,GL_ONE); 
			glEnable(GL_BLEND);	

			m_pTextures.alpha = fade;//was 0.32
			m_pTextures.r = 1;//was 0.3
			m_pTextures.g = 1;//was 0.3
			m_pTextures.b = 1;//was 0.3
			
			m_pTextures.Draw(ScreenWidth,ScreenHeight);

			m_pTextures.BindTexture(ScreenWidth,ScreenHeight);

			glDisable(GL_BLEND);
		}
	}
	else if (CVAR_GET_FLOAT("r_blur") == 2)
	{
		if(rate <= 60)
		{
			if( gEngfuncs.GetClientTime() >= 5 )
			{
			int ofset = - (CVAR_GET_FLOAT("r_blur_strenght"));

			glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);  
			glEnable(GL_BLEND);	

			m_pTextures.Draw(ScreenWidth,ScreenHeight);

			m_pTextures.alpha = fade;//was 0.2
			m_pTextures.of = ofset;

			if(AnimateNextFrame(rate))
				m_pTextures.BindTexture(ScreenWidth,ScreenHeight);

			glDisable(GL_BLEND);
			}
		}
		else
		{
			int ofset = - (CVAR_GET_FLOAT("r_blur_strenght"));

			glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);  
			glEnable(GL_BLEND);	

			m_pTextures.Draw(ScreenWidth,ScreenHeight);

			m_pTextures.alpha = fade;//was 0.2
			m_pTextures.of = ofset;

			m_pTextures.BindTexture(ScreenWidth,ScreenHeight);

			glDisable(GL_BLEND);
		}
	}

}