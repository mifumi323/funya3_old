// enraAudioPlugin.cpp

#include "stdafx.h"
#include "mtknPcmreader.h"
#include "yaneFile.h"
#include "enraAudioPlugin.h"

bool operator < ( const smart_ptr<CAudioPlugin>& l, const smart_ptr<CAudioPlugin>& r)
{
	// filenameで判別しよう^^;
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
	// 初期化
	ReleasePlugin();
	m_strFilename = CFile::MakeFullName(strPluginFilename);
	// DLLを読み込む
	m_hDll = ::LoadLibrary(m_strFilename.c_str());
	if(m_hDll==NULL){
		Err.Out("CAudioPlugin::InitPlugin \"%s\"のロードに失敗", m_strFilename.c_str());
	}
}

void CAudioPlugin::ReleasePlugin()
{
	// Dllの開放
	if(m_hDll!=NULL){
		::FreeLibrary(m_hDll);
		m_hDll = NULL;
	}
}

mtknLib::IkmPCMstream* CAudioPlugin::QueryInterface()
{
	// DLLの関数を取得する
	if(m_hDll==NULL){ return NULL; }
	typedef void (WINAPI *Func)(LPVOID*,LPCSTR);
	Func CreateInstance = (Func)::GetProcAddress(m_hDll, "CreateInstance");
	if(CreateInstance==NULL){ return NULL; }

	// DLL内のReaderを取得する
	LPVOID lp = NULL;
	CreateInstance(&lp, "AudioReaderPlugin");
	return (mtknLib::IkmPCMstream*)lp;
}