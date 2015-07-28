// f3sEditDLL.cpp : DLL アプリケーション用のエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "yaneMacro.h"
#include "yaneLZSS.h"

#ifdef F3SEDITDLL_EXPORTS
#define F3SEDITDLL_API __declspec(dllexport)
#else
#define F3SEDITDLL_API __declspec(dllimport)
#endif

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

// データを取得。なければNULL
F3SEDITDLL_API BYTE* __stdcall GetStageData(BYTE *lpData, DWORD dwDataSize, DWORD dwType, DWORD *lpdwSize)
{
	if (lpdwSize != NULL) *lpdwSize = 0;
	if (lpData==NULL) return NULL;

	DWORD NumberOfBytesRead = 0;
	DWORD size, type;
	while (NumberOfBytesRead+8<=dwDataSize) {
		size = *(DWORD*)(lpData+NumberOfBytesRead);
		NumberOfBytesRead += 4;
		type = *(DWORD*)(lpData+NumberOfBytesRead);
		NumberOfBytesRead += 4;
		if (type == dwType) {
			if (lpdwSize != NULL) *lpdwSize = size;
			return lpData+NumberOfBytesRead;
		}
		NumberOfBytesRead += size;
	}
	return NULL;
}

// 圧縮データを取得。なければNULL
F3SEDITDLL_API BYTE* __stdcall GetCompressedStageData(BYTE *lpData, DWORD dwDataSize, DWORD dwPackSize, DWORD dwType, DWORD *lpdwSize)
{
	if (lpData==NULL) return NULL;

	BYTE* data = lpData;
	if (dwDataSize>dwPackSize) {
		data = (BYTE*)::GlobalAlloc(GMEM_FIXED | GMEM_NOCOMPACT,dwDataSize+1);
		CLZSS lzss;
		lzss.Decode(lpData,data,dwDataSize,false);
	}

	BYTE* ret = GetStageData(data, dwDataSize, dwType, lpdwSize);
	if (dwDataSize>dwPackSize) ::GlobalFree(data);
	return ret;
}

// 圧縮書き出し
F3SEDITDLL_API int __stdcall CompileCompress(LPSTR fileIn, LPSTR fileOut)
{
	// 開く
	HANDLE hFile = ::CreateFile(fileIn,
		GENERIC_READ,		// Read
		FILE_SHARE_READ,	// ReadOpenなら共有を許すのはマナー 
		NULL,				// security
		OPEN_EXISTING,		// 存在してなければエラー
		FILE_ATTRIBUTE_NORMAL,	//	ファイル属性
		NULL				// テンプレートファイル
	);
	// あ、開けない？そうなの。そうなんですか。
	if (hFile==INVALID_HANDLE_VALUE) return 1;
	DWORD nFileSize = GetFileSize(hFile,NULL);
	BYTE* lpFileAdr = new BYTE[nFileSize];
	DWORD NumberOfBytesRead;
	ReadFile(hFile,lpFileAdr,nFileSize,&NumberOfBytesRead,NULL);
	// クローズしなきゃね
	CloseHandle(hFile);

	// 圧縮
	BYTE* lpDstAdr=NULL;
	CLZSS lzss;
	DWORD nDstSize;
	if (lzss.Encode(lpFileAdr,lpDstAdr,nFileSize,nDstSize)) { DELETEPTR_SAFE(lpFileAdr); return 2; }

	// 書き出し
	hFile = ::CreateFile(fileOut,
		GENERIC_WRITE,0,NULL,TRUNCATE_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL
	);
	if (hFile == INVALID_HANDLE_VALUE) { // あかんやん！
		hFile = ::CreateFile(fileOut,
			GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL
		);
		if (hFile == INVALID_HANDLE_VALUE) {
			DELETEPTR_SAFE(lpDstAdr);
			DELETEPTR_SAFE(lpFileAdr);
			return 3; // Open失敗
		}	
	}
	DWORD NumberOfBytesWritten=0;
	WriteFile(hFile, (LPVOID)"funya3s1", strlen("funya3s1"), &NumberOfBytesWritten, NULL);
	WriteFile(hFile, (LPVOID)&nFileSize, sizeof(nFileSize), &NumberOfBytesWritten, NULL);
	WriteFile(hFile, (LPVOID)&nDstSize, sizeof(nDstSize), &NumberOfBytesWritten, NULL);
	WriteFile(hFile, (LPVOID)lpDstAdr, nDstSize, &NumberOfBytesWritten, NULL);
	// 例によってクローズ
	CloseHandle(hFile);

	// 終了処理
	DELETEPTR_SAFE(lpDstAdr);
	DELETEPTR_SAFE(lpFileAdr);
	return 0;
}
