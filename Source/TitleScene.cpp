// TitleScene.cpp: CTitleScene �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TitleScene.h"
#include "App.h"
#include "f3Map.h"
#include "f3StageFile.h"
#include "f3Select.h"
#include "yaneSDK/yaneDir.h"
#include "yaneSDK/yaneFile.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CTitleScene::CTitleScene()
{
	m_Map=NULL;
	m_StageFile=NULL;
	m_Select=NULL;
	m_NavigationBar=NULL;
	m_TextStageFile=NULL;
}

CTitleScene::~CTitleScene()
{
	delete m_Map;
	delete m_StageFile;
	delete m_Select;
	delete m_NavigationBar;
	delete m_TextStageFile;
	delete m_TextNavi.text;
}

void CTitleScene::OnInit()
{
	FindAllStageFile();
	m_NavigationBar = new Cf3NavigationBar;
	m_StageFile = new Cf3StageFile;
	m_Select = new Cf3Select;
	m_Select->SetPos(160-32*5/2,120+8);
	m_TextStageFile = new CTextDIB32;
	m_TextStageFile->GetFont()->SetSize(32);
	m_TextStageFile->GetFont()->SetColor(0xffffff);
	m_TextStageFile->GetFont()->SetBackColor(0x303030);
	m_TextNavi.Set(new CTextDIB32);
	m_TextNavi.text->GetFont()->SetSize(16);
	m_TextNavi.text->GetFont()->SetColor(0x000020);
	m_TextNavi.text->GetFont()->SetBackColor(0x303030);
	m_NavigationBar->Add(&m_TextNavi);
	SetStage();
	if (m_Select->GetSelected()==0) {
		SetMap(0);
	}else if (m_Select->GetSelected()==1) {
		SetMap(theSetting->GetProgress(*m_itStageFile));
	}
	theApp->GetBGM()->Play(BGMN_TITLE);
}

void CTitleScene::OnDraw(CDIB32 *lp)
{
	if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
		int next = GAME_SCENE;
		if (m_Select->GetSelected()==0) {
			theSetting->SetProgress(*m_itStageFile,0);
		}else if (m_Select->GetSelected()==1) {
			theSetting->m_Banana -= m_nPenalty;
		}else {
			next = FIRST_SCENE;
		}
		f3Input.Input();
		JumpScene(next);
	}
	bool Change = false;
	if (f3Input.GetKeyPushed(F3KEY_LEFT)) {
		if (m_itStageFile==m_StageList.begin()) m_itStageFile=m_StageList.end();
		m_itStageFile--;
		Change = true;
	}
	if (f3Input.GetKeyPushed(F3KEY_RIGHT)) {
		m_itStageFile++;
		if (m_itStageFile==m_StageList.end()) m_itStageFile=m_StageList.begin();
		Change = true;
	}
	if (Change) SetStage();
	if (Change||m_Select->Changed()) {
		if (m_Select->GetSelected()==0) {
			SetMap(0);
		}else if (m_Select->GetSelected()==1) {
			SetMap(theSetting->GetProgress(*m_itStageFile));
		}
	}
	m_Map->OnDraw(lp);
	int w,h;
	m_TextStageFile->GetSize(w,h);
	if (w<=320) lp->Blt(m_TextStageFile,160-w/2,120-24-h/2);
	else {
		SIZE sz = { 320, h*320/w };
		lp->Blt(m_TextStageFile,0,120-24-sz.cy/2,NULL, &sz);
	}
	m_NavigationBar->OnDraw(lp);
}

void CTitleScene::FindAllStageFile()
{
	CDir dir;
	string file;
	dir.SetPath(CFile::GetCurrentDir());
	dir.SetFindFile("*.f3s");
	while (!dir.FindFile(file)) {
		m_StageList.push_back(file);
	}
	m_itStageFile = m_StageList.begin();
	for (list<string>::iterator it=m_StageList.begin(); it!=m_StageList.end(); it++) {
		if ((*it)==theApp->GetStageFile()) {
			m_itStageFile = it;
			break;
		}
	}
	theApp->SetStageFile(*m_itStageFile);
}

void CTitleScene::SetStage()
{
	theApp->SetStageFile(*m_itStageFile);
	m_StageFile->Read(*m_itStageFile);
	DWORD size;
	char*str=(char*)m_StageFile->GetStageData(CT_TITL,&size);
	if (str>0 && size>0) {
		char strbuf[256]={0};	// 0�ŏ�����
		size = min(size,255);
		CopyMemory(strbuf,str,size);
		m_TextStageFile->GetFont()->SetText(strbuf);
	}else{
		m_TextStageFile->GetFont()->SetText(CFile::GetPureFileNameOf(*m_itStageFile));
	}
	m_TextStageFile->UpdateText();
	{
		string navi="";
		if (theSetting->GetProgress(*m_itStageFile)) {
			char buf[256];
			sprintf(buf,"�@�i�s�󋵁F%d�ʂ܂ŃN���A",
				theSetting->GetProgress(*m_itStageFile));
			navi+=buf;
		}
		navi+="�@�X�e�[�W���F";
		navi+=m_TextStageFile->GetFont()->GetText();
		str=(char*)m_StageFile->GetStageData(CT_AUTH,&size);
		if (str>0 && size>0) {
			char strbuf[257]={0};	// 0�ŏ�����
			size = min(size,256);
			CopyMemory(strbuf,str,size);
			navi+="�@��ҁF";
			navi+=strbuf;
		}
		str=(char*)m_StageFile->GetStageData(CT_DESC,&size);
		if (str>0 && size>0) {
			char strbuf[257]={0};	// 0�ŏ�����
			size = min(size,256);
			CopyMemory(strbuf,str,size);
			navi+="�@�R�����g�F";
			navi+=strbuf;
		}
		m_TextNavi.text->GetFont()->SetText(navi);
		m_TextNavi.Update();
	}
	m_Select->Clear();
	m_Select->Add("�͂��߂���", 0);
	BYTE*Buf = m_StageFile->GetStageData(CT_PNLT);
	m_nPenalty = (Buf?*Buf:100);
	if (theSetting->GetProgress(*m_itStageFile)&&
		m_nPenalty<=theSetting->m_Banana) {
		char strbuf[256];
		sprintf(strbuf,"��������(%d)",m_nPenalty);
		m_Select->Add(strbuf, 1);
		m_Select->Select(1);
	}
	m_Select->Add("�I���", 99);
}

void CTitleScene::SetMap(int map)
{
	DELETE_SAFE(m_Map);
	m_Map = new Cf3Map(m_StageFile,map,false);
	m_Map->OnPreDraw();
}
