// f3MIDIOutput.h: Cf3MIDIOutput クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MIDIOUTPUT_H__ABBE2765_E40F_48E2_A8BD_6ED1F1153079__INCLUDED_)
#define AFX_F3MIDIOUTPUT_H__ABBE2765_E40F_48E2_A8BD_6ED1F1153079__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "YANESDK\yaneMIDIOutputDM.h"
#include "ControlCheckTool.h"

class Cf3MIDIOutput : public CMIDIOutputDM
{
protected:
	bool	m_bSecondary;
	DWORD	m_Option;
public:
	void SetPlayOption(DWORD option);
	LRESULT Open(string pFileName, bool secondary);
	LRESULT SetLoopPos(DWORD start, DWORD end);
	LRESULT Play(void);
	LRESULT SetSecondary();
	Cf3MIDIOutput();
	virtual ~Cf3MIDIOutput();

};

#endif // !defined(AFX_F3MIDIOUTPUT_H__ABBE2765_E40F_48E2_A8BD_6ED1F1153079__INCLUDED_)
