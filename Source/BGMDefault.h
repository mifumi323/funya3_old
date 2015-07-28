// BGMDefault.h: CBGMDefault クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BGMDEFAULT_H__62B14241_0790_4905_A83F_5ED7D3873447__INCLUDED_)
#define AFX_BGMDEFAULT_H__62B14241_0790_4905_A83F_5ED7D3873447__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BGMBase.h"

class Cf3MIDIOutput;
class CDirectMusicFader;

enum MIDINumber {
	MIDI_RESET,
	MIDI_PLAY,
	MIDI_GEASPRIN,
	MIDI_CLEAR,
	MIDI_MISS,
	MIDI_EXPLAIN,
	MIDI_SIZE,				// この番号のBGMは存在しない。配列のサイズを決めるため使う
};

class CBGMDefault : public CBGMBase  
{
protected:
	void InnerStop();
	void InnerPlay(BGMNumber no);
	LRESULT Load(int no, string filename, bool secondary=false, DWORD start=0, DWORD end=0, DWORD option=0);
	Cf3MIDIOutput	**m_pMIDI;
	CDirectMusicFader	*m_pFader;
	int				m_MIDINo;
public:
	void Update();
	void MusicEffect(MENumber no, float param=0.0f);
	bool Init();
	string GetClassName();
	CBGMDefault();
	virtual ~CBGMDefault();

};

#endif // !defined(AFX_BGMDEFAULT_H__62B14241_0790_4905_A83F_5ED7D3873447__INCLUDED_)
