#include "stdafx.h"
#include  <mbctype.h> // _ismbblead
#include "yaneFile.h"
#include "yaneLZSS.h"
/*	yaneSDK3rd����o����^^;
#include "../AppFrame/yaneAppInitializer.h"

//	CFile�̏������p�I�u�W�F�N�g
CFileInitializer::CFileInitializer(){
	smart_ptr<function_callback> p(
		function_callback_v::Create(CFile::SetCurrentDir));
	CAppInitializer::RegistInitCallback(p);
}
CFileInitializer CFileInitializer::m_dummy;
*/

//////////////////////////////////////////////////////////////////////////////

string	CFile::m_szCurrentDirectory;		// ���ݑΏۂƂ��Ă���f�B���N�g��
smart_ptr<vector<string> >	CFile::m_aszPathList;	// �p�X���X�g

//////////////////////////////////////////////////////////////////////////////
//	<yanepack�p�\���̂ɂ���>
//		�t�@�C���w�b�_�[ "yanepack" 8�o�C�g + �i�[�t�@�C���� DWORD
//		���̌�A�ȉ���CFileInfo���i�[�t�@�C�����������āA���Ƃ̓f�[�^
struct CFileInfo {
	CHAR	filename[32];
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

CFile::CFile() {
	m_lpFile = NULL;
	m_lpFileAdr		= NULL;
	m_dwFileSize	= 0;
	m_lpFileMemPos	= NULL;
	m_bEncode		= false;
	m_bTempFile		= false;
	m_bCompress		= false;
	m_bMemoryImage	= false;
}

CFile::~CFile() {
	Close();
}

LRESULT CFile::Close(){
	if (m_lpFile!=NULL) {
		LRESULT l;
		l = ::fclose(m_lpFile);
		m_lpFile = NULL;
		return l;
	}
	if (m_bMemoryImage) {
	//	��������̉��z�t�@�C��
		m_lpFileAdr = NULL;
		m_dwFileSize = 0;
		m_lpFileMemPos = NULL;
		m_bMemoryImage = false;
	}
	if (m_lpFileAdr!=NULL) {
		::GlobalFree(m_lpFileAdr);
//		delete [] m_lpFileAdr;
		m_lpFileAdr = NULL;
		m_dwFileSize = 0;
		m_lpFileMemPos = NULL;
	}
	if (m_bTempFile) {
		m_bTempFile = false;
		Delete(m_szFileName);	//	�t�@�C���̍폜
	}
	m_bCompress	= false;
	return 0;
}

//	�t�@�C���̍폜
LRESULT	CFile::Delete(const string& filename){
	//	����Ȃ�n���Ȃ�...
	if (filename.empty()) return -1;

	string fn(MakeFullName(filename));
	return !::DeleteFile(fn.c_str());
}

LRESULT	CFile::CreateTemporary(){
	if (m_lpFileAdr==NULL) return 1;
	if (m_bCompress || m_bMemoryImage) {
	//	���k�t�@�C�����A�������C���[�W�i�t�@�C�����̂������j
	//	�̂Ƃ��̂�duplicate����

		string szTmpFileName;
		LRESULT lr = GetTemporaryFileName(szTmpFileName);
		if (lr!=0) return 2;

		if (WriteBack(szTmpFileName) != 0) {
//			Err.Out("CFile::CreateTemporary::�e���|�����t�@�C���ɏ������߂Ȃ��B");
			return 2;
		}
		Close();
		//	write back�����ق��������̃K�����Ă���t�@�C�����Ƃ���
		m_bTempFile = true;
		m_szFileName = szTmpFileName;
	} else {
		//	��������AClose���Ă������ق������S�O�O�G
		string s;
		s = m_szFileName;
		Close();
		m_szFileName = s;
	}
	return 0;
}


LRESULT	CFile::GetTemporaryFileName(string& szFileName){
	//	�e���|�����t�@�C�����擾����

	CHAR	szTmpPath[MAX_PATH];
	CHAR	szTmpFileName[MAX_PATH];
	if (::GetTempPath(sizeof(szTmpPath), szTmpPath) == 0) {
//		Err.Out("CFile::CreateTemporary::�e���|�����p�X���擾�ł��Ȃ��B");
		return 2;
	}
	if (::GetTempFileName(szTmpPath, "YNE", 0, szTmpFileName) == 0) {
//		Err.Out("CFile::CreateTemporary::�e���|�����t�@�C�����쐬�ł��Ȃ��B");
		return 2;
	}
	szFileName = szTmpFileName;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
string CFile::GetCurrentDir() {
	if (m_szCurrentDirectory.empty()) {
		//	�����悤�Ȃ̂ŁA�ݒ肷��
		CHAR dir[_MAX_PATH];
		::GetCurrentDirectory(_MAX_PATH,dir);
		SetCurrentDir(dir);
	}
	return m_szCurrentDirectory;
}

void CFile::SetCurrentDir(){
	CHAR dir[_MAX_PATH];
	::GetCurrentDirectory(_MAX_PATH,dir);
	SetCurrentDir(dir);
}

void CFile::SetCurrentDir(const string& dir){
	//	����Ȃ�n���Ȃ�...
	if (dir.empty()) return ;

	bool bDir = false;
	m_szCurrentDirectory = dir;
	if (m_szCurrentDirectory[0]=='"') {
		bDir = true;
		m_szCurrentDirectory.erase(0,1);	// �R�}���h���C����n�����Ȃ�...
	}
	// c:\test\test.exe�@�̂悤�ɃR�}���h���C�������ăJ�����g�f�B���N�g����
	// �ݒ�ł���悤�ɍH�v�B(bDir==true�̂Ƃ�)
	// c:\test\�@�܂ŁB�Ō��\���܂߂ĕۑ��B

	string::size_type pos;
	if (bDir) {
		pos = m_szCurrentDirectory.find('.');
	} else {
		pos = string::npos;
	}
	string::iterator it;
	if (pos==string::npos) {		// . �������Ȃ��
		it = m_szCurrentDirectory.end() - 1;
		if (*it!='\\' && *it!='/') {	
			// \��/�ŏI����Ă��Ȃ��Ȃ��\��ǉ�
			m_szCurrentDirectory += '\\';
		}
	} else { // c:\test\test or c:\test\test.exe
		GetParentDir(m_szCurrentDirectory);
	}
}

LRESULT	CFile::GetParentDir(string& filename){						//	�e�t�H���_��Ԃ�
// e.g. "c:\test1\test2\test3.exe" -> "c:\test1\test2\" -> "c:\test1\" -> "c:\" -> "c:\"
	//	�����̂P�o�C�g�ڂ������K�v����...
	//
	//	1. �����A�l�b�g���[�N�h���C�u�ŗL��΁A�R���s���[�^�����X�L�b�v����B
	//	�@�@�������邱�ƂŁA���[�J���̃p�X�Ɠ��������ɂȂ�B
	//�@�@�@�i\\HOST\PATH �܂ł��A�h���C�u���^�[�ł���Ƃ��� �� �d�l
	//	2. \��/��T���B���̍ۂɍŌ�Ɍ��������ʒu�ƍŌォ��Q�ԖڂɌ��������ʒu���L�^���Ă���
	//	3. ������\�ł�/�ł��Ȃ��ꍇ�A���[�g�Ȃ�΁���t������B���[�g�łȂ���΁A�Ō�@�@�@�@�@�Ɍ��������Ƃ���܂ł�e�f�B���N�g���Ƃ��ĕԂ�
	//	4. ������\ �� /�ł���ꍇ�@�A���[�g�Ȃ�΂��̂܂ܕԂ��B���[�g�łȂ���΁A�Ōォ��Q�ԖڂɌ��������Ƃ���܂ł�e�f�B���N�g���Ƃ��ĕԂ�
	//
	//	2001/07/29 sohei
	//	  ���[�g�ɗL��t�@�C������n���ꂽ��\��/�͈�����Ȃ����A���[�g��Ԃ��Ȃ��Ƃ����Ȃ�
	//	2001/08/06 sohei
	//	  string �� \ ��t�����邱�ƂŊg�����ꂽ�ꍇ�A�o�O��̂��C��
	//	�@�˂�������ӂ��`�@����񍐗L���������܂��B

	if (filename.empty()) return 1;	//	����Ȃ�n����^^;

	string::iterator it = filename.begin();
	if (filename.substr(0,2)=="\\\\") {
		//	�l�b�g���[�N�p�X
		it += 2;	if (*it == '\0')	return 1;	//	\\ �ł���肩����^^;
		//	���̂Ƃ��́A�R���s���[�^�����X�L�b�v����
		bool bKanji=false;
		for (;true; it++) {
			if (*it == '\0') break;
			if (bKanji) { bKanji=false; continue; }
//			if (((BYTE)*it>=0x80 && (BYTE)*it<=0xa0) || ((BYTE)*it>=0xe0 && (BYTE)*it<=0xff)) {
			if (_ismbblead(*it)){
				bKanji = true;	//	�Q�o�C�g�R�[�h�̂P�o�C�g�ڂł�����
			} else {
				bKanji = false;	//	�Q�o�C�g�R�[�h�̂P�o�C�g�ڂł͂Ȃ�����
				if (*it == '\\' || *it=='/') {	//	��؂蕶��
					it++;	//	\\HOST\ �̎�����Ƃ���΃��[�J���Ɠ��������ɂȂ�
					break;
				}
			}
		}
	} else {
	}
	string::iterator pos1,pos2;	
	// pos1 : �Ō�Ɍ�������\��/�̈ʒu
	// pos2 : �Ōォ��Q�ԖڂɌ�������\��/�̈ʒu
	pos1 = pos2 = NULL;	// not found

	bool bKanji=false;
	
	while (true) {
		if (*it == '\0') break;
		if (bKanji) { bKanji=false; goto skip; }
//		if (((BYTE)*it>=0x80 && (BYTE)*it<=0xa0) || ((BYTE)*it>=0xe0 && (BYTE)*it<=0xff)) {
		if (_ismbblead(*it)){
			bKanji = true;	//	�Q�o�C�g�R�[�h�̂P�o�C�g�ڂł�����
		} else {
			bKanji = false;	//	�Q�o�C�g�R�[�h�̂P�o�C�g�ڂł͂Ȃ�����
			if (*it == '\\' || *it=='/') {	//	��؂蕶��
				pos2 = pos1;
				pos1 = it;
			}
		}
skip:;
		it++;
	}
	//	2001/08/06	sohei
	//		string ���L�������Ƃ��ɁA�C�e���[�^�������ɂȂ��āA
	//		�s���ȏ����������N�����Ă����̂��C���B
	//		�˂�������ӂ��`�@����L���������܂��B

	//	������\�ł�/�ł��Ȃ����
	if ( !bKanji && (*(it-1)!='\\' && *(it-1)!='/')) {
		//	���̏ꍇ�͍Ō�Ɍ����������܂ł��K�v�B
		if (pos1 == NULL) {	//	���\�������Ȃ烋�[�g�Ȃ̂�\��t������
			filename+='\\';
		} else {
			filename.erase(pos1+1,filename.end());	//	���[�g�ł͂Ȃ��̂ŁA�Ō�Ɍ��������Ƃ���܂ł�Ԃ�
		}
	} else {
		//	���̏ꍇ�͂Q�ԖڂɌ����������܂ł��K�v�B
		if (pos2 == NULL) return 1;	//	���[�g�Ȃ�Ŏ�菜���K�v�͂Ȃ�
		filename.erase(pos2+1,filename.end());		//	�Ōォ��Q�Ԗڂ܂ł�Ԃ��΂悢
	}
	return 0;
}

string	CFile::GetSuffix(){
	return GetSuffixOf(m_szFileName);
}

string	CFile::GetPureFileName(){
	return GetPureFileNameOf(m_szFileName);
}

// ------ static function ------
string	CFile::GetSuffixOf(const string& filename){
	string suffix;
	suffix = GetPureFileNameOf(filename);
	// �@�@�@�@���ԂɃf�B���N�g���L��������Ƃ܂����̂Ńp�X������
	string::size_type pos;
	pos = suffix.rfind('.');
	if (pos==string::npos) {
		suffix.erase();
	} else {
		suffix.erase(0,pos+1);
	}
	return suffix;
}	

string	CFile::GetPureFileNameOf(const string& filename){

	if (filename.empty()) return string("");

	string purefilename;
	string::iterator pos1;
	// pos1 : �Ō�Ɍ�������\��/�̈ʒu
	purefilename = filename;
	string::iterator it = purefilename.begin();
	pos1 = NULL;	// not found
	bool bKanji=false;
	while (true) {
		if (*it == '\0') break;
		if (bKanji) { bKanji=false; goto skip; }
//		if (((BYTE)*it>=0x80 && (BYTE)*it<=0xa0) || ((BYTE)*it>=0xe0 && (BYTE)*it<=0xff)) {
		if (_ismbblead(*it)){
			bKanji = true;	//	�Q�o�C�g�R�[�h�̂P�o�C�g�ڂł�����
		} else {
			bKanji = false;	//	�Q�o�C�g�R�[�h�̂P�o�C�g�ڂł͂Ȃ�����
			if (*it == '\\' || *it=='/') {
				pos1 = it;
			}
		}
skip:;
		it++;
	}
	if (pos1!=NULL){
		purefilename.erase(purefilename.begin(),pos1+1);
	}
	return purefilename;
}

//////////////////////////////////////////////////////////////////////////////
//	static function..
void  CFile::ToLower(string &str){
	//	������������(do_tolower)
//	LPSTR it = str.begin();
	string::iterator it = str.begin();
	bool bSkip = false;
	while (it!=str.end()){
		if (bSkip) {
			bSkip = false;
			goto Repeat;
		}

//		if (((BYTE)*it>=0x80 && (BYTE)*it<=0xa0) || ((BYTE)*it>=0xe0 && (BYTE)*it<=0xff)) {
		if (_ismbblead(*it)){
			//	�Q�o�C�g�R�[�h�̂P�o�C�g�ڂł�����
			bSkip = true;
			goto Repeat;
		} else {
			//	�Q�o�C�g�R�[�h�̂P�o�C�g�ڂł͂Ȃ�����
			bSkip = false;
		}
		*it = tolower(*it);
Repeat:;
		++it;
	}
}
//////////////////////////////////////////////////////////////////////////////

LRESULT CFile::Open(const string& filename,const string& access)
{
	LRESULT lr;
	//	�J�����g�t�H���_�̓ǂݍ���
	lr = InnerOpen(MakeFullName(filename), access);
	if (lr==0) return 0;

	if (GetPathList().isNull()) return lr;	//	�p�X�w�肳��ĂȂ������D�D�O�O�G

	//	�ݒ肳��Ă����p�X���������ǂ��Ē��ׂ�
	vector<string>::iterator it = GetPathList()->begin();
	while (it!=GetPathList()->end()){
		lr = InnerOpen(*it + '/' + filename, access);
		if (lr == 0) return 0; // ����������O�O�G
		it++;
	}
	return lr;	//	���߂���������A�A�i���Ԃ�j
}

LRESULT CFile::InnerOpen(const string& filename,const string& access)
{
	Close();
	m_lpFile = fopen(filename.c_str(), access.c_str());
	if (m_lpFile==NULL) return 1;

	// �t�@�C������ݒ�
	m_szFileName = filename;
	return 0;
}

LRESULT CFile::Write(const string& s) {
	if (m_lpFile==NULL) return -1;
	if (fprintf(m_lpFile, "%s", s.c_str())<0) return 1;
	return 0;
}

LRESULT	CFile::WriteBack(const string& filename){
	return Write(filename,GetMemory(),GetSize());
}

//////////////////////////////////////////////////////////////////////////////

DWORD	CFile::GetSize() const {
	if (m_bEncode) return m_dwFileSize - 4;	//	�S�o�C�g����
	return m_dwFileSize;
}

LPVOID	CFile::GetMemory() const {
	if (m_bEncode) return (BYTE*)m_lpFileAdr + 4;	//	���ʃw�b�_�̂S�o�C�g�����
	return m_lpFileAdr;
}

LRESULT	CFile::Encode(){
	if (*(DWORD*)m_lpFileMemPos == 0x314B5059) return 1;	// already encoding!!
	if (m_bEncode) return 1;	//	already encoding!!
	
	DWORD size = m_dwFileSize+4;
	BYTE* m_lpFileAdr2 // = new BYTE[size+1];
		= (BYTE*)GlobalAlloc(GMEM_FIXED | GMEM_NOCOMPACT,size+1);

	//	�P�o�C�g���߂Ɋm�ۂ��āA�Ō��\0��ۏ؂���B��������΁A
	//	�������r�������₷���Ȃ�B
	*((BYTE*)m_lpFileAdr2 + size) = 0;

	for (DWORD i=0;i<size-4;i++){
		//	�j�u���œ���ւ��邾��:p
		*(m_lpFileAdr2+i+4) =	(((*((BYTE*)m_lpFileAdr+i) & 0xf) << 4) +
								 ((*((BYTE*)m_lpFileAdr+i) &0xf0) >> 4)) ^ 0xcc;
	}
	*(DWORD*)m_lpFileAdr2 =	 0x314B5059;	// YPK1:�w�b�_�t�^

	Close();

	m_lpFileAdr		= (LPVOID)m_lpFileAdr2;
	m_dwFileSize	= size;
	m_lpFileMemPos	= (LPSTR)m_lpFileAdr; // for ReadLine
	m_bEncode		= false;	//	������false�ɂ��邱�Ƃɂ���ăw�b�_�[�t�^
	return 0;
}

string	CFile::GetWindowsDir(){
	//	Windows�f�B���N�g���̎擾�i�I�[��\\�j
	CHAR	buf[MAX_PATH];
	::GetWindowsDirectory(buf,MAX_PATH);
	if (buf[::lstrlen(buf)-1]!='\\'){	//	�I�[��'\'���H
		::lstrcat(buf,"\\");
	}
	return buf;	//	�R�s�[�R���X�g���N�^���N������̂ň��S
}

//////////////////////////////////////////////////////////////////////////////
string	CFile::MakeFullName(const string& filename){	// �N���f�B���N�g�������Ċ��S�p�X�����
	string file;
	if (filename.substr(0,2)=="\\\\") {
			//	�l�b�g���[�N�p�X�Ȃ̂ŉ������Ȃ�
			file = filename;
	} else {
		string::size_type pos = filename.find(':');
		if (pos==string::npos) {
			//	�t���p�X�łȂ���΃J�����g�f�B���N�g����₤

			// ..\\�ł̋삯�オ�葊�΃p�X���T�|�[�g
			string dir;
//			dir = m_szCurrentDirectory;
			dir = GetCurrentDir();
				//	���ݒ肳��Ă��Ȃ���΁A���̏u�Ԃɐݒ肷��
			file = filename;
			while (true) {
				if (file.substr(0,3)=="..\\" || file.substr(0,3)=="../"){
					GetParentDir(dir);
					file.erase(0,3);
				} else if ((file.substr(0,2)==".\\" || file.substr(0,2)=="./")) {
					//	./�ɂ��J�����g�t�H���_������
					file.erase(0,2);
				} else if (!_ismbblead(file.at(0))&&(file.substr(0,1)=="\\" || file.substr(0,1)=="/")){
					//	\�P�ƂȂ疳������  �}���`�o�C�g������1byte�ڂ��l������
					file.erase(0,1);
				} else {
					break;
				}
			}
			file = dir + file;
		} else {
			file = filename;
		}
	}
	/*
		'01/10/07	��˂��炨
			/	���Ƃ��܂��ǂݍ��߂Ȃ����Ƃ�����悤�Ȃ̂ŁA\�ɒu������O�O�G
		'02/02/11	����
			�}���`�o�C�g���l�����Ă��Ȃ������̂ŏC������
			�t�@�C���̑��݃`�F�b�N�����āA����������GetPath�ŒT���悤�ɂ���
	*/


//	�R�����g�A�E�g by ��˂��炨 '02/03/14 01:36
#if 0

	// GetPath()!=NULL���́A�w�肳�ꂽ�t�@�C�������݂��邩�`�F�b�N
	if(!GetPath().isNull()){
		bool bFound = false;
		if(!PathFileExists(file)){
			// ���k����Ă��邩��
			string fullname(file);
			string inner_filename(fullname);
			GetParentDir(fullname);
			inner_filename.erase(0, fullname.size());
			fullname[fullname.size() - 1] = '.';
			fullname += "dat";
			if(PathFileExists(fullname)) { bFound=true; }	// ���k�t�@�C������������
		}else{
			bFound=true;
		}

		// ������Ȃ����Ď��͑��̏ꏊ�ɂ���
		if(!bFound){
			const string file2 = file;
			vector<string>::iterator it = GetPath()->begin();
			while(it!=GetPath()->end()){
				file = *it + '/' + filename;
				if(!PathFileExists(file)){
					// ���k����Ă��邩��
					string fullname(file);
					string inner_filename(fullname);
					GetParentDir(fullname);
					inner_filename.erase(0, fullname.size());
					fullname[fullname.size() - 1] = '.';
					fullname += "dat";
					if(PathFileExists(fullname)) { bFound=true;	 break; }	// ���k�t�@�C������������
				}else{
					bFound=true;  break;
				}
				it++;
			}
			// ������Ȃ�������A�J�����g�̂ɖ߂��Ă�����
			if(!bFound) { file = file2; }
		}
	}
#endif

	{
		bool bKanji=false;
		string::iterator it = file.begin();
		while (it!=file.end()){
			if ( bKanji ) {
				bKanji=false;
				it++;
				continue ;
			}
			//	�Q�o�C�g�R�[�h�̂P�o�C�g�ڂł͂Ȃ�����
			if ( *it == '/' ) { *it='\\'; }
			if ( _ismbblead(*it) ) {
				//	�Q�o�C�g�R�[�h�̂P�o�C�g�ڂł�����
				bKanji=true;
			}
			it++;
		}
	}

	return file;
}

bool CFile::PathFileExists(const string& fullname)
{
	HANDLE hFile = ::CreateFile(fullname.c_str(),
		GENERIC_READ,		// Read
		FILE_SHARE_READ,	// ReadOpen�Ȃ狤�L�������̂̓}�i�[ 
		NULL,				// security
		OPEN_EXISTING,		// ���݂��ĂȂ���΃G���[
		FILE_ATTRIBUTE_NORMAL,	//	�t�@�C������
		NULL				// �e���v���[�g�t�@�C��
	);

	if (hFile == INVALID_HANDLE_VALUE) { // ��������I
		// close����K�v�͂Ȃ�(INVALID�n���h���Ȃ񂾂�)
		return false;
	}
	::CloseHandle(hFile);
	return true;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CFile::Read(const string& szFileName){
	LRESULT lr;
	//	�J�����g�t�H���_�̓ǂݍ���
	lr = InnerRead(szFileName);
	if (lr==0) return 0;

	if (m_aszPathList.isNull()) return lr;	//	�p�X�w�肳��ĂȂ������D�D�O�O�G

	//	�ݒ肳��Ă����p�X���������ǂ��Ē��ׂ�
	vector<string>::iterator it = m_aszPathList->begin();
	while (it!=m_aszPathList->end()){
		lr = InnerRead(*it + '/' + szFileName);
		if (lr == 0) return 0; // ����������O�O�G
		it++;
	}
	return lr;	//	���߂���������A�A�i���Ԃ�j
}

LRESULT CFile::InnerRead(const string& filename){

	if (filename.empty()) return -1; // ����H

	DWORD NumberOfBytesRead;
	int nFileType = 0;
	//	0:���ʂ̃t�@�C�� 1:yanePack
	//	2:yanePackEx 3:yanePackDx

	DWORD dwReadSize;	// �ǂݍ��ރT�C�Y

	m_szFileName = filename;
	Close(); // �O�ɓǂݍ���ł���A������������Ȃ�I

	if (filename[0] == '!') {
		//	�������C���[�W
		if(::sscanf(filename.c_str(),"!%x,%x",&m_lpFileAdr,&m_dwFileSize)!=2) {
			return 6; // ��������̃t�@�C���łȂ����H
		}
		m_lpFileMemPos	= (LPSTR)m_lpFileAdr;
		m_bMemoryImage	= true;
		return 0;
	}
	m_bMemoryImage = false;

	string fullname;
	fullname = MakeFullName(filename).c_str();
	HANDLE hFile = ::CreateFile(fullname.c_str(),
		GENERIC_READ,		// Read
		FILE_SHARE_READ,	// ReadOpen�Ȃ狤�L�������̂̓}�i�[ 
		NULL,				// security
		OPEN_EXISTING,		// ���݂��ĂȂ���΃G���[
		FILE_ATTRIBUTE_NORMAL,	//	�t�@�C������
		NULL				// �e���v���[�g�t�@�C��
	);

	if (hFile == INVALID_HANDLE_VALUE) { // ��������I
		// close����K�v�͂Ȃ�(INVALID�n���h���Ȃ񂾂�)

		//	�Ђ���Ƃ��Ĉ��k�t�@�C�����H
		//	test/script/game.c
		//	�� test/script.dat��game.c��T��
		//	�� test.dat��script/game.c��T��
		//	���̂悤�ɏ��Ԃɂ����������Ē��ׂĂ���

		/*
		string::size_type pos1,pos2;
		pos1 = fullname.rfind('/');
		pos2 = fullname.rfind('\\');
		//	�����̋L����������Ȃ��Ƃ������Ƃ͍l�����Ȃ��B
		//	��ɑ��݂���ق��ȍ~��؂藎�Ƃ�
		if (pos1 == string::npos) pos1=0;
		if (pos2 == string::npos) pos2=0;
		if (pos2 > pos1) pos1 = pos2;
		string inner_filename;
		inner_filename = fullname.substr(pos1+1,string::npos);
		fullname.erase(pos1);
		fullname += ".dat";
		*/

		//	fixed by TearDrop_Stone
		string inner_filename(fullname);
		string dirname(fullname);

	//	---- ���k�t�@�C���͋삯�オ���Ē��ׂ�̂ŁA���̃��g���C���[�v
CompressFileRetry:;

		GetParentDir(dirname);
		{
			string temp(dirname);
			GetParentDir(temp);
			if (temp==dirname){
				return 1;	//	����ȏ�A�삯�オ��܂�����
			}
		}

		inner_filename = fullname.substr(dirname.size()
			,fullname.length()-dirname.length());
		dirname[dirname.size() - 1] = '.';
		dirname += "dat";

		hFile = ::CreateFile(dirname.c_str(),
			GENERIC_READ,		// Read
			FILE_SHARE_READ,	// ReadOpen�Ȃ狤�L�������̂̓}�i�[ 
			NULL,				// security
			OPEN_EXISTING,		// ���݂��ĂȂ���΃G���[
			FILE_ATTRIBUTE_NORMAL,	//	�t�@�C������
			NULL				// �e���v���[�g�t�@�C��
		);

		if (hFile == INVALID_HANDLE_VALUE) {
			goto CompressFileRetry;
		}

		//	���k�t�@�C�������������I
		CHAR ident[9];
		ident[8] = '\0';
		if (!::ReadFile(hFile,ident,8,&NumberOfBytesRead,NULL)){
			::CloseHandle(hFile);
			goto CompressFileRetry;
		}
		if (!strcmp(ident,"yanepack")) nFileType = 1;
		ef (!strcmp(ident,"yanepkEx")) nFileType = 2;
		ef (!strcmp(ident,"yanepkDx")) nFileType = 3;
		if (nFileType==0) {
			::CloseHandle(hFile);
			goto CompressFileRetry;
		}

		DWORD filenum;
		if (!::ReadFile(hFile,&filenum,sizeof(filenum),&NumberOfBytesRead,NULL)){
			::CloseHandle(hFile);
			goto CompressFileRetry;
		}
		
		bool bFound = false;
		
		ToLower(inner_filename);
		CFileInfo info;
		CFileInfoEx infoEx;
		CFileInfoDx	infoDx;
		DWORD dwStartPos; // �t�@�C���J�n�I�t�Z�b�g
		for(int i=0;i<filenum;i++){
			string s;
			switch (nFileType){
			case 1: // yanePack
				if (!::ReadFile(hFile,&info,sizeof(info),&NumberOfBytesRead,NULL)){
					::CloseHandle(hFile);
					goto CompressFileRetry;
				}
				s = info.filename;	//	�����������Ă���e�X�g����
				break;
			case 2: // yanePackEx
				if (!::ReadFile(hFile,&infoEx,sizeof(infoEx),&NumberOfBytesRead,NULL)){
					::CloseHandle(hFile);
					goto CompressFileRetry;
				}
				s = infoEx.filename;	//	�����������Ă���e�X�g����
				break;
			case 3: // yanePackDx
				if (!::ReadFile(hFile,&infoDx,sizeof(infoDx),&NumberOfBytesRead,NULL)){
					::CloseHandle(hFile);
					goto CompressFileRetry;
				}
				s = infoDx.filename;	//	�����������Ă���e�X�g����
				break;
			}
			ToLower(s);
			if (s == inner_filename) {
				//	��v�����I
				// info.startpos��Seek
				switch (nFileType){
				case 1:	m_dwFileSize = info.filesize;
						dwReadSize	 = info.filesize;
						dwStartPos	 = info.startpos;
					break;
				case 2:	m_dwFileSize = infoEx.filesize;
						dwReadSize	 = infoEx.arcsize;
						dwStartPos	 = infoEx.startpos;
					break;
				case 3:	m_dwFileSize = infoDx.filesize;
						dwReadSize	 = infoDx.arcsize;
						dwStartPos	 = infoDx.startpos;
					break;
				}
				bFound = true;
				break;
			}
		}
		if (!bFound) {
			::CloseHandle(hFile);
			//	����ɋ삯�オ���Ē��ׂ�
			goto CompressFileRetry;
		}
		m_bCompress	= true;	//	�����Őݒ肵�Ƃ����O�O�G
		if (::SetFilePointer(hFile,dwStartPos,NULL,FILE_BEGIN)==0xFFFFFFFF) {
			::CloseHandle(hFile);
			goto CompressFileRetry;
		}
		goto StartOfRead;
	}
	
	// 4GB�ȏ�̃t�@�C�����������ɓǂݍ��ނ��ƂȂ�čl�����Ȃ�
	m_dwFileSize = ::GetFileSize(hFile,NULL);
	
	if (m_dwFileSize == 0xFFFFFFFF) {
		m_dwFileSize = 0;
		::CloseHandle(hFile);
		// Err.Out("CFile::ReadFile��FileSize�擾���s");
		return 2; // filesize�̎擾�Ɏ��s�B�i�ő��ɂȂ��Ǝv�����ǁj
	}

	//	�ǂݍ��݃T�C�Y
	dwReadSize = m_dwFileSize;

StartOfRead:;
	//	�܂��ǂݍ��ނ��߂̃������m�ہI
	m_lpFileAdr = (LPVOID)::GlobalAlloc(GMEM_FIXED | GMEM_NOCOMPACT,m_dwFileSize+1);

	// m_lpFileAdr = new BYTE[m_dwFileSize+1];

	// ���܂��炷���g���̂ɁA���̃G���A���k�������[��B
	if (m_lpFileAdr==NULL) {
		::CloseHandle(hFile);
		// Err.Out("CFile::ReadFile�Ń������m�ێ��s");
		return 3; // �������m�ۂł��񂩂�����B
				  // ����σt�@�C�����ł����Ƃ�����ȁ[�i�΁j
	}
	//	�P�o�C�g���߂Ɋm�ۂ��āA�Ō��\0��ۏ؂���B��������΁A
	//	�������r�������₷���Ȃ�B
	*((BYTE*)m_lpFileAdr + m_dwFileSize) = 0;

	if ((nFileType==2||nFileType==3) && (m_dwFileSize!=dwReadSize)) {
		//	���k�t�@�C���Ȃ̂Ŏ��O�œǂݍ���
		//smart_ptr<BYTE> lpBuf;
		//lpBuf.AddArray(dwReadSize);
		smart_array<BYTE> lpBuf;
		lpBuf.Add(dwReadSize);
/*		if (lpBuf.isNull()) {
		//	�ł������ă����������:p
			Close();
			::CloseHandle(hFile); // �����...
			return 4;
		}*/
		if (!::ReadFile(hFile,lpBuf.get(),dwReadSize,&NumberOfBytesRead,NULL)){
		//	�Ȃ����ǂݍ��ݎ��s..
			Close();
			::CloseHandle(hFile); // �����...
			return 4;
		}
		//	���k�t�@�C��
		CLZSS lzss;
		lzss.Decode(lpBuf.get(),*(BYTE**)&m_lpFileAdr,m_dwFileSize,false);	//	�G���[�����͂�����ˁH�H
	} else if (!::ReadFile(hFile,m_lpFileAdr,dwReadSize,&NumberOfBytesRead,NULL)){
		Close();
		::CloseHandle(hFile); // �����...
		// Err.Out("CFile�Ńt�@�C���ǂݍ��݂Ɏ��s");
		return 4; // �t�@�C���̓ǂݍ��݂Ɏ��s�B
	}

	if (::CloseHandle(hFile)==false) {
		Close();
		// Err.Out("CFile�Ńt�@�C����close�Ɏ��s");
		return 5; // Close�̎��s�B����Ȃ񂠂�񂩂���...
	}

	if (*(DWORD*)m_lpFileAdr == 0x314B5059) {	//	HEADER:YPK1(yanepack1) == Data is Encoding...
		m_bEncode = true;
		for (DWORD i=4;i<m_dwFileSize;i++){
			//	�j�u���œ���ւ��邾��:p
			*((BYTE*)m_lpFileAdr+i) =	(((*((BYTE*)m_lpFileAdr+i) & 0xf) << 4) +
										 ((*((BYTE*)m_lpFileAdr+i) &0xf0) >> 4)) ^ 0xcc;
		}
		m_lpFileMemPos = (LPSTR)m_lpFileAdr + 4; // for ReadLine
	} else {
		m_bEncode = false;
		m_lpFileMemPos = (LPSTR)m_lpFileAdr; // for ReadLine
	}

	return 0; // ����Ƃ�����I��:)
}

LRESULT CFile::Write(const string& fileName,LPVOID lpMem,DWORD dwSize){

	HANDLE hFile = ::CreateFile(MakeFullName(fileName).c_str(),
		GENERIC_WRITE,		// Write
		0,					// 
		NULL,				// security
		TRUNCATE_EXISTING,
		FILE_ATTRIBUTE_NORMAL,	//	�t�@�C������
		NULL				// �e���v���[�g�t�@�C��
	);
	//	OPEN_ALWAYS�ł́A�O�̃t�@�C���T�C�Y���c���Ă��܂�...

	if (hFile == INVALID_HANDLE_VALUE) { // ��������I
		hFile = ::CreateFile(MakeFullName(fileName).c_str(),
			GENERIC_WRITE,		// Write
			0,					// 
			NULL,				// security
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,	//	�t�@�C������
			NULL				// �e���v���[�g�t�@�C��
		);
		if (hFile == INVALID_HANDLE_VALUE) return 1; // Open���s		
	}


	DWORD NumberOfBytesRead = 0;
	if (!::WriteFile(hFile,lpMem,dwSize,&NumberOfBytesRead,NULL)){
		::CloseHandle(hFile); // �����...
		// Err.Out("CFile::Write�Ńt�@�C���������݂Ɏ��s");
		return 2;
	}

	if (!::CloseHandle(hFile)) {
		return 3; // Close�̎��s�B����Ȃ񂠂�񂩂���...
	}

	return 0; // ����Ƃ�����I��:)
}

string	CFile::GetName() const {
	// �ǂݍ���ł���t�@�C������Ԃ�
	return m_szFileName;
}

void CFile::Reset(){
	m_lpFileMemPos = (LPSTR)GetMemory();
}

//	UNICODE��Ή��B������Windows�̃e�L�X�g�t�@�C����p:p
LRESULT		CFile::ReadLine(LPSTR buf,DWORD dwSize){	 // �o�b�t�@��dwSize�o�C�g�p�ӂ��Ƃ��Ă˂�
	if (m_lpFileMemPos == NULL) return 3; // �ǂ��Ȃ��Ƃ�́[�I�I
	if (m_lpFileMemPos >= (LPSTR)m_lpFileAdr + m_dwFileSize) return 1;

	LPSTR lp = m_lpFileMemPos;
	for(int i=0;i<dwSize-1;i++){
		if (*m_lpFileMemPos == 0x0D && *(m_lpFileMemPos+1) == 0x0A ||
			(m_lpFileMemPos >= (LPSTR)m_lpFileAdr + m_dwFileSize)) { // CR+LF
			*buf = '\0'; // ��s�I���I
			m_lpFileMemPos+=2;
			return 0;
		}
		*(buf++) = *(m_lpFileMemPos++);
	}
	*buf = '\0';
	return 2;	// buffer over
}

LRESULT		CFile::ReadLine(string&s){
	// ������́A�o�b�t�@�͎��O�ŗp�ӂ��Ȃ��ėǂ��B
	//	�Ԃ��l 0: ����I�� 1:EOF   3:�t�@�C���ǂݍ���łȂ��O�O�G
	if (m_lpFileMemPos == NULL) return 3; // �ǂ��Ȃ��Ƃ�́[�I�I
	if (m_lpFileMemPos >= (LPSTR)m_lpFileAdr + m_dwFileSize) return 1;
//	if (m_lpFileMemPos+1 >= (LPSTR)m_lpFileAdr + m_dwFileSize) return 1;
				//	���P�c��'\0'��ǉ����Ă���̂ŁA+1���K�v?

	LPSTR lp = m_lpFileMemPos;
	DWORD dwSize = 0;
	while (true) {
		if (*m_lpFileMemPos == 0x0D && *(m_lpFileMemPos+1) == 0x0A ||
			(m_lpFileMemPos >= (LPSTR)m_lpFileAdr + m_dwFileSize)) { // CR+LF
//			*buf = '\0'; // ��s�I���I
			LPSTR psz = new CHAR[dwSize+1];
			::CopyMemory(psz,m_lpFileMemPos-dwSize,dwSize);
			*(psz+dwSize) = '\0';
			s = psz;
			delete [] psz;
			m_lpFileMemPos+=2;
			return 0;
		}
//		*(buf++) = *(m_lpFileMemPos++);
		m_lpFileMemPos++;
		dwSize++;
	}
	//	�����ɂ͗��Ȃ�..
	return 0;
}

LRESULT		CFile::ReadLine2(LPSTR Senario,DWORD dwSize){
	//smart_ptr<CHAR> lpBuf;
	//lpBuf.AddArray(dwSize);	//	���T�C�Y�m��
	smart_array<CHAR> lpBuf;
	lpBuf.Add(dwSize);

	::lstrcpy(Senario,"");	//	�ꉞ�A�I�������Ƃ��ɃS�~���o���Ȃ��悤��...
	if (ReadLine(lpBuf.get())!=0) return 1;
	if (!::lstrcmp(lpBuf.get(),"\"END\"")) return 1; // �I����
	::lstrcpy(Senario,lpBuf.get()+1);	//	�擪��"���폜

	while (true) {
		if (Senario[strlen(Senario)-1] == '\"') break;	//	�f���~�^:="
		if (ReadLine(lpBuf.get())!=0) return 1;
		::lstrcat(Senario,"\n");
		::lstrcat(Senario,lpBuf.get());
	}
	Senario[::lstrlen(Senario)-1] = '\0';
	return 0;
}

LRESULT	CFile::ReadData(BYTE*p,DWORD size){
	if (m_lpFileMemPos == NULL) return 2; // �ǂ��Ȃ��Ƃ�́[�I�I
	if (m_lpFileMemPos >= (LPSTR)m_lpFileAdr + m_dwFileSize) return 1;

	::CopyMemory(p,m_lpFileMemPos,size);
	m_lpFileMemPos+=size;
	return 0;
}

//	�f�[�^�̕ۑ��^�ǂݍ���(CSerialize�ƕ��p����Ɨǂ�)
LRESULT		CFile::Load(const string& filename,vector<BYTE>*vData){
	LRESULT lr = Read(filename);
	if (lr!=0) return lr; // �ǂݍ��ݎ��s���Ƃ���D�D�D
	//	�ǂݍ��񂾃t�@�C���̒����𒲂ׂāA����S��vData�ɃR�s������I
	int nSize = GetSize();
	vData->resize(nSize);
	::CopyMemory((LPVOID)&(*vData)[0],GetMemory(),nSize);
	return 0;
}

LRESULT		CFile::Save(const string& filename,vector<BYTE>*vData){
	return Write(filename,(LPVOID)&(*vData)[0],vData->size());
}

LRESULT		CFile::Save(const string& filename,vector<string>*vData){
	//	����Afopen���ď������񂾂ق����A�Ă��Ƃ葁���񂩂ȁH
	int nSize = vData->size();
	FILE *fp = fopen(filename.c_str(),"w");
	if (fp==NULL) return 1;

	for(int i=0;i<nSize;i++){
		if (fprintf(fp,(*vData)[i].c_str())<0) return 2;
		//	�������݂��Ă�Œ���HDD��ꂽ�񂿂Ⴄ�񂯁H
	}
	fclose(fp);
	return 0;
}