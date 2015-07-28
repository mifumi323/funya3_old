#include "stdafx.h"
#include "yaneSerialize.h"
#include "yaneFile.h"
	//	�N���X�̓Ɨ������ቺ���邩��A
	//	���������̗p�ӂ������Ȃ��񂾂��ȁA�A�A

void	CSerialize::Clear(){
	m_abyData.clear();	//	�X�g���[���̃N���A
	m_nDataPos = 0;		//	�擪���w������
	m_bStoring = true;	//	�i�[������
}

//	����Ȃ�ł����񂩂��ȁD�D
CSerialize& CSerialize::operator << (CArchive& vData){
	vData.Serialize(*this);
	return *this;
}

//	--- for storing or unstoring primitive data
CSerialize& CSerialize::operator << (BYTE& byData){
	if (IsStoring()){
	//	�ۑ�����񂩂��ȁH
		GetData()->push_back(byData);
	} else {
	//	���o���񂩂��ȁH
		byData = (*GetData())[m_nDataPos];
		m_nDataPos++;
	}
	return *this;
}

CSerialize& CSerialize::operator << (int& nData){
	if (IsStoring()){
	//	�ۑ�����񂩂��ȁH
		//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
		int nSize = sizeof(int);
		BYTE* pByte = (BYTE*)&nData;
		for(int i=0;i<nSize;i++){
			GetData()->push_back(pByte[i]);
		}
	} else {
	//	�X�g���[��������o���񂩂��ȁH
		//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
		int nSize = sizeof(int);
		BYTE* pByte = (BYTE*)&nData;
		for(int i=0;i<nSize;i++){
			pByte[i] = (*GetData())[m_nDataPos++];
		}
	}
	return *this;
}

CSerialize& CSerialize::operator << (bool& bData){
	if (IsStoring()){
	//	�ۑ�����񂩂��ȁH
		//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
		int nSize = sizeof(bool);
		BYTE* pByte = (BYTE*)&bData;
		for(int i=0;i<nSize;i++){
			GetData()->push_back(pByte[i]);
		}
	} else {
	//	�X�g���[��������o���񂩂��ȁH
		//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
		int nSize = sizeof(bool);
		BYTE* pByte = (BYTE*)&bData;
		for(int i=0;i<nSize;i++){
			pByte[i] = (*GetData())[m_nDataPos++];
		}
	}
	return *this;
}

CSerialize& CSerialize::operator << (string& szData){
	if (IsStoring()){
	//	�ۑ�����񂩂��ȁH
		//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
		BYTE* pByte = (BYTE*)szData.c_str();
		int nSize = szData.size() + 1;
		//	string�����Ɏ�������Ă��NULL�͊܂܂ւ�˂�..
		//	���ꂵ�����Astring�̃G�������g��char���Ɖ��肵�Ă���̂�
		//	�{���͖�肪����..���̂ւ�unicode�Ή��̂Ƃ��ɏC�����ׂ�

		//	NULL�̃o�C�g���܂߂�K�v������
		for(int i=0;i<nSize;i++){
			GetData()->push_back(pByte[i]);
		}
	} else {
	//	�X�g���[��������o���񂩂��ȁH
		//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
		int nSize = ::strlen((LPCSTR)&(*GetData())[m_nDataPos]) + 1;
		szData = (LPCSTR)(&(*GetData())[m_nDataPos]);
		m_nDataPos += nSize;
	}
	return *this;
}

CSerialize& CSerialize::operator << (vector<int>& anData){
	int n;
	if (IsStoring()) {
		n = anData.size();
		(*this) << n;	//	�T�C�Y�����łɕۑ����Ȃ��ẮI
	} else {
		(*this) << n;	//	�����𕜌�
		anData.resize(n);
	}
	for(int i=0;i<n;i++){
		(*this) << anData[i];
	}
	return *this;
}

CSerialize& CSerialize::operator << (vector<bool>& abData){
	int n;// = abData.size();
// �ǉ�
	if (IsStoring()) {
		n = abData.size();
		(*this) << n;	//	�T�C�Y�����łɕۑ����Ȃ��ẮI
	} else {
		(*this) << n;	//	�����𕜌�
		abData.resize(n);
	}
// �ǉ��I���
	for(int i=0;i<n;i++){
		(*this) << abData[i];
	}
	return *this;
}

CSerialize& CSerialize::operator << (vector<BYTE>& abyData){
	int n;
	if (IsStoring()) {
		n = abyData.size();
		(*this) << n;	//	�T�C�Y�����łɕۑ����Ȃ��ẮI
	} else {
		(*this) << n;	//	�����𕜌�
		abyData.resize(n);
	}
	for(int i=0;i<n;i++){
		*this << abyData[i];
	}
	return *this;
}

CSerialize& CSerialize::operator << (vector<string>& szData){
	int n;
	if (IsStoring()) {
		n = szData.size();
		(*this) << n;	//	�T�C�Y�����łɕۑ����Ȃ��ẮI
	} else {
		(*this) << n;	//	�����𕜌�
		szData.resize(n);
	}
	for(int i=0;i<n;i++){
		*this << szData[i];
	}
	return *this;
}

// �ǉ�
CSerialize& CSerialize::operator << (vector<WORD>& awData){
	int n;
	if (IsStoring()) {
		n = awData.size();
		(*this) << n;	//	�T�C�Y�����łɕۑ����Ȃ��ẮI
	} else {
		(*this) << n;	//	�����𕜌�
		awData.resize(n);
	}
	for(int i=0;i<n;i++){
		*this << awData[i];
	}
	return *this;
}

CSerialize& CSerialize::operator << (vector<DWORD>& adwData){
	int n;
	if (IsStoring()) {
		n = adwData.size();
		(*this) << n;	//	�T�C�Y�����łɕۑ����Ȃ��ẮI
	} else {
		(*this) << n;	//	�����𕜌�
		adwData.resize(n);
	}
	for(int i=0;i<n;i++){
		*this << adwData[i];
	}
	return *this;
}
// �ǉ��I���

//	�������g���I�O�O�G
CSerialize& CSerialize::operator << (CSerialize& vData){
	//	vector<BYTE>�Ȃ̂ŁA���Ȃ��V���A���C�Y�ł���
	*this << *vData.GetData();
	*this << vData.m_bStoring;	//	�i�[����
	*this << vData.m_nDataPos;	//	�|�W�V����
	return *this;
}

//	�����āA�V���A���C�Y�Ԃ̃R�s�[�I
CSerialize& CSerialize::operator = (CSerialize& vSeri){
	//	vector�̃R�s�[�Ȃ̂ŁAstd::vector�ɂ��C���O�O�G
	m_abyData  = vSeri.m_abyData;
	m_bStoring = vSeri.m_bStoring;
	m_nDataPos = vSeri.m_nDataPos;
//	������R�s�[����ق��������悤�ȋC�����邪�A�A�A
//	m_bStoring = true;
//	m_nDataPos = 0;
//	�����Z�b�g���Ă����̂��D�D
	return *this;
}

LRESULT CSerialize::Save(string filename){
	//	�X�g���[���̃t�@�C���ւ̕ۑ�
	CFile file;
	return file.Save(filename,GetData());
	//	return CFile().Save(...)�̂悤��
	//	�e���|�����I�u�W�F�N�g�ŏ����Ă��ǂ����A
	//	VC++6.0���āAreturn�̂Ƃ���ł���������A
	//	�o�O���Ă��悤�ȋC������O�O�G > �T�[�r�X�p�b�N���ĂĂȂ���
}

LRESULT CSerialize::Load(string filename){	//	�X�g���[���̃t�@�C������̕���
	//	�X�g���[������̃t�@�C���̓ǂݏo��
	CFile file;
	return file.Load(filename,GetData());
}