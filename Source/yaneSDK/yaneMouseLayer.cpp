#include "stdafx.h"
#include "yaneMouseLayer.h"
#include "yanePlaneBase.h"
#include "yaneAppManager.h"
#include "yaneAppInitializer.h"

CMouseLayer::CMouseLayer(smart_ptr<CPlaneBase> v,int x,int y){
	SetPlane(v,x,y);
	m_hWnd	=	CAppInitializer::GetHWnd();
	//--- �ǉ� '02/02/01  by ENRA ---
	memset(&m_LastDrawRect, 0, sizeof(m_LastDrawRect));
	//-------------------------------
}

void CMouseLayer::SetPlane(smart_ptr<CPlaneBase> v,int x,int y){
	m_bEnable = false;
	m_bShow	  = true;
	Enable(true);
	m_vPlane = v;
	m_nX = x;
	m_nY = y;
	//--- �ǉ� '02/02/01  by ENRA ---
	m_vPlane->GetSize(m_nWidth, m_nHeight);
	//-------------------------------
}

CMouseLayer::CMouseLayer(void){
	m_bEnable = false;
	m_bShow	  = true;
	m_nX = 0;
	m_nY = 0;
	m_hWnd	=	CAppInitializer::GetHWnd();
	//--- �ǉ� '02/02/01  by ENRA ---
	memset(&m_LastDrawRect, 0, sizeof(m_LastDrawRect));
	m_nWidth = 0;
	m_nHeight = 0;
	//-------------------------------
}

CMouseLayer::~CMouseLayer() {
	Enable(false);
}

void	CMouseLayer::Enable(bool bEnable){
	if (m_bEnable == bEnable) return ;
	m_bEnable = bEnable;
	InnerShowCursor(!bEnable);
}

void	CMouseLayer::InnerShowCursor(bool bShow){
	if (m_bShow==bShow) return ;
	m_bShow = bShow;
	CAppManager::GetMyApp()->GetMyWindow()->UseMouseLayer(!bShow);
}

void CMouseLayer::InnerOnDraw(CPlaneBase* lpDraw){

	if (!m_bEnable) return ;

	//	CMouse���C���X�g�[�������͂��邪�A
	//	�}�E�X���W�𓾂邾���Ȃ̂ŁA�����܂ł��鉿�l�͖���

	// �\�t�g�E�F�A�ɂ��J�[�\���\���i�T���v���j
	POINT point,point2 = { 0,0 };
	::GetCursorPos(&point);

	if (!CAppInitializer::IsFullScreen()){
		::ClientToScreen(m_hWnd,&point2);
		if (point.y - point2.y <0){
			InnerShowCursor(true);
			return ;
		} else {
			if (::GetActiveWindow()==m_hWnd) {
		//						������Ƀ��C���E�C���h�E���A�N�e�B�u(�_�C�A���O�Ȃǂ��Ăяo����ĂȂ�)
				InnerShowCursor(false);
			} else {
				InnerShowCursor(true);
				return ;
			}
		}
	} else {
		//	�t���X�N���[������menu�\�����Ă�̂��H
		POINT point;
		::GetCursorPos(&point);
		//	���̒萔�킩���B
		int nMenu=0;
#ifdef USE_DIB32
		CDIBDraw* lpDIBDraw = CAppManager::GetMyDIBDraw();
		//	DIBDraw�Ȃ�΃��j���[���o���Ă��邩���O�O�G
		if (lpDIBDraw!=NULL) {
			nMenu = lpDIBDraw->GetMenuHeight();
		}
#endif
#ifdef USE_FastDraw
		CFastDraw* lpFastDraw = CAppManager::GetMyFastDraw();
		//	FastDraw�Ȃ�΃��j���[���o���Ă��邩���O�O�G
		if (lpFastDraw!=NULL) {
			nMenu = lpFastDraw->GetMenuHeight();
		}
#endif
#if (defined(USE_DIB32) || defined(USE_FastDraw))
		int y = point.y; // y��0�����̉\����r��
		if (y>=0 && y<nMenu) {
			InnerShowCursor(true);
			return ;
		} else {
			InnerShowCursor(false);
		}
#else
		InnerShowCursor(false);
#endif
	}

	//--- �ǉ� '02/02/01  by ENRA ---
	// DirtyRect�΍�^^;
	m_LastDrawRect.left = point.x - point2.x - m_nX;
	m_LastDrawRect.top = point.y - point2.y - m_nY;
	m_LastDrawRect.right = m_LastDrawRect.left + m_nWidth;
	m_LastDrawRect.bottom = m_LastDrawRect.top + m_nHeight;
	//-------------------------------
	lpDraw->BltNatural(m_vPlane, m_LastDrawRect.left, m_LastDrawRect.top);
}