// BGMDefault.cpp: CBGMDefault クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BGMDefault.h"
#include "f3MIDIOutput.h"
#include "DirectMusicFader.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CBGMDefault::CBGMDefault()
{
	m_pMIDI = new Cf3MIDIOutput*[MIDI_SIZE];
	for (int i=0; i<MIDI_SIZE; i++) {
		m_pMIDI[i] = NULL;
	}
	m_MIDINo = MIDI_RESET;
	m_pFader = NULL;
}

CBGMDefault::~CBGMDefault()
{
	for (int i=0; i<MIDI_SIZE; i++) {
		DELETE_SAFE(m_pMIDI[i]);
	}
	delete [] m_pMIDI;
	DELETE_SAFE(m_pFader);
}

void CBGMDefault::InnerPlay(BGMNumber no)
{
	switch (no) {
	case BGMN_SIRENT: {
		break;
					  }
	case BGMN_TITLE: {
		break;
					  }
	case BGMN_GAMEBANANA: {
//		break;
					  }
	case BGMN_GAMENEEDLE: {
//		break;
					  }
	case BGMN_GAMEGEASPRIN: {
//		break;
					  }
	case BGMN_GAMEWIND: {
//		break;
					  }
	case BGMN_GAMEICE: {
//		break;
					  }
	case BGMN_GAMEFIRE: {
//		break;
					  }
	case BGMN_GAMEMRFRAME: {
//		break;
					  }
	case BGMN_GAMEEELPITCHER: {
//		break;
					  }
	case BGMN_GAMEFUNYA: {
		if (m_pMIDI[MIDI_PLAY]) {
			m_pMIDI[MIDI_PLAY]->Play();
			m_MIDINo = MIDI_PLAY;
		}
		break;
					  }
	case BGMN_CLEAR: {
		if (m_pMIDI[MIDI_CLEAR]) {
			m_pMIDI[MIDI_CLEAR]->Play();
			m_MIDINo = MIDI_CLEAR;
		}
		break;
					  }
	case BGMN_ENDING: {
		break;
					  }
	case BGMN_MISS: {
		if (m_pMIDI[MIDI_MISS]) {
			m_pMIDI[MIDI_MISS]->Play();
			m_MIDINo = MIDI_MISS;
		}
		break;
					  }
	case BGMN_GAMEOVER: {
		break;
					  }
	case BGMN_EXPLAIN: {
		if (m_pMIDI[MIDI_EXPLAIN]) {
			m_pMIDI[MIDI_EXPLAIN]->Play();
			m_MIDINo = MIDI_EXPLAIN;
		}
		break;
					  }
	}
}

void CBGMDefault::InnerStop()
{
	if (m_pMIDI[m_MIDINo]) m_pMIDI[m_MIDINo]->Stop();
}

string CBGMDefault::GetClassName()
{
	return "CBGMDefault";
}

bool CBGMDefault::Init()
{
	if (Load(MIDI_RESET,"Reset",false,3072,6144)) return false;
	if (Load(MIDI_PLAY,"Main",false,3072,187392)) return false;
//	if (Load(MIDI_SLEEP,"Sleep",true)) return false;
//	if (Load(MIDI_AWAKE,"Awake",true)) return false;
	if (Load(MIDI_GEASPRIN,"Geasprin",true,0,0,DMUS_SEGF_BEAT)) return false;
	if (Load(MIDI_CLEAR,"Clear",false,12288,24576)) return false;
	if (Load(MIDI_MISS,"Miss",false,12288,24576)) return false;
	if (Load(MIDI_EXPLAIN,"Explain",false,3072,187392)) return false;
	m_pFader = new CDirectMusicFader(CDirectMusic::GetCDirectMusic()->GetDirectMusicPerformance());
	return true;
}

LRESULT CBGMDefault::Load(int no, string filename, bool secondary, DWORD start, DWORD end, DWORD option)
{
	m_pMIDI[no] = new Cf3MIDIOutput;
	if (m_pMIDI[no]->Open("resource/"+filename+".mid",secondary)) return 1;
	if (secondary && m_pMIDI[no]->SetSecondary()) return 2;
	if (m_pMIDI[no]->SetLoopMode(!secondary)) return 3;
	if (m_pMIDI[no]->SetLoopPos(start, end)) return 4;
	if (option) m_pMIDI[no]->SetPlayOption(option);
	return 0;
}

void CBGMDefault::MusicEffect(MENumber no)
{
	switch (no) {
	case MEN_SLEEP: {
//		if (m_pMIDI[MIDI_SLEEP]) m_pMIDI[MIDI_SLEEP]->Play();
		for (int i=0; i<16; i++) m_pFader->FadeExpression(i, i==15?100:0, 1);
		break;
					  }
	case MEN_AWAKE: {
//		if (m_pMIDI[MIDI_AWAKE]) m_pMIDI[MIDI_AWAKE]->Play();
		for (int i=0; i<16; i++) m_pFader->FadeExpression(i, i!=15?100:0, 1);
		break;
					  }
	case MEN_GEASPRIN: {
		if (m_pMIDI[MIDI_GEASPRIN]) m_pMIDI[MIDI_GEASPRIN]->Play();
		break;
					  }
	}
}

void CBGMDefault::Update()
{
	m_pFader->OnFrame();
}
