#include "stdafx.h"
#include "yaneDir.h"

//////////////////////////////////////////////////////////////////////////////
//	<yanepack�p�\���̂ɂ���>
//		�t�@�C���w�b�_�[ "yanepack" 8�o�C�g + �i�[�t�@�C���� DWORD
//		���̌�A�ȉ���CFileInfo���i�[�t�@�C�����������āA���Ƃ̓f�[�^
class CFileInfo {
public:
	char	filename[32];
	DWORD	startpos;	//	seek pos
	size_t	filesize;	//	file size
};
struct CFileInfoEx {
	CHAR	filename[32];
	DWORD	startpos;	//	seek pos
	size_t	filesize;	//	file size(���k�T�C�Y)
	size_t	arcsize;	//	arc size (�W�J��̃t�@�C���T�C�Y)
};
struct CFileInfoDx {
	CHAR	filename[256];	//	�K�w���t�@�C���Ȃ̂ŁA�T�C�Y�ł����Ƃ�
	DWORD	startpos;	//	seek pos
	size_t	filesize;	//	file size(���k�T�C�Y)
	size_t	arcsize;	//	arc size (�W�J��̃t�@�C���T�C�Y)
};
//////////////////////////////////////////////////////////////////////////////

CDir::CDir() {
	m_handle	=	INVALID_HANDLE_VALUE;
	m_hPackFile =	INVALID_HANDLE_VALUE;
	m_lpDir		=	NULL;
	m_file		=	"*.*";

	m_bSubDir	= true;
	m_bPackFile	= true;
	m_bAllFile	= false;
}

CDir::~CDir()	{ CloseFindFile(); }

//////////////////////////////////////////////////////////////////////////////

void CDir::SetFindFile(const string& file){
	CloseFindFile();	//	�����������˂�
	m_file = file;
}

void CDir::SetPath(const string& file){
	m_path = file;
	if (m_path.empty()) return ;
	string::iterator p = m_path.end()-1;
	if (*p=='/' || *p=='\\') m_path.erase(p);
}

void CDir::CloseFindFile(void) {
	if(m_handle!=INVALID_HANDLE_VALUE) {
		::FindClose(m_handle);
		m_handle=INVALID_HANDLE_VALUE;
	}
	if(m_hPackFile!=INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hPackFile);
		m_hPackFile=INVALID_HANDLE_VALUE;
	}
	DELETE_SAFE(m_lpDir);
}

//////////////////////////////////////////////////////////////////////////////

bool	CDir::IsFileExist(const string& file){		//	���̃t�@�C���͑��݂���̂��H
	HANDLE h;
	h  = ::CreateFile(file.c_str(),
					GENERIC_READ,		// Read
					FILE_SHARE_READ,	// ReadOpen�Ȃ狤�L�������̂̓}�i�[ 
					NULL,				// security
					OPEN_EXISTING,		// ���݂��ĂȂ���΃G���[
					FILE_ATTRIBUTE_NORMAL,	//	�t�@�C������
					NULL				// �e���v���[�g�t�@�C��
					);
	if (h==INVALID_HANDLE_VALUE) return false;
	::CloseHandle(h);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
LRESULT CDir::FindFile(string &file){
	int result;
	WIN32_FIND_DATA FindFileData;
	DWORD NumberOfBytesRead;		// dummy

retry:;
	//	sub folder���L���Ȃ�΍ċA�I�Ɍ�������
	if (m_lpDir!=NULL) {
		if (m_lpDir->FindFile(file)==0) {
			return 0;
		}
		DELETE_SAFE(m_lpDir);
	}
	if (m_hPackFile!=INVALID_HANDLE_VALUE) {
		while (m_dwFileNum--!=0){
			switch(m_nPackFile) {
			case 0: {
				CFileInfo info;
				if (!::ReadFile(m_hPackFile,&info,sizeof(info),&NumberOfBytesRead,NULL)){
					::CloseHandle(m_hPackFile);
					m_hPackFile=INVALID_HANDLE_VALUE;
					goto retry;
				}
				file = info.filename;	//	�����������Ă���e�X�g����
					} break;
			case 1: {
				CFileInfoEx info;
				if (!::ReadFile(m_hPackFile,&info,sizeof(info),&NumberOfBytesRead,NULL)){
					::CloseHandle(m_hPackFile);
					m_hPackFile=INVALID_HANDLE_VALUE;
					goto retry;
				}
				file = info.filename;	//	�����������Ă���e�X�g����
					} break;
			case 2: {
				CFileInfoDx info;
				if (!::ReadFile(m_hPackFile,&info,sizeof(info),&NumberOfBytesRead,NULL)){
					::CloseHandle(m_hPackFile);
					m_hPackFile=INVALID_HANDLE_VALUE;
					goto retry;
				}
				file = info.filename;	//	�����������Ă���e�X�g����
					} break;
			}
			if (IsMatch(file.c_str(),m_file.c_str())) {
				//	��v�����I
				file = m_PackName + file;
				//	���k�t�@�C���ȊO�̂Ƃ���ɑ��݂���΁A����͋p��:p
				//	�i�d�����ă��X�g�A�b�v���Ă��܂����߁j
				if (!IsFileExist(file)) return 0;
			}
		}
		::CloseHandle(m_hPackFile);
		m_hPackFile=INVALID_HANDLE_VALUE;
	}

	//	�V�K�����Ȃ̂��H
	if (m_handle==INVALID_HANDLE_VALUE) {
		m_handle = ::FindFirstFile((m_path+"\\*.*").c_str(),&FindFileData);
		if (m_handle == INVALID_HANDLE_VALUE) result=1; else result=0;
	} else {
		if (::FindNextFile(m_handle,&FindFileData)!=0) result=0; else result=1;
	}

	if (result==0) {
		file = FindFileData.cFileName;
		if (!m_bAllFile && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if (file == "." || file == "..") goto retry; // ����Ȃ�E���Ȃ�...
			if (m_bSubDir) {	// sub�t�H���_�͌����Ώۂ��H
				m_lpDir = new CDir;
				m_lpDir->EnableSubdir(m_bSubDir);		// must be true
				m_lpDir->EnablePackFile(m_bPackFile);
				m_lpDir->SetFindFile(m_file);
				m_lpDir->SetPath(m_path+"\\"+file);
			}
			goto retry;
		} else {
			//	���k�t�@�C�������p�i�߂�ǂ������Ȃ��c�j
			if (m_nPackFile>=1 && m_bSubDir && IsMatch(file.c_str(),"*.dat")){
				//	���k�t�@�C���������ΏۂȂ񂩁H
				m_hPackFile = ::CreateFile((m_path+"\\"+file).c_str(),
				GENERIC_READ,		// Read
				FILE_SHARE_READ,	// ReadOpen�Ȃ狤�L�������̂̓}�i�[ 
				NULL,				// security
				OPEN_EXISTING,		// ���݂��ĂȂ���΃G���[
				FILE_ATTRIBUTE_NORMAL,	//	�t�@�C������
				NULL				// �e���v���[�g�t�@�C��
				);
			}
			if (m_hPackFile == INVALID_HANDLE_VALUE) goto check;
			char ident[9];
			ident[8] = '\0';
			if (!::ReadFile(m_hPackFile,ident,8,&NumberOfBytesRead,NULL)){
				::CloseHandle(m_hPackFile);
				m_hPackFile = INVALID_HANDLE_VALUE;
				goto check;
			}
			if (!strcmp(ident,"yanepack")){
				m_nPackFile = 0;
			} else if (!strcmp(ident,"yanepkEx")){
				m_nPackFile = 1;
			} else if (!strcmp(ident,"yanepkDx")){
				m_nPackFile = 2;
			} else {
				::CloseHandle(m_hPackFile);
				m_hPackFile = INVALID_HANDLE_VALUE;
				goto check;
			}
			if (!::ReadFile(m_hPackFile,&m_dwFileNum,sizeof(m_dwFileNum),&NumberOfBytesRead,NULL)){
				::CloseHandle(m_hPackFile);
				m_hPackFile = INVALID_HANDLE_VALUE;
				goto check;
			}
			//	.dat��؂藎�Ƃ��ăt�H���_���ɕϊ�
			string::size_type pos1;
			pos1 = file.rfind('.');
			file.erase(pos1);
			m_PackName = m_path + "\\" + file + "\\";
			goto retry;	//	���k�t�@�C������T��

check:;
			//	��v�����I�H
			if (IsMatch(file.c_str(),m_file.c_str())) {
				//	pack file�Ȃ�ΊO���K�v����...!?
				file = m_path + "\\" + file;	//	�t���p�X�ŕԂ�
				m_bDir = (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) !=0;
				result = 0;
			} else {
				goto retry;
			}

		}
	} else {
		CloseFindFile();
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////////

//	Check two strings match,using wildcard('*'or'?'). 
//	This algorithm is examine recursively.(C) yaneurao 1999
bool CDir::IsMatch(LPCSTR it1,LPCSTR it2){
//	it1��it2�ƃ}�b�`���邩
//	it1�ɂ́A'*'��'.'���g�p�ł���

	//	�����܂���͊��ق��Ă���
	while (true){
		if (*it1 == '*') {
			it1++;
			do {
				if (IsMatch(it1,it2)) return true;	//	���ł���v�����true
			} while (*(it2++)!='\0'); 
			return false;	//	���ׂĈ�v���Ȃ��Ȃ��false
		}
		if (*it2 == '*') {
			it2++;
			do {
				if (IsMatch(it1,it2)) return true;	//	���ł���v�����true
			} while (*(it1++)!='\0'); 
			return false;	//	���ׂĈ�v���Ȃ��Ȃ��false
		}
		if ((toupper(*it1) != toupper(*it2)) && (*it1 != '?') && (*it2 != '?')) return false;		// �_��
		if (*it1=='\0' && *it2=='\0') return true;	//	��v����
		if (*it1=='\0' || *it2=='\0') return false;
		it1 ++;
		it2 ++;
	}
}