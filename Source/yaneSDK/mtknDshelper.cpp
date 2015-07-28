#include "stdafx.h"

#include "yaneDirectSound.h"
#include "mtknPcmreader.h"
#include "mtknDshelper.h"
namespace mtknLib {

class kmStreamSoundHelper : public IkmStreamSoundHelper
{
	CDirectSound *cds;
	IDirectSoundBuffer *m_lpDSBuffer;
	IkmPCMstream *m_reader;

	DWORD m_differ;
	DWORD m_writepos;
	DWORD m_prevWritepos;
	DWORD m_bufferSize;
	
public:
	kmStreamSoundHelper(
			class CDirectSound *a_cds,
			class IkmPCMstream *a_reader
		)
		: cds( a_cds )
		, m_reader( a_reader)
		, m_lpDSBuffer(NULL)
	{
		CDirectSound::AddRef();
		init();
	}
	bool init()
	{
		m_differ=0;
		m_writepos=0;
		m_prevWritepos=0;
		m_bufferSize=0;
		return true;
	}
	~kmStreamSoundHelper()
	{
		if( m_lpDSBuffer ){
			m_lpDSBuffer->Stop();
			m_lpDSBuffer->Release();
			m_lpDSBuffer=NULL;
		}
		CDirectSound::DelRef();
	}

	DWORD getBufferSize()
	{	return m_bufferSize;	}

	int m_time;
	IDirectSoundBuffer *CreateDirectSoundBuffer(int time)
	{
		m_time=time;
		DSBUFFERDESC  dsbdDesc;
		ZeroMemory(&dsbdDesc, sizeof(DSBUFFERDESC));
		dsbdDesc.dwSize = sizeof(DSBUFFERDESC);
		//--- �C�� '01/11/21  by enra ---
		dsbdDesc.dwFlags = 
			DSBCAPS_CTRLVOLUME |
			// DSBCAPS_CTRLPOSITIONNOTIFY���w�肷���VxD�h���C�o�ł�
			// �����I�Ƀ\�t�g�E�F�A�o�b�t�@�ɂȂ�B
			// �����炱�̃N���X�ł͍ŏ�����\�t�g�E�F�A�o�b�t�@���w�肵�ėǂ��B
			DSBCAPS_LOCSOFTWARE |
			DSBCAPS_GETCURRENTPOSITION2 |
			DSBCAPS_GLOBALFOCUS |
			//DSBCAPS_CTRLPOSITIONNOTIFY |
			0;
		//-------------------------------
		m_bufferSize			= m_reader->GetBlockAlignedBufferSize(time);
		dsbdDesc.dwBufferBytes	= m_bufferSize;
		dsbdDesc.lpwfxFormat	= m_reader->GetFormat();

		// DirectSound�o�b�t�@�̍쐬
		HRESULT hr;
		if FAILED(hr=cds->m_lpDirectSound->CreateSoundBuffer
								(&dsbdDesc,&m_lpDSBuffer, NULL) )
		{
			Err.Out("kmStreamSoundHelper::CreateDirectSoundBuffer��CreateSoundBuffer()�Ŏ��s�I:%x",hr);
			return NULL;
		}
		ClearDSBuffer();
		return m_lpDSBuffer;
	}

	//--- �ǉ� '01/01/07  by enra ---
	LRESULT ClearDSBuffer(void)
	{
		// ���b�N
		BYTE* lpDSBuffData = NULL;
		DWORD dwBufferLength = 0;
		HRESULT hr;
		hr = m_lpDSBuffer->Lock(0, m_bufferSize, (void**)&lpDSBuffData, &dwBufferLength, NULL, 0, 0);
		if (hr==DSERR_BUFFERLOST){
			cds->CheckSoundLost(); 
			m_lpDSBuffer->Restore();
			hr = m_lpDSBuffer->Lock(0, m_bufferSize, (void**)&lpDSBuffData, &dwBufferLength, NULL, 0, 0);
		}
		if FAILED(hr) {
			m_lpDSBuffer->Stop();
			Err.Out("kmStreamSoundHelper::CreateDirectSoundBuffer��m_lpDSBuffer->Lock�Ɏ��s�I%x",hr);
			return 1;
		}
		// �o�b�t�@���[���N���A����
		memset(lpDSBuffData, 0, dwBufferLength);
		// �A�����b�N
		if FAILED(hr=m_lpDSBuffer->Unlock(lpDSBuffData, dwBufferLength, NULL, 0)) {
			Err.Out("kmStreamSoundHelper::CreateDirectSoundBuffer��m_lpDSBuffer->Unlock�Ɏ��s�I%x",hr);
			return 2;
		}
		return 0;
	}
	//-------------------------------

	//IPCMStream���� size ��IDirectSoundBuffer�ɕ��荞�ށB
	DWORD UpdateDirectSound( DWORD size)
	{

		size += m_differ;
		m_differ=0;

		if( m_writepos + size > m_bufferSize){
			DWORD newsize = m_bufferSize - m_writepos;
			m_differ=size - newsize;
			size=newsize;
		}

		BYTE* lpDSBuffData;
		DWORD dwBufferLength;

		//���b�N
		HRESULT hr;
		hr = m_lpDSBuffer->Lock( m_writepos, size ,
								(void**)&lpDSBuffData , &dwBufferLength,
								NULL, 0, 0 );
		if (hr==DSERR_BUFFERLOST){
			cds->CheckSoundLost();
			m_lpDSBuffer->Restore();
			hr = m_lpDSBuffer->Lock( m_writepos, size,
									(void**)&lpDSBuffData , &dwBufferLength,
									NULL,0,
									0L );
		}

		if FAILED(hr) {
			m_lpDSBuffer->Stop();
			return 8;
		}
		//�ǂݍ���
		DWORD read	= m_reader->Read( lpDSBuffData , dwBufferLength	 );

		//�����鏀��
		if FAILED(hr=m_lpDSBuffer->Unlock(	lpDSBuffData, dwBufferLength, 
										NULL,0 ) ) {
			Err.Out("kmStreamSoundHelper::UpdateDirectSound��m_lpDSBuffer->Unlock�Ɏ��s�I%x",hr);
			return 10;
		}
		m_writepos += read;
		m_writepos %= m_bufferSize;
		//�l�X�ȗv���ɂ��ǂݍ��݂���Ȃ��������͐摗�肷��
		if(read < size)
			m_differ= size - read;

		return read;

	}

	
	virtual DWORD getWritepos()
	{
		return m_writepos;
	}
	virtual bool setPos( DWORD pos)
	{	
		m_differ=0;
		m_writepos=0;
		m_prevWritepos=0;
		return m_reader->SetPos(pos);
	}
};

IkmStreamSoundHelper *IkmStreamSoundHelper::create(
			class CDirectSound *a_cds,
			class IkmPCMstream *a_reader
		)
{
		return new	kmStreamSoundHelper(
			a_cds,	a_reader
		);
}
};