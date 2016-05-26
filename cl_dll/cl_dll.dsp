# Microsoft Developer Studio Project File - Name="cl_dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=cl_dll - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cl_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cl_dll.mak" CFG="cl_dll - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cl_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cl_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "cl_dll - Win32 DEBUG2" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cl_dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /Zi /O2 /I "..\dlls" /I "..\common" /I "..\pm_shared" /I "..\engine" /I "..\utils\vgui\include" /I "..\game_shared" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "CLIENT_DLL" /D "CLIENT_WEAPONS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib winmm.lib ../utils/vgui/lib/win32_vc6/vgui.lib wsock32.lib opengl32.lib ../common/cg/cg.lib ../common/cg/cgGL.lib glu32.lib msacm32.lib ../avikit/lib/avikit.lib glaux.lib /nologo /subsystem:windows /dll /machine:I386 /out:".\Release\client.dll"
# SUBTRACT LINK32 /pdb:none /incremental:yes
# Begin Custom Build - COPING CL_DLL TO MOD DIRECTORY $(InputPath)
TargetDir=.\Release
InputPath=.\Release\client.dll
SOURCE="$(InputPath)"

"c:\Archivos de programa\Valve\AM\cl_dlls\client.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetDir)\client.dll "c:\Archivos de programa\Valve\AM\cl_dlls\client.dll"

# End Custom Build

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GR /GX /ZI /Od /Op /Oy /I "..\dlls" /I "..\common" /I "..\pm_shared" /I "..\engine" /I "..\utils\vgui\include" /I "..\game_shared" /I "..\fmod\include" /I "..\avikit\include" /I "..\fmod\lib" /I "..\avikit\lib" /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "CLIENT_DLL" /D "CLIENT_WEAPONS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib winmm.lib ../utils/vgui/lib/win32_vc6/vgui.lib wsock32.lib opengl32.lib ../common/cg/cg.lib ../common/cg/cgGL.lib glu32.lib vfw32.lib Msacm32.lib ../avikit/lib/avikit.lib ../fmod/lib/fmodvc.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\client.dll"
# SUBTRACT LINK32 /incremental:no /nodefaultlib
# Begin Custom Build - -----DEBUG MODE---- COPING CL_DLL TO MOD DIRECTORY $(InputPath)
TargetDir=.\Debug
InputPath=.\Debug\client.dll
SOURCE="$(InputPath)"

BuildCmds= \
	copy $(TargetDir)\client.dll "e:\Archivos de programa\Valve\AM\cl_dlls\client.dll" \
	copy $(TargetDir)\client.dll "e:\Archivos de programa\Valve\AMdemo\cl_dlls\client.dll" \
	

"e:\Archivos de programa\Valve\AM\cl_dlls\client.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"e:\Archivos de programa\Valve\AMdemo\cl_dlls\client.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "cl_dll - Win32 DEBUG2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cl_dll___Win32_DEBUG2"
# PROP BASE Intermediate_Dir "cl_dll___Win32_DEBUG2"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "cl_dll___Win32_DEBUG2"
# PROP Intermediate_Dir "cl_dll___Win32_DEBUG2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MTd /W3 /Gm /GR /GX /ZI /Od /Op /Oy /I "..\dlls" /I "..\common" /I "..\pm_shared" /I "..\engine" /I "..\utils\vgui\include" /I "..\game_shared" /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "CLIENT_DLL" /D "CLIENT_WEAPONS" /FR /YX /FD /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GR /GX /Zi /Ot /Oa /Ow /Og /Oi /Op /Oy /I "..\dlls" /I "..\common" /I "..\pm_shared" /I "..\engine" /I "..\utils\vgui\include" /I "..\game_shared" /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "CLIENT_DLL" /D "CLIENT_WEAPONS" /FR /YX /FD /c
# SUBTRACT CPP /Ox
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib winmm.lib ../utils/vgui/lib/win32_vc6/vgui.lib wsock32.lib opengl32.lib ../common/cg/cg.lib ../common/cg/cgGL.lib glu32.lib vfw32.lib msacm32.lib ../avikit/lib/avikit.lib ../common/libs/cvaLib.lib glu32.lib glaux.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\client.dll"
# SUBTRACT BASE LINK32 /incremental:no
# ADD LINK32 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib winmm.lib ../utils/vgui/lib/win32_vc6/vgui.lib wsock32.lib opengl32.lib ../common/cg/cg.lib ../common/cg/cgGL.lib glu32.lib vfw32.lib msacm32.lib ../avikit/lib/avikit.lib ../common/libs/cvaLib.lib glu32.lib glaux.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\client.dll"
# SUBTRACT LINK32 /incremental:no
# Begin Custom Build - -----DEBUG MODE---- COPING CL_DLL TO MOD DIRECTORY $(InputPath)
TargetDir=.\Debug
InputPath=.\Debug\client.dll
SOURCE="$(InputPath)"

"c:\Archivos de programa\Valve\AM\cl_dlls\client.dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetDir)\client.dll "c:\Archivos de programa\Valve\AM\cl_dlls\client.dll"

# End Custom Build

!ENDIF 

# Begin Target

# Name "cl_dll - Win32 Release"
# Name "cl_dll - Win32 Debug"
# Name "cl_dll - Win32 DEBUG2"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Group "Am"

# PROP Default_Filter "*.CPP"
# Begin Source File

SOURCE=.\ev_hldm.cpp
# End Source File
# Begin Source File

SOURCE=.\gl_postprocess.cpp
# End Source File
# Begin Source File

SOURCE=.\gl_sky.cpp
# End Source File
# Begin Source File

SOURCE=.\hl\hl_baseentity.cpp
# End Source File
# Begin Source File

SOURCE=.\hl\hl_events.cpp
# End Source File
# Begin Source File

SOURCE=.\hl\hl_objects.cpp
# End Source File
# Begin Source File

SOURCE=.\hl\hl_weapons.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_noise.cpp
# End Source File
# Begin Source File

SOURCE=..\common\interface.cpp
# End Source File
# Begin Source File

SOURCE=.\log.cpp
# End Source File
# Begin Source File

SOURCE=.\r_video.cpp
# End Source File
# Begin Source File

SOURCE=.\tri_rope.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_keypad.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_OS.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_weapon_panel.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_banmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_status.cpp
# End Source File
# End Group
# Begin Group "Particles and TGA Loaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\particles\grass_particle.cpp
# End Source File
# Begin Source File

SOURCE=.\particles\grass_particles.cpp
# End Source File
# Begin Source File

SOURCE=.\particles\mapped_particle.cpp
# End Source File
# Begin Source File

SOURCE=.\particles\mapped_particles.cpp
# End Source File
# Begin Source File

SOURCE=.\particles\particle.cpp
# End Source File
# Begin Source File

SOURCE=.\particles\particle_systems.cpp
# End Source File
# Begin Source File

SOURCE=.\particles\system_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\particles\tga_loader.cpp
# End Source File
# End Group
# Begin Group "Weapons"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dlls\squeakgrenade.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_1911.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_357.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_92f.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_ak47.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_deagle.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_epipen.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_famas.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_flash.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_glock18.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_handgrenade.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_heal.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_hl2_crowbar.cpp

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "cl_dll - Win32 DEBUG2"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dlls\wep_hl2_handgrenade.cpp

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "cl_dll - Win32 DEBUG2"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dlls\wep_hl2_irgun.cpp

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "cl_dll - Win32 DEBUG2"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dlls\wep_hl2_mp7.cpp

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "cl_dll - Win32 DEBUG2"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dlls\wep_hl2_shotgun.cpp

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "cl_dll - Win32 DEBUG2"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dlls\wep_hl2_usp.cpp

!IF  "$(CFG)" == "cl_dll - Win32 Release"

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "cl_dll - Win32 DEBUG2"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\dlls\wep_knife_heavy.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_m16.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_m249.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_m4a1.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_mp5.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_oicw.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_pda.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_rpg.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_satchel.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_shotgun.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_shotgun_lesslethal.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_skorpion.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_sniper.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_svd.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_tripmine.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_usas.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_uzi.cpp
# End Source File
# End Group
# Begin Group "Vgui"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\game_shared\vgui_checkbutton2.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_cine.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ClassMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ConsolePanel.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ControlConfigPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_CustomObjects.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_grid.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_int.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_listbox.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_loadtga.cpp
# End Source File
# Begin Source File

SOURCE=.\VGUI_Menu.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_MOTDWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_nvg.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_panel.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_paranoiaHUD.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_paranoiaText.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_radio.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ricardo.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_SchemeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ScorePanel.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_scrollbar2.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_ServerBrowser.cpp
# End Source File
# Begin Source File

SOURCE=..\game_shared\vgui_slider2.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_SpectatorPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_subtitles.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_TeamFortressViewport.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_teammenu.cpp
# End Source File
# Begin Source File

SOURCE=.\vgui_tips.cpp
# End Source File
# End Group
# Begin Group "HUD & Modifiers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ammo.cpp
# End Source File
# Begin Source File

SOURCE=.\ammo_secondary.cpp
# End Source File
# Begin Source File

SOURCE=.\ammohistory.cpp
# End Source File
# Begin Source File

SOURCE=.\battery.cpp
# End Source File
# Begin Source File

SOURCE=.\BLUR.CPP
# End Source File
# Begin Source File

SOURCE=.\cine.cpp
# End Source File
# Begin Source File

SOURCE=.\com_weapons.cpp
# End Source File
# Begin Source File

SOURCE=.\death.cpp
# End Source File
# Begin Source File

SOURCE=.\flashlight.cpp
# End Source File
# Begin Source File

SOURCE=.\geiger.cpp
# End Source File
# Begin Source File

SOURCE=.\GLOW.CPP
# End Source File
# Begin Source File

SOURCE=.\health.cpp
# End Source File
# Begin Source File

SOURCE=.\hud.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_msg.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_redraw.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_servers.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_spectator.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_update.cpp
# End Source File
# Begin Source File

SOURCE=.\hudradar.cpp
# End Source File
# Begin Source File

SOURCE=.\luzbar.cpp
# End Source File
# Begin Source File

SOURCE=.\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\message.cpp
# End Source File
# Begin Source File

SOURCE=.\money.cpp
# End Source File
# Begin Source File

SOURCE=.\nvg.cpp
# End Source File
# Begin Source File

SOURCE=.\ricardo.cpp
# End Source File
# Begin Source File

SOURCE=.\saytext.cpp
# End Source File
# Begin Source File

SOURCE=.\SlowMotionBar.cpp
# End Source File
# Begin Source File

SOURCE=.\status_icons.cpp
# End Source File
# Begin Source File

SOURCE=.\statusbar.cpp
# End Source File
# Begin Source File

SOURCE=.\tbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\text_message.cpp
# End Source File
# Begin Source File

SOURCE=.\train.cpp
# End Source File
# Begin Source File

SOURCE=.\VGUI_HUD.CPP
# End Source File
# End Group
# Begin Group "render"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gl_debug.cpp
# End Source File
# Begin Source File

SOURCE=.\gl_decals.cpp
# End Source File
# Begin Source File

SOURCE=.\gl_dlight.h
# End Source File
# Begin Source File

SOURCE=.\gl_light_dynamic.cpp
# End Source File
# Begin Source File

SOURCE=.\gl_lightmap.cpp
# End Source File
# Begin Source File

SOURCE=.\gl_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\gl_manager.h
# End Source File
# Begin Source File

SOURCE=.\gl_matbump.cpp
# End Source File
# Begin Source File

SOURCE=.\gl_matbump.h
# End Source File
# Begin Source File

SOURCE=.\gl_rsurf.cpp
# End Source File
# Begin Source File

SOURCE=.\gl_texloader.cpp
# End Source File
# Begin Source File

SOURCE=.\gl_texloader.h
# End Source File
# Begin Source File

SOURCE=.\glows.cpp
# End Source File
# Begin Source File

SOURCE=.\quake_bsp.cpp
# End Source File
# End Group
# Begin Group "water shader"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cg_shader.cpp
# End Source File
# Begin Source File

SOURCE=.\cg_shader.h
# End Source File
# Begin Source File

SOURCE=.\gl_bored.cpp
# End Source File
# Begin Source File

SOURCE=.\gl_bored.h
# End Source File
# Begin Source File

SOURCE=.\gl_pbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\gl_pbuffer.h
# End Source File
# Begin Source File

SOURCE=.\glext.h
# End Source File
# Begin Source File

SOURCE=.\texture.h
# End Source File
# Begin Source File

SOURCE=.\textures.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\BreakOut.cpp
# End Source File
# Begin Source File

SOURCE=.\cdll_int.cpp
# End Source File
# Begin Source File

SOURCE=.\demo.cpp
# End Source File
# Begin Source File

SOURCE=.\entity.cpp
# End Source File
# Begin Source File

SOURCE=.\ev_common.cpp
# End Source File
# Begin Source File

SOURCE=.\events.cpp
# End Source File
# Begin Source File

SOURCE=.\exception.cpp
# End Source File
# Begin Source File

SOURCE=.\GameStudioModelRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\hud_lensflare.cpp
# End Source File
# Begin Source File

SOURCE=.\in_camera.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\inputw32.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3.cpp
# End Source File
# Begin Source File

SOURCE=.\overview.cpp

!IF  "$(CFG)" == "cl_dll - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "cl_dll - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "cl_dll - Win32 DEBUG2"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\parsemsg.cpp
# End Source File
# Begin Source File

SOURCE=.\parsemsg.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_debug.c
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_math.c
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_shared.c
# End Source File
# Begin Source File

SOURCE=.\RAIN.CPP
# End Source File
# Begin Source File

SOURCE=.\studio_util.cpp
# End Source File
# Begin Source File

SOURCE=.\StudioModelRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\tri.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\view.cpp
# End Source File
# Begin Source File

SOURCE=..\dlls\wep_fists.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\ammo.h
# End Source File
# Begin Source File

SOURCE=.\ammohistory.h
# End Source File
# Begin Source File

SOURCE=.\BLUR.H
# End Source File
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=.\cl_dll.h
# End Source File
# Begin Source File

SOURCE=.\cl_util.h
# End Source File
# Begin Source File

SOURCE=.\com_weapons.h
# End Source File
# Begin Source File

SOURCE=.\demo.h
# End Source File
# Begin Source File

SOURCE=.\ev_hldm.h
# End Source File
# Begin Source File

SOURCE=.\eventscripts.h
# End Source File
# Begin Source File

SOURCE=.\exception.h
# End Source File
# Begin Source File

SOURCE=.\GameStudioModelRenderer.h
# End Source File
# Begin Source File

SOURCE=.\getfont.h
# End Source File
# Begin Source File

SOURCE=.\gl_renderer.h
# End Source File
# Begin Source File

SOURCE=.\glmanager.h
# End Source File
# Begin Source File

SOURCE=.\health.h
# End Source File
# Begin Source File

SOURCE=.\hud.h
# End Source File
# Begin Source File

SOURCE=.\hud_iface.h
# End Source File
# Begin Source File

SOURCE=.\hud_servers.h
# End Source File
# Begin Source File

SOURCE=.\hud_servers_priv.h
# End Source File
# Begin Source File

SOURCE=.\hud_spectator.h
# End Source File
# Begin Source File

SOURCE=.\in_defs.h
# End Source File
# Begin Source File

SOURCE=..\common\itrackeruser.h
# End Source File
# Begin Source File

SOURCE=.\kbutton.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\mp3.h
# End Source File
# Begin Source File

SOURCE=.\overview.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_debug.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_defs.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_info.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_materials.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_movevars.h
# End Source File
# Begin Source File

SOURCE=..\pm_shared\pm_shared.h
# End Source File
# Begin Source File

SOURCE=..\common\r_studioint.h
# End Source File
# Begin Source File

SOURCE=.\rain.h
# End Source File
# Begin Source File

SOURCE=..\common\ref_params.h
# End Source File
# Begin Source File

SOURCE=.\studio_util.h
# End Source File
# Begin Source File

SOURCE=.\StudioModelRenderer.h
# End Source File
# Begin Source File

SOURCE=.\tri_rope.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\util_vector.h
# End Source File
# Begin Source File

SOURCE=.\vgui_ConsolePanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_ControlConfigPanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_hud.h
# End Source File
# Begin Source File

SOURCE=.\vgui_int.h
# End Source File
# Begin Source File

SOURCE=.\vgui_keypad.h
# End Source File
# Begin Source File

SOURCE=.\vgui_OS.h
# End Source File
# Begin Source File

SOURCE=.\vgui_paranoiatext.h
# End Source File
# Begin Source File

SOURCE=.\vgui_pickup.h
# End Source File
# Begin Source File

SOURCE=.\vgui_radio.h
# End Source File
# Begin Source File

SOURCE=.\vgui_SchemeManager.h
# End Source File
# Begin Source File

SOURCE=.\vgui_ScorePanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_screenmsg.h
# End Source File
# Begin Source File

SOURCE=.\vgui_ServerBrowser.h
# End Source File
# Begin Source File

SOURCE=.\vgui_shadowtext.h
# End Source File
# Begin Source File

SOURCE=.\vgui_SpectatorPanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_subtitles.h
# End Source File
# Begin Source File

SOURCE=.\vgui_tabpanel.h
# End Source File
# Begin Source File

SOURCE=.\vgui_TeamFortressViewport.h
# End Source File
# Begin Source File

SOURCE=.\vgui_tips.h
# End Source File
# Begin Source File

SOURCE=.\vgui_weapon_panel.h
# End Source File
# Begin Source File

SOURCE=.\view.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_banmgr.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_status.h
# End Source File
# Begin Source File

SOURCE=..\game_shared\voice_vgui_tweakdlg.h
# End Source File
# Begin Source File

SOURCE=.\wrect.h
# End Source File
# End Group
# End Target
# End Project
