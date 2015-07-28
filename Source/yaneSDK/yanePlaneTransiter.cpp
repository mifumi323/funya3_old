#include "stdafx.h"
#include "yanePlaneTransiter.h"

void	CPlaneTransiter::Inc(){
	m_nX++;	m_nY++;
	m_nT++;	m_nR++;
}

void	CPlaneTransiter::Dec(){
	m_nX--;	m_nY--;
	m_nT--;	m_nR--;
}

void CPlaneTransiter::OnDraw(CPlaneBase*lp){
	int x,y;
	m_lpPlane->GetSize(x,y);
	double r=1;
//	if ( GetR()->GetEnd() != 0 ){
//		r = *GetR() / GetR()->GetEnd();
//	}
	SIZE s = { x, y };
	lp->BlendBlt(m_lpPlane,m_nX,m_nY,RGB(255-m_nT,255-m_nT,255-m_nT),RGB(m_nT,m_nT,m_nT),NULL,NULL);
}