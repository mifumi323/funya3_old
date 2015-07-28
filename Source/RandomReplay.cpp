// RandomReplay.cpp: CRandomReplay クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RandomReplay.h"

#include "App.h"
#include "f3MapObjectMain.h"
#include "f3Replay.h"
#include "f3NavigationBar.h"
#include "yaneSDK/yaneDir.h"
#include "yaneSDK/yaneFile.h"

CRandomReplay f3Replay;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CRandomReplay::CRandomReplay()
{
	m_bValid = false;
	m_pPlayer = NULL;
}

CRandomReplay::~CRandomReplay()
{
	Leave();
}

void CRandomReplay::Enter()
{
	if (m_bValid) return;
	Cf3MapObjectMain::SetInput(&ReplayInput);
	m_Phase=100;
	m_bValid = true;
}

void CRandomReplay::Leave()
{
	if (!m_bValid) return;
	Cf3MapObjectMain::SetInput(&KeyInput);
	DELETE_SAFE(m_pPlayer);
	m_bValid = false;
}

void CRandomReplay::Replay(CDIB32 *lp, TTextNavi *text)
{
	if (!m_bValid) return;

	if (m_Phase) m_Phase++;
	if (m_pPlayer==NULL) m_pPlayer = new Cf3Replay();
	if (m_pPlayer->Finished()) {
		if (m_Phase>=80) {
			// 前回の再生が終わったので次を読み込む
			if (LoadNextReplay()) {
				// ダメだったのでリプレイ中止
				Leave();
				return;
			}
			if (text) {
				string navi="";
				navi += "デモプレイ：";
				navi += CFile::GetPureFileNameOf(m_pPlayer->GetFileName());
				text->text->GetFont()->SetText(navi);
				text->Update();
			}
			m_Phase=-60;
		}ef(!m_Phase) {
			m_Phase++;
		}
	}
	if (!m_Phase) m_pPlayer->Replay();
	m_pPlayer->OnDraw(lp);
	int fade=600-abs(10*m_Phase);
	Saturate(0,fade,200);
	lp->FadeEffect(fade);
}

HRESULT CRandomReplay::LoadNextReplay()
{
	if (m_pPlayer==NULL) m_pPlayer = new Cf3Replay();
	CDir dir;
	string file, next=m_pPlayer->GetFileName();
	dir.SetPath(CFile::GetCurrentDir());
	dir.SetFindFile("*.f3r");
	int i=0;
	while (!dir.FindFile(file)) {
		if (file!=m_pPlayer->GetFileName()) {
			if (CApp::random(++i)==0) next = file;
		}
	}
	if (!dir.IsFileExist(next)) return 1;
	m_pPlayer->Load(next);
	return 0;
}
