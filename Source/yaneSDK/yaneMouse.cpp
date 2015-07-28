#include "stdafx.h"
#include "yaneMouse.h"
#include "yaneAppInitializer.h"

CMouse::CMouse(){
	m_bOutScreenInput = false;
	m_bRB = false;
	m_bLB = false;
	m_bHistRB = false;
	m_bHistLB = false;
	CAppInitializer::Hook(this);		//	WindowMessage��Hook
}

CMouse::~CMouse(){
	CAppInitializer::Unhook(this);		//	WindowMessage��Unhook
}

//////////////////////////////////////////////////////////////////////////////

LRESULT	CMouse::GetXY(int &x,int &y) const {
	POINT point,point2 = { 0,0 };
	::GetCursorPos(&point);

	if (CAppInitializer::IsFullScreen()) {
	//	�t���X�N���[�����[�h�ł͌����Ă��Ȃ��E�B���h�D�L���v�V�������̉e����
	//	�R�̈ʒu�ɂȂ��Ă��܂��̂ŁA���̂܂ܕԂ��Ă����Ηǂ��B
		x = point.x;
		y = point.y;
	} else {
		HWND hWnd = CAppInitializer::GetHWnd();
		::ClientToScreen(hWnd,&point2);
		x = point.x - point2.x;
		y = point.y - point2.y;

		if (!m_bOutScreenInput) {
			//	�E�B���h�D�͈͊O�ł���΁A�{�^���̓��Z�b�g
			::ScreenToClient(hWnd,&point);// �N���C�A���g���W�ɕϊ�
			RECT rt;
			::GetClientRect(hWnd,&rt);// �N���C�A���g�̈�擾
			// �͈͊O�Ȃ�}�E�X��ԏ�����
			if(point.x<0 || rt.right<=point.x || point.y<0 || rt.bottom<=point.y) {
				//	const_cast fake
				*const_cast<bool*>(&m_bLB) = false;
				*const_cast<bool*>(&m_bRB) = false;
			}
		}
		
	}
	return 0;
}

LRESULT CMouse::SetXY(int x,int y) {
	POINT point,point2 = { 0,0 };

	if (CAppInitializer::IsFullScreen()) {
		point.x=x;
		point.y=y;
		::SetCursorPos(point.x,point.y);
	} else {
		::ClientToScreen(CAppInitializer::GetHWnd(),&point2);
		point.x= x + point2.x;
		point.y= y + point2.y;
		::SetCursorPos(point.x,point.y);
	}
	return 0;
}

LRESULT CMouse::GetInfo(int &x,int &y,int &b) const {	// �}�E�X�|�W�V�����ƃ{�^����Ԃ�Ԃ�
	CMouse::GetXY(x,y);	//	virtual�֐����Ăяo���ꂯ�Ă͉����Ȃ�Ȃ�

	b = 0;
	if (m_bRB) b++;		//	�{�^�����B�E�N���b�N�Ȃ��1
	if (m_bLB) b+=2;	//	�{�^�����B���N���b�N�Ȃ��2

	return 0;
}

bool CMouse::RButton() const {
	return m_bRB;
}

bool CMouse::LButton() const {
	return m_bLB;
}

void CMouse::SetOutScreenInput(bool bEnable) { m_bOutScreenInput=bEnable;}

//////////////////////////////////////////////////////////////////////////////

void	CMouse::GetButton(bool&bL,bool&bR){
	bL = !m_bHistLB && m_bLB;
	bR = !m_bHistRB && m_bRB;
	m_bHistLB = m_bLB;
	m_bHistRB = m_bRB;
}

void	CMouse::ResetButton(){
	m_bHistRB = false;
	m_bHistLB = false;
}

//////////////////////////////////////////////////////////////////////////////
// ���b�Z�[�W���t�b�N���Ȃ��ƃ}�E�X�̃{�^����Ԃ��킩���Ƃ�...
LRESULT CMouse::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){ // ���b�Z�[�W�̃R�[���o�b�N
	switch(uMsg){
	/*
		�������܂܉�ʊO�ɏo���Ƃ��ɁA�ǂ���������̂��H
		(��ʊO�ł�WM_MOUSEMOVE���b�Z�[�W�͂���ė��Ȃ��̂�..)
		��WM_NCBUTTONUP���͂���Ă���H�@�˂���ė��Ȃ��݂���(��_�G)�
	*/

/*
	//	�˂��̃��b�Z�[�W���K�����ŗ���Ƃ͌���Ȃ��悤���D�D
	case WM_NCHITTEST:
	{
		POINT pos;
		RECT rt;
		pos.x = LOWORD(lParam);
		pos.y = HIWORD(lParam);
		::ScreenToClient(hWnd,&pos);// �N���C�A���g���W�ɕϊ�
		GetClientRect(hWnd,&rt);// �N���C�A���g�̈�擾
		// �͈͊O�Ȃ�}�E�X��ԏ�����
		if(pos.x<0 || rt.right<=pos.x || pos.y<0 || rt.bottom<=pos.y) {
			m_bLB = false;
			m_bRB = false;
		}
		break;
	}
*/

	case WM_LBUTTONDOWN:
//	case WM_NCLBUTTONDOWN:
		m_bLB = true; break;
	case WM_LBUTTONUP:
//	case WM_NCLBUTTONUP:
		m_bLB = /* m_bLDC = */ false; break;
	case WM_RBUTTONDOWN:
//	case WM_NCRBUTTONDOWN:
		m_bRB = true; break;
	case WM_RBUTTONUP:
//	case WM_NCRBUTTONUP:
		m_bRB = /* m_bRDC = */ false; break;
/*	//	���̃��b�Z�[�W�͔��ł��Ȃ�
	case WM_LBUTTONDBLCLK:
//	case WM_NCLBUTTONDBLCLK:
		m_bLDC= true; break;
	case WM_RBUTTONDBLCLK:
//	case WM_NCRBUTTONDBLCLK:
		m_bRDC= true; break;
*/
	case WM_MOUSEMOVE:
//	case WM_NCMOUSEMOVE:	//	��ʊO�Ƀ}�E�X���o���Ƃ��̂��߂�
		m_bLB = (wParam & MK_LBUTTON)!=0;
		m_bRB = (wParam & MK_RBUTTON)!=0;
		break;

	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

CMouseEx::CMouseEx(){
	m_bRBN = false;
	m_bLBN = false;
	m_nRLBN = 0;
	m_nX = m_nY = 0;
	m_nGuardTime = 2;
}

CMouseEx::~CMouseEx(){
}

LRESULT	CMouseEx::Flush(){
	if (m_nGuardTime!=0) m_nGuardTime--;

	m_bHistLB = m_bLBN;
	m_bHistRB = m_bRBN;

	if (CMouse::GetInfo(m_nX,m_nY,m_nRLBN)!=0) {
		m_bRBN = m_bLBN = false;
		return 1;
	}
	m_bRBN = (m_nRLBN&1)!=0;
	m_bLBN = (m_nRLBN&2)!=0;

	return 0;
}

LRESULT CMouseEx::GetXY(int &x,int &y)const{
	x = m_nX;
	y = m_nY;
	return 0;
}

LRESULT CMouseEx::SetXY(int x,int y){
					//	�}�E�X���w��̃|�W�V�����Ɉړ��i�N���C�A���g���W�n�ɂāj
	m_nX = x; m_nY = y;	//	���܂������X�V�O�O�G
	return CMouse::SetXY(x,y);
}

bool	CMouseEx::RButton()const{
	return m_bRBN;
}

bool	CMouseEx::LButton()const{
	return m_bLBN;
}

LRESULT CMouseEx::GetInfo(int &x,int &y,int &b)const{
	x = m_nX;
	y = m_nY;
	b = m_nRLBN;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//	�O���Flush���牟���ꂽ���H

void	CMouseEx::GetButton(bool&bL,bool&bR){
	bR = !m_bHistRB && m_bRBN;
	bL = !m_bHistLB && m_bLBN;
}
bool	CMouseEx::IsPushRButton() const{
	return !m_bHistRB && m_bRBN && (m_nGuardTime==0);
}
bool	CMouseEx::IsPushLButton() const{
	return !m_bHistLB && m_bLBN && (m_nGuardTime==0);
}

/////////////////////////////////////////////////////////////////////////////
//	�O���Flush���牟���グ��ꂽ���H

void	CMouseEx::GetUpButton(bool&bL,bool&bR){
	bR = m_bHistRB && !m_bRBN;
	bL = m_bHistLB && !m_bLBN;

}
bool	CMouseEx::IsPushUpRButton()const{
	return m_bHistRB && !m_bRBN && (m_nGuardTime==0);
}
bool	CMouseEx::IsPushUpLButton()const{
	return m_bHistLB && !m_bLBN && (m_nGuardTime==0);
}

/////////////////////////////////////////////////////////////////////////////

void	CMouseEx::ResetButton(){
	m_bHistRB = m_bRBN = false;
	m_bHistLB = m_bLBN = false;
}

void	CMouseEx::SetGuardTime(int nTime){
	m_nGuardTime = nTime;
}

bool	CMouseEx::IsGuardTime() const {
	return m_nGuardTime!=0;
}