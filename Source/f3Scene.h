// f3Scene.h: Cf3Scene クラスのインターフェイス

#if !defined(__F3SCENE_H__)
#define __F3SCENE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "App.h"

class Cf3Scene {
public:
	void Init(CApp* app, int* nextscene, void* status,	void* ret);
	Cf3Scene();
	virtual ~Cf3Scene();

	virtual void Run() = 0;

protected:
	CApp*	m_app;
	int*	m_nextscene;
	void*	m_status;			// なんにでも使おう(使い道なければNULL)
	void*	m_ret;				// なんにでも使おう(戻り値専用)
};

#endif