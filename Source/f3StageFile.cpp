// f3StageFile.cpp: Cf3StageFile クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3StageFile.h"
#include "yaneSDK/yaneFile.h"
#include "yaneSDK/yaneLZSS.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

Cf3StageFile::Cf3StageFile()
{
}

Cf3StageFile::~Cf3StageFile()
{
	ClearData();
}

// ステージファイルを読み込みメモリに格納する
LRESULT Cf3StageFile::Read(string filename)
{
	ClearData();
	CFile File;
	if (File.Read(filename)) return 1;
	::CopyMemory(&m_StageHeader, File.GetMemory(),sizeof(tagf3StageHeader));
	{
		// ステージヘッダチェック
		char ident[9];
		::CopyMemory(ident, m_StageHeader.ident,8);
		ident[8] = '\0';
		if (strcmp(ident,"funya3s1")) {
			// ふにゃさんのステージちゃうっての
			return 2;
		}
	}
	if (m_StageHeader.datasize == m_StageHeader.packsize) {
		// 無圧縮
		AnalyzeData((BYTE*)File.GetMemory()+sizeof(tagf3StageHeader));
	}ef(m_StageHeader.datasize > m_StageHeader.packsize) {
		// LZSSによる圧縮
		BYTE* data = (BYTE*)::GlobalAlloc(GMEM_FIXED | GMEM_NOCOMPACT,m_StageHeader.datasize+1);
		CLZSS lzss;
		lzss.Decode((BYTE*)File.GetMemory()+sizeof(tagf3StageHeader),data,m_StageHeader.datasize,false);
		AnalyzeData(data);
		::GlobalFree(data);
	}else{
		// 膨張してるなんてありえない！！
		return 3;
	}
	// これにて終了
	return 0;
}

// データを取得。なければNULL
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
	// サイズ計算するのジャー
	DWORD dwSrcSize = 0;
	map<DWORD,HGLOBAL>::iterator it;
	for (it=m_Data.begin(); it!=m_Data.end(); it++) {
		dwSrcSize += 4+4+::GlobalSize((*it).second);
	}
	// 書き込むデータを用意する
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

	// おもむろに圧縮(←「おもむろに」の使い方間違ってる)
	BYTE* lpDstAdr=NULL;
	CLZSS lzss;
	DWORD dwDstSize;
	if (lzss.Encode(lpSrcAdr,lpDstAdr,dwSrcSize,dwDstSize)) {
		dwDstSize = dwSrcSize;
		lpDstAdr = lpSrcAdr;
		lpSrcAdr = new BYTE[1];
	}

	// やーとこさ書き込みジャー
	HANDLE hFile = ::CreateFile(filename.c_str(),
		GENERIC_WRITE,0,NULL,TRUNCATE_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL
	);
	if (hFile == INVALID_HANDLE_VALUE) { // あかんやん！
		hFile = ::CreateFile(filename.c_str(),
			GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL
		);
		if (hFile == INVALID_HANDLE_VALUE) return 3; // Open失敗		
	}
	DWORD NumberOfBytesWritten=0;
	WriteFile(hFile, (LPVOID)"funya3s1", strlen("funya3s1"), &NumberOfBytesWritten, NULL);
	WriteFile(hFile, (LPVOID)&dwSrcSize, sizeof(dwSrcSize), &NumberOfBytesWritten, NULL);
	WriteFile(hFile, (LPVOID)&dwDstSize, sizeof(dwDstSize), &NumberOfBytesWritten, NULL);
	WriteFile(hFile, (LPVOID)lpDstAdr, dwDstSize, &NumberOfBytesWritten, NULL);
	// 例によってクローズ
	CloseHandle(hFile);

	delete[]lpDstAdr;
	delete[]lpSrcAdr;
	return 0;
}
