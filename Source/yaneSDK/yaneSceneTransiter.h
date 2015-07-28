//	CSceneTransiter

#ifndef __yaneSceneTransiter_h__
#define __yaneSceneTransiter_h__

#include "yanePlaneBase.h"
#include "yaneScene.h"
#include "yaneRootCounter.h"

//	�V�[���Ԃ̃g�����W�V�������~���ɍs�Ȃ����߂̃v���[���̒����ɁO�O�G
//	���̃V�[���Ƀv���[����n�������Ƃ��ȂǂɎg���ƕ֗�
//	�g�����W�V�����@�\���ځO�O
class CSceneTransiter {
public:
	//	���̃v���[����Plane�Ƀo�b�N�A�b�v����
	void	Backup(CPlaneBase*lp,int nPlaneNo=1);

	//	�V�[���R���g���[���[��ݒ�
	void	SetSceneControl(smart_ptr<CSceneControl> v) { m_vSceneControl=v;}

	//	�v���[���̐ݒ�^�擾
	void	SetPlane(smart_ptr<CPlaneBase> v,int nPlaneNo) { m_vPlane[nPlaneNo] = v; }
	smart_ptr<CPlaneBase> GetPlane(int nPlaneNo) { return m_vPlane[nPlaneNo]; }
	//	�o�b�N�A�b�v���Ƃ�����́��Ŏ��o����B
	//	�O�̃V�[����Backup�Ńo�b�N�A�b�v�����v���[���́A
	//	�����smart_ptr�Ɉڂ��Ďg���Ɨǂ��B

	//	�����̏��L�������j��
	void	ReleasePlane(int nPlaneNo) { m_vPlane[nPlaneNo].Delete(); }

	//	�g�����W�V�����̐ݒ�
	void	BeginTransit(int nTransType,int nSpeed=16,int nStartPhase=0,int nEndPhase=256);
	//	�g�����W�V���������H
	bool	IsEndTransit(void) const { return m_bTransition; }
	/*
		�g�����W�V�����̕��@
			�P�D���݂̃V�[����Backup(GetDraw());�Ńv���[���P�ɃR�s�[���āA
			�Q�DBeginTransit�ŃR�s�[�J�n�B
			�R�D���̃V�[����JumpScene����΁A
				�ŏ���OnDraw�̌��ʂ��v���[���Q�ɃR�s�[����Ď����I��
				�g�����W�V�������J�n�����B
	*/

	//	�g�����W�V������}�~����
	void	StopTransit() { m_bStop = true; }
	void	RestartTransit() { m_bStop = false; }

	LRESULT OnDraw(CPlaneBase* lp);
	//	�����ӂ��ɕ����������
	LRESULT OnSimpleDraw(CPlaneBase* lp);
	LRESULT OnSimpleMove(CPlaneBase* lp);

	//	---- 
	CSceneTransiter();
	virtual ~CSceneTransiter(){}

protected:
	smart_ptr<CPlaneBase>	m_vPlane[4];
	//	�ێ����Ă���v���[��
	//	0:���̃V�[���ɓn�����߂̈ꎞ�I�ȃv���[��
	//	1:�g�����W�V�����̂��߂̃v���[���P�i�O�V�[���j
	//	2:�g�����W�V�����̂��߂̃v���[���Q�i���V�[���j
	//	3:�\���̃v���[���i�I���V�[�����̂��߁j

	bool			m_bTransition;		//	�g�����W�V���������H
	bool			m_bSnapNextScene;	//	�g�����W�V�����̂��߂Ɏ��̃V�[�����X�i�b�v����
	CRootCounter	m_nTransCount;		//	�g�����W�V�����J�E���^
	int				m_nTransType;		//	�g�����W�V�����̃^�C�v
	smart_ptr<CSceneControl> m_vSceneControl; // �g�����W�V�����p�̃V�[���R���g���[��
	bool			m_bStop;			//	�g�����W�V�����̗}�~�����H
};

#endif