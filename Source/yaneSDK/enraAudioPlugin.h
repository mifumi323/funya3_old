// enraAudioPluginReader.h
//	"IkmPCMstream Plugin" Wrapper for CPCMReaderFactory
//
//		programmed by enra '01/12/13

#ifndef __enraAudioPlugin_h__
#define __enraAudioPlugin_h__

class CAudioPlugin
{
public:
	CAudioPlugin(const string& strPluginFilename);
	virtual ~CAudioPlugin();

	// Plugin��Reader�C���^�[�t�F�[�X�̎擾
	mtknLib::IkmPCMstream*	QueryInterface();

	// std::set�p��operator
	friend bool operator < ( const smart_ptr<CAudioPlugin>& l, const smart_ptr<CAudioPlugin>& r);

	string	GetFilename() { return m_strFilename; }

protected:
	// Plugin DLL �̓ǂݍ���
	void	InitPlugin(const string& strPluginFilename);
	// Plugin DLL �̊J��
	void	ReleasePlugin();

private:
	HINSTANCE	m_hDll;
	string		m_strFilename;
};
#endif // __enraAudioPlugin_h__