// f3Scene.h: Cf3Scene �N���X�̃C���^�[�t�F�C�X

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
	void*	m_status;			// �Ȃ�ɂł��g����(�g�����Ȃ����NULL)
	void*	m_ret;				// �Ȃ�ɂł��g����(�߂�l��p)
};

#endif