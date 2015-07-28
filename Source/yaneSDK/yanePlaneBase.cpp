#include "stdafx.h"
#include "yanePlaneBase.h"
#include "yanePlane.h"
#include "yaneDIB32.h"
#include "yaneAppManager.h"

smart_ptr<CPlaneBaseFactory> CPlaneBase::m_pvPlaneFactory;

CPlaneBase* CPlaneBase::CreatePlane(void){
//	�����֐��B����CDirectDraw���g���Ă���̂�CDIBDraw���g���Ă���̂��ɉ�����
//	CPlane,CDIB32,CFastPlane�𐶐�����B

	//	����Factory���ݒ肳��Ă���΁A����Ő�������
	if (m_pvPlaneFactory!=NULL)
		return m_pvPlaneFactory->Create();

	switch(CAppManager::GetDrawType()){
#ifdef USE_DirectDraw
	case 1: return new CPlane;
#endif

#ifdef USE_DIB32
	case 2: return new CDIB32;
#endif

#ifdef USE_FastDraw
	case 3: return new CFastPlane;
#endif
	}
	return NULL; // error..
}

LRESULT CPlaneBase::BltNaturalPos(CPlaneBase* lpSrc,int x,int y,int nMode,int nFade){
	//	nMode == �x�[�X�ʒu(0:�摜���S 1:���� 2:�E�� 3:���� 4:�E��)
	if (lpSrc==NULL) return -1;

	//	�T�C�Y�擾
	int sx,sy;
	lpSrc->GetSize(sx,sy);

	switch (nMode){	//	�t�F�[�h���x��
	case 0: return BltNatural(lpSrc,x-(sx>>1),y-(sy>>1)	,nFade);
	case 1: return BltNatural(lpSrc,x	,y				,nFade);
	case 2: return BltNatural(lpSrc,x-sx,y				,nFade);
	case 3: return BltNatural(lpSrc,x	,y-sy			,nFade);
	case 4: return BltNatural(lpSrc,x-sx,y-sy			,nFade);
	}
	return -2;	//	�Ȃ��HnMode�͈͊O
}

///////////////////////////////////////////////////////////////////////
//	�T�[�t�F�[�X�̃o�b�N�A�b�v���쐬���ĕԂ�
smart_ptr<CPlaneBase> CPlaneBase::GetBackup(void){
	smart_ptr<CPlaneBase> vPlane(CreatePlane(),true);
	int sx,sy;
	GetSize(sx,sy);
	vPlane->CreateSurface(sx,sy);
	vPlane->BltFast(this,0,0);
	return vPlane;
}

///////////////////////////////////////////////////////////////////////
#ifdef USE_DirectDraw
CPlaneBase* CPlaneBaseFactoryForCPlane::Create() { return new CPlane; }
#endif

#ifdef USE_FastDraw
CPlaneBase* CPlaneBaseFactoryForCFastPlane::Create() { return new CFastPlane; }
#endif

#ifdef USE_DIB32
CPlaneBase* CPlaneBaseFactoryForCDIB32::Create() { return new CDIB32; }
#endif