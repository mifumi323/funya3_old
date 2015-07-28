// f3sEditDLL.cpp : DLL �A�v���P�[�V�����p�̃G���g�� �|�C���g���`���܂��B
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

// �f�[�^���擾�B�Ȃ����NULL
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

// ���k�f�[�^���擾�B�Ȃ����NULL
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

// ���k�����o��
F3SEDITDLL_API int __stdcall CompileCompress(LPSTR fileIn, LPSTR fileOut)
{
	// �J��
	HANDLE hFile = ::CreateFile(fileIn,
		GENERIC_READ,		// Read
		FILE_SHARE_READ,	// ReadOpen�Ȃ狤�L�������̂̓}�i�[ 
		NULL,				// security
		OPEN_EXISTING,		// ���݂��ĂȂ���΃G���[
		FILE_ATTRIBUTE_NORMAL,	//	�t�@�C������
		NULL				// �e���v���[�g�t�@�C��
	);
	// ���A�J���Ȃ��H�����Ȃ́B�����Ȃ�ł����B
	if (hFile==INVALID_HANDLE_VALUE) return 1;
	DWORD nFileSize = GetFileSize(hFile,NULL);
	BYTE* lpFileAdr = new BYTE[nFileSize];
	DWORD NumberOfBytesRead;
	ReadFile(hFile,lpFileAdr,nFileSize,&NumberOfBytesRead,NULL);
	// �N���[�Y���Ȃ����
	CloseHandle(hFile);

	// ���k
	BYTE* lpDstAdr=NULL;
	CLZSS lzss;
	DWORD nDstSize;
	if (lzss.Encode(lpFileAdr,lpDstAdr,nFileSize,nDstSize)) { DELETEPTR_SAFE(lpFileAdr); return 2; }

	// �����o��
	hFile = ::CreateFile(fileOut,
		GENERIC_WRITE,0,NULL,TRUNCATE_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL
	);
	if (hFile == INVALID_HANDLE_VALUE) { // ��������I
		hFile = ::CreateFile(fileOut,
			GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL
		);
		if (hFile == INVALID_HANDLE_VALUE) {
			DELETEPTR_SAFE(lpDstAdr);
			DELETEPTR_SAFE(lpFileAdr);
			return 3; // Open���s
		}	
	}
	DWORD NumberOfBytesWritten=0;
	WriteFile(hFile, (LPVOID)"funya3s1", strlen("funya3s1"), &NumberOfBytesWritten, NULL);
	WriteFile(hFile, (LPVOID)&nFileSize, sizeof(nFileSize), &NumberOfBytesWritten, NULL);
	WriteFile(hFile, (LPVOID)&nDstSize, sizeof(nDstSize), &NumberOfBytesWritten, NULL);
	WriteFile(hFile, (LPVOID)lpDstAdr, nDstSize, &NumberOfBytesWritten, NULL);
	// ��ɂ���ăN���[�Y
	CloseHandle(hFile);

	// �I������
	DELETEPTR_SAFE(lpDstAdr);
	DELETEPTR_SAFE(lpFileAdr);
	return 0;
}
