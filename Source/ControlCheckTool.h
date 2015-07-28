// ControlCheckTool.h: CControlCheckTool �N���X�̃C���^�[�t�F�C�X
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
	long m_cRef;				// �Q�ƃJ�E���^�B
	CRITICAL_SECTION m_CrSec;	// �����o�ϐ����X���b�h�Z�[�t�ɂ���B

	CMidiParamListener* m_pListener;
};

#endif // !defined(AFX_CONTROLCHECKTOOL_H__51BB6DFE_ACC4_4DFC_BEA0_5BAE64235BFA__INCLUDED_)
