//	yaneTextPlane.h :

#ifdef USE_DirectDraw

#ifndef __yaneTextPlane_h__
#define __yaneTextPlane_h__

#include "yaneFont.h"
#include "yanePlane.h"

class CTextPlane : public CPlane {
public:

	CFont*	GetFont(void) { return &m_Font; }
	LRESULT	UpdateText(void);
	void	SetTextPos(int x,int y){ m_nTextX=x; m_nTextY=y; }

	CTextPlane(void);

protected:
	CFont		m_Font;				//	�����ŕ`��
	int			m_nTextX,m_nTextY;	//	�e�L�X�g�`��ʒu
	virtual LRESULT OnDraw(void){ return UpdateText(); }
};

#endif

#endif // USE_DirectDraw