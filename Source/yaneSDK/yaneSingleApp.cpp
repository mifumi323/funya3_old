#include "stdafx.h"
#include "yaneSingleApp.h"
#include "yaneAppInitializer.h"
#include "yaneFindWindow.h"

CSingleApp::CSingleApp(void) {

	//	�N��Exe�����A���d�N���h�~��Mutex���Ƃ���
	CHAR szFileName[_MAX_PATH],szFileName2[_MAX_PATH];
	::GetModuleFileName(CAppInitializer::GetInstance(),szFileName,_MAX_PATH);
	::lstrcpy(szFileName2,szFileName);	//	�R�s�[���Ď����Ă���

	//	Mutex���ɂ�\�͎g���Ȃ��̂�+�ɒu��
	for(int i=0;i<_MAX_PATH;i++){
		if (szFileName[i]=='\\') szFileName[i] = '+'; // + �̓t�@�C�����ɂ͏o�����Ȃ��̂łn�j
		else if (szFileName[i]=='\0') break;
	}

	if (m_oMutex.Open(szFileName)) {	// ��������...

		//	���̋N���t�@�C���Ɠ����̎��s�t�@�C����T���D�D�D
		//	�������AWindowClass����������Έ�ӂɓ���ł��Ȃ��悤��...

		CFindWindow fw;
		fw.Find(szFileName2);

		m_bValid = false;
	} else {
		m_bValid = true;
	}
}

bool	CSingleApp::IsValid(void){
	return m_bValid;
}