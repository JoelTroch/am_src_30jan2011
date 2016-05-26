/***
*
*		   °°
*			 °     °°°°°
*			° °   °  °  °
*		   °   °  °  °  °
*		  °     ° °  °  °
*	   HALF-LIFE: ARRANGEMENT
*
*	AR Software (c) 2004-2007. ArrangeMent, S2P Physics, Spirit Of Half-Life and their
*	logos are the property of their respective owners.
*
*	You should have received a copy of the Developers Readme File
*   along with Arrange Mode's Source Code for Half-Life. If you
*	are going to copy, modify, translate or distribute this file
*	you should agree whit the terms of Developers Readme File.
*
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*
*	This product includes information related to Half-Life 2 licensed from Valve 
*	(c) 2004. 
*
*	All Rights Reserved.
*
*	Modifications by SysOp (sysop_axis@hotmail).
*
***/

//	For now this file does nothing, just load a file and read it
//	Will be very fun to implement it.

//-----------------------
//	AI Skills Data File
//-----------------------
//
//	Purpose: 
//
//	Loads a file which contains important keys and set a few variables used by monsters.
//	This can change A LOT AI procedures, than just change skill values.
//
//	The file is loaded at world.cpp, just before spawn. Of course you can
//	check 'skill' value and load a different file.
//
//----------------------
//	AI Collector  
//----------------------
//
//	Purpose: 
//
//	Read the values stored at "ai_player_" on .cfg file and set a few
//	variables, used by monster's AI.
//	
//	The main purpose is detect player's style. I mean, whit this the
//	monster can collect info about the player and how plays Am.
//
//	A profile will be created, following the values readed before.
//
//	An example of profile could be 'Sniper' (a player who uses a lot
//	Sig3000, duck a lot, and use the 'walk' key). It also can check
//	if the player is a talented headshoter, if so, the monster
//	will move a bit harder and will duck and stand faster and so on...
//	
//	A security check must be implemented to prevent cheats.
//

//| Code by SysOp |

#if _I_WANT_TO_COMPILE_THIS_CRAP 

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"

//this does nothing for now
#define MAX_COLLECTIONS	10

class CAICollector
{
public:

	CAICollector() {}
	~CAICollector() {}

	BOOL ParseAICollected (void);

	int iSkillCollected[MAX_COLLECTIONS];

//private:
};

CAICollector myCollector;

char *COM_ParseFile (char *data);
static char com_token[ 1500 ];
void ParseAISkillsDataFile(char *datafile);

char *COM_ParseFile (char *data)
{
	int             c;
	int             len;
	
	len = 0;
	com_token[0] = 0;
	
	if (!data)
		return NULL;
		
// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;                    // end of file;
		data++;
	}
	
// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}
	

// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		}
	}

// parse single characters
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data+1;
	}

// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
	if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
			break;
	} while (c>32);
	
	com_token[len] = 0;
	return data;
}

void ParseAISkillsDataFile(char *datafile)
{
	char szToken[1024];
	int length = 0;

	char *szFile = (char *)LOAD_FILE_FOR_ME( datafile, &length );

	if (!szFile)
	{
		ALERT ( at_console, "\n-----------------------------------------------------------\n");
		ALERT ( at_console, "AI Skills: Failed to load AI File!!\n" );
		ALERT ( at_console, "-----------------------------------------------------------\n\n");

		return;
	}
	else
	{
		ALERT ( at_console, "\n-----------------------------------------------------------\n");
		ALERT ( at_console, "AI Skills: Loading AI File...\n" );
		ALERT ( at_console, "-----------------------------------------------------------\n\n");

		szFile = COM_ParseFile(szFile);

		while (szFile)
		{
			ALERT ( at_console, "while (szFile)...\n" );

			if ( stricmp( szToken, "can_drop_weapons" ) )
			{
				szFile = COM_ParseFile(szFile);	
				ALERT ( at_console, "can_drop_weapons\n");
			}
			
			if ( stricmp( szToken, "can_duck" ) )
			{
				szFile = COM_ParseFile(szFile);
				ALERT ( at_console, "can_duck\n");
			}
			
			if ( stricmp( szToken, "can_throw_he_grenades" ) )
			{
				szFile = COM_ParseFile(szFile);
				ALERT ( at_console, "can_throw_he_grenades\n");
			}
			
			if ( stricmp( szToken, "can_throw_sg_grenades" ) )
			{
				szFile = COM_ParseFile(szFile);
				ALERT ( at_console, "can_throw_sg_grenades\n");
			}
			
			if ( stricmp( szToken, "can_throw_fb_grenades" ) )
			{
				szFile = COM_ParseFile(szFile);
				ALERT ( at_console, "can_throw_fb_grenades\n");
			}
			
			if ( stricmp( szToken, "can_surrender" ) )
			{
				szFile = COM_ParseFile(szFile);
				ALERT ( at_console, "can_surrender\n");
			}
		
			if ( stricmp( szToken, "airfriction" ) )
			{
				szFile = COM_ParseFile(szFile);
				ALERT ( at_console, "can_drop_weapons\n");
			}
			
			if ( stricmp( szToken, "sim_speed" ) )
			{
				szFile = COM_ParseFile(szFile);
				ALERT ( at_console, "can_drop_weapons\n");
			}
			
			if ( stricmp( szToken, "update_range" ) )
			{
				szFile = COM_ParseFile(szFile);
				ALERT ( at_console, "can_drop_weapons\n");
			}
			
			if ( stricmp( szToken, "sprite" ) )
			{
				szFile = COM_ParseFile(szFile);
				ALERT ( at_console, "can_drop_weapons\n");
			}
			
			if ( stricmp( szToken, "can_collide" ) )
			{
				szFile = COM_ParseFile(szFile);
				ALERT ( at_console, "can_drop_weapons\n");
			}
					
			szFile = COM_ParseFile(szFile);
		}
	}
		
	ALERT ( at_console, "\n-----------------------------------------------------------\n");
	ALERT ( at_console, "AI Skills: Reading done!\n" );
	ALERT ( at_console, "-----------------------------------------------------------\n\n");

	FREE_FILE( szFile );
}



BOOL CAICollector::ParseAICollected (void)
{
	int iMax = -1;
	int iMaxPos = 0;

	for (int i = 0; i < MAX_COLLECTIONS; i++)
	{
			
		if (myCollector.iSkillCollected[i]>iMax)
		{
			iMax= myCollector.iSkillCollected[i];
			iMaxPos = i;
		}

		int iMaxSkill = max (0, myCollector.iSkillCollected[MAX_COLLECTIONS]);

		int iGamesPlayed = CVAR_GET_FLOAT("ai_player_sessions");

		int iTotalPercent = iMaxSkill*iGamesPlayed/100;

		if (iTotalPercent)
		{
			;
		}
	}

/*
	int iAI_Ducks = CVAR_GET_FLOAT("ai_player_ducks");
	int iAI_Heads = CVAR_GET_FLOAT("ai_player_headshots");
	int iAI_Knifed = CVAR_GET_FLOAT("ai_player_knifed");
	int iAI_Kills = CVAR_GET_FLOAT("ai_player_kills");
	int iAI_Jumps = CVAR_GET_FLOAT("ai_player_jumps");
	int iAI_Walks = CVAR_GET_FLOAT("ai_player_walks");
	int iAI_Sniper = CVAR_GET_FLOAT("ai_player_deploy_sniper");
	int iAI_Nades = CVAR_GET_FLOAT("ai_player_deploy_grenades");
	int iAI_Silent = CVAR_GET_FLOAT("ai_player_put_silencer");
	int iAI_Heals = CVAR_GET_FLOAT("ai_player_heals");
*/
	return TRUE;
}

#endif