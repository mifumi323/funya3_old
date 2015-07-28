#include "stdafx.h"
#include "yaneScene.h"

void	CScene::ReturnScene(){
	GetSceneControl()->ReturnScene();
}

void	CScene::ExitScene(){
	GetSceneControl()->ExitScene();
}

//	�V�[���Ԃł̃p�����[�^�𓊂������O�O�G
int*	CScene::GetSceneParam(void){
	return GetSceneControl()->GetSceneParam();
}

/////////////////////////////////////////////////////////////////////////////

CSceneControl::CSceneControl(smart_ptr<CSceneFactory> pvSceneFactory){
	m_nNextScene		= -1;
	m_nMessage			= 0;
	m_pvSceneFactory	= pvSceneFactory;
	//	�O�̂��߃N���A���Ă���
	ZERO(m_anSceneParam);
}

void	CSceneControl::CreateScene(int nScene){
	if (nScene==-1) return ; // ���ꎸ�s�Ȃ�
	//	�V�����V�[�����\�z
//	m_vSceneFrame.m_lpScene.Delete();
	m_vSceneFrame.m_lpScene.Add(m_pvSceneFactory->CreateScene(nScene));
	m_vSceneFrame.m_lpScene->SetSceneControl(this);
	m_vSceneFrame.m_lpScene->OnInit();	//	����ŏ������B
	//	���݂̃V�[���́A���̃V�[���ɂȂ�B
	m_vSceneFrame.m_nScene = nScene;
}

LRESULT CSceneControl::OnDraw(CPlaneBase*lpDraw) {// �ǉ�
	// �C�� '02/03/01  by ENRA
	// ��{�I��OnSimpleMove�̃R�s�y�@OnSimpleMove�̑����OnDraw���Ă�
  do {
	//	OnInit�̂Ȃ��Ŏ��X�ƃV�[���W�����v������̂Ő�Ƀ��b�Z�[�W���N���A
	int nMessage = m_nMessage;
	m_nMessage = 0;

	//	���b�Z�[�W��dispatch
	switch (nMessage) {
		//	0:No Message 1:JumpScene 2:CallScene
		//	3:CallSceneFast 4:ReturnScene 5:ExitScene
	case 0: break;
	case 1:

		//	���݂̃V�[����j��
		m_vSceneFrame.m_lpScene.Delete();
		//	�V�����V�[�����\�z
		CreateScene(m_nNextScene);
		break;
	case 2: {
		//	���݂̃V�[����j��
		m_vSceneFrame.m_lpScene.Delete();
		//	���݂̃V�[�����X�^�b�N��ɐς�ŁADelete����B
		CSceneFrame	sf;
		sf.m_nScene = m_vSceneFrame.m_nScene;
		m_SceneFrameStack.push(sf);
		//	�V�����V�[�����\�z
		CreateScene(m_nNextScene);
		break;
			}
	case 3:
		//	���݂̃V�[�����X�^�b�N��ɐς�ŁADelete���Ȃ��B
		//	�˃X�}�[�g�|�C���^�ɂȂ��Ă���̂�push���Ă�����Delete����Ȃ��B
		m_SceneFrameStack.push(m_vSceneFrame);
		//	�V�����V�[�����\�z
		CreateScene(m_nNextScene);
		break;
	case 4:	{		//	���̃V�[���ɖ߂�B
		//	���݂̃V�[����j��
		m_vSceneFrame.m_lpScene.Delete();
		int nScene = m_vSceneFrame.m_nScene;
		//	�ЂƂO�̃V�[����pop
		//	WARNING(m_SceneFrameStack.size()==0,"PushScene/CallScene���Ă��Ȃ��̂�ReturnScene���悤�Ƃ���");
		//	���ꂪ�����̂Ȃ�΁A�����ł��܂ւ�O�O�G
		if (m_SceneFrameStack.size()==0) break;	//	�����߂�V�[�������˂�

		m_vSceneFrame = m_SceneFrameStack.top();
		m_SceneFrameStack.pop();
		if (m_vSceneFrame.m_lpScene == NULL) {
			//	�V�[��������Ă��邩��Afactory�ŁA�܂��������ɂ�I
			CreateScene(m_vSceneFrame.m_nScene);
			//	�ꉞ�A�Ăяo�����O�O�G
			if (m_vSceneFrame.m_lpScene!=NULL) {
				m_vSceneFrame.m_lpScene->OnComeBack(nScene);
			}
		} else {
			//	�A���Ă����Ł`�@�W�F�[���J���o�[�N�b�O�O�G
			m_vSceneFrame.m_lpScene->OnComeBack(nScene);
		}
		break;
			}
	case 5:	return 1;	//	�`�悹���ɔ�����
	}

	if (m_vSceneFrame.m_lpScene!=NULL) {
		m_vSceneFrame.m_lpScene->OnDraw(lpDraw);
	}

  } while (m_nMessage!=0);
	//	��L��OnDraw�̂Ȃ��ł���Ɏ��̃V�[���ɔ�Ԃ��Ƃ�����B
	//	���b�Z�[�W�͖����Ȃ�܂ŏ�����������̂�����

  return 0;
}

LRESULT CSceneControl::OnSimpleDraw(CPlaneBase*lpDraw) {// �ǉ�
	if (m_vSceneFrame.m_lpScene!=NULL) {// �����������񂾂�(^^;
		m_vSceneFrame.m_lpScene->OnSimpleDraw(lpDraw);
	}
	return 0;
}

LRESULT		CSceneControl::OnSimpleMove(CPlaneBase* lpPlane){
  do {
	//	OnInit�̂Ȃ��Ŏ��X�ƃV�[���W�����v������̂Ő�Ƀ��b�Z�[�W���N���A
	int nMessage = m_nMessage;
	m_nMessage = 0;

	//	���b�Z�[�W��dispatch
	switch (nMessage) {
		//	0:No Message 1:JumpScene 2:CallScene
		//	3:CallSceneFast 4:ReturnScene 5:ExitScene
	case 0: break;
	case 1:

		//	���݂̃V�[����j��
		m_vSceneFrame.m_lpScene.Delete();
		//	�V�����V�[�����\�z
		CreateScene(m_nNextScene);
		break;
	case 2: {
		//	���݂̃V�[����j��
		m_vSceneFrame.m_lpScene.Delete();
		//	���݂̃V�[�����X�^�b�N��ɐς�ŁADelete����B
		CSceneFrame	sf;
		sf.m_nScene = m_vSceneFrame.m_nScene;
		m_SceneFrameStack.push(sf);
		//	�V�����V�[�����\�z
		CreateScene(m_nNextScene);
		break;
			}
	case 3:
		//	���݂̃V�[�����X�^�b�N��ɐς�ŁADelete���Ȃ��B
		//	�˃X�}�[�g�|�C���^�ɂȂ��Ă���̂�push���Ă�����Delete����Ȃ��B
		m_SceneFrameStack.push(m_vSceneFrame);
		//	�V�����V�[�����\�z
		CreateScene(m_nNextScene);
		break;
	case 4:	{		//	���̃V�[���ɖ߂�B
		//	���݂̃V�[����j��
		m_vSceneFrame.m_lpScene.Delete();
		int nScene = m_vSceneFrame.m_nScene;
		//	�ЂƂO�̃V�[����pop
		//	WARNING(m_SceneFrameStack.size()==0,"PushScene/CallScene���Ă��Ȃ��̂�ReturnScene���悤�Ƃ���");
		//	���ꂪ�����̂Ȃ�΁A�����ł��܂ւ�O�O�G
		if (m_SceneFrameStack.size()==0) break;	//	�����߂�V�[�������˂�

		m_vSceneFrame = m_SceneFrameStack.top();
		m_SceneFrameStack.pop();
		if (m_vSceneFrame.m_lpScene == NULL) {
			//	�V�[��������Ă��邩��Afactory�ŁA�܂��������ɂ�I
			CreateScene(m_vSceneFrame.m_nScene);
			//	�ꉞ�A�Ăяo�����O�O�G
			if (m_vSceneFrame.m_lpScene!=NULL) {
				m_vSceneFrame.m_lpScene->OnComeBack(nScene);
			}
		} else {
			//	�A���Ă����Ł`�@�W�F�[���J���o�[�N�b�O�O�G
			m_vSceneFrame.m_lpScene->OnComeBack(nScene);
		}
		break;
			}
	case 5:	return 1;	//	�`�悹���ɔ�����
	}

	if (m_vSceneFrame.m_lpScene!=NULL) {
		m_vSceneFrame.m_lpScene->OnSimpleMove(lpPlane);
	}

  } while (m_nMessage!=0);
	//	��L��OnDraw�̂Ȃ��ł���Ɏ��̃V�[���ɔ�Ԃ��Ƃ�����B
	//	���b�Z�[�W�͖����Ȃ�܂ŏ�����������̂�����

  return 0;
}

void	CSceneControl::PushScene(int nScene){
	//	�w�肳�ꂽ�V�[�������X�^�b�N��ɐςށB
	CSceneFrame	sf;
	sf.m_nScene = nScene;
	m_SceneFrameStack.push(sf);
}

bool	CSceneControl::IsEnd(void){
	return m_vSceneFrame.m_lpScene == NULL;
}

bool	CSceneControl::IsPrecedentScene(void){
	//	�V�[���������ȁH
	CScene* lpScene = m_vSceneFrame.m_lpScene;
	if (lpScene == NULL) return false;

	return lpScene->IsPrecedentScene();
}