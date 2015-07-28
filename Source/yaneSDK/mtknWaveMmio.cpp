#include "stdafx.h"
#include "mtknWave.h"
#include "yaneFile.h"

namespace mtknLib {

class kmMmioFile : public virtual IkmWaveFileReader
{
	std::string m_filename;
	HMMIO hmmio;
	DWORD dwDataLen;
	WAVEFORMATEX  *m_pWaveFormat;
	LONG dwDataPos;
	CFile m_file;

public:
	kmMmioFile() 
		: m_pWaveFormat(NULL)
		,hmmio(NULL)
	{}

	~kmMmioFile(){close();}

	WAVEFORMATEX *getWaveFormat()
	{
		if(!hmmio)
			return NULL;
		return m_pWaveFormat;
	}

	bool close(){
		if(!hmmio)
			return false;
		mmioClose(hmmio,0);
		hmmio=NULL;
		if(m_pWaveFormat)
			free(m_pWaveFormat);
		return true;
	}
	bool open(const char *filename){
		m_filename=filename;
		// ����ƃI�[�v���ł���B�i��͒������[�j
//		char *tFile=strdup(m_filename.c_str());
//		hmmio = mmioOpen(tFile,NULL,MMIO_READ);
//		free(tFile);
//		if (hmmio==NULL) {
//			Err.Out("CSound::LoadWaveFile : mmioOpen�̎��s");
//			return false;
//		}
		if (m_file.Read(m_filename)) {
			Err.Out("kmMmioFile::open " + m_filename + "�̃t�@�C���ǂݍ��ݎ��̃G���[");
			return 1; // �t�@�C���ǂݍ��݃G���[
		}

		// WAVEFORMATEX�\���̂𓾂�
		// ���̌`����wave�f�[�^���l�����Ȃ��ẮI�i��ς����j

		// �f�[�^�́A��������Af.fileadr����f.filesize����������B
		MMIOINFO mmio; // �������t�@�C����mmioOpen����̂��I�I(C)yaneurao
		ZeroMemory(&mmio,sizeof(mmio));
		mmio.pchBuffer = (LPSTR)m_file.GetMemory();	// �����Ƀt�@�C���o�b�t�@adr
		mmio.fccIOProc = FOURCC_MEM;				// ����������I
		mmio.cchBuffer = m_file.GetSize();			// �����Ƀt�@�C���T�C�Y

		// ����ƃI�[�v���ł���B�i��͒������[�j
		hmmio = mmioOpen(NULL,&mmio,MMIO_READ);
		if (hmmio==NULL) {
			Err.Out("kmMmioFile::open : mmioOpen�̎��s");
			return false;
		}

		MMCKINFO ciPC,ciSC; // �e�`�����N�ƃT�u�`�����N
		ciPC.fccType = mmioFOURCC('W','A','V','E');
		if (mmioDescend(hmmio,(LPMMCKINFO)&ciPC,NULL,MMIO_FINDRIFF)){
			mmioClose(hmmio,0);
			//	�Ђ���Ƃ���mp3���H
			return false;
		}
		
		ciSC.ckid = mmioFOURCC('f','m','t',' ');
		if (mmioDescend(hmmio,&ciSC,&ciPC,MMIO_FINDCHUNK)){
			Err.Out("CSound::LoadWaveFile : fmt�`�����N���w��ł��Ȃ�");
			mmioClose(hmmio,0);
			return false;
		}

		m_pWaveFormat = (WAVEFORMATEX*)calloc(1, max(sizeof(WAVEFORMATEX),(size_t)ciSC.cksize) );
		mmioRead(hmmio,(HPSTR)m_pWaveFormat,ciSC.cksize);
		if(m_pWaveFormat->cbSize ==0 )
			m_pWaveFormat->cbSize=sizeof(WAVEFORMATEX);


		mmioAscend(hmmio,&ciSC,0); // fmt�T�u�`�����N�O���ֈړ�
		ciSC.ckid = mmioFOURCC('d','a','t','a');
		if (mmioDescend(hmmio,&ciSC,&ciPC,MMIO_FINDCHUNK)){
			Err.Out("CSound::LoadWaveFile : �f�[�^�`�����N�ɍs���Ȃ�");
			mmioClose(hmmio,0);
			return false;
		}

		dwDataLen = ciSC.cksize; // �f�[�^�T�C�Y
		dwDataPos=mmioSeek(hmmio,0,SEEK_CUR);
		return true;
	}
	virtual DWORD		  getFileSize() const
	{	
		return dwDataLen;
	}
	
	LONG read(BYTE *buf,DWORD size)
	{
		if(!hmmio)
			return 0;
		return mmioRead(hmmio,(HPSTR)buf,size);
	}

	virtual int seekToTop()
	{
		if(!hmmio)
			return 0;
		return mmioSeek(hmmio,dwDataPos,SEEK_SET);

	}

	virtual int seek(LONG pos)
	{
		if(!hmmio)
			return 0;
		return mmioSeek(hmmio,pos,SEEK_CUR);
	}

	virtual LONG GetCurrentPos(void)
	{
		if ( !hmmio ) return -1;
		Err.Out("gcp dwDataPos %d",dwDataPos);
		return mmioSeek(hmmio,0,SEEK_CUR)-dwDataPos;
	}
	/*
	virtual LRESULT SetCurrentPos(LONG lPos)
	{
		if ( !hmmio ) return -1;
		seek(
		return mmioSeek(hmmio,lPos+dwDataPos,SEEK_SET);
	}
	*/

};

IkmWaveFileReader* IkmWaveFileReader::createMMIO()
{
	return new kmMmioFile;
}
};