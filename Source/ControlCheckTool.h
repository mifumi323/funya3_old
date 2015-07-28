// ControlCheckTool.h: CControlCheckTool クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLCHECKTOOL_H__51BB6DFE_ACC4_4DFC_BEA0_5BAE64235BFA__INCLUDED_)
#define AFX_CONTROLCHECKTOOL_H__51BB6DFE_ACC4_4DFC_BEA0_5BAE64235BFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dmusici.h>

class CMidiParamListener {
public:
	CMidiParamListener() {}
	virtual ~CMidiParamListener() {}

	virtual void OnVolumeChange(BYTE channel, BYTE value) {}
	virtual void OnPanChange(BYTE channel, BYTE value) {}
	virtual void OnExpressionChange(BYTE channel, BYTE value) {}
};

class CControlCheckTool : public IDirectMusicTool  
{
public:
	CControlCheckTool();
	virtual ~CControlCheckTool();

	void SetListener(CMidiParamListener* pListener);
public:
	// IUnknown
	virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);
	virtual STDMETHODIMP_(ULONG) AddRef();
	virtual STDMETHODIMP_(ULONG) Release();

	// IDirectMusicTool
	HRESULT STDMETHODCALLTYPE Init(IDirectMusicGraph* pGraph);
	HRESULT STDMETHODCALLTYPE GetMsgDeliveryType(DWORD* pdwDeliveryType);
	HRESULT STDMETHODCALLTYPE GetMediaTypeArraySize(DWORD* pdwNumElements);
	HRESULT STDMETHODCALLTYPE GetMediaTypes(DWORD** padwMediaTypes,DWORD dwNumElements);
	HRESULT STDMETHODCALLTYPE ProcessPMsg(IDirectMusicPerformance* pPerf,DMUS_PMSG* pDMUS_PMSG);
	HRESULT STDMETHODCALLTYPE Flush(IDirectMusicPerformance* pPerf,DMUS_PMSG* pDMUS_PMSG,REFERENCE_TIME rtTime);
private:
	long m_cRef;				// 参照カウンタ。
	CRITICAL_SECTION m_CrSec;	// メンバ変数をスレッドセーフにする。

	CMidiParamListener* m_pListener;
};

#endif // !defined(AFX_CONTROLCHECKTOOL_H__51BB6DFE_ACC4_4DFC_BEA0_5BAE64235BFA__INCLUDED_)
