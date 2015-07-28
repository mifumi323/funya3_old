// Version.cpp: CVersion �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Version.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CVersion::CVersion()
{
	HMODULE	hModule;
	HRSRC	hRsrc;
	HGLOBAL	hGlobal;
	LPVOID	pData;
	VS_FIXEDFILEINFO*pVersion;

	m_nMajor=m_nMinor=0;
	sprintf(m_strVersion,"%d.%02d",m_nMajor,m_nMinor);

	// �t�c�[�Ƀ��\�[�X��ǂނ킯�ł���
	hModule = GetModuleHandle(NULL);
	if ((hRsrc = FindResource(hModule,(LPCTSTR)VS_VERSION_INFO,RT_VERSION))==NULL) return;
	if ((hGlobal = LoadResource(hModule,hRsrc))==NULL) return;
	pData = LockResource(hGlobal);
	// wLength(2)+wValueLength(2)+wType(2)+szKey(32)+Padding1(2)=40
	pVersion = (VS_FIXEDFILEINFO*)((BYTE*)pData + 40);
	m_nMajor = pVersion->dwFileVersionMS>>16;
	m_nMinor = pVersion->dwFileVersionMS&0xffff;
	sprintf(m_strVersion,"%d.%02d",m_nMajor,m_nMinor);
}

CVersion::~CVersion()
{

}
