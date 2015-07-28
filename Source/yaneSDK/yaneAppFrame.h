//	yaneAppFrame.h :
//			programmed by yaneurao	'00/06/16

#ifndef __yaneAppFrame_h__
#define __yaneAppFrame_h__

#include "yaneAppBase.h"
#include "yaneAppManager.h"

class CAppFrame {
public:
	void	Start(void) { m_lpMyApp = CAppManager::GetMyApp(); MainThread();}
	void	MesSleep(int nTime);	//	���b�Z�[�W���������Ȃ���܂��

	//	�X���b�h�̖�����
	void	InvalidateThread(void) { GetMyApp()->InvalidateThread(); }

	//	�Ăяo�����̃A�v���N���X�̎擾
	CAppBase* GetMyApp(void)const { return m_lpMyApp; }

	//	���ꂪ�^�̊ԁA�X���b�h���܂킵�ĂˁI
	bool IsThreadValid(void)const { return GetMyApp()->IsThreadValid(); }

	//	�R���X�g���N�^�`�f�X�g���N�^�ŁACAppManager�ɓo�^���Ă���
	CAppFrame(void);
	virtual ~CAppFrame();

protected:
	//	����̓��[�U�[���p�ӂ���
	virtual void MainThread(void) = 0;

private:
	CAppBase* m_lpMyApp;
};

#endif