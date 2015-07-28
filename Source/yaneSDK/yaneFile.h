// yaneFile.h
//
//	standard file stream wrapper
//		programmed by yaneurao '00/02/25
//		modified for yanePackDx '02/03/13

#ifndef __yaneFile_h__
#define __yaneFile_h__

class IFile {
public:
	virtual LRESULT		Open(const string& filename,const string& access)=0;
	virtual LRESULT		Write(const string&)=0;
	virtual LRESULT		Close()=0;
	virtual FILE*		GetFilePtr()const=0;
	virtual LRESULT		Read(const string& filename)=0;
	virtual string		GetName()const=0;
	virtual string		GetPureFileName()=0;
	virtual string		GetSuffix()=0;
	virtual LPVOID		GetMemory()const=0;
	virtual DWORD		GetSize()const=0;
	virtual LRESULT		ReadLine(LPSTR buf,DWORD dwSize=256)=0;
	virtual LRESULT		ReadLine2(LPSTR buf,DWORD dwSize=512)=0;
	virtual LRESULT		ReadLine(string&s)=0;
	virtual void		Reset()=0;
	virtual LRESULT		ReadData(BYTE*p,DWORD size)=0;
	virtual LRESULT		Write(const string& filename,LPVOID mem,DWORD size)=0;
	virtual LRESULT		WriteBack(const string& filename)=0;
	virtual LRESULT		Encode()=0;
	virtual LRESULT		Delete(const string& filename)=0;
	virtual LRESULT		CreateTemporary()=0;
	virtual LRESULT		Load(const string& filename,vector<BYTE>*vData)=0;
	virtual LRESULT		Save(const string& filename,vector<BYTE>*vData)=0;
	virtual LRESULT		Save(const string& filename,vector<string>*vData)=0;
	virtual ~IFile(){}
};

class CFile : public IFile {
/**
�t�@�C���̓��o�͗p�̃N���X�ł��B

�ǂݍ��݂́AyanePack�`��(�t�@�C�������ɂ�鍇��)�AyanePackEx�`��
(LZSS�@�ɂ�鈳�k),yanePackDx�`��(�T�u�t�H���_���k�Ή���yanePackEx)
�̃t�@�C�����Ή����Ă��܂��B

�������݂́A�c�O�Ȃ���AyanePack/yanePackEx�`���̃t�@�C���ɂ͏������߂܂���B

yanePack�`���^yanePackEx�`���^yanePackDx�`���̎d�l�ɂ��Ă͌�q���܂��B
yanePack�`���^yanePackEx�`���^yanePackDx�̃t�@�C������邽�߂ɂ́A�ʓr�A
yanePack�^yanePackEx�^yanePackDx�����p�ӂ��������B


yanePack�̎d�l(yaneuraoGameSDK1.00�ŃT�|�[�g���Ă����`���^�݊��̂��ߓǂݍ��߂܂�)

���@���e

yanePack�́A�����t�@�C������ɂ܂Ƃ߂邽�߂̂��̂ł��B�i���k�͂��܂���j

CFile�̈ꊇ�ǂݍ��݌n�̊֐��Ȃ�΁AyanePack�`���ł��邱�Ƃ��ӎ������ɓǂݏo�����\�ł��B

 <yanepack�p�\���̂ɂ���>
// �t�@�C���w�b�_�[ "yanepack" 8�o�C�g + �i�[�t�@�C������DWORD�ł��āA
// ���̌�A�ȉ���CFileInfo���i�[�t�@�C�����������āA���Ƃ̓f�[�^�ł��B
struct CFileInfo {
char filename[32];
DWORD startpos; // seek pos(�t�@�C���擪����̃I�t�Z�b�g)
size_t filesize; �@�@// file size�i���̃t�@�C���̃T�C�Y�j
};

�t�@�C���̊g���q��dat�ɂ��Ă��������B
--------------------------------------------------------------------------------yanePack�d���̎d�l(yaneuraoGameSDK2nd�ŃT�|�[�g����`���j
--------------------------------------------------------------------------------���@���e
yanePackEx�́A�����t�@�C������ɂ܂Ƃ߁A�����k���邽�߂̂��̂ł��B���k�ɂ�LZSS�@��p���܂��iCLZSS�N���X�����̂܂ܗ��p���܂��j
 <yanepackEx�p�\���̂ɂ���>
// �t�@�C���w�b�_�[ "yanepkEx" 8�o�C�g + �i�[�t�@�C������DWORD�ł��āA
// ���̌�A�ȉ���CFileInfo���i�[�t�@�C�����������āA���Ƃ̓f�[�^�ł��B
struct CFileInfo {
char filename[32];
DWORD startpos; // seek pos(�t�@�C���擪����̃I�t�Z�b�g)
size_t filesize; �@�@// file size�i���̃t�@�C���̈��k�T�C�Y�j
size_t packsize;�@ // packsize�i���̃t�@�C���̓W�J��̃T�C�Y)
// �񈳏k�̃t�@�C���́Afilesize==packsize�ƂȂ��Ă��܂�
};
�t�@�C���̊g���q��dat�ɂ��Ă��������B
--------------------------------------------------------------------------------yanePack�c���̎d�l(yaneuraoGameSDK3rd�ŃT�|�[�g����`���j
--------------------------------------------------------------------------------���@���e
yanePackDx�́AyanePackEx�̃t�H���_�Ή��łł��B
 <yanepackDx�p�\���̂ɂ���>
// �t�@�C���w�b�_�[ "yanepkDx" 8�o�C�g + �i�[�t�@�C������DWORD�ł��āA
// ���̌�A�ȉ���CFileInfo���i�[�t�@�C�����������āA���Ƃ̓f�[�^�ł��B
struct CFileInfo {
char filename[256];
DWORD startpos; // seek pos(�t�@�C���擪����̃I�t�Z�b�g)
size_t filesize; �@�@// file size�i���̃t�@�C���̈��k�T�C�Y�j
size_t packsize;�@ // packsize�i���̃t�@�C���̓W�J��̃T�C�Y)
// �񈳏k�̃t�@�C���́Afilesize==packsize�ƂȂ��Ă��܂�
};
�t�@�C���̊g���q��dat�ɂ��Ă��������B
�t�@�C�����́A
	"test\subfolder\sample.txt"
�̂悤�ɁA�t�H���_�t���Ŋi�[����Ă��邱�Ƃ�����܂��B
����āA
	CFile::Read�́A
		"test\subfolder\sample.txt"
	�Ƃ����t�@�C����������΁A����
		"test\subfolder.dat" �̂Ȃ���"sample.txt"
	���ꂪ�������
		"test.dat"�̂Ȃ���"subfolder\sample.txt"
	�Ƃ����悤�ɁA���[�g�f�B���N�g���܂ők�y���Č������Ă����܂��B

*/
public:
	///	�X�g���[���n�t�@�C������
	virtual LRESULT		Open(const string& filename,const string& access);	//	Stream�nOpen
	virtual LRESULT		Write(const string&);								//	Stream�nWrite
	virtual LRESULT		Close();											//	Close
	virtual FILE*		GetFilePtr()const { return m_lpFile; }
	/**
	Open�Ńt�@�C�������w�肵�A�I�[�v�����܂��B
	access�̓A�N�Z�X�w��q�ŁAfopen�Ŏw�肷����̂Ɠ����ł��B
	Write�́A��������o�͂��܂��i���ꂵ���L��܂���j
	Close�́A�t�@�C������܂��B
	�����̂Ƃ���GetFilePtr�ŃI�[�v�����Ă���t�@�C����FILE*��������̂ŁA
	������Ԑڂŏ�������ōs�����Ƃ͏o���܂��B
	�t�Ƀ��[�U�[����Open����FILE*��SetFilePtr�Őݒ肵�A
	CFile�̃����o�֐�(�ƌ����Ă�Write��Close���炢��������܂���)���g�����Ƃ��o���܂��B
	*/

	///	�ꊇ�ǂݍ��݌n
	///	Read�œǂݍ��񂾂��̂��������ɂ́AClose()���Ăяo������
	/**
	�t�@�C������"c:\"�Ƃ��̂悤��:���܂܂�Ă��Ȃ���΁A�N���f�B���N�g��
	(SetCurrentDir�Őݒ肳��Ă���f�B���N�g��)���΃p�X�ł��B

	���@�������C���[�W��CFile�ɓǂݍ��܂���
	�@���Ƃ��΁A�t�@�C���������炢�A�����CFile��Read�œǂݍ��ނ悤�ȃN���X�����Ƃ��A
	�@��������ɔz�u���ꂽ�C���[�W���w�肵�����ꍇ�Ȃǂ́A����p�̊֐����p�ӂ��Ȃ��Ă͂Ȃ�܂���B
	�@���̂悤�ȕ�����CFile�������ł��z�����Ă����Ηǂ��Ǝv�����̂ŁA
	�@��������ɔz�u�����f�[�^���A�ȉ��̂悤�ɂ��āu�I�v�Ŏn�܂�A
	�@�P�U�i���̕�����Ń������̐擪�A�h���X�ƁA���̃u���b�N�̃T�C�Y���t�@�C�����Ƃ��Ďw�肵�Ă���
	�@CFile�́A������t�@�C���ƍ��o���A���������t�@�C���ł��邩�̂悤�Ɉ������Ƃ��o���܂��B
	�@[��]
	�@�@�@LPCSTR lpsz = "����\���ł��邩�ȁH<HR>���܂������Ηǂ��̂�����"
	�@�@�@�@�@�@�@�@�@�@"�����ȒP�ł��Ȃ����H<HR>�����Ă邢���Ă�D�D<HR>";
	�@�@�@::wsprintf(buf,"!%x,%x",lpsz,strlen(lpsz)+1);
	*/
	virtual LRESULT		Read(const string& filename);

	virtual string		GetName()const;
	///	�ǂݍ���ł���t�@�C������Ԃ�
	virtual string		GetPureFileName();
	///		�p�X�����t�@�C�������擾
	virtual string		GetSuffix();
	///		�ǂݍ���ł���t�@�C���̊g���q��Ԃ�
	virtual LPVOID		GetMemory()const;
	///		�ǂݍ���ł��郁������Ԃ�
	virtual DWORD		GetSize()const;
	///		�ǂݍ���ł���t�@�C���T�C�Y��Ԃ�

	/**
	Read�ŃI�[�v�����܂��B�I�[�v�������u�ԂɁA�������Ɉꊇ�œǂݍ��܂�܂��BRead�œǂݍ��񂾂��̂��������ɂ́AClose()���Ăяo���܂��BClose()���Ăяo���܂ŁAGetMemory�Ŏ擾�����t�@�C���̓��e���i�[����Ă��郁�����͗L���ł��B
	*/

	///		�������ɓǂݍ��񂾃t�@�C������s���ǂݏo�����[�`��
	virtual LRESULT		ReadLine(LPSTR buf,DWORD dwSize=256);
	/// �o�b�t�@��dwSize�o�C�g�p�ӂ��Ƃ��Ă˂�
	///		�Ԃ��l 0: ����I��	1: EOF	2:�o�b�t�@���ӂ�
	///		3:�t�@�C���ǂݍ���ł��Ȃ�

	virtual LRESULT		ReadLine2(LPSTR buf,DWORD dwSize=512);
	///		�o�b�t�@��dwSize�o�C�g�p�ӂ��Ƃ��Ă˂�
	///		����́A�V�i���I�t�@�C��("..."��1���b�Z�[�W,�I������"END"�ł���t�@�C��)�̓ǂݍ��ݗp

	virtual LRESULT		ReadLine(string&s);
	/// ������́A�o�b�t�@�͎��O�ŗp�ӂ��Ȃ��ėǂ��B
	///	�Ԃ��l 0: ����I�� 1:EOF   3:�t�@�C���ǂݍ���łȂ��O�O�G

	virtual void		Reset();
	///	���ReadLine�ł̓ǂݍ��݃|�C���^��擪�|�C���^�ɖ߂�

	virtual LRESULT		ReadData(BYTE*p,DWORD size);
	//	�o�C�g�P�ʂ̓ǂݏo��

	///		�ꊇ�������݌n
	///	��C�Ƀt�@�C���ɏ������݂܂��B���k�t�@�C���ɂ͏����o���܂���B
	virtual LRESULT		Write(const string& filename,LPVOID mem,DWORD size);

	///	ReadOpen�������̂������߂��܂��B���k�t�@�C���ɂ͏����߂��܂���B
	virtual LRESULT		WriteBack(const string& filename);

	///	�Ƃ����ɈÍ����i�P�Ȃ�j�u������ւ��j
	virtual LRESULT		Encode();	///	�����o�b�t�@���G���R�[�h����

	///	�t�@�C���̍폜
	virtual LRESULT		Delete(const string& filename);

	virtual LRESULT		CreateTemporary();
	/**
	�K�v�Ȃ�΃e���|�����t�@�C�����쐬�������o���B�i�ꊇ�ǂݍ��݌n�̃I�[�v�����������Ă��邱�Ƃ��O��j
	���̊֐����I����́ACFile�̓t�@�C���̒��g�͕ۗL���Ă��Ȃ��B
	�t�@�C���������͎����Ă���̂ŁAGetName�Ńt�@�C�����̎擾�͉\�B
	�e���|�����t�@�C�����쐬��������́A�ǂݍ���ł����t�@�C�������k�t�@�C���ł��邱�ƁB
	�e���|�����t�@�C�����쐬�����ꍇ�́AGetName�ŕԂ��Ă���t�@�C�����́A���̃t�@�C�����ƂȂ�B
	�e���|�����t�@�C���́AClose���A�f�X�g���N�^�ō폜�����B
	�܂�A��������̃t�@�C���ł͎��s�ł��Ȃ�API(��FMCI�֘A��)�ɑ΂��A�ꎞ�t�@�C����p�ӂ���̂Ɏg���B
	*/

	///	�f�[�^�̕ۑ��^�ǂݍ���(class CSerialize�ƕ��p����Ɨǂ�)
	virtual LRESULT		Load(const string& filename,vector<BYTE>*vData);
	virtual LRESULT		Save(const string& filename,vector<BYTE>*vData);

	//	�f�[�^�̕ۑ�(������ۑ�)
	virtual LRESULT		Save(const string& filename,vector<string>*vData);

	///	-----------------------------------------------------
	///		���@�t�@�C���̒T��path��ݒ肷��

	///	Read����Ƃ���Path�ݒ�
	static void		SetPathList(const smart_ptr<vector<string> >& aszPathList){ m_aszPathList = aszPathList; }
	///	��vList�ɂ�����Ă���p�X�ւ��t�@�C�������ɍs���B
	///	�J�����g�t�H���_����̃p�X���D�挟���o�H�B
	///	�p�X���̍Ō���ɂ́A'\'�͕s�v�B

	static smart_ptr<vector<string> > GetPathList() { return m_aszPathList; }

	///	-----------------------------------------------------

	///		�N���t�H���_���o�n
	/**
	CFile���ΏۂƂ��Ă���̂́ACFile�̃����om_szCurrentDirectory�ł��B
	����́AGetCurrentDir��SetCurrentDir���Ăяo�����܂ŏ���������܂���B
	�N������̃J�����g�t�H���_��CFile�̑Ώۃt�H���_�Ƃ������̂ł���΁A
	�N�������CFile::SetCurrentDir()���Ăяo���K�v������܂��B
	���̊֐��́A���݂̃J�����g�t�H���_��CFile�̃J�����g�t�H���_�ɐݒ肷�邩��ł��B
	Windows�́A�_�C�A���O�����J���ăt�H���_�ړ����s�Ȃ��ƁA�������J�����g�t�H���_��
	�ݒ肵�Ă��܂��܂��̂ŁA�����Ȃ��Ă���GetCurrentDir���Ăяo���ƁA
	CFile�̃����om_szCurrentDirectory�́A����������Ă��Ȃ����߁A
	���̏u�ԁA���݂̃J�����g�t�H���_���擾���悤�Ƃ��܂����A
	����͈ړ�������̃t�H���_�ł��邽�߁A�N���t�H���_���΂ɂ͂Ȃ�܂���B
	�ׂ������Ƃł����A�C�����Ă��������B

	class CFileInitializer ���Aclass CAppInitializer �̏������R�[���o�b�N�ɂ�����
	SetCurrentDir����̂ŁAclass CAppInitializer ��p���Ă���v���O�����Ȃ��
	�ȏ�̂��Ƃ́A�C�ɂ��Ȃ��ėǂ��B
	*/
	static string	GetCurrentDir();
	/// ���݁ACFile�ŃJ�����g�t�H���_�Ƃ��Đݒ肳��Ă���t�H���_�����擾

	static void		SetCurrentDir();
	/// ���݂̃J�����g�t�H���_���ACFile�̃J�����g�t�H���_�ɂ���

	static void		SetCurrentDir(const string& dir);
	/// �w�肵���p�X��CFile�̃J�����g�t�H���_�ɂ���B

	static	LRESULT	GetParentDir(string& filename);
	//	�e�t�H���_��Ԃ�
	/**
	��j "c:\test1\test2\test3.exe" -> "c:\test1\test2\" -> "c:\test1\" -> "c:\" -> "c:\"�Ƃ����悤�ɁA��x�Ăяo�����ƂɈ���e�t�H���_�ɂȂ�B�l�b�g���[�N�h���C�u�ɂ��Ή����Ă���B�i�ȉ��̗�j

	��
	\\ -> \\
	\\aaa -> \\aaa\
	\\aaa\ -> \\aaa\
	\\aaa\bbb -> \\aaa\
	\\aaa\bbb\ -> \\aaa\
	\\aaa\bbb\ccc -> \\aaa\bbb\
	\\aaa\bbb\ccc\ -> \\aaa\bbb\
	\\aaa\bbb\ccc\ddd -> \\aaa\bbb\ccc\
	\\aaa\bbb\ccc\ddd\ -> \\aaa\bbb\ccc\
	\ -> \
	C: -> C:\
	C:\ -> C:\
	C:\aaa -> C:\
	C:\aaa\ -> C:\
	C:\aaa\bbb -> C:\aaa\
	C:\aaa\bbb\ -> C:\aaa\
	C:\aaa\bbb\ccc -> C:\aaa\bbb\
	C:\aaa\bbb\ccc\ -> C:\aaa\bbb\
	C:\aaa\bbb\ccc\ddd -> C:\aaa\bbb\ccc\
	C:\aaa\bbb\ccc\ddd\ -> C:\aaa\bbb\ccc\
	*/

	static string	MakeFullName(const string& filename);
	/// �N���f�B���N�g�������Ċ��S�p�X�����
	/**
	�ˁ@���̂Ƃ��A / �i�X���b�V���j�́A \ �i�o�b�N�X���b�V���j�֕ϊ�����悤�ɂȂ��Ă��܂��B
	�@�@�i�������Ȃ��ƁA���܂������Ȃ��R�[�f�b�N�������݂��邽�߁j
	�@�@����āA���炩�̓ǂݍ��݊֐������삷��ꍇ�́A���Ȃ炸�A
	�@�@����MakeFullName���o�R�������ق�������ƌ����܂��B
	*/

	static string	GetWindowsDir();
	///	Windows�f�B���N�g���̎擾�i�I�[��'\'�j

	///		�t�@�C���֘A�����񑀍�n
	static string	GetSuffixOf(const string&);
	///		�t�@�C�����̊g���q�擾

	static string	GetPureFileNameOf(const string&);
	///		�p�X�t���t�@�C��������t�@�C���������݂̂��擾

	static void		ToLower(string &);
	///		������������

	static LRESULT	GetTemporaryFileName(string& szFileName);
	///	�e���|�����t�@�C�����擾����
	///	���̊֐������s�����u�ԁA�t�@�C������������A���̃t�@�C�������Ԃ�

	/**
	�t�@�C�����́A�����I��MakeFullName�Ŋ��S�p�X�t���̃t�@�C�����ɕϊ�����Ă�����s����܂��B
	MakeFullName�ɂ́ACFile�̃����om_szCurrentDirectory���g���܂��B
	�i��̑Ώۃt�H���_�ݒ�n�̐�����ǂނ��Ɓj
	�ŏ��̌Ăяo���i�K�ŁA���̃����o�ϐ�������������Ă��Ȃ��ꍇ�́A
	���݂̃J�����g�f�B���N�g�����擾���A�����m_szCurrentDirectory�ɂ��܂��B
	�ʏ�A���̎d�l�Ŗ��Ȃ��Ǝv���̂ł����A�_�C�A���O�����g���ꍇ�͂��̌���ł͂���܂���B
	���ӂ��Ă��������B

	MakeFullName�ł́A"../../test.wav"�̂悤�ȋ삯�オ��p�X���T�|�[�g���Ă��܂��B
	Open���̊֐��ł��A�ŏ��ɂ��̊֐����Ăяo���ăp�X�␳������̂ŁA
	���̂悤�ȋ삯�オ��p�X�����p�ł��܂��B

	�i�ꉞ�ACAppInitializer::Init()����SetCurrentDir()���Ăяo���Ă���̂Ŗ��Ȃ��Ǝv���܂����A
	�@CFile�����������̃v���O�����ɗ��p�����Ƃ��Ȃǂɂ͋C�����Ă��������j

	�܂��A�t�@�C����ǂݍ��񂾂Ƃ��ɓ����I�Ɋm�ۂ���郁�����́A
	�e�L�X�g�t�@�C����ǂݍ��񂾂Ƃ��ɁA�Ō��'\0'��ۏ؂��邽�߁A
	�P�o�C�g���߂Ɋm�ۂ��čŌ�̃o�C�g��'\0'�ɂ��Ă��܂�
*/

	static bool PathFileExists(const string& fullname);

	CFile();
	virtual ~CFile();

protected:
	LRESULT			InnerRead(const string& filename);				//	Read��path�����o�[�W����
	LRESULT			InnerOpen(const string& filename,const string& access);	//	Open��path�����o�[�W����

	FILE*			m_lpFile;			// �������ݎ��Ɏg�p����(cf.yanefileDLL)
	LPVOID			m_lpFileAdr;		// �t�@�C���̓ǂݍ��񂾃A�h���X
	DWORD			m_dwFileSize;		// �t�@�C���T�C�Y�Ȃ́I
	LPSTR			m_lpFileMemPos;		// ReadLine�̂��߂̃������|�W�V����
	string			m_szFileName;		// �ǂݍ���ł���t�@�C����
	bool			m_bEncode;
	bool			m_bTempFile;		//	�e���|�����t�@�C���Ȃ̂��H(close�̍ۂɍ폜�����)
	bool			m_bCompress;		//	Open�œǂݍ��񂾃t�@�C���͈��k�t�@�C���������̂��H
	bool			m_bMemoryImage;		//	��������ɓǂݍ��܂ꂽ���z�t�@�C��

	static string	m_szCurrentDirectory;		// ���ݑΏۂƂ��Ă���f�B���N�g��
	static smart_ptr<vector<string> >	m_aszPathList;	//�p�X���X�g
};

//	CFile�̃O���[�o���������q
class CFileInitializer {
	static CFileInitializer m_dummy;
public:
	CFileInitializer();
};

#endif