// yaneIME.h : easy ime wrapper
//		programmed by yaneurao(M.Isozaki) '00/02/25

#ifndef __yaneIMEBase_h__
#define __yaneIMEBase_h__

#include <imm.h>	// for ImmAssociateContext
#pragma comment(lib,"imm32.lib")

class CIMEBase {
public:
static void Show(void);
static void Hide(void);

protected:
	static HIMC m_hIME;

	static int m_nStat;
	//	現在の状態 0:不定 1:Show 2:Hide
};

#endif