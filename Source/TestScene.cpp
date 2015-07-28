// TestScene.cpp: CTestScene クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestScene.h"
#include "f3Map.h"
#include "f3StageFile.h"
#include "f3NavigationBar.h"
#include "f3Select.h"
#include "f3MapObjectMain.h"
#include "yaneSDK/yaneAppInitializer.h"
#include "yaneSDK/yaneLineParser.h"
#include "ResourceManager.h"
#include "f3Replay.h"

#define CENTERX 160
#define CENTERY 104

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CTestScene::CTestScene()
{
}

CTestScene::~CTestScene()
{
	delete m_Map;
	delete m_StageFile;
	delete m_NavigationBar;
	delete m_BananaCount.text;
	delete m_StageTitle.text;
	delete m_RestCount.text;
	delete m_Message;
	delete m_Select;
	delete m_Temparature;
}

void CTestScene::OnInit()
{
	m_pRecorder = NULL;
	Cf3MapObjectMain::SetInput(&KeyInput);
	CLineParser l;
	l.SetLine(CAppInitializer::GetCmdLine());
	l.IsMatch("TEST");	// マッチしている場合のみ呼ばれる
	string s;
	l.GetStr(s);
	theApp->SetStageFile(s+".f3s");
	int n=0;
	l.GetNum(n);
	m_NavigationBar = new Cf3NavigationBar;
	m_StageFile = new Cf3StageFile;
	m_StageFile->Read(theApp->GetStageFile());
	BYTE*Buf = m_StageFile->GetStageData(0x43475453);
	m_nStageCount = (Buf?*Buf:1);
	Buf = m_StageFile->GetStageData(0x54534552);
	m_nRest = (Buf?*Buf:4);
	Buf = m_StageFile->GetStageData(0x544C4E50);
	m_nPenalty = (Buf?*Buf:100);
	m_Map = NULL;
	m_BananaGraphic.dib = new CDIB32;
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
	m_RestGraphic.dib = new CDIB32;
	m_RestGraphic.dib = ResourceManager.Get(RID_MAIN);
	m_RestGraphic.offset=0;
	m_RestGraphic.rect.left=352;
	m_RestGraphic.rect.top=96;
	m_RestGraphic.rect.right=368;
	m_RestGraphic.rect.bottom=112;
	m_NavigationBar->Add(&m_RestGraphic);
	m_RestCount.Set(new CTextDIB32);
	m_RestCount.text->GetFont()->SetSize(16);
	m_RestCount.text->GetFont()->SetColor(0x000020);
	m_RestCount.text->GetFont()->SetBackColor(0x303030);
	m_RestCount.offset = 0;
	m_NavigationBar->Add(&m_RestCount);
	m_Message = new CTextDIB32;
	m_Message->GetFont()->SetSize(32);
	m_Message->GetFont()->SetColor(0xffffff);
	m_Message->GetFont()->SetBackColor(0x303030);
	m_Select = new Cf3Select;
	m_Select->SetPos(64,64);
	m_Temparature = new CDIB32;
	m_Temparature->CreateSurface(320,240);
	UpdateRest();
	StartGame(n);
}

void CTestScene::OnDraw(CDIB32 *lp)
{
	switch (m_State) {
	case GSS_PLAYING: {
		if (m_MessageCount==0) {
			if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
				Pause();
				break;
			}
			if (m_pRecorder) m_pRecorder->Record();
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
/*			if (++m_nStage<m_nStageCount) {
				StartGame(m_nStage);
				break;
			}else{
				Ending();
				break;
			}*/
			StartGame(m_nStage);
		}
		break;
					 }
	case GSS_ALLCLEARED: {
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
		if (!f3Input.GetKeyPressed(F3KEY_SMILE)) lp->Blt(m_Message,160-w/2,120-sh/2-h/2);
		m_MessageCount--;
	}
}

void CTestScene::StartGame(int stage)
{
	m_nStage = stage;
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
	m_State = GSS_PLAYING;
	theApp->GetBGM()->Play(m_Map->GetBGM());
	m_Select->Clear();
	DELETE_SAFE(m_pRecorder);
	m_pRecorder = new Cf3Replay();
	m_pRecorder->StartRecording();
}

void CTestScene::StageClear()
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

void CTestScene::Miss()
{
	if (CApp::binaryrandom(theSetting->m_RecordMiss)) SaveReplay();
	m_RestGraphic.rect.left=384;
	m_RestGraphic.rect.right=400;
	m_Message->GetFont()->SetText("Miss!!");
	m_Message->UpdateText();
	m_MessageCount = -1;
	//m_nRest--;
	m_State = GSS_DEAD;
	UpdateRest();
	theApp->GetBGM()->Play(BGMN_MISS);
	m_Map->GetMainChara()->Die();
	m_Select->Clear();
}

void CTestScene::Ending()
{
	m_State = GSS_ALLCLEARED;
	m_Message->GetFont()->SetText("All Clear!!");
	m_Message->UpdateText();
	m_MessageCount = -1;
	theApp->GetBGM()->Play(BGMN_ENDING);
}

void CTestScene::GameOver()
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

void CTestScene::RestartGame(int stage)
{
	BYTE*Buf = m_StageFile->GetStageData(0x54534552);
	m_nRest = (Buf?*Buf:4);
	UpdateRest();
	StartGame(stage);
}

void CTestScene::UpdateRest()
{
/*	if (m_nRest>1) {
		m_RestCount->GetFont()->SetText("x%03d",m_nRest);
	}ef(m_nRest==1) {
		m_RestCount->GetFont()->SetText("LAST");
	}else{
		m_RestCount->GetFont()->SetText("OOPS");
	}*/
	m_RestCount.text->GetFont()->SetText("TEST");
	m_RestCount.Update();
}

void CTestScene::Pause()
{
	m_State = GSS_PAUSED;
	m_Map->GarbageMapObject();
	if (theSetting->m_ESP) m_Map->CreateTemparatureMap(m_Temparature);
	m_Select->Clear();
	m_Select->Add("Return to Game");
	m_Select->Add("Give Up");
	m_Select->Add("Exit");
	int sw,sh;
	m_Select->GetSize(sw,sh);
	m_Select->SetPos(160-sw/2,136-sh/2);
	m_Message->GetFont()->SetText("Pause");
	m_Message->UpdateText();
	m_MessageCount = -1;
}

void CTestScene::Unpause()
{
	m_State = GSS_PLAYING;
	m_Select->Clear();
	m_MessageCount = 0;
}

void CTestScene::SaveReplay()
{
	if (m_pRecorder==NULL) return;
	m_pRecorder->Save(m_StageFile, m_nStage);
	DELETE_SAFE(m_pRecorder);
}
