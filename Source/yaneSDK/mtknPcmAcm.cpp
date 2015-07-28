#include "stdafx.h"
#include <memory>
#include <msacm.h>

#pragma comment(lib,"msacm32.lib")

#include "mtknPCMReader.h"
#include "mtknwave.h"
//--- �ǉ� '01/11/19  by enra---
#include "enraPCMReaderFactory.h"
//------------------------------

namespace mtknLib {
class kmACMtoPCM : public virtual IkmPCMstream
{
protected:
	WAVEFORMATEX	*m_psrcWFormat;		//	�ϊ��O�̃t�H�[�}�b�g
	WAVEFORMATEX	m_destWFormat;		//	�ϊ���t�H�[�}�b�g
	HACMSTREAM		m_hAcm;				//	handle of acmStream

	BYTE *buddyBuffer;
	DWORD buddyLength;
	int buddyIndex;
	int minimumDest;

	void * srcBuf;
	IkmWaveFileReader *f;
	bool m_bLoop;

	//ACM�̓u���b�N���̐����{�ȊO���ƁA�͂˂��邱�Ƃ�����
	//���̂��߁A���r���[�ȕ����o�b�t�@�ŋz������B
	void *srcAlloc(void *src,DWORD ReadCompressSize)
	{
		if(!src)
		{
			src=GlobalAlloc(GMEM_FIXED, ReadCompressSize);
		}else if(GlobalSize(srcBuf) < ReadCompressSize){
			src=GlobalReAlloc(src, ReadCompressSize, GMEM_MOVEABLE);
		}
		if(!src){
			Err.Out("kmACMtoPCM::alloc. %d byte Allocation Error: %x.",
					 ReadCompressSize,GetLastError());
			return 0;
		}
		return src;
	}

	DWORD BuddyCopy(BYTE *&lpDestBuf,DWORD &destSize)
	{
		DWORD CopySize = min(buddyLength, destSize);
		if(CopySize != 0){
//			Err.Out("msg: use BuddyBuff %d/%d byte.",CopySize,buddyLength);
			memcpy(lpDestBuf, buddyBuffer + buddyIndex, CopySize );
			lpDestBuf += CopySize;
			destSize  -= CopySize;
			buddyLength -= CopySize;
			buddyIndex  += CopySize;
			if(buddyLength == 0)
				buddyIndex =0;
		}
		return CopySize;
	}

	//�o�b�t�@��ACM���Ή��ł���u���b�N�T�C�Y�ɂ���
	DWORD getReadBuffer(DWORD srcsize)
	{
		DWORD newdest;
		if (acmStreamSize(m_hAcm, srcsize, &newdest,
						ACM_STREAMSIZEF_SOURCE ) !=0 ){
			return 0;	//	�Ȃ�ł�˂�ƌ��������i�΁j
		}
		return newdest;
	}

	DWORD doConvert(LPVOID srcBuf, DWORD srcSize, LPVOID lpDestBuf, DWORD destSize);
	
public:
	kmACMtoPCM(void)
	{
		m_psrcWFormat = NULL;	//	�ϊ��O�̃t�H�[�}�b�g
		ZERO(m_destWFormat);	//	�ϊ���t�H�[�}�b�g
		m_hAcm = NULL;			//	handle of acmStream

		buddyBuffer = NULL;
		buddyLength = 0;
		buddyIndex = 0;
		minimumDest = 0;

		srcBuf = NULL;
		f = NULL;
		m_bLoop = true;
	}

	bool		Open(const char *filename);

	~kmACMtoPCM()
	{
		delete f;
		if (m_hAcm!=NULL) acmStreamClose(m_hAcm,NULL);
		if (srcBuf!=NULL) GlobalFree(srcBuf);
		if (buddyBuffer!=NULL) GlobalFree(buddyBuffer);
	}

	bool SetLoop(bool f)
	{
		bool old=m_bLoop;
		m_bLoop=f;
		return old;
	}

	WAVEFORMATEX	*GetFormat(void)
	{	return &m_destWFormat;}					//	����ŕϊ���t�H�[�}�b�g���擾

	DWORD Read(BYTE *lpDestBuf,DWORD destSize);	//	����ŕϊ�����

	//time�b�̃o�b�t�@�������߂�B�������A�u���b�N�T�C�Y�̂ق����D��B
	virtual DWORD GetBlockAlignedBufferSize(DWORD time)
	{
		DWORD destSize= m_destWFormat.nSamplesPerSec 
					  * m_destWFormat.nBlockAlign
					  * time ;
		DWORD  srcsize,newdest;

		if (acmStreamSize(m_hAcm, destSize, &srcsize,
						ACM_STREAMSIZEF_DESTINATION ) !=0 ){
			return 0;	//	�Ȃ�ł�˂�ƌ��������i�΁j
		}
		if (acmStreamSize(m_hAcm, srcsize, &newdest,
						ACM_STREAMSIZEF_SOURCE ) !=0 ){
			return 0;	//	�Ȃ�ł�˂�ƌ��������i�΁j
		}
		return newdest;
	}
	virtual DWORD GetPCMLength(void)
	{
		DWORD  srcsize = f->getFileSize();
		DWORD  newdest;
		if (acmStreamSize(m_hAcm, srcsize, &newdest,
						ACM_STREAMSIZEF_SOURCE ) !=0 ){
			return 0;	//	�Ȃ�ł�˂�ƌ��������i�΁j
		}
		return newdest;
	}

	virtual LONG GetLength(void)
	{
		DWORD size = GetPCMLength();
		return (LONG)( (DWORDLONG)size * 1000  / m_destWFormat.nAvgBytesPerSec);
	}

	virtual bool SetPos(DWORD posByte)
	{
		DWORD srcpos;
		if (acmStreamSize(m_hAcm, posByte, &srcpos,
						ACM_STREAMSIZEF_DESTINATION) !=0 ){
//			Err.Out("setPos acmStreamSize Error");
//			return false;	//	�Ȃ�ł�˂�ƌ��������i�΁j
			srcpos = 0;
		}
//		Err.Out("ACM setPos %d,%d",srcpos,posByte);
		f->seekToTop();
		f->seek(srcpos);
		return true;
	}	

	virtual LONG GetCurrentPos(void)
	{
		DWORD dwBytes = f->GetCurrentPos();
//		Err.Out("GetCurPos:%d",dwBytes);
		return (LONG)( (DWORDLONG)dwBytes * 1000  / m_psrcWFormat->nAvgBytesPerSec);
	}

	virtual DWORD GetPosToByte(LONG lPos)	// �A��͓W�J��̃T�C�Y�ł�
	{
		DWORD	dwBytes;
		dwBytes = lPos / 1000 * m_destWFormat.nAvgBytesPerSec;
		return dwBytes;
	}
	virtual LONG GetByteToPos(DWORD dwByte) 
	{
		LONG lPos;
		lPos = (LONG)( (DWORDLONG)dwByte * 1000 / m_psrcWFormat->nAvgBytesPerSec);
		return lPos;
	}

};


bool kmACMtoPCM::Open(const char *filename)
{
	std::auto_ptr<IkmWaveFileReader> fr( IkmWaveFileReader::create(filename) );

	if( !fr.get() ){
		return false;
	}

	m_psrcWFormat=fr->getWaveFormat();
	if( !m_psrcWFormat )
	{
		Err.Out("kmACMtoPCM:getWaveFotmat���s");
		return false;
	}

	if(m_psrcWFormat->wFormatTag == WAVE_FORMAT_PCM){
		return false;
	}

	ZERO(m_destWFormat);
	m_destWFormat.wFormatTag = WAVE_FORMAT_PCM;	//	PCM�ɂȂ��ė~�����˂�I
	if (acmFormatSuggest(NULL, m_psrcWFormat, &m_destWFormat,
						 sizeof(WAVEFORMATEX), ACM_FORMATSUGGESTF_WFORMATTAG) !=0 ){
		Err.Out("kmACMtoPCM::open acmFormatSuggest. No such ACM? %x",m_psrcWFormat->wFormatTag);
		return false;	//	acm������Ƃ��Ⴄ�H
	}
	if (acmStreamOpen(&m_hAcm, NULL, m_psrcWFormat, &m_destWFormat,
					  NULL, NULL, NULL, ACM_STREAMOPENF_NONREALTIME) !=0 ){
		Err.Out("kmACMtoPCM::open acmStreamOpen. No such ACM? %x",m_psrcWFormat->wFormatTag);
		return false;	//	acm��������Ƃ��Ⴄ�H
	}

	if( (minimumDest=getReadBuffer(m_psrcWFormat->nBlockAlign) ) == 0){
		Err.Out("Minimun BUffer Unknowm.");
	}

	f=fr.release();
	return true;
}


DWORD kmACMtoPCM::Read(BYTE *lpDestBuf,DWORD destSize)
{
	if( !lpDestBuf )
		return 0;

	DWORD CopySize = BuddyCopy(lpDestBuf,destSize);

	if(destSize==0 )
		return CopySize;

	DWORD actualRead,convSize;
	DWORD ReadCompressSize;

	//�u���b�N����菬�����Ȃ�A�u���b�N�����ǂݍ��ށB
	if(minimumDest > destSize){
		if (acmStreamSize(m_hAcm, minimumDest, &ReadCompressSize,
						  ACM_STREAMSIZEF_DESTINATION ) !=0 ){
			Err.Out("ACM::Read StreamSize Failed:%d (min:%d)",destSize,minimumDest);
			return 0;
		}
		buddyBuffer = (BYTE*)srcAlloc( buddyBuffer, minimumDest		);
		srcBuf		= (void*)srcAlloc( srcBuf	  , ReadCompressSize);
		actualRead	= f->read ((BYTE*)srcBuf, ReadCompressSize);
		if( actualRead != ReadCompressSize  &&  m_bLoop)
			f->seekToTop();
		buddyLength = doConvert(srcBuf, actualRead, buddyBuffer, minimumDest);
		buddyIndex=0;
		return BuddyCopy(lpDestBuf,destSize);
	}

	if (acmStreamSize(m_hAcm, destSize, &ReadCompressSize,
					  ACM_STREAMSIZEF_DESTINATION ) !=0 ){
		Err.Out("ACM::Read StreamSize Failed:%d (min:%d)",destSize,minimumDest);
		return 0;
	}

	srcBuf=srcAlloc(srcBuf,ReadCompressSize);
	actualRead	= f->read((BYTE*)srcBuf, ReadCompressSize);
	convSize	= doConvert(srcBuf, actualRead, lpDestBuf, destSize);

	if( actualRead != ReadCompressSize  &&  m_bLoop)
	{
		f->seekToTop();
		return convSize + this->Read(lpDestBuf + convSize,  destSize - convSize);
	}
	return convSize;
}

DWORD kmACMtoPCM::doConvert(LPVOID srcBuf, DWORD srcSize, LPVOID lpDestBuf, DWORD destSize)
{
	ACMSTREAMHEADER	acmheader;		//	header of acmStream
	ZeroMemory(&acmheader,sizeof(acmheader));
	acmheader.cbStruct		=	sizeof(acmheader);
	acmheader.pbSrc			=	(BYTE*)srcBuf;
	acmheader.cbSrcLength	=	srcSize;
	acmheader.pbDst			=	(BYTE*)lpDestBuf;		//	�����ɃR�s�[�������˂�I
	acmheader.cbDstLength	=	destSize;

	if (acmStreamPrepareHeader(m_hAcm,&acmheader,NULL)!=0) {
		Err.Out("kmACMtoPCM::convert acmPrepareHeader�Ŏ��s:%x",GetLastError());
		return 0;	//	���ق��ā`�i�΁j
	}
	if (acmStreamConvert(m_hAcm,&acmheader,ACM_STREAMCONVERTF_BLOCKALIGN )!=0){
		Err.Out("kmACMtoPCM::convert acmStreamConvert�Ŏ��s:%x",GetLastError());
		return 0;	//	�_�������i�΁j
	}
	if (acmStreamUnprepareHeader(m_hAcm,&acmheader,NULL)!=0){
		Err.Out("kmACMtoPCM::convert acmUnprepareHeader�Ŏ��s:%x",GetLastError());
		return 0;	//	�_�������i�΁j
	}

	f->seek(acmheader.cbSrcLengthUsed - srcSize);
	if(acmheader.cbDstLengthUsed == 0 )
	{
		FillMemory( (BYTE*)lpDestBuf + acmheader.cbDstLengthUsed, 
					destSize  - acmheader.cbDstLengthUsed, 
					(BYTE)(m_destWFormat.wBitsPerSample == 8 ? 128 : 0 ) 
					);
		return destSize;
	}
	return acmheader.cbDstLengthUsed;
}
};

//--- �C�� '01/11/19  by enra---
mtknLib::IkmPCMstream* CPCMReaderFactory::CreateACM()
{
	return new mtknLib::kmACMtoPCM;
}