//Saving SensibleData
//By SysOp, FolderExists by James "Minuit"

#pragma warning(disable: 4530)// semantics are not enabled

#include "extdll.h"
#include "util.h"
#include "cbase.h"

#include <fstream> 
#include <iostream>
#include <windows.h>

using namespace std;

#if !defined (INVALID_FILE_ATTRIBUTES) 
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1) 
#endif
/*struct SensibleData 
{
	char name[40];
	int value;
	int unlocked_pages;
};
*/
struct SensibleData 
{
	char name[40];
	ULONG xp_points;//-2147483647 to 2147483647
	int unlocked_pages;
	int iFinalCode;
		
	ULONG uKills;//-2147483647 to 2147483647
	ULONG uHeadshots;//-2147483647 to 2147483647
	ULONG uMelees;//-2147483647 to 2147483647
};

BOOL FolderExists(const char *strFolderName)
{   
    return GetFileAttributes(strFolderName) != INVALID_FILE_ATTRIBUTES;   
}

bool LoadDB(SensibleData *per) 
{
	char szGamePath[32];
	char szFile[64];

	GET_GAME_DIR( szGamePath );
	sprintf( szFile, "%s/Profiles", szGamePath );

	if (FolderExists(szFile) == FALSE)
		CreateDirectory( szFile, NULL );

//	ALERT( at_console, "old szFile = '%s'\n", szFile ); 

	sprintf( szFile, "%s/Profiles/%s.p7r", szGamePath, CVAR_GET_STRING( "name" ) );
//	ALERT( at_console, "szFile = '%s'\n", szFile ); 

	std::ifstream fin( szFile, ios_base::in | ios_base::binary);

	if (!fin.is_open())
	return false;

	fin.read((char*)per, sizeof(SensibleData));
	fin.close();// close the file - not necessary, but we should be neat

	return true;
}

bool SaveDB(SensibleData *per) 
{
	char szGamePath[32];
	char szFile[64];

	GET_GAME_DIR( szGamePath );

	sprintf( szFile, "%s/Profiles/%s.p7r", szGamePath, CVAR_GET_STRING( "name" ) );
//	ALERT( at_console, "szFile = '%s'\n", szFile ); 

	std::ofstream fout( szFile, ios_base::out | ios_base::binary | ios_base::trunc);

	if (!fout.is_open())
	return false;

	fout.write((const char*)per, sizeof(SensibleData));
	fout.close();// close the file - not necessary, but we should be neat

	return true;
}

// Checks if a database is present. If not, a default one is created
// Called at CbasePlayer::Spawn
void DBInit()
{ 	
	SensibleData per;

	if (LoadDB(&per)) 
	{
	/*	ALERT( at_console, "Loaded Database...\n" ); 
		ALERT( at_console, "Name: %s\n", per.name ); 
		ALERT( at_console, "Value: %i\n", per.value ); 
		ALERT( at_console, "Unlocked pages: %i\n", per.unlocked_pages );*/
	}
	else
	{
	/*	ALERT( at_console, "\nUnable to read database!\n" );
		ALERT( at_console, "Creating default database info...\n" );*/

		SensibleData per;

		sprintf( per.name, "%s", CVAR_GET_STRING( "name" ) );

		per.xp_points = 0;
		per.unlocked_pages = 0;

		per.iFinalCode = RANDOM_LONG(0000,9999);

		per.uKills = 0;
		per.uHeadshots = 0;
		per.uMelees = 0;
			
		CVAR_SET_FLOAT( "xp_points", int(per.xp_points) );//client side uses

		CVAR_SET_FLOAT( "score_killed", int(per.uKills) );//client side uses
		CVAR_SET_FLOAT( "score_head", int(per.uHeadshots) );//client side uses
		CVAR_SET_FLOAT( "score_knifed", int(per.uMelees) );//client side uses

	/*	ALERT( at_console, "Loaded Database...\n" ); 
		ALERT( at_console, "Name: %s\n", per.name ); 
		ALERT( at_console, "Value: %i\n", per.value ); 
		ALERT( at_console, "Unlocked pages: %i\n", per.unlocked_pages );*/
		
		if (SaveDB(&per))
		ALERT( at_console, "\nDatabase saved!\n" ); 
		else
		ALERT( at_console, "\nUnable to save database!\n" ); 
	}
}

// Compare names. Changing names or modify the file p7r file wil cause, likely, an error.
// Called right after DBInit
void DBCheckIntegrity()
{
	SensibleData per;

	if (LoadDB(&per)) 
	{
		if ( strcmp( per.name, CVAR_GET_STRING( "name" ) ) )//if different
		{
			char msg[64];
			sprintf( msg, "Database file for '%s' is corrupted!\nPress OK to quit", CVAR_GET_STRING( "name" ) );

			MessageBox(NULL, msg, "FATAL ERROR", MB_OK | MB_ICONERROR);

			SERVER_COMMAND("quit\n");
		}
		else
		{
		/*	char msg[64];
			sprintf( msg, "Welcome '%s'!\n", CVAR_GET_STRING( "name" ) );
			MessageBox(NULL, "Your profile has been loaded sucessfully!", msg, MB_OK | MB_ICONINFORMATION);*/
/*
			char acUserName[100];
			DWORD nUserName = sizeof(acUserName);

			if (GetUserName(acUserName, &nUserName)) 
			{
				char msg[64];
				sprintf( msg, "Welcome '%s'", acUserName );

			//	ALERT( at_console, "USERNAME = '%s'\n", acUserName ); 

				MessageBox(NULL, "Your profile has been loaded sucessfully!", msg, MB_OK | MB_ICONINFORMATION);
			}*/
		}
	}
}

//////////
// HELPERS
//////////

// Called when you change your XP
void DBSetExperiencePoints( int iValue )
{
	SensibleData per;

	if (LoadDB(&per)) 
	{
	/*	ALERT( at_console, "Loaded Database...\n" ); 
		ALERT( at_console, "Name: %s\n", per.name ); 
		ALERT( at_console, "Value: %i\n", per.value ); 
		ALERT( at_console, "Unlocked pages: %i\n", per.unlocked_pages );

		ALERT( at_console, "Changing database info...\n" );
		*/
 		per.xp_points += iValue;
	//	ALERT( at_console, "Value: %i\n", per.value );
			
		CVAR_SET_FLOAT( "xp_points", int(per.xp_points) );
		
		if (SaveDB(&per))
		ALERT( at_console, "\nDatabase saved!\n" ); 
		else
		ALERT( at_console, "\nUnable to save database!\n" ); 
	}
	else
	{
		DBInit();//create a database for this player right now
	}
}
int DBGetExperiencePoints()
{
	SensibleData per;

	if (LoadDB(&per)) 
	{
	/*	ALERT( at_console, "Loaded Database...\n" ); 
		ALERT( at_console, "Name: %s\n", per.name ); 
		ALERT( at_console, "Value: %i\n", per.value ); 
		ALERT( at_console, "Unlocked pages: %i\n", per.unlocked_pages );*/

		return per.xp_points;
	}
	else
	{
		ALERT( at_console, "\nUnable to read database!\n" );
		return 0;
	}
}

void DBGet()
{
	SensibleData per;

	if (LoadDB(&per)) 
	{
		ALERT( at_console, "Loaded Database...\n" ); 

		ALERT( at_console, "Name: %s\n", per.name ); 
		ALERT( at_console, "Experience points: %i\n", per.xp_points ); 
		
		ALERT( at_console, "Final code: %i\n", per.iFinalCode ); 

		ALERT( at_console, "Unlocked pages: %i\n", per.unlocked_pages );
		ALERT( at_console, "Kills: %i\n", per.uKills );
		ALERT( at_console, "Headshots: %i\n", per.uHeadshots );
		ALERT( at_console, "Melees: %i\n", per.uMelees );
	}
	else
	ALERT( at_console, "\nUnable to read database!\n" );
}

void DBSetPageUnlocked()
{
	SensibleData per;

	if (LoadDB(&per)) 
	{
	/*	ALERT( at_console, "Loaded Database...\n" ); 
		ALERT( at_console, "Name: %s\n", per.name ); 
		ALERT( at_console, "Value: %i\n", per.value ); 
		ALERT( at_console, "Unlocked pages: %i\n", per.unlocked_pages );

		ALERT( at_console, "Changing database info...\n" );*/
		
 		per.unlocked_pages += 1;
	//	ALERT( at_console, "unlocked_pages: %i\n", per.unlocked_pages );

		CVAR_SET_FLOAT( "score_pages", int(per.unlocked_pages) );//client side uses

		if (SaveDB(&per))
		ALERT( at_console, "\nDatabase saved!\n" ); 
		else
		ALERT( at_console, "\nUnable to save database!\n" ); 
	}
	else
	{
		DBInit();//create a database for this player right now
	}
}

int DBGetPageUnlocked()
{
	SensibleData per;

	if (LoadDB(&per)) 
	{
	/*	ALERT( at_console, "Loaded Database...\n" ); 
		ALERT( at_console, "Name: %s\n", per.name ); 
		ALERT( at_console, "Value: %i\n", per.value ); 
		ALERT( at_console, "Unlocked pages: %i\n", per.unlocked_pages );*/

		return per.unlocked_pages;
	}
	else
	{
		ALERT( at_console, "\nUnable to read database!\n" );
		return 0;
	}
}

void DBSetKills( int iValue )
{
	SensibleData per;

	if (LoadDB(&per)) 
	{		
 		per.uKills += iValue;
			
		CVAR_SET_FLOAT( "score_killed", int(per.uKills) );//client side uses

		if (SaveDB(&per))
		ALERT( at_console, "\nDatabase saved!\n" ); 
		else
		ALERT( at_console, "\nUnable to save database!\n" ); 
	}
	else
	DBInit();//create a database for this player right now
}

void DBSetHeadshots( int iValue )
{
	SensibleData per;

	if (LoadDB(&per)) 
	{		
 		per.uHeadshots += iValue;
	
		CVAR_SET_FLOAT( "score_head", int(per.uHeadshots) );//client side uses

		if (SaveDB(&per))
		ALERT( at_console, "\nDatabase saved!\n" ); 
		else
		ALERT( at_console, "\nUnable to save database!\n" ); 
	}
	else
	DBInit();//create a database for this player right now
}

void DBSetMelees( int iValue )
{
	SensibleData per;

	if (LoadDB(&per)) 
	{		
 		per.uMelees += iValue;

		CVAR_SET_FLOAT( "score_knifed", int(per.uMelees) );//client side uses

		if (SaveDB(&per))
		ALERT( at_console, "\nDatabase saved!\n" ); 
		else
		ALERT( at_console, "\nUnable to save database!\n" ); 
	}
	else
	DBInit();//create a database for this player right now
}

int DBGetFinalCode()
{
	SensibleData per;

	if (LoadDB(&per)) 
	{
	/*	ALERT( at_console, "Loaded Database...\n" ); 
		ALERT( at_console, "Name: %s\n", per.name ); 
		ALERT( at_console, "Value: %i\n", per.value ); 
		ALERT( at_console, "Unlocked pages: %i\n", per.unlocked_pages );*/
		ALERT( at_console, "per.iFinalCode: %i\n", per.iFinalCode );

		return per.iFinalCode;
	}
	else
	{
		ALERT( at_console, "\nUnable to read database!\n" );
		return 0;
	}
}

// Returns number of bytes in a file
int GetFileSize(char *file) 
{
	char szGamePath[32];
	char szFile[32];

	ALERT( at_console, "original entry = '%s'", file ); 

	GET_GAME_DIR( szGamePath );
	sprintf( szFile, "%s/%s", szGamePath, file );

	ALERT( at_console, "modified entry = '%s'\n", szFile ); 

	// Open file
	std::ifstream f(szFile, std::ios_base::binary | std::ios_base::in);

	// Make sure it's opened correctly
	if ( !f.good() || f.eof() || !f.is_open() ) 
	{
		return 0;
	}

	// Beginning of file
	f.seekg(0, std::ios_base::beg);
	std::ifstream::pos_type begin_pos = f.tellg();

	// End of file
	f.seekg(0, std::ios_base::end);

	// Return the difference
	return static_cast<int>(f.tellg() - begin_pos);
}





/*
#include <sys/stat.h>
#include <iostream>
#include <time.h>


using namespace std;

int main()
{
	char filename1[] = "c:\\autoexec.bat";
	char filename2[] = "c:\\pagefile.sys";
	
	struct stat buf1;
	struct stat buf2;
	
	//stat puts a lot of info about a file into buf1 or buf2
	if (!stat(filename1, &buf1) && !stat(filename2, &buf2))
	{
        //buf1.st_mtime returns last modified of filename1, difftime just gets time elapsed between two times
		if(difftime(buf1.st_mtime, buf2.st_mtime) < 0)
		{
            cout << "File 2 was modified more recently than file 1.\n";
        } else {
            cout << "File 1 was modified more recently than file 2.\n";
        }
        
	} else
	{//one of the files probably didn't exist
		cout << "error getting mtime\n";
	}
	system("PAUSE");
	return 0;
}
*/

