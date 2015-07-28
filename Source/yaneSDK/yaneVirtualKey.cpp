//	yaneVirtualKey.cpp :

#include "stdafx.h"
#include "yaneVirtualKey.h"

//bool	operator <(const CVKeyBase& x,const CVKeyBase& y)  { return x.m_key < y.m_key; }

//////////////////////////////////////////////////////////////////////////////
CVirtualKey::CVirtualKey(void){
	ClearKeyDevice();
	ClearKeyList();
	m_bResetKey=false;
}

CVirtualKey::~CVirtualKey() {}
//////////////////////////////////////////////////////////////////////////////

//	�L�[�f�o�C�X�̓o�^
void	CVirtualKey::ClearKeyDevice(void){
	m_nDeviceMax = 0;
}

void	CVirtualKey::AddDevice(CKeyBase* keybase){
	m_lpDevice[m_nDeviceMax++] = keybase;
}

void	CVirtualKey::RemoveDevice(CKeyBase* keybase){
	//	STL�g���ĂȂ��̂Ŏ��O��erase����:p
	int i;
	for(i=0;i<m_nDeviceMax;i++){
		if (m_lpDevice[i]==keybase){
			for(int j=i+1;j<m_nDeviceMax;j++){
				m_lpDevice[i]=m_lpDevice[j];
			}
			m_nDeviceMax--;
		}
	}

	//	����ɉ��z�L�[�̍폜���
	for(i=0;i<VIRTUAL_KEY_MAX;i++){
		CVKeyList::iterator it;
		for(it = m_VKey[i].begin();it!=m_VKey[i].end();){
			if ((*it)->m_lpKeyBase == keybase){
				//	DELETE_SAFE(*it);
				//	��auto_ptr�Ȃ̂ŕs�v
				it = m_VKey[i].erase(it);
			} else {
				it++;
			}
		}
	}
}

CKeyBase* CVirtualKey::GetDevice(int n){
	return m_lpDevice[n];
}

void	CVirtualKey::Input(void){
	m_bResetKey = false;
	for(int i=0;i<m_nDeviceMax;i++)
		m_lpDevice[i]->GetKeyState();
}

//////////////////////////////////////////////////////////////////////////////

//	���z�L�[�̒ǉ��E�폜
void	CVirtualKey::ClearKeyList(void){
	for(int i=0;i<VIRTUAL_KEY_MAX;i++) {
		m_VKey[i].clear();	//	auto_ptr�Ȃ̂ŁA����ŃI�b�P�`
	}
}

void	CVirtualKey::AddKey(int vkey,CKeyBase* base,int key){
	CVKeyBase* lp = new CVKeyBase;
	lp->m_lpKeyBase = base;
	lp->m_key		= key;
	m_VKey[vkey].insert(lp);
}

void	CVirtualKey::RemoveKey(int vkey,CKeyBase* base,int key){
	CVKeyList::iterator it;
	for(it = m_VKey[vkey].begin();it!=m_VKey[vkey].end();){
		if (((*it)->m_lpKeyBase == base) &&
			((*it)->m_key		== key)){
		//	DELETE_SAFE(*it);
		//	��auto_ptr�Ȃ̂ŁA�s�v
			it = m_VKey[vkey].erase(it);
		} else {
			it++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

bool	CVirtualKey::IsVKeyPress(int vkey) {					//	���z�L�[�𒲂ׂ�
	if (m_bResetKey) return false;
	CVKeyList::iterator it;
	it = m_VKey[vkey].begin();		//	���ꂪ����Ƃ��̊֐���const�����o�֐��ɏo���Ȃ�
	while (it!=m_VKey[vkey].end()) {
		if ((*it)->m_lpKeyBase->IsKeyPress((*it)->m_key)) return true;
		it++;
	}
	return false;
}

bool	CVirtualKey::IsVKeyPush(int vkey)  {					//	���z�L�[�𒲂ׂ�
	if (m_bResetKey) return false;
	CVKeyList::iterator it;
	it = m_VKey[vkey].begin();
	while (it!=m_VKey[vkey].end()) {
		if ((*it)->m_lpKeyBase->IsKeyPush((*it)->m_key)) return true;
		it++;
	}
	return false;
}

void	CVirtualKey::ResetKey(void){
	m_bResetKey = true;
}