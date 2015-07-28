#include "stdafx.h"
#include <objbase.h>
#include "yaneCOMBase.h"

CCriticalSection CCOMBase::m_oCriticalSection;
map<DWORD,int> CCOMBase::m_vCount;

LRESULT CCOMBase::AddRef(void){

	DWORD	dwThreadID = ::GetCurrentThreadId();
	int nCount;

	m_oCriticalSection.Enter();
	{

	map<DWORD,int>::iterator p = m_vCount.find(dwThreadID);
	if (p == m_vCount.end()){
		nCount = 0;
		m_vCount.insert(pair<DWORD,int>(dwThreadID,1)); // �L�[�}��
	} else {
		nCount = p->second;
		p->second = nCount+1;
	}

	}
	m_oCriticalSection.Leave();

	if (nCount==0) {
		if (::CoInitialize(NULL)) {
			Err.Out("CoInitialize�Ɏ��s...");
			return 1;
		}
	}
	return 0;
}

void CCOMBase::Release(void){
	DWORD	dwThreadID = ::GetCurrentThreadId();

	m_oCriticalSection.Enter();
	{

	int nCount;
	map<DWORD,int>::iterator p = m_vCount.find(dwThreadID);
	if (p == m_vCount.end()){
		return ;	//	����Ȃ񖳂��ŁH
	} else {
		nCount = p->second;
	}
	nCount--;
	p->second = nCount;


	if (nCount==0) {
//		  CoUnitialize(); // DirectX�̃}�j���A���̌�L�H
		::CoUninitialize(); // COM�̃V���b�g�_�E��

		m_vCount.erase(p);	//	�����Ă܂��I
	}

	}
	m_oCriticalSection.Leave();
}