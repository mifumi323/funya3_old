/*
	yaneSDK configulation

	�g��Ȃ����̂ɂ��Ă͂�����define���R�����g�A�E�g
	����΃R���p�C������Ȃ��B
*/

#ifndef __yaneConfig_h__
#define __yaneConfig_h__

// -=-=-=-=-=-=-=-=- �g��Ȃ��N���X�̔r�� -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// --- MIDI�o�͌n  --------------------------------------------------------

#define USE_MIDIOutputMCI	//	CMIDIOutputMCI���g����
#define USE_MIDIOutputDM	//	CMIDIOutputDM���g����
	//	��DirectMusic���g���ɂ́ADirectX6�ȍ~���K�v�Ȃ̂Œ��ӁI
	//	�������A�g���Ȃ����Ȃ�΁AUSE_MIDIOutputMCI���L���ł���΁A
	//	�����I��MCI�ōĐ�����悤�ɐ؂�ւ��

// --- �X�g���[���Đ�  ----------------------------------------------------

#define USE_StreamSound		//	CStreamSound���g����

// --- �`��	 --------------------------------------------------------------

//#define USE_DirectDraw		//	CPlane,CDirectDraw���g����
#define USE_DIB32			//	CDIB32,CDIBDraw���g����

#define USE_YGA				//	YGA���g����

#define USE_DIRECTX

// --- �������}�l�[�W�����g -----------------------------------------------

//#define USE_MEMORY_CHECK	//	new,delete���ƂɃ��O�L�^���c�����H
#define USE_YANE_NEWDELETE	//	new,delete�̃J�X�^�}�C�Y�o�[�W����(������)
							//	���USE_MEMORY_CHECK�ƕ��p�͕s��

// --- CErrorLog�o�͌n ----------------------------------------------------

//#define USE_ErrorLog		//	CErrorLog�N���X��L���ɂ���B
//	�����define���Ȃ���΁ACErrorLog�N���X�͋�̃N���X�ɂȂ�A�f�o�b�O�p�̃G���[
//	�����񓙂�VC++�̍œK���ɂ���ď��ł���B

#endif
