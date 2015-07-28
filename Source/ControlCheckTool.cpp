// ControlCheckTool.cpp: CControlCheckTool クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ControlCheckTool.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CControlCheckTool::CControlCheckTool()
{
	m_cRef = 1;
	InitializeCriticalSection(&m_CrSec);
	m_pListener = NULL;
}

CControlCheckTool::~CControlCheckTool()
{
	DeleteCriticalSection(&m_CrSec);
}

STDMETHODIMP CControlCheckTool::QueryInterface(const IID &iid, void **ppv)
{
	if (iid == IID_IUnknown || iid == IID_IDirectMusicTool)
	{
		*ppv = static_cast<IDirectMusicTool*>(this);
	} else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(this)->AddRef();
	return S_OK;
}
 
STDMETHODIMP_(ULONG) CControlCheckTool::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}
 
STDMETHODIMP_(ULONG) CControlCheckTool::Release()
{
    if( 0 == InterlockedDecrement(&m_cRef) )
    {
        delete this;
        return 0;
    }
    return m_cRef;
}

HRESULT STDMETHODCALLTYPE CControlCheckTool::GetMsgDeliveryType(DWORD* pdwDeliveryType )
{
	*pdwDeliveryType = DMUS_PMSGF_TOOL_IMMEDIATE;
	return S_OK;
}
 
HRESULT STDMETHODCALLTYPE CControlCheckTool::GetMediaTypeArraySize(DWORD* pdwNumElements)
{
	*pdwNumElements = 1;
	return S_OK;
}
 
HRESULT STDMETHODCALLTYPE CControlCheckTool::GetMediaTypes(DWORD** padwMediaTypes, DWORD dwNumElements)
{
	if (dwNumElements == 1) {
		(*padwMediaTypes)[0] = DMUS_PMSGT_MIDI;
		return S_OK;
	}else {
		return E_FAIL;
	}
}

HRESULT STDMETHODCALLTYPE CControlCheckTool::ProcessPMsg(IDirectMusicPerformance* pPerf, DMUS_PMSG* pMsg)
{
	if ((NULL == pMsg->pGraph) || FAILED(pMsg->pGraph->StampPMsg(pMsg))) return DMUS_S_FREE;
	DMUS_MIDI_PMSG* pMidi = (DMUS_MIDI_PMSG*)pMsg;

	EnterCriticalSection(&m_CrSec);
	if (m_pListener!=NULL && (pMidi->bStatus & 0xf0)==0xb0) {
		// コントロールチェンジである
		BYTE channel = pMidi->dwPChannel & 0x0f;
		switch(pMidi->bByte1) {
		case 0x07: m_pListener->OnVolumeChange    (channel, pMidi->bByte2); break;
		case 0x0a: m_pListener->OnPanChange       (channel, pMidi->bByte2); break;
		case 0x0b: m_pListener->OnExpressionChange(channel, pMidi->bByte2); break;
		}
	}
	LeaveCriticalSection(&m_CrSec);

	return DMUS_S_REQUEUE;
}

HRESULT STDMETHODCALLTYPE CControlCheckTool::Init(IDirectMusicGraph* pGraph)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CControlCheckTool::Flush(IDirectMusicPerformance* pPerf,DMUS_PMSG* pDMUS_PMSG,REFERENCE_TIME rtTime)
{
	return E_NOTIMPL;
}

void CControlCheckTool::SetListener(CMidiParamListener* pListener)
{
	EnterCriticalSection(&m_CrSec);
	m_pListener = pListener;
	LeaveCriticalSection(&m_CrSec);
}
