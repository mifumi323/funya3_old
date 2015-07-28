// AVIRecorder.h: CAVIRecorder クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVIRECORDER_H__34EC9CF5_A98D_4E99_9B8E_C51B10F312EF__INCLUDED_)
#define AFX_AVIRECORDER_H__34EC9CF5_A98D_4E99_9B8E_C51B10F312EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vfw.h>
#include "yaneSDK/yaneDIB32.h"

#pragma comment(lib,"vfw32.lib")

class CAVIRecorder  
{
public:
	void Write(CDIB32* lpDIB);
	CAVIRecorder();
	virtual ~CAVIRecorder();

protected:
	bool m_bInitialized;
	CDIB32 m_DIB;

	AVISTREAMINFO si;
	AVICOMPRESSOPTIONS opt;
	COMPVARS cv;
	PAVIFILE pavi;
	PAVISTREAM pstm,ptmp;
};

#endif // !defined(AFX_AVIRECORDER_H__34EC9CF5_A98D_4E99_9B8E_C51B10F312EF__INCLUDED_)
