#include "stdafx.h"
#include "yaneCPUID.h"

int		CCPUID::m_nCPUID = 0;

//	WinMainからそのパラメータをそのまま渡してね。
int		CCPUID::GetID(void){
	if (m_nCPUID) return m_nCPUID;	//	すでに検査済みならば、それを返す

	int nID,nErr = 0;

	//	VC++5ではPentium用の命令は使えない。^^;
	#define cpuid __asm _emit 0x0F __asm _emit 0xA2

	//	CPUを検査する
	_asm {
		//	CPUID命令は使えるのか？
		pushfd
		pop		eax
		mov		ebx, eax
		xor		eax,0x200000	//	toggle bit21
		push	eax
		popfd
		pushfd
		pop		eax
		cmp		eax,ebx
		jz		NO_CPUID

		mov		eax,1			//	get version info
		cpuid
		mov		nID,edx
		jmp		CPUID_EXIT

	NO_CPUID:
		mov		nErr,1		//	486 lower
	CPUID_EXIT:
	}

	if (nErr) {
		//	CPUID命令に失敗しているので486以前
		m_nCPUID = 1;
	} else if ((nID & 0x800000)==0) {
		//	MMXのサポートビットが立っていないので非MMX
		m_nCPUID = 2;
	} else if ((nID & 0x8000)==0) {
		//	CMOV条件分岐をサポートしていないのでPentium Pro未満
		m_nCPUID = 3;
	} else {
		//	CMOV条件分岐をサポートしているのでPentium Pro以降
		m_nCPUID = 4;
	}
	return m_nCPUID;
}