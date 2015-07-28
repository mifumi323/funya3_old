// BGMUser.cpp: CBGMUser クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BGMUser.h"
#include "yaneSDK/yaneMIDIOutput.h"
#include "yaneSDK/yaneSound.h"
#include "yaneSDK/yaneStreamSound.h"
#include "yaneSDK/yaneFile.h"
#include "yaneSDK/yaneLineParser.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CBGMUser::CBGMUser()
{
	for (int i=0; i<BGMN_SIZE; i++) {
		m_Sound[i] = NULL;
	}
}

CBGMUser::~CBGMUser()
{
	for (int i=0; i<BGMN_SIZE; i++) {
		DELETE_SAFE(m_Sound[i]);
	}
}

bool CBGMUser::Init()
{
	string BGMName[BGMN_SIZE];
	BGMName[BGMN_TITLE]			= "TITLE";
	BGMName[BGMN_GAMEFUNYA]		= "GAMEFUNYA";
	BGMName[BGMN_GAMEBANANA]	= "GAMEBANANA";
	BGMName[BGMN_GAMENEEDLE]	= "GAMENEEDLE";
	BGMName[BGMN_GAMEGEASPRIN]	= "GAMEGEASPRIN";
	BGMName[BGMN_GAMEWIND]		= "GAMEWIND";
	BGMName[BGMN_GAMEICE]		= "GAMEICE";
	BGMName[BGMN_GAMEFIRE]		= "GAMEFIRE";
	BGMName[BGMN_GAMEMRFRAME]	= "GAMEMRFRAME";
	BGMName[BGMN_GAMEEELPITCHER]= "GAMEEELPITCHER";
	BGMName[BGMN_CLEAR]			= "CLEAR";
	BGMName[BGMN_ENDING]		= "ENDING";
	BGMName[BGMN_MISS]			= "MISS";
	BGMName[BGMN_GAMEOVER]		= "GAMEOVER";
	BGMName[BGMN_EXPLAIN]		= "EXPLAIN";
	CFile file;
	file.Read("BGM.dat");
	string line, data, suffix;
	CLineParser parser;
	CMIDIOutput* lpm;
	CSoundBase* lps;
	int i;
	while (file.ReadLine(line)==0) {
		parser.SetLine((char*)line.c_str());
		for (i=BGMN_SIRENT+1; i<BGMN_SIZE; i++) {
			if (BGMName[i].size() && parser.IsMatch((char*)BGMName[i].c_str())) {
				if (parser.GetStr(data)==0) {
					if (CFile::PathFileExists(data)) {
						suffix = CFile::GetSuffixOf(data);
						CFile::ToLower(suffix);
						if (suffix=="mid") {
							lpm = new CMIDIOutput;
							if (lpm->Open(data.c_str())==0) {
								lpm->SetLoopMode(parser.IsMatch("LOOP"));
								m_Sound[i] = lpm;
							}else{
								delete lpm;
							}
						}else{
#ifdef USE_StreamSound
							// BGMに使うぐらいだからさぞかし重いに違いない
							lps = new CStreamSound;
#else
							lps = new CSound;
#endif
							if (lps->Open(data.c_str())==0) {
								lps->SetLoopMode(parser.IsMatch("LOOP"));
								m_Sound[i] = lps;
							}else{
								delete lps;
							}
						}
					}
				}
				break;
			}
		}
	}
	return true;
}

void CBGMUser::InnerPlay(BGMNumber no)
{
	if (m_Sound[no]!=NULL) m_Sound[no]->Play();
}

void CBGMUser::InnerStop()
{
	if (m_Sound[m_PlayNo]!=NULL) m_Sound[m_PlayNo]->Stop();
}

string CBGMUser::GetClassName()
{
	return "CBGMUser";
}
