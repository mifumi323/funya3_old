// yaneSound.cpp
//	 re-programmed by yaneurao(M.Isozaki) '99/07/07

#include "stdafx.h"
#include "yaneSound.h"
#include "yaneFile.h"		//	Wave�ǂݍ��݂Ɏg��
#include "yaneAcm.h"		//	��W��WAVE�̓ǂݍ��݂̂��߂̎g��
#include "yaneTimer.h"

#define sign(val) (val>0?1:(val<0?-1:0))	//	������Ԃ�

	//////////////////////////////////////////////////////////////////////////

CSoundPtrArray		CSound::m_lpaSound;

	//////////////////////////////////////////////////////////////////////////

LRESULT	CSound::SetFormat(int type){				//	�v���C�}���T�E���h�o�b�t�@�̎��g���ύX
	if (GetDirectSound()==NULL) return 1;
	return GetCDirectSound()->SetFormat(type);
}

	//////////////////////////////////////////////////////////////////////////

CSound::CSound(void){
	CDirectSound::AddRef();
	//	���������s���Ă邩�H
	if (GetDirectSound() == NULL) {
		//	���߂������:p
	}

	m_lpDSBuffer = NULL;
	m_bPaused = 0;			//	�Q�ƃJ�E���g����
	m_nAvgBytesPerSec = 0;

	m_lpaSound.insert(this);	//	�`�F�C���ɃC���X�^���X�ǉ�

	// default factory
	m_lpReaderFactory.Add();

	// �f�t�H���g�Ń��[�v���Ȃ�
	m_bLoop = false;
}

CSound::~CSound(){
	Release();
	m_lpaSound.erase(this);
	CDirectSound::DelRef();
}

	//////////////////////////////////////////////////////////////////////////
//*/--- �C�� '02/01/10  by ENRA ---
LRESULT CSound::Load(string filename){
	DWORD dwDataLen = 0;
	
//	m_bLoop = false;	// �Ȃ�Ń��Z�b�g������R(`�D�L)�m '02/03/03  by ENRA

	// ���̊֐��ADirectSound�̂Ȃ��ŁA����������ԂȂ񂿂Ⴄ�H
	if (GetDirectSound() == NULL) return 1;	// ����ꂳ��I

	Release(); // �܂��́A������悤�ˁI
	m_szFileName.erase();
	if (filename.empty()) return 0;

	//	���łɓ����̃t�@�C����ǂݍ���ł���Ȃ��...Duplicate����
	for (set<CSound*>::iterator it=m_lpaSound.begin();it!=m_lpaSound.end();it++){

		if (*it!=this && (*it)->m_szFileName == filename) {
//			DWORD dwStatus;
//			(*it)->m_lpDSBuffer->GetStatus(&dwStatus);
//			if (dwStatus & DSBSTATUS_BUFFERLOST) continue;	// lost���Ă��Ŏ��I

			//	Lost���Ă��Ă�Duplicate���ׂ�?
			(*it)->Restore();	//	�������𕜋A������...

			if (GetDirectSound()->DuplicateSoundBuffer((*it)->m_lpDSBuffer,&m_lpDSBuffer)==DS_OK) {
				m_szFileName = filename;
				m_nAvgBytesPerSec = (*it)->m_nAvgBytesPerSec;	//	������R�s�[:p
				return 0;
			} else {
				return 1;
			}
		}
	}

	// Factory����ւ���(;�L�D`)
	if(m_lpReaderFactory==NULL){ m_lpReaderFactory.Add(); }
	// �I�[�v���ł������邩�[�H
	mtknLib::IkmPCMstream* pReader = m_lpReaderFactory->CreateReader(filename.c_str());
	if (pReader==NULL) {
		Err.Out("CSound::Load " + filename + "�̃t�@�C���ǂݍ��ݎ��̃G���[");
		return 1; // �t�@�C���ǂݍ��݃G���[
	}

	// �K�v�ȃo�b�t�@���𓾂�
	dwDataLen = pReader->GetPCMLength();

	// ����Ƃ�DirectSound�̏�����
	DSBUFFERDESC  dsbdDesc;
	ZeroMemory(&dsbdDesc, sizeof(DSBUFFERDESC));
	dsbdDesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY; // �{�����[�������Ō��\�I�I
	// dsbdDesc.dwFlags = DSBCAPS_LOCSOFTWARE;
	// ��ʂ̃f�[�^�������ꍇ�A�\�t�g�������ł�����ق�����肪���Ȃ��̂���
	// �i�n�[�h�E�F�A�ɂ���Ƃ������ă~�L�V���O�Ɏ��Ԃ�������\��������j
	// �����������Ȃ�\������B
	// ���ƁA�{�����[���R���g���[���\�͂��������Ă����K�v����B
	// �n�[�h�E�F�A�o�b�t�@���R���g���[���\�͂����ƁA
	// �����Ɏ��Ԃ������邩��...

	// | DSBCAPS_GETCURRENTPOSITION2;
	// ���������ƁA���t�����ǂ����̃X�e�[�^�X�������x���Ȃ�̂�
	// �X�e�[�^�X�����Ă̘A���Đ����ł��Ȃ��Ȃ�I

	// | DSBCAPS_GLOBALFOCUS
	// ����������ƁABackGround�ł������葱����

	dsbdDesc.dwBufferBytes = dwDataLen;
	dsbdDesc.lpwfxFormat = pReader->GetFormat();

	// DirectSound�o�b�t�@�̍쐬
	if (GetDirectSound()->CreateSoundBuffer(&dsbdDesc,&m_lpDSBuffer, NULL) != DS_OK){
		//	���g���ύX�̃R���g���[�����܂��������񂩁H
		dsbdDesc.dwFlags = DSBCAPS_CTRLVOLUME; // �{�����[�������Ȃ�ǂ���I�H
		if (GetDirectSound()->CreateSoundBuffer(&dsbdDesc,&m_lpDSBuffer, NULL) != DS_OK){
			Err.Out("CSound::Load��CreateSoundBuffer()�Ŏ��s�I");
			return 7;
		}
	}

	// DirectSound�o�b�t�@��Lock
	BYTE* lpDSBuffData;
	HRESULT hr;
	hr = m_lpDSBuffer->Lock(0, dwDataLen, (void**)&lpDSBuffData,
		&dwDataLen, NULL, 0, 0);
		// ����́A���́A���s���邱�Ƃ͑��X�L��̂�:p

	if (hr==DSERR_BUFFERLOST){
		m_lpDSBuffer->Restore(); // ����ŃI�b�P!�i�΁j
		hr = m_lpDSBuffer->Lock(0, dwDataLen, (void**)&lpDSBuffData,
			&dwDataLen, NULL, 0, 0);
		// ��ŁA�����������g���C����́I�I
	}

	if (hr!=DS_OK) {
		// ����Ń_���Ȃ�A�����������̂��Ⴄ�H
		Err.Out("CSound::Load��Lock()�Ɏ��s�I");
		return 8;
	}

	// �t�@�C����W�J
	pReader->SetLoop(false);
	pReader->Read(lpDSBuffData, dsbdDesc.dwBufferBytes);
	// DLL����o�����Ȃ̂�smart_ptr�͎g�����Ⴞ�߂��߁B
	pReader->DeleteSelf();  pReader=NULL;

	// DirectSound�o�b�t�@��Unlock...
	if (m_lpDSBuffer->Unlock(lpDSBuffData, dwDataLen, NULL, 0)
		!= DS_OK) {
		// ����Ȃ�ӂ[�A���s���邩��...�ǂȂ����[����[�񂶂�
		Err.Out("CSound::Load��Unlock()�Ɏ��s�I");
		return 10;
	}

	//	�b�Ԃ̍Đ��b��
	WAVEFORMATEX wfe;
	if( (m_lpDSBuffer->GetFormat( &wfe, sizeof(wfe), NULL )) == DS_OK &&
		(wfe.nAvgBytesPerSec)){
		m_nAvgBytesPerSec = wfe.nAvgBytesPerSec;
	} else {
		//	���������Ȃ��B����G���[�ɂȂ�񂩂��H
		m_nAvgBytesPerSec = 0;
	}

	m_szFileName = filename;
	m_lLength = LONG(((DWORDLONG)dwDataLen * 1000) / m_nAvgBytesPerSec);

	return 0;
}
//-------------------------------*/
/*--- �폜 '02/01/10  by ENRA ---
LRESULT CSound::Load(string filename){
	DWORD dwDataLen = 0;
	LPVOID pWaveData = NULL;
	WAVEFORMATEX  *pWFormat=NULL;
	
	m_bLoop = false;

	// ���̊֐��ADirectSound�̂Ȃ��ŁA����������ԂȂ񂿂Ⴄ�H
	if (GetDirectSound() == NULL) return 1;	// ����ꂳ��I

	Release(); // �܂��́A������悤�ˁI
	m_szFileName.erase();
	if (filename.empty()) return 0;

	//	���łɓ����̃t�@�C����ǂݍ���ł���Ȃ��...Duplicate����
	for (set<CSound*>::iterator it=m_lpaSound.begin();it!=m_lpaSound.end();it++){

		if (*it!=this && (*it)->m_szFileName == filename) {
//			DWORD dwStatus;
//			(*it)->m_lpDSBuffer->GetStatus(&dwStatus);
//			if (dwStatus & DSBSTATUS_BUFFERLOST) continue;	// lost���Ă��Ŏ��I

			//	Lost���Ă��Ă�Duplicate���ׂ�?
			(*it)->Restore();	//	�������𕜋A������...

			if (GetDirectSound()->DuplicateSoundBuffer((*it)->m_lpDSBuffer,&m_lpDSBuffer)==DS_OK) {
				m_szFileName = filename;
				m_nAvgBytesPerSec = (*it)->m_nAvgBytesPerSec;	//	������R�s�[:p
				return 0;
			} else {
				return 1;
			}
		}
	}

	CFile file;
	if (file.Read(filename)) {
		Err.Out("CSound::Load " + filename + "�̃t�@�C���ǂݍ��ݎ��̃G���[");
		return 1; // �t�@�C���ǂݍ��݃G���[
	}
	
	// WAVEFORMATEX�\���̂𓾂�
	// ���̌`����wave�f�[�^���l�����Ȃ��ẮI�i��ς����j

	// �f�[�^�́A��������Af.fileadr����f.filesize����������B
	MMIOINFO mmio; // �������t�@�C����mmioOpen����̂��I�I(C)yaneurao
	ZeroMemory(&mmio,sizeof(mmio));
	mmio.pchBuffer = (LPSTR)file.GetMemory();	// �����Ƀt�@�C���o�b�t�@adr
	mmio.fccIOProc = FOURCC_MEM;				// ����������I
	mmio.cchBuffer = file.GetSize();			// �����Ƀt�@�C���T�C�Y
	// mmio.adwInfo	  = 0;			// �o�b�t�@�͑����Ȃ��Ă����񂾂��Ă΁I

	CAcm	acm;		//	acm�ϊ��̏���
	// ����ƃI�[�v���ł���B�i��͒������[�j
	HMMIO hmmio = mmioOpen(NULL,&mmio,MMIO_READ);
	if (hmmio==NULL) {
		Err.Out("CSound::Load : mmioOpen�̎��s");
		return 2;
	}

	MMCKINFO ciPC,ciSC; // �e�`�����N�ƃT�u�`�����N
	ciPC.fccType = mmioFOURCC('W','A','V','E');
	if (mmioDescend(hmmio,(LPMMCKINFO)&ciPC,NULL,MMIO_FINDRIFF)){
		mmioClose(hmmio,0);
		//	�Ђ���Ƃ���mp3���H
		if (acm.Open((BYTE*)file.GetMemory(),file.GetSize())==0) goto OPEN_MP3;
		Err.Out("CSound::Load : Wave�t�@�C���łȂ�");
		return 3;
	}
	ciSC.ckid = mmioFOURCC('f','m','t',' ');
	if (mmioDescend(hmmio,&ciSC,&ciPC,MMIO_FINDCHUNK)){
		Err.Out("CSound::Load : fmt�`�����N���w��ł��Ȃ�");
		mmioClose(hmmio,0);
		return 4;
	}

	//	�����ŁAif (pWFormat->wFormatTag !=WAVE_FORMAT_PCM){
	//	�Ƃ���āA�W��WAVE�ȊO�̃t�@�C�����͂������Ƃ͏o���邪�A
	//	�W��WAVE�t�@�C���łȂ��ꍇ�́ACODEC���Ăяo���悤�ɂ��Ă݂�B

	// ��������ɂ���̂�����ARead����K�v�͂Ȃ��B
	// ���̃|�C���^����������Ηǂ��B
	// ���݈ʒu���瑊�΃V�[�N��0�ړ������
	// GetCurrentPosition�I�Ȃ��Ƃ��\�ƂȂ�
	// �����ɐ擪�A�h���X�����Z����΂n�j�B

	pWFormat = (WAVEFORMATEX*)
		((BYTE*)file.GetMemory() + mmioSeek(hmmio,0,SEEK_CUR));
	//	(C) yaneurao

	mmioAscend(hmmio,&ciSC,0); // fmt�T�u�`�����N�O���ֈړ�
	ciSC.ckid = mmioFOURCC('d','a','t','a');
	if (mmioDescend(hmmio,&ciSC,&ciPC,MMIO_FINDCHUNK)){
		Err.Out("CSound::Load : �f�[�^�`�����N�ɍs���Ȃ�");
		mmioClose(hmmio,0);
		return 5;
	}

	// �������Ɠ�����@�Ńf�[�^�ւ̃|�C���^�𓾂�
	dwDataLen = ciSC.cksize; // �f�[�^�T�C�Y
	pWaveData = (LPVOID)
		((BYTE*)file.GetMemory() + mmioSeek(hmmio,0,SEEK_CUR));
	// (C) yaneurao
	mmioClose(hmmio,0);
	// Close���Ă���������GlobalAlloc�ŌŒ胁�����Ɋ��蓖�Ă��Ă���
	// �̂Ŗ��Ȃ��B

	// ����ŁApWFormat,pWaveData,dwDataLen���l���ł����I

OPEN_MP3:;	//	MP3�𒼐ڃI�[�v�������Ƃ��͂����ɔ��:p

	// ����Ƃ�DirectSound�̏�����
	DSBUFFERDESC  dsbdDesc;
	ZeroMemory(&dsbdDesc, sizeof(DSBUFFERDESC));
	dsbdDesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY; // �{�����[�������Ō��\�I�I
	// dsbdDesc.dwFlags = DSBCAPS_LOCSOFTWARE;
	// ��ʂ̃f�[�^�������ꍇ�A�\�t�g�������ł�����ق�����肪���Ȃ��̂���
	// �i�n�[�h�E�F�A�ɂ���Ƃ������ă~�L�V���O�Ɏ��Ԃ�������\��������j
	// �����������Ȃ�\������B
	// ���ƁA�{�����[���R���g���[���\�͂��������Ă����K�v����B
	// �n�[�h�E�F�A�o�b�t�@���R���g���[���\�͂����ƁA
	// �����Ɏ��Ԃ������邩��...

	// | DSBCAPS_GETCURRENTPOSITION2;
	// ���������ƁA���t�����ǂ����̃X�e�[�^�X�������x���Ȃ�̂�
	// �X�e�[�^�X�����Ă̘A���Đ����ł��Ȃ��Ȃ�I

	// | DSBCAPS_GLOBALFOCUS
	// ����������ƁABackGround�ł������葱����

	//	�W��WAVE�t�@�C���ł͂Ȃ���CODEC���Ăтɍs��
	if (acm.IsOpen()) {	//	����MP3�t�@�C�����I�[�v�����Ă���
		dwDataLen	= acm.GetSize();	//	acm����ϊ���̃T�C�Y�𓾂�
		pWFormat	= acm.GetFormat();	//	acm����ϊ���̃t�H�[�}�b�g�𓾂�
	} else
	if (pWFormat->wFormatTag !=WAVE_FORMAT_PCM){
		if (acm.Open(pWFormat,pWaveData,dwDataLen)!=0){
			Err.Out("CSound::��Ή���WAVE FORMAT");	
			return 6;
		};
		dwDataLen	= acm.GetSize();	//	acm����ϊ���̃T�C�Y�𓾂�
		pWFormat	= acm.GetFormat();	//	acm����ϊ���̃t�H�[�}�b�g�𓾂�
	}

	dsbdDesc.dwBufferBytes = dwDataLen; // ����ˁI
	dsbdDesc.lpwfxFormat = pWFormat;	// ����ˁI

	// DirectSound�o�b�t�@�̍쐬
	if (GetDirectSound()->CreateSoundBuffer(&dsbdDesc,&m_lpDSBuffer, NULL) != DS_OK){
		//	���g���ύX�̃R���g���[�����܂��������񂩁H
		dsbdDesc.dwFlags = DSBCAPS_CTRLVOLUME; // �{�����[�������Ȃ�ǂ���I�H
		if (GetDirectSound()->CreateSoundBuffer(&dsbdDesc,&m_lpDSBuffer, NULL) != DS_OK){
			Err.Out("CSound::Load��CreateSoundBuffer()�Ŏ��s�I");
			return 7;
		}
	}

	// DirectSound�o�b�t�@��Lock
	BYTE* lpDSBuffData;
	LRESULT hr;
	hr = m_lpDSBuffer->Lock(0, dwDataLen, (void**)&lpDSBuffData,
		&dwDataLen, NULL, 0, 0);
		// ����́A���́A���s���邱�Ƃ͑��X�L��̂�:p

	if (hr==DSERR_BUFFERLOST){
		m_lpDSBuffer->Restore(); // ����ŃI�b�P!�i�΁j
		hr = m_lpDSBuffer->Lock(0, dwDataLen, (void**)&lpDSBuffData,
			&dwDataLen, NULL, 0, 0);
		// ��ŁA�����������g���C����́I�I
	}

	if (hr!=DS_OK) {
		// ����Ń_���Ȃ�A�����������̂��Ⴄ�H
		Err.Out("CSound::Load��Lock()�Ɏ��s�I");
		return 8;
	}

	if (acm.IsOpen()) {
		//	acm���g���Ȃ�΁Aacm��Lock�����o�b�t�@�|�C���^��n����
		//	���ڂ����ɕϊ����Ă��炤�B�i�Ȃ�ł݂�Ȃ������[�ւ�́H�j
		if (acm.Convert(lpDSBuffData)!=0) {
			Err.Out("CSound::Acm�ł̕ϊ��Ɏ��s");
			return 9;
		};
	} else {
		// WaveData��DirectSound�o�b�t�@�ɓ]��
		CopyMemory(lpDSBuffData,(LPVOID)pWaveData,dwDataLen);
	}

	// DirectSound�o�b�t�@��Unlock...
	if (m_lpDSBuffer->Unlock(lpDSBuffData, dwDataLen, NULL, 0)
		!= DS_OK) {
		// ����Ȃ�ӂ[�A���s���邩��...�ǂȂ����[����[�񂶂�
		Err.Out("CSound::Load��Unlock()�Ɏ��s�I");
		return 10;
	}

	//	�b�Ԃ̍Đ��b��
	WAVEFORMATEX wfe;
	if( (m_lpDSBuffer->GetFormat( &wfe, sizeof(wfe), NULL )) == DS_OK &&
		(wfe.nAvgBytesPerSec)){
		m_nAvgBytesPerSec = wfe.nAvgBytesPerSec;
	} else {
		//	���������Ȃ��B����G���[�ɂȂ�񂩂��H
		m_nAvgBytesPerSec = 0;
	}

	m_szFileName = filename;
	m_lLength = LONG(((DWORDLONG)dwDataLen * 1000) / m_nAvgBytesPerSec);

	return 0;
}
//-------------------------------*/

LRESULT CSound::Release(void){
	if (m_lpDSBuffer!= NULL) {
		if (IsPlay()) Stop();
		// �Đ����Ȃ�Ƃ߂�Ƃ������I �i�ꉞ�ˁj
		RELEASE_SAFE(m_lpDSBuffer);
		m_szFileName.erase();
	}
	return 0;
}

LRESULT CSound::Restore(void){
	if (!m_szFileName.empty()) {// ������ēǂݍ���łȂ��񂿂Ⴄ��H
		DWORD dwStatus;
		if (m_lpDSBuffer==NULL) return 1;
		m_lpDSBuffer->GetStatus(&dwStatus);
		if (!(dwStatus & DSBSTATUS_BUFFERLOST)) return 0; // Lost���ĂȂ����H
	}

	RELEASE_SAFE(m_lpDSBuffer); // ���A������Ă܂��ق����Ă��Ƃ葁���I

	string string;
	string = m_szFileName; // �R�s�[���ēn�����Release���ꂿ�Ⴄ:p
	return Load(string);
}


LRESULT CSound::Play(void) {

	m_bPaused = 0;
	if (m_lpDSBuffer==NULL) return 1; // �o�b�t�@���p�ӂ���ƌĂԂȂ�����[�ɁI
	
	// �Đ����Ȃ�A�~�߂Ăł��Đ����Ȃ����B
	DWORD dwStatus;
	// �{���Ȃ�A���t���Ȃ�~�߂��ɃJ�����g�|�W�V���������ύX���Ă�����
	// �ǂ��͂��Ȃ̂����A���̏u�Ԃ�PLAY���I������Ƃ����\��������B
	// �i�Ȃ񂩁ADirectSound�̃o�O�̂悤�ȋC�����Ȃ����Ȃ��j

	// �����ŁA�J�����g�|�W�V������擪�ɖ߂��āA�v���C���~�܂���
	// ����΁A�Đ��Ƃ����A���S���Y���ɕύX����B
	
	m_lpDSBuffer->SetCurrentPosition(0);
	// ���ɖ߂��Ă���̂ف[��Play���萳�m��� (C) yaneurao
	m_lpDSBuffer->GetStatus(&dwStatus);
	if (dwStatus & DSBSTATUS_PLAYING) {		
		m_lpDSBuffer->Stop(); // �ˑR�~�߂��班���m�C�Y���邯�ǂȁB�d���Ȃ���ȁB
		m_lpDSBuffer->SetCurrentPosition(0);
	}

	if (m_bLoop) {	//	���[�v�Đ��ɑΉ�����̂�:p
		if (m_lpDSBuffer->Play(0,0,DSBPLAY_LOOPING)==DSERR_BUFFERLOST){
		// ���炢��������B�o�b�t�@LOST�������Brestore����i���I
			return Restore();
		}
	} else {
		if (m_lpDSBuffer->Play(0,0,0)==DSERR_BUFFERLOST){
		// ���炢��������B�o�b�t�@LOST�������Brestore����i���I
			return Restore();
		}
	}
	return 0;
}

	//////////////////////////////////////////////////////////////////////////

LRESULT CSound::Stop(void)
{
	if (!IsPlay()) return 0; // �����~�܂��Ă������[�ɁI

	// �����ɔ����Ă���������[���Ƃ́Am_lpDSBuffer!=NULL
	//--- �ǉ� '02/02/27  by enra ---
	// �����Ɏ~�߂�ƃm�C�Y���o��̂Ń{�����[�����׍H����
	const LONG now = GetVolume();
	const LONG step = (now/*<=0*/-DSBVOLUME_MIN/*=-10000*/)/25;
	CTimer vTimer;
	vTimer.Reset();
	for(int i=0; i<25; i=vTimer.Get()){
		// ������ƌ덷�o�邯�ǋC�ɂ��Ȃ��R(�L���M)�m
		SetVolume(now - step*i);
	}
	// �Ō�̎d�グ
	SetVolume(DSBVOLUME_MIN);
	// ��Ŏ~�߂�
	LRESULT ret = m_lpDSBuffer->Stop();
	// �{�����[�������ɖ߂�
	SetVolume(now);
	//-------------------------------
	m_lpDSBuffer->SetCurrentPosition(0); // ���ǂ��Ƃ��ȁI

	return 0;
}

bool CSound::IsPlay(void)
// ���̃`�����l�����Đ����Ȃ�true
{
	if (m_lpDSBuffer==NULL) return false; // �o�b�t�@���p�ӂ���ƌĂԂȂ�����[�ɁI
	
	DWORD dwStatus;
	m_lpDSBuffer->GetStatus(&dwStatus);
	return dwStatus & DSBSTATUS_PLAYING;
}

LRESULT CSound::Pause(void)
 // ����`�����l����pause
{
	m_bPaused += sign(m_bPaused);	//	�K�E�Z:p
	if (!IsPlay()) {
		return 0; // �����I����Ă������[�ɁI
	}
	//	�Đ����̂�pause������Ȃ��
	m_bPaused = 1;

	// �����ɔ����Ă���������[���Ƃ́Am_lpDSBuffer!=NULL
	return m_lpDSBuffer->Stop();
}

LRESULT CSound::Replay(void)
// pause�Ŏ~�߂Ă����A���̃`�����l���̍Đ����ĊJ�B
{
	if (!m_bPaused) return 0; // pause���Ăւ�āI
	
	if (--m_bPaused==0) {	//	�Q�ƃJ�E���g����
		if (m_bLoop) {	//	���[�v�Đ��ɑΉ�����̂�:p
			return m_lpDSBuffer->Play(0,0,DSBPLAY_LOOPING);
		} else {
			return m_lpDSBuffer->Play(0,0,0);
		}
	}
	return 0;
}

LRESULT CSound::SetVolume(LONG volume)
{
	if (m_lpDSBuffer==NULL) return 1; // �s���ł����[

	// �͈͊O�̃G���[�`�F�b�N���[�ւ񂩂�A����ɂ���Ă�[
	return m_lpDSBuffer->SetVolume(volume);

	// DSBVOLUME_MAX�i�����Ȃ��j :		0
	// DSBVOLUME_MIN�i�����j	 : -10000
	// �̊ԂŎw�肷��̂��B
}

LONG CSound::GetVolume(void) 
// �擾�́A����`�����l���ɑ΂��Ă����ł��Ȃ�...
{
	if (m_lpDSBuffer==NULL) return 1; // �s���ł����[
	LONG volume;
	if ((m_lpDSBuffer->GetVolume(&volume))!=DS_OK){
		Err.Out("CSound::GetVolume�Ɏ��s");
		return 0; // volume��DSBVOLUME_MAX��Ԃ��Ƃ���?
	}
	return volume;
}

LONG CSound::GetPosition(void) {

	if (m_lpDSBuffer==NULL ) return -1;
	if (m_nAvgBytesPerSec==0) return -1;	//	Load�̏��擾�Ŏ��s���Ă���
	
	DWORD pos, dm;
	if( (m_lpDSBuffer->GetCurrentPosition( &pos, &dm )) != DS_OK ) return -1;
	return LONG(((DWORDLONG) pos*1000 ) / m_nAvgBytesPerSec);
	//	DWORDLONG�ɃL���X�g���Ȃ��ƈ�ꂿ�Ⴄ:p
}

LRESULT CSound::SetCurrentPos(LONG lPos){
	if (m_lpDSBuffer==NULL ) return -1;
	if (m_nAvgBytesPerSec==0) return -1;	//	Load�̏��擾�Ŏ��s���Ă���

	DWORD pos;
	pos = lPos * m_nAvgBytesPerSec / 1000;
//	Err.Out("pos %d",pos);
	return m_lpDSBuffer->SetCurrentPosition(pos);
}

//	bmd����̒��
LONG CSound::GetFrequency(void){

	if(m_lpDSBuffer==NULL) return -1; // �o�b�t�@���p�ӂ���ƌĂԂȂ�����[�ɁI

	DWORD dwFreq;
	if( m_lpDSBuffer->GetFrequency( &dwFreq ) != DS_OK ) return -2;
	return( (LONG)dwFreq );
}

LRESULT CSound::SetFrequency( DWORD freq )
{
	if(m_lpDSBuffer==NULL) return 1; // �o�b�t�@���p�ӂ���ƌĂԂȂ�����[�ɁI
	if((DSBFREQUENCY_MIN<=freq && freq<=DSBFREQUENCY_MAX) || freq==DSBFREQUENCY_ORIGINAL)
		if( m_lpDSBuffer->SetFrequency( freq ) == DS_OK ) return 0;
	return 2;
}

//////////////////////////////////////////////////////////////////////////////
//	quest������

LONG CSound::GetPos(void) {	//	��Έʒu�����̂܂ܕԂ�

	if (m_lpDSBuffer==NULL ) return -1;
	if (m_nAvgBytesPerSec==0) return -1;	//	LoadWaveFile�̏��擾�Ŏ��s���Ă���
	
	DWORD pos, dm;
	if( (m_lpDSBuffer->GetCurrentPosition( &pos, &dm )) != DS_OK ) return -1;
	return pos;
}

/*
LRESULT CSound::SetPos( DWORD pos )
{
	if(m_lpDSBuffer==NULL) return 1; 
	if( m_lpDSBuffer->SetCurrentPosition( pos ) != DS_OK ) return 2;
	return 0;
}
*/

LRESULT CSound::Play( DWORD pos )
{
	m_bPaused = 0;
	if (m_lpDSBuffer==NULL) return 1; // �o�b�t�@���p�ӂ���ƌĂԂȂ�����[�ɁI
	
	// �Đ����Ȃ�A�~�߂Ăł��Đ����Ȃ����B
	DWORD dwStatus;
	m_lpDSBuffer->SetCurrentPosition(pos);
	// ���ɖ߂��Ă���̂ف[��Play���萳�m��� (C) yaneurao
	m_lpDSBuffer->GetStatus(&dwStatus);
	if (dwStatus & DSBSTATUS_PLAYING) {		
		m_lpDSBuffer->Stop(); // �ˑR�~�߂��班���m�C�Y���邯�ǂȁB�d���Ȃ���ȁB
		m_lpDSBuffer->SetCurrentPosition(pos);
	}

	if (m_bLoop) {	//	���[�v�Đ��ɑΉ�����̂�:p
		if (m_lpDSBuffer->Play(0,0,DSBPLAY_LOOPING)==DSERR_BUFFERLOST){
		// ���炢��������B�o�b�t�@LOST�������Brestore����i���I
			return Restore();
		}
	} else {
		if (m_lpDSBuffer->Play(0,0,0)==DSERR_BUFFERLOST){
		// ���炢��������B�o�b�t�@LOST�������Brestore����i���I
			return Restore();
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//	�S�C���X�^���X�ɑ΂��鑀��
//////////////////////////////////////////////////////////////////////////////

LRESULT CSound::ReleaseAll(void){
	if (m_lpaSound.empty()) return 0;
	
	for (set<CSound*>::iterator it=m_lpaSound.begin();it!=m_lpaSound.end();it++){
		(*it)->Release();
	}
	return 0;
}

LRESULT	CSound::RestoreAll(void){
	for (set<CSound*>::iterator it=m_lpaSound.begin();it!=m_lpaSound.end();it++){
		(*it)->Restore();
	}
	return 0;
}

LRESULT	CSound::SetVolumeAll(long volume){
	for (set<CSound*>::iterator it=m_lpaSound.begin();it!=m_lpaSound.end();it++){
		(*it)->SetVolume(volume);
	}
	return 0;
}

void	CSound::StopAll(void){
	for (set<CSound*>::iterator it=m_lpaSound.begin();it!=m_lpaSound.end();it++){
		(*it)->Stop();
	}
}

void	CSound::PauseAll(void){
	for (set<CSound*>::iterator it=m_lpaSound.begin();it!=m_lpaSound.end();it++){
		(*it)->Pause();
	}
}

void	CSound::ReplayAll(void){
	for (set<CSound*>::iterator it=m_lpaSound.begin();it!=m_lpaSound.end();it++){
		(*it)->Replay();
	}
}

LRESULT	CSound::SetLoopMode(bool bLoop){
	m_bLoop = bLoop;
	return 0;
}