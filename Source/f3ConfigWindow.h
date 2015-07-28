// f3ConfigWindow.h: Cf3ConfigWindow クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3CONFIGWINDOW_H__59E17803_3DEE_41D0_A8D7_7DF8CEEBB810__INCLUDED_)
#define AFX_F3CONFIGWINDOW_H__59E17803_3DEE_41D0_A8D7_7DF8CEEBB810__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "yaneSDK/yaneAppBase.h"

class Cf3ConfigWindow : public CAppBase  
{
public:
	LRESULT OnPreCreate(CWindowOption &opt);
	void Open();
	Cf3ConfigWindow();
	virtual ~Cf3ConfigWindow();
};

#endif // !defined(AFX_F3CONFIGWINDOW_H__59E17803_3DEE_41D0_A8D7_7DF8CEEBB810__INCLUDED_)
