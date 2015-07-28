#include "stdafx.h"

#ifdef USE_DirectDraw

#include "yaneTextPlane.h"

CTextPlane::CTextPlane(void){

	m_nTextX = 0;
	m_nTextY = 0;

	m_bHybrid = true;	//	���̃v���[����duplicate���Ȃ��B

}

LRESULT	CTextPlane::UpdateText(void){
	//	�r�b�g�}�b�v��ǂݍ���ł��Ȃ���΃}�C�v���[������
	if (!m_bBitmap) {
		int sx,sy;
		m_Font.GetSize(sx,sy);
		// m_Font�̃e�L�X�g����ł��v���[�������D
		if ((sx == 0) || (sy == 0)) {
			sx = sy = 1;
		}
		if (CreateSurface(sx,sy)) return 1;
//		EnableBlendColorKey(true);
	}

	m_bOwnerDraw = true;	//	�������Ăяo����Ȃ����ƂɂႢ���Ȃ��O�O

	HDC hdc = GetDC();
	if (hdc==NULL) return 1;

	m_Font.OnDraw(hdc,m_nTextX,m_nTextY);

	ReleaseDC();
	return 0;
}

#endif // USE_DirectDraw