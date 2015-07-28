//	CPlaneTransiter

#ifndef __yanePlaneTransiter_h__
#define __yanePlaneTransiter_h__

#include "yaneInteriorCounter.h"
#include "yanePlaneBase.h"

//	���W�E�u�����h�䗦�̃V�t�g�E�傫���E�ړ����x���𓝊��I�ɊǗ�����
class CPlaneTransiter {
public:
	//	�R���X�g���f�X�g
	CPlaneTransiter(){}
	virtual ~CPlaneTransiter() {}

	virtual void	Inc();					//	���̈ʒu�ɑJ��
	CPlaneTransiter& operator++() { Inc(); return (*this); }
	CPlaneTransiter operator++(int) { CPlaneTransiter _Tmp = *this; Inc(); return (_Tmp); }
	//	�����Q��virtual�łȂ��Ƃ�Inc��virtual�Ȃ̂łn�j�B
	//	�������A�Ō�̂�CPlaneTransiter�ɃR�s�[���Ă���Ƃ��낪
	//	�h���N���X�ł�������Ɗ�Ȃ��̂����c

	virtual void	Dec();		//	���Z
	CPlaneTransiter& operator--() { Dec(); return (*this); }
	CPlaneTransiter operator--(int) { CPlaneTransiter _Tmp = *this; Dec(); return (_Tmp); }

	//	�ݒ�^�擾
	virtual CInteriorCounter* GetX() { return& m_nX; }		//	���W
	virtual CInteriorCounter* GetY() { return& m_nY; }
	virtual CInteriorCounter* GetT() { return& m_nT; }		//	�s�����x
	virtual CInteriorCounter* GetR() { return& m_nR; }		//	�g��k���䗦
	virtual int*			  GetB() { return& m_nB; }		//	�x�[�X�ʒu

	virtual void SetPlane(CPlaneBase*p){ m_lpPlane = p;}
	virtual CPlaneBase* GetPlane(){ return m_lpPlane;}

	bool	IsEnd() { return (m_nX.IsEnd() == m_nY.IsEnd() == true) ;} 

	//	����ňꔭ�\��
	virtual void	OnDraw(CPlaneBase*);	//	�����ɕ\������B

private:
	CInteriorCounter	m_nX,m_nY;		//	���W
	CInteriorCounter	m_nT;			//	�s�����x�i�u�����h�䗦0-256�j
	CInteriorCounter	m_nR;			//	�g��k���䗦(�����256�œ��{)
	int		m_nB;						//	�x�[�X�ʒu(0:�摜���S 1:���� 2:�E�� 3:���� 4:�E��)

	//	��]�p�C�G�t�F�N�g�����T�|�[�g�������̂����c

	smart_ptr<CPlaneBase>	m_lpPlane;
};

#endif