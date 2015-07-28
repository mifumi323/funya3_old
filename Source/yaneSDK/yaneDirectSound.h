// yaneDirectSound.h
//	 This is DirectSound wrapper.
//		programmed by yaneurao(M.Isozaki) '99/06/18
//				modified by yaneurao	  '00/03/02

#ifndef __yaneDirectSound_h__
#define __yaneDirectSound_h__

#include "yaneWinHook.h"

namespace mtknLib { class kmStreamSoundHelper;};

class CDirectSound : public CWinHook {
public:
	LRESULT	SetFormat(int nType);	//	�v���C�}���T�E���h�o�b�t�@�̎��g���ύX

	CDirectSound(void);
	virtual ~CDirectSound();

	friend class CSound;
	friend class mtknLib::kmStreamSoundHelper;
	friend class CMovieAVIAUDIO;

	//	�����ŎQ�ƃJ�E���g���Ƃ�
	static void AddRef(void) { if (m_nRef++==0) m_lpCDirectSound.Add(); }
	static void DelRef(void) { if (--m_nRef==0) m_lpCDirectSound.Delete(); }
	//	AddRef�������ƂȂ�Έȉ��̊֐����L��
	static CDirectSound* GetCDirectSound(void){ return m_lpCDirectSound; }
	static LPDIRECTSOUND GetDirectSound(void) { return m_lpCDirectSound->m_lpDirectSound; }

protected:

	LRESULT Initialize(void);
	LRESULT Terminate(void);

	LPDIRECTSOUND	m_lpDirectSound;
	LPDIRECTSOUNDBUFFER	m_lpPrimary;
	int				m_nFormat;		//	�v���C�}���T�E���h�o�b�t�@�̎��g��

	HWND			m_hWnd;			//	�ۗL���Ă���E�B���h�D�n���h��
	bool			m_bSuccessInit;	//	�������ɐ���������

	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	//	override from CWinHook
	LRESULT CheckSoundLost(void);	// ���X�g���Ă���C������
	//	�������A�Đ��|�C���g�܂ł͏C���ł��Ȃ��i�d�����Ȃ��j�̂Œ��ӂ��K�v
	//	���̃`�F�b�N��WM_ACTIVATEAPP�ɉ����ČĂяo���I

	static int		m_nRef;
	static auto_ptrEx<CDirectSound> m_lpCDirectSound;
};

#endif