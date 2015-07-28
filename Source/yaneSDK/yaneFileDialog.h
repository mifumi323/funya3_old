// yaneFileDialog.h
//
//	���̃N���X�A�ˊэH���ō��������A�}���`�X���b�h�Ή����ア�B
//	���̂ւ񒍈ӂ��Ďg�����ƁB

#ifndef __yaneFileDialog_h__
#define __yaneFileDialog_h__

#include <commdlg.h>
#pragma comment(lib,"comdlg32.lib")

class CFileDialog {
public:
	LRESULT GetOpenFile(LPSTR filename);
	// filaname�o�b�t�@��256�o�C�g�ȏ�m�ۂ��Ă�������
	LRESULT	GetSaveAsFile(LPSTR filename);
	// ���O�����ĕۑ���I�ԁBfilename�o�b�t�@��256�o�C�g�ȏ�m�ۂ��Ă������ƁB

	//////////////////////////////////////////////////////////////////////

	void SetFlag(DWORD flag);			// �ǂ�ȃt�@�C�����J���̂��ݒ肷��
	void SetFillter(LPCTSTR p,LPTSTR q);
	void SetExt(LPSTR p);				// �f�B�t�H���g�g���q�̐ݒ�
	OPENFILENAME * GetOfn(void);		// �ň��A���꒼�ڂ�������:p

	//////////////////////////////////////////////////////////////////////

	CFileDialog(void);
	virtual ~CFileDialog();
	//////////////////////////////////////////////////////////////////////
protected:
	OPENFILENAME ofn;
	DWORD	m_flag;
};

#endif
