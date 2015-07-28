// enraAudioPlugin.cpp

#include "stdafx.h"
#include "mtknPcmreader.h"
#include "yaneFile.h"
#include "enraAudioPlugin.h"

bool operator < ( const smart_ptr<CAudioPlugin>& l, const smart_ptr<CAudioPlugin>& r)
{
	// filename�Ŕ��ʂ��悤^^;
	return (l->GetFilename()) < (r->GetFilename());
}

CAudioPlugin::CAudioPlugin(const string& strPluginFilename)
{
	InitPlugin(strPluginFilename);
}

CAudioPlugin::~CAudioPlugin()
{
	ReleasePlugin();
}

void CAudioPlugin::InitPlugin(const string& strPluginFilename)
{
	// ������
	ReleasePlugin();
	m_strFilename = CFile::MakeFullName(strPluginFilename);
	// DLL��ǂݍ���
	m_hDll = ::LoadLibrary(m_strFilename.c_str());
	if(m_hDll==NULL){
		Err.Out("CAudioPlugin::InitPlugin \"%s\"�̃��[�h�Ɏ��s", m_strFilename.c_str());
	}
}

void CAudioPlugin::ReleasePlugin()
{
	// Dll�̊J��
	if(m_hDll!=NULL){
		::FreeLibrary(m_hDll);
		m_hDll = NULL;
	}
}

mtknLib::IkmPCMstream* CAudioPlugin::QueryInterface()
{
	// DLL�̊֐����擾����
	if(m_hDll==NULL){ return NULL; }
	typedef void (WINAPI *Func)(LPVOID*,LPCSTR);
	Func CreateInstance = (Func)::GetProcAddress(m_hDll, "CreateInstance");
	if(CreateInstance==NULL){ return NULL; }

	// DLL����Reader���擾����
	LPVOID lp = NULL;
	CreateInstance(&lp, "AudioReaderPlugin");
	return (mtknLib::IkmPCMstream*)lp;
}