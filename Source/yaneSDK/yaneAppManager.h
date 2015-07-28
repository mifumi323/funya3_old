//	yaneAppManager.h :
//		application management class
//			programmed by yaneurao	'00/03/03
//
//		'00/08/04	sohei	Saver�֌W��ǉ�

#ifndef __yaneAppManager_h__
#define __yaneAppManager_h__

#include "yaneCriticalSection.h"
#include "yaneAppBase.h"

//---	Drawing Units
#include "yaneDirectDraw.h"
#include "yaneDIBDraw.h"

class CAppFrame;

struct CAppManageInfo {
	CAppBase*		m_lpAppBase;		//	�ЂƂ̃A�v���N���X������
#ifdef USE_DirectDraw
	CDirectDraw*	m_lpDirectDraw;		//	�ЂƂ̕`��N���X������
#endif
#ifdef USE_DIB32
	CDIBDraw*		m_lpDIBDraw;		//	��DIB�̏ꍇ�́A������
#endif
#ifdef USE_SAVER
	CSaverDraw*		m_lpSaverDraw;		//	�X�N���[���Z�[�o�[�p
#endif
	CAppFrame*		m_lpAppFrame;		//	�������������̃t���[���N���X������
	DWORD			m_dwThreadID1;		//	���C���X���b�h��ThreadID
//	DWORD			m_dwThreadID2;		//	���b�Z�[�W���[�v��ThreadID
};	//	�}���`�X���b�h��߂܂����O�O

typedef auto_vector_ptr<CAppManageInfo> CAppManageList;

class CAppManager {
public:
	static	void	Add(CAppBase*);
	static	void	Del(CAppBase*);
	static	void	Add(CAppFrame*);
	static	void	Del(CAppFrame*);
#ifdef USE_DirectDraw
	static	void	Add(CDirectDraw*);
	static	void	Del(CDirectDraw*);
	static	CDirectDraw* GetMyDirectDraw();
#endif
#ifdef USE_FastDraw
	static	void	Add(CFastDraw*);
	static	void	Del(CFastDraw*);
	static	CFastDraw* GetMyFastDraw();
#endif
#ifdef USE_DIB32
	static	void	Add(CDIBDraw*);
	static	void	Del(CDIBDraw*);
	static	CDIBDraw*	 GetMyDIBDraw();
#endif
#ifdef USE_SAVER
	static	void	Add(CSaverDraw*);
	static	void	Del(CSaverDraw*);
	static	CSaverDraw*	 GetMySaverDraw();
#endif
	static	CAppBase*	 GetMyApp();
	static	CAppFrame*	 GetMyFrame();
	static	bool	IsDirectDraw();
	static	int		GetDrawType();

	static	int		GetAppInstanceNum();
	static	void	StopAllThread();

	static	void Inc();	//	�Q�ƃJ�E���g�̃C���N�������g
	static	void Dec();	//	�Q�ƃJ�E���g�̃f�N�������g
	static	int GetRef();

protected:
// volatile���Ȃ̂���...
	static	CAppManageList m_alpInfo;
	static	CCriticalSection m_oCriticalSection;
	static	int m_nRef;	//	�Q�ƃJ�E���g
};

#endif