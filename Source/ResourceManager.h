// ResourceManager.h: CResourceManager クラスのインターフェイス
// よく使う画像を先読みしておく

#if !defined(AFX_RESOURCEMANAGER_H__D4DFD558_6D0E_4489_80C7_7858C533DD87__INCLUDED_)
#define AFX_RESOURCEMANAGER_H__D4DFD558_6D0E_4489_80C7_7858C533DD87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "yaneSDK/yaneDIB32.h"


enum {
	RID_MAIN,
	RID_MAINICY,
	RID_NEEDLE,
	RID_GEASPRIN,
	RID_EELPITCHER,
	RID_ICEFIRE,
	RID_MRFRAME,
	RID_EFFECT,
	RID_NAVI,
	RID_HIT,

	RID_END
};

extern class CResourceManager  
{
protected:
	CDIB32* m_Resource;
public:
	void Quit() { DELETEPTR_SAFE(m_Resource); }
	void Init();
	CResourceManager() { m_Resource = NULL; }
	virtual ~CResourceManager() { Quit(); }

	CDIB32* Get(int i) { return m_Resource+i; }

} ResourceManager;

#endif // !defined(AFX_RESOURCEMANAGER_H__D4DFD558_6D0E_4489_80C7_7858C533DD87__INCLUDED_)
