#ifdef USE_MIDIOutputMCI
// yaneMIDIOutputMCI.h
//
//		MIDI�o�͂��AMCI���g���čs�Ȃ�������B

#ifndef __yaneMIDIOutputMCI_h__
#define __yaneMIDIOutputMCI_h__

#include "yaneSoundBase.h"
#include "yaneFile.h"
#include "yaneWinHook.h"

class CMIDIOutputMCI : public CSoundBase, public CWinHook {
public:
	CMIDIOutputMCI(void);
	virtual ~CMIDIOutputMCI();

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
	virtual LONG	SetCurrentPos(LONG lPos);
	virtual LONG	GetLength(void);	// [ms]
	virtual LRESULT SetVolume(LONG volume) {return -1;}	// not implement
	virtual LONG	GetVolume(void) {return 0;}			// not implement

protected:
	LRESULT	InnerOpen(void);	//	MCI���I�[�v������

	CFile	m_File;
	DWORD	m_dwPosition;
	DWORD	m_wDeviceID;		//	WORD�Ƃ��Ďg���M�~�b�N
	int		m_bPaused;
	LONG	m_lLength;

	LRESULT	LoopPlay(void);	//	LoopPlay�p
	bool	m_bLoopPlay;	//	Loop�Đ�����̂��H
	bool	m_bNowPlay;		//	���ݍĐ�����

	// override from CWinHook
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
};

#endif

#endif // USE_MIDIOutputMCI