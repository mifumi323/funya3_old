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
//	m_NavigationBar = NULL;
	m_pPlayer = NULL;
}

CReplayScene::~CReplayScene()
{
//	DELETE_SAFE(m_NavigationBar);
	delete m_BananaCount.text;
	delete m_StageTitle.text;
	if (theSetting->m_ViewTime&3) delete m_TimeCount.text;
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
//	m_NavigationBar = new Cf3NavigationBar;
	f3Navi.Clear();
	m_StageTitle.Set(new CTextDIB32);
	f3Navi.Add(&m_StageTitle);
	m_BananaGraphic.dib = ResourceManager.Get(RID_MAIN);
	m_BananaGraphic.offset=0;
	m_BananaGraphic.rect.left=416;
	m_BananaGraphic.rect.top=96;
	m_BananaGraphic.rect.right=432;
	m_BananaGraphic.rect.bottom=112;
	f3Navi.Add(&m_BananaGraphic);
	m_BananaCount.Set(new CTextDIB32);
	f3Navi.Add(&m_BananaCount);
	if (theSetting->m_ViewTime&3) {
		m_BananaCount.offset = 0;
		m_TimeGraphic.dib = ResourceManager.Get(RID_MAIN);
		m_TimeGraphic.offset=0;
		m_TimeGraphic.rect.left=416;
		m_TimeGraphic.rect.top=112;
		m_TimeGraphic.rect.right=432;
		m_TimeGraphic.rect.bottom=128;
		f3Navi.Add(&m_TimeGraphic);
		m_TimeCount.Set(new CTextDIB32);
		f3Navi.Add(&m_TimeCount);
	}
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
			m_nTime = 0;
			UpdateTime();
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
		m_nTime++;
		UpdateTime();
		m_BananaCount.text->GetFont()->SetText("%03d/%03d",m_pMap->GetGotBanana(),m_pMap->GetTotalBanana());
		m_BananaCount.Update();
	}
	m_pPlayer->OnDraw(lp);
	f3Navi.OnDraw(lp);
	if (m_Phase) {
		int w,h;
		m_Message->GetSize(w,h);
		lp->Blt(m_Message,160-w/2,120-h/2);
	}
}

void CReplayScene::UpdateTime()
{
	switch (theSetting->m_ViewTime&3){
	case 0:{
		return;
		   }
	case 1:{
		char fmt[] = "%00d/%00d";
		int i=1;
		for (int size=m_pPlayer->GetSize(); size>=10; size/=10) i++;
		Saturate(1, i, 9);
		sprintf(fmt, "%%0%dd/%%0%dd", i, i);
		m_TimeCount.text->GetFont()->SetText(fmt, m_nTime, m_pPlayer->GetSize());
		break;
		   }
	case 2:{
		int size = m_pPlayer->GetSize();
		int hour =  m_nTime/40 /60 /60;
		int min  = (m_nTime/40 /60)%60;
		int sec  = (m_nTime/40)%60;
		int shour =  size/40 /60 /60;
		int smin  = (size/40 /60)%60;
		int ssec  = (size/40)%60;
		if(size<10*60*40) {
			m_TimeCount.text->GetFont()->SetText("%d:%02d/%d:%02d",
				min, sec, smin, ssec);
		}ef(size<60*60*40) {
			m_TimeCount.text->GetFont()->SetText("%02d:%02d/%02d:%02d",
				min, sec, smin, ssec);
		}else {
			m_TimeCount.text->GetFont()->SetText("%d:%02d:%02d/%d:%02d:%02d",
				hour, min, sec, shour, smin, ssec);
		}
		break;
		   }
	case 3:{
		int size = m_pPlayer->GetSize();
		int hour =  m_nTime/40 /60 /60;
		int min  = (m_nTime/40 /60)%60;
		int sec  = (m_nTime/40)%60;
		int msec = (m_nTime%40)*25;
		int shour =  size/40 /60 /60;
		int smin  = (size/40 /60)%60;
		int ssec  = (size/40)%60;
		int smsec = (size%40)*25;
		if (size<10*40) {
			m_TimeCount.text->GetFont()->SetText("%d.%03d/%d.%03d",
				sec, msec, ssec, smsec);
		}ef(size<60*40) {
			m_TimeCount.text->GetFont()->SetText("%02d.%03d/%02d.%03d",
				sec, msec, ssec, smsec);
		}ef(size<10*60*40) {
			m_TimeCount.text->GetFont()->SetText("%d:%02d.%03d/%d:%02d.%03d",
				min, sec, msec, smin, ssec, smsec);
		}ef(size<60*60*40) {
			m_TimeCount.text->GetFont()->SetText("%02d:%02d.%03d/%02d:%02d.%03d",
				min, sec, msec, smin, ssec, smsec);
		}else {
			m_TimeCount.text->GetFont()->SetText("%d:%02d:%02d.%03d/%d:%02d:%02d.%03d",
				hour, min, sec, msec, shour, smin, ssec, smsec);
		}
		break;
		   }
	}
	m_TimeCount.Update();
}
