// GameScene.cpp: CGameScene クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameScene.h"
#include "f3Map.h"
#include "f3StageFile.h"
#include "f3Select.h"
#include "f3MapObjectMain.h"
#include "yaneSDK/yaneFile.h"
#include "ResourceManager.h"
#include "f3Replay.h"

#define CENTERX 160
#define CENTERY 104

bool CGameScene::m_bTest = false;
int CGameScene::m_nStage = 0;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CGameScene::CGameScene()
{
}

CGameScene::~CGameScene()
{
	delete m_Map;
	delete m_StageFile;
	delete m_NavigationBar;
	delete m_BananaCount.text;
	delete m_StageTitle.text;
	delete m_RestCount.text;
	if (theSetting->m_ViewTime&3) delete m_TimeCount.text;
	delete m_Message;
	delete m_Select;
	delete m_Temparature;
	DELETE_SAFE(m_pRecorder);
}

void CGameScene::OnInit()
{
	// リプレイレコーダー
	m_pRecorder = NULL;
	Cf3MapObjectMain::SetInput(&KeyInput);
	// 基本情報
	m_StageFile = new Cf3StageFile;
	m_StageFile->Read(theApp->GetStageFile());
	BYTE*Buf = m_StageFile->GetStageData(CT_STGC);
	m_nStageCount = (Buf?*Buf:1);
	Buf = m_StageFile->GetStageData(CT_REST);
	m_nRest = (Buf?*Buf:4);
	Buf = m_StageFile->GetStageData(CT_PNLT);
	m_nPenalty = (Buf?*Buf:100);
	m_Map = NULL;
	// ナビゲーション
	m_NavigationBar = new Cf3NavigationBar;
	m_StageTitle.Set(new CTextDIB32);
	m_NavigationBar->Add(&m_StageTitle);
	m_BananaGraphic.dib = ResourceManager.Get(RID_MAIN);
	m_BananaGraphic.offset=0;
	m_BananaGraphic.rect.left=416;
	m_BananaGraphic.rect.top=96;
	m_BananaGraphic.rect.right=432;
	m_BananaGraphic.rect.bottom=112;
	m_NavigationBar->Add(&m_BananaGraphic);
	m_BananaCount.offset = 0;
	m_BananaCount.Set(new CTextDIB32);
	m_NavigationBar->Add(&m_BananaCount);
	m_RestGraphic.dib = ResourceManager.Get(RID_MAIN);
	m_RestGraphic.offset=0;
	m_RestGraphic.rect.left=352;
	m_RestGraphic.rect.top=96;
	m_RestGraphic.rect.right=368;
	m_RestGraphic.rect.bottom=112;
	m_NavigationBar->Add(&m_RestGraphic);
	m_RestCount.Set(new CTextDIB32);
	m_NavigationBar->Add(&m_RestCount);
	if (theSetting->m_ViewTime&3) {
		m_RestCount.offset = 0;
		m_TimeGraphic.dib = ResourceManager.Get(RID_MAIN);
		m_TimeGraphic.offset=0;
		m_TimeGraphic.rect.left=416;
		m_TimeGraphic.rect.top=112;
		m_TimeGraphic.rect.right=432;
		m_TimeGraphic.rect.bottom=128;
		m_NavigationBar->Add(&m_TimeGraphic);
		m_TimeCount.Set(new CTextDIB32);
		m_NavigationBar->Add(&m_TimeCount);
	}
	// メニュー
	m_Message = new CTextDIB32;
	m_Message->GetFont()->SetSize(32);
	m_Message->GetFont()->SetColor(0xffffff);
	m_Message->GetFont()->SetBackColor(0x303030);
	m_Select = new Cf3Select;
	m_Select->SetPos(64,64);
	// その他
	m_Temparature = new CDIB32;
	m_Temparature->CreateSurface(320,240);
	// ゲーム開始
	if (!m_bTest) m_nStage = theSetting->GetProgress(theApp->GetStageFile());
	UpdateRest();
	m_bSecret4 = m_nStage==0;
	StartGame(m_nStage);
}

void CGameScene::OnDraw(CDIB32 *lp)
{
	switch (m_State) {
	case GSS_PLAYING: {
		if (m_MessageCount==0) {
			if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
				Pause();
				break;
			}
			if (m_pRecorder) m_pRecorder->Record();
			m_nTime++;
			UpdateTime();
			m_Map->OnMove();
			if (m_Map->GetGotBanana()==m_Map->GetTotalBanana()) {
				StageClear();
			}ef(m_Map->IsMainCharaDied()) {
				Miss();
			}
			m_BananaCount.text->GetFont()->SetText("%03d/%03d",m_Map->GetGotBanana(),m_Map->GetTotalBanana());
			m_BananaCount.Update();
			m_Map->OnPreDraw();
		}
		break;
					  }
	case GSS_PAUSED: {
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
			if (m_Select->GetSelected()==0) {	// 戻る
				Unpause();
			}ef (m_Select->GetSelected()==1) {	// 自爆
				Miss();
			}ef (m_Select->GetSelected()==2) {	// 終了
				f3Input.Input();
				JumpScene(FIRST_SCENE);
			}
			break;
		}
		m_Select->Enable(!f3Input.GetKeyPressed(F3KEY_SMILE));
		break;
					 }
	case GSS_CLEARED: {
		if (f3Input.GetKeyPushed(F3KEY_RECORD)) SaveReplay();
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
			if (!m_bTest && ++m_nStage>=m_nStageCount) {
				Ending();
				break;
			}
			StartGame(m_nStage);
			break;
		}
		break;
					 }
	case GSS_ALLCLEARED: {
		// テストでは呼ばれない
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
			f3Input.Input();
			JumpScene(FIRST_SCENE);
			break;
		}
		break;
					 }
	case GSS_DEAD: {
		if (f3Input.GetKeyPushed(F3KEY_RECORD)) SaveReplay();
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
			if (m_nRest) StartGame(m_nStage);
			else GameOver();
			break;
		}
		break;
					 }
	case GSS_GAMEOVER: {
		// テストでは呼ばれない
		if (m_MessageCount==0) {
			f3Input.Input();
			JumpScene(FIRST_SCENE);
		}else {
			if (m_MessageCount%40==0&&m_MessageCount>0) {
				m_Message->GetFont()->SetText("Continue?%2d",m_MessageCount/40-1);
				m_Message->UpdateText();
			}
			if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
				if (theSetting->m_Banana>=m_nPenalty&&m_Select->GetSelected()==0) {
					theSetting->m_Banana-=m_nPenalty;
					RestartGame(m_nStage);
				}else {
					f3Input.Input();
					JumpScene(FIRST_SCENE);
				}
			}
		}
		break;
					 }
	default: {
		f3Input.Input();
		JumpScene(FIRST_SCENE);
			 }
	}
	switch (m_State) {
	case GSS_PLAYING: {
		m_Map->OnDraw(lp);
		m_NavigationBar->OnDraw(lp);
		break;
					  }
	case GSS_PAUSED: {
		m_Map->OnDraw(lp);
		if (theSetting->m_ESP) lp->BlendBlt(m_Temparature,0,0,0x808080,0x7f7f7f);
		m_NavigationBar->OnDraw(lp);
		break;
					 }
	case GSS_CLEARED: {
		m_Map->OnDraw(lp);
		m_NavigationBar->OnDraw(lp);
		break;
					 }
	case GSS_ALLCLEARED: {
		m_Map->OnDraw(lp);
		m_NavigationBar->OnDraw(lp);
		break;
					 }
	case GSS_DEAD: {
		m_Map->OnDraw(lp);
		m_NavigationBar->OnDraw(lp);
		break;
					 }
	case GSS_GAMEOVER: {
		m_Map->OnDraw(lp);
		m_NavigationBar->OnDraw(lp);
		break;
					 }
	default: {
			 }
	}
	if (m_MessageCount!=0) {
		int w,h,sw,sh;
		m_Message->GetSize(w,h);
		m_Select->GetSize(sw,sh);
		if (!f3Input.GetKeyPressed(F3KEY_SMILE)) {
			if (w<=320) lp->Blt(m_Message,160-w/2,120-sh/2-h/2);
			else {
				SIZE sz = { 320, h*320/w };
				lp->Blt(m_Message,0,120-sh/2-sz.cy/2,NULL, &sz);
			}
		}
		m_MessageCount--;
	}
}

void CGameScene::StartGame(int stage)
{
	m_nStage = stage;
	if (!m_bTest) theSetting->SetProgress(theApp->GetStageFile(), stage);
	DELETE_SAFE(m_Map);
	m_Map = new Cf3Map(m_StageFile,stage);
	m_BananaCount.text->GetFont()->SetText("%03d/%03d",m_Map->GetGotBanana(),m_Map->GetTotalBanana());
	m_BananaCount.Update();
	m_RestGraphic.rect.left=352;
	m_RestGraphic.rect.right=368;
	string t = m_Map->GetTitle();
	t = replace_all(t, (string)"\\n", (string)" ");
	m_StageTitle.text->GetFont()->SetText("ステージ%d：%s",stage+1,t.c_str());
	m_StageTitle.Update();
	t = m_Map->GetTitle();
	t = replace_all(t, (string)"\\n", (string)"\n");
	m_Message->GetFont()->SetText(t);
	m_Message->UpdateText();
	m_MessageCount = 40;
	m_nTime = 0;
	UpdateTime();
	m_State = GSS_PLAYING;
	theApp->GetBGM()->Play(m_Map->GetBGM());
	m_Select->Clear();
	DELETE_SAFE(m_pRecorder);
	m_pRecorder = new Cf3Replay();
	m_pRecorder->StartRecording();
}

void CGameScene::StageClear()
{
	if (CApp::binaryrandom(theSetting->m_RecordClear)) SaveReplay();
	m_RestGraphic.rect.left=368;
	m_RestGraphic.rect.right=384;
	m_BananaCount.text->GetFont()->SetText(
		"%03d/%03d",m_Map->GetGotBanana(), m_Map->GetTotalBanana());
	m_BananaCount.Update();
	m_Message->GetFont()->SetText("Stage Clear!!");
	m_Message->UpdateText();
	m_MessageCount = -1;
	m_State = GSS_CLEARED;
	theSetting->m_Banana += m_Map->GetTotalBanana();
	theApp->GetBGM()->Play(BGMN_CLEAR);
}

void CGameScene::Miss()
{
	if (CApp::binaryrandom(theSetting->m_RecordMiss)) SaveReplay();
	m_RestGraphic.rect.left=384;
	m_RestGraphic.rect.right=400;
	m_Message->GetFont()->SetText("Miss!!");
	m_Message->UpdateText();
	m_MessageCount = -1;
	if (!m_bTest) {
		m_nRest--;
		m_bSecret4 = false;
	}
	m_State = GSS_DEAD;
	UpdateRest();
	theApp->GetBGM()->Play(BGMN_MISS);
	m_Map->GetMainChara()->Die();
	m_Select->Clear();
}

void CGameScene::Ending()
{
	m_State = GSS_ALLCLEARED;
	m_Message->GetFont()->SetText("All Clear!!");
	m_Message->UpdateText();
	m_MessageCount = -1;
	theApp->GetBGM()->Play(BGMN_ENDING);
	// Secret 1!!
	theSetting->m_Smiles++;
	// Secret 2!!
	if (CFile::GetPureFileNameOf(theApp->GetStageFile())=="0.f3s")
		theSetting->m_TimeMaster++;
	// Secret 3!!
	if (theSetting->m_Banana>=1000) theSetting->m_Eyewitness++;
	// Secret 4!!
	if (m_bSecret4) theSetting->m_FeatherIron++;
	// Secret 5!!
	if (theSetting->m_SleepTime>=40*60*10) theSetting->m_GrapeColored++;
	// Secret 6!!
	if (theSetting->m_PlayTime>=60*60*10) theSetting->m_Esrever++;
	// Secret 7!!
	if (theSetting->m_Smiles>=100) theSetting->m_Outline++;
	// Secret 8!!
	if (theSetting->m_Banana>=10000) theSetting->m_ColdMan++;
	// Secret 9!!
	if (theSetting->m_PlayTime>=60*60*100) theSetting->m_AndBalloon++;
	// Perfect!!
/*	if (setting->m_Smiles&&setting->m_TimeMaster&&setting->m_Eyewitness&&
		setting->m_FeatherIron&&setting->m_GrapeColored&&
		setting->m_Esrever&&setting->m_Outline&&setting->m_ColdMan)
		setting->???++;*/
	theSetting->SetProgress(theApp->GetStageFile(), 0);
}

void CGameScene::GameOver()
{
	m_Message->GetFont()->SetText("Game Over!!");
	m_Message->UpdateText();
	m_Select->Clear();
	if (theSetting->m_Banana>=m_nPenalty) {
		char buf[1024];
		sprintf(buf,"Yes(%4d)",-m_nPenalty);
		m_MessageCount = 480;
		m_Select->Add(buf);
		m_Select->Add("No (Exit)");
		int sw,sh;
		m_Select->GetSize(sw,sh);
		m_Select->SetPos(160-sw/2,136-sh/2);
	}else {
		m_MessageCount = -1;
	}
	m_State = GSS_GAMEOVER;
	theApp->GetBGM()->Play(BGMN_GAMEOVER);
}

void CGameScene::RestartGame(int stage)
{
	BYTE*Buf = m_StageFile->GetStageData(0x54534552);
	m_nRest = (Buf?*Buf:4);
	UpdateRest();
	StartGame(stage);
}

void CGameScene::UpdateRest()
{
	if (!m_bTest) {
		if (m_nRest>1) {
			m_RestCount.text->GetFont()->SetText("x%03d",m_nRest);
		}ef(m_nRest==1) {
			m_RestCount.text->GetFont()->SetText("LAST");
		}else{
			m_RestCount.text->GetFont()->SetText("OOPS");
		}
	}else {
		m_RestCount.text->GetFont()->SetText("TEST");
	}
	m_RestCount.Update();
}

void CGameScene::Pause()
{
	m_State = GSS_PAUSED;
	m_Map->GarbageMapObject();
	if (theSetting->m_ESP) m_Map->CreateTemparatureMap(m_Temparature);
	m_Select->Clear();
	m_Select->Add("続ける");
	m_Select->Add("自爆");
	m_Select->Add("もうやめる");
	int sw,sh;
	m_Select->GetSize(sw,sh);
	m_Select->SetPos(160-sw/2,136-sh/2);
	m_Message->GetFont()->SetText("Pause");
	m_Message->UpdateText();
	m_MessageCount = -1;
}

void CGameScene::Unpause()
{
	m_State = GSS_PLAYING;
	m_Select->Clear();
	m_MessageCount = 0;
}

void CGameScene::SaveReplay()
{
	if (m_pRecorder==NULL) return;
	m_pRecorder->Save(m_StageFile, m_nStage);
	DELETE_SAFE(m_pRecorder);
}

void CGameScene::UpdateTime()
{
	switch (theSetting->m_ViewTime&3){
	case 0:{
		return;
		   }
	case 1:{
		m_TimeCount.text->GetFont()->SetText("%04d", m_nTime);
		break;
		   }
	case 2:{
		int hour =  m_nTime/40 /60 /60;
		int min  = (m_nTime/40 /60)%60;
		int sec  = (m_nTime/40)%60;
		if(m_nTime<10*60*40) {
			m_TimeCount.text->GetFont()->SetText("%d:%02d", min, sec);
		}ef(m_nTime<60*60*40) {
			m_TimeCount.text->GetFont()->SetText("%02d:%02d", min, sec);
		}else {
			m_TimeCount.text->GetFont()->SetText("%d:%02d:%02d", hour, min, sec);
		}
		break;
		   }
	case 3:{
		int hour =  m_nTime/40 /60 /60;
		int min  = (m_nTime/40 /60)%60;
		int sec  = (m_nTime/40)%60;
		int msec = (m_nTime%40)*25;
		if(m_nTime<60*40) {
			m_TimeCount.text->GetFont()->SetText("%02d.%03d", sec, msec);
		}ef(m_nTime<10*60*40) {
			m_TimeCount.text->GetFont()->SetText("%d:%02d.%03d", min, sec, msec);
		}ef(m_nTime<60*60*40) {
			m_TimeCount.text->GetFont()->SetText("%02d:%02d.%03d", min, sec, msec);
		}else {
			m_TimeCount.text->GetFont()->SetText("%d:%02d:%02d.%03d", hour, min, sec, msec);
		}
		break;
		   }
	}
	m_TimeCount.Update();
}
