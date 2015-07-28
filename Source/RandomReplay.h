// RandomReplay.h: CRandomReplay クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RANDOMREPLAY_H__D7125476_00FE_4852_9139_73BF1A251DE1__INCLUDED_)
#define AFX_RANDOMREPLAY_H__D7125476_00FE_4852_9139_73BF1A251DE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDIB32;
class Cf3Replay;
class TTextNavi;

extern class CRandomReplay  
{
public:
	void Replay(CDIB32 *lp, TTextNavi *text=NULL);
	void Leave();
	void Enter();
	CRandomReplay();
	virtual ~CRandomReplay();

protected:
	HRESULT LoadNextReplay();
	bool m_bValid;
	int m_Phase;

	Cf3Replay* m_pPlayer;

} f3Replay;

#endif // !defined(AFX_RANDOMREPLAY_H__D7125476_00FE_4852_9139_73BF1A251DE1__INCLUDED_)
