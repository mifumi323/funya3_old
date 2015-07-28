#pragma once
#ifndef __IPCMStream_h__
#define __IPCMStream_h__

struct IMultiMediaStream;
namespace mtknLib {

//--- �C�� '01/11/19  by enra ---
class IkmPCMstream {
protected:
	// ����delete�֎~�IDeleteSelf()���g���ĂˁB
	virtual ~IkmPCMstream(){}

public:
	// DLL����������IkmPCMstream�̏ꍇ�́ADLL��ԓ���delete����K�v������B
	// ���̂��߂̎��E���\�b�h�B
	virtual void DeleteSelf() { delete this; }

	// �w�肳�ꂽ�t�@�C�����I�[�v�����A���̐��ۂ�Ԃ��B
	virtual bool			Open(const char *filename)=0;
	// lpDestBuf�ɁAdestSize���W�J����B
	virtual DWORD 			Read(BYTE *lpDestBuf,DWORD destSize)=0;

	// �Ȃ̒�����msec�P�ʂŕԂ��B
	virtual LONG			GetLength(void) = 0;
	// ���݂̍Đ��ʒu��Byte�ŕԂ��B
	virtual LONG			GetCurrentPos(void)=0;
	// �Đ��ʒu��ύX����B
	virtual bool			SetPos(DWORD posByte)=0;

	// ���[�v���邩�ǂ������w�肷��B
	virtual bool			SetLoop(bool loop)=0;

	// WAVEFORMATEX�\���̂�Ԃ��B
	virtual WAVEFORMATEX*	GetFormat(void)=0;
	// �t�@�C���̓W�J���Byte����Ԃ��B
	virtual DWORD			GetPCMLength(void)=0;
	// size�b�̕ێ��ɕK�v�ȓW�J���Byte����Ԃ��B
	virtual DWORD			GetBlockAlignedBufferSize(DWORD size)=0;

	// msec->Byte�̕ϊ�
	virtual DWORD			GetPosToByte(LONG lPos)=0;
	// Byte->msec�̕ϊ�
	virtual LONG			GetByteToPos(DWORD dwByte)=0;// [Byte]->[ms]
};
//-------------------------------


};

#endif