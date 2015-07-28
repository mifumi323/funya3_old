//	�f�o�b�O��ʖ�[
//		original program was programmed by Roual '00/05/01 ( for Yobai Mania )
//		remade by yaneurao '00/12/01 ( for Happy Hotaru Sou )

#ifndef __yaneDebugger_h__
#define __yaneDebugger_h__

#include "yaneLayer.h"

class CMouseEx;
class CFlagDebugger : public CHDCLayer {
public:
	//	����A�ŏ��ɐݒ肵�ĂˁI
	void	SetMouse(CMouseEx*lp) { m_lpMouse = lp; }
	void	SetFlag(DWORD*lpdw,int nSize) { m_lpdw = lpdw; m_nSize = nSize; }

	//	����́AHook����Ύ����I�ɃR�[���o�b�N�����B
	void	OnDraw(HDC hdc);

	CFlagDebugger(void);
	virtual ~CFlagDebugger();

protected:
	CMouseEx* GetMouse(void) { return m_lpMouse; }
	void	ShowGameFlagList(HDC hdc);

private:
	CMouseEx* 	m_lpMouse;	//	�e��CMouseEx
	DWORD*		m_lpdw;		//	�\�����ׂ�DWORD�z��
	int			m_nSize;	//	�z��T�C�Y
	int 		m_nGameFlagOffset;		//	���݂̕\������Ă��鍶��̃Q�[���t���O
	int			m_nSelectedGameFlag;	//	���ݑI������Ă���Q�[���t���O
};

#endif