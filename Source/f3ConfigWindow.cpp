// f3ConfigWindow.cpp: Cf3ConfigWindow クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3ConfigWindow.h"
#include "App.h"
#include "f3Setting.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3ConfigWindow::Cf3ConfigWindow()
{

}

Cf3ConfigWindow::~Cf3ConfigWindow()
{

}

void Cf3ConfigWindow::Open()
{
	Run();
	SetParent(GetHWnd(), theApp->GetMyApp()->GetHWnd());
//	while (IsMessage()) {
		theApp->MesSleep(10000);
//	}
}

LRESULT Cf3ConfigWindow::OnPreCreate(CWindowOption &opt)
{
	opt.caption = "ふにゃさん 設定";
	opt.classname = "funya3";
	opt.size_x = 240;
	opt.size_y = 180;
	opt.style = WS_VISIBLE | WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX | WS_POPUP;
	return 0;
}
