#include "stdafx.h"

#include "yaneFileDialog.h"
#include "yaneAppInitializer.h"

LRESULT CFileDialog::GetOpenFile(LPSTR filename){
	// �t�@�C���_�C�A���O���o��
	::ZeroMemory(filename,256);
	ofn.lpstrFile	= filename;
	ofn.Flags		= OFN_FILEMUSTEXIST;

//	ysDraw.ShowCursor(true);	// �\�t�g�E�F�A�}�E�X�J�[�\���s��

	if (::GetOpenFileName(&ofn)) {
		return 0;
	} else {
		return 1;
	}
}

LRESULT CFileDialog::GetSaveAsFile(LPSTR filename){
	// �t�@�C���_�C�A���O���o��
	::ZeroMemory(filename,256);
	ofn.lpstrFile	= filename;
	ofn.Flags		= OFN_OVERWRITEPROMPT;

//	ysDraw.ShowCursor(true);	// �\�t�g�E�F�A�}�E�X�J�[�\���s��

	if (::GetSaveFileName(&ofn)) {
		return 0;
	} else {
		return 1;
	}
}

//////////////////////////////////////////////////////////////////////////////

void CFileDialog::SetFillter(LPCTSTR p,LPTSTR q){
	ofn.lpstrFilter = p;
	ofn.lpstrCustomFilter = q;
}

void CFileDialog::SetFlag(DWORD dw){
	m_flag = dw;
}

OPENFILENAME * CFileDialog::GetOfn(void){
	return &ofn;
}

void CFileDialog::SetExt(LPSTR p){
	ofn.lpstrDefExt	= p;
}

//////////////////////////////////////////////////////////////////////////////

CFileDialog::CFileDialog(void){
	m_flag = 0;
	ZERO(ofn);
	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= CAppInitializer::GetHWnd();
	ofn.lpstrFilter		= "All files {*.*}\0*.*\0\0";
	ofn.lpstrDefExt		= "txt";
	ofn.nMaxFile		= 256;
}

CFileDialog::~CFileDialog(){}

