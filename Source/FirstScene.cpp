// FirstScene.cpp: FirstScene �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FirstScene.h"
#include "GameScene.h"
#include "App.h"
#include "f3Select.h"
#include "f3Replay.h"
#include "Version.h"
#include "ResourceManager.h"
#include "f3MapObjectMain.h"
#include "yaneSDK/yaneDir.h"
#include "yaneSDK/yaneFile.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CFirstScene::CFirstScene()
{
	m_Select = NULL;
	m_NavigationBar = NULL;
	m_TextTitle = NULL;
	m_TextNavi.dib = NULL;
	m_TextDemo.dib = NULL;
	m_pPlayer = NULL;
}

CFirstScene::~CFirstScene()
{
	DELETE_SAFE(m_Select);
	DELETE_SAFE(m_NavigationBar);
	DELETE_SAFE(m_TextTitle);
	DELETE_SAFE(m_TextNavi.dib);
	DELETE_SAFE(m_TextDemo.dib);
	DELETE_SAFE(m_pPlayer);
}

void CFirstScene::OnInit()
{
	// ���C�����j���[
	m_Select = new Cf3Select;
	m_NavigationBar = new Cf3NavigationBar;
	m_TextTitle = new CTextDIB32;
	m_Select->SetPos(80,80);
	m_Select->Add("�Q�[���J�n");
	m_Select->Add("�������");
	m_Select->Add("�I��");
	m_TextTitle->GetFont()->SetSize(32);
	m_TextTitle->GetFont()->SetColor(0xffffff);
	m_TextTitle->GetFont()->SetBackColor(0x303030);
	m_TextTitle->GetFont()->SetText("�ӂɂႳ��");
	m_TextTitle->UpdateText();
	// �i�r�Q�[�V����
	m_TextNavi.Set(new CTextDIB32);
//	m_TextNavi.text->GetFont()->SetSize(16);
//	m_TextNavi.text->GetFont()->SetColor(0x000020);
//	m_TextNavi.text->GetFont()->SetBackColor(0x303030);
	{
		string navi="�@�@TokioSoft�@�ӂɂႳ��";
		char strbuf[256];
		navi += "�@���쌠�F����(�Ƃ���)";
		sprintf(strbuf,"�@�o�[�W�����F%s",CVersion().GetVersion());
		navi += strbuf;
		sprintf(strbuf,"�@%s�������ăX�^�[�g",f3Input.GetKeyName(F3KEY_PAUSE));
		navi += strbuf;
		sprintf(strbuf,"�@�@���n�F%d��",theSetting->m_Banana);
		navi += strbuf;
		sprintf(strbuf,"�@���S�N���A�񐔁F%d��",theSetting->m_Smiles);
		navi += strbuf;
		m_TextNavi.text->GetFont()->SetText(navi);
	}
	m_TextNavi.Update();
	m_NavigationBar->Add(&m_TextNavi);
	m_TextDemo.Set(new CTextDIB32);
//	m_TextDemo.text->GetFont()->SetSize(16);
//	m_TextDemo.text->GetFont()->SetColor(0x000020);
//	m_TextDemo.text->GetFont()->SetBackColor(0x303030);
	m_TextDemo.text->GetFont()->SetText("");
	m_TextDemo.Update();
	m_NavigationBar->Add(&m_TextDemo);
	// ���v���C�֌W
	Cf3MapObjectMain::SetInput(&ReplayInput);
	FindAllReplayFile();
	m_Phase=100;
	// ���̑�
	m_Icon = ResourceManager.Get(RID_MAIN);
	theApp->GetBGM()->Play(BGMN_TITLE);
	CGameScene::m_bTest = false;
}

void CFirstScene::OnDraw(CDIB32 *lp)
{
	lp->Clear(0);
	if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
		if (m_Select->GetSelected()==0) {
			f3Input.Input();
			JumpScene(TITLE_SCENE);
		}ef(m_Select->GetSelected()==1) {
			f3Input.Input();
			JumpScene(EXPLAIN_SCENE);
		}else {
			ExitScene();
		}
	}
	Replay(lp);
	lp->Blt(m_TextTitle,80,32);
	RECT rc = { 256, 96, 288, 128 };
	lp->Blt(m_Icon,48,32,&rc);
	m_NavigationBar->OnDraw(lp);
}

void CFirstScene::FindAllReplayFile()
{
	CDir dir;
	string file;
	list<string> replayList;
	dir.SetPath(CFile::GetCurrentDir());
	dir.SetFindFile("*.f3r");
	while (!dir.FindFile(file)) replayList.push_back(file);
	m_ReplayList.resize(replayList.size());
	int i=0;
	for (list<string>::iterator it=replayList.begin(); it!=replayList.end(); it++)
		m_ReplayList[i++] = *it;
}

void CFirstScene::Replay(CDIB32 *lp)
{
	if (m_Phase) m_Phase++;
	if (m_ReplayList.size()==0) return;
	if (m_pPlayer==NULL) m_pPlayer = new Cf3Replay();
	if (m_pPlayer->Finished()) {
		if (m_Phase>=80) {
			m_pPlayer->Load(m_ReplayList[CApp::random(m_ReplayList.size())]);
			string navi="";
			navi += "�@�@�f���v���C�F";
			navi += CFile::GetPureFileNameOf(m_pPlayer->GetFileName());
			
			m_TextDemo.text->GetFont()->SetText(navi);
			m_TextDemo.Update();
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
