// yaneDir.h :
//
//		directory file���� wrapper
//
//			programmed by yaneurao(M.Isozaki) '99/10/19 - '99/11/28

#ifndef __yaneDir_h__
#define __yaneDir_h__

class CDir {
public:
	//	Directory
	void	SetFindFile(const string& file);					//	�����t�@�C�����ݒ�
	void	SetPath(const string& file);						//	�����p�X�ݒ�
	void	EnableSubdir(bool b) { m_bSubDir = b; }				//	�����̂Ƃ��ɃT�u�t�H���_��L���ɂ���̂�
	void	EnablePackFile(bool b) { m_bPackFile = b; }			//	yanePack�t�@�C���������Ώۂɂ���̂��H

	LRESULT	FindFile(string& file);								//	�t�@�C�����擾(�Ԃ��l��O�Ȃ��Not Found)
	static bool IsMatch(LPCSTR it1,LPCSTR it2);					//	��̕����񂪈�v���邩�H(wildcard�g�p�\)
	void	Reset(void) { CloseFindFile(); }

	void	EnableDirListup(bool b) { m_bAllFile = b; }			//	�t�H���_���}�b�`���O�Ώۂɂ���
	bool	IsDir(void) const { return m_bDir; }				//	FindFile�Ŏ擾�����̂̓t�H���_���H

	bool	IsFileExist(const string& file);					//	���̃t�@�C���͑��݂���̂��H

	//	Drives
	DWORD	CheckDrive() { return m_dwDrive=::GetLogicalDrives(); }
	bool	IsExistDrive(int n) const { return (m_dwDrive & (1L << n))!=0; }

	CDir();
	virtual ~CDir();

protected:
	void	CloseFindFile(void);								//	�I������
	string	m_file;			//	�����t�@�C��
	string	m_path;			//	path
	HANDLE	m_handle;		//	�����n���h��
	bool	m_bSubDir;		//	subdir�L�����H	
	bool	m_bPackFile;	//	packfile�L�����H	
	CDir*	m_lpDir;		//	subdir�����p(�ċA�I�ɗ��p)

	//	�t�H���_�̃��X�g�A�b�v�p
	bool	m_bAllFile;		//	�t�H���_�������Ώۂɂ���̂��H
	bool	m_bDir;			//	���܎擾�����̂̓t�H���_���H

	//	���k�t�@�C���p
	HANDLE	m_hPackFile;	//	���k�t�@�C�������p�n���h��
	DWORD	m_dwFileNum;	//	���k�t�@�C�����t�@�C����
	int		m_nPackFile;	//	���k�t�@�C���� 0:yanePack 1:yanePackEx 2:yanePackDx
	string	m_PackName;		//	�������̈��k�t�@�C����
protected:
	DWORD	m_dwDrive;
};

#endif