//	yaneScene.h :
//
//		�V�[���Ǘ��N���X
//		�ˁ@��˂��炨�z�[���y�[�W�u�V�˃Q�[���v���O���}�{���M�v�X�v��
//			��W�C�X�C�P�P�C�P�R�͂��Q�l�̂��ƁB

#ifndef __yaneScene_h__
#define __yaneScene_h__

#include "yanePlaneBase.h"

class CSceneControl;

/*	�V�[����`��
enum SCENE {
	SCENE1,			//	�V�[���P
	SCENE2,			//	�V�[���Q
};
*/

class CScene {
public:
	friend class CSceneControl;

	// ------ �V�[���̈ړ��֘A�B�����OnDraw�^�C�~���O�Ő؂�ւ��B
	//	�i����OnDraw�ł́A���̃V�[���̕`��͂���Ȃ��j

	//	���ɍs���V�[����ݒ肷��
	template <class T>	void	JumpScene(T eScene){ GetSceneControl()->JumpScene(eScene); }

	//	�V�[�����R�[������iReturnScene�ŁA���̃N���X�ɖ߂��Ă���j
	//		�������A���̃N���X�͂�������delete�����B
	template <class T>	void	CallScene(T eScene){ GetSceneControl()->CallScene(eScene); }

	//	�V�[�����R�[������iReturnScene�ŁA���̃N���X�ɖ߂��Ă���j
	//		�������A���̃N���X��delete���ꂸ�Ɏc��B
	//		���̃V�[���Ɉڍs����Ƃ��ɁAOnCallSceneFast���Ăяo�����B
	template <class T>	void	CallSceneFast(T eScene){ GetSceneControl()->CallSceneFast(eScene); }
	//	CallScene/CallSceneFast�ŌĂ΂ꂽ�V�[���ɕ��A����
	void	ReturnScene();

	//	�V�[�����I������
	void	ExitScene();

	// --------------------------------------------------------------

	//	�V�[�����X�^�b�N�ɐςށB
	//	(ReturnScene�����Ƃ��ɁA�����ɐς܂ꂽ���i�X�^�b�N�Ȃ̂ŋt���j��
	//	�V�[�����Ăяo�����)
	template <class T>	void	PushScene(T eScene){ GetSceneControl()->PushScene(eScene); }

	//	PopScene���ł�����ʔ��������m��Ȃ����ǁA
	//	����ł͐������V�[���Ǘ��Ƃ͌����Ȃ��C������B(���Jump�֎~�̘_��)

	// --------------------------------------------------------------

	//	�V�[���̕`����s�Ȃ�
	virtual void	OnDraw(CPlaneBase *lp) {
		OnSimpleDraw(lp);
		OnSimpleMove(lp);
	}

	//	�����Q�̃t�F�[�Y�ɕ�����

	virtual void	OnSimpleDraw(CPlaneBase *lp) {};
	//	�I�u�W�F�N�g�̈ړ����s��
	virtual void	OnSimpleMove(CPlaneBase* lp) {}// �ǉ�

	//	�������́A�ȉ��̊֐��ōs�Ȃ��B�i�R���X�g���N�^�ł�outer���g���Ȃ����߁j
	virtual void	OnInit() {}

	//	CallScene/CallSceneFast�ŌĂяo���AReturnScene�Ŗ߂��Ă����Ƃ��ɌĂяo�����
	//	nScene�ɂ́A�ǂ̃V�[������߂��Ă�����������B
	virtual void	OnComeBack(int nScene){}

	// --------------------------------------------------------------

	//	���܎��s���Ă���V�[���́A�g�����W�V�������D�悷��V�[�����H
	virtual bool IsPrecedentScene(void) { return m_bPrecedentScene; }

	// --------------------------------------------------------------

	//	�V�[���Ԃł̃p�����[�^�𓊂������O�O�G
	virtual int*	GetSceneParam(void);

	// --------------------------------------------------------------

	CScene() { m_bPrecedentScene = false; }
	virtual ~CScene(){} // place holder

protected:
	//	�V�[���R���g���[���[�̐ݒ�^�擾
	void	SetSceneControl(CSceneControl* lp) { m_lpSceneControl = lp; }
	CSceneControl* GetSceneControl(){ return m_lpSceneControl; }

	//	���܎��s���Ă���V�[���́A�g�����W�V�������D�悷��V�[�����H
	//	�iWM_CLOSE�ɑ΂��鏈�����s�Ȃ��V�[���Ȃǂ́A�����On�ɂ���Ɨǂ��j
	void	SetPrecedentScene(bool b) { m_bPrecedentScene = b; }

private:
	CSceneControl* m_lpSceneControl;
	bool		   m_bPrecedentScene;		//	�g�����W�V�������D�悷��V�[�����H
};

//	�V�[���̍\�z�́A���̃N���X��h�������āA�����ɓo�^����B
class CSceneFactory {
public:
	//	enum�𖳗����int�ɃL���X�g���邽�߂Ƀ����o�֐��e���v���[�g��p����O�O�G
	template <class T>
			CScene* CreateScene(T eScene) { return CreateScene((int)eScene); }
	virtual CScene* CreateScene(int nScene) = 0;
	//	�V�[���𐶐�����(template�֐��͉��z���ł��Ȃ��̂ŁA���������݌v�ɂȂ�)
};

class CSceneControl {
public:
	CSceneControl(smart_ptr<CSceneFactory> pvSceneFactory=NULL);

	//	---	�֐��̈Ӗ���CScene�Ɠ�����
	//	enum�𖳗����int�ɃL���X�g���邽�߂Ƀ����o�֐��e���v���[�g��p����O�O�G
	template <class T>
			void	JumpScene(T eScene){ JumpScene( (int)eScene ); }
	virtual void	JumpScene(int nScene){ m_nNextScene = nScene; m_nMessage=1; }

	template <class T>
			void	CallScene(T eScene){ CallScene( (int)eScene ); }
	virtual void	CallScene(int nScene){ m_nNextScene = nScene; m_nMessage=2; }

	template <class T>
			void	CallSceneFast(T eScene){ CallSceneFast( (int)eScene ); }
	virtual void	CallSceneFast(int nScene){ m_nNextScene = nScene; m_nMessage=3; }

	virtual void	ReturnScene(void){ m_nMessage=4; }
	virtual void	ExitScene(void){ m_nMessage=5; }

	template <class T>
			void	PushScene(T eScene){ PushScene( (int)eScene ); }
	virtual void	PushScene(int nScene);

	virtual LRESULT		OnDraw(CPlaneBase* lpPlane);
	//	�����Q�t�F�[�Y�ɕ��������̂���
	virtual LRESULT		OnSimpleDraw(CPlaneBase* lpPlane);
	virtual LRESULT		OnSimpleMove(CPlaneBase* lpPlane);

	//	factory��ݒ�^�擾�ł���BCScene�h���N���X������A
	//		CScene* m_p = GetSceneControl()->GetSceneFactory()->CreateScene(CSCENE1);
	//	�̂悤�ɂ��Ă����ЂƂ̃V�[���𐶐����Ă��̂Ȃ��Ŏq�V�[�����Ăяo�����Ƃ��o����B
	void SetSceneFactory(smart_ptr<CSceneFactory> pv) { m_pvSceneFactory = pv;}
	smart_ptr<CSceneFactory> GetSceneFactory() { return m_pvSceneFactory; }

	//	----- property

	//	�X�^�b�N�ɌĂяo�����V�[����������Ԃ�ReturnScene�����Ƃ��ꂪtrue�ɂȂ�
	virtual bool IsEnd(void);		//	�I�������̂��H

	//	���܎��s���Ă���V�[���́A�g�����W�V�������D�悷��V�[�����H
	virtual bool IsPrecedentScene(void);

	//	���݂̃V�[���i���o�[��Ԃ�
	virtual int GetSceneNo(void)	{ return m_vSceneFrame.m_nScene; }
	//	���݂̃V�[����Ԃ�
	virtual CScene* GetScene(void)	{ return m_vSceneFrame.m_lpScene; }

	//	�V�[���Ԃ̂�����Ƃ����p�����[�^�̂��Ƃ�Ɏg���Ɨǂ��B
	int*	GetSceneParam(void) { return& m_anSceneParam[0];}

protected:
	//	�V�[����factory
	smart_ptr<CSceneFactory> m_pvSceneFactory;

	virtual void	CreateScene(int nScene);	//	�V�[���̐����i�����I�Ɏg�p�j

	//	-----�ړ��v�����b�Z�[�W
	int		m_nMessage;
	//	0:No Message 1:JumpScene 2:CallScene
	//	3:CallSceneFast 4:ReturnScene 5:ExitScene
	int			m_nNextScene;				//	���Ɉړ����ׂ��V�[��

	class CSceneFrame {
	public:
		smart_ptr<CScene>	m_lpScene;		//	�V�[���̃|�C���^
		int					m_nScene;		//	���邢��Create���Ȃ���΂Ȃ�Ȃ��V�[����

		CSceneFrame() {
			m_nScene = -1;	//	����Create���Ȃ��B
		}
	};
	CSceneFrame	m_vSceneFrame;				//	���݂̃V�[��
	stack<CSceneFrame>	m_SceneFrameStack;	//	�Ăяo�����V�[���͂����ɋL��

private:
	//	�V�[���Ԃ̂�����Ƃ����f�[�^�̂��Ƃ�Ɏg���B�i�ƕ֗��O�O�j
	int		m_anSceneParam[16];
};

#endif