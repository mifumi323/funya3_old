// BGMUser.h: CBGMUser クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BGMUSER_H__EA960D76_A7C6_43E7_BB78_A39806725D54__INCLUDED_)
#define AFX_BGMUSER_H__EA960D76_A7C6_43E7_BB78_A39806725D54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BGMBase.h"

class CSoundBase;

class CBGMUser : public CBGMBase  
{
protected:
	CSoundBase*	m_Sound[BGMN_SIZE];
public:
	string GetClassName();
	void InnerStop();
	void InnerPlay(BGMNumber no);
	bool Init();
	CBGMUser();
	virtual ~CBGMUser();

};

#endif // !defined(AFX_BGMUSER_H__EA960D76_A7C6_43E7_BB78_A39806725D54__INCLUDED_)
