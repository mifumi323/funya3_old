// ExplainScene.cpp: CExplainScene �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExplainScene.h"
#include "App.h"
#include "f3Map.h"
#include "f3StageFile.h"
#include "f3MapObjectfunya.h"
#include "f3MapObjectBanana.h"
#include "f3MapObjectNeedle.h"
#include "f3MapObjectGeasprin.h"
#include "f3MapObjectmrframe.h"
#include "f3MapObjectEelPitcher.h"
#include "f3MapObjectIce.h"
#include "f3MapObjectIceSource.h"
#include "f3MapObjectFire.h"
#include "f3MapObjectEffect.h"
#include "f3MapObjectWind.h"

#define BEGIN_SECTION	int nSectionID=0;m_nStep++;if(nSectionID++==m_nSection)
#define NEXT_SECTION	else if(nSectionID++==m_nSection)
#define FINAL_SECTION	else
#define FORWARD_SECTION	if(m_nSection>=0){m_nSection++;m_nStep=0;}
#define INIT_SECTION	if(m_nStep==1)
#define FINISH_SECTION	m_nSection=-1;m_nStep=0

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CExplainScene::CExplainScene()
{
	m_nSection = m_nStep = 0;
	m_OldGravity = theSetting->m_Gravity;
	m_OldHyper = theSetting->m_Hyper;
}

CExplainScene::~CExplainScene()
{
	theSetting->m_Gravity = m_OldGravity;
	theSetting->m_Hyper = m_OldHyper;
	delete m_Map;
//	delete m_NavigationBar;
	delete m_Text.text;
}

void CExplainScene::OnInit()
{
	Cf3MapObjectMain::SetInput(&KeyInput);
	// �i�r�Q�[�V����
//	m_NavigationBar = new Cf3NavigationBar;
	f3Navi.Clear();
	CTextDIB32* text = new CTextDIB32;
//	text->GetFont()->SetSize(16);
//	text->GetFont()->SetColor(0x000020);
//	text->GetFont()->SetBackColor(0x303030);
	m_Text.Set(text);
	f3Navi.Add(&m_Text);
	// �����p��255*255�̃}�b�v�̓������t�@�C���C���[�W�𐶐�����
	// "funya3s1"		8Byte
	// �f�[�^�T�C�Y		4Byte*2
	// �`�����N�T�C�Y	4Byte
	// �`�����NID		4Byte
	// �}�b�v�T�C�Y		1Byte*2
	// �}�b�v�f�[�^		1Byte*255*255
	BYTE*pFileImage = (BYTE*)::GlobalAlloc(GMEM_FIXED | GMEM_NOCOMPACT | GMEM_ZEROINIT,8+4*2+4+4+1*2+1*255*255);
	strcpy((char*)pFileImage,"funya3s1");
	*(DWORD*)(&pFileImage[8]) = *(DWORD*)(&pFileImage[12]) = 4+4+1*2+1*255*255;
	*(DWORD*)(&pFileImage[16]) = 1*2+1*255*255;
	strcpy((char*)pFileImage+20,"M100");
	pFileImage[24] = pFileImage[25] = 255;
	pFileImage[26+127] = 0xf1;
	pFileImage[26+253*255+127] = 0xf0;
	pFileImage[26+254*255+127] = 0x01;
	char fn[256];
	::wsprintf(fn,"!%x,%x",pFileImage,8+4*2+4+4+1*2+1*255*255);
	Cf3StageFile stagefile;
	stagefile.Read(fn);
	::GlobalFree(pFileImage);
	m_Map = new Cf3Map(&stagefile,0);
	// �ӂɂ�ȊO�͋�����I�I
	int x,y;
	m_Map->m_MainChara->GetCPos(x,y);
	delete m_Map->m_MainChara;
	m_Map->m_MainChara = m_funya = new Cf3MapObjectfunya(x,y);
	theSetting->m_Gravity = 0;
	theSetting->m_Hyper = 0;
	theApp->GetBGM()->Play(BGMN_EXPLAIN);
}

void CExplainScene::OnDraw(CDIB32 *lp)
{
	m_Map->OnMove();
	int x,y,i,j;
	m_funya->GetCPos(x, y);
	BEGIN_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�ł͎n�߂܂��傤(%s�Ŏ���)", f3Input.GetKeyName(F3KEY_PAUSE));
			m_Text.Update();
		}
		m_Map->SetMapData(1, x-10, 254, 0);
		m_Map->SetMapData(1, x-1, 254, 1);
		m_Map->SetMapData(1, x+1, 254, 1);
		m_Map->SetMapData(1, x+10, 254, 0);
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("%s�A%s�ő���܂�", f3Input.GetKeyName(F3KEY_LEFT), f3Input.GetKeyName(F3KEY_RIGHT));
			m_Text.Update();
			m_nCounter = 0;
		}
		m_Map->SetMapData(1, x-10, 254, 0);
		m_Map->SetMapData(1, x-1, 254, 1);
		m_Map->SetMapData(1, x+1, 254, 1);
		m_Map->SetMapData(1, x+10, 254, 0);
		if (m_funya->m_State==Cf3MapObjectfunya::RUNNING){
			if (++m_nCounter>=40) FORWARD_SECTION;
		}else {
			m_nCounter = 0;
		}
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("����������Ղ�ł���(%s�Ŏ���)", f3Input.GetKeyName(F3KEY_PAUSE));
			m_Text.Update();
		}
		m_Map->SetMapData(1, x-10, 254, 0);
		m_Map->SetMapData(1, x-1, 254, 1);
		m_Map->SetMapData(1, x+1, 254, 1);
		m_Map->SetMapData(1, x+10, 254, 0);
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("%s�ł��Ⴊ�݁A���̂܂�%s�A%s�ŕ����܂�", f3Input.GetKeyName(F3KEY_DOWN), f3Input.GetKeyName(F3KEY_LEFT), f3Input.GetKeyName(F3KEY_RIGHT));
			m_Text.Update();
			m_nCounter = 0;
		}
		m_Map->SetMapData(1, x-10, 254, 0);
		m_Map->SetMapData(1, x-1, 254, 1);
		m_Map->SetMapData(1, x+1, 254, 1);
		m_Map->SetMapData(1, x+10, 254, 0);
		if (m_funya->m_State==Cf3MapObjectfunya::WALKING&&m_funya->m_DX!=0){
			if (++m_nCounter>=40) FORWARD_SECTION;
		}else {
			m_nCounter = 0;
		}
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�����������Ղ�ł���(%s�Ŏ���)", f3Input.GetKeyName(F3KEY_PAUSE));
			m_Text.Update();
		}
		m_Map->SetMapData(1, x-10, 254, 0);
		m_Map->SetMapData(1, x-1, 254, 1);
		m_Map->SetMapData(1, x+1, 254, 1);
		m_Map->SetMapData(1, x+10, 254, 0);
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("%s�ŃW�����v�I�����ė����ƃW�����v���܂�", f3Input.GetKeyName(F3KEY_JUMP));
			m_Text.Update();
			m_nCounter = 0;
		}
		m_Map->SetMapData(1, x-10, 254, 0);
		m_Map->SetMapData(1, x-1, 254, 1);
		m_Map->SetMapData(1, x+1, 254, 1);
		m_Map->SetMapData(1, x+10, 254, 0);
		if (m_funya->m_State==Cf3MapObjectfunya::CHARGING) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�i��������Ă݂܂��傤(%s�������ŏ��W�����v)", f3Input.GetKeyName(F3KEY_JUMP));
			m_Text.Update();
			m_Map->SetMapData(1, x-1, 254, 1);
			m_Map->SetMapData(1, x+1, 254, 1);
			m_Map->SetMapData(1, x-2, 253, 1);
			m_Map->SetMapData(1, x+2, 253, 1);
		}
		m_Map->SetMapData(1, x-10, 253, 0);
		m_Map->SetMapData(1, x-3, 253, 1);
		m_Map->SetMapData(1, x+3, 253, 1);
		m_Map->SetMapData(1, x+10, 253, 0);
		if (m_funya->m_State==Cf3MapObjectfunya::STANDING&&y==252) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("%s�����Z�������Ē��W�����v", f3Input.GetKeyName(F3KEY_JUMP));
			m_Text.Update();
			m_Map->SetMapData(1, x-1, 253, 1);
			m_Map->SetMapData(1, x+1, 253, 1);
			m_Map->SetMapData(1, x-2, 252, 1);
			m_Map->SetMapData(1, x+2, 252, 1);
			m_Map->SetMapData(1, x-2, 251, 1);
			m_Map->SetMapData(1, x+2, 251, 1);
		}
		m_Map->SetMapData(1, x-10, 251, 0);
		m_Map->SetMapData(1, x-3, 251, 1);
		m_Map->SetMapData(1, x+3, 251, 1);
		m_Map->SetMapData(1, x+10, 251, 0);
		if (m_funya->m_State==Cf3MapObjectfunya::STANDING&&y==250) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("%s�������ɗ����đ�W�����v�I(%s�ŏ�������܂�)", f3Input.GetKeyName(F3KEY_JUMP), f3Input.GetKeyName(F3KEY_UP));
			m_Text.Update();
			m_Map->SetMapData(1, x-1, 251, 1);
			m_Map->SetMapData(1, x+1, 251, 1);
			m_Map->SetMapData(1, x-2, 250, 1);
			m_Map->SetMapData(1, x+2, 250, 1);
			m_Map->SetMapData(1, x-2, 249, 1);
			m_Map->SetMapData(1, x+2, 249, 1);
			m_Map->SetMapData(1, x-2, 248, 1);
			m_Map->SetMapData(1, x+2, 248, 1);
		}
		m_Map->SetMapData(1, x-10, 248, 0);
		m_Map->SetMapData(1, x-3, 248, 1);
		m_Map->SetMapData(1, x+3, 248, 1);
		m_Map->SetMapData(1, x+10, 248, 0);
		if (m_funya->m_State==Cf3MapObjectfunya::STANDING&&y==247) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("���ł܂���(%s�Ŏ���)", f3Input.GetKeyName(F3KEY_PAUSE));
			m_Text.Update();
		}
		m_Map->SetMapData(1, x-10, 248, 0);
		m_Map->SetMapData(1, x-3, 248, 1);
		m_Map->SetMapData(1, x+3, 248, 1);
		m_Map->SetMapData(1, x+10, 248, 0);
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�o�i�i��S�����ƃQ�[���N���A�ł�");
			m_Text.Update();
			if (x>=m_Map->m_Width[1]-2) {
				new Cf3MapObjectBanana(x-2,244);
			}ef(x<2){
				new Cf3MapObjectBanana(x+2,244);
			}else {
				new Cf3MapObjectBanana(x-2+CApp::random(2)*4,244);
			}
		}
		m_Map->SetMapData(1, x-10, 248, 0);
		m_Map->SetMapData(1, x-3, 248, 1);
		m_Map->SetMapData(1, x+3, 248, 1);
		m_Map->SetMapData(1, x+10, 248, 0);
		if (m_funya->m_State==Cf3MapObjectfunya::SMILING){
			m_Map->m_nGotBanana--;
			FORWARD_SECTION;
		}
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�Ƃ肠�������߂łƂ�(%s�Ŏ���)", f3Input.GetKeyName(F3KEY_PAUSE));
			m_Text.Update();
		}
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�Ƃ��Ȃǂ̊댯���ɓ�����ƃ~�X�ł�");
			m_Text.Update();
			m_Map->SetMapData(1, x, 248, 2);
			m_funya->Fall();
		}
		m_Map->SetMapData(1, x-10, 248, 0);
		m_Map->SetMapData(1, x-1, 248, 2);
		m_Map->SetMapData(1, x+1, 248, 2);
		m_Map->SetMapData(1, x+10, 248, 0);
		if (m_funya->m_State==Cf3MapObjectfunya::DEAD) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�h���ꂿ�Ⴂ�܂���(%s�Ŏ���)", f3Input.GetKeyName(F3KEY_PAUSE));
			m_Text.Update();
		}
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("��ʉ��ɗ����Ă��~�X�ł�");
			m_Text.Update();
			m_Map->SetMapData(1, x, y, 0);
			m_funya->Fall();
		}
		m_Map->SetMapData(1, x-1, y  , 0);
		m_Map->SetMapData(1, x-1, y+1, 0);
		m_Map->SetMapData(1, x  , y+1, 0);
		m_Map->SetMapData(1, x+1, y+1, 0);
		m_Map->SetMapData(1, x+1, y  , 0);
		if (m_funya->m_State==Cf3MapObjectfunya::DEAD) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�������Ⴂ�܂���(%s�Ŏ���)", f3Input.GetKeyName(F3KEY_PAUSE));
			m_Text.Update();
		}
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�d�|���̐������s���̂ŏ���Ă�������");
			m_Text.Update();
			m_funya->m_Y = 32*255-1;
			m_funya->HighJump();
		}
		m_Map->SetMapData(1, x-1, y-1, 0);
		m_Map->SetMapData(1, x  , y-1, 0);
		m_Map->SetMapData(1, x+1, y-1, 0);
		if (m_funya->m_DY>0) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			for (i=-3; i<=3; i++) m_Map->SetMapData(1, x+i, 254, 1);
		}
		m_Map->SetMapData(1, x-5, 254, 0);
		m_Map->SetMapData(1, x-4, 254, 1);
		m_Map->SetMapData(1, x+4, 254, 1);
		m_Map->SetMapData(1, x+5, 254, 0);
		if (m_funya->m_State==Cf3MapObjectfunya::STANDING) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�M���o�l�ł��B�G��ƒe����܂�");
			m_Text.Update();
			for (j=247; j<=253; j++) {
				m_Map->SetMapData(1, x-5, j, 1);
				for (i=-4; i<=4; i++) m_Map->SetMapData(1, x+i, j, 0);
				m_Map->SetMapData(1, x+5, j, 1);
			}
			for (j=243; j<=246; j++) {
				m_Map->SetMapData(1, x-6, j, 1);
				for (i=-5; i<=5; i++) m_Map->SetMapData(1, x+i, j, 0);
				m_Map->SetMapData(1, x+6, j, 1);
			}
			new Cf3MapObjectBanana(x,243);
			new Cf3MapObjectGeasprin(CApp::random(max(x-4,0),min(x+4,254)), 246);
		}
		if (m_funya->m_State==Cf3MapObjectfunya::SMILING){
			m_Map->m_nGotBanana--;
			FORWARD_SECTION;
		}
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�������ɏ��܂���(%s�Ŏ���)", f3Input.GetKeyName(F3KEY_PAUSE));
			m_Text.Update();
		}
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�E�i�M�J�Y���ł��B�������܂�");
			m_Text.Update();
			for (j=239; j<=242; j++) {
				for (i=-4; i<=4; i++) new Cf3MapObjectEelPitcher(x+i, j);
			}
			new Cf3MapObjectBanana(x,253);
			m_funya->Fall();
		}
		if (m_funya->m_State==Cf3MapObjectfunya::SMILING){
			m_Map->m_nGotBanana--;
			FORWARD_SECTION;
		}
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�M���o�l���H�ׂ��Ⴂ�܂�(%s�Ŏ���)", f3Input.GetKeyName(F3KEY_PAUSE));
			m_Text.Update();
		}
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�A�C�X�]�[���Ńp���[�A�b�v����%s�ŃE�i�M�J�Y���𓀂点�悤", f3Input.GetKeyName(F3KEY_ATTACK));
			m_Text.Update();
			new Cf3MapObjectIceSource(CApp::random(max(x-4,0),min(x+4,254)),251);
			new Cf3MapObjectBanana(x,246);
			m_funya->Fall();
		}
		if (m_funya->m_State==Cf3MapObjectfunya::SMILING){
			m_Map->m_nGotBanana--;
			FORWARD_SECTION;
		}
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�M���o�l���H�ׂ��Ⴂ�܂�(%s�Ŏ���)", f3Input.GetKeyName(F3KEY_PAUSE));
			m_Text.Update();
		}
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�Ƃ��Ƃ��͗����Ă��Ċ댯�ł�");
			m_Text.Update();
			for (i=-4; i<=4; i++) new Cf3MapObjectNeedle(x+i, 243);
			new Cf3MapObjectGeasprin(x-5, 246, DIR_RIGHT);
			new Cf3MapObjectGeasprin(x+5, 246, DIR_LEFT);
			m_funya->Fall();
		}
		if (m_funya->m_State==Cf3MapObjectfunya::DEAD||y==253) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�Ƃ��Ƃ��͗����Ă��Ċ댯�ł�(%s�Ŏ���)", f3Input.GetKeyName(F3KEY_PAUSE));
			m_Text.Update();
		}
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)) FORWARD_SECTION;
	}
	NEXT_SECTION{
		INIT_SECTION{
			m_Text.text->GetFont()->SetText("�ł̓o�i�i������ďI���ɂ��܂��傤");
			m_Text.Update();
			new Cf3MapObjectBanana(x,y-1);
			m_funya->Fall();
			m_nCounter = 0;
		}
		if (m_funya->m_State==Cf3MapObjectfunya::SMILING||
			m_funya->m_State==Cf3MapObjectfunya::DEAD||
			++m_nCounter>=2400){
			m_nCounter = 0;
			m_Map->m_nGotBanana--;
			FORWARD_SECTION;
		}
	}
	FINAL_SECTION{
		INIT_SECTION{
			if (m_funya->m_State==Cf3MapObjectfunya::SMILING) {
				m_Text.text->GetFont()->SetText("�����l�ł���(%s�ŏI��)", f3Input.GetKeyName(F3KEY_PAUSE));
			}ef (m_funya->m_State==Cf3MapObjectfunya::DEAD) {
				m_Text.text->GetFont()->SetText("�����Ŏ��ʐl�������ł���(%s�ŏI��)", f3Input.GetKeyName(F3KEY_PAUSE));
			}else {
				m_Text.text->GetFont()->SetText("���Ԑ؂�ł�(%s�ŏI��)", f3Input.GetKeyName(F3KEY_PAUSE));
			}
			m_Text.Update();
		}
		if (f3Input.GetKeyPushed(F3KEY_PAUSE)){
			f3Input.Input();
			JumpScene(FIRST_SCENE);
		}
	}
	m_Map->OnPreDraw();
	m_Map->OnDraw(lp);
	f3Navi.OnDraw(lp);
}
