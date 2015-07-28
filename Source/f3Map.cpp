// f3Map.cpp: Cf3Map �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3Map.h"
#include "yaneSDK/yaneDIB32.h"
#include "yaneSDK/yaneDir.h"
#include "yaneSDK/yaneFile.h"
#include "yaneSDK/yanePlaneEffectBlt.h"
#include "f3StageFile.h"
#include "f3MapObjectMain.h"
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

BYTE Cf3Map::m_defHit[240] = {
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
	0x00,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
};
float Cf3Map::m_Friction[8] = {
	0.10f,0.00f,0.02f,0.05f,0.07f,0.15f,0.20f,1.00f,
};
int Cf3Map::m_nEffect=0;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

Cf3Map::Cf3Map(Cf3StageFile*lp,int stage,bool playable)
{
	BYTE* buf;
	DWORD s;
	DWORD bgm[BGMN_SIZE] = {0};
	string ChipFile = "";
	m_pDIBBuf = new CDIB32;
	m_pDIBBuf->CreateSurface(320,240);
	m_Stage = stage;
	m_bPlayable = playable;
	Cf3MapObjectBase::SetParent(this);
	m_nGotBanana = m_nTotalBanana = 0;
	m_Wind = NULL;
	// �L����
	m_MainChara = NULL;
	// �^�C�g��
	m_Title = "";
	if ((buf = lp->GetStageData(GetChunkType(CT_TL00,stage),&s))!=NULL) {
		char tl[256];
		CopyMemory(tl,buf,s);
		tl[s]='\0';
		m_Title = tl;
	}
	// �}�b�v�`�b�v
	m_MapChip[0] = ReadMapChip(lp, 0);
	m_MapChip[1] = ReadMapChip(lp, 1);
	m_MapChip[2] = ReadMapChip(lp, 2);
/*	m_MapChip[0]->Load("resource/Cave.bmp",false);
	m_MapChip[1]->Load("resource/Cave.bmp",false);
	m_MapChip[2]->Load("resource/Cave.bmp",false);
	if ((buf = lp->GetStageData(0x3046434D,&s))!=NULL) {
		char fn[256];
		CopyMemory(fn,buf,s);
		fn[s]='\0';
		if ((buf = lp->GetStageData(0x3044434D,&s))!=NULL) {
			// ����Ƀt�@�C�������ւ����鋰�ꂪ���邽�߃X�e�[�W�����f�[�^��D��
			char fn2[256];
			::wsprintf(fn2,"!%x,%x",buf,s);
			if (m_MapChip[0]->Load(fn2,false)) m_MapChip[0]->Load(fn,false);
		}else{
			m_MapChip[0]->Load(fn,false);
		}
	}
	if ((buf = lp->GetStageData(0x3146434D,&s))!=NULL) {
		char fn[256];
		CopyMemory(fn,buf,s);
		fn[s]='\0';
		ChipFile = fn;
		if ((buf = lp->GetStageData(0x3144434D,&s))!=NULL) {
			// ����Ƀt�@�C�������ւ����鋰�ꂪ���邽�߃X�e�[�W�����f�[�^��D��
			char fn2[256];
			::wsprintf(fn2,"!%x,%x",buf,s);
			if (m_MapChip[1]->Load(fn2,false)) m_MapChip[1]->Load(fn,false);
		}else{
			m_MapChip[1]->Load(fn,false);
		}
	}
	if ((buf = lp->GetStageData(0x3246434D,&s))!=NULL) {
		char fn[256];
		CopyMemory(fn,buf,s);
		fn[s]='\0';
		if ((buf = lp->GetStageData(0x3244434D,&s))!=NULL) {
			// ����Ƀt�@�C�������ւ����鋰�ꂪ���邽�߃X�e�[�W�����f�[�^��D��
			char fn2[256];
			::wsprintf(fn2,"!%x,%x",buf,s);
			if (m_MapChip[2]->Load(fn2,false)) m_MapChip[2]->Load(fn,false);
		}else{
			m_MapChip[2]->Load(fn,false);
		}
	}*/
	// �����蔻��
	CopyMemory(m_Hit,m_defHit,240);
	if ((buf = lp->GetStageData(CT_HITS,&s))!=NULL) {
		if (s>240) s=240;
		CopyMemory(m_Hit,buf,s);
	}ef(CDir().IsFileExist(ChipFile+".f3h")) {
		CFile hitfile;
		if (hitfile.Read(ChipFile+".f3h")==0) {
			CopyMemory(m_Hit,hitfile.GetMemory(),min(hitfile.GetSize(),240));
		}
		// hitfile.Close(); �͕s�v�Ȃ͂�
	}
	// �}�b�v�f�[�^
	if ((buf = lp->GetStageData(GetChunkType(CT_M000,stage),&s))!=NULL) {
		m_Width[0] = *buf;
		m_Height[0] = *(buf+1);
		m_MapData[0] = new BYTE[m_Width[0]*m_Height[0]];
		CopyMemory(m_MapData[0],buf+2,m_Width[0]*m_Height[0]);
	}else{
		m_MapData[0] = NULL;
	}
	if ((buf = lp->GetStageData(GetChunkType(CT_M100,stage),&s))!=NULL) {
		m_Width[1] = *buf;
		m_Height[1] = *(buf+1);
		DWORD stagesize = m_Width[1]*m_Height[1];
		m_MapData[1] = new BYTE[stagesize];
		m_Wind = new float[stagesize];
		BYTE *windmap = new BYTE[stagesize];
		CopyMemory(m_MapData[1],buf+2,stagesize);
		int x,y,z,n;
		z=0;
		for (y=0; y<m_Height[1]; y++) {
			for (x=0; x<m_Width[1]; x++) {
				windmap[z]=0;
				n=m_MapData[1][z];
				if (n>=0xf0) {
					if (n==0xf0){	// ��l��
						if (m_MainChara==NULL) m_MainChara = Cf3MapObjectMain::Create(x,y);
						bgm[BGMN_GAMEFUNYA]+=99;
					}ef(n==0xf1){	// �o�i�i
						new Cf3MapObjectBanana(x,y);
						bgm[BGMN_GAMEBANANA]+=1;
						m_nTotalBanana++;
					}ef(n==0xf2){	// �Ƃ��Ƃ�
						new Cf3MapObjectNeedle(x,y);
						bgm[BGMN_GAMENEEDLE]+=4;
					}ef(n==0xf3){	// �M���o�l������
						new Cf3MapObjectGeasprin(x,y,DIR_LEFT);
						bgm[BGMN_GAMEGEASPRIN]+=10;
					}ef(n==0xf4){	// �M���o�l�E����
						new Cf3MapObjectGeasprin(x,y,DIR_RIGHT);
						bgm[BGMN_GAMEGEASPRIN]+=10;
					}ef(n==0xf5){	// ���X�g�b�v
						windmap[z]=0xC;
					}ef(n==0xf6){	// ��������
						windmap[z]=0x1;
						bgm[BGMN_GAMEWIND]+=1;
					}ef(n==0xf7){	// ���E����
						windmap[z]=0x2;
						bgm[BGMN_GAMEWIND]+=1;
					}ef(n==0xf8){	// �~�X�^�[�E�t���[��
						new Cf3MapObjectmrframe(x,y);
						bgm[BGMN_GAMEMRFRAME]+=40;
					}ef(n==0xf9){	// �E�i�M�J�Y��
						new Cf3MapObjectEelPitcher(x,y);
						bgm[BGMN_GAMEEELPITCHER]+=5;
					}ef(n==0xfa){	// �X
						new Cf3MapObjectIceSource(x,y);
						bgm[BGMN_GAMEICE]+=8;
					}ef(n==0xfb){	// ��
						new Cf3MapObjectFire(x,y);
						bgm[BGMN_GAMEFIRE]+=8;
					}ef(n==0xfc){	// �Ƃ��Ƃ�
						new Cf3MapObjectNeedle(x,y,1);
						bgm[BGMN_GAMENEEDLE]+=4;
					}ef(n==0xfd){	// �Ƃ��Ƃ�
						new Cf3MapObjectNeedle(x,y,2);
						bgm[BGMN_GAMENEEDLE]+=4;
					}ef(n==0xfe){	// �Ƃ��Ƃ�
						new Cf3MapObjectNeedle(x,y,3);
						bgm[BGMN_GAMENEEDLE]+=4;
					}
					m_MapData[1][z] = 0;
				}else{
					if (GetHit(x,y,HIT_LEFT)) windmap[z]=0x4; else windmap[z] = 0;
					if (GetHit(x,y,HIT_RIGHT)) windmap[z]|=0x8;
				}
				z++;
			}
		}
		z=0;
		float wind;
		int dx;
		for (y=0; y<m_Height[1]; y++) {
			for (x=0; x<m_Width[1]; x++) {
				if (windmap[z]&0x10) {
				}else{
					wind=0.0f;
					for (dx=0; x+dx<m_Width[1]; dx++) {
						if (windmap[z+dx]&0x4) break;
						if (windmap[z+dx]&0x1) wind-=1.0f;
						if (windmap[z+dx]&0x2) wind+=1.0f;
						windmap[z+dx]|=0x10;
						if (windmap[z+dx]&0x8) break;
					}
					if (IsIn(-0.01f,wind,0.01f)) {
						wind = 0.0f;
					}else{
						new Cf3MapObjectWind(x,y,dx,wind);
					}
				}
				m_Wind[z] = wind;
				z++;
			}
		}
		DELETEPTR_SAFE(windmap);
	}else{
		m_MapData[1] = NULL;
	}
	if ((buf = lp->GetStageData(GetChunkType(CT_M200,stage),&s))!=NULL) {
		m_Width[2] = *buf;
		m_Height[2] = *(buf+1);
		m_MapData[2] = new BYTE[m_Width[2]*m_Height[2]];
		CopyMemory(m_MapData[2],buf+2,m_Width[2]*m_Height[2]);
	}else{
		m_MapData[2] = NULL;
	}
	// BGM
	DWORD bgmm = 0;
	m_BGMNumber = BGMN_SIRENT;
	for (int i=BGMN_SIRENT; i<BGMN_SIZE; i++) {
		if (bgmm<bgm[i]) {
			bgmm = bgm[i];
			m_BGMNumber = (BGMNumber)i;
		}
	}
	m_ScrollX = m_ScrollY = 0;
	if (m_MainChara!=NULL) m_MainChara->GetPos(m_ScrollRX, m_ScrollRY);
}

Cf3Map::~Cf3Map()
{
	KillAllMapObject();
	GarbageMapObject();
	DELETEPTR_SAFE(m_Wind);
	DELETEPTR_SAFE(m_MapData[2]);
	DELETEPTR_SAFE(m_MapData[1]);
	DELETEPTR_SAFE(m_MapData[0]);
	DELETE_SAFE(m_MapChip[2]);
	DELETE_SAFE(m_MapChip[1]);
	DELETE_SAFE(m_MapChip[0]);
	DELETE_SAFE(m_pDIBBuf);
}

void Cf3Map::OnDraw(CDIB32 *lp)
{
	int x,y,z;
	int vx,vy;
	int sx, sy, ex, ey;
	RECT r;
	lp->Clear(0);
	if (m_MapData[0]) {
		float mx = 1;
		if (m_Width[1]-10>0) mx = (float)(m_Width[0]-10)/(float)(m_Width[1]-10);
		float my = 1;
		if (m_Height[1]-7>0) my = (float)(m_Height[0]-7)/(float)(m_Height[1]-7);
		sx = 0; sy = 0;
		GetViewPos(sx,sy,mx,my);
		sx = (-sx)>>5; sy = (-sy)>>5;
		ex = sx+320/32; ey = sy+224/32;
		Saturate(sx,ex,m_Width[0]-1);
		Saturate(sy,ey,m_Height[0]-1);
		for (y=sy; y<=ey; y++) {
			for (x=sx; x<=ex; x++) {
				z = y*m_Width[0]+x;
				r.left = (m_MapData[0][z]&0xf)*32;
				r.top = (m_MapData[0][z]>>4)*32;
				r.bottom = r.top + 32;
				r.right = r.left+32;
				vx = x*32; vy = y*32;
				GetViewPos(vx,vy,mx,my);
				lp->BltFast(m_MapChip[0],vx,vy,&r);
			}
		}
	}
	if (m_MapData[1]) {
		sx = 0; sy = 0;
		GetViewPos(sx,sy);
		sx = (-sx)>>5; sy = (-sy)>>5;
		ex = sx+320/32; ey = sy+224/32;
		Saturate(sx,ex,m_Width[1]-1);
		Saturate(sy,ey,m_Height[1]-1);
		for (y=sy; y<=ey; y++) {
			for (x=sx; x<=ex; x++) {
				z = y*m_Width[1]+x;
				r.left = (m_MapData[1][z]&0xf)*32;
				r.top = (m_MapData[1][z]>>4)*32;
				r.bottom = r.top + 32;
				r.right = r.left+32;
				vx = x*32; vy = y*32;
				GetViewPos(vx,vy);
				if (m_MapData[0]) lp->Blt(m_MapChip[1],vx,vy,&r);
				else lp->BltFast(m_MapChip[1],vx,vy,&r);
			}
		}
	}
	Cf3MapObjectBanana::OnDrawAll(lp);
	Cf3MapObjectmrframe::OnDrawAll(lp);
	if (m_MainChara != NULL) m_MainChara->OnDraw(lp);
	Cf3MapObjectGeasprin::OnDrawAll(lp);
	Cf3MapObjectNeedle::OnDrawAll(lp);
	Cf3MapObjectEelPitcher::OnDrawAll(lp);
	Cf3MapObjectIceSource::OnDrawAll(lp);
	Cf3MapObjectFire::OnDrawAll(lp);
	Cf3MapObjectIce::OnDrawAll(lp);
	Cf3MapObjectEffect::OnDrawAll(lp);
	Cf3MapObjectWind::OnDrawAll(lp);
	if (m_MapData[2]) {
		float mx = 1;
		if (m_Width[1]-10>0) mx = (float)(m_Width[2]-10)/(m_Width[1]-10);
		float my = 1;
		if (m_Height[1]-7>0) my = (float)(m_Height[2]-7)/(m_Height[1]-7);
		sx = 0; sy = 0;
		GetViewPos(sx,sy,mx,my);
		sx = (-sx)>>5; sy = (-sy)>>5;
		ex = sx+320/32; ey = sy+224/32;
		Saturate(sx,ex,m_Width[2]-1);
		Saturate(sy,ey,m_Height[2]-1);
		for (y=sy; y<=ey; y++) {
			for (x=sx; x<=ex; x++) {
				z = y*m_Width[2]+x;
				r.left = (m_MapData[2][z]&0xf)*32;
				r.top = (m_MapData[2][z]>>4)*32;
				r.bottom = r.top + 32;
				r.right = r.left+32;
				vx = x*32*mx; vy = y*32*my;
				GetViewPos(vx,vy,mx,my);
				lp->Blt(m_MapChip[2],vx,vy,&r);
			}
		}
	}
	CDIB32 *lpSrc=lp, *lpDst=m_pDIBBuf;
	if (m_nEffect&1) {
		CPlaneTransBlt::MirrorBlt1(lpDst, lpSrc,0,0,128);
		swap(lpSrc, lpDst);
	}
	if (m_nEffect&2) {
		CPlaneTransBlt::MirrorBlt2(lpDst, lpSrc,0,0,128);
		RECT rc={0,16,320,240};
		lpSrc->BltFast(lpDst,0,0,&rc);
	}
	if (m_nEffect&4) {
		CPlaneTransBlt::FlushBlt1(lpDst, lpSrc,0,0,128);
		swap(lpSrc, lpDst);
	}
	if (lpDst==lp)  lpDst->BltFast(lpSrc,0,0);
}

bool Cf3Map::GetHit(int x, int y, BYTE hit)
{
	if (x<0 || m_Width[1]<=x) return (0x0f&hit)!=0;
	if (y<0) return GetHit(x,0,hit);
	if (y>=m_Height[1]) return GetHit(x,m_Height[1]-1,hit);
	return (m_Hit[GetMapData(1,x,y)]&hit)!=0;
}

BYTE Cf3Map::GetMapData(int level, int x, int y)
{
	if (level<0 || 2<level || x<0 || m_Width[level]<=x || y<0 || m_Height[level]<=y) return 0;
	return m_MapData[level][x+y*m_Width[level]];
}

void Cf3Map::OnMove()
{
	if (m_MainChara != NULL) m_MainChara->OnMove();
	Cf3MapObjectEelPitcher::OnMoveAll();
	Cf3MapObjectGeasprin::OnMoveAll();
	Cf3MapObjectmrframe::OnMoveAll();
	Cf3MapObjectNeedle::OnMoveAll();
	Cf3MapObjectIce::OnMoveAll();
	Cf3MapObjectFire::OnMoveAll();
	if (m_MainChara != NULL) m_MainChara->Synergy();
	Cf3MapObjectBanana::SynergyAll();
	Cf3MapObjectEelPitcher::SynergyAll();
	Cf3MapObjectGeasprin::SynergyAll();
	Cf3MapObjectmrframe::SynergyAll();
	Cf3MapObjectNeedle::SynergyAll();
	Cf3MapObjectIce::SynergyAll();
	Cf3MapObjectFire::SynergyAll();
}

void Cf3Map::GetViewPos(int &x, int &y, float scrollx,  float scrolly)
{
	int offx = m_ScrollRX-320/2, offy = m_ScrollRY-224/2-2;
	Saturate(0,offx,m_Width[1]*32-320);
	Saturate(0,offy,m_Height[1]*32-224);
	x -=(int)(offx*scrollx); y -= (int)(offy*scrolly);
}

float Cf3Map::GetFriction(int x, int y)
{
	if (x<0 || m_Width[1]<=x || y<0 || m_Height[1]<=y) return 0.0f;
	return m_Friction[m_Hit[GetMapData(1,x,y)]>>5];
}

void Cf3Map::OnPreDraw()
{
	if (m_MainChara != NULL) {
		m_MainChara->OnPreDraw();
	}
	Cf3MapObjectBanana::OnPreDrawAll();
	Cf3MapObjectEelPitcher::OnPreDrawAll();
	Cf3MapObjectGeasprin::OnPreDrawAll();
	Cf3MapObjectmrframe::OnPreDrawAll();
	Cf3MapObjectNeedle::OnPreDrawAll();
	Cf3MapObjectIce::OnPreDrawAll();
	Cf3MapObjectIceSource::OnPreDrawAll();
	Cf3MapObjectFire::OnPreDrawAll();
	Cf3MapObjectEffect::OnPreDrawAll();
	Cf3MapObjectWind::OnPreDrawAll();
	if (m_MainChara!=NULL) m_MainChara->GetViewPos(m_ScrollX,m_ScrollY);
	m_ScrollRX = (m_ScrollRX+m_ScrollX)/2;
	m_ScrollRY = (m_ScrollRY+m_ScrollY)/2;
}

void Cf3Map::GarbageMapObject()
{
	if (m_MainChara!=NULL&&!m_MainChara->IsValid()){
		m_MainChara=NULL;
	}
	Cf3MapObjectBase::Garbage();
}

void Cf3Map::KillAllMapObject()
{
	Cf3MapObjectBase::KillAll();
}

bool Cf3Map::IsMainCharaDied()
{
	return m_MainChara!=NULL&&m_MainChara->IsDied();
}

long Cf3Map::GetChunkType(long type, int stage)
{
	long r=stage&0xf,l=stage&0xf0;
	if (r>=0xa) r+=0x7;
	if (l>=0xa0) l+=0x70;
	return type + (r<<24) + (l<<12);
}

float Cf3Map::GetWind(int x, int y)
{
	if (m_Wind==NULL || x<0 || m_Width[1]<=x || y<0 || m_Height[1]<=y) return 0.0f;
	return m_Wind[x+y*m_Width[1]];
}

void Cf3Map::CreateTemparatureMap(CDIB32 *dib)
{
	float objX, objY, dX, dY, fX, fY, power=0.0f;
	DWORD*pixel=dib->GetPtr();
	float offx = m_ScrollRX-320/2, offy = m_ScrollRY-224/2-2;
	Saturate(0.0f,offx,m_Width[1]*32-320.0f);
	Saturate(0.0f,offy,m_Height[1]*32-224.0f);
	for (int y=0; y<224; y++) {
		for (int x=0; x<320; x++) {
			fX=x+offx; fY=y+offy;	// GetViewPos�ƃI�t�Z�b�g�̊|�������t
			power = 0.0f;
			// �X�]�[��
			for(set<Cf3MapObjectIceSource*>::iterator is = Cf3MapObjectIceSource::IteratorBegin();
			is!=Cf3MapObjectIceSource::IteratorEnd();is++){
//			if ((*is)->IsValid()) {	// IsValid==false�ɂȂ邱�Ƃ͂Ȃ�(�Ǝv��)
					(*is)->GetPos(objX,objY);
					dX = objX-fX; dY = objY-fY;
					power += 1.0f/(dX*dX+dY*dY);
//				}
			}
			// ���]�[��
			for(set<Cf3MapObjectFire*>::iterator fr = Cf3MapObjectFire::IteratorBegin();
			fr!=Cf3MapObjectFire::IteratorEnd();fr++){
				if (/*(*fr)->IsValid()&&*/(*fr)->IsActive()) {	// IsValid==false�ɂȂ邱�Ƃ͂Ȃ�(�Ǝv��)
					(*fr)->GetPos(objX,objY);
					dX = objX-fX; dY = objY-fY;
					power -= 1.0f/(dX*dX+dY*dY);
				}
			}
			if (power>1.0f/256.0f) {
				// ������]�[��
				*pixel = 0x008080;
			}ef(power>1.0f/4096.0f) {
				// �p���[�A�b�v�]�[��
				*pixel = 0x00ffff;
			}ef(power<-1.0f/256.0f) {
				// �v���]�[��
				*pixel = 0x800000;
			}ef(power<-1.0f/4096.0f) {
				// �����]�[��
				*pixel = 0xff0000;
			}else{
				// ���ʃ]�[��
				*pixel = 0x000000;
			}
			pixel++;
		}
	}
	CDIB32 *lpSrc=dib, *lpDst=m_pDIBBuf;
	if (m_nEffect&1) {
		CPlaneTransBlt::MirrorBlt1(lpDst, lpSrc,0,0,128);
		swap(lpSrc, lpDst);
	}
	if (m_nEffect&2) {
		CPlaneTransBlt::MirrorBlt2(lpDst, lpSrc,0,0,128);
		RECT rc={0,16,320,240};
		lpSrc->BltFast(lpDst,0,0,&rc);
	}
	if (lpDst==dib)  lpDst->BltFast(lpSrc,0,0);
}

LRESULT Cf3Map::SetMapData(int level, int x, int y, BYTE data)
{
	if (level<0 || 2<level || x<0 || m_Width[level]<=x || y<0 || m_Height[level]<=y || data>=0xf0) return 1;
	m_MapData[level][x+y*m_Width[level]] = data;
	return 0;
}

void Cf3Map::GetMainCharaCPos(int &x, int &y)
{
	m_MainChara->GetCPos(x,y);
}

CDIB32* Cf3Map::ReadMapChip(Cf3StageFile *lp, int level)
{
	BYTE* buf;
	DWORD s;
	CDIB32*dib = new CDIB32;
	// �X�e�[�W�����f�[�^��ǂݍ���
	if ((buf = lp->GetStageData(CT_MCD0|(level<<24),&s))!=NULL) {
		char fn2[256];
		::wsprintf(fn2,"!%x,%x",buf,s);
		if (dib->Load(fn2,false)==0) return dib;
	}
	// �ʖڂ����������Ȃ̂Ńt�@�C��������ǂݍ���
	if ((buf = lp->GetStageData(CT_MCF0|(level<<24),&s))!=NULL) {
		char fn[256];
		CopyMemory(fn,buf,s);
		fn[s]='\0';
		if (dib->Load(fn,false)==0) return dib;
	}
	dib->Load("resource/Cave.bmp",false);
	return dib;
}