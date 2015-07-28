// yaneKeyBase.cpp

#include "stdafx.h"
#include "yaneKeyBase.h"

//////////////////////////////////////////////////////////////////////////////

CKeyBase::CKeyBase(void){
	m_nKeyBufNo=0;	// ���ƕ\��flip���āA�������Ƃ�̂ɗ��p
	ZERO(m_byKeyBuffer);
}

CKeyBase::~CKeyBase(){
}

//////////////////////////////////////////////////////////////////////////////

inline bool CKeyBase::IsKeyPress(int key) const {
	return	(m_byKeyBuffer[m_nKeyBufNo][key]) !=0;
};

inline bool CKeyBase::IsKeyPush(int key) const {
	// �����ꂽ�u�Ԃ�true�ɂ���ꍇ
	if (!(m_byKeyBuffer[  m_nKeyBufNo][key])) return false;
	if (  m_byKeyBuffer[1-m_nKeyBufNo][key] ) return false;
	return true;
}

inline BYTE*	CKeyBase::GetKeyData(void) const {
	return	(BYTE*)&m_byKeyBuffer[m_nKeyBufNo];
}