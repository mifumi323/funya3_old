#ifdef USE_MIDIOutputDM
// yaneMIDIOutputDM.h
//
//		MIDI�o�͂��ADirectMusic���g���čs�Ȃ�������B
//
//		DirectMusic���g���ɂ́ADirectX6�ȍ~���K�v�Ȃ̂Œ��ӁI

#ifndef __yaneMIDIOutputDM_h__
#define __yaneMIDIOutputDM_h__

#include "yaneSoundBase.h"
#include "yaneFile.h"

#include <dmusicc.h>	//	directX6 �ȍ~��SDK���K�v�ɂ�
#include <dmusici.h>

//	DirectMusic wrapper
class CDirectMusic {
public:
	CDirectMusic(void);
	virtual ~CDirectMusic();

	// CDirectSound�Ɠ������A�Q�ƃJ�E���g�����ŊǗ�����
	//	�����ŎQ�ƃJ�E���g���Ƃ�
	static void AddRef(void) { if (m_nRef++==0) m_lpCDirectMusic.Add(); }
	static void DelRef(void) { if (--m_nRef==0){ m_lpCDirectMusic.Delete(); m_bFirst = true;}}
	//	AddRef�������ƂȂ�Έȉ��̊֐����L��
	static CDirectMusic* GetCDirectMusic(void){ return m_lpCDirectMusic; }

	bool	CanUseDirectMusic(void) const;	//	DirectMusic���g���邩��Ԃ�

//	IDirectMusic* GetDirectMusic(void) { return m_lpDMusic; }
	//	�����������ۂɎg�����Ƃ͗L�蓾�Ȃ��i�Ǝv���j

	IDirectMusicPerformance* GetDirectMusicPerformance(void) { return m_lpDMPerf; }
	IDirectMusicLoader* GetDirectMusicLoader(void) { return m_lpDMLoader; }

private:
	static bool	m_bFirst;
	static bool m_bCanUseDirectMusic;

	IDirectMusicPerformance* m_lpDMPerf;
	IDirectMusic* m_lpDMusic;
	IDirectMusicLoader* m_lpDMLoader;

	static int		m_nRef;
	static auto_ptrEx<CDirectMusic> m_lpCDirectMusic;
};

class CMIDIOutputDM : public CSoundBase {
public:
	CMIDIOutputDM(void);
	virtual ~CMIDIOutputDM();

	bool	CanUseDirectMusic(void){
		//	DirectMusic���g���邩��Ԃ�
		return GetDirectMusic()->CanUseDirectMusic();
	}

	//	override from CSoundBase
	virtual LRESULT Open(LPCTSTR pFileName);
	virtual LRESULT Close(void);
	virtual LRESULT Play(void);
	virtual LRESULT Replay(void);
	virtual LRESULT Stop(void);
	virtual LRESULT Pause(void){ return Stop(); }
	virtual bool IsPlay(void);
	virtual bool IsLoopPlay(void){ return m_bLoopPlay; }// �ǉ�
	virtual LRESULT SetLoopMode(bool bLoop);
	virtual LONG	GetCurrentPos(void);
	virtual LRESULT	SetCurrentPos(LONG lPos);
	virtual LONG	GetLength(void);
	virtual LRESULT SetVolume(LONG volume); 
	virtual LONG	GetVolume(void);

protected:
	LRESULT	InnerOpen(void);	//	DirectMusic���I�[�v������

	CFile	m_File;
	MUSIC_TIME m_mtPosition;	//	Pause����
	int		m_bPaused;

	LRESULT	LoopPlay(void);	//	LoopPlay�p
	bool	m_bLoopPlay;	//	Loop�Đ�����̂��H
	LONG	m_lLength;

	//	�Đ�����Z�O�����g
	IDirectMusicSegment* m_lpDMSegment;
	//	�Đ��Z�O�����g�̃X�e�[�^�X
	IDirectMusicSegmentState* m_lpDMSegmentState;
	CDirectMusic* GetDirectMusic(void) { return CDirectMusic::GetCDirectMusic(); }

	//	�������Đ��́A�����A�h���X���w�肷��ƃL���b�V�����L���ɂȂ��Ă��܂��āA
	//	���̋Ȃ��Đ�����Ȃ��̂ł��̑΍�(thanks > DEARNA)
	//	auto_array<BYTE> m_alpMIDICache;
};

#endif

#endif // USE_MIDIOutputDM