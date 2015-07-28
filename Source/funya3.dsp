# Microsoft Developer Studio Project File - Name="funya3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=funya3 - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "funya3.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "funya3.mak" CFG="funya3 - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "funya3 - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "funya3 - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "funya3 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../"
# PROP Intermediate_Dir "中間ファイル"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 user32.lib gdi32.lib ole32.lib oleaut32.lib shell32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "funya3 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../"
# PROP Intermediate_Dir "中間ファイル"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "funya3 - Win32 Release"
# Name "funya3 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "funya3 Source"

# PROP Default_Filter "cpp"
# Begin Group "MapObject Source"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\f3MapObjectBanana.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectBase.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectEelPitcher.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectfff.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectFire.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectfunya.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectGeasprin.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectIce.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectIceBase.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectIceSource.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectItem.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectMain.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectmrframe.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectNeedle.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectWind.cpp
# End Source File
# End Group
# Begin Group "Scene Source"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\ExplainScene.cpp
# End Source File
# Begin Source File

SOURCE=.\f3SceneFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\FirstScene.cpp
# End Source File
# Begin Source File

SOURCE=.\GameScene.cpp
# End Source File
# Begin Source File

SOURCE=.\ReplayScene.cpp
# End Source File
# Begin Source File

SOURCE=.\TitleScene.cpp
# End Source File
# End Group
# Begin Group "Sound Source"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\BGMBase.cpp
# End Source File
# Begin Source File

SOURCE=.\BGMDefault.cpp
# End Source File
# Begin Source File

SOURCE=.\BGMUser.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlCheckTool.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectMusicFader.cpp
# End Source File
# Begin Source File

SOURCE=.\f3MIDIOutput.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\App.cpp
# End Source File
# Begin Source File

SOURCE=.\AVIRecorder.cpp
# End Source File
# Begin Source File

SOURCE=.\f3Input.cpp
# End Source File
# Begin Source File

SOURCE=.\f3Map.cpp
# End Source File
# Begin Source File

SOURCE=.\f3NavigationBar.cpp
# End Source File
# Begin Source File

SOURCE=.\f3Replay.cpp
# End Source File
# Begin Source File

SOURCE=.\f3Select.cpp
# End Source File
# Begin Source File

SOURCE=.\f3Setting.cpp
# End Source File
# Begin Source File

SOURCE=.\f3StageFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Version.cpp
# End Source File
# End Group
# Begin Group "yaneSDK Source"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\yaneSDK\enraAudioPlugin.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\enraPCMReaderFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\mtknDshelper.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\mtknLongsound.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\mtknPcmAcm.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\mtknPcmDirectShow.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\mtknPcmRaw.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\mtknWave.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\mtknWaveMmio.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\mtknWaveMp3.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneAppBase.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneAppFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneAppInitializer.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneAppIntervalTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneAppManager.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneCOMBase.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneCPUID.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneCriticalSection.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDIB32.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDIB32Alpha.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDIBDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDIBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDir.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDirectDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDirectSound.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneError.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFile.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFindWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFont.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFPSLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFPSTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneGraphicLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneIMEBase.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneLineParser.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneLZSS.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMemory.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMIDIOutput.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMIDIOutputDM.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMIDIOutputMCI.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMutex.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yanePlane.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yanePlaneBase.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yanePlaneEffectBlt.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneRand.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneScene.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSerialize.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSingleApp.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSinTable.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSound.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSoundBase.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneTextDIB32.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneThread.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneWinHook.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\resource\funya3.rc
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp

!IF  "$(CFG)" == "funya3 - Win32 Release"

!ELSEIF  "$(CFG)" == "funya3 - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "funya3 Header"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\App.h
# End Source File
# Begin Source File

SOURCE=.\AVIRecorder.h
# End Source File
# Begin Source File

SOURCE=.\BGMBase.h
# End Source File
# Begin Source File

SOURCE=.\BGMDefault.h
# End Source File
# Begin Source File

SOURCE=.\BGMUser.h
# End Source File
# Begin Source File

SOURCE=.\ControlCheckTool.h
# End Source File
# Begin Source File

SOURCE=.\DirectMusicFader.h
# End Source File
# Begin Source File

SOURCE=.\ExplainScene.h
# End Source File
# Begin Source File

SOURCE=.\f3Input.h
# End Source File
# Begin Source File

SOURCE=.\f3Map.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectBanana.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectBase.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectEelPitcher.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectEffect.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectfff.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectFire.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectfunya.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectGeasprin.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectIce.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectIceBase.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectIceSource.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectItem.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectMain.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectmrframe.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectNeedle.h
# End Source File
# Begin Source File

SOURCE=.\f3MapObjectWind.h
# End Source File
# Begin Source File

SOURCE=.\f3MIDIOutput.h
# End Source File
# Begin Source File

SOURCE=.\f3NavigationBar.h
# End Source File
# Begin Source File

SOURCE=.\f3Replay.h
# End Source File
# Begin Source File

SOURCE=.\f3SceneFactory.h
# End Source File
# Begin Source File

SOURCE=.\f3Select.h
# End Source File
# Begin Source File

SOURCE=.\f3Setting.h
# End Source File
# Begin Source File

SOURCE=.\f3StageFile.h
# End Source File
# Begin Source File

SOURCE=.\f3TL.h
# End Source File
# Begin Source File

SOURCE=.\FirstScene.h
# End Source File
# Begin Source File

SOURCE=.\GameScene.h
# End Source File
# Begin Source File

SOURCE=.\GameTitle.h
# End Source File
# Begin Source File

SOURCE=.\ReplayScene.h
# End Source File
# Begin Source File

SOURCE=.\ResourceManager.h
# End Source File
# Begin Source File

SOURCE=.\SceneBase.h
# End Source File
# Begin Source File

SOURCE=.\TitleScene.h
# End Source File
# Begin Source File

SOURCE=.\Version.h
# End Source File
# End Group
# Begin Group "yaneSDK Header"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\yaneSDK\YTL\auto_array.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\auto_ptrDx.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\auto_ptrEx.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\auto_vector_ptr.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\enraAudioPlugin.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\enraPCMReaderFactory.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\function_callbacker.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\mediator.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\mtknDshelper.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\mtknLongsound.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\mtknPcmreader.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\mtknWave.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\prototype_factory.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\smart_array.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\smart_ptr.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\smart_vector_ptr.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\surrogate_mother.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\swap.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\vectorEx.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneAcm.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneAppBase.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneAppFrame.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneAppInitializer.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneAppIntervalTimer.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneAppManager.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneAudioMixer.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneBGMLoader.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneBumpMap.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneCDDA.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneCellAutomaton.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneCOMBase.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneCPUID.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneCriticalSection.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDebugWindow.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDIB32.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDIB32Effect.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDIBDraw.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDIBitmap.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDir.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDirectDraw.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDirectInput.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDirectSound.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneDirtyRect.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneError.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFastDraw.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFastPlane.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFastPlaneInfo.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFile.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFindWindow.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFlagDebugger.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFont.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFPSLayer.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneFPSTimer.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneGameObjectBase.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneGraphicLoader.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneGTL.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneGUIButton.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneGUIParts.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneGUISlider.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneIMEBase.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneInteriorCounter.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneIntervalTimer.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneJoyStick.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneJoyStickDI.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneKey.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneKeyBase.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneLayer.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneLineParser.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneLoadCache.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneLZSS.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\YTL\yaneMacro.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMapIO.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMapLayer.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMIDIInput.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMIDILoader.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMIDIOutput.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMIDIOutputDM.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMIDIOutputMCI.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMorpher.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMouse.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMouseDecorator.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMouseLayer.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMsgDlg.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMsgSR.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneMutex.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yanePalette.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yanePlane.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yanePlaneBase.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yanePlaneEffectBlt.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yanePlaneLoader.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yanePlaneTransiter.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneRand.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneRegion.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneRegionHook.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneRootCounter.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneScenarioView.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneScene.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSceneTransiter.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSELoader.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSequencer.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSerialize.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneShell.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSingleApp.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSinTable.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSound.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSoundBase.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSoundFader.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneSoundLoader.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneStreamSound.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneStringMap.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneStringScanner.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneTextDIB32.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneTextDraw.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneTextFastPlane.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneTextLayer.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneTextPlane.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneThread.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneTimer.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneVirtualKey.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneVolumeFader.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneWindow.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneWinHook.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\yaneWMUSER.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\resource\resource.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\yaneSDK\config\yaneConfig.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\resource\funya3.ICO
# End Source File
# End Group
# Begin Source File

SOURCE=..\memo.txt
# End Source File
# End Target
# End Project
