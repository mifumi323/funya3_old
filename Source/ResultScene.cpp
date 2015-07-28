// ResultScene.cpp: CResultScene �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResultScene.h"

#include "App.h"
#include "RandomReplay.h"
#include "ResourceManager.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CResultScene::CResultScene()
{
	m_nLines = m_nScroll = m_nFocus = 0;
	m_NavigationBar = NULL;
	m_TextNavi.dib = NULL;
	m_TextTitle = NULL;
	m_TextResult = NULL;
}

CResultScene::~CResultScene()
{
	DELETE_SAFE(m_NavigationBar);
	DELETE_SAFE(m_TextNavi.dib);
	DELETE_SAFE(m_TextTitle);
	DELETE_SAFE(m_TextResult);
}

void CResultScene::OnInit()
{
	m_TextTitle = new CTextDIB32;
	m_TextTitle->GetFont()->SetSize(32);
	m_TextTitle->GetFont()->SetColor(0xffffff);
	m_TextTitle->GetFont()->SetBackColor(0x303030);
	m_TextTitle->GetFont()->SetText("�v���C�L�^");
	m_TextTitle->UpdateText();
	m_TextResult = new CTextDIB32;
	m_TextResult->GetFont()->SetSize(16);
	m_TextResult->GetFont()->SetColor(0xffffff);
	m_TextResult->GetFont()->SetBackColor(0x303030);
	m_TextResult->GetFont()->SetWeight(FW_BOLD);
	m_TextResult->GetFont()->SetHeight(24);
	// �i�r�Q�[�V�����o�[
	m_NavigationBar = new Cf3NavigationBar;
	m_TextNavi.Set(new CTextDIB32);
	m_NavigationBar->Add(&m_TextNavi);
	// �v���C�L�^
	Add("�o�i�i�擾�����F%d��",
		(string)"���܂łɎ�����o�i�i�̐��ł��B"+
		(theSetting->m_Character==2?
		"�@�c�ȂɁH�u�h�E�����āI�H":"�R���e�B�j���[�ȂǂŌ��邱�Ƃ�����܂��B"),
		theSetting->m_Banana);
	Add("�݌v�N�����ԁ@�F"+SecToStr(theSetting->m_PlayTime),
		"�O��̃Q�[���I���܂ł̋N�����Ԃ̗݌v�ł��B����̋N�����Ԃ͎���N�����ɔ��f����܂��B");
	Add("�݌v�������ԁ@�F"+SecToStr(theSetting->m_SleepTime/40),
		"�ӂɂ�͂��΂炭�����Ă����ƐQ�܂��B���₷�݂Ȃ���");
	Add("���S�N���A�񐔁F%d��",
		"�X�e�[�W�W���Ō�܂ŃN���A�����񐔂ł��B�������X�e�[�W���Ɗy�ɑ����܂��B",
		theSetting->m_Smiles);
	if (theSetting->m_Smiles)
		Add("Secret1:���ʂ̏Ί��",
			"�L�[�ݒ聨�ɂ�����@�ǂ̃X�e�[�W�ł���������ŏI�ʂ܂ŃN���A����(���̑S�ẴV�[�N���b�g�����S�N���A���ɉ��ւ���܂�)");
	if (theSetting->m_TimeMaster)
		Add("Secret2:���Ԃ��i���",
			"�S�ʁ��Q�[���e���|�@�W���X�e�[�W���N���A");
	if (theSetting->m_Eyewitness)
		Add("Secret3:�ڌ���",
			"�S�ʁ����x���m�@�o�i�i1000�ȏ�");
	if (theSetting->m_FeatherIron)
		Add("Secret4:�H�̂悤�ɓS�̂悤��",
			"�S�ʁ��d�́@�m�[�~�X�N���A");
	if (theSetting->m_GrapeColored)
		Add("Secret5:�u�h�E�F�̂��̎q",
			"�S�ʁ��L�����N�^�[�F���񂱁@10���ȏ㖰��");
	if (theSetting->m_Esrever)
		Add("Secret6:�������",
			"�S�ʁ���ʕ\���@10���Ԉȏ�N������");
	if (theSetting->m_Outline)
		Add("Secret7:�֊s�����̂�����",
			"�S�ʁ��L�����N�^�[�F�~�X�^�[�t���[���@100��ȏ㊮�S�N���A");
	if (theSetting->m_ColdMan)
		Add("Secret8:�₽���l�I",
			"�S�ʁ��X�̑��������@�o�i�i10000�ȏ�");
	if (theSetting->m_AndBalloon)
		Add("Secret9:�����ĕ��D�̂悤��",
			"�S�ʁ��d�́F�Ȃ��@�N������100���Ԉȏ�");
//	if (theSetting->m_)
//		Add("Secret:",
//			"�@");
	m_itBegin = m_Lines.begin();
	m_nLines = m_Lines.size();
	Update();
	// ���v���C�֌W
	f3Replay.Enter();
	// ���̑�
	m_Icon = ResourceManager.Get(RID_MAIN);
}

void CResultScene::OnDraw(CDIB32 *lp)
{
	lp->Clear(0);
	if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
		f3Input.Input();
		JumpScene(FIRST_SCENE);
	}ef (f3Input.GetKeyPushed(F3KEY_UP)) {
		if (m_nFocus<=0) {
			m_nFocus = m_nLines;
		}
		m_nFocus--;
		Update();
	}ef (f3Input.GetKeyPushed(F3KEY_DOWN)) {
		m_nFocus++;
		if (m_nFocus>=m_nLines) {
			m_nFocus = 0;
		}
		Update();
	}
	f3Replay.Replay(lp);
	if (!f3Input.GetKeyPressed(F3KEY_SMILE)) {
		lp->Blt(m_TextTitle,80,32);
		RECT rc = { 320, 96, 352, 128 };
		lp->Blt(m_Icon,48,32,&rc);
		lp->Blt(m_TextResult,32,80);
		SetRect(&rc, 352+16*(m_nIcon%4), 96+16*(m_nIcon/4), 368+16*(m_nIcon%4), 112+16*(m_nIcon/4));
		lp->Blt(m_Icon,16,80+24*(m_nFocus-m_nScroll),&rc);
	}
	m_NavigationBar->OnDraw(lp);
}

void __cdecl CResultScene::Add(string summary, string description, ...)
{
	char buf[100];
	wvsprintf(buf, summary.c_str(), (LPSTR)(&description+1));
	m_Lines.push_back(strstrpair(buf, description));
}

void CResultScene::Update()
{
	m_nIcon = CApp::random(8);
	string str="";
	if (m_nLines>6) {
		if (m_nScroll>=m_nFocus) m_nScroll = m_nFocus-1;
		if (m_nScroll+5<=m_nFocus) m_nScroll = m_nFocus-4;
		Saturate(0, m_nScroll, m_nLines-6);
	}
	int i=0;
	for (strstriterator it=m_Lines.begin(); it!=m_Lines.end()&&i<m_nLines&&i<m_nScroll+6; it++, i++) {
		if (i>=m_nScroll) str += (*it).first + "\n";
		if (i==m_nFocus) {
			m_TextNavi.text->GetFont()->SetText((*it).second);
			m_TextNavi.Update();
		}
	}
	m_TextResult->GetFont()->SetText(str);
	m_TextResult->UpdateText();
}

string CResultScene::SecToStr(int sec)
{
	if (sec<=0) return "-";
	string ret = "";
	char buf[32];
    int h, m, s;
    h = sec / 3600;
    m = (sec % 3600) / 60;
    s = sec % 60;
    if (h) ret = ret + itoa(h, buf, 10) + "����";
    if (m) ret = ret + itoa(m, buf, 10) + "��";
    if (s) ret = ret + itoa(s, buf, 10) + "�b";
	return ret;
}
