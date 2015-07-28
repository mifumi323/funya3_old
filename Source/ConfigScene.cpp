// ConfigScene.cpp: CConfigScene クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigScene.h"
#include "App.h"
#include "yaneSDK/yaneAppInitializer.h"
#include <stdarg.h>

enum ControlID{
	CID_AUTO,	// 0は自動でIDを割り振る

	// スクロールなし
	CID_CB_CATEGORY,
	CID_BS_RETURN,
	CID_SB_SCROLL,

	CID_CB_BGMMODE,
	CID_BS_FULLSCREEN,
	CID_CB_BACKGROUND,
	CID_CB_VIEWTIME,
	CID_CB_FPS,
	CID_BS_ESP,	// 未実装
	CID_CB_GRAVITY,	// 未実装
	CID_CB_DRAWMETHOD,	// 未実装
	CID_BS_HYPER,	// 未実装

	CID_BASE_BGM=1000,	// 未実装
	CID_BASE_BGMOPEN=1100,	// 未実装
	CID_BASE_KEY=1200,	// 未実装
	CID_BASE_KEYRESET=1300,	// 未実装
	CID_BASE_CHARACTER=1400,	// 未実装

	CID_MAX,
	CID_BASE_AUTO=CID_MAX+99,
};

#define ANCHORHEIGHT	20
#define DEFWIDTH	(320-16)

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CConfigScene::CConfigScene()
{
	m_nAutoID = CID_BASE_AUTO;
	m_hScroll = NULL;
}

CConfigScene::~CConfigScene()
{
	theApp->GetMyApp()->GetMyWindow()->GetHookList()->Del(this);
	for (ControlMapIterator it=m_Control.begin(); it!=m_Control.end(); it++) {
		if (!(*it).second.bScroll) DestroyWindow((*it).second.hWnd);
	}
	theApp->SetAutoDraw(true);
}

void CConfigScene::OnInit()
{
	m_nFPS = theApp->GetFPS()->GetFPS();
	// 背景の設定
	theApp->SetAutoDraw(false);
	DWORD col=GetSysColor(COLOR_BTNFACE);
//	theApp->GetDraw()->Clear(((col&0xff)<<16)+(col&0xff00)+((col&0xff0000)>>16));
//	theApp->GetDraw()->OnDraw();
	// コントロールの配置
	int y=0;
	{
		int x=0, w=0, h=0, comboHeight=20;
		HWND hWnd;
		RECT rc;

		// カテゴリージャンプ
		hWnd = AddComboBox(0, y, 320-60, CID_CB_CATEGORY, false, NULL);
		GetWindowRect(hWnd, &rc);
		w = rc.right-rc.left; comboHeight = rc.bottom-rc.top;
		// 戻るボタン
		hWnd = Add("BUTTON", "戻る", BS_DEFPUSHBUTTON, w, y, 320-w, comboHeight, CID_BS_RETURN, false);
		// スクロール
		hWnd = Add("SCROLLBAR", NULL, SBS_VERT, DEFWIDTH, comboHeight, 320-DEFWIDTH, 240-comboHeight, CID_SB_SCROLL, false);
		m_hScroll = Add("STATIC", NULL, WS_CLIPCHILDREN, 0, comboHeight, DEFWIDTH, 240-comboHeight, CID_AUTO, false);

		hWnd = AddAnchor("動作", y); y += ANCHORHEIGHT;
		// 非アクティブ時の動作
		hWnd = AddComboBox(0, y, DEFWIDTH, CID_CB_BACKGROUND, true,
			"非アクティブ時：ポーズ",
			"非アクティブ時：操作を受け付けない",
			"非アクティブ時：操作可能",
			NULL);
		SendMessage(hWnd, CB_SETCURSEL, (WPARAM)theSetting->m_Background, 0);
		y += comboHeight;
		// FPS
		if (theSetting->m_TimeMaster) {
			hWnd = AddComboBox(0, y, DEFWIDTH, CID_CB_FPS, true, NULL);
			AddFPS(hWnd, "コマ送り",    2);
			AddFPS(hWnd, "すごく遅い", 10);
			AddFPS(hWnd, "遅い",       20);
			AddFPS(hWnd, "普通",       40);
			AddFPS(hWnd, "速い",       60);
			AddFPS(hWnd, "すごく速い", 80);
			AddFPS(hWnd, "最速",     1000);
			y += comboHeight;
		}

		hWnd = AddAnchor("表示", y); y += ANCHORHEIGHT;
		// フルスクリーン
		hWnd = AddCheckBox("フルスクリーン", 0, y, DEFWIDTH, 20, CID_BS_FULLSCREEN, theSetting->m_FullScreen!=0);
		y += 20;
		// 時間表示
		hWnd = AddComboBox(0, y, DEFWIDTH, CID_CB_VIEWTIME, true, "時間表示：なし", "時間表示：フレーム数", "時間表示：秒数まで", "時間表示：ミリ秒まで", NULL);
		SendMessage(hWnd, CB_SETCURSEL, (WPARAM)theSetting->m_ViewTime, 0);
		y += comboHeight;

		hWnd = AddAnchor("操作", y); y += ANCHORHEIGHT;

		hWnd = AddAnchor("BGM", y); y += ANCHORHEIGHT;
		// BGMモード
		hWnd = AddComboBox(0, y, DEFWIDTH, CID_CB_BGMMODE, true, "BGM:鳴らさない", "BGM:標準", "BGM:ユーザー指定", NULL);
		SendMessage(hWnd, CB_SETCURSEL, (WPARAM)theSetting->m_BGMMode, 0);
		y += comboHeight;

		hWnd = AddAnchor("リプレイ", y); y += ANCHORHEIGHT;
	}
	// コンボボックス調整
	SendMessage(m_Control[CID_CB_CATEGORY].hWnd, CB_SETCURSEL, 0, 0);
	SetWindowPos(m_Control[CID_CB_CATEGORY].hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetWindowPos(m_Control[CID_BS_RETURN].hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	// スクロールバー調整
	SCROLLINFO si = {
		sizeof(SCROLLINFO),
		SIF_ALL, 
		0, y,
		100, 0, 0
	};
	SetScrollInfo(m_Control[CID_SB_SCROLL].hWnd, SB_CTL, &si, false);
	for (ControlMapIterator it=m_Control.begin(); it!=m_Control.end(); it++)
		ShowWindow((*it).second.hWnd, SW_SHOW);
	theApp->GetMyApp()->GetMyWindow()->GetHookList()->Add(this);
	theApp->GetBGM()->Stop();
	theApp->GetFPS()->SetFPS(60);
}

LRESULT CConfigScene::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID = LOWORD(wParam);
	HWND hCtl = (HWND)lParam;
	switch(uMsg) {
	case WM_COMMAND: {
		switch(nID) {
		case CID_BS_RETURN:
			// 戻るボタン(ほとんどの項目はここで反映する)
			theApp->ChangeBGM((BGMMode)(theSetting->m_BGMMode=
				SendMessage(m_Control[CID_CB_BGMMODE].hWnd, CB_GETCURSEL , 0 , 0)
				));
			theApp->SetFullScreen((theSetting->m_FullScreen=(int)GetCheck(CID_BS_FULLSCREEN))!=0);
			theSetting->m_Background = SendMessage(m_Control[CID_CB_BACKGROUND].hWnd, CB_GETCURSEL , 0 , 0);
			if (theSetting->m_TimeMaster) theSetting->m_ViewTime=SendMessage(m_Control[CID_CB_VIEWTIME].hWnd, CB_GETCURSEL , 0 , 0);
			theApp->GetFPS()->SetFPS(m_nFPS);
			JumpScene(FIRST_SCENE);
			return 1;
		case CID_CB_CATEGORY: {
			int nScroll = SendMessage(hCtl, CB_GETITEMDATA, (WPARAM)SendMessage(hCtl, CB_GETCURSEL, 0, 0), 0);
			SendMessage(hWnd, WM_VSCROLL, MAKELONG(SB_THUMBTRACK, nScroll), (LPARAM)m_Control[CID_SB_SCROLL].hWnd);
			return 1;
							  }
		case CID_CB_FPS:
			// FPSコンボボックス
			m_nFPS = SendMessage(hCtl, CB_GETITEMDATA, (WPARAM)SendMessage(hCtl, CB_GETCURSEL, 0, 0), 0);
			return 1;
		}
		break;
					 }
	case WM_HSCROLL: case WM_VSCROLL: {
		// スクロールバーの共通処理(自動でやってくれよ…)
		SCROLLINFO si = {
			sizeof(SCROLLINFO),
			SIF_ALL, 
		};
		GetScrollInfo(hCtl, SB_CTL, &si);
		int nPos = HIWORD(wParam);
		int nMax = si.nMax-si.nPage+1;
		// この段階でnIDはメッセージの属性(IDじゃない)
		switch(nID) {
		case SB_TOP:// case SB_LEFT:
			si.nPos = si.nMin;
			break;
		case SB_BOTTOM:// case SB_RIGHT:
			si.nPos = si.nMax;
			break;
		case SB_LINEUP:// case SB_LINELEFT:
			si.nPos--;
			break;
		case SB_LINEDOWN:// case SB_LINERIGHT:
			si.nPos++;
			break;
		case SB_PAGEUP:// case SB_PAGELEFT:
			si.nPos -= si.nPage;
			break;
		case SB_PAGEDOWN:// case SB_PAGERIGHT:
			si.nPos += si.nPage;
			break;
		case SB_THUMBPOSITION: case SB_THUMBTRACK:
			si.nPos = nPos;
			break;
		}
		if (si.nPos<si.nMin) si.nPos = si.nMin;
		if (si.nPos>nMax) si.nPos = nMax;
		SetScrollInfo(hCtl, SB_CTL, &si, true);
		nPos = si.nPos;
		//SetScrollPos(hCtl, SB_CTL, nPos, true);
		// ここから個別の処理
		// まず本当のIDを得る
		nID = GetWindowLong(hCtl, GWL_ID);
		switch (nID) {
		case CID_SB_SCROLL:
			for (ControlMapIterator it=m_Control.begin(); it!=m_Control.end(); it++) {
				if (!(*it).second.bScroll) continue;
				SetWindowPos((*it).second.hWnd, NULL,
					(*it).second.x, (*it).second.y-nPos, 0, 0,
					SWP_NOSIZE | SWP_NOZORDER);
				InvalidateRect((*it).second.hWnd, NULL, false);
			}
			return 1;
		}
		break;
					 }
	}
	return 0;
}

HWND CConfigScene::Add(LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, int nID, bool bScroll)
{
	if (nID<=0) nID = m_nAutoID++;
	HWND hWnd = CreateWindow(
		lpClassName,
		lpWindowName,
		WS_CHILD | dwStyle,
		x, y, nWidth, nHeight,
		bScroll?m_hScroll:CAppInitializer::GetHWnd(),
		(HMENU)nID,
		CAppInitializer::GetInstance(),
		NULL);
	m_Control.insert(ControlMapItem(nID, Control(hWnd, bScroll, x, y)));
	return hWnd;
}

HWND CConfigScene::AddAnchor(LPCSTR lpName, int y)
{
	HWND combo = m_Control[CID_CB_CATEGORY].hWnd;
	int index = SendMessage(combo , CB_ADDSTRING , (WPARAM)0 , (LPARAM)lpName);
	SendMessage(combo , CB_SETITEMDATA , (WPARAM)index , (LPARAM)y);
	return Add("STATIC", lpName, SS_CENTER, 0, y, 320-16, ANCHORHEIGHT);
}

HWND CConfigScene::AddComboBox(int x, int y, int nWidth, int nID, bool bScroll, ...)
{
	HWND hWnd = Add("COMBOBOX", NULL, CBS_DROPDOWNLIST, x, y, nWidth, 240, nID, bScroll);
	va_list vl;
	va_start(vl, bScroll);
	for (int i=0; ; i++) {
		LPSTR str=va_arg(vl, LPSTR);
		if (str==NULL) break;
		SendMessage(hWnd, CB_ADDSTRING , (WPARAM)0 , (LPARAM)str);
	}
	va_end(vl);
	return hWnd;
}

HWND CConfigScene::AddComboBoxWithData(int x, int y, int nWidth, int nID, bool bScroll, ...)
{
	HWND hWnd = Add("COMBOBOX", NULL, CBS_DROPDOWNLIST, x, y, nWidth, 240, nID, bScroll);
	va_list vl;
	va_start(vl, bScroll);
	for (int i=0; ; i++) {
		LPSTR str=va_arg(vl, LPSTR);
		if (str==NULL) break;
		int i=SendMessage(hWnd, CB_ADDSTRING , (WPARAM)0 , (LPARAM)str);
		SendMessage(hWnd , CB_SETITEMDATA , (WPARAM)i , (LPARAM)va_arg(vl, int));
	}
	va_end(vl);
	return hWnd;
}

void CConfigScene::AddFPS(HWND hWnd, LPCSTR desc, int nFPS)
{
	char str[256];
	sprintf(str, "ゲームテンポ：%s(%dFPS)", desc, nFPS);
	int i=SendMessage(hWnd, CB_ADDSTRING , (WPARAM)0 , (LPARAM)str);
	SendMessage(hWnd , CB_SETITEMDATA , (WPARAM)i , (LPARAM)nFPS);
	if (m_nFPS==nFPS) SendMessage(hWnd , CB_SETCURSEL , (WPARAM)i , 0);
}

HWND CConfigScene::AddCheckBox(LPCSTR lpText, int x, int y, int w, int h, int nID, bool bChecked, bool bScroll)
{
	HWND hWnd = Add("BUTTON", lpText, BS_AUTOCHECKBOX, x, y, w, h, nID, bScroll);
	SendMessage(hWnd , BM_SETCHECK , (WPARAM)(bChecked?BST_CHECKED:BST_UNCHECKED) , (LPARAM)0);
	return hWnd;
}

bool CConfigScene::GetCheck(int nID)
{
	return SendMessage(m_Control[nID].hWnd, BM_GETCHECK, 0, 0)!=0;
}
