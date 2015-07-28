#include "stdafx.h"
#include "mtknLongsound.h"
#include "mtknDshelper.h"

#include "yaneDirectSound.h"
#include "yaneThread.h"

#include "mtknPcmreader.h"
#include "enraPCMReaderFactory.h"

namespace mtknLib {

class kmLongSound : public virtual IkmLongSound, public CThread
{
public:
	void SetOffset(LONG lPos){ m_StartOffsetPos = lPos; }
	LONG GetOffset(void){ return m_StartOffsetPos; }
	void SetLoop(bool bLoop)
	{
		m_CriticalSection.Enter();
		m_bLoop = bLoop;
		if(GetReader()!=NULL) { GetReader()->SetLoop(m_bLoop); }
		m_CriticalSection.Leave();
	}
	bool IsLoop(bool bLoop){ return m_bLoop; }
		

protected:
	bool canUseNotify;
	bool m_bLoop;
	bool m_bOpen;
	bool m_bSuspend;

	//--- �ǉ� '01/11/20  by enra ---
	// �Đ����̃I�t�Z�b�g�ʒu(msec)
	LONG m_StartOffsetPos;
	// �Đ��X���b�h�����������C�x���g
	HANDLE m_hCreateEvent;
	// PCMReader�̃t�@�N�g���[
	smart_ptr<CPCMReaderFactory> m_lpReaderFactory;
	virtual void SetFactory(smart_ptr<CPCMReaderFactory> pv)
	{
		m_lpReaderFactory = pv;
	}
	virtual smart_ptr<CPCMReaderFactory> GetFactory(void)
	{
		return m_lpReaderFactory;
	}
	IkmPCMstream* m_reader;
	IkmPCMstream* GetReader(void) const { return m_reader; }
	//-------------------------------
	//--- �ǉ� '02/01/03  by enra ---
	string m_filename;
	//-------------------------------
	//--- �ǉ� '02/01/07  by enra ---
	CCriticalSection m_CriticalSection;
	//-------------------------------
	//--- �ǉ� '02/03/03  by enra ---
	LONG m_Volume;
	//-------------------------------

	CDirectSound *cds;
	int m_bufTime;

	IkmStreamSoundHelper *m_lpDShelper;
	IDirectSoundBuffer *m_lpDSBuffer;
	IDirectSoundNotify *m_pDSNotify;
	DSBPOSITIONNOTIFY *m_aPosNotify;
	HANDLE m_hNotificationEvent;


	DWORD m_prevWritepos;
	DWORD m_TotalWrittenBytes;

	WAVEFORMATEX *m_pWaveFormat;
	DWORD m_PCMFileSize;

	bool fadeOut()
	{
		HRESULT SetVolume( LONG lVolume );
		return true;
	}
public:
	kmLongSound() 
	{
		CDirectSound::AddRef();
		cds = CDirectSound::GetCDirectSound();
		m_bOpen=false;
		m_bLoop=true;
		m_lpDSBuffer=NULL;
		m_lpDShelper=NULL;
		m_prevWritepos=0;
		m_bufTime=4;
		m_pDSNotify=NULL;
		m_aPosNotify=NULL;
		m_reader=NULL;
		m_hNotificationEvent=CreateEvent(NULL,FALSE,FALSE,NULL);;
		m_TotalWrittenBytes=0;
		m_bSuspend=false;
		m_PCMFileSize=~1;

		//--- �ǉ� '01/11/20  by enra ---
		m_StartOffsetPos = 0;
		m_lpReaderFactory.Add(); // Default Factory
		m_hCreateEvent = NULL;
		//-------------------------------
		//--- �ǉ� '02/03/03  by enra ---
		m_Volume = 0;
		//-------------------------------
	}
	~kmLongSound()
	{
		close();
		if(m_hNotificationEvent){
			CloseHandle(m_hNotificationEvent);
			m_hNotificationEvent=NULL;
		}
		CDirectSound::DelRef();
	}
/*	bool pause()
	{
		if( m_lpDSBuffer && IsThreadExecute() ){
			m_lpDSBuffer->Stop();
			m_bSuspend=true;
		}
		return true;
	}
*/
	bool resume()
	{
		if(m_bOpen && m_bSuspend) {
			CreateThread();
			//--- �C�� '01/11/20  by enra ---
			//Sleep(10);
			// �X���b�h�͏o���Ă�̂��[�H
			::WaitForSingleObject(m_hCreateEvent,INFINITE);
			// �C�x���g�n���h�������
			CloseHandle(m_hCreateEvent);
			m_hCreateEvent = NULL;
			//-------------------------------
			//--- �ǉ� '02/01/03  by enra ---
			m_bSuspend = false;	// ��~��Ԃ���Ȃ�
			//-------------------------------
			return true;
		}
		else 
			return false;
	}

	virtual bool open(const char *filename, bool aLoop/*=true*/)
	{
		close();
		m_bLoop=aLoop;
		m_filename = filename;
		if(inner_open()){
			m_bOpen = false;
			return false;
		}
		//play();
		return true;
	}

	//--- �C�� '01/11/19  by enra ---
	virtual bool play(){
		if(m_bOpen){
			inner_open();	// ����J�������Ă��Ȃ��ƃo�b�t�@���������Ȃ�
			m_bSuspend = false;// ��~��Ԃ���Ȃ�
			m_hCreateEvent = ::CreateEvent(NULL,false,false,NULL);
			CreateThread();
			//--- �C�� '01/11/20  by enra ---
			//Sleep(10);
			// �X���b�h�͏o���Ă�̂��[�H
			::WaitForSingleObject(m_hCreateEvent,INFINITE);
			// �C�x���g�n���h�������
			CloseHandle(m_hCreateEvent);
			m_hCreateEvent = NULL;
			//-------------------------------
			return true;
		} else {
			return false;
		}
	}
	//-------------------------------

	bool stop(bool doFadeOut=false)
	{
		if(!m_bOpen)
			return false;

		if(doFadeOut){
			fadeOut();
		}
		if(m_bSuspend && m_hNotificationEvent )
		{
			InvalidateThread();
			PulseEvent(m_hNotificationEvent);
		}

		StopThread();
		// �~�܂�܂ő҂�
		while(!ThreadSleep(100)){};

//		m_bOpen=false;		// �����Ǝv��
		if(m_aPosNotify!=NULL){
			delete [] m_aPosNotify;
			m_aPosNotify=NULL;
		}
		return true;
	}

	// �ǉ�
	virtual bool isplay(void) {
		return ((GetCurrentPos()>=0) && !m_bSuspend
				//--- �ǉ� '01/11/18  by enra ---
				&& m_lpDSBuffer!=NULL
				//-------------------------------
				);
		// ���݈ʒu�� 0 �ȏ�� pause ���łȂ������� true
	}
//	GetCurrentPos�����Ĕ��f���Ă��܂����A����̓o�b�t�@�̈�
//	�u�ŁA�S�̒ʂ��Ă̈ʒu�ɂ͂Ȃ��Ă��܂���B
//	����ł�����getCurrentPosition�ɁsKaine�t
//  ����܂萳�m����Ȃ�����g��Ȃ������g�s�����t

	LONG GetFrequency(void){// CSound �̂�R�s�y(^^;

		if(m_lpDSBuffer==NULL) return -1; // �o�b�t�@���p�ӂ���ƌĂԂȂ�����[�ɁI

		DWORD dwFreq;
		if( m_lpDSBuffer->GetFrequency( &dwFreq ) != DS_OK ) return -2;
		return( (LONG)dwFreq );
	}
	LRESULT SetFrequency( DWORD freq ){// CSound �̂�R�s�y(^^;
		if(m_lpDSBuffer==NULL) return 1; // �o�b�t�@���p�ӂ���ƌĂԂȂ�����[�ɁI
		if((DSBFREQUENCY_MIN<=freq && freq<=DSBFREQUENCY_MAX) || freq==DSBFREQUENCY_ORIGINAL)
			if( m_lpDSBuffer->SetFrequency( freq ) == DS_OK ) return 0;
		return 2;
	}

	virtual const WAVEFORMATEX* getFormat() const
	{
		if(GetReader()!=NULL){
			return GetReader()->GetFormat();
		}
		return NULL;
	}

	//--- �폜 '02/01/05  by enra ---
/*	virtual LRESULT SetCurrentPos(LONG lPos) // [ms]
	{	
		HRESULT hr;
//		pause();	// ��������Ȃ��Ǝv��
//		hr = GetReader()->setPos(lPos);
		DWORD dwPos = GetReader()->GetPosToByte(lPos);
//		Err.Out("SetPos %d,%d",lPos,dwPos);
		hr = InitSoundData(dwPos);
		resume();
		return 0;
	}*/
	//-------------------------------

/*	virtual LONG	GetCurrentPos(void){
		if( !m_lpDSBuffer )// �o�b�t�@�Ȃ�
			return -1;
	//write�̎���x��͍l���Ȃ�
		DWORD playpos,writepos;
		m_lpDSBuffer->GetCurrentPosition( &playpos, &writepos);// ���݈ʒu�擾
		writepos = helper->getWritepos();// �{���̏������݈ʒu�擾(�Ȃ̂��ȁH)

		LONG lTotalMS =	 GetReader()->GetCurrentPos();// �ǂݍ���ł�ʒu���擾

		DWORD differ_totalLen;// ���ݏ�������ł钷����ێ�
		if( writepos < playpos)// �������� < �v���C�ʒu ���Ă��Ƃ̓��[�v����
			differ_totalLen =  helper->getBufferSize() 
						+writepos - playpos;
		else if (writepos > playpos)// �������� > �v���C�ʒu ���Ă��Ƃ͒ʏ�
			differ_totalLen =  writepos- playpos;
		else // �������� == �v���C�ʒu
		{	//�����ɂ́A�ŏ�(or �v���C�O)�ɂ���B
			if( m_bSuspend )// �ꎞ��~��
				differ_totalLen = helper->getBufferSize();
			else // �Đ�����ĂȂ�
				return -1; 
		}
		LONG lDifferMS = (LONG) ((DWORDLONG)differ_totalLen * 1000 / getFormat()->nAvgBytesPerSec);// ���� ��ms �����邩�H
//		Err.Out("total %d,dif %d",lTotalMS,lDifferMS);
		return lTotalMS - lDifferMS;
	}
*/
	//--- �C�� '02/01/03  by enra ---
	virtual LONG GetCurrentPos()
	{
		if( !m_lpDSBuffer )	return m_StartOffsetPos;	//	�o�b�t�@��������ŃI�t�Z�b�g�Ԃ��Ƃ�^^;
		//write�̎���x��͍l���Ȃ�
		DWORD playpos,writepos;
		m_lpDSBuffer->GetCurrentPosition( &playpos, &writepos);
		writepos = m_lpDShelper->getWritepos();

		DWORD total=m_TotalWrittenBytes;

		DWORD differ_totalLen;
		if( writepos < playpos) {
			differ_totalLen = m_lpDShelper->getBufferSize() + writepos - playpos;
		} else if (writepos > playpos) {
			differ_totalLen =  writepos - playpos;
		} else {
			//�����ɂ́A�ŏ�(or �v���C�O)�ɂ���B
			if( m_bSuspend ) {
				differ_totalLen = m_lpDShelper->getBufferSize();
			} else {
				return m_StartOffsetPos;	// �I�t�Z�b�g�Ԃ��Ƃ�^^;
			}
		}
		// Byte�̈ʒu�𓾂�
		DWORD pos = (total - differ_totalLen
					+ GetReader()->GetPosToByte(m_StartOffsetPos)
					) % m_PCMFileSize;
		// msec�ɕϊ�
		const WAVEFORMATEX* ex = getFormat();
		if(ex!=NULL&&ex->nAvgBytesPerSec!=0) {
			return ::MulDiv(pos, 1000, ex->nAvgBytesPerSec);
		}else{
			return m_StartOffsetPos;
		}
	}
	//-------------------------------

	virtual LRESULT SetVolume(LONG volume)
	{
		//--- �ǉ� '02/03/03  by enra ---
		m_Volume = volume;
		//-------------------------------
		if (m_lpDSBuffer==NULL) return 1; // �s���ł����[

		// �͈͊O�̃G���[�`�F�b�N���[�ւ񂩂�A����ɂ���Ă�[
		return m_lpDSBuffer->SetVolume(volume);

		// DSBVOLUME_MAX�i�����Ȃ��j :		0
		// DSBVOLUME_MIN�i�����j	 : -10000
		// �̊ԂŎw�肷��̂��B
	}

	virtual LONG GetVolume(void) 
		// �擾�́A����`�����l���ɑ΂��Ă����ł��Ȃ�...
	{
		//--- �C�� '02/03/03  by enra ---
		if (m_lpDSBuffer==NULL) return m_Volume;	// �����ϐ���Ԃ�
		//-------------------------------
		LONG volume;
		if ((m_lpDSBuffer->GetVolume(&volume))!=DS_OK){
			Err.Out("CStreamSound::GetVolume�Ɏ��s");
			return 0; // volume��DSBVOLUME_MAX��Ԃ��Ƃ���?
		}
		return volume;
	}

	virtual LONG GetLength(void)
	{
		if(GetReader()!=NULL){
			return GetReader()->GetLength();
		}
		return 0;
	}

protected:

	int inner_open();
	int InitDirectSoundBuffer();
	LRESULT GetNextSoundData();
	void ThreadProc();

	int close()
	{
		stop(false);

		if(m_pDSNotify){
			m_pDSNotify->Release();
			m_pDSNotify=NULL;
		}
		if(m_lpDSBuffer){
			m_lpDSBuffer->Release();
			m_lpDSBuffer=NULL;
		}
		if(m_aPosNotify!=NULL){
			delete [] m_aPosNotify;
			m_aPosNotify=NULL;
		}
		//--- �C�� '01/11/19  by enra ---
		//delete m_reader;	m_reader=NULL;
		if(m_reader!=NULL){
			// DLL���痈��Reader�̏ꍇ�ADLL��ԓ���delete���Ȃ��Ă͂Ȃ�Ȃ�
			GetReader()->DeleteSelf();
			m_reader=NULL;
		}
		//-------------------------------
		if(m_lpDShelper!=NULL){
			delete m_lpDShelper;
			m_lpDShelper=NULL;
		}

		m_bSuspend = false;// ��~��Ԃ���Ȃ�
		m_bOpen = false;
		return 0;
	}

	bool InitSoundData(DWORD pos=0/*byte*/)
	{
		if(m_lpDSBuffer==NULL){return false;}
		if(m_lpDShelper==NULL){return false;}

		m_lpDSBuffer->SetCurrentPosition(0);
		m_prevWritepos=0;
		DWORD len=m_lpDShelper->getBufferSize()/8*7;

		m_lpDShelper->setPos(pos);

		DWORD read=m_lpDShelper->UpdateDirectSound(len);
		m_TotalWrittenBytes += read;

		//�Ȃ����_�C���N�g�T�E���h�o�b�t�@�̏I�[���܂�����
		if( len != read)
			m_TotalWrittenBytes += m_lpDShelper->UpdateDirectSound(0);
		return true;
	}
	
};

int kmLongSound::inner_open()
{
	//--- �C�� '02/01/03  by enra ---
	if (m_filename==""||m_filename.empty()) return 2;
	close();
	//-------------------------------
	//--- �C�� '01/11/19  by enra ---
	m_reader = GetFactory()->CreateReader(m_filename.c_str());
	//-------------------------------
	if ( !m_reader ){
		return 3;
	}

	GetReader()->SetLoop(m_bLoop);
	m_PCMFileSize = GetReader()->GetPCMLength();
	if(m_PCMFileSize==0){ m_PCMFileSize = ~1; }

	m_TotalWrittenBytes=0;

	m_bOpen=true;
	return 0;
}
// DirectSound�̏�����
int kmLongSound::InitDirectSoundBuffer()
{
	m_lpDShelper = IkmStreamSoundHelper::create(cds,m_reader);
	if( !m_lpDShelper )
		return 1;

	m_lpDSBuffer = m_lpDShelper->CreateDirectSoundBuffer(m_bufTime);
	if( !m_lpDSBuffer )
		return 2;

	m_lpDSBuffer->AddRef();

	//	�{�����[���̐ݒ�
	const LONG temp = m_Volume;
	m_CriticalSection.Enter();
	SetVolume(temp);
	m_CriticalSection.Leave();

	canUseNotify=false;
/*
	// Notify�ݒ� Nt4�ł�QuertInterface�����s����B
	int divideNum = m_bufTime ;
	DWORD m_dwSoundLength = m_lpDShelper->getBufferSize();

	if FAILED( m_lpDSBuffer->QueryInterface( IID_IDirectSoundNotify, 
											 (VOID**)&m_pDSNotify ) ) 
	{
		Err.Out("SLongSound::open IdirectSoundNotify QUery�Ɏ��s");
		canUseNotify=false;
		return 0;
	}

	m_aPosNotify = new DSBPOSITIONNOTIFY[ divideNum + 1 ];
	int i;
	for( i = 0; i <divideNum; i++ )
	{
		m_aPosNotify[i].dwOffset = m_dwSoundLength*(i+1)/divideNum;
		m_aPosNotify[i].hEventNotify = m_hNotificationEvent;
	}
	m_aPosNotify[i-1].dwOffset	 = m_dwSoundLength-1;

	m_aPosNotify[i].dwOffset	 = DSBPN_OFFSETSTOP;
	m_aPosNotify[i].hEventNotify = m_hNotificationEvent;
	
	if FAILED( m_pDSNotify->SetNotificationPositions( divideNum +1, 
													   m_aPosNotify ) ) 
	{
		Err.Out("SLongSound::open IdirectSoundNotify Set�Ɏ��s");
		canUseNotify=false;
		return 0;
	}
	canUseNotify=true;
*/

	return 0;
}



//�O��̌Ăяo���������ꂽ���̃o�b�t�@���[����
LRESULT kmLongSound::GetNextSoundData()
{
	DWORD playpos,writepos;
	m_lpDSBuffer->GetCurrentPosition( &playpos, &writepos);
	DWORD usedSize ;

	//���H����ĂȂ��́H
	//for cannot use notify
	if( writepos == m_prevWritepos)
	{
		//--- �C�� '01/11/19  by enra ---
		// ����[�A����Ȃ�ő҂ȁ[
		//Sleep(50);
		ThreadSleep(50);
		//-------------------------------
		return 0;
	}

	//�z�����H
	if( writepos >	m_prevWritepos)
		usedSize = writepos- m_prevWritepos;
	else
		usedSize =	m_lpDShelper->getBufferSize() +
					writepos - m_prevWritepos;
//		   wpos playpos wpos+usedSize
//	--------|-----|-------|-----------(buffer)
//			 ~~~~~~~~~~~~~usedSize

	if( playpos > m_lpDShelper->getWritepos() &&
		m_lpDShelper->getWritepos() + usedSize  > playpos )
		usedSize = playpos - m_lpDShelper->getWritepos();
//	printf("%7d %7d %7d %7d",m_prevWritepos,writepos,usedSize, 
//						abs((int)helper->getWritepos()- (int)writepos) );
//	if( helper->getWritepos() < playpos)
//		printf(" *");
//	printf("\n");

	m_prevWritepos = writepos;
	DWORD UpdateSize = usedSize;
	DWORD written=m_lpDShelper->UpdateDirectSound(UpdateSize);
	m_TotalWrittenBytes += written;
	if(written==0){
		if(!m_bLoop){
//			InvalidateThread();
			return 1;
		}
	}
	return 0;
}

//�u���[�N�|�C���g���d�|���₷���Ƃ��̗��R�ŉ��̕��ɁB
void kmLongSound::ThreadProc()
{
	if(!m_bOpen){
		::SetEvent(m_hCreateEvent);
		return;
	}

	if(!m_bSuspend) {// �ꎞ��~���łȂ������珉��������
		if( InitDirectSoundBuffer() != 0){
			m_bOpen=false;
			::SetEvent(m_hCreateEvent);
			return;
		}
		//--- �C�� '01/11/19�@by enra ---
		InitSoundData(GetReader()->GetPosToByte(m_StartOffsetPos));
		//-------------------------------
	}

	::SetEvent(m_hCreateEvent);
	if( false/*canUseNotify*/ ){
		m_lpDSBuffer->Play(0,0,DSBPLAY_LOOPING);
		while(IsThreadValid()){
			m_CriticalSection.Enter();
			GetNextSoundData();
			m_CriticalSection.Leave();
			//--- �C�� '01/11/19�@by enra ---
			// ���ꂾ�ƒ�~���Ɏ��Ԃ�H��
			//WaitForSingleObject(m_hNotificationEvent, INFINITE);
			WaitForSingleObject(m_hNotificationEvent, 100);
			//-------------------------------
		}
	}else{
		m_lpDSBuffer->Play(0,0,DSBPLAY_LOOPING);
		const LONG len = GetLength();
		int ret = 0;
		while(IsThreadValid()){
			m_CriticalSection.Enter();
			ret = GetNextSoundData();
			m_CriticalSection.Leave();
			//--- �ǉ� '02/01/07  by ENRA ---
			const int diff = len - GetCurrentPos();
			// ret==1 -> ���[�v����Ȃ�����o�b�t�@�̍Ō�܂ōĐ����Ȃ���^^;;
			if(-200<=diff&&diff<=200&&ret==1){
				for(;;){
					const int diff = len - GetCurrentPos();
					if(-100<=diff&&diff<=100){	// ���̕ӂ̐��x�Ŋ��ق��Ă�^^;
						m_lpDSBuffer->Stop();
						m_bSuspend = true;// ��~���
						return;
					}
					ThreadSleep(50);
				}
			}
			// while�̊O�ɒu���ƁAstop()���ɂ�ret==1�ƂȂ邽�߃}�Y��
			//-------------------------------
		}
	}
	m_lpDSBuffer->Stop();
	m_bSuspend = true;// ��~���
}

IkmLongSound *IkmLongSound::create()
{
	return new kmLongSound();
}

};