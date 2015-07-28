#include "stdafx.h"
#include "yaneAppManager.h"

CAppManageList CAppManager::m_alpInfo;
CCriticalSection CAppManager::m_oCriticalSection;
int	CAppManager::m_nRef = 0;

//////////////////////////////////////////////////////////////////////////////
void CAppManager::Inc(){
	m_oCriticalSection.Enter();
	{
		m_nRef++;
	}
	m_oCriticalSection.Leave();
}

void CAppManager::Dec(){
	m_oCriticalSection.Enter();
	{
		m_nRef--;
	}
	m_oCriticalSection.Leave();
}

int CAppManager::GetRef(){
	int n;
	m_oCriticalSection.Enter();
	{
		n = m_nRef;
	}
	m_oCriticalSection.Leave();
	return n;
}
//////////////////////////////////////////////////////////////////////////////

void CAppManager::Add(CAppBase*lpAppBase){

	DWORD	dwThreadID = ::GetCurrentThreadId();

	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if ((*it)->m_lpAppBase == lpAppBase) {
				if ((*it)->m_dwThreadID1 == NULL) { (*it)->m_dwThreadID1 = dwThreadID; goto Exit; }
//				if ((*it)->m_dwThreadID2 == NULL) { (*it)->m_dwThreadID2 = dwThreadID; goto Exit; }
//	�}���`�X���b�h��߂܂����O�O
				Err.Out("CAppManager::Add����̂ɋ󂫂�����");
				goto Exit;
			}
			it++;
		}

		{ // info�̕ϐ��X�R�[�v�𐧌����邽�߂� { }
			CAppManageInfo* info = new CAppManageInfo;
			info->m_lpAppBase	=	lpAppBase;	
#ifdef USE_DirectDraw
			info->m_lpDirectDraw=	NULL;
#endif
#ifdef USE_FastDraw
			info->m_lpFastDraw=	NULL;
#endif
#ifdef USE_DIB32
			info->m_lpDIBDraw	=	NULL;
#endif
#ifdef USE_SAVER
			info->m_lpSaverDraw	=	NULL;
#endif
			info->m_lpAppFrame	=	NULL;
			info->m_dwThreadID1	=	dwThreadID;
//			info->m_dwThreadID2	=	NULL;
//	�}���`�X���b�h��߂܂����O�O
			m_alpInfo.insert(info);
		}

Exit:;
	}
	m_oCriticalSection.Leave();

}

void CAppManager::Add(CAppFrame*lpAppFrame){

	DWORD	dwThreadID = ::GetCurrentThreadId();
	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if (((*it)->m_dwThreadID1 == dwThreadID) /* ||
				((*it)->m_dwThreadID2 == dwThreadID) */ ) {
				//	�����X���b�hID�̃`�����N�������āA�����ɒǉ�����
				(*it)->m_lpAppFrame = lpAppFrame;
				//	��̃X���b�h�ɑ΂��ĂQ��CAppFrame�̃C���X�^���X��������
				//	���݂��邱�Ƃ͑z�肵�Ă��Ȃ�
				break;
			}
			it++;
		}
	}
	m_oCriticalSection.Leave();
}

////////////////////////////////////////////////////////////

#ifdef USE_DirectDraw
void CAppManager::Add(CDirectDraw* lpDirectDraw){

	DWORD	dwThreadID = ::GetCurrentThreadId();

	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID1 == dwThreadID /* ||
				(*it)->m_dwThreadID2 == dwThreadID */ ) { (*it)->m_lpDirectDraw = lpDirectDraw; goto Exit; }
//	�}���`�X���b�h��߂܂����O�O
			it++;
		}
		//	��v����X���b�h�������Ƃ͂ǂ��䂤��������H
		//	�iCDirectDraw�̃R���X�g���N�^���ςȂƂ���Ŏ��s���ꂽ�H�j
		Err.Out("CAppManager::Add����̂ɋ󂫂�����");
Exit:;
	}
	m_oCriticalSection.Leave();

}

void CAppManager::Del(CDirectDraw*lpDraw) {

	DWORD	dwThreadID = ::GetCurrentThreadId();

	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if (((*it)->m_dwThreadID1 == dwThreadID) && ((*it)->m_lpDirectDraw == lpDraw)){
				(*it)->m_lpDirectDraw = NULL;
			}
//	�}���`�X���b�h��߂܂����O�O
			it++;
		}
	}
	m_oCriticalSection.Leave();
}
CDirectDraw* CAppManager::GetMyDirectDraw(){
	CAppManageList::iterator it = m_alpInfo.begin();
	DWORD dwThreadID = ::GetCurrentThreadId();

	CDirectDraw* lpDraw = NULL;
	m_oCriticalSection.Enter();
	{
		while (it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID1 == dwThreadID /* ||
				(*it)->m_dwThreadID2 == dwThreadID */ ) {
//	�}���`�X���b�h��߂܂����O�O
				lpDraw = (*it)->m_lpDirectDraw;
				break;
			}
			it++;
		}
	}
	m_oCriticalSection.Leave();
	return lpDraw;		//	return null if it was not found...
}

#endif	//	USE_DirectDraw

////////////////////////////////////////////////////////////

#ifdef USE_FastDraw
void CAppManager::Add(CFastDraw* lpFastDraw){

	DWORD	dwThreadID = ::GetCurrentThreadId();

	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID1 == dwThreadID /* ||
				(*it)->m_dwThreadID2 == dwThreadID */ ) { (*it)->m_lpFastDraw = lpFastDraw; goto Exit; }
//	�}���`�X���b�h��߂܂����O�O
			it++;
		}
		//	��v����X���b�h�������Ƃ͂ǂ��䂤��������H
		//	�iCDirectDraw�̃R���X�g���N�^���ςȂƂ���Ŏ��s���ꂽ�H�j
		Err.Out("CAppManager::Add����̂ɋ󂫂�����");
Exit:;
	}
	m_oCriticalSection.Leave();

}

void CAppManager::Del(CFastDraw*lpDraw) {

	DWORD	dwThreadID = ::GetCurrentThreadId();

	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if (((*it)->m_dwThreadID1 == dwThreadID) && ((*it)->m_lpFastDraw == lpDraw)){
				(*it)->m_lpFastDraw = NULL;
			}
//	�}���`�X���b�h��߂܂����O�O
			it++;
		}
	}
	m_oCriticalSection.Leave();
}

CFastDraw* CAppManager::GetMyFastDraw(){
	CAppManageList::iterator it = m_alpInfo.begin();
	DWORD dwThreadID = ::GetCurrentThreadId();

	CFastDraw* lpDraw = NULL;
	m_oCriticalSection.Enter();
	{
		while (it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID1 == dwThreadID /* ||
				(*it)->m_dwThreadID2 == dwThreadID */ ) {
//	�}���`�X���b�h��߂܂����O�O
				lpDraw = (*it)->m_lpFastDraw;
				break;
			}
			it++;
		}
	}
	m_oCriticalSection.Leave();
	return lpDraw;		//	return null if it was not found...
}

#endif	//	USE_FastDraw

	/////////////////////////////////////////////////////////////////

void CAppManager::Del(CAppBase*app) {

	DWORD	dwThreadID = ::GetCurrentThreadId();

	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if ((*it)->m_lpAppBase == app) {
				if ((*it)->m_dwThreadID1 == dwThreadID) { (*it)->m_dwThreadID1 = NULL; }
//				if ((*it)->m_dwThreadID2 == dwThreadID) { (*it)->m_dwThreadID2 = NULL; }
//	�}���`�X���b�h��߂܂����O�O
#if 0
				if ((*it)->m_dwThreadID1 == NULL /* && (*it)->m_dwThreadID2 == NULL */ ) {
				//	DELETE_SAFE(*it);
				//	��auto_ptr�Ȃ̂ň��S�`
					it = m_alpInfo.erase(it);
				} else {
					it++;
				}
#endif
				//	�}���`�X���b�h��߂��̂ŁA�X���b�h�̑��݂ɂ�����炸
				//	������폜�o���ėǂ��B
				it = m_alpInfo.erase(it);
			} else {
				it++;
			}
		}
	}
	m_oCriticalSection.Leave();
}

void CAppManager::Del(CAppFrame*lpAppFrame){

	DWORD	dwThreadID = ::GetCurrentThreadId();
	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if ((*it)->m_lpAppFrame == lpAppFrame) {
				(*it)->m_lpAppFrame = NULL;	//	���ׂĂɑ΂��čs�Ȃ�
			}
			it++;
		}
	}
	m_oCriticalSection.Leave();
}

#ifdef USE_DIB32
void CAppManager::Add(CDIBDraw* lpDIBDraw){

	DWORD	dwThreadID = ::GetCurrentThreadId();

	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID1 == dwThreadID /* ||
				(*it)->m_dwThreadID2 == dwThreadID */ ) { (*it)->m_lpDIBDraw = lpDIBDraw; goto Exit; }
//	�}���`�X���b�h��߂܂����O�O
			it++;
		}
		//	��v����X���b�h�������Ƃ͂ǂ��䂤��������H
		//	�iCDIBDraw�̃R���X�g���N�^���ςȂƂ���Ŏ��s���ꂽ�H�j
		Err.Out("CAppManager::Add����̂ɋ󂫂�����");
Exit:;
	}
	m_oCriticalSection.Leave();

}
void CAppManager::Del(CDIBDraw*lpDraw) {

	DWORD	dwThreadID = ::GetCurrentThreadId();

	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if (((*it)->m_dwThreadID1 == dwThreadID) && ((*it)->m_lpDIBDraw == lpDraw)){
				(*it)->m_lpDIBDraw = NULL;
			}
			it++;
		}
	}
	m_oCriticalSection.Leave();
}
CDIBDraw* CAppManager::GetMyDIBDraw(){
	CAppManageList::iterator it = m_alpInfo.begin();
	DWORD dwThreadID = ::GetCurrentThreadId();

	CDIBDraw* lpDraw = NULL;
	m_oCriticalSection.Enter();
	{
		while (it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID1 == dwThreadID /* ||
				(*it)->m_dwThreadID2 == dwThreadID */ ) {
//	�}���`�X���b�h��߂܂����O�O
				lpDraw = (*it)->m_lpDIBDraw;
				break;
			}
			it++;
		}
	}
	m_oCriticalSection.Leave();
	return lpDraw;		//	return null if it was not found...
}
#endif

#ifdef USE_SAVER
void CAppManager::Add(CSaverDraw* lpSaverDraw){

	DWORD	dwThreadID = ::GetCurrentThreadId();

	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID1 == dwThreadID /* ||
				(*it)->m_dwThreadID2 == dwThreadID */ ) { (*it)->m_lpSaverDraw = lpSaverDraw; goto Exit; }
//	�}���`�X���b�h��߂܂����O�O
			it++;
		}
		//	��v����X���b�h�������Ƃ͂ǂ��䂤��������H
		//	�iCDIBDraw�̃R���X�g���N�^���ςȂƂ���Ŏ��s���ꂽ�H�j
		Err.Out("CAppManager::Add����̂ɋ󂫂�����");
Exit:;
	}
	m_oCriticalSection.Leave();

}
void CAppManager::Del(CSaverDraw*lpDraw) {

	DWORD	dwThreadID = ::GetCurrentThreadId();

	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		while(it!=m_alpInfo.end()) {
			if (((*it)->m_dwThreadID1 == dwThreadID) && ((*it)->m_lpSaverDraw == lpDraw)){
				(*it)->m_lpSaverDraw = NULL;
			}
			it++;
		}
	}
	m_oCriticalSection.Leave();
}
CSaverDraw* CAppManager::GetMySaverDraw(){
	CAppManageList::iterator it = m_alpInfo.begin();
	DWORD dwThreadID = ::GetCurrentThreadId();

	CSaverDraw* lpDraw = NULL;
	m_oCriticalSection.Enter();
	{
		while (it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID1 == dwThreadID /* ||
				(*it)->m_dwThreadID2 == dwThreadID */ ) {
//	�}���`�X���b�h��߂܂����O�O
				lpDraw = (*it)->m_lpSaverDraw;
				break;
			}
			it++;
		}
	}
	m_oCriticalSection.Leave();
	return lpDraw;		//	return null if it was not found...
}
#endif

CAppBase* CAppManager::GetMyApp(){
	CAppBase* app = NULL;
	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		DWORD dwThreadID = ::GetCurrentThreadId();
		while (it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID1 == dwThreadID /* ||
				(*it)->m_dwThreadID2 == dwThreadID */ ) {
//	�}���`�X���b�h��߂܂����O�O
				app = (*it)->m_lpAppBase;
				break;
			}
			it++;
		}
	}
	m_oCriticalSection.Leave();
	return app;		//	return null if it was not found...
}

CAppFrame* CAppManager::GetMyFrame(){
	CAppFrame* app = NULL;
	m_oCriticalSection.Enter();
	{
		CAppManageList::iterator it = m_alpInfo.begin();
		DWORD dwThreadID = ::GetCurrentThreadId();
		while (it!=m_alpInfo.end()) {
			if ((*it)->m_dwThreadID1 == dwThreadID /* ||
				(*it)->m_dwThreadID2 == dwThreadID */ ) {
//	�}���`�X���b�h��߂܂����O�O
				app = (*it)->m_lpAppFrame;
				break;
			}
			it++;
		}
	}
	m_oCriticalSection.Leave();
	return app;		//	return null if it was not found...
}


//////////////////////////////////////////////////////////////////////////////

bool	CAppManager::IsDirectDraw() {

#ifdef USE_DirectDraw
	CDirectDraw*lp = CAppManager::GetMyDirectDraw();
	if (lp!=NULL) {
		return true;
	}
#endif

#ifdef USE_DIB32
	{
		CDIBDraw*lp = CAppManager::GetMyDIBDraw();
		if (lp!=NULL) {
			return false;
		}
	}
#endif

#ifdef USE_FastDraw
	{
		CFastDraw*lp = CAppManager::GetMyFastDraw();
		if (lp!=NULL) {
			return false;
		}
	}
#endif

#ifdef USE_SAVER
	{
		CSaverDraw*lp = CAppManager::GetMySaverDraw();
		if (lp!=NULL) {
			return false;
		} else {
			WARNING(true,"CAppManager::IsDirectDraw��CPlane��CDIB32�ƁA�ǂ��炪�L���Ȃ̂����킩��Ȃ�");
			return false;
		}
	}
#endif

	WARNING(true,"CAppManager::IsDirectDraw��CPlane��CDIB32�ƁA�ǂ��炪�L���Ȃ̂����킩��Ȃ�");
	return false;
}

int	CAppManager::GetDrawType() {

#ifdef USE_DirectDraw
	CDirectDraw*lp = CAppManager::GetMyDirectDraw();
	if (lp!=NULL) {
		return 1;
	}
#endif

#ifdef USE_DIB32
	{
		CDIBDraw*lp = CAppManager::GetMyDIBDraw();
		if (lp!=NULL) {
			return 2;
		}
	}
#endif

#ifdef USE_FastDraw
	{
		CFastDraw*lp = CAppManager::GetMyFastDraw();
		if (lp!=NULL) {
			return 3;
		}
	}
#endif

#ifdef USE_SAVER
	{
		CSaverDraw*lp = CAppManager::GetMySaverDraw();
		if (lp!=NULL) {
			return 4;
		}
	}
#endif

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

int	CAppManager::GetAppInstanceNum(){
	int n;
	m_oCriticalSection.Enter();
	{
		n = m_alpInfo.size();
	}
	m_oCriticalSection.Leave();
	return n;
}

void CAppManager::StopAllThread(){
	
	//	�܂��́A�S�X���b�h�ɒ�~�M���𑗂�
	CAppManageList::iterator it;
	m_oCriticalSection.Enter();
	{
		it = m_alpInfo.begin();
		while (it!=m_alpInfo.end()) {
			static_cast<CThread*>((*it)->m_lpAppBase)->InvalidateThread();
			it++;
		}
	}
	m_oCriticalSection.Leave();

#if 0
	int nThreadNum;
	while (true) {		//	�S�X���b�h�����s���~�����̂�
		nThreadNum = 0;
//	�}���`�X���b�h��߂܂����O�O
		m_oCriticalSection.Enter();
		{
		/*
			it = m_alpInfo.begin();
			while (it!=m_alpInfo.end()) {
				if (static_cast<CThread*>((*it)->m_lpAppBase)->IsThreadExecute()) nThreadNum++;
				if (static_cast<CThread*>((*it)->m_lpAppBase)->IsMessage()) nThreadNum++;
				it++;
			}
		*/
			nThreadNum = m_alpInfo.size();

		}
		m_oCriticalSection.Leave();

		//	�����̏������̃��b�Z�[�W�X���b�h�������̂ŁA
		//	��������������ɂȂ邱�Ƃ��I�������ł���B
		//	�˃}���`�X���b�h��߂��̂ŁA�O�ɂȂ邱�Ƃ��I�������I

		if (nThreadNum == 0) break;
		::Sleep(100);
	}
#endif
	//	�����̃R�[�h��߂āACAppBase�̃C���X�^���X���ЂƂł��邱�Ƃ�ۏ؂����ق���
	//	�ǂ��Ǝv���B
	//	CAppBase�h���N���X�̃C���X�^���X�ɂ����ẮA
	//	���N���X�ł���CAppBase�̃f�X�g���N�^�����ƂŌĂяo�����̂�
	//	CAppBase�̃f�X�g���N�^��CAppManager::Dec�����Ƃ��́A
	//	���̔h���N���X�̃f�X�g���N�^�͏I�����Ă��邱�Ƃ��ۏ؂����B
	while (true){
		if (CAppManager::GetRef() <= 1) break;
		::Sleep(100);
	}
	//	���S�X���b�h����~�����̂��m�F���Ă���
}