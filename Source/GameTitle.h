// CGameTitle

#if !defined(__GAMETITLE_H__)
#define __GAMETITLE_H__

#include "f3Scene.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGameTitle : public Cf3Scene{
public:
	void Run();
	CGameTitle();
	virtual ~CGameTitle();
};

#endif