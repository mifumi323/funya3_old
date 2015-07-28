// DirectMusicFader.h: CDirectMusicFader クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTMUSICFADER_H__22BF1B94_4D43_4746_9AF4_4E0B88F7A03B__INCLUDED_)
#define AFX_DIRECTMUSICFADER_H__22BF1B94_4D43_4746_9AF4_4E0B88F7A03B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ControlCheckTool.h"

class CDirectMusicFader : public CMidiParamListener 
{
protected:
	void OnChangeVolume(BYTE channel, BYTE value) {
		m_Volume[channel].value = m_Volume[channel].end
			= m_Volume[channel].previous = value;
		m_Volume[channel].difference = 0;
	}
	void OnPanChange(BYTE channel, BYTE value) {
		m_Pan[channel].value = m_Pan[channel].end
			= m_Pan[channel].previous = value;
		m_Pan[channel].difference = 0;
	}
	void OnExpressionChange(BYTE channel, BYTE value) {
		m_Expression[channel].value = m_Expression[channel].end
			= m_Expression[channel].previous = value;
		m_Expression[channel].difference = 0;
	}

	CControlCheckTool* m_Check;
	IDirectMusicPerformance* m_lpPerf;

	struct TFadeInfo {
		float value, previous, end, difference;
	} m_Volume[16], m_Expression[16], m_Pan[16];
public:
	void FadeVolume(BYTE channel, float end, float difference) {
		m_Volume[channel].end = end;
		m_Volume[channel].difference = difference;
	}
	void FadePan(BYTE channel, float end, float difference) {
		m_Pan[channel].end = end;
		m_Pan[channel].difference = difference;
	}
	void FadeExpression(BYTE channel, float end, float difference) {
		m_Expression[channel].end = end;
		m_Expression[channel].difference = difference;
	}
	void OnFrame();
	CDirectMusicFader(IDirectMusicPerformance* lpPerf);
	virtual ~CDirectMusicFader();

};

#endif // !defined(AFX_DIRECTMUSICFADER_H__22BF1B94_4D43_4746_9AF4_4E0B88F7A03B__INCLUDED_)
