// yaneSound.h
//	 This is DirectSoundBuffer wrapper.
//		programmed by yaneurao(M.Isozaki) '99/07/07
//		reprogrammed by yaneurao '00/03/04

#ifndef __yaneSound_h__
#define __yaneSound_h__

class CSound;
typedef set<CSound*> CSoundPtrArray;		// �`�F�C��

#include "yaneDirectSound.h"
#include "yaneSoundBase.h"
#include "enraPCMReaderFactory.h"

//	���̃N���X�́A���C���E�B���h�D��ۗL����X���b�h����
//	�쐬�^���ł����邱�ƁI

class CSound : public CSoundBase {
public:
	//	CSoundBase member function..
	virtual LRESULT Open(LPCTSTR pFileName) { return Load(pFileName); }
	virtual LRESULT Close(void)	{ return Release(); }
	virtual LRESULT Play(void);			// �Đ�
			LRESULT Play(DWORD);		// �Đ��i�w��ʒu����j
	virtual LRESULT Replay(void);		// pause�Ŏ~�߂Ă����Đ����ĊJ�B
	virtual LRESULT Stop(void);			// ��~
	virtual LRESULT Pause(void);		// �Đ���pause
	virtual bool	IsPlay(void);	// �Đ����Ȃ�true
	virtual bool	IsLoopPlay(void) { return m_bLoop; }// �ǉ�
	virtual LRESULT	SetLoopMode(bool bLoop); // ���[�v�ōĐ����邩
	virtual LONG	GetCurrentPos(void) { return GetPosition(); }
	virtual LRESULT SetCurrentPos(LONG lPos);	// [ms]
	virtual LONG	GetLength(void) { return m_lLength;}
	virtual LRESULT SetVolume(LONG volume); // ���̃`�����l���ɑ΂��郔�H�����[��
	virtual LONG	GetVolume(void);	// �擾�́A����`�����l���ɑ΂��Ă����ł��Ȃ�...

	// CSound
	virtual LRESULT Load(string filename);
	virtual LRESULT Release(void);			// Wave�̉��
	virtual LRESULT Restore(void);			// �ێ����Ă���t�@�C�����ɏ]���ǂݒ���

	virtual LONG	GetFrequency(void);			//	�Đ����g���̐ݒ�
	virtual LRESULT SetFrequency( DWORD freq );	//	�Đ����g���̎擾

	LONG	GetPosition(void);			//	���݂̍Đ��ӏ��擾[ms]
	LONG	GetPos(void);				//	���݂̍Đ��|�W�V�����擾

	//////////////////////////////////////////////////////////////////

	static	LRESULT	SetFormat(int nType);	//	�v���C�}���T�E���h�o�b�t�@�̎��g���ύX

	CSound(void);
	virtual ~CSound();

	//////////////////////////////////////////////////////////////////////////

	// ��������CSound�C���X�^���X���ׂĂɃA�N�Z�X���邽�߂̃`�F�C��
	static CSoundPtrArray	m_lpaSound;

	static LRESULT ReleaseAll(void);			// �S�C���X�^���X��Wave�̉��	
	static LRESULT RestoreAll(void);			// �S�C���X�^���X��Reload
	static LRESULT SetVolumeAll(LONG volume);	// �S�`�����l���ɑ΂��郔�H�����[��
	static void	StopAll(void);					// �S�C���X�^���X��Stop
	static void	PauseAll(void);					// �S�C���X�^���X�̍Đ�pause
	static void	ReplayAll(void);				// �S�C���X�^���Xpause�Ŏ~�߂Ă����Đ����ĊJ

	//////////////////////////////////////////////////////////////////////////

//	virtual LRESULT Play(void)	{ return Play(); }
//	virtual LRESULT Replay(void){ return Replay(); }
//	virtual LRESULT Stop(void)	{ return Stop(); }
//	virtual LRESULT Pause(void)	{ return Pause(); }
//	virtual bool IsPlay(void)	{ return IsPlay(); }
//	virtual LRESULT SetLoopMode(bool bLoop) { return SetLoopMode(bLoop); }

	static	LPDIRECTSOUND GetDirectSound(void) {
		return CDirectSound::GetCDirectSound()->m_lpDirectSound;
	}
	static	CDirectSound* GetCDirectSound(void) {
		return CDirectSound::GetCDirectSound();
	}

	//--- �ǉ� '02/01/10  by enra ---
	virtual void SetReaderFactory(smart_ptr<CPCMReaderFactory> p)
	{
		m_lpReaderFactory = p;
	}
	virtual smart_ptr<CPCMReaderFactory> GetReaderFactory(void)
	{
		return m_lpReaderFactory;
	}
	//-------------------------------

protected:
	LPDIRECTSOUNDBUFFER m_lpDSBuffer;			//	�Z�J���_���o�b�t�@
	string	m_szFileName;						//	WAVE�t�@�C���̖��O
	int		m_bPaused;							//	�T�E���hpause�����H
	bool	m_bLoop;							//	���[�v���[�h�ōĐ�����̂��H
	DWORD	m_nAvgBytesPerSec;					//	�b�Ԃ̃f�[�^�o�C�g���i�Đ��ʒu�Z�o�p�j
	LONG	m_lLength;
	//	CDirectSound�͕K�v�ɂȂ������_�œ��I�ɐ�������i����͂��Ȃ��j
	//	�����Am_lpCDirectSound!=NULL�ŁAm_lpCDirectSound->m_lpDirectSound==NULL�ł���΁A
	//	DirectSound��COM�C���^�[�t�F�[�X�����Ɏ��s�������T�E���h�J�[�h�������ł���Ƃ���
	//	���Ƃ��Ӗ�����B

	//--- �ǉ� '02/01/10  by enra ---
	smart_ptr<CPCMReaderFactory> m_lpReaderFactory;
	//-------------------------------
};

#endif