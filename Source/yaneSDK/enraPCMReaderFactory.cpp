#include "stdafx.h"

#include "mtknPCMReader.h"
#include "enraAudioPlugin.h"
#include "enraPCMReaderFactory.h"
#include "yaneFile.h"
#include "yaneCriticalSection.h"

set< smart_ptr<CAudioPlugin> >	CPCMReaderFactory::m_lpAudioPlugin_List;
CCriticalSection				CPCMReaderFactory::m_vCS;

mtknLib::IkmPCMstream* CPCMReaderFactory::CreateReader(const string& filename)
{
	//	CFile�o�R�œǂގ���O��ɂ�����
	LPCSTR szFilename = filename.c_str();

	{	// PluginLoader����Reader interface���擾���āAopen����
		set< smart_ptr<CAudioPlugin> >::iterator it		= m_lpAudioPlugin_List.begin();
		set< smart_ptr<CAudioPlugin> >::iterator itEnd	= m_lpAudioPlugin_List.end();
		while (it!=itEnd){
			mtknLib::IkmPCMstream* f = (*it)->QueryInterface();
			if (f && f->Open(szFilename)){ return f; }
			if (f!=NULL){ f->DeleteSelf(); }
			++it;
		}
	}

	{	// mmio�ɂ��wave�t�@�C����open
		mtknLib::IkmPCMstream* f = CreateRAW();
		if (f && f->Open(szFilename)){ return f; }
		if (f!=NULL){ f->DeleteSelf(); }
	}

	#if 1
	{	// DirectShowFilter�ɂ��e��t�@�C����open
		mtknLib::IkmPCMstream* f = CreateDirectShow();
		if (f && f->Open(szFilename)){ return f; }
		if (f!=NULL){ f->DeleteSelf(); }
	}
	#endif

	#if 1
	{	// ACM�ɂ��e��t�@�C��(mp3,aiff,...etc)��open
		mtknLib::IkmPCMstream* f = CreateACM();
		if (f && f->Open(szFilename)){ return f; }
		if (f!=NULL){ f->DeleteSelf(); }
	}
	#endif

	Err.Out("CPCMReaderFactory::CreateReader Open�ł��Ȃ�-%s", szFilename);
	return NULL;
}

void CPCMReaderFactory::AddPlugin(const string& strPluginFilename)
{
	//	�}���`�X���b�h�Ή��`
	m_vCS.Enter();
	AddPlugin(smart_ptr<CAudioPlugin>(new CAudioPlugin(strPluginFilename),true));
	m_vCS.Leave();
}

void CPCMReaderFactory::AddPlugin(const smart_ptr<CAudioPlugin>& lpPlugin)
{
	//	�}���`�X���b�h�Ή��`
	m_vCS.Enter();
	m_lpAudioPlugin_List.insert(lpPlugin);
	m_vCS.Leave();
}