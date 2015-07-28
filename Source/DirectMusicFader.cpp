// DirectMusicFader.cpp: CDirectMusicFader クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirectMusicFader.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CDirectMusicFader::CDirectMusicFader(IDirectMusicPerformance* lpPerf)
{
	(m_lpPerf = lpPerf)->AddRef();
	m_Check = new CControlCheckTool();
	m_Check->SetListener(this);

	IDirectMusicGraph* pGraph;
	if (SUCCEEDED(CoCreateInstance(CLSID_DirectMusicGraph,NULL,
			CLSCTX_INPROC, IID_IDirectMusicGraph,(void**)&pGraph))) {
		if (SUCCEEDED(pGraph->InsertTool( (IDirectMusicTool*)m_Check, NULL, 0, 0)))
			m_lpPerf->SetGraph(pGraph);
		pGraph->Release();
	} 
}

CDirectMusicFader::~CDirectMusicFader()
{
	IDirectMusicGraph* pGraph;
	if (SUCCEEDED(m_lpPerf->GetGraph((IDirectMusicGraph**)&pGraph))) {
		pGraph->RemoveTool(m_Check);
		pGraph->Release();
	} 
	delete m_Check;	// この時点でm_Checkの参照カウントは1のはず
	m_lpPerf->Release();
}

void CDirectMusicFader::OnFrame()
{
	TFadeInfo* info;
	for (int channel=0; channel<16; channel++) {
		// ボリューム
		info = m_Volume+channel;
		if (info->difference != 0 && info->value != info->end) {
			BringClose(info->value, info->end, info->difference);
			if (floor(info->value)!=info->previous) {
				info->previous = floor(info->value);

				DMUS_MIDI_PMSG* pMidi;
				if (SUCCEEDED(m_lpPerf->AllocPMsg(sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pMidi))) {
					pMidi->dwSize = sizeof(DMUS_MIDI_PMSG);
					pMidi->rtTime = 0;
					pMidi->mtTime = 0;
					pMidi->dwFlags = DMUS_PMSGF_REFTIME|DMUS_PMSGF_TOOL_IMMEDIATE;
					pMidi->dwPChannel = channel;
					pMidi->dwVirtualTrackID = 0;
					pMidi->pTool = NULL;
					pMidi->pGraph = NULL;
					pMidi->dwType = DMUS_PMSGT_MIDI;
					pMidi->dwVoiceID = 0;
					pMidi->dwGroupID = 0xFFFFFFF;
					pMidi->punkUser = NULL;
					pMidi->bStatus = 0xb0;//|channel;
					pMidi->bByte1 = 0x07;
					pMidi->bByte2 = info->previous;
					pMidi->bPad[0] = 0;

					m_lpPerf->SendPMsg((DMUS_PMSG*)pMidi);
				}
			}
		}

		// パン
		info = m_Pan+channel;
		if (info->difference != 0 && info->value != info->end) {
			BringClose(info->value, info->end, info->difference);
			if (floor(info->value)!=info->previous) {
				info->previous = floor(info->value);

				DMUS_MIDI_PMSG* pMidi;
				if (SUCCEEDED(m_lpPerf->AllocPMsg(sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pMidi))) {
					pMidi->dwSize = sizeof(DMUS_MIDI_PMSG);
					pMidi->rtTime = 0;
					pMidi->mtTime = 0;
					pMidi->dwFlags = DMUS_PMSGF_REFTIME|DMUS_PMSGF_TOOL_IMMEDIATE;
					pMidi->dwPChannel = channel;
					pMidi->dwVirtualTrackID = 0;
					pMidi->pTool = NULL;
					pMidi->pGraph = NULL;
					pMidi->dwType = DMUS_PMSGT_MIDI;
					pMidi->dwVoiceID = 0;
					pMidi->dwGroupID = 0xFFFFFFF;
					pMidi->punkUser = NULL;
					pMidi->bStatus = 0xb0;//|channel;
					pMidi->bByte1 = 0x0a;
					pMidi->bByte2 = info->previous;
					pMidi->bPad[0] = 0;

					m_lpPerf->SendPMsg((DMUS_PMSG*)pMidi);
				}
			}
		}

		// エクスプレッション
		info = m_Expression+channel;
		if (info->difference != 0 && info->value != info->end) {
			BringClose(info->value, info->end, info->difference);
			if (floor(info->value)!=info->previous) {
				info->previous = floor(info->value);

				DMUS_MIDI_PMSG* pMidi;
				if (SUCCEEDED(m_lpPerf->AllocPMsg(sizeof(DMUS_MIDI_PMSG), (DMUS_PMSG**)&pMidi))) {
					pMidi->dwSize = sizeof(DMUS_MIDI_PMSG);
					pMidi->rtTime = 0;
					pMidi->mtTime = 0;
					pMidi->dwFlags = DMUS_PMSGF_REFTIME|DMUS_PMSGF_TOOL_IMMEDIATE;
					pMidi->dwPChannel = channel;
					pMidi->dwVirtualTrackID = 0;
					pMidi->pTool = NULL;
					pMidi->pGraph = NULL;
					pMidi->dwType = DMUS_PMSGT_MIDI;
					pMidi->dwVoiceID = 0;
					pMidi->dwGroupID = 0xFFFFFFF;
					pMidi->punkUser = NULL;
					pMidi->bStatus = 0xb0;//|channel;
					pMidi->bByte1 = 0x0b;
					pMidi->bByte2 = info->previous;
					pMidi->bPad[0] = 0;

					m_lpPerf->SendPMsg((DMUS_PMSG*)pMidi);
				}
			}
		}
	}
}
