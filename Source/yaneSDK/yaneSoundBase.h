// yaneSoundBase.h
//
//		MIDI,WAVE�o�͊��N���X

#ifndef __yaneSoundBase_h__
#define __yaneSoundBase_h__

/////////////////////////////////////////////////////////////////////////////
class CSoundBase {
public:
	CSoundBase(void) {}
	virtual ~CSoundBase() {}

	virtual LRESULT Open(LPCTSTR pFileName) = 0;
	virtual LRESULT Close(void) = 0;
	virtual LRESULT Play(void)	= 0;
	virtual LRESULT Replay(void) = 0;
	virtual LRESULT Stop(void)	= 0;
	virtual LRESULT Pause(void) = 0;
	virtual bool IsPlay(void) = 0;
	virtual bool IsLoopPlay(void) = 0;// �ǉ�
	virtual LRESULT SetLoopMode(bool bLoop) = 0;
	virtual LONG	GetCurrentPos(void) = 0;
	virtual LRESULT	SetCurrentPos(LONG lPos) = 0;	// [ms]
	virtual LONG	GetLength(void) = 0; // �Ȓ��𓾂� [ms]
	virtual LRESULT SetVolume(LONG volume)=0; // ���̃`�����l���ɑ΂��郔�H�����[��
	virtual LONG	GetVolume(void)=0;	// �擾�́A����`�����l���ɑ΂��Ă����ł��Ȃ�...

	virtual LRESULT ConvertToMealTime(LONG nPos,int&nHour,int&nMin,int&nSec,int&nMS);
	virtual LRESULT ConvertFromMealTime(LONG&nPos,int nHour,int nMin,int nSec,int nMS);
};

//	���Ɍ���NULL DEVICE�B
//	MIDI�̕t���Ă��Ȃ����ł́A����ł��A�b�v�L���X�g���Ďg���Ηǂ�
class CSoundNullDevice : public CSoundBase {
public:
	virtual LRESULT Open(LPCTSTR pFileName) { return 0; }
	virtual LRESULT Close(void)	 { return 0; }
	virtual LRESULT Play(void)	 { return 0; }
	virtual LRESULT Replay(void) { return 0; }
	virtual LRESULT Stop(void)	 { return 0; }
	virtual LRESULT Pause(void)	 { return 0; }
	virtual bool IsPlay(void)	 { return false; }
	virtual bool IsLoopPlay(void){ return m_bLoop; }
	virtual LRESULT SetLoopMode(bool bLoop) { m_bLoop=bLoop; return 0; }
	virtual LONG	GetCurrentPos(void) { return -1; }
	virtual LRESULT	SetCurrentPos(LONG lPos) { return -1;}
	virtual LONG	GetLength(void){ return -1;}
	virtual LRESULT SetVolume(LONG volume){ return -1;}
	virtual LONG	GetVolume(void){ return DSBVOLUME_MIN;}

	CSoundNullDevice(void) { m_bLoop = false; }

protected:
	bool m_bLoop;
};

#endif