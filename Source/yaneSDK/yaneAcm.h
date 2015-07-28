// yaneAcm.h
//	Acm(Audio Compression Manager) wrapper	for DirectSound
//
//		programmed by yaneurao(M.Isozaki) '00/03/04

#ifndef __yaneACM_h__
#define __yaneACM_h__

#pragma comment(lib,"msacm32.lib")
#include <msacm.h>

class CAcm {
public:
	LRESULT			Open(WAVEFORMATEX*pWFormat,LPVOID lpSrcBuf,DWORD dwSrcLength);
	LRESULT			Open(BYTE*p,DWORD size);			//	����MP3�̃t�@�C�����I�[�v���I
	
	DWORD			GetSize(void) const;				//	����ŕϊ���T�C�Y���擾
	WAVEFORMATEX*	GetFormat(void);					//	����ŕϊ���t�H�[�}�b�g���擾
	LRESULT			Convert(LPVOID dstBuf);				//	����ŕϊ�����
	LRESULT			Close(void);						//	Close�̓f�X�g���N�^�����

	bool			IsOpen(void) const;					//	Open�����񂩁H

	CAcm(void);
	virtual ~CAcm();

protected:
	WAVEFORMATEX*	m_lpSrcWFormat;		//	�ϊ��O�̃t�H�[�}�b�g
	LPVOID			m_lpSrcBuf;			//	�ϊ��O�̃o�b�t�@�ʒu
	DWORD			m_dwSrcLength;		//	�ϊ��O�̃o�b�t�@�T�C�Y

	WAVEFORMATEX	m_destWFormat;		//	�ϊ���t�H�[�}�b�g
	HACMSTREAM		m_hAcm;				//	handle of acmStream
	ACMSTREAMHEADER	m_acmheader;		//	header of acmStream
	DWORD			m_dwDestLength;		//	�ϊ���̃o�b�t�@�T�C�Y
	
	MPEGLAYER3WAVEFORMAT m_WFormat;		//	�����mp3�̃w�b�_�[��p�ӂ���
	bool			m_bOpen;			//	�I�[�v���͐��������̂��H
};

#endif