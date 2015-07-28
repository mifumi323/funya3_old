//	effect�n��blt

#ifndef __yanePlaneEffectBlt_h__
#define __yanePlaneEffectBlt_h__

#include "yaneSinTable.h"
class CPlaneBase;

/////////////////////////////////////////////////////////////////
//	�X�N���[�������p�i�wRevolution�x�̃I�[�v�j���O�Ŏg�p�j

class CPlaneFadeBlt {
public:
	LRESULT	FadeBlt(CPlaneBase*,CPlaneBase*,int x,int y);

	int*	GetFadeTable(void)	 { return& m_nFadeTable[0];	  }
	int*	GetRasterTable(void) { return& m_nRasterTable[0]; }

	CPlaneFadeBlt(void);
	virtual ~CPlaneFadeBlt();

protected:
	int		m_nFadeTable[480];			//	�e���X�^�[��Fade�l(0-256)��ݒ�
	int		m_nRasterTable[480];		//	�e���X�^�[�̃��X�^�[���炵�l��ݒ�
};

/////////////////////////////////////////////////////////////////
//	�g�����W�V�����n��generic blter

//	���X�i
class CPlaneTransBltListener {
public:
	virtual LRESULT Blt(CPlaneBase*lpDst,CPlaneBase*lpSrc,int x,int y,int nPhase,int nTransMode=0,BYTE byFadeRate=255,LPRECT lpDstClipRect=NULL) = 0;
	virtual ~CPlaneTransBltListener() {}
};

#define TransBltFunc(FuncName) \
LRESULT FuncName(CPlaneBase*lpDst,CPlaneBase*lpSrc,int x,int y,int nPhase,int nTransMode=0,BYTE byFadeRate=255,LPRECT lpDstClipRect=NULL);
	//	nTransMode : 0 == �k�L�Ȃ�	1==�k�L�L��	 2==���t���摜(CDIB32����)
	//				 3 == BltNatural�ł̓]��

class CPlaneTransBlt {
public:
	static LRESULT Blt(int nTransNo,CPlaneBase*lpDst,CPlaneBase*lpSrc,int x,int y,int nPhase,int nTransMode=0,BYTE byFadeRate=255,LPRECT lpDstClipRect=NULL);	//	�ԍ��ŌĂяo����̂�

	static TransBltFunc(MirrorBlt1);	// ���E�~���[
	static TransBltFunc(MirrorBlt2);	// �㉺�~���[
	static TransBltFunc(MirrorBlt3);	// ������
	static TransBltFunc(MirrorBlt4);	// �c����

	static TransBltFunc(CutInBlt1);	// ���������_�����X�^
	static TransBltFunc(CutInBlt2);	// ��`�����_��
	static TransBltFunc(CutInBlt3);	// 4*2�h�b�g��` �����_���t�F�[�h
	static TransBltFunc(CutInBlt4);	// 4*2�h�b�g��` �����_�� �t�F�[�h�ォ��
	static TransBltFunc(CutInBlt5);	// 4*2�h�b�g��` �����_�� �t�F�[�h������
	static TransBltFunc(CutInBlt6);	// 4*2�h�b�g��` �����_�� �t�F�[�h������
	static TransBltFunc(CutInBlt7);	// 4*2�h�b�g��` �����_�� �t�F�[�h�E����
	static TransBltFunc(CutInBlt8);	// ���������`
	static TransBltFunc(CutInBlt9);	// ���ォ���`
	static TransBltFunc(CutInBlt10);// �E�ォ���`
	static TransBltFunc(CutInBlt11);// ���������`
	static TransBltFunc(CutInBlt12);// ���E�Ăꂱ�ŏo��
	static TransBltFunc(CutInBlt13);// �㉺�Ăꂱ�ŏo��
	static TransBltFunc(CutInBlt14);// ���E�Ăꂱ�Q�i�ŏo��
	static TransBltFunc(CutInBlt15);// �㉺�Ăꂱ�Q�i�ŏo��
	static TransBltFunc(CutInBlt16);// �������烉���_���t�F�[�h 4*2��`
	static TransBltFunc(CutInBlt17);// �������烉���_���t�F�[�h 2*2��`
	static TransBltFunc(CutInBlt18);// �V���b�t�����O�ォ�牺��
	static TransBltFunc(CutInBlt19);// �V���b�t�����O��������

	static TransBltFunc(WaveBlt1);	// �gBlt
	static TransBltFunc(WaveBlt2);	// �gBlt 180�ʑ��Ⴂ
	static TransBltFunc(WaveBlt3);	// ���gBlt
	static TransBltFunc(WaveBlt4);	// ���gBlt 180�ʑ��Ⴂ

	static TransBltFunc(WhorlBlt1);	// �����܂� �O����
	static TransBltFunc(WhorlBlt2);	// �����܂� ������
	static TransBltFunc(WhorlBlt3);	// �����܂� �O���� 2*2
	static TransBltFunc(WhorlBlt4);	// �����܂� ������ 2*2
	static TransBltFunc(WhorlBlt5);	// �����܂� �O���� 4*4
	static TransBltFunc(WhorlBlt6);	// �����܂� ������ 4*4
	static TransBltFunc(WhorlBlt7);	// �����܂� �O���� 8*8
	static TransBltFunc(WhorlBlt8);	// �����܂� ������ 8*8

	static TransBltFunc(CircleBlt1);	// �ォ��~
	static TransBltFunc(CircleBlt2);	// ������~
	static TransBltFunc(CircleBlt3);	// ������~
	static TransBltFunc(CircleBlt4);	// �E����~
	static TransBltFunc(CircleBlt5);	// ��������~

	static TransBltFunc(RectBlt1);	// ��`�^ 4�h�b�g
	static TransBltFunc(RectBlt2);	// ��`�^ 8�h�b�g
	static TransBltFunc(RectBlt3);	// ��`�^ 16�h�b�g
	
	static TransBltFunc(BlindBlt1);	// �c����Blt������8�h�b�g
	static TransBltFunc(BlindBlt2);	// �c����Blt�E����8�h�b�g
	static TransBltFunc(BlindBlt3);	// �c����Blt������16�h�b�g
	static TransBltFunc(BlindBlt4);	// �c����Blt�E����16�h�b�g
	static TransBltFunc(BlindBlt5);	// ������Blt�ォ��8�h�b�g
	static TransBltFunc(BlindBlt6);	// ������Blt������8�h�b�g
	static TransBltFunc(BlindBlt7);	// ������Blt�ォ��16�h�b�g
	static TransBltFunc(BlindBlt8);	// ������Blt������16�h�b�g
	static TransBltFunc(BlindBlt9);	// �E�΂ߊ���Blt 8�h�b�g
	static TransBltFunc(BlindBlt10);// �E�΂ߊ���Blt 16�h�b�g

	static TransBltFunc(BlendBlt1);	// ���ʂ�BlendBlt

	//	Thanks ! > TearDrop_Stone

	static TransBltFunc(MosaicBlt1);// ���U�C�N
	static TransBltFunc(FlushBlt1);	// �l�K�|�W���]
	static TransBltFunc(SlitCurtainBlt1);	// ������J�[�e���B16 dot�B
	static TransBltFunc(SlitCurtainBlt2);	// �E����J�[�e���B16 dot�B
	static TransBltFunc(SlitCurtainBlt3);	// ������J�[�e���B8 dot�B
	static TransBltFunc(SlitCurtainBlt4);	// �E����J�[�e���B8 dot�B
	static TransBltFunc(SlitCurtainBlt5);	// �ォ��̃J�[�e���B16 dot�B
	static TransBltFunc(SlitCurtainBlt6);	// ������̃J�[�e���B16 dot�B
	static TransBltFunc(SlitCurtainBlt7);	// �ォ��̃J�[�e���B8 dot�B
	static TransBltFunc(SlitCurtainBlt8);	// ������̃J�[�e���B8 dot�B
	static TransBltFunc(TensileBlt1);		// ��ʍ��[��������L�΂���->���̉摜�B
	static TransBltFunc(TensileBlt2);		// ��ʉE�[��������L�΂���->���̉摜�B
	static TransBltFunc(TensileBlt3);		// ��[��������L�΂�->���摜�B
	static TransBltFunc(TensileBlt4);		// ���[��������L�΂�->���摜�B

	// yaneSDK1st�ł��������
	static TransBltFunc(DiagonalDiffusionBlt);		// �΂߂ɏ����Ă���
	static TransBltFunc(DiffusionCongeriesBlt1);	// �����_���g�U����̏W�� 4*2
	static TransBltFunc(DiffusionCongeriesBlt2);	// �����_���g�U����̏W�� 2*2
	static TransBltFunc(DiffusionCongeriesBlt3);	// �����_���g�U����̏W��
	static TransBltFunc(SquashBlt);					// ������Ԃ�
	static TransBltFunc(ForwardRollBlt);			// �O����]
	static TransBltFunc(RotationBlt1);				// ��ʒ��S��]
	static TransBltFunc(RotationBlt2);				// ��ʒ��S��]
	static TransBltFunc(RotationBlt3);				// ��ʒ��S��]
	static TransBltFunc(RotationBlt4);				// ��ʒ��S��]
	static TransBltFunc(EnterUpBlt1);				// 8*8 ����������In
	static TransBltFunc(EnterUpBlt2);				// 16*16 ����������In
	static TransBltFunc(CellGatherBlt1);			// ����݂���݂P
	static TransBltFunc(CellGatherBlt2);			// ����݂���݂Q

//	static TransBltFunc(WaveFadeIn1);				// ���߂��Ȃ���FadeIn

	static vector< smart_ptr<CPlaneTransBltListener> >* GetBltListener(){
		return &m_avListener;
	}

protected:
	//	�R�[���o�b�N�p�֐�
	static vector< smart_ptr<CPlaneTransBltListener> > m_avListener;

	//	SlitCurtainBlt��TensileBlt�̃A�V�X�g�֐�
	static LRESULT BltTransHelper1(CPlaneBase* pSrc, CPlaneBase* pDest, int x, int y,
									int nPhase, bool Direction, int nTransMode, BYTE byFadeRate,
									int WidthNum,LPRECT lpDstClipRect);
	static LRESULT BltTransHelper2(CPlaneBase* pSrc, CPlaneBase* pDest, int x, int y,
									int nPhase, bool Direction, int nTransMode, BYTE byFadeRate,
									int WidthNum,LPRECT lpDstClipRect);
	static LRESULT TensileBltHelper1(CPlaneBase* pSrc, CPlaneBase* pDest, int x, int y,
									 int nPhase, bool Direction, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect);
	static LRESULT TensileBltHelper2(CPlaneBase* pSrc, CPlaneBase* pDest, int x, int y,
									 int nPhase, bool Direction, int nTransMode, BYTE byFadeRate,LPRECT lpDstClipRect);


	//	�����_�����̕K�v�Ȃ���
	static void MakeBltTable();

	class BltTransTable {
	public:
		BltTransTable();
		BYTE   RandTable[256];	//	���U�C�N�p�����_���p�^�[��
	};

	static auto_ptrEx<CSinTable>		m_sin_table;
	static auto_ptrEx<BltTransTable>	m_blt_table;

};

#endif