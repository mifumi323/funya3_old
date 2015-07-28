// f3Select.cpp: Cf3Select クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3Select.h"
#include "f3Input.h"
#include "ResourceManager.h"
#include "yaneSDK/yaneTextDIB32.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3Select::Cf3Select()
{
	m_Text = new CTextDIB32;
	m_Text->GetFont()->SetSize(32);
	m_Text->GetFont()->SetColor(0xffffff);
	m_Text->GetFont()->SetBackColor(0x303030);
	m_Cursor = ResourceManager.Get(RID_MAIN);
	m_CursorY = 0;
	Clear();
}

Cf3Select::~Cf3Select()
{
	delete m_Text;
}

void Cf3Select::InnerOnDraw(CPlaneBase *lp)
{
	if (m_Lines) {
		m_Prev = m_Selected;
		if (f3Input.GetKeyPushed(F3KEY_UP)) {
			if (--m_Selected<0) m_Selected += m_Lines;
		}
		if (f3Input.GetKeyPushed(F3KEY_DOWN)) {
			if (++m_Selected>=m_Lines) m_Selected -= m_Lines;
		}
		lp->Blt(m_Text,m_nX, m_nY);
		RECT rc = { 288, 96, 320, 128 };
		//BringClose(m_CursorY, 32*m_Selected, 8);
		m_CursorY = (m_CursorY+32*m_Selected)/2;
		lp->Blt(m_Cursor,m_nX-32, m_nY+m_CursorY,&rc);
	}
}

void Cf3Select::Clear()
{
	m_Menu = "";
	m_Text->GetFont()->SetText(m_Menu);
	m_Text->UpdateText();
	m_Selected = m_Prev = m_Lines = 0;
	m_IDs.clear();
}

void Cf3Select::GetSize(int &sx, int &sy)
{
	if (m_Lines) {
		m_Text->GetSize(sx,sy);
	}else {
		sx=0; sy=0;
	}
}

void Cf3Select::SelectPos(int num)
{
	if (num<m_Lines) {
		m_Selected = num;
	}else {
		m_Selected = m_Lines-1;
	}
}

void Cf3Select::Add(string item, int id)
{
	m_Menu += item+"\n";
	m_Text->GetFont()->SetText(m_Menu);
	m_Text->UpdateText();
	m_IDs.push_back(id);
	m_Lines++;
}
