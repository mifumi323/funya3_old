//	yaneSoundFader.h :
//		programmed by yaneurao '00/08/20

#ifndef __yaneSoundFader_h__
#define __yaneSoundFader_h__

#include "yaneIntervalTimer.h"
#include "yaneTimer.h"
#include "yaneSound.h"

class CSoundFader : public CIntervalTimer {
public:
	void	Set(CSoundBase*lpSound=NULL);			//	�ΏۂƂ���CSound��ݒ肷��
	//	lpSound==NULL�Ȃ�΁A���ׂĂ�CSound�N���X���ΏۂƂȂ�B

	LRESULT ResetVolume(void);					//	�����{�����[���l�ɖ߂�
	LRESULT FadeOut(int dwTime);				//	�t�F�[�h�A�E�g���J�n������
	LRESULT FadeIn(int dwTime);					//	�t�F�[�h�C�����J�n������
	// dwTime[ms]��Ƀ{�����[���̃t�F�[�h�͊�������

	bool	IsFade(void)const;					//	�t�F�[�h���Ȃ̂��H

	CSoundFader(void);
	virtual ~CSoundFader();

protected:
	class CVolumeFadeInfo {
	public:
		CTimer		m_Timer;		//	Fade�̂��߂�Timer
		DWORD		m_dwWholeTime;	//	Fade���J�n����I��������܂ł̎���
		bool		m_bFadeIn;		//	FadeIn���Ȃ̂�?
		bool		m_bFadeOut;		//	FadeOut���Ȃ̂�?

		CVolumeFadeInfo(void) {
			m_bFadeIn = m_bFadeOut = false;
		}
	} m_FadeInfo;

	CSoundBase*		m_lpSound;			//	Fade�����T�E���h

	void	SetVolume(DWORD dw) {
		if (m_lpSound!=NULL) {
			m_lpSound->SetVolume(dw);
		} else {
			//	CSound�����Ƃ͌���Ȃ��̂���...
			CSound::SetVolumeAll(dw);
		}
	}

	virtual void TimerProc(void);	//	�R�[���o�b�N�����֐�
};

#endif