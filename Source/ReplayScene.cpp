// ReplayScene.cpp: CReplayScene クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReplayScene.h"
#include "App.h"
#include "f3Select.h"
#include "f3Replay.h"
#include "Version.h"
#include "ResourceManager.h"
#include "f3MapObjectMain.h"
#include "yaneSDK/yaneDir.h"
#include "yaneSDK/yaneFile.h"

bool CReplayScene::record = false;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CReplayScene::CReplayScene()
{
	m_NavigationBar = NULL;
	m_pPlayer = NULL;
}

CReplayScene::~CReplayScene()
{
	DELETE_SAFE(m_NavigationBar);
	delete m_BananaCount.text;
	delete m_StageTitle.text;
	delete m_Message;
	DELETE_SAFE(m_pPlayer);
}

void CReplayScene::OnInit()
{
	// メインメニュー
	m_Message = new CTextDIB32;
	m_Message->GetFont()->SetSize(32);
	m_Message->GetFont()->SetColor(0xffffff);
	m_Message->GetFont()->SetBackColor(0x303030);
	// ナビゲーション
	m_NavigationBar = new Cf3NavigationBar;
	m_BananaGraphic.dib = ResourceManager.Get(RID_MAIN);
	m_BananaGraphic.offset=0;
	m_BananaGraphic.rect.left=416;
	m_BananaGraphic.rect.top=96;
	m_BananaGraphic.rect.right=432;
	m_BananaGraphic.rect.bottom=112;
	m_NavigationBar->Add(&m_BananaGraphic);
	m_BananaCount.Set(new CTextDIB32);
	m_BananaCount.text->GetFont()->SetSize(16);
	m_BananaCount.text->GetFont()->SetColor(0x000020);
	m_BananaCount.text->GetFont()->SetBackColor(0x303030);
	m_NavigationBar->Add(&m_BananaCount);
	m_StageTitle.Set(new CTextDIB32);
	m_StageTitle.text->GetFont()->SetSize(16);
	m_StageTitle.text->GetFont()->SetColor(0x000020);
	m_StageTitle.text->GetFont()->SetBackColor(0x303030);
	m_NavigationBar->Add(&m_StageTitle);
	// リプレイ関係
	Cf3MapObjectMain::SetInput(&ReplayInput);
	m_Phase=100;
	// その他
	if (record) theApp->AVIRecordingStart();
}

void CReplayScene::OnDraw(CDIB32 *lp)
{
	lp->Clear(0);
	if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
		// TODO
	}
	if (m_Phase) m_Phase++;
	if (m_pPlayer==NULL) m_pPlayer = new Cf3Replay();
	if (m_pPlayer->Finished()) {
		if (m_Phase>=80) {
			if(record) record = false;
			else theApp->AVIRecordingStop();

			m_pPlayer->Load(theApp->GetStageFile());
			m_pMap = m_pPlayer->GetMap();
			string t = m_pMap->GetTitle();
			t = replace_all(t, (string)"\\n", (string)" ");
			m_StageTitle.text->GetFont()->SetText("リプレイ：%s",t.c_str());
			m_StageTitle.Update();
			m_BananaCount.text->GetFont()->SetText("%03d/%03d",m_pMap->GetGotBanana(),m_pMap->GetTotalBanana());
			m_BananaCount.Update();
			t = m_pMap->GetTitle();
			t = replace_all(t, (string)"\\n", (string)"\n");
			m_Message->GetFont()->SetText(t);
			m_Message->UpdateText();
			theApp->GetBGM()->Play(m_pMap->GetBGM());
			
			m_Phase=-40;
		}ef(!m_Phase) {
			if (m_pMap->GetGotBanana()==m_pMap->GetTotalBanana()) {
				m_Message->GetFont()->SetText("Stage Clear!!");
				theApp->GetBGM()->Play(BGMN_CLEAR);
			}else {
				m_Message->GetFont()->SetText("Miss!!");
				theApp->GetBGM()->Play(BGMN_MISS);
			}
			m_Message->UpdateText();
			m_Phase++;
		}
	}
	if (!m_Phase) {
		m_pPlayer->Replay();
		m_BananaCount.text->GetFont()->SetText("%03d/%03d",m_pMap->GetGotBanana(),m_pMap->GetTotalBanana());
		m_BananaCount.Update();
	}
	m_pPlayer->OnDraw(lp);
	m_NavigationBar->OnDraw(lp);
	if (m_Phase) {
		int w,h;
		m_Message->GetSize(w,h);
		lp->Blt(m_Message,160-w/2,120-h/2);
	}
}
