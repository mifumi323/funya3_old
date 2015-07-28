#include "stdafx.h"
#include "yaneCPUID.h"

int		CCPUID::m_nCPUID = 0;

//	WinMain���炻�̃p�����[�^�����̂܂ܓn���ĂˁB
int		CCPUID::GetID(void){
	if (m_nCPUID) return m_nCPUID;	//	���łɌ����ς݂Ȃ�΁A�����Ԃ�

	int nID,nErr = 0;

	//	VC++5�ł�Pentium�p�̖��߂͎g���Ȃ��B^^;
	#define cpuid __asm _emit 0x0F __asm _emit 0xA2

	//	CPU����������
	_asm {
		//	CPUID���߂͎g����̂��H
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
		//	CPUID���߂Ɏ��s���Ă���̂�486�ȑO
		m_nCPUID = 1;
	} else if ((nID & 0x800000)==0) {
		//	MMX�̃T�|�[�g�r�b�g�������Ă��Ȃ��̂Ŕ�MMX
		m_nCPUID = 2;
	} else if ((nID & 0x8000)==0) {
		//	CMOV����������T�|�[�g���Ă��Ȃ��̂�Pentium Pro����
		m_nCPUID = 3;
	} else {
		//	CMOV����������T�|�[�g���Ă���̂�Pentium Pro�ȍ~
		m_nCPUID = 4;
	}
	return m_nCPUID;
}