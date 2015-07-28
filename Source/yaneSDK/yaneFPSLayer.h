//	yaneFPSLayer.h :
//
//		FPS displaying

#ifndef __yaneFPSLayer_h__
#define __yaneFPSLayer_h__

#include "yaneLayer.h"
#include "yaneFont.h"
#include "yaneFPSTimer.h"

class CFPSLayer : public CHDCLayer {
public:
	virtual void	OnDraw(HDC hdc);

	void SetSkipFrameDraw(bool b) { m_bSkipFrameDraw = b; }

	CFPSLayer(CFPSTimer*);
	virtual ~CFPSLayer();

protected:
	CFont		m_Font;	//	�����ŕ`��
	CFPSTimer*	m_lpFpsTime;
	bool		m_bSkipFrameDraw;	//	�X�L�b�v�t���[������`�悷��̂�(*false)
};

#endif