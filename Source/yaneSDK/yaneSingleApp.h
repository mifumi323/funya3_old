#ifndef __yaneSingleApp_h__
#define __yaneSingleApp_h__

#include "yaneMutex.h"

class CSingleApp {
public:
	CSingleApp(void);

	bool	IsValid(void);	//�@���Ɏ����Ɠ����A�v�����N�����Ă��Ȃ��������H

protected:
	CMutex	m_oMutex;
	bool	m_bValid;
};

#endif