#include "stdafx.h"
#include "yaneImeBase.h"
#include "yaneAppInitializer.h"

//	static�ȕϐ�
int CIMEBase::m_nStat = 0;
//	���݂̏�� 0:�s�� 1:Show 2:Hide
//	�˘A���I�ȌĂяo����h�~����

//	2001/07/31 sohei
//		http://www.hcn.zaq.ne.jp/no-ji/reseach/980715.htm
//		���Q�l�ɂ����Ē����܂���
//
#include "winnls32.h"	//	Win3.1����̈�Y�O�O�G

void	CIMEBase::Show(void){
	if (m_nStat==1) return ;
	m_nStat = 1;
	::WINNLSEnableIME(CAppInitializer::GetHWnd(),TRUE);		 // �h�l�d������
}

void	CIMEBase::Hide(void) {
	if (m_nStat==2) return ;
	m_nStat = 2;
	::WINNLSEnableIME(CAppInitializer::GetHWnd(),FALSE);	 // �h�l�d������
}

/*
HIMC	CIMEBase::m_hIME = NULL;

void	CIMEBase::Show(void){
	if (m_hIME!=NULL) {
		::ImmAssociateContext(NULL,m_hIME);		// IME On
	}
}

void	CIMEBase::Hide(void) {
	if (m_hIME==NULL) {
		m_hIME=::ImmAssociateContext(NULL,NULL);	// IME Off
	}
}
*/