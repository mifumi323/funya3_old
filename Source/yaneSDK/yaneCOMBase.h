// yaneCOM.h :
//	initialize/terminate COM
//
//	COM���g�p����ŏ��ƍŌ�ŏ������^�I���������K�v�Ȃ̂ŁA
// ����Ȃ��񂪕K�v�ɂȂ��Ă���...

#ifndef __yaneCOMBase_h__
#define __yaneCOMBase_h__

#include "yaneCriticalSection.h"

class CCOMBase {
public:
	static LRESULT AddRef(void);	// COM�̎Q�ƃJ�E���g�̉��Z
	static void Release(void);		// COM�̎Q�ƃJ�E���g�̌��Z

protected:
	static	CCriticalSection m_oCriticalSection;
	static  map<DWORD,int> m_vCount;
	//		mapping from GetCurrentThreadId to RefCount

};

#endif