//	yaneSinTable.h :
//
//		Sin Table

#ifndef __yaneSinTable_h__
#define __yaneSinTable_h__

class CSinTable {
public:
	//	n:0-511�ň���B���ʂ�<<16����ĕԂ�
static LONG	Cos(int n){ return m_lTable[n & 511]; }
static LONG	Sin(int n){ return m_lTable[(n+384) & 511]; }

	//	�R���X�g���N�^�ŏ��������邽�߁A
	//	CSinTable�N���X���g�p����ꍇ�͕K���ǂ�����
	//	�R���X�g���N�g����K�v������
	CSinTable(void);

protected:

static	LONG	m_lTable[512];	// cos table
static	bool	m_bInitialize;
};
	//	Visual C++6.0�Ȃ�΁A���̃N���X�̃R���X�g���N�^���g��Ȃ�����
	//	m_lTable�̗̈�͓��I�ɂ��m�ۂ���Ȃ��B
	//	�܂��A�z��́A�ő�Y�����Ƃ��Ďg���Ă���͈͂ł����m�ۂ���Ȃ��B
	//	��:for��0����255�܂ŉ񂵁A�����ȍ~�ɃA�N�Z�X���Ȃ����Ƃ��ۏ؂����Ȃ��
	//	�@�@�z���256�������m�ۂ���Ȃ��B(VC�̍œK��)

#endif