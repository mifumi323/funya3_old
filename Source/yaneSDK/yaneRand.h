//	Mersenne Twister�@�ɂ�鍂���x��������
//	�@2^19937-1�Ƃ������ɒ����������������������������ɐ�������I

#ifndef __yaneRand_h__
#define __yaneRand_h__

#include "yaneSerialize.h"

class CRand : public CArchive { // ���̃N���X�́A�V���A���C�Y�Ή��I
public:
	DWORD	Get(void);				//	�����̎擾
	DWORD	Get(int n){				//	0�`n-1�̗����̎擾
		//WARNING(n==0,"CRand��n==0");
		if (n==0) return 0;
		return Get() % n;
	}

	void	SetSeed(DWORD dwSeed);	//	�����̎�̐ݒ�B�K����x�Ăяo���K�v������

	void	Randomize(void){		//	�����_�}�C�Y
		SetSeed(::GetTickCount());
	}

	CRand(void) { m_nMti = 624+1; } /* means m_dwMt is not initialized */
	CRand(DWORD dwSeed) { SetSeed(dwSeed); }
	virtual ~CRand() {}

protected:
	DWORD	m_dwMt[624];	// the array for the state vector
	int		m_nMti;			// initialization counter

protected:
	//	override from CArchive
	virtual void Serialize(CSerialize& s){
		s.Store(ArraySerialize(m_dwMt));	//	�����o�ϐ��܂邲�ƁA
		s << m_nMti;						//	�V���A���C�Y����̂��I
	}
};

#endif