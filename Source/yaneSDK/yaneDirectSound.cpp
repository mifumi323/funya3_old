// DirectSound Wrapper

	//�@�T�E���h�h���C�o�[�𕡐��T�|�[�g����K�v������̂Ȃ�΁A
	//	�T�E���h����݂̊֐��́A���ׂĊ֐��|�C���^�Ƃ��Ĉ����ׂ�
	//	�����A�K���ɂ��āADirectSound�����邩�Ȃ��������Ȃ̂ŁA
	//	�����܂ł��Ȃ��B

#include "stdafx.h"
#include "yaneDirectSound.h"
#include "yaneSound.h"
#include "yaneCOMBase.h"		//	COM�̗��p�̂���
#include "yaneAppInitializer.h"

//////////////////////////////////////////////////////////////////////////////

int		CDirectSound::m_nRef = 0;
auto_ptrEx<CDirectSound> CDirectSound::m_lpCDirectSound;

//////////////////////////////////////////////////////////////////////////////

CDirectSound::CDirectSound(void){
	m_lpDirectSound = NULL;
	m_lpPrimary		= NULL;

	if (CCOMBase::AddRef()) { // COM�g���Ł[
		m_bSuccessInit = false;
	} else {
		m_bSuccessInit = true;
	}
	m_nFormat = 7;	// default

	if (Initialize()!=0) return ; 

	CAppInitializer::Hook(this);				//	�t�b�N���J�n����
}

CDirectSound::~CDirectSound(){
	Terminate(); // �s���Ȏ�҂̂��߂Ɉꉞ�A�I������:p
	CCOMBase::Release(); // COM�g���I������Ł[
	CAppInitializer::Unhook(this);				//	�t�b�N����������B
}

//////////////////////////////////////////////////////////////////////////////
LRESULT CDirectSound::Initialize(void){
	Terminate(); // �O�̂��߂ɏ��������Ă�ɌĂ�ł��

	m_hWnd = CAppInitializer::GetHWnd();

	if (!m_bSuccessInit) {
		Err.Out("CDirectSound::CoInitialize�Ɏ��s");
		return 1;	// ���b�ɂȂ�܂����...
	}

	//	���������ADirectX3��5��DirectSound�͂��܂�ς���ĂȂ��B	
	// Create the DirectSound object
	if (CoCreateInstance(CLSID_DirectSound,
		NULL,CLSCTX_ALL,IID_IDirectSound, (LPVOID*)&m_lpDirectSound)!=DS_OK) {
		Err.Out("CDirectSound object�̍쐬�Ɏ��s");
		// SoundCARD�����݂��Ȃ��ꍇ�́A���̃G���[�ˁI
		return 2;
	}

	// ���̑�P�p�����[�^�̓T�E���h�J�[�h��GUID
	if (m_lpDirectSound->Initialize(NULL)!=DS_OK){
		Err.Out("CDirectSound::Initialize�Ɏ��s");
		return 3;
	}

retry:;
	if (m_nFormat==7) {
		// Set the cooperation level for the DirectSound object
		if (m_lpDirectSound->SetCooperativeLevel(m_hWnd,DSSCL_NORMAL) != DS_OK) {
			Err.Out("CDirectSound�̏������G���[�BSetCooperativeLevel�̎��s");
			Terminate();
			return 4;
		}
	} else {
		// Set the cooperation level for the DirectSound object
		if (m_lpDirectSound->SetCooperativeLevel(m_hWnd,DSSCL_PRIORITY) != DS_OK) {
			Err.Out("CDirectSound�̏������G���[�BSetCooperativeLevel�ŗD�拦�����x���ɕύX���s");
			// recover����
			m_nFormat=7; goto retry;
		} else {
		//	����ł���ƁA�v���C�}���T�E���h�o�b�t�@�̎��g���ύX�������ꂽ
			DSBUFFERDESC dsbdesc;
			ZERO(dsbdesc);
			dsbdesc.dwSize = sizeof(dsbdesc);
			dsbdesc.dwFlags = /* DSBCAPS_CTRLVOLUME | */ DSBCAPS_PRIMARYBUFFER; // �v���C�}���o�b�t�@�擾
			dsbdesc.dwBufferBytes = 0;
			dsbdesc.lpwfxFormat = NULL;
			if (m_lpDirectSound->CreateSoundBuffer(&dsbdesc,&m_lpPrimary,NULL)!=DS_OK){
				// recover����
				Err.Out("CDirectSound�̏������G���[�BCreateSoundBuffer�Ńv���C�}���o�b�t�@�����Ȃ�����");
				m_nFormat=7; goto retry;
			}
			WAVEFORMATEX pcmwf;
			ZERO(pcmwf);
			pcmwf.wFormatTag		= WAVE_FORMAT_PCM;
			pcmwf.nChannels			= (m_nFormat & 1) + 1;					// 1 or 2 channel
			pcmwf.wBitsPerSample	= (((m_nFormat & 2) >> 1) + 1) * 8;		// 8 or 16 bits
			pcmwf.nSamplesPerSec	= (((m_nFormat & (4+8)) >> 2) + 1) * 11025; // 11KHz or 22KHz or 44KHz
			pcmwf.nBlockAlign	= 4;
			pcmwf.nAvgBytesPerSec= pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
			m_lpPrimary->SetFormat(&pcmwf);
		}
	}

	return 0; // ����I��
}

LRESULT CDirectSound::Terminate(void) {

	CSound::ReleaseAll();

	RELEASE_SAFE(m_lpPrimary);
	if (m_lpDirectSound!=NULL) {
		m_lpDirectSound->SetCooperativeLevel(m_hWnd,DSSCL_NORMAL);	// �O�̂��߁A���Ƃ̋������x���ɖ߂�
	}
	RELEASE_SAFE(m_lpDirectSound);

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT CDirectSound::CheckSoundLost(void){	// ���X�g���Ă���C������
	if (m_lpDirectSound==NULL) return 0;
	if (CSound::RestoreAll()) return 1;
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

LRESULT	CDirectSound::SetFormat(int nType){	//	�v���C�}���T�E���h�o�b�t�@�̎��g���ύX
	if (nType!=m_nFormat) {
		m_nFormat = nType;
		return Initialize();				//	�ēx�A�v���C�}���o�b�t�@������Ȃ���...
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

//	�T�E���h�̃��X�g�`�F�b�N�ƁA���J�o�[�����͎��O�ōs�Ȃ��B
LRESULT CDirectSound::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg){
	case WM_ACTIVATEAPP: {
		UINT bActive = wParam;
		if(bActive) {
			CheckSoundLost();		//	�T�E���h�̃��X�g�`�F�b�N
		}
		return 0;
						 }
	}
	return 0;
}