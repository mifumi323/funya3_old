// yaneMouseDecorator.h
//	window base class
//		���ō���Ă݂����ACMouseEx�`CButton���g�����ق����ǂ����O�O�G

#ifndef __yaneMouseDecorator_h__
#define __yaneMouseDecorator_h__

#include "yaneMouse.h"

class CMouseDecorator {
public:
	virtual LRESULT OnGetInfoBefore(int x,int y,int b);
	//	�}�E�X���悬�����Ƃ��ɒʒm

	virtual LRESULT OnGetInfoAfter(int x,int y,int b);
	//	�{�^�����͂��ꂽ�Ƃ��ɂ́A��O��Ԃ��΁A���ȍ~��Decorator�͌Ăяo����Ȃ�
};

class CMouseDecoratorManager {
public:
	//	initialization
	void SetMouse(CMouse*lpMouse) { m_lpMouse = lpMouse; }
	chain<CMouseDecorator>* GetDecorator(void) { return& m_alpDecorators; }

	//	get info and dispatch mouse message..
	LRESULT	DispatchMouseMessage(void);
	//	dispatch message..
	LRESULT DispatchMouseMessage(int x,int y,int info);

protected:
	CMouse* m_lpMouse;
	chain<CMouseDecorator> m_alpDecorators;
};

#endif