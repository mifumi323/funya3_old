#include "stdafx.h"
#include "yaneSoundFader.h"

///////////////////////////////////////////////////////////

CSoundFader::CSoundFader(void){
	m_lpSound = NULL;
}

CSoundFader::~CSoundFader(){
	Stop();
	ResetVolume();
}

///////////////////////////////////////////////////////////

LRESULT CSoundFader::ResetVolume(void){
	CVolumeFadeInfo& info = m_FadeInfo;
	info.m_bFadeIn		= false;
	info.m_bFadeOut		= false;
	SetVolume(DSBVOLUME_MAX);
	return 0;
}

LRESULT CSoundFader::FadeOut(int dwTime){
	//	�t�F�[�h�A�E�g���J�n������
	CVolumeFadeInfo& info = m_FadeInfo;
	info.m_Timer.Reset();
	info.m_dwWholeTime	= dwTime;
	info.m_bFadeIn		= false;
	info.m_bFadeOut		= true;

	//	�C���^�[�o���^�C�}�[�𔭓�������O�O
	TimerProc();
	if (!IsTimer()) return Start(20);	//	���ꂭ�炢�ŏ\�����H
	return 0;
}

LRESULT CSoundFader::FadeIn(int dwTime){
	//	�t�F�[�h�C�����J�n������
	CVolumeFadeInfo& info = m_FadeInfo;
	info.m_Timer.Reset();
	info.m_dwWholeTime	= dwTime;
	info.m_bFadeIn		= true;
	info.m_bFadeOut		= false;

	//	�C���^�[�o���^�C�}�[�𔭓�������O�O
	TimerProc();
	if (!IsTimer()) return Start(20);	//	���ꂭ�炢�ŏ\�����H
	return 0;
}

void CSoundFader::TimerProc(void){
	bool bReset = false;
	CVolumeFadeInfo& info = m_FadeInfo;
	if (info.m_bFadeIn) {
		// DSBVOLUME_MAX�i�����Ȃ��j :		0
		// DSBVOLUME_MIN�i�����j	 : -10000
		double d = (double)info.m_Timer.Get() / info.m_dwWholeTime;
		LONG l = (1-d)*DSBVOLUME_MIN;
		if (l>DSBVOLUME_MAX) { l=DSBVOLUME_MAX; info.m_bFadeIn = false; }
		SetVolume(l);
	} else if (info.m_bFadeOut){
		double d = (double)info.m_Timer.Get() / info.m_dwWholeTime;
		LONG l = d*DSBVOLUME_MIN;
		if (l<DSBVOLUME_MIN) { l=DSBVOLUME_MIN; info.m_bFadeOut = false; }
		SetVolume(l);
	} else {
		bReset=true;
	}

	//	���ׂẴt�F�[�h���I����Ă��邩�H
	if (bReset) Stop();	//	�^�C�}�[�̒�~
}

bool CSoundFader::IsFade(void) const {
	return m_FadeInfo.m_bFadeIn || m_FadeInfo.m_bFadeOut;
}

void CSoundFader::Set(CSoundBase *lpSound)
{
	Stop();	//	�t�F�[�h�����ƍ���悤�ȁH
	m_lpSound = lpSound;
}