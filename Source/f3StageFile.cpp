// f3StageFile.cpp: Cf3StageFile �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3StageFile.h"
#include "yaneSDK/yaneFile.h"
#include "yaneSDK/yaneLZSS.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

Cf3StageFile::Cf3StageFile()
{
}

Cf3StageFile::~Cf3StageFile()
{
	ClearData();
}

// �X�e�[�W�t�@�C����ǂݍ��݃������Ɋi�[����
LRESULT Cf3StageFile::Read(string filename)
{
	ClearData();
	CFile File;
	if (File.Read(filename)) return 1;
	::CopyMemory(&m_StageHeader, File.GetMemory(),sizeof(tagf3StageHeader));
	{
		// �X�e�[�W�w�b�_�`�F�b�N
		char ident[9];
		::CopyMemory(ident, m_StageHeader.ident,8);
		ident[8] = '\0';
		if (strcmp(ident,"funya3s1")) {
			// �ӂɂႳ��̃X�e�[�W���Ⴄ���Ă�
			return 2;
		}
	}
	if (m_StageHeader.datasize == m_StageHeader.packsize) {
		// �����k
		AnalyzeData((BYTE*)File.GetMemory()+sizeof(tagf3StageHeader));
	}ef(m_StageHeader.datasize > m_StageHeader.packsize) {
		// LZSS�ɂ�鈳�k
		BYTE* data = (BYTE*)::GlobalAlloc(GMEM_FIXED | GMEM_NOCOMPACT,m_StageHeader.datasize+1);
		CLZSS lzss;
		lzss.Decode((BYTE*)File.GetMemory()+sizeof(tagf3StageHeader),data,m_StageHeader.datasize,false);
		AnalyzeData(data);
		::GlobalFree(data);
	}else{
		// �c�����Ă�Ȃ�Ă��肦�Ȃ��I�I
		return 3;
	}
	// ����ɂďI��
	return 0;
}

// �f�[�^���擾�B�Ȃ����NULL
BYTE* Cf3StageFile::GetStageData(const DWORD dwType, DWORD *dwSize)
{
	map<DWORD,HGLOBAL>::iterator it = m_Data.find(dwType);
	if (it!=m_Data.end()) {
		if (dwSize != NULL) *dwSize = ::GlobalSize((*it).second);
		return (BYTE*)((*it).second);
	}
	return NULL;
}

void Cf3StageFile::AnalyzeData(BYTE *data)
{
	DWORD NumberOfBytesRead = 0;
	DWORD size, type;
	HGLOBAL data2;
	while (NumberOfBytesRead+8<=m_StageHeader.datasize) {
		size = *(DWORD*)(data+NumberOfBytesRead);
		NumberOfBytesRead += 4;
		type = *(DWORD*)(data+NumberOfBytesRead);
		NumberOfBytesRead += 4;
		if (size) {
			data2 = ::GlobalAlloc(GMEM_FIXED | GMEM_NOCOMPACT,size);
			::CopyMemory(data2, data+NumberOfBytesRead,size);
			m_Data.insert(map<DWORD,HGLOBAL>::value_type(type,data2));
		}
		NumberOfBytesRead += size;
	}
}

void Cf3StageFile::ClearData()
{
	for (map<DWORD,HGLOBAL>::iterator it=m_Data.begin(); it!=m_Data.end(); it++) {
		::GlobalFree((*it).second);
	}
	m_Data.clear();
}

void Cf3StageFile::SetStageData(DWORD dwType, DWORD dwSize, void *lpData)
{
	HGLOBAL hData = ::GlobalAlloc(GMEM_FIXED | GMEM_NOCOMPACT,dwSize);
	::CopyMemory(hData, lpData, dwSize);
	m_Data[dwType] = hData;
}

LRESULT Cf3StageFile::Write(string filename)
{
	// �T�C�Y�v�Z����̃W���[
	DWORD dwSrcSize = 0;
	map<DWORD,HGLOBAL>::iterator it;
	for (it=m_Data.begin(); it!=m_Data.end(); it++) {
		dwSrcSize += 4+4+::GlobalSize((*it).second);
	}
	// �������ރf�[�^��p�ӂ���
	BYTE* lpSrcAdr=new BYTE[dwSrcSize];
	BYTE* lpPos = lpSrcAdr;
	for (it=m_Data.begin(); it!=m_Data.end(); it++) {
		DWORD chunksize = ::GlobalSize((*it).second);
		DWORD chunktype = (*it).first;
		::CopyMemory(lpPos, &chunksize, 4);
		lpPos += 4;
		::CopyMemory(lpPos, &chunktype, 4);
		lpPos += 4;
		::CopyMemory(lpPos, (BYTE*)(*it).second, chunksize);
		lpPos += chunksize;
	}

	// �����ނ�Ɉ��k(���u�����ނ�Ɂv�̎g�����Ԉ���Ă�)
	BYTE* lpDstAdr=NULL;
	CLZSS lzss;
	DWORD dwDstSize;
	if (lzss.Encode(lpSrcAdr,lpDstAdr,dwSrcSize,dwDstSize)) {
		dwDstSize = dwSrcSize;
		lpDstAdr = lpSrcAdr;
		lpSrcAdr = new BYTE[1];
	}

	// ��[�Ƃ����������݃W���[
	HANDLE hFile = ::CreateFile(filename.c_str(),
		GENERIC_WRITE,0,NULL,TRUNCATE_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL
	);
	if (hFile == INVALID_HANDLE_VALUE) { // ��������I
		hFile = ::CreateFile(filename.c_str(),
			GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL
		);
		if (hFile == INVALID_HANDLE_VALUE) return 3; // Open���s		
	}
	DWORD NumberOfBytesWritten=0;
	WriteFile(hFile, (LPVOID)"funya3s1", strlen("funya3s1"), &NumberOfBytesWritten, NULL);
	WriteFile(hFile, (LPVOID)&dwSrcSize, sizeof(dwSrcSize), &NumberOfBytesWritten, NULL);
	WriteFile(hFile, (LPVOID)&dwDstSize, sizeof(dwDstSize), &NumberOfBytesWritten, NULL);
	WriteFile(hFile, (LPVOID)lpDstAdr, dwDstSize, &NumberOfBytesWritten, NULL);
	// ��ɂ���ăN���[�Y
	CloseHandle(hFile);

	delete[]lpDstAdr;
	delete[]lpSrcAdr;
	return 0;
}
