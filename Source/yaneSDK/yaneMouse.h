// MouseInput.h :
//	�}�E�X���͗p
//		programmed by yaneurao(M.Isozaki) '99/7/31
//
//	���������N���X�ł͂Ȃ��B:p
//
//		���@�_�u���N���b�N�̊��m�͂��Ȃ��B

#ifndef __yaneMouseInput_h__
#define __yaneMouseInput_h__

#include "yaneWinHook.h"

class CMouse : public CWinHook {
public:
	virtual LRESULT GetXY(int &x,int &y)const;			//	�}�E�X�|�W�V�����𓾂�i�N���C�A���g���W�n�ɂāj
	virtual bool	RButton()const;						//	�E�{�^����Ԃ𓾂�
	virtual bool	LButton()const;						//	���{�^����Ԃ𓾂�
	virtual LRESULT GetInfo(int &x,int &y,int &b)const;	// �}�E�X�|�W�V�����ƃ{�^����Ԃ�Ԃ�
	virtual void	GetButton(bool&bL,bool&bR);			//	�O���GetButton���牟���ꂽ���H
	virtual void	ResetButton();						//	���{�^����ԃ��Z�b�g

	virtual LRESULT SetXY(int x,int y);					//	�}�E�X���w��̃|�W�V�����Ɉړ��i�N���C�A���g���W�n�ɂāj

	virtual void	SetOutScreenInput(bool bEnable);
	//	�������܂܁A�}�E�X����ʊO�ɂ�����Ƃ��A�ǂ��Ȃ�̂��H
	//		true  == �{�^���͉������܂܂��ƌ��Ȃ����
	//		false == �{�^���͗����ꂽ���̂ƌ��Ȃ����
	//		default�ł�false

	CMouse();
	virtual ~CMouse();

protected:
	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam); // ���b�Z�[�W�̃R�[���o�b�N

	bool	m_bRB;					//�@�}�E�X�{�^�����
	bool	m_bLB;
	bool	m_bHistRB;				//	����
	bool	m_bHistLB;
	bool	m_bOutScreenInput;		//	��ʊO�ł̓���
};

class CMouseEx : public CMouse {
public:
	virtual LRESULT GetXY(int &x,int &y)const;			//	�}�E�X�|�W�V�����𓾂�i�N���C�A���g���W�n�ɂāj
	virtual bool	RButton()const;						//	�E�{�^����Ԃ𓾂�
	virtual bool	LButton()const;						//	���{�^����Ԃ𓾂�
	virtual LRESULT GetInfo(int &x,int &y,int &b)const;	// �}�E�X�|�W�V�����ƃ{�^����Ԃ�Ԃ�

	//	�O���Flush���牟���ꂽ���H
	virtual void	GetButton(bool&bL,bool&bR);
	virtual bool	IsPushRButton()const;
	virtual bool	IsPushLButton()const;
	//	�O���Flush���牟���グ��ꂽ���H
	virtual void	GetUpButton(bool&bL,bool&bR);
	virtual bool	IsPushUpRButton()const;
	virtual bool	IsPushUpLButton()const;

	virtual void	ResetButton();						//	���{�^����ԃ��Z�b�g

	virtual LRESULT SetXY(int x,int y);					//	�}�E�X���w��̃|�W�V�����Ɉړ��i�N���C�A���g���W�n�ɂāj
														//	���܂������X�V�O�O�G
	//	�t���b�V��������
	virtual LRESULT	Flush();

	//	�K�[�h�^�C�������ǂ�����Ԃ�
	virtual bool	IsGuardTime()const;
	//	�K�[�h�^�C����ݒ肷��
	void	SetGuardTime(int nTime);

	CMouseEx();
	virtual ~CMouseEx();

protected:
	bool	m_bRBN;					//�@Flush�����Ƃ��̃}�E�X�{�^�����
	bool	m_bLBN;
	int		m_nRLBN;				//	Flush�����Ƃ��̃}�E�X�{�^�����
	int		m_nX,m_nY;				//	Flush�����Ƃ��̃}�E�X�̃|�W�V����
	int		m_nGuardTime;			//	�K�[�h�^�C��
};

#endif