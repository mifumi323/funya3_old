#include "stdafx.h"
#include "yanePlaneEffectBlt.h"
#include "yanePlaneBase.h"
#include "yaneSinTable.h"
#include <math.h>		//	sqrt

///////////////////////////////////////////////////////////////////////////////
//	class CPlaneFadeBlt

LRESULT	CPlaneFadeBlt::FadeBlt(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y){
	int sx,sy;
	lpPlane->GetSize(sx,sy);

	//	�S���X�^�����ʂȂ�Βʏ�]���ŉ�������
	bool bNormal = true;
	int i;
	for(i=0;i<sy;i++){
		if (i+y < 0) continue;	//	upper cliping
		if (i+y > 480) break;	//	lower cliping
		if ((m_nFadeTable[i+y]!=256) || (m_nRasterTable[i+y]!=0)){
			bNormal = false;
			break;
		}
	}
	if (bNormal) {
		return lpDraw->Blt(lpPlane,x,y);
	}
	//	�S���X�^��0���H
	for(i=0;i<sy;i++){
		if (i+y < 0) continue;	//	upper cliping
		if (i+y > 480) break;	//	lower cliping
		if (m_nFadeTable[i+y]==0) {
			bNormal = false;
			break;
		}
	}
	if (bNormal){
		return 0;
	}
	for(i=0;i<sy;i++){
		if (i+y < 0) continue;	//	upper cliping
		if (i+y > 480) break;	//	lower cliping

		int nFade = m_nFadeTable[i+y];
		RECT rcSrc;
		::SetRect(&rcSrc,0,i,sx,i+1);	//	�������X�^
		if (nFade==0) {
			continue;
		};
		if (nFade==256) {
			lpDraw->Blt(lpPlane,x+m_nRasterTable[i+y],y+i,&rcSrc);
			continue;
		}
		lpDraw->BlendBlt(lpPlane,x+m_nRasterTable[i+y],y+i,nFade,&rcSrc);
//		lpDraw->BlendBlt(lpPlane,x+m_nRasterTable[i+y],y+i,PlaneRGB(255-nFade,255-nFade,255-nFade),PlaneRGB(nFade,nFade,nFade),&rcSrc);
	}
	return 0;
}

CPlaneFadeBlt::CPlaneFadeBlt(void){
	for(int i=0;i<480;i++){
		m_nFadeTable[i] = 256;
	}
	ZERO(m_nRasterTable);
}

CPlaneFadeBlt::~CPlaneFadeBlt(){
}

///////////////////////////////////////////////////////////////////////////////
//
//		�g�����W�V�����n�̊֐�
//
///////////////////////////////////////////////////////////////////////////////
//--- �C�� '01/11/29  by ENRA  ---
// nTransMode=2�̎��AbyFadeRate��L���ɂ���
//--------------------------------
//--- �C�� '02/02/01  by ENRA  ---
// �N���b�s���O���o����悤�ɂ���
// �N���X�g�����W�V�������o���邾���T�|�[�g����
//--------------------------------

//	�R�[���o�b�N�p�֐�
vector< smart_ptr<CPlaneTransBltListener> > CPlaneTransBlt::m_avListener;

//	�܂��́A�}�N���ނ���

//	�ʏ�]��
#define CheckNormal \
	if (nPhase <= 0) return 0;				\
	if (nPhase >= 256) {					\
		switch (nTransMode) {				\
		case 0 :return lpDraw->BltFast(lpPlane,x,y,NULL,NULL,lpDstClipRect);\
		case 1 :return lpDraw->Blt(lpPlane,x,y,NULL,NULL,lpDstClipRect);\
		case 2 :return lpDraw->BlendBltFastAlpha(lpPlane,x,y,byFadeRate,NULL,NULL,lpDstClipRect); \
		case 3 :return lpDraw->BltNatural(lpPlane,x,y,NULL,NULL,lpDstClipRect); \
		}									\
	}										\
	int sx,sy;								\
	lpPlane->GetSize(sx,sy);


//	���ʂ�Blt����}�N��
#define BLT(dst,src) {\
	switch (nTransMode) {				\
	case 0 :dst ->BltFast(src,x,y,NULL,NULL,lpDstClipRect); break; \
	case 1 :dst ->Blt(src,x,y,NULL,NULL,lpDstClipRect); break;\
	case 2 :dst ->BlendBltFastAlpha(src,x,y,byFadeRate,NULL,NULL,lpDstClipRect); break;\
	case 3 :dst ->BltNatural(src,x,y,NULL,NULL,lpDstClipRect); break;\
	}				\
					}

//	�������C����]������}�N��
#define BLTHLINE(p) {\
	RECT rc;								\
	::SetRect(&rc,0,p,sx,p+1);				\
	switch (nTransMode) {					\
	case 0 : lpDraw->BltFast(lpPlane,x,y+p,&rc,NULL,lpDstClipRect); break;\
	case 1 : lpDraw->Blt(lpPlane,x,y+p,&rc,NULL,lpDstClipRect); break;\
	case 2 : lpDraw->BlendBltFastAlpha(lpPlane,x,y+p,byFadeRate,&rc,NULL,lpDstClipRect); break;\
	case 3 : lpDraw->BltNatural(lpPlane,x,y+p,&rc,NULL,lpDstClipRect); break;\
	}										\
					}

//	�������C��p��n�h�b�g�ڂ���k�h�b�g�]������}�N��
#define BLTHLINE2(p,n,k) {\
	RECT rc;									\
	int k2,n2;									\
	k2 = k;										\
	n2 = n;										\
	if (n2+k2>sx) { k2=sx-n2; }					\
	if (n2<0) { k2+=n2; n2=0; if (k2<0) k2=0;}	\
	::SetRect(&rc,n2,p,n2+k2,p+1);				\
	switch (nTransMode) {					\
	case 0 : lpDraw->BltFast(lpPlane,x+n2,y+p,&rc,NULL,lpDstClipRect); break;\
	case 1 : lpDraw->Blt(lpPlane,x+n2,y+p,&rc,NULL,lpDstClipRect); break;\
	case 2 : lpDraw->BlendBltFastAlpha(lpPlane,x+n2,y+p,byFadeRate,&rc,NULL,lpDstClipRect); break;\
	case 3 : lpDraw->BltNatural(lpPlane,x+n2,y+p,&rc,NULL,lpDstClipRect); break;\
	}										\
					}

//	�������C����]������}�N��
#define BLTVLINE(p) {\
	RECT rc;							\
	::SetRect(&rc,p,0,p+1,sy);			\
	switch (nTransMode) {					\
	case 0 : lpDraw->BltFast(lpPlane,x+p,y,&rc,NULL,lpDstClipRect);break;\
	case 1 : lpDraw->Blt(lpPlane,x+p,y,&rc,NULL,lpDstClipRect);break;\
	case 2 : lpDraw->BlendBltFastAlpha(lpPlane,x+p,y,byFadeRate,&rc,NULL,lpDstClipRect); break;\
	case 3 : lpDraw->BltNatural(lpPlane,x+p,y,&rc,NULL,lpDstClipRect); break;\
	}										\
					}

//	�������C�������炵�ē]������}�N��
#define BLTHLINEOFFSET(p,ox) {\
	RECT rc;								\
	::SetRect(&rc,0,p,sx,p+1);				\
	switch (nTransMode) {					\
	case 0 : lpDraw->BltFast(lpPlane,x+ox,y+p,&rc,NULL,lpDstClipRect);break;\
	case 1 : lpDraw->Blt(lpPlane,x+ox,y+p,&rc,NULL,lpDstClipRect);break;\
	case 2 : lpDraw->BlendBltFastAlpha(lpPlane,x+ox,y+p,byFadeRate,&rc,NULL,lpDstClipRect); break;\
	case 3 : lpDraw->BltNatural(lpPlane,x+ox,y+p,&rc,NULL,lpDstClipRect); break;\
	}										\
					}

//	��`��]������}�N��(Clip����)
#define BLTRECT(_x,_y,_sx,_sy) {\
	RECT rc;									\
	int	sx2,sy2;								\
	sx2=_sx; sy2=_sy;							\
	int x2,y2;									\
	x2 = _x; y2=_y;								\
	if(x2<0) { x2=0; }							\
	if(y2<0) { y2=0; }							\
	if(x2+sx2 > sx) { sx2 = sx-x2; }			\
	if(y2+sy2 > sy) { sy2 = sy-y2; }			\
	::SetRect(&rc,x2,y2,x2+sx2,y2+sy2);			\
	switch (nTransMode) {					\
	case 0 : lpDraw->BltFast(lpPlane,x+x2,y+y2,&rc,NULL,lpDstClipRect);break;\
	case 1 : lpDraw->Blt(lpPlane,x+x2,y+y2,&rc,NULL,lpDstClipRect);break;\
	case 2 : lpDraw->BlendBltFastAlpha(lpPlane,x+x2,y+y2,byFadeRate,&rc,NULL,lpDstClipRect);break;\
	case 3 : lpDraw->BltNatural(lpPlane,x+x2,y+y2,&rc,NULL,lpDstClipRect);break;\
	}										\
					}

//	��`���I�t�Z�b�g���ē]������}�N��(Clip����)
#define BLTRECTOFFSET(_x,_y,_sx,_sy,_ox,_oy) {\
	RECT rc;									\
	int	sx2 = _sx, sy2 = _sy;					\
	int x2 = _x, y2 = _y;						\
	if(x2<0) { x2=0; }							\
	if(y2<0) { y2=0; }							\
	if(x2+sx2 > sx) { sx2 = (sx)-(x2); }		\
	if(y2+sy2 > sy) { sy2 = (sy)-(y2); }		\
	::SetRect(&rc,x2,y2,(x2)+(sx2),(y2)+(sy2));	\
	switch (nTransMode) {					\
	case 0 : lpDraw->BltFast(lpPlane,(x)+(x2)+(_ox),(y)+(y2)+(_oy),&rc,NULL,lpDstClipRect);break;\
	case 1 : lpDraw->Blt(lpPlane,(x)+(x2)+(_ox),(y)+(y2)+(_oy),&rc,NULL,lpDstClipRect);break;\
	case 2 : lpDraw->BlendBltFastAlpha(lpPlane,(x)+(x2)+(_ox),(y)+(y2)+(_oy),byFadeRate,&rc,NULL,lpDstClipRect);break;\
	case 3 : lpDraw->BltNatural(lpPlane,(x)+(x2)+(_ox),(y)+(y2)+(_oy),&rc,NULL,lpDstClipRect);break;\
	}										\
					}

//	���ߏ��̋�`��]������}�N��
#define BLTRECTNEIGHBOR(_x,_y,_sx,_sy,level) {\
	RECT rc;									\
	int	sx2,sy2;								\
	sx2=_sx; sy2=_sy;							\
	if(_x+sx2 > sx) { sx2 = sx-_x; }			\
	if(_y+sy2 > sy) { sy2 = sy-_y; }			\
	int ox,oy;									\
	ox = rand() % level; oy = rand() % level;	\
	if (ox+_x+sx2 > sx) ox=0;					\
	if (oy+_y+sy2 > sy) oy=0;					\
	::SetRect(&rc,_x+ox,_y+oy,_x+sx2+ox,_y+sy2+oy);\
	switch (nTransMode) {					\
	case 0 : lpDraw->BltFast(lpPlane,x+_x,y+_y,&rc,NULL,lpDstClipRect);break;\
	case 1 : lpDraw->Blt(lpPlane,x+_x,y+_y,&rc,NULL,lpDstClipRect);break;\
	case 2 : lpDraw->BlendBltFastAlpha(lpPlane,x+_x,y+_y,byFadeRate,&rc,NULL,lpDstClipRect);break;\
	case 3 : lpDraw->BltNatural(lpPlane,x+_x,y+_y,&rc,NULL,lpDstClipRect);break;\
	}										\
				}

//	�_��]������}�N���i�x���j
#define BLTPSET(_x,_y) {\
	RECT rc;									\
	::SetRect(&rc,_x,_y,_x+1,_y+1);				\
	switch (nTransMode) {					\
	case 0 : lpDraw->BltFast(lpPlane,x+_x,y+_y,&rc,NULL,lpDstClipRect);break;\
	case 1 : lpDraw->Blt(lpPlane,x+_x,y+_y,&rc,NULL,lpDstClipRect);break;\
	case 2 : lpDraw->BlendBltFastAlpha(lpPlane,x+_x,y+_y,byFadeRate,&rc,NULL,lpDstClipRect);break;\
	case 3 : lpDraw->BltNatural(lpPlane,x+_x,y+_y,&rc,NULL,lpDstClipRect);break;\
	}										\
				}

//	�������C�����㉺�ɂ��炵�ē]������}�N��
#define BLTHLINEFROM(p,oy) {\
	RECT rc;							\
	::SetRect(&rc,0,p,sx,p+1);			\
	switch (nTransMode) {					\
	case 0 : lpDraw->BltFast(lpPlane,x,y+oy,&rc,NULL,lpDstClipRect);break;\
	case 1 : lpDraw->Blt(lpPlane,x,y+oy,&rc,NULL,lpDstClipRect);break;\
	case 2 : lpDraw->BlendBltFastAlpha(lpPlane,x,y+oy,byFadeRate,&rc,NULL,lpDstClipRect);break;\
	case 3 : lpDraw->BltNatural(lpPlane,x,y+oy,&rc,NULL,lpDstClipRect);break;\
	}										\
				}

//	�������C�������E�ɂ��炵�ē]������}�N��
#define BLTVLINEFROM(p,ox) {\
	RECT rc;							\
	::SetRect(&rc,p,0,p+1,sy);			\
	switch (nTransMode) {					\
	case 0 : lpDraw->BltFast(lpPlane,x+ox,y,&rc,NULL,lpDstClipRect);break;\
	case 1 : lpDraw->Blt(lpPlane,x+ox,y,&rc,NULL,lpDstClipRect);break;\
	case 2 : lpDraw->BlendBltFastAlpha(lpPlane,x+ox,y,byFadeRate,&rc,NULL,lpDstClipRect);break;\
	case 3 : lpDraw->BltNatural(lpPlane,x+ox,y,&rc,NULL,lpDstClipRect);break;\
	}										\
				}

//	�������C��������rx���X�g���b�`���ē]������}�N��
#define BLTHLINEST(p,rx) {\
	RECT rc;													\
	::SetRect(&rc,0,p,sx,p+1);									\
	SIZE sz = { (LONG)((rx)*sx),1 };							\
	switch (nTransMode) {					\
	case 0 : lpDraw->BltFast(lpPlane,x-(int)((sx>>1)*(rx-1)),y+p,&rc,&sz,lpDstClipRect);break;\
	case 1 : lpDraw->Blt(lpPlane,x-(int)((sx>>1)*(rx-1)),y+p,&rc,&sz,lpDstClipRect);break;\
	case 2 : lpDraw->BlendBltFastAlpha(lpPlane,x-(int)((sx>>1)*(rx-1)),y+p,byFadeRate,&rc,&sz,lpDstClipRect);break;\
	case 3 : lpDraw->BltNatural(lpPlane,x-(int)((sx>>1)*(rx-1)),y+p,&rc,&sz,lpDstClipRect);break;\
	}										\
				}

//	�������C�����c��ry���X�g���b�`���ē]������}�N��
#define BLTVLINEST(p,ry) {\
	RECT rc;													\
	::SetRect(&rc,p,0,p+1,sy);									\
	SIZE sz = { 1,(LONG)((ry)*sy) };							\
	switch (nTransMode) {					\
	case 0 : lpDraw->BltFast(lpPlane,x+p,y-(int)((sy>>1)*(ry-1)),&rc,&sz,lpDstClipRect);break;\
	case 1 : lpDraw->Blt(lpPlane,x+p,y-(int)((sy>>1)*(ry-1)),&rc,&sz,lpDstClipRect);break;\
	case 2 : lpDraw->BlendBltFastAlpha(lpPlane,x+p,y-(int)((sy>>1)*(ry-1)),byFadeRate,&rc,&sz,lpDstClipRect);break;\
	case 3 : lpDraw->BltNatural(lpPlane,x+p,y-(int)((sy>>1)*(ry-1)),&rc,&sz,lpDstClipRect);break;\
	}										\
				}

///////////////////////////////////////////////////////////////////////////////
LRESULT CPlaneTransBlt::Blt(int nTransNo,CPlaneBase*lpDst,CPlaneBase*lpSrc,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){	//	�ԍ��ŌĂяo����̂�
	static LRESULT (*FuncList[])(CPlaneBase*,CPlaneBase*,int,int,int,int nTransMode=0,BYTE byFadeRate=255,LPRECT lpDstClipRect=NULL) = {
		CPlaneTransBlt::MirrorBlt1,	//	�ꉞ�O�ł��Ăяo���Ă���
		CPlaneTransBlt::MirrorBlt1,	//	1
		CPlaneTransBlt::MirrorBlt2,
		CPlaneTransBlt::MirrorBlt3,
		CPlaneTransBlt::MirrorBlt4,
		CPlaneTransBlt::CutInBlt1,
		CPlaneTransBlt::CutInBlt2,
		CPlaneTransBlt::CutInBlt3,
		CPlaneTransBlt::CutInBlt4,
		CPlaneTransBlt::CutInBlt5,
		CPlaneTransBlt::CutInBlt6,	//	10
		CPlaneTransBlt::CutInBlt7,
		CPlaneTransBlt::CutInBlt8,
		CPlaneTransBlt::CutInBlt9,
		CPlaneTransBlt::CutInBlt10,
		CPlaneTransBlt::CutInBlt11,
		CPlaneTransBlt::CutInBlt12,
		CPlaneTransBlt::CutInBlt13,
		CPlaneTransBlt::CutInBlt14,
		CPlaneTransBlt::CutInBlt15,
		CPlaneTransBlt::CutInBlt16,	//	20
		CPlaneTransBlt::CutInBlt17,
		CPlaneTransBlt::CutInBlt18,
		CPlaneTransBlt::CutInBlt19,
		CPlaneTransBlt::WaveBlt1,
		CPlaneTransBlt::WaveBlt2,
		CPlaneTransBlt::WaveBlt3,
		CPlaneTransBlt::WaveBlt4,
		CPlaneTransBlt::CircleBlt1,
		CPlaneTransBlt::CircleBlt2,
		CPlaneTransBlt::CircleBlt3,	// 30
		CPlaneTransBlt::CircleBlt4,
		CPlaneTransBlt::CircleBlt5,
		CPlaneTransBlt::RectBlt1,
		CPlaneTransBlt::RectBlt2,
		CPlaneTransBlt::RectBlt3,
		CPlaneTransBlt::BlindBlt1,
		CPlaneTransBlt::BlindBlt2,
		CPlaneTransBlt::BlindBlt3,
		CPlaneTransBlt::BlindBlt4,
		CPlaneTransBlt::BlindBlt5,	// 40
		CPlaneTransBlt::BlindBlt6,
		CPlaneTransBlt::BlindBlt7,
		CPlaneTransBlt::BlindBlt8,
		CPlaneTransBlt::BlindBlt9,
		CPlaneTransBlt::BlindBlt10,
		CPlaneTransBlt::WhorlBlt1,
		CPlaneTransBlt::WhorlBlt2,
		CPlaneTransBlt::WhorlBlt3,
		CPlaneTransBlt::WhorlBlt4,
		CPlaneTransBlt::WhorlBlt5,	// 50
		CPlaneTransBlt::WhorlBlt6,
		CPlaneTransBlt::WhorlBlt7,
		CPlaneTransBlt::WhorlBlt8,
		CPlaneTransBlt::BlendBlt1,
		CPlaneTransBlt::DiagonalDiffusionBlt,
		CPlaneTransBlt::DiffusionCongeriesBlt1,
		CPlaneTransBlt::DiffusionCongeriesBlt2,
		CPlaneTransBlt::DiffusionCongeriesBlt3,
		CPlaneTransBlt::SquashBlt,
		CPlaneTransBlt::ForwardRollBlt,	// 60
		CPlaneTransBlt::RotationBlt1,
		CPlaneTransBlt::RotationBlt2,
		CPlaneTransBlt::RotationBlt3,
		CPlaneTransBlt::RotationBlt4,
		CPlaneTransBlt::EnterUpBlt1,
		CPlaneTransBlt::EnterUpBlt2,
		CPlaneTransBlt::CellGatherBlt1,
		CPlaneTransBlt::CellGatherBlt2,
		CPlaneTransBlt::MosaicBlt1,
		CPlaneTransBlt::FlushBlt1,		//	70
		CPlaneTransBlt::SlitCurtainBlt1,
		CPlaneTransBlt::SlitCurtainBlt2,
		CPlaneTransBlt::SlitCurtainBlt3,
		CPlaneTransBlt::SlitCurtainBlt4,
		CPlaneTransBlt::SlitCurtainBlt5,
		CPlaneTransBlt::SlitCurtainBlt6,
		CPlaneTransBlt::SlitCurtainBlt7,
		CPlaneTransBlt::SlitCurtainBlt8,
		CPlaneTransBlt::TensileBlt1,
		CPlaneTransBlt::TensileBlt2,	// 80
		CPlaneTransBlt::TensileBlt3,
		CPlaneTransBlt::TensileBlt4,
	//	��̂��g�����W�V�����nBlt
	};

//	WARNING(nTransNo >= NELEMS(FuncList) ,"CPlaneTransBlt::Blt�Ŕ͈͊O");
	//	����G���[�ł͂Ȃ��A�s���I���Ƃ��ă��^�[������ق��������B

	//	�}�C�i�X�̒l�̏ꍇ�́A���[�U�[�o�^�֐�
	if (nTransNo<0) {
		nTransNo = -nTransNo-1;
		int n = GetBltListener()->size();
		if (nTransNo < n){
			return (*GetBltListener())[nTransNo]->Blt(lpDst,lpSrc,x,y,nPhase,nTransMode,byFadeRate,lpDstClipRect);
		} else {
			return 1;	//�@�͈͊O
		}
	}else{
		// �N���X�g�����W�V�������T�|�[�g����ꍇ�͂�����case������
		switch(nTransNo){
		case  5:	// CutInBlt1
		case  7:	// CutInBlt3
			if(nPhase>256){
				nPhase = 512-nPhase;
			} break;
		case  8:	// CutInBlt4
		case  9:	// CutInBlt5
		case 10:	// CutInBlt6
		case 11:	// CutInBlt7
			if(nPhase>256){
				nTransNo = nTransNo - 2*(nTransNo&1) + 1;
				nPhase = 512-nPhase;
			} break;
		case 13:	// CutInBlt9
		case 14:	// CutInBlt10
		case 15:	// CutInBlt11
		case 16:	// CutInBlt12
			if(nPhase>256){
				nTransNo = 16-nTransNo+13;
				nPhase = 512-nPhase;
			} break;
		case 17:	// CutInBlt13
		case 18:	// CutInBlt14
		case 19:	// CutInBlt15
		case 20:	// CutInBlt16
		case 21:	// CutInBlt17
			if(nPhase>256){
				nPhase = 512-nPhase;
			} break;
		case 22:	// CutInBlt18
		case 23:	// CutInBlt19
			if(nPhase>256){
				nTransNo = nTransNo - 2*(nTransNo&1) + 1;
				nPhase = 512 - nPhase;
			} break;
		case 26:	// WaveBlt3
		case 27:	// WaveBlt4
		case 36:	// BlindBlt1
		case 37:	// BlindBlt2
		case 38:	// BlindBlt3
		case 39:	// BlindBlt4
		case 40:	// BlindBlt5
		case 41:	// BlindBlt6
		case 42:	// BlindBlt7
		case 43:	// BlindBlt8
		case 44:	// BlindBlt9
		case 45:	// BlindBlt10
			if(nPhase>256){
				nTransNo = nTransNo - 2*(nTransNo&1) + 1;
				nPhase = 512 - nPhase;
			} break;
		case 54:	// BlendBlt1
		case 55:	// DiagonalDiffusionBlt
		case 56:	// DiffusionCongeriesBlt1
		case 57:	// DiffusionCongeriesBlt2
		case 58:	// DiffusionCongeriesBlt3
		case 59:	// SquashBlt
		case 60:	// ForwardRollBlt
		case 67:	// CellGatherBlt1
		case 68:	// CellGatherBlt2
			if(nPhase>256){
				nPhase = 512-nPhase;
			} break;
		case 71:	// SlitCurtainBlt1
		case 72:	// SlitCurtainBlt2
		case 73:	// SlitCurtainBlt3
		case 74:	// SlitCurtainBlt4
		case 75:	// SlitCurtainBlt5
		case 76:	// SlitCurtainBlt6
		case 77:	// SlitCurtainBlt7
		case 78:	// SlitCurtainBlt8
			if(nPhase>256){
				nTransNo = nTransNo + 2*(nTransNo&1) - 1;
				nPhase = 512 - nPhase;
			} break;
		default:
			// �N���X�g�����W�V������Ή�
			if(nPhase>256) nPhase = 0;
		}
	}

	if (nTransNo >= NELEMS(FuncList)) return 1; // �͈͊O

	return FuncList[nTransNo](lpDst,lpSrc,x,y,nPhase,nTransMode,byFadeRate,lpDstClipRect);
}

LRESULT CPlaneTransBlt::MirrorBlt1(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int px=0;px<sx;px++){
		BLTVLINEFROM(px,sx-px-1);
	}
	return 0;
}
LRESULT CPlaneTransBlt::MirrorBlt2(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int py=0;py<sy;py++){
		BLTHLINEFROM(py,sy-py-1);
	}
	return 0;
}
LRESULT CPlaneTransBlt::MirrorBlt3(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static CSinTable st;
	double phase;
	phase = (double)(256-nPhase) / 256;
	for(int py=0;py<sy;py++){
		BLTHLINEST(py,1+ (2 * st.Sin(256*py/sy) * phase)/65536);
	}
	return 0;
}
LRESULT CPlaneTransBlt::MirrorBlt4(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static CSinTable st;
	double phase;
	phase = (double)(256-nPhase) / 256;
	for(int px=0;px<sx;px++){
		BLTVLINEST(px,1+ (2 * st.Sin(256*px/sx) * phase)/65536);
	}
	return 0;
}

LRESULT CPlaneTransBlt::CutInBlt1(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int py=0;py<sy;py++){
		int r,r2;	//	���ꕝ
		r2 = 256-nPhase;	//	���ꕝ
		r = (rand() % r2) - (r2>>1);
		BLTHLINEOFFSET(py,r);
	}
	return 0;
}

LRESULT CPlaneTransBlt::CutInBlt2(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int level;
	if (nPhase<128) level = 10; else if (nPhase<200) level = 6;
	else if(nPhase<230) level = 4; else level = 2;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=4){
			if ((rand()&255) <= nPhase){
				BLTRECT(px,py,4,2);
			} else {
				BLTRECTNEIGHBOR(px,py,4,2,level);	//	�ߏ��̂��Blt
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt3(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=4){
			if ((rand()&255) <= nPhase){
				BLTRECT(px,py,4,2);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt4(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=4){
			if ((rand()&255) <= (nPhase<<1) + ((sy-py-1)<<8)/sy - 256){
				BLTRECT(px,py,4,2);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt5(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=4){
			if ((rand()&255) <= (nPhase<<1) + (py<<8)/sy - 256){
				BLTRECT(px,py,4,2);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt6(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=4){
			if ((rand()&255) <= (nPhase<<1) + ((sx-px-1)<<8)/sx - 256){
				BLTRECT(px,py,4,2);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt7(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=4){
			if ((rand()&255) <= (nPhase<<1) + (px<<8)/sx - 256){
				BLTRECT(px,py,4,2);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt8(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sm;	// max size
	if (sy>sx) sm=sy; else sm=sx;
	int sr;	// rest size
	sr = (sm * nPhase) >> 8;
	BLTRECT((sx>>1)-(sr>>1),(sy>>1)-(sr>>1),sr,sr);
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt9(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sm;	// max size
	if (sy>sx) sm=sy; else sm=sx;
	int sr;	// rest size
	sr = (sm * nPhase) >> 8;
	BLTRECT(0,0,sr,sr);
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt10(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sm;	// max size
	if (sy>sx) sm=sy; else sm=sx;
	int sr;	// rest size
	sr = (sm * nPhase) >> 8;
	BLTRECT(sx-sr,0,sr,sr);
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt11(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sm;	// max size
	if (sy>sx) sm=sy; else sm=sx;
	int sr;	// rest size
	sr = (sm * nPhase) >> 8;
	BLTRECT(0,sy-sr,sr,sr);
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt12(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sm;	// max size
	if (sy>sx) sm=sy; else sm=sx;
	int sr;	// rest size
	sr = (sm * nPhase) >> 8;
	BLTRECT(sx-sr,sy-sr,sr,sr);
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt13(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sr;	// start size
	sr = (sx * nPhase) >> 8;
	for(int py=0;py<sy;py+=2){
		BLTRECT(0,py,sr,1); // �N���b�v�t��HLINE
		BLTRECT(sx-sr-1,py+1,sx,1); // �N���b�v�t��HLINE
	}
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt14(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sr;	// start size
	sr = (sy * nPhase) >> 8;
	for(int px=0;px<sx;px+=2){
		BLTRECT(px,0,1,sr); // �N���b�v�t��VLINE
		BLTRECT(px+1,sy-sr-1,1,sy); // �N���b�v�t��VLINE
	}
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt15(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sr;	// start size
	sr = (sx * nPhase) >> 7;
	for(int py=0;py<sy;py+=4){
		BLTRECT(0,py,sr,1);			// �N���b�v�t��HLINE
		BLTRECT(sx-sr-1,py+2,sx,1);	// �N���b�v�t��HLINE
	}
	if (nPhase > 128) {
		for(int py=0;py<sy;py+=4){
			sr = (sx * (nPhase-128)) >> 7;
			BLTRECT(0,py+1,sr,1);		// �N���b�v�t��HLINE
			BLTRECT(sx-sr-1,py+3,sx,1); // �N���b�v�t��HLINE
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt16(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sr;	// start size
	sr = (sy * nPhase) >> 7;
	for(int px=0;px<sx;px+=4){
		BLTRECT(px,0,1,sr);			// �N���b�v�t��VLINE
		BLTRECT(px+2,sy-sr-1,1,sy);	// �N���b�v�t��VLINE
	}
	if (nPhase > 128) {
		for(int px=0;px<sx;px+=4){
			sr = (sy * (nPhase-128)) >> 7;
			BLTRECT(px+1,0,1,sr);		// �N���b�v�t��VLINE
			BLTRECT(px+3,sy-sr-1,1,sy); // �N���b�v�t��VLINE
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt17(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sm;	// max size
	sm = sx+sy;
	int sr;	// rest size
	sr = (sm * nPhase) >> 8;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=2){
			if ((rand()&255) <=
				sr-sqrt((px*2-sx)*(px*2-sx)+(py*2-sy)*(py*2-sy)))
			{
				BLTRECT(px,py,2,2);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt18(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sr;	// rest size
	sr = (sy * nPhase) >> 8;
	BLTRECT(0,0,sx,sr);
	BLTHLINEOFFSET(sr,-sx/9);
	BLTHLINEOFFSET(sr+1,sx/9);
	BLTHLINEOFFSET(sr+2,-sx*2/9);
	BLTHLINEOFFSET(sr+3,sx*2/9);
	BLTHLINEOFFSET(sr+4,-sx/3);
	BLTHLINEOFFSET(sr+5,sx/3);
	return 0;
}
LRESULT CPlaneTransBlt::CutInBlt19(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sr;	// rest size
	sr = sy - ((sy * nPhase) >> 8);
	BLTRECT(0,sr,sx,sy-sr);
	BLTHLINEOFFSET(sr-1,-sx/9);
	BLTHLINEOFFSET(sr-2,sx/9);
	BLTHLINEOFFSET(sr-3,-sx*2/9);
	BLTHLINEOFFSET(sr-4,sx*2/9);
	BLTHLINEOFFSET(sr-5,-sx/3);
	BLTHLINEOFFSET(sr-6,sx/3);
	return 0;
}

LRESULT CPlaneTransBlt::BlindBlt1(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int px=0;px<sx;px++){
		if ((px & 7) <= (nPhase >> 5)) {
			BLTVLINE(px);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::BlindBlt2(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int px=0;px<sx;px++){
		if ((7-(px & 7)) <= (nPhase >> 5)) {
			BLTVLINE(px);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::BlindBlt3(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int px=0;px<sx;px++){
		if ((px & 15) <= (nPhase >> 4)) {
			BLTVLINE(px);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::BlindBlt4(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int px=0;px<sx;px++){
		if ((15-(px & 15)) <= (nPhase >> 4)) {
			BLTVLINE(px);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::BlindBlt5(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int py=0;py<sy;py++){
		if ((py & 7) <= (nPhase >> 5)) {
			BLTHLINE(py);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::BlindBlt6(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int py=0;py<sy;py++){
		if ((7-(py & 7)) <= (nPhase >> 5)) {
			BLTHLINE(py);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::BlindBlt7(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int py=0;py<sy;py++){
		if ((py & 15) <= (nPhase >> 4)) {
			BLTHLINE(py);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::BlindBlt8(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	for(int py=0;py<sy;py++){
		if ((15-(py & 15)) <= (nPhase >> 4)) {
			BLTHLINE(py);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::BlindBlt9(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int phase;
	phase = nPhase >> 5;
	for(int py=0;py<sy;py++){
		for(int px=-(py % 7);px<sx;px+=8){
			BLTHLINE2(py,px,phase);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::BlindBlt10(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int phase;
	phase = nPhase >> 4;
	for(int py=0;py<sy;py++){
		for(int px=-(py % 15);px<sx;px+=16){
			BLTHLINE2(py,px,phase);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::RectBlt1(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int phase;
	phase = nPhase >> 6;
	for(int py=0;py<sy;py+=4){
		for(int px=0;px<sx;px+=4){
			BLTRECT(px,py,phase,phase);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::RectBlt2(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int phase;
	phase = nPhase >> 5;
	for(int py=0;py<sy;py+=8){
		for(int px=0;px<sx;px+=8){
			BLTRECT(px,py,phase,phase);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::RectBlt3(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int phase;
	phase = nPhase >> 4;
	for(int py=0;py<sy;py+=16){
		for(int px=0;px<sx;px+=16){
			BLTRECT(px,py,phase,phase);
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::CircleBlt1(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sm;	// max size
	sm = (int)(sqrt(sx*sx/4+sy*sy)*2);
	int sr;	// rest size
	sr = (sm * nPhase) >> 8;
	int ssy,py;
	ssy = 0;
	for(py=0;py<(sr>>1);py++,ssy++){
		int px,rx;
		rx = (int)(sqrt(sr*sr/4-ssy*ssy)*2);
		px = (sx>>1)-(rx>>1);
		BLTRECT(px,py,rx,1); // �N���b�v�t��HLINE
	}
	return 0;
}
LRESULT CPlaneTransBlt::CircleBlt2(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sm;	// max size
	sm = (int)(sqrt(sx*sx/4+sy*sy)*2);
	int sr;	// rest size
	sr = (sm * nPhase) >> 8;
	int ssy,py;
	ssy = 0;
	for(py=sy;py>sy-(sr>>1);py--,ssy++){
		int px,rx;
		rx = (int)(sqrt(sr*sr/4-ssy*ssy)*2);
		px = (sx>>1)-(rx>>1);
		BLTRECT(px,py,rx,1); // �N���b�v�t��HLINE
	}
	return 0;
}
LRESULT CPlaneTransBlt::CircleBlt3(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sm;	// max size
	sm = (int)(sqrt(sx*sx+sy*sy/4)*2);
	int sr;	// rest size
	sr = (sm * nPhase) >> 8;
	int ssy,py;
	ssy =-(sr >> 1);
	for(py=(sy>>1)-(sr>>1);py<((sy>>1)+(sr>>1));py++,ssy++){
		int px,rx;
		rx = (int)(sqrt(sr*sr/4-ssy*ssy));
		px = 0;
		BLTRECT(px,py,rx,1); // �N���b�v�t��HLINE
	}
	return 0;
}
LRESULT CPlaneTransBlt::CircleBlt4(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sm;	// max size
	sm = (int)(sqrt(sx*sx+sy*sy/4)*2);
	int sr;	// rest size
	sr = (sm * nPhase) >> 8;
	int ssy,py;
	ssy =-(sr >> 1);
	for(py=(sy>>1)-(sr>>1);py<((sy>>1)+(sr>>1));py++,ssy++){
		int px,rx;
		rx = (int)(sqrt(sr*sr/4-ssy*ssy));
		px = sx-rx;
		BLTRECT(px,py,rx,1); // �N���b�v�t��HLINE
	}
	return 0;
}
LRESULT CPlaneTransBlt::CircleBlt5(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	int sm;	// max size
	sm = sx+sy;
	int sr;	// rest size
	sr = (sm * nPhase) >> 8;
	int ssy,py;
	ssy =-(sr >> 1);
	for(py=(sy>>1)-(sr>>1);py<((sy>>1)+(sr>>1));py++,ssy++){
		int px,rx;
		rx = (int)(sqrt(sr*sr/4-ssy*ssy)*2); // bug-fixed '00/02/24
		px = (sx>>1)-(rx>>1);
		BLTRECT(px,py,rx,1); // �N���b�v�t��HLINE
	}
	return 0;
}
LRESULT CPlaneTransBlt::WhorlBlt1(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static int uzu[16] = {
		 0, 1, 2, 3,
		11,12,13, 4,
		10,15,14, 5,
		 9, 8, 7, 6
	};
	int phase;
	phase = nPhase >> 4;
	for(int py=0;py<sy;py++){
		for(int px=0;px<sx;px++){
			if (uzu[(px & 3)+((py & 3)<<2) & 15] <= phase){
				BLTPSET(px,py);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::WhorlBlt2(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static int uzu[16] = {
		 6, 7, 8, 9,
		 5, 0, 1,10,
		 4, 3, 2,11,
		15,14,13,12
	};
	int phase;
	phase = nPhase >> 4;
	for(int py=0;py<sy;py++){
		for(int px=0;px<sx;px++){
			if (uzu[(px & 3)+((py & 3)<<2) & 15] <= phase){
				BLTPSET(px,py);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::WhorlBlt3(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static int uzu[16] = {
		 0, 1, 2, 3,
		11,12,13, 4,
		10,15,14, 5,
		 9, 8, 7, 6
	};
	int phase;
	phase = nPhase >> 4;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=2){
			if (uzu[((px>>1) & 3)+(((py>>1) & 3)<<2) & 15] <= phase){
				BLTRECT(px,py,2,2);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::WhorlBlt4(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static int uzu[16] = {
		 6, 7, 8, 9,
		 5, 0, 1,10,
		 4, 3, 2,11,
		15,14,13,12
	};
	int phase;
	phase = nPhase >> 4;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=2){
			if (uzu[((px>>1) & 3)+(((py>>1) & 3)<<2) & 15] <= phase){
				BLTRECT(px,py,2,2);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::WhorlBlt5(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static int uzu[16] = {
		 0, 1, 2, 3,
		11,12,13, 4,
		10,15,14, 5,
		 9, 8, 7, 6
	};
	int phase;
	phase = nPhase >> 4;
	for(int py=0;py<sy;py+=4){
		for(int px=0;px<sx;px+=4){
			if (uzu[((px>>2) & 3)+(((py>>2) & 3)<<2) & 15] <= phase){
				BLTRECT(px,py,4,4);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::WhorlBlt6(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static int uzu[16] = {
		 6, 7, 8, 9,
		 5, 0, 1,10,
		 4, 3, 2,11,
		15,14,13,12
	};
	int phase;
	phase = nPhase >> 4;
	for(int py=0;py<sy;py+=4){
		for(int px=0;px<sx;px+=4){
			if (uzu[((px>>2) & 3)+(((py>>2) & 3)<<2) & 15] <= phase){
				BLTRECT(px,py,4,4);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::WhorlBlt7(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static int uzu[16] = {
		 0, 1, 2, 3,
		11,12,13, 4,
		10,15,14, 5,
		 9, 8, 7, 6
	};
	int phase;
	phase = nPhase >> 4;
	for(int py=0;py<sy;py+=8){
		for(int px=0;px<sx;px+=8){
			if (uzu[((px>>3) & 3)+(((py>>3) & 3)<<2) & 15] <= phase){
				BLTRECT(px,py,8,8);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::WhorlBlt8(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static int uzu[16] = {
		 6, 7, 8, 9,
		 5, 0, 1,10,
		 4, 3, 2,11,
		15,14,13,12
	};
	int phase;
	phase = nPhase >> 4;
	for(int py=0;py<sy;py+=8){
		for(int px=0;px<sx;px+=8){
			if (uzu[((px>>3) & 3)+(((py>>3) & 3)<<2) & 15] <= phase){
				BLTRECT(px,py,8,8);
			}
		}
	}
	return 0;
}
LRESULT CPlaneTransBlt::WaveBlt1(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static CSinTable st;
	for(int py=0;py<sy;py++){
		BLTHLINEOFFSET(py,(int) (st.Cos(py) * ((255-nPhase)*2) >> 16));
	}
	return 0;
}
LRESULT CPlaneTransBlt::WaveBlt2(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static CSinTable st;
	for(int py=0;py<sy;py++){
		BLTHLINEOFFSET(py,(int) (st.Cos(py+256) * ((255-nPhase)*2) >> 16));
	}
	return 0;
}
LRESULT CPlaneTransBlt::WaveBlt3(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static CSinTable st;
	for(int py=0;py<sy;py++){
		BLTHLINEOFFSET(py,(int) (st.Cos(py<<3) * (255-nPhase) >> 16));
	}
	return 0;
}
LRESULT CPlaneTransBlt::WaveBlt4(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	static CSinTable st;
	for(int py=0;py<sy;py++){
		BLTHLINEOFFSET(py,(int) (st.Cos((py<<3)+256) * (255-nPhase) >> 16));
	}
	return 0;
}

LRESULT CPlaneTransBlt::BlendBlt1(CPlaneBase*lpDraw,CPlaneBase*lpPlane,int x,int y,int nPhase,int nTransMode,BYTE byFadeRate,LPRECT lpDstClipRect){
	CheckNormal;
	switch (nTransMode) {
	case 0 :return lpDraw->BlendBltFast(lpPlane,x,y,nPhase,NULL,NULL,lpDstClipRect);
//	case 0 :return lpDraw->BlendBltFast(lpPlane,x,y,PlaneRGB(256-nPhase,256-nPhase,256-nPhase),PlaneRGB(nPhase,nPhase,nPhase));
	case 1 :return lpDraw->BlendBlt(lpPlane,x,y,nPhase,NULL,NULL,lpDstClipRect);
//	case 1 :return lpDraw->BlendBlt(lpPlane,x,y,PlaneRGB(256-nPhase,256-nPhase,256-nPhase),PlaneRGB(nPhase,nPhase,nPhase));
//	case 2 :return lpDraw->FadeBltAlpha(lpPlane,x,y,nPhase); //	�x������A����Ȃ�g���Ƃ��Ă�[
	case 2 :return lpDraw->BlendBltFastAlpha(lpPlane,x,y,nPhase,NULL,NULL,lpDstClipRect);
//	case 3 :return -1; // not supported
	case 3 :return lpDraw->BltNatural(lpPlane,x,y,nPhase,NULL,NULL,lpDstClipRect);
	default: return -1;
	}
}

///////////////////////////////////////////////////////////////////////////////
// BYTE byFadeRate��ǉ� '01/11/29	by enra
// �Ƃ����int nTransMode /* = false */���āH�H
// �Ȃ��boolean���o�Ă���́H�����܂����B
LRESULT CPlaneTransBlt::MosaicBlt1(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;

	nPhase = ((255 - nPhase) >> 2) + 1;
	BLT(lpDraw, lpPlane);
	RECT rc;
	::SetRect(&rc, x, y, x + sx, y + sy);
	if(lpDstClipRect!=NULL){
		RECT rc2;
		::IntersectRect(&rc2, &rc, lpDstClipRect);
		return lpDraw->MosaicEffect(nPhase, &rc2);	//	�]����`�Ƀ��U�C�N��������
	}
	return lpDraw->MosaicEffect(nPhase, &rc);	//	�]����`�Ƀ��U�C�N��������
}

LRESULT CPlaneTransBlt::FlushBlt1(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
					  int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;

	BLT(lpDraw, lpPlane);
	RECT rc;
	::SetRect(&rc, x, y, x + sx, y + sy);
	if(lpDstClipRect!=NULL){
		RECT rc2;
		::IntersectRect(&rc2, &rc, lpDstClipRect);
		return lpDraw->FlushEffect(&rc2);	//	�]����`�Ƀ��U�C�N��������
	}

	return lpDraw->FlushEffect(&rc);	//	�]����`�Ƀ��U�C�N��������
}

///////////////////////////////////////////////////////////////////////////////
//	Thanks ! > TearDrop_Stone

// �X���b�g�J�[�e���g�����W�V�����B
LRESULT CPlaneTransBlt::SlitCurtainBlt1(CPlaneBase* pDest, CPlaneBase* pSrc,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	return BltTransHelper1(pSrc, pDest, x, y, nPhase, true, nTransMode, byFadeRate, 16, lpDstClipRect);
}
LRESULT CPlaneTransBlt::SlitCurtainBlt2(CPlaneBase* pDest, CPlaneBase* pSrc,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	return BltTransHelper1(pSrc, pDest, x, y, nPhase, false, nTransMode, byFadeRate, 16, lpDstClipRect);
}
LRESULT CPlaneTransBlt::SlitCurtainBlt3(CPlaneBase* pDest, CPlaneBase* pSrc,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	return BltTransHelper1(pSrc, pDest, x, y, nPhase, true, nTransMode, byFadeRate, 8, lpDstClipRect);
}
LRESULT CPlaneTransBlt::SlitCurtainBlt4(CPlaneBase* pDest, CPlaneBase* pSrc,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	return BltTransHelper1(pSrc, pDest, x, y, nPhase, false, nTransMode, byFadeRate, 8, lpDstClipRect);
}
LRESULT CPlaneTransBlt::SlitCurtainBlt5(CPlaneBase* pDest, CPlaneBase* pSrc,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	return BltTransHelper2(pSrc, pDest, x, y, nPhase, true, nTransMode, byFadeRate, 16, lpDstClipRect);
}
LRESULT CPlaneTransBlt::SlitCurtainBlt6(CPlaneBase* pDest, CPlaneBase* pSrc,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	return BltTransHelper2(pSrc, pDest, x, y, nPhase, false, nTransMode, byFadeRate, 16, lpDstClipRect);
}
LRESULT CPlaneTransBlt::SlitCurtainBlt7(CPlaneBase* pDest, CPlaneBase* pSrc,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	return BltTransHelper2(pSrc, pDest, x, y, nPhase, true, nTransMode, byFadeRate, 8, lpDstClipRect);
}
LRESULT CPlaneTransBlt::SlitCurtainBlt8(CPlaneBase* pDest, CPlaneBase* pSrc,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	return BltTransHelper2(pSrc, pDest, x, y, nPhase, false, nTransMode, byFadeRate, 8, lpDstClipRect);
}

// �����L�΂��g�����W�V�����B
LRESULT CPlaneTransBlt::TensileBlt1(CPlaneBase* pDest, CPlaneBase* pSrc,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	return TensileBltHelper1(pSrc, pDest, x, y, nPhase, true, nTransMode, byFadeRate, lpDstClipRect);
}
LRESULT CPlaneTransBlt::TensileBlt2(CPlaneBase* pDest, CPlaneBase* pSrc,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	return TensileBltHelper1(pSrc, pDest, x, y, nPhase, false, nTransMode, byFadeRate, lpDstClipRect);
}
LRESULT CPlaneTransBlt::TensileBlt3(CPlaneBase* pDest, CPlaneBase* pSrc,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	return TensileBltHelper2(pSrc, pDest, x, y, nPhase, true, nTransMode, byFadeRate, lpDstClipRect);
}
LRESULT CPlaneTransBlt::TensileBlt4(CPlaneBase* pDest, CPlaneBase* pSrc,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	return TensileBltHelper2(pSrc, pDest, x, y, nPhase, false, nTransMode, byFadeRate, lpDstClipRect);
}

// �����L�΂��g�����W�V�����̎���
LRESULT CPlaneTransBlt::TensileBltHelper1(CPlaneBase* pSrc, CPlaneBase* pDest, int x, int y,
										   int nPhase, bool Direction, int nTransMode,	BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CPlaneBase *lpDraw=pDest,*lpPlane=pSrc;	//	�����������O�O�G
	CheckNormal;

	int i, j;
	int dx, dy;
	pDest->GetSize(dx, dy);

	RECT rc;
	SIZE sz;
	sz.cy = sy;
	rc.top = 0;
	rc.bottom = sy;
	// ���łɕ\������Ă��镔�������݂���͂�������A
	// ������\��
	i = (nPhase * sx) / 256;
	if(Direction){
		rc.right = sx;
		rc.left = rc.right - i;
	}else{
		rc.left = 0;
		rc.right = i;
	}
	if(rc.left != rc.right){
		switch (nTransMode) {
		case 0 :pDest->BltFast(pSrc,x + rc.left, y, &rc,NULL,lpDstClipRect); break;
		case 1 :pDest->Blt(pSrc,x + rc.left, y, &rc,NULL,lpDstClipRect); break;
		case 2 :pDest->BlendBltFastAlpha(pSrc,x + rc.left, y, byFadeRate, &rc,NULL,lpDstClipRect); break;
		case 3 :pDest->BltNatural(pSrc,x + rc.left, y, &rc,NULL,lpDstClipRect); break;
		}
	}
	// ���ɕ\�������ׂ������͈����L�΂����B
	if(Direction){
		i = 0;
		j = rc.right = rc.left;
		j += x;
		rc.left--;
	}else{
		i = rc.right + x;
		rc.left = rc.right;
		rc.right++;
		j = dx - rc.left - x;
	}
	// �����L�΂����ׂ����������Ɋg�傷��BFPS ���|�����ǂˁB
	sz.cx = j;
	if(rc.left < 0)
		rc.left = 0;
	if(rc.right > sx)
		rc.right = sx;
	switch (nTransMode) {
	case 0 :pDest->BltFast(pSrc,i, y, &rc, &sz,lpDstClipRect); break;
	case 1 :pDest->Blt(pSrc,i, y, &rc, &sz,lpDstClipRect); break;
	case 2 :pDest->BlendBltFastAlpha(pSrc,i, y, byFadeRate, &rc, &sz,lpDstClipRect); break;
	case 3 :pDest->BltNatural(pSrc,i, y, &rc, &sz,lpDstClipRect); break;
	}
	return 0;
}
LRESULT CPlaneTransBlt::TensileBltHelper2(CPlaneBase* pSrc, CPlaneBase* pDest, int x, int y,
										   int nPhase, bool Direction, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CPlaneBase *lpDraw=pDest,*lpPlane=pSrc;	//	�����������O�O�G
	CheckNormal;

	int i, j;
	int dx, dy;
	pDest->GetSize(dx, dy);

	SIZE sz;
	RECT rc;
	sz.cx = sx;
	rc.left = 0;
	rc.right = sx;
	// ���łɕ\������Ă��镔�������݂���͂�������A
	// ������\��
	i = (nPhase * sy) / 256;
	if(Direction){
		rc.bottom = sy;
		rc.top = rc.bottom - i;
	}else{
		rc.top = 0;
		rc.bottom = i;
	}
	switch (nTransMode) {
	case 0 :pDest->BltFast(pSrc,x, y + rc.top, &rc,NULL,lpDstClipRect); break;
	case 1 :pDest->Blt(pSrc,x, y + rc.top, &rc,NULL,lpDstClipRect); break;
	case 2 :pDest->BlendBltFastAlpha(pSrc,x, y + rc.top, byFadeRate, &rc,NULL,lpDstClipRect); break;
	case 3 :pDest->BltNatural(pSrc,x, y + rc.top, &rc,NULL,lpDstClipRect); break;
	}

	// ���ɕ\�������ׂ������͈����L�΂����B
	if(rc.top < 0 || rc.bottom > sy)
		return 0;
	if(Direction){
		i = 0;
		j = rc.bottom = rc.top;
		j += y;
		rc.top--;
	}else{
		i = rc.bottom + y;
		rc.top = rc.bottom;
		rc.bottom++;
		j = dy - rc.top - y;
	}
	// �����L�΂����ׂ��������c�Ɋg�傷��BFPS ���|�����ǂˁB
	sz.cy = j;
	if(rc.top < 0)
		rc.top = 0;
	if(rc.bottom > sy)
		rc.bottom = sy;
	switch (nTransMode) {
	case 0 :pDest->BltFast(pSrc,x, i, &rc, &sz,lpDstClipRect); break;
	case 1 :pDest->Blt(pSrc,x, i, &rc, &sz,lpDstClipRect); break;
	case 2 :pDest->BlendBltFastAlpha(pSrc,x, i, byFadeRate, &rc, &sz,lpDstClipRect); break;
	case 3 :pDest->BltNatural(pSrc,x, i, &rc, &sz,lpDstClipRect); break;
	}

	return 0;
}

// ���E����̃J�[�e���B
LRESULT CPlaneTransBlt::BltTransHelper1(CPlaneBase* pSrc, CPlaneBase* pDest, int x, int y,
										int nPhase, bool Direction, int nTransMode, BYTE byFadeRate,
										int WidthNum,LPRECT lpDstClipRect)
{
	CPlaneBase *lpDraw=pDest,*lpPlane=pSrc;	//	�����������O�O�G
	CheckNormal;

	const int ColumnNum = (sx + WidthNum + 1) / WidthNum;
	const int c = (nPhase * (ColumnNum + WidthNum)) / 256;
	int i, j;
	RECT rc;

	rc.top = 0;
	rc.bottom = sy;
	for(i = 0; i < ColumnNum && i < c; ++i){
		j = c - i;
		if(j > WidthNum)
			j = WidthNum;
		rc.left = i * WidthNum;
		if(Direction){
			rc.right = rc.left + j;
		}else{
			rc.right = sx - rc.left;
			rc.left = rc.right - j;
		}
		if(rc.right > sx)
			rc.right = sx - 1;
		if(rc.left < 0)
			rc.left = 0;
		switch (nTransMode) {
		case 0 :pDest->BltFast(pSrc,x + rc.left, y, &rc,NULL,lpDstClipRect); break;
		case 1 :pDest->Blt(pSrc,x + rc.left, y, &rc,NULL,lpDstClipRect); break;
		case 2 :pDest->BlendBltFastAlpha(pSrc,x + rc.left, y, byFadeRate, &rc,NULL,lpDstClipRect); break;
		case 3 :pDest->BltNatural(pSrc,x + rc.left, y, &rc,NULL,lpDstClipRect); break;
		}
	}

	return 0;
}
// �㉺�̃J�[�e���B
LRESULT CPlaneTransBlt::BltTransHelper2(CPlaneBase* pSrc, CPlaneBase* pDest, int x, int y,
										int nPhase, bool Direction, int nTransMode, BYTE byFadeRate,
										int WidthNum,LPRECT lpDstClipRect)
{
	CPlaneBase *lpDraw=pDest,*lpPlane=pSrc;	//	�����������O�O�G
	CheckNormal;

	const int ColumnNum = (sy + WidthNum + 1) / WidthNum;
	const int c = (nPhase * (ColumnNum + WidthNum)) / 256;
	int i, j;
	RECT rc;

	rc.left = 0;
	rc.right = sx;
	for(i = 0; i < ColumnNum && i < c; ++i){
		j = c - i;
		if(j > WidthNum)
			j = WidthNum;
		rc.top = i * WidthNum;
		if(Direction){
			rc.bottom = rc.top + j;
		}else{
			rc.bottom = sy - rc.top;
			rc.top = rc.bottom - j;
		}
		if(rc.bottom > sy)
			rc.bottom = sy - 1;
		if(rc.top < 0)
			rc.top = 0;
		switch (nTransMode) {
		case 0 :pDest->BltFast(pSrc, x, y + rc.top, &rc,NULL,lpDstClipRect); break;
		case 1 :pDest->Blt(pSrc, x, y + rc.top, &rc,NULL,lpDstClipRect); break;
		case 2 :pDest->BlendBltFastAlpha(pSrc, x, y + rc.top, byFadeRate, &rc,NULL,lpDstClipRect); break;
		case 3 :pDest->BltNatural(pSrc, x, y + rc.top, &rc,NULL,lpDstClipRect); break;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//�@�e�[�u���̍쐬

auto_ptrEx<CSinTable>						CPlaneTransBlt::m_sin_table;
auto_ptrEx<CPlaneTransBlt::BltTransTable>	CPlaneTransBlt::m_blt_table;

void CPlaneTransBlt::MakeBltTable(){
	if (m_sin_table==NULL){
		m_sin_table.Add();
	}
	if (m_blt_table==NULL){
		m_blt_table.Add();
	}
}

CPlaneTransBlt::BltTransTable::BltTransTable()
{
	::srand(NULL);	//	randomize�i�Œ�^�j
	for(int i = 0; i < 256; i++){
		RandTable[i] = rand() & 255;
	}
}

///////////////////////////////////////////////////////////////////////////////

LRESULT CPlaneTransBlt::DiagonalDiffusionBlt(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
											 int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	int sm;	// max size
	sm = sx+sy;
	int sr;	// rest size
	sr = sm - ((sm * nPhase) >> 8);

	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=4){
			BLTRECTOFFSET(px,py,4,2,(sr*m_blt_table->RandTable[(px+py)&0xff])>>8,(sr*m_blt_table->RandTable[(px+py+1)&0xff])>>8);
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::DiffusionCongeriesBlt1(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
						  int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	int sm;	// max size
	sm = sx+sy;
	int sr;	// rest size
	sr = sm - ((sm * nPhase) >> 8);

	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=4){
			int ex,ey;	//	�摜���S����s�N�Z���̕����������P�ʃx�N�g��*256�{
			ex = px - sx/2;
			ey = py - sy/2;
			int r;
			r = sqrt(ex*ex + ey*ey);
			if (r!=0) {
				ex = (ex<<8) / r;
				ey = (ey<<8) / r;
			}
			BLTRECTOFFSET(px,py,4,2,(sr*ex*m_blt_table->RandTable[(px+py*13)&0xff])>>16,(sr*ey*m_blt_table->RandTable[(px+py*19+1)&0xff])>>16);
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::DiffusionCongeriesBlt2(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
						  int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	int sm;	// max size
	sm = sx+sy;
	int sr;	// rest size
	sr = sm - ((sm * nPhase) >> 8);

	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=2){
			int ex,ey;	//	�摜���S����s�N�Z���̕����������P�ʃx�N�g��*256�{
			ex = px - sx/2;
			ey = py - sy/2;
			int r;
			r = sqrt(ex*ex + ey*ey);
			if (r!=0) {
				ex = (ex<<8) / r;
				ey = (ey<<8) / r;
			}
			BLTRECTOFFSET(px,py,2,2,(sr*ex*m_blt_table->RandTable[(px+py*13)&0xff])>>16,(sr*ey*m_blt_table->RandTable[(px+py*19+1)&0xff])>>16);
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::DiffusionCongeriesBlt3(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
						  int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	int sm;	// max size
	sm = sx+sy;
	int sr;	// rest size
	sr = sm - ((sm * nPhase) >> 8);

	int nP = (256-nPhase)<<1;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=4){
			int ex,ey;	//	�摜���S����s�N�Z���̕����������P�ʃx�N�g��*256�{
			ex = px - sx/2;
			ey = py - sy/2;
			int r;
			r = sqrt(ex*ex + ey*ey);
			if (r!=0) {
				ex = (ex<<8) / r;
				ey = (ey<<8) / r;
			}
			BLTRECTOFFSET(px,py,4,2
				,((sr*ex*m_blt_table->RandTable[(px+py*13)&0xff]  )>>16)*m_sin_table->Cos(nP) >>16
				,((sr*ey*m_blt_table->RandTable[(px+py*19+1)&0xff])>>16)*m_sin_table->Sin(nP) >>16
			);
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::SquashBlt(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
						  int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	int sm;	// max size
	sm = sx+sy;
	int sr;	// rest size
	sr = sm - ((sm * nPhase) >> 8);

	int nP = (256-nPhase)<<1;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=4){
			int ex,ey;	//	�摜���S����s�N�Z���̕����������P�ʃx�N�g��*256�{
			ex = px - sx/2;
			ey = py - sy/2;
			int r;
			r = sqrt(ex*ex + ey*ey);
			if (r!=0) {
				ex = ((ex<<8)*m_sin_table->Sin(nP) >>16) / r;
				ey = ((ey<<8)*m_sin_table->Cos(nP) >>16) / r;
			}
			BLTRECTOFFSET(px,py,4,2,(sr*ex) >> 8,(sr*ey) >> 8);
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::ForwardRollBlt(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
						  int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	int sm;	// max size
	sm = sx+sy;
	int sr;	// rest size
	sr = sm - ((sm * nPhase) >> 8);

	int nP = (256-nPhase)<<1;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=4){
			int ex,ey;	//	�摜���S����s�N�Z���̕����������P�ʃx�N�g��*256�{
			ex = px - sx/2;
			ey = py - sy/2;
			ex *= m_sin_table->Cos(nP>>2);
			ey *= m_sin_table->Sin(nP);
			BLTRECTOFFSET(px,py,4,2,::MulDiv(sr,ex,1<<24),::MulDiv(sr,ey,1<<24));
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::RotationBlt1(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
						  int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	int sm;	// max size
	sm = sx+sy;
	int sr;	// rest size
	sr = sm - ((sm * nPhase) >> 8);

	int ph = (256-nPhase) << 1;
	for(int py=0;py<sy;py+=2){
		for(int px=0;px<sx;px+=2){
			int ex,ey,ex2,ey2;	//	�摜���S����s�N�Z���̕����������P�ʃx�N�g��*256�{
			ex = px - sx/2;
			ey = py - sy/2;
			ex2 = (ex * m_sin_table->Cos(ph)>>16)	- (ey * m_sin_table->Sin(ph)>>16);
			ey2 = (ex * m_sin_table->Sin(ph)>>16)	+ (ey * m_sin_table->Cos(ph)>>16);
			BLTRECTOFFSET(px,py,2,2,-ex + ex2,-ey + ey2);
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::RotationBlt2(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
					 int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	int sm;	// max size
	sm = sx+sy;
	int sr;	// rest size
	sr = sm - ((sm * nPhase) >> 8);

	int ph = (256-nPhase)<<1;
	for(int py=0;py<sy;py+=4){
		for(int px=0;px<sx;px+=4){
			int ex,ey,ex2,ey2;	//	�摜���S����s�N�Z���̕����������P�ʃx�N�g��*256�{
			ex = px - sx/2;
			ey = py - sy/2;
			ex2 = (ex * m_sin_table->Cos(ph)>>16)	- (ey * m_sin_table->Sin(ph)>>16);
			ey2 = (ex * m_sin_table->Sin(ph)>>16)	+ (ey * m_sin_table->Cos(ph)>>16);
			BLTRECTOFFSET(px,py,4,4,-ex + ex2,-ey + ey2);
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::RotationBlt3(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
					 int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	int sm;	// max size
	sm = sx+sy;
	int sr;	// rest size
	sr = sm - ((sm * nPhase) >> 8);

	int ph = (256-nPhase)<<1;
	for(int py=0;py<sy;py+=8){
		for(int px=0;px<sx;px+=8){
			int ex,ey,ex2,ey2;	//	�摜���S����s�N�Z���̕����������P�ʃx�N�g��*256�{
			ex = px - sx/2;
			ey = py - sy/2;
			ex2 = (ex * m_sin_table->Cos(ph)>>16)	- (ey * m_sin_table->Sin(ph)>>16);
			ey2 = (ex * m_sin_table->Sin(ph)>>16)	+ (ey * m_sin_table->Cos(ph)>>16);
			BLTRECTOFFSET(px,py,8,8,-ex + ex2,-ey + ey2);
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::RotationBlt4(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
					 int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	int sm;	// max size
	sm = sx+sy;
	int sr;	// rest size
	sr = sm - ((sm * nPhase) >> 8);

	int ph = (256-nPhase)<<1;
	for(int py=0;py<sy;py+=4){
		for(int px=0;px<sx;px+=4){
			int ex,ey,ex2,ey2;	//	�摜���S����s�N�Z���̕����������P�ʃx�N�g��*256�{
			ex = px - sx/2;
			ey = py - sy/2;
			ex2 = (ex * m_sin_table->Cos(ph)>>16)	- (ey * m_sin_table->Sin(ph)>>16);
			ey2 = (ex * m_sin_table->Sin(ph)>>16)	+ (ey * m_sin_table->Cos(ph)>>16);
			int r;
			r = sqrt(ex*ex + ey*ey);
			if (r!=0) {
				ex = (ex<<8) / r;
				ey = (ey<<8) / r;
			}
			BLTRECTOFFSET(px,py,4,4
				,-ex + ex2 + (((sr*ex*m_blt_table->RandTable[(px+py*13)&0xff])>>16)	 *m_sin_table->Cos(ph)>>16)
				,-ey + ey2 + (((sr*ey*m_blt_table->RandTable[(px+py*19+1)&0xff])>>16)*m_sin_table->Sin(ph)>>16)
			);
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::EnterUpBlt1(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
					 int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	for(int py=0;py<sy;py+=8){
		for(int px=0;px<sx;px+=8){
			int hy;
			hy = (nPhase + (m_blt_table->RandTable[((px>>3)+py*13)&255] >> 1))>>4;
			hy -= 8;
			if (hy>0) {
				if (hy>8) hy = 8;
				BLTRECTOFFSET(px,py,8,hy,0,8-hy);
			}
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::EnterUpBlt2(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
					int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	for(int py=0;py<sy;py+=16){
		for(int px=0;px<sx;px+=16){
			int hy;
			hy = (nPhase + (m_blt_table->RandTable[((px>>3)+py*13)&255] >> 1))>>3;
			hy -= 16;
			if (hy>0) {
				if (hy>16) hy = 16;
				BLTRECTOFFSET(px,py,16,hy,0,16-hy);
			}
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::CellGatherBlt1(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
					int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	int ph;
	ph = (nPhase >> 1) + 1;
	for(int py=0;py<sy;py+=4){
		for(int px=0;px<sx;px+=4){
			int oox,ooy;
			oox = m_blt_table->RandTable[((px>>2)+(py>>2)*13)&255] - 128;
			ooy = m_blt_table->RandTable[((px>>2)+(py>>2)*19)&255] - 128;
			oox = oox / ph;
			ooy = ooy / ph;
			BLTRECTOFFSET(px,py,4,4,oox,ooy);
		}
	}

	return 0;
}

LRESULT CPlaneTransBlt::CellGatherBlt2(CPlaneBase* lpDraw, CPlaneBase* lpPlane,
					   int x, int y, int nPhase, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect)
{
	CheckNormal;
	MakeBltTable();

	int ph;
	ph = 256-nPhase;
	for(int py=0;py<sy;py+=4){
		for(int px=0;px<sx;px+=4){
			int oox,ooy;
			oox = m_blt_table->RandTable[((px>>2)+(py>>2)*13)&255] - 128;
			ooy = m_blt_table->RandTable[((px>>2)+(py>>2)*19)&255];
			oox = oox * ph >> 11;
			ooy = ooy * ph >> 11;
			BLTRECTOFFSET(px,py,4,4,oox,ooy);
		}
	}

	return 0;
}