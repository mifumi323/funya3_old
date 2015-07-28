#include "stdafx.h"
#include "yaneRootCounter.h"
#include <limits.h>

CRootCounter::CRootCounter(void) {
	Set(0,INT_MAX,1);
	m_bInit = false;
	m_bReverse = false;
	Reset();
}

CRootCounter::CRootCounter(int nEnd){
	Set(0,nEnd,1);
	m_bInit = false;
	m_bReverse = false;
	Reset();
}

CRootCounter::CRootCounter(int nStart,int nEnd,int nStep){
	Set(nStart,nEnd,nStep);
	m_bInit = false;
	m_bReverse = false;
	Reset();
}

CRootCounter::~CRootCounter() {
}

//////////////////////////////////////////////////////////////////////////////

void	CRootCounter::Reset(void){
	m_nRate			= 0;
	m_bLapAround	= false;
	m_bLapAroundI	= false;
	m_bReversing	= false;
	if (!m_bInit) {
		m_nRootCount	= m_nStart;
	} else {
		m_nRootCount	= m_nInit;
	}
}

void	CRootCounter::SetStep(int nStep){
	m_nStep = nStep;
}

void	CRootCounter::SetStart(int nStart){
	m_nStart	= nStart;
}

void	CRootCounter::SetEnd(int nEnd){
	m_nEnd = nEnd;
}

void	CRootCounter::Set(int nStart,int nEnd,int nStep){
	m_nStep		= nStep;
	m_nStart	= nStart;
	m_nEnd		= nEnd;
}


//	�J�E���^�̃C���N�������g
void	CRootCounter::Inc(void){
	if (m_bReverse){
		if (!m_bReversing) {
			if (m_nStep>0) {
				m_nRootCount+=m_nStep;
				if (m_nRootCount>=m_nEnd) {
					m_nRootCount = m_nEnd;
					m_bLapAround = true;
					m_bLapAroundI = false;
					m_bReversing = true;	//	�_�E���J�E���g�̊J�n
				} else {
					m_bLapAround = false;
					m_bLapAroundI = false;
				}
			} else {
				m_nRate++;
				if (m_nRate==-m_nStep) {
					m_nRate = 0;
					m_nRootCount++;
					if (m_nRootCount>=m_nEnd) {
						m_nRootCount = m_nStart;
						m_bLapAround = true;
						m_bLapAroundI = false;
						m_bReversing = true;	//	�_�E���J�E���g�̊J�n
					} else {
						m_bLapAround = false;
						m_bLapAroundI = false;
					}
				}
			}
		} else {
			if (m_nStep>0) {
				m_nRootCount-=m_nStep;
				if (m_nRootCount<=m_nStart) {
					m_nRootCount = m_nStart;
					m_bLapAround = true;
					m_bLapAroundI = true;
					m_bReversing = false;	//	�A�b�v�J�E���g�̊J�n
				} else {
					m_bLapAround = false;
					m_bLapAroundI = false;
				}
			} else {
				m_nRate++;
				if (m_nRate==-m_nStep) {
					m_nRate = 0;
					m_nRootCount--;	//	�_�E���J�E���g
					if (m_nRootCount<=m_nStart) {
						m_nRootCount = m_nStart;
						m_bLapAround = true;
						m_bLapAroundI = true;	//	���̎��Ɍ���true
						m_bReversing = false;	//	�A�b�v�J�E���g�̊J�n
					} else {
						m_bLapAround = false;
						m_bLapAround = false;
					}
				}
			}
		}
	} else {
	//	���o�[�X�J�E���^�łȂ��Ȃ�΁A���o�[�X��Ԃł��邩�Ɋւ�炸�O�����Z
		if (m_nStep>0) {
			m_nRootCount+=m_nStep;
			if (m_nRootCount>=m_nEnd) {
				m_nRootCount = m_nStart;
				m_bLapAround = true;
			} else {
				m_bLapAround = false;
			}
		} else {
			m_nRate++;
			if (m_nRate==-m_nStep) {
				m_nRate = 0;
				m_nRootCount++;
				if (m_nRootCount>=m_nEnd) {
					m_nRootCount = m_nStart;
					m_bLapAround = true;
				} else {
					m_bLapAround = false;
				}
			}
		}
	}
}

//	�J�E���^�̃T�`�����[�V�����C���N�������g
void	CRootCounter::IncS(void){
//	if (m_bReverse){
	//	�����o�[�X�J�E���^���ǂ����ɂ�����炸�A
	//	�@�@�@����������Ĕ��f���ׂ�
		if (!m_bReversing) {
			if (m_nStep>0) {
				m_nRootCount+=m_nStep;
				if (m_nRootCount>=m_nEnd) {
					m_nRootCount = m_nEnd;
					m_bLapAround = true;
				//	m_bReversing = true;	//	�_�E���J�E���g�̊J�n
				} else {
					m_bLapAround = false;
				}
			} else {
				m_nRate++;
				if (m_nRate==-m_nStep) {
					m_nRate = 0;
					m_nRootCount++;
					if (m_nRootCount>=m_nEnd) {
						m_nRootCount = m_nEnd;
						m_bLapAround = true;
					//	m_bReversing = true;	//	�_�E���J�E���g�̊J�n
					} else {
						m_bLapAround = false;
					}
				}
			}
		} else {
			if (m_nStep>0) {
				m_nRootCount-=m_nStep;
				if (m_nRootCount<=m_nStart) {
					m_nRootCount = m_nStart;
					m_bLapAround = true;
				//	m_bReversing = false;	//	�A�b�v�J�E���g�̊J�n
				} else {
					m_bLapAround = false;
				}
			} else {
				m_nRate++;
				if (m_nRate==-m_nStep) {
					m_nRate = 0;
					m_nRootCount--;	//	�_�E���J�E���g
					if (m_nRootCount<=m_nStart) {
						m_nRootCount = m_nStart;
						m_bLapAround = true;
				//		m_bReversing = false;	//	�A�b�v�J�E���g�̊J�n
					} else {
						m_bLapAround = false;
					}
				}
			}
		}
//	}
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CRootCounterS::CRootCounterS(void) {
	Set(0,INT_MAX,1);
	Reset();
}

CRootCounterS::CRootCounterS(int nEnd){
	Set(0,nEnd,1);
	Reset();
}

CRootCounterS::CRootCounterS(int nStart,int nEnd,int nStep){
	Set(nStart,nEnd,nStep);
	Reset();
}

void CRootCounterS::Inc(bool bAdd){
	bool bInc = (m_nStart > m_nEnd) ^ bAdd; // �t�����J�E���^�H
	if (bInc) {
	//	�C���N�������g
		if (m_nStep>0) {
		//	�����C���N�������g
			m_nRootCount += m_nStep;
		} else {
		//	�����C���N�������g
			m_nRate++; if (m_nRate>=(-m_nStep)) { m_nRate = 0; m_nRootCount++; }
		}
		//	�T�`�����[�g�����̂��H
		int nMax = m_nStart < m_nEnd ? m_nEnd : m_nStart;
		if (m_nRootCount > nMax) m_nRootCount = nMax;
	} else {
	//	�f�N�������g
		if (m_nStep>0) {
		//	�����f�N�������g
			m_nRootCount -= m_nStep;
		} else {
		//	�����f�N�������g
			m_nRate--; if (m_nRate<=(m_nStep)) { m_nRate = 0; m_nRootCount--; }
			//	�ˁ@m_nRate++�łȂ����Ƃɒ��ӁB
			//	++�̂���--���āA���̐��������Ƃ�Ȃ��Ă͂Ȃ�Ȃ�
			//	���AnStep<0�̂Ƃ��ŏ��̂P��ڂ�--��RootCounter��
			//	1�����Ă͂����Ȃ��B����Ă������������ɂȂ�
		}
		//	�T�`�����[�g�����̂��H
		int nMin = m_nStart < m_nEnd ? m_nStart : m_nEnd;
		if (m_nRootCount < nMin) m_nRootCount = nMin;
	}
}