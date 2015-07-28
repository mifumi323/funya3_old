//	MIDI�����N���X�B

#include "stdafx.h"
#include "yaneMIDIOutput.h"
#include "yaneSoundBase.h"
#include "yaneMIDIOutputMCI.h"
#include "yaneMIDIOutputDM.h"

#pragma comment(lib,"winmm.lib")
#include <mmsystem.h>

//	0 : NULL device 1:CMIDIOutputMM 2:CMIDIOutputMCI 3:CMIDIOutputDM
int CMIDIOutput::m_nMIDIType =
#ifdef USE_MIDIOutputDM
		3;		//	DirectMusic���g����Ȃ��DirectMusic�ōĐ�
#else
 #ifdef USE_MIDIOutputMCI
		2;		//	�_���Ȃ��MCI�Đ�
 #else
//   #ifdef USE_MIDIOutputMM
//	�p�~('01/02/04)
//		1;		//	MM�Đ�
//   #else
		0;		//	no-device
//   #endif
 #endif
#endif

bool CMIDIOutput::m_bFirst=true;			//	�ŏ��̈�񂩁H
bool CMIDIOutput::m_bMIDINoDevice = false;	//	MIDI�f�o�C�X�̗L��(m_bFirst==false�ɂȂ��Ă���L��)
set<CMIDIOutput*> CMIDIOutput::m_lpaMIDIList;

//	CMIDIOutputBase�̑㗝��
CMIDIOutput::CMIDIOutput(void){
	MIDITest();
	CreateInstance(this);
	m_lpaMIDIList.insert(this);
}

CMIDIOutput::~CMIDIOutput(){
	m_lpaMIDIList.erase(this);
}

//	����Ńf�o�C�X�Z���N�g���o����悤�ɂ���B
void	CMIDIOutput::SelectDevice(int nDevice){
	if (m_bFirst) {
		m_nMIDIType = nDevice;
		return ; // �C���X�^���X�������̂�����A����łn�j
	}

	//	�f�o�C�X��������ΐ������Ȃ�
	if (m_bMIDINoDevice) {
		return ;
	}

	if (m_nMIDIType != nDevice){
		m_nMIDIType = nDevice;

		//	�S�C���X�^���X�𐶐����Ȃ����I
		//	�i�{���́A���̂Ƃ��Đ��t���O���܂ŃR�s�[���ׂ������j
		for (set<CMIDIOutput*>::iterator it = m_lpaMIDIList.begin();
			it!=m_lpaMIDIList.end();it++){
			CreateInstance(*it);	//	����ł��܂������񂩂ȁH�O�O�G
		}
	}
}

//	MIDI�f�o�C�X�̃e�X�g�R�[�h
void	CMIDIOutput::MIDITest(void){
	if (m_bFirst) {	//	MIDI�f�o�C�X�L��񂩂��H
		m_bFirst = false;

		//	�����ɃI�[�v�����Ă݂�
		HMIDIOUT hMidi;
		if (::midiOutOpen(&hMidi,MIDI_MAPPER,NULL,NULL,CALLBACK_NULL) == MMSYSERR_NOERROR){
			::midiOutClose(hMidi);
		} else {
			m_nMIDIType = 0;	//	���߂����O�O�G
			m_bMIDINoDevice = true;
		}

	}
}

void	CMIDIOutput::CreateInstance(CMIDIOutput* t){
	switch (m_nMIDIType) {
	case 0 : t->m_lpSound.Add(new CSoundNullDevice); break;
#ifdef USE_MIDIOutputMM
//	case 1 : m_lpSound.Add(new CMIDIOutputMM); break;
//	�p�~('01/02/04)
#endif
#ifdef USE_MIDIOutputMCI
	case 2 : t->m_lpSound.Add(new CMIDIOutputMCI); break;
#endif
#ifdef USE_MIDIOutputDM
	case 3 : {
		t->m_lpSound.Add(new CMIDIOutputDM);
		if (!((CMIDIOutputDM*)(CSoundBase*)t->m_lpSound)->CanUseDirectMusic())

		#ifdef USE_MIDIOutputMCI
			t->m_lpSound.Add(new CMIDIOutputMCI);		// DirectMusic���g���Ȃ����Ȃ̂�MCI�ōĐ�
			//	����͔j��I�R�s�[
		#else
			t->m_lpSound.Add(new CSoundNullDevice);	//	DirectMusic���g���Ȃ��āA
													//	����MCI�͎g��Ȃ��Ƃ̂��ƂȂ̂ōĐ����Ȃ�
			//	����͔j��I�R�s�[
		#endif

		break;
			 }
#endif
	default: WARNING(true,"CMIDIOutput::CMIDIOutput�ő��݂��Ȃ��f�o�C�X�̐���");
	}
}