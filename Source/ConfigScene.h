// ConfigScene.h: CConfigScene クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGSCENE_H__F2EA6CCC_17C6_42DE_B89F_CF0F553E7816__INCLUDED_)
#define AFX_CONFIGSCENE_H__F2EA6CCC_17C6_42DE_B89F_CF0F553E7816__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SceneBase.h"
#include "yaneSDK/yaneWinHook.h"

class CConfigScene : public CSceneBase, public CWinHook  
{
protected:
	HWND AddCheckBox(LPCSTR lpText, int x, int y, int w, int h, int nID, bool bChecked, bool bScroll=true);
	void AddFPS(HWND hWnd, LPCSTR desc, int nFPS);
	HWND Add(LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x=CW_USEDEFAULT, int y=CW_USEDEFAULT, int nWidth=CW_USEDEFAULT, int nHeight=CW_USEDEFAULT, int nID=0, bool bScroll=true);
	HWND AddAnchor(LPCSTR lpName, int y);
	HWND AddComboBox(int x, int y, int nWidth, int nID, bool bScroll, ...);

	struct Control
	{
		Control(){}
		Control(HWND _hWnd, bool _bScroll, int _x, int _y)
		{
			hWnd = _hWnd;
			bScroll = _bScroll;
			x = _x; y = _y;
		}
		HWND hWnd;
		bool bScroll;
		int x, y;
	};
	typedef map<int, Control> ControlMap;
	typedef pair<int, Control> ControlMapItem;
	typedef ControlMap::iterator ControlMapIterator;
	ControlMap m_Control;
	int m_nAutoID;
	HWND m_hScroll;

	int m_nFPS;
public:
	bool GetCheck(int nID);
	HWND AddComboBoxWithData(int x, int y, int nWidth, int nID, bool bScroll, ...);
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	void OnInit();
	CConfigScene();
	virtual ~CConfigScene();

};

#endif // !defined(AFX_CONFIGSCENE_H__F2EA6CCC_17C6_42DE_B89F_CF0F553E7816__INCLUDED_)
