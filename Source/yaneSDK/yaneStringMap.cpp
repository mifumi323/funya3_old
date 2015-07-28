#include "stdafx.h"
#include "yaneStringMap.h"
#include "yaneStringScanner.h"
#include "yaneFile.h"

void	CStringMap::Serialize(CSerialize&s){
	//	m_vStringMap���܂邲��serialize
	int n;
	if (s.IsStoring()){
		n = GetMap()->size();
		s << n; // �T�C�Y��˂����ނȂ��[�O�O

		map<string,string>::iterator it;
		for(it = GetMap()->begin();it!=GetMap()->end();it++){
			s << static_cast<string>(it->first);
			s << static_cast<string>(it->second);
		}

	} else {
		s << n;		//	�����𕜌�����Ȃ��
		GetMap()->clear();	//	��������N���A����Ȃ��

		for(int i = 0; i<n ; i++){
			string s1,s2;
			s << s1;
			s << s2;
			GetMap()->insert(pair<string,string>(s1,s2));
		}
	}
}


//	�L�[�ɑ΂���f�[�^�̏�������
void	CStringMap::Write(string szKey,string szData){
	map<string,string>::iterator it;
	it = GetMap()->find(szKey);
	if (it==GetMap()->end()){
		//	����key�͑��݂��Ȃ�
		//	�قȁA�}���������[
		GetMap()->insert(pair<string,string>(szKey,szData));
	} else {
		//	�㏑���㏑���㏑���`��(���ށ�)o
		it->second = szData;
	}
}

void	CStringMap::Write(string szKey,LONGLONG nData){
	string s;
	CStringScanner::NumToString(nData,s);
	Write(szKey,s);
}

//	�L�[�ɑ΂���f�[�^�̒ǉ���������
void	CStringMap::Append(string szKey,string szData){
	map<string,string>::iterator it;
	it = GetMap()->find(szKey);
	if (it==GetMap()->end()){
		//	����key�͑��݂��Ȃ�
		//	�قȁA�}���������[
		GetMap()->insert(pair<string,string>(szKey,szData));
	} else {
		//	�ǉ��ǉ��ǉ��`��(���ށ�)o�@�Ă̓X�C�J�`��(���ށ�)o
		it->second += szData;
		//	string�̒ǉ��Ȃ�ŁA����ł�����ł���H
	}
}

void	CStringMap::Append(string szKey,LONGLONG nData){
	string s;
	CStringScanner::NumToString(nData,s);
	Write(szKey,s);
}

//	�L�[�ɑ΂���f�[�^�̓ǂݏo��
LRESULT	CStringMap::Read(string szKey,string& szData){
	map<string,string>::iterator it;
	it = GetMap()->find(szKey);
	if (it==GetMap()->end()){
		//	����key�͑��݂��Ȃ�
		szData.erase();
		return -1;
	}
	szData = it->second;
	return 0;
}

LRESULT	CStringMap::Read(string szKey,LONGLONG& nData){
	string s;
	LRESULT lr = Read(szKey,s);
	if (lr!=0) {
		nData = 0; // fail safe�΍�
		return lr;
	}
	return CStringScanner::StringToNum(s,nData);
}

//	�L�[�ɑ΂���f�[�^�̒��ڎ擾
	//	(���̃L�[�����݂��Ȃ��Ƃ��́AszDefault or lDefault���Ԃ�)
string	CStringMap::GetStr(string szKey,string szDefault){
	string s;
	Read(szKey,s);
	if (s.empty()) {
		return szDefault; // �G���[����szDefault��Ԃ�
	}
	return s;
}

int CStringMap::GetNum(string szKey,int nDefault){
	LONGLONG l;
	if (Read(szKey,l)!=0) return nDefault; // error����nDefault��Ԃ�
	return l;
}

LONGLONG	CStringMap::GetLongNum(string szKey,LONGLONG nDefault){
	LONGLONG l;
	if (Read(szKey,l)!=0) return nDefault; // error����nDefault��Ԃ�
	return l;
}

//	�L�[�ɑ΂���f�[�^�̍폜
bool	CStringMap::Delete(string szKey){
	//	�폜�����Ƃ��ɂ�true
	return GetMap()->erase(szKey)!=0;
}

//	�܂邲�ƍ폜
void	CStringMap::Clear(){
	//	clear all data..
	GetMap()->clear();
}


//	��`�t�@�C������f�[�^��ǂݏo���āA�i�[����@�\
//	(�V���A���C�Y�Ƃ͈Ⴂ�A����ȃt�H�[�}�b�g�̃t�@�C������ǂݏo��)
LRESULT	CStringMap::ReadFromConfigFile(string szFileName){
	CFile file;
	LRESULT lr = file.Read(szFileName);
	if (lr!=0) return lr;	//	�ǂݍ��߂Ė��������..

	Clear();	//	�f�[�^�̃N���A

	string buf;
	string szKey,szData;
	while (true){
		LRESULT lr = file.ReadLine(buf);
		if (lr==1) break;
		if (buf.empty()) continue;
		if (buf.size()>=2 && buf[0] == '/' && buf[1] =='/') continue; // �R�����g�s���..
		if (buf.size()>=1 && buf[0]=='#') {	//	�ݒ�s���..
			LPSTR psz = (LPSTR)(buf.c_str()+1);
			if (CStringScanner::SkipSpace(psz)!=0) continue;
			szKey = CStringScanner::GetStr(psz);
			if (CStringScanner::SkipSpace(psz)!=0) continue;
			szData = CStringScanner::GetStr(psz);
			if (szData.empty()) continue ; // �f�[�^����ւ�ŁH
			Write(szKey,szData);
		}
	}
	return 0;
}

//	����������̘A�z�L���Œu������
void	CStringMap::Replace(string& s,bool bCaseSensitive){
	//	�u�����Ȃ���̕\��
	map<string,string>::iterator it;
	for(it = GetMap()->begin();it!=GetMap()->end();it++){
		CStringScanner::Replace(s,
			static_cast<string>(it->first).c_str(),
			static_cast<string>(it->second).c_str(),
			bCaseSensitive);
	}
}