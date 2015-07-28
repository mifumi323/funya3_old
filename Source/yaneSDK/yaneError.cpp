#include "stdafx.h"
#include "yaneFile.h"
#include <time.h>		// debug�p�ɓ��t�����̏o�͂�������:p

//	�G���[�o�͂̂��߂̃O���[�o���I�u�W�F�N�g
CErrorLog Err;

#ifdef USE_ErrorLog

//////////////////////////////////////////////////////////////////////////////

CErrorLog::CErrorLog(void) {
#ifndef _DEBUG
	m_nDevice = 0;	//	�f�o�b�O���łȂ��Ȃ�΃f�B�t�H���g�ł͂ǂ��ɂ������Ȃ�
#else
	m_nDevice = 1;	//	�f�o�b�O���Ȃ�΃f�B�t�H���g�ł̓t�@�C���ɏ����o��
#endif
}

CErrorLog::~CErrorLog(){
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CErrorLog::Out(string s)
{
	m_oErrorMes = s;	// �G���[���O���c���B

	CHAR buf[32],tbuf[32],buf2[256];
	_strdate(buf);	// ���t
	_strtime(tbuf); // ����
	wsprintf(buf2,"%s %s : %s\n",buf,tbuf,s.c_str());

	switch (m_nDevice) {
		case 0 : return 0; // null device
		case 1 : {
			LRESULT result = 0;
			{
				CFile file;
				if (file.Open("ErrorLog.txt","aw+")) {
					result = 1;
				}
				if (file.Write(buf2)) {
					result = 1;
				}
			}
			return result;
		}
	}
	return 0;
}

LRESULT CErrorLog::Out(int i){
	CHAR buf[16];
	::wsprintf(buf,"%d",i);
	return Out(buf);
}

LRESULT __cdecl CErrorLog::Out( LPSTR fmt, ... ) {
	CHAR buf[512];
	::wvsprintf(buf,fmt,(LPSTR)(&fmt+1));
	return Out((string)buf);
}

string CErrorLog::GetError(void) const {
	return m_oErrorMes;
}

LRESULT	CErrorLog::SelectDevice(int n){
	if (m_nDevice==n) return 0;
	m_nDevice = n;
	return 0;
}

#endif // USE_ErrorLog || _DEBUG