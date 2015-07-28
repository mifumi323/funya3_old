#include "stdafx.h"

#include "yaneFPSLayer.h"

//////////////////////////////////////////////////////////////////////////////

CFPSLayer::CFPSLayer(CFPSTimer*lpFpsTimer){
	m_lpFpsTime = lpFpsTimer;
	m_Font.SetSize(25);
	m_Font.SetColor(RGB(128,192,128));
	m_Font.SetBackColor(RGB(64,128,64));
	m_bSkipFrameDraw = false;
}

CFPSLayer::~CFPSLayer(){
}

void	CFPSLayer::OnDraw(HDC hdc){
	CHAR	buf[64];
	int nRealFps = m_lpFpsTime->GetRealFPS();
	int nFps = m_lpFpsTime->GetFPS();
	int nSkip = m_lpFpsTime->GetSkipFrame();

	//	max fps�̂Ƃ��ɐ������u���ĕ\�������̂�␮
/*
	if ((nRealFps == nFps -1) && (nRealFps>=1)){
		//	�P�������Ȃ��̂́A���e�덷
		nRealFps = nFps;
	} else
*/
	if (nRealFps > nFps) {
		//	�ݒ肳�ꂽFps����I�[�o�[�V���[�g���Ă���
		nRealFps = nFps;
	}

	if (!m_bSkipFrameDraw){
		::wsprintf(buf,"FPS:%d/%d",nRealFps,nFps);
	} else {
		::wsprintf(buf,"FPS:%d/%d(SkipFrame %d)",nRealFps,nFps,nSkip);
	}
	m_Font.SetText(buf);
	m_Font.OnDraw(hdc,m_nX,m_nY);
}