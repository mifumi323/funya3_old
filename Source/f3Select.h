// f3Select.h: Cf3Select クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3SELECT_H__69AD766D_F75A_4D13_A920_B9217F4FF6A9__INCLUDED_)
#define AFX_F3SELECT_H__69AD766D_F75A_4D13_A920_B9217F4FF6A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "YANESDK\yaneLayer.h"

class CTextDIB32;
class CDIB32;

class Cf3Select : public CLayer  
{
protected:
	string		m_Menu;
	int			m_Selected;
	int			m_Prev;
	int			m_Lines;
	CTextDIB32	*m_Text;
	CDIB32		*m_Cursor;
	vector<int>	m_IDs;
public:
	void Add(string item, int id);
	void Add(string item) { Add(item, m_Lines); }
	bool Changed() { return m_Prev!=m_Selected; }
	void Select(int num);
	void GetSize(int &sx,int &sy);
	int GetSelected() { return m_IDs[m_Selected]; }
	void Clear();
	void InnerOnDraw(CPlaneBase*lp);
	Cf3Select();
	virtual ~Cf3Select();

};

#endif // !defined(AFX_F3SELECT_H__69AD766D_F75A_4D13_A920_B9217F4FF6A9__INCLUDED_)
