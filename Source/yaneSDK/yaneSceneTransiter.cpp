#include "stdafx.h"
#include "yaneSceneTransiter.h"
#include "yanePlaneEffectBlt.h"

CSceneTransiter::CSceneTransiter(){
	m_bTransition	 = false;
	m_bSnapNextScene = false;
	m_bStop			 = false;
	m_nTransCount.Set(0,256,16);	//	�g�����W�V�����̃f�B�t�H���g�X�s�[�h��16
}

void	CSceneTransiter::Backup(CPlaneBase*lp,int n){
	int sx,sy;
	lp->GetSize(sx,sy);
	m_vPlane[n].Delete();	//	�O�̂͂�������ˁ[�O�O
	//	�T�[�t�F�[�X�̐���
	m_vPlane[n].Add(CPlaneBase::CreatePlane());
	//	��������CreatePlane���g���̂ŁA�C�����ĂˁO�O
	m_vPlane[n]->CreateSurface(sx,sy);
	m_vPlane[n]->BltFast(lp,0,0);
}

void	CSceneTransiter::BeginTransit(int nTransType,int nSpeed
		,int nStartPhase,int nEndPhase){
	m_bTransition	 = true;
	m_bSnapNextScene = true;	//	���̃V�[���̓X�i�b�v����i�X�i�b�v�\��j
	m_nTransType  = nTransType;
	m_nTransCount.Set(nStartPhase,nEndPhase,nSpeed);
	m_nTransCount.Reset();
}

LRESULT CSceneTransiter::OnSimpleMove(CPlaneBase* lpPlane){

	if (m_vSceneControl->IsPrecedentScene()){
		//	�g�����W�V�������D�悷��V�[�����H(�I�����܂����_�C�A���O�Ȃ�)
		return m_vSceneControl->OnSimpleMove(lpPlane);
	}

	if (!m_bTransition || m_bStop) {
		//	�g�����W�V�������łȂ��Ȃ�ʏ�`��
		LRESULT lr = m_vSceneControl->OnSimpleMove(lpPlane);
		if (lr!=0) return lr;
		if (!m_bTransition) return 0;
		//	�g�����W�V������~���Ȃ̂ŁA���̂܂܋A��O�O�G
		if (m_bStop) return 0;

		//	�X�i�b�v�\�񂪓����Ă��邩�H
		if (m_bSnapNextScene){
			int sx,sy;
			lpPlane->GetSize(sx,sy);
			//	�T�[�t�F�[�X�̐���
			m_vPlane[2].Add(CPlaneBase::CreatePlane());
			m_vPlane[2]->CreateSurface(sx,sy);
			LRESULT lr;
			lr	= m_vSceneControl->OnSimpleDraw((CPlaneBase*)m_vPlane[2]); // �����ɕ`�悵�Ă��炨���O�O�G
			if (lr!=0) return lr; // �����Ȃ�G���[�R�[�h�Ԃ��Ȃ�[�O�O
			m_bSnapNextScene = false;
		}
	}

	//	�g�����W�V�����J�E���^�̉��Z
	m_nTransCount.IncS();
	if (m_nTransCount.IsLapAround()){
//		m_bTransition = false;	//	�g�����W�V�����I���`�O�O
	}
	return 0;
}

LRESULT	CSceneTransiter::OnSimpleDraw(CPlaneBase* lpPlane){

	if (m_vSceneControl->IsPrecedentScene()){
		//	�g�����W�V�������D�悷��V�[�����H(�I�����܂����_�C�A���O�Ȃ�)
		return m_vSceneControl->OnSimpleDraw(lpPlane);
	}

	if (!m_bTransition || m_bStop) {
		//	�g�����W�V�������łȂ��Ȃ�ʏ�`��
		LRESULT lr = m_vSceneControl->OnSimpleDraw(lpPlane);
		if (lr!=0) return lr;
		if (!m_bTransition) return 0;

		//	�g�����W�V������~���Ȃ̂ŁA���̂܂܋A��O�O�G
		if (m_bStop) return 0;

		// �g�����W�V�����t�F�[�Y�ɓ�����

		//	----	��˂��炨����
		//	OnSimpleMove�Ńg�����W�V���������o���A�o�b�N�A�b�v�v���[����
		//	�`�悷�邱�Ƃ��ۏ؂����̂Ȃ�΂��̏����͕s�v�Ȃ̂����A
		//	�Ԉ����OnSimpleDraw���Ńg�����W�V�������s�Ȃ���\��������̂�
		//	�ꉞ�A���̃`�F�b�N���s�Ȃ��Ă������Ƃɂ���B

		//	�X�i�b�v�\�񂪓����Ă��邩�H
		if (m_bSnapNextScene){
			int sx,sy;
			lpPlane->GetSize(sx,sy);
			//	�T�[�t�F�[�X�̐���
			m_vPlane[2].Add(CPlaneBase::CreatePlane());
			m_vPlane[2]->CreateSurface(sx,sy);
			LRESULT lr;
			lr	= m_vSceneControl->OnSimpleDraw((CPlaneBase*)m_vPlane[2]); // �����ɕ`�悵�Ă��炨���O�O�G
			if (lr!=0) return lr; // �����Ȃ�G���[�R�[�h�Ԃ��Ȃ�[�O�O
			m_bSnapNextScene = false;
		}
	}

	//	�g�����W�V�������s�Ȃ��̂�
	lpPlane->BltFast(m_vPlane[1],0,0);
	CPlaneTransBlt::Blt(m_nTransType,lpPlane,m_vPlane[2],0,0,m_nTransCount,0);
	//	�g�����W�V�����J�E���^�̉��Z
//	m_nTransCount.IncS();
	if (m_nTransCount.IsLapAround()){
		m_bTransition = false;	//	�g�����W�V�����I���`�O�O
	}
	return 0;
}

//	������́A�]���̃v���O�����Ƃ̌݊����̂��߁A
//	OnSimpleDraw,OnSimpleMove�ɈϏ�����̂ł͂Ȃ��A
//	�V�[���N���X��OnDraw�𒼐ڌĂяo���ɍs��
LRESULT	CSceneTransiter::OnDraw(CPlaneBase* lpPlane){
	if (m_vSceneControl->IsPrecedentScene()){
		//	�g�����W�V�������D�悷��V�[�����H(�I�����܂����_�C�A���O�Ȃ�)
		return m_vSceneControl->OnDraw(lpPlane);
	}

	if (!m_bTransition || m_bStop) {
		//	�g�����W�V�������łȂ��Ȃ�ʏ�`��
		LRESULT lr = m_vSceneControl->OnDraw(lpPlane);
		if (lr!=0) return lr;
		if (!m_bTransition) return 0;
		//	�g�����W�V������~���Ȃ̂ŁA���̂܂܋A��O�O�G
		if (m_bStop) return 0;
		
		// �g�����W�V�����t�F�[�Y�ɓ�����

		//	�X�i�b�v�\�񂪓����Ă��邩�H
		if (m_bSnapNextScene){
			int sx,sy;
			lpPlane->GetSize(sx,sy);
			//	�T�[�t�F�[�X�̐���
			m_vPlane[2].Add(CPlaneBase::CreatePlane());
			m_vPlane[2]->CreateSurface(sx,sy);
			LRESULT lr;
			lr	= m_vSceneControl->OnDraw((CPlaneBase*)m_vPlane[2]); // �����ɕ`�悵�Ă��炨���O�O�G
			if (lr!=0) return lr; // �����Ȃ�G���[�R�[�h�Ԃ��Ȃ�[�O�O
			m_bSnapNextScene = false;
		}
	}

	//	�g�����W�V�������s�Ȃ��̂�
	lpPlane->BltFast(m_vPlane[1],0,0);
	CPlaneTransBlt::Blt(m_nTransType,lpPlane,m_vPlane[2],0,0,m_nTransCount,0);
	//	�g�����W�V�����J�E���^�̉��Z
	m_nTransCount.IncS();
	if (m_nTransCount.IsLapAround()){
		m_bTransition = false;	//	�g�����W�V�����I���`�O�O
	}
	return 0;
}