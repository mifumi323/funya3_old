#include "stdafx.h"
#include "yaneImeBase.h"
#include "yaneAppInitializer.h"

//	staticな変数
int CIMEBase::m_nStat = 0;
//	現在の状態 0:不定 1:Show 2:Hide
//	⇒連続的な呼び出しを防止する

//	2001/07/31 sohei
//		http://www.hcn.zaq.ne.jp/no-ji/reseach/980715.htm
//		を参考にさせて頂きました
//
#include "winnls32.h"	//	Win3.1時代の遺産＾＾；

void	CIMEBase::Show(void){
	if (m_nStat==1) return ;
	m_nStat = 1;
	::WINNLSEnableIME(CAppInitializer::GetHWnd(),TRUE);		 // ＩＭＥを現す
}

void	CIMEBase::Hide(void) {
	if (m_nStat==2) return ;
	m_nStat = 2;
	::WINNLSEnableIME(CAppInitializer::GetHWnd(),FALSE);	 // ＩＭＥを消す
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