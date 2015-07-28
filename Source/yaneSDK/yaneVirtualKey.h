//	yaneVirtualKey.h :
//		Virtual Key Class for CKeyBase
//				programmed by yaneurao(M.Isozaki) '99/11/13

#ifndef __yaneVirtualKey_h__
#define __yaneVirtualKey_h__

#include "yaneKeyBase.h"

const int KEY_DEVICE_MAX= 16;	//	�f�o�C�X�̓o�^�\�ő吔
const int VIRTUAL_KEY_MAX=88;	//	���z�L�[�̍ő�i88��:p�j

class CVKeyBase {
public:
	CKeyBase*	m_lpKeyBase;
	int			m_key;
};
//	set��insert,erase�ɂ͔�r���Z�q�̒�`���K�v�i�_�T...�j
//	bool	operator <(const CVKeyBase& x,const CVKeyBase& y);

typedef auto_vector_ptr<CVKeyBase> CVKeyList;

class CVirtualKey {
public:
	CVirtualKey(void);
	virtual ~CVirtualKey();

	//	�L�[�f�o�C�X�̓o�^
	void	ClearKeyDevice(void);					//	�f�o�C�X�̃N���A
	void	AddDevice(CKeyBase*);					//	�f�o�C�X�̒ǉ�
	void	RemoveDevice(CKeyBase*);				//	�f�o�C�X�̍폜
	CKeyBase* GetDevice(int n);						//	���Ԗڂɓo�^�����f�o�C�X�̎擾
	void	Input(void);							//�@�o�^���Ă������f�o�C�X����ǂ݂���

	//	���z�L�[�̒ǉ��E�폜
	void	ClearKeyList(void);						//	���z�L�[�̃��Z�b�g
	void	AddKey(int vkey,CKeyBase*,int key);		//	���z�L�[�̒ǉ�
	void	RemoveKey(int vkey,CKeyBase*,int key);	//	���z�L�[�̍폜

	bool	IsVKeyPress(int vkey);					//	���z�L�[�𒲂ׂ�
	bool	IsVKeyPush(int vkey);					//	���z�L�[�𒲂ׂ�

	void	ResetKey(void);							//	����̃L�[���͂̃N���A

protected:
	int			m_nDeviceMax;						//	�o�^�f�o�C�X��
	CKeyBase*	m_lpDevice[KEY_DEVICE_MAX];			//	���̓L�[�f�o�C�X���X�g
	CVKeyList	m_VKey[VIRTUAL_KEY_MAX];			//	���z�L�[���X�g
	bool		m_bResetKey;						//	�L�[���Z�b�g���O�O�G�i�����Input�܂Łj
};

#endif