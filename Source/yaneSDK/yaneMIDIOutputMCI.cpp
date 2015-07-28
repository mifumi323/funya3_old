#include "stdafx.h"

#ifdef USE_MIDIOutputMCI

#include "yaneMIDIOutputMCI.h"
#include "yaneFile.h"
#include "yaneAppInitializer.h"

#define sign(val) (val>0?1:(val<0?-1:0))	//	������Ԃ� * yane

////////////////
//	�R���X�g���N�^�E�f�X�g���N�^
////////////////
CMIDIOutputMCI::CMIDIOutputMCI(void)
{
	m_wDeviceID = -1;	//	�{�������word�Ȃ̂ŁA(DWORD)-1�͂��肦�Ȃ�
	m_bPaused = 0;
	m_bLoopPlay = false;
	m_bNowPlay = false;
	m_lLength = 0;
	CAppInitializer::Hook(this);		//	�t�b�N���J�n����
}

CMIDIOutputMCI::~CMIDIOutputMCI()
{
	Close();
	CAppInitializer::Unhook(this);		//	�t�b�N����������B
}

////////////////
//	�I�[�v��
///////////////
LRESULT CMIDIOutputMCI::Open(LPCTSTR pName)
{
	Close();

	//	���̃t�@�C����ǂݍ���
	if(m_File.Read((LPSTR)pName)!=0)	{
		Err.Out("CMIDIOutputMCI::Open�œǂݍ��ރt�@�C��������"+string(pName));
		return 1;
	}

	if (m_File.CreateTemporary()!=0)	{
		Err.Out("CMIDIOutputMCI::Open��CreateTemporary�Ɏ��s");
		return 2;
	}

	return 0;
}

LRESULT	CMIDIOutputMCI::InnerOpen(void){	//	MCI���I�[�v������
	if (m_wDeviceID!=-1) return 0;	//	open�ς�

	//	�o�̓t�@�C���łl�h�c�h�f�o�C�X�I�[�v��
	DWORD	dwOpenFlags = MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT;
	MCI_OPEN_PARMS open;
	ZERO(open);
	open.lpstrDeviceType = (LPTSTR)MCI_DEVTYPE_SEQUENCER;
//	open.lpstrElementName = (LPTSTR)(CFile::MakeFullName(m_File.GetName()).c_str());
	//	������ł̓e���|�����I�u�W�F�N�g���ł�����O�O�G
	string s;
	s = CFile::MakeFullName(m_File.GetName());
	open.lpstrElementName = (LPTSTR)(s.c_str());

	if (::mciSendCommand(0, MCI_OPEN, dwOpenFlags, (DWORD)&open)) {
		Err.Out("CMIDIOutput::MIDI�f�o�C�X���I�[�v���ł��Ȃ��B");
		return 1;
	}

	m_wDeviceID = open.wDeviceID;

	// �P�ʂ�ms�ɂ��� '01/04/09 add kaine
	MCI_SET_PARMS set;
	set.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	mciSendCommand(m_wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID)&set);


	return 0;
}

//////////
//	�N���[�Y
//////////
LRESULT CMIDIOutputMCI::Close(void)
{
	m_File.Close();	//	delete temporary file
	if (m_wDeviceID == -1) return 0; // not opened
	if (m_bNowPlay) Stop();	// stop���Ȃ���Close�ł��Ȃ���[��

	if (::mciSendCommand(m_wDeviceID, MCI_CLOSE, 0, (DWORD)NULL)) {
		Err.Out("CMIDIOutput::MIDI�f�o�C�X���N���[�Y�ł��Ȃ��B");
		return 1;
	}

	m_wDeviceID = -1;
	m_dwPosition = 0;
	return 0;
}

//////////
//	���t����
/////////

LRESULT CMIDIOutputMCI::Play(void)
{
	m_bPaused = 0;
	if (InnerOpen()!=0) return 1;

	Stop();
	//	�Đ��J�n
	MCI_PLAY_PARMS play;
	play.dwFrom = 0;
	play.dwCallback = (DWORD)CAppInitializer::GetHWnd();
	if (mciSendCommand(m_wDeviceID, MCI_PLAY, MCI_FROM|MCI_NOTIFY,(DWORD)&play)) {
		return 2;
	}
	m_bNowPlay = true;
	return 0;
}

LRESULT CMIDIOutputMCI::Replay(void)
{
	if (m_bPaused==0) return 0;		//	pause���Ăւ�āI
	if (--m_bPaused!=0) return 0;	//	�Q�ƃJ�E���g����

	if (m_wDeviceID == -1) return 1;	//	�I�[�v�����ĂȂ���I
	if (InnerOpen()!=0) return 1;

	//	�Ȃ����Đ����Ȃ̂ŉ��������ɋA��
	if (IsPlay()) return 0;

	//	�Đ��ĊJ
	MCI_PLAY_PARMS play;
	play.dwFrom = m_dwPosition;
	play.dwCallback = (DWORD) (DWORD)CAppInitializer::GetHWnd();
	if (::mciSendCommand(m_wDeviceID, MCI_PLAY, MCI_FROM|MCI_NOTIFY,(DWORD)&play)) {
		return 2;
	}
	m_bPaused = 0;
	m_bNowPlay = true;
	return 0;
}

LRESULT CMIDIOutputMCI::Stop(void)
{
	m_bPaused += sign(m_bPaused);	//	�K�E�Z:p
	if (m_wDeviceID == -1) return 1;	//	not open
	if (!IsPlay()) return 0;			//	���łɒ�~���Ă���

	//	�Đ����̂�pause�����Ȃ��
	m_bPaused = 1;

	//	���ݏ�ԓǂݏo���Ē�~
	MCI_STATUS_PARMS status;
	ZERO(status);
	status.dwItem = MCI_STATUS_POSITION;
	if (::mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&status)) {
		Err.Out("CMIDIOutput::MIDI�f�o�C�X�X�e�[�^�X���ǂݏo���Ȃ��B");
		m_dwPosition = 0;
		if (::mciSendCommand(m_wDeviceID, MCI_STOP, 0, (DWORD)NULL)) {
			Err.Out("CMIDIOutput::MIDI�f�o�C�X����~�ł��Ȃ��B");
		}
		return 2;
	}
	m_dwPosition = status.dwReturn;
	if (::mciSendCommand(m_wDeviceID, MCI_STOP, 0, (DWORD)NULL)) {
		Err.Out("CMIDIOutput::MIDI�f�o�C�X����~�ł��Ȃ��B");
		return 2;
	}
	m_bNowPlay = false;
	return 0;
}


LONG	CMIDIOutputMCI::GetCurrentPos(void){
	if (m_wDeviceID == -1) return -1;

	// ���݂̍Đ��|�W�V�����𓾂�
	MCI_STATUS_PARMS status;
	status.dwItem = MCI_STATUS_POSITION;
	if (mciSendCommand(m_wDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&status)){
		Err.Out("CMIDIOutputMCI::GetCurrentPos��MCI_STATUS(MCI_STATUS_POSITION)�Ɏ��s");
		return -1;	// �G���[
	}
//	return status.dwReturn * 100; // .1�b�P�ʂŎ擾�ł���
	return status.dwReturn; // [ms]�P�ʂŎ擾�ł���
}

LRESULT CMIDIOutputMCI::SetCurrentPos(LONG lPos){	// [ms]
	if ( m_wDeviceID == -1) return -1;

	MCI_SEEK_PARMS seek;
	seek.dwTo = lPos;
	seek.dwCallback = (DWORD) (DWORD)CAppInitializer::GetHWnd();
	if ( mciSendCommand(m_wDeviceID,MCI_SEEK, MCI_TO|MCI_WAIT , (DWORD)&seek)){
		Err.Out("Err SetCurrentPos");
		return -1;
	}

	if ( m_bNowPlay ){
		MCI_PLAY_PARMS play;
		play.dwCallback = (DWORD)CAppInitializer::GetHWnd();
		if (mciSendCommand(m_wDeviceID, MCI_PLAY, MCI_NOTIFY,(DWORD)&play)) {
			return 2;
		}
	}

	return 0;
}

LRESULT CMIDIOutputMCI::SetLoopMode(bool bLoop){
	m_bLoopPlay = bLoop;
	return 0;
}

bool CMIDIOutputMCI::IsPlay(void){
	return m_bNowPlay;
}

LRESULT	CMIDIOutputMCI::LoopPlay(void){
	if (m_bLoopPlay) {
		return Play();
	} else {
		return Stop();
	}
}

// add '01/04/09 kaine
LONG	CMIDIOutputMCI::GetLength(void){
	if (m_wDeviceID == -1) return -1;

	// �����𓾂�
	MCI_STATUS_PARMS status;
	status.dwItem = MCI_STATUS_LENGTH;
	if (mciSendCommand(m_wDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&status)){
		Err.Out("CMIDIOutputMCI::GetCurrentPos��MCI_STATUS(MCI_STATUS_LENGTH)�Ɏ��s");
		return -1;	// �G���[
	}
	return status.dwReturn; // [ms]�P�ʂŎ擾�ł���
}


//////////////////////////////////////////////////////////////////////////////

// WM_MCINOTIFY�̒ʒm���󂯎��K�v����
LRESULT CMIDIOutputMCI::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
	switch(uMsg){
	case MM_MCINOTIFY : {
		// ���̃��b�Z�[�W��CD�̂񂩁H
		if (lParam!=(LPARAM)m_wDeviceID) return 0; // �Ⴄ���I

		// Stop�Œ�~�������Ƃ��́AMCI_NOTIFY_ABORTED
		if (wParam==MCI_NOTIFY_SUCCESSFUL){
			LoopPlay();
			return 1;
		}
	}

	} // end switch

	return 0;
}

#endif