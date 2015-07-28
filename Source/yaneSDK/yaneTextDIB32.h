//	yaneTextDIB32.h :

#ifdef USE_DIB32

#ifndef __yaneTextDIB32_h__
#define __yaneTextDIB32_h__

#include "yaneFont.h"
#include "yaneDIB32.h"

class CTextDIB32 : public CDIB32 {
public:

	CFont*	GetFont(void) { return &m_Font; }
	void	SetTextPos(int x,int y){ m_nTextX=x; m_nTextY=y; }
	void	GetTextPos(int&x,int&y){ x=m_nTextX; y=m_nTextY; }
	void	SetVertical(bool bVertical = true);	// �c�����ɐݒ�
	bool	IsVertical(void) const { return m_bVertical; }

	LRESULT	UpdateText(void);
	LRESULT	UpdateTextA(void);	//	�A���`�F��������o�[�W����
										//	�����l�����悤�ɂȂ�̂ŕ`���BlendBltFastAlpha�ōs�Ȃ�
	LRESULT	UpdateTextAA(void); //	���ׂȃA���`�F����������o�[�W����
										//	UpdateTextAA��肸���Ԃ�x���O�O�G�G

	CTextDIB32();
	virtual ~CTextDIB32() {};	//	plane holder

protected:
	LRESULT UpdateTextHorizontal(void);		//�@������ UpdateText
	LRESULT UpdateTextHorizontalA(void);	//�@������ UpdateTextA
	LRESULT UpdateTextHorizontalAA(void);	//�@������ UpdateTextAA
	LRESULT UpdateTextVertical(void);		//�@�c���� UpdateText
	LRESULT UpdateTextVerticalA(void);		//�@�c���� UpdateTextA
	LRESULT UpdateTextVerticalAA(void);		//�@�c���� UpdateTextAA

	CFont		m_Font;				//	�����ŕ`��
	int			m_nTextX,m_nTextY;	//	�e�L�X�g�`��ʒu
	bool		m_bVertical;		//�@�c�������ǂ���
};

/*	//	���������R�[�f�B���O�����肩�ȁH
#ifdef USE_YGA
class CTextDIB32Alpha : public CTextDIB32 {
public:
	LRESULT	UpdateText(void){ return CTextDIB32::UpdateTextA; }
};
class CTextDIB32Alpha2 : public CTextDIB32 {
public:
	LRESULT	UpdateText(void){ return CTextDIB32::UpdateTextAA; }
};
#endif	//	ifdef USE_YGA
*/

#endif	//	ifdef USE_DIB32

#endif // USE_DIB32