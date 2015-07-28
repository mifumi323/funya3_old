// f3Replay.h: Cf3Replay �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3REPLAY_H__2299CA74_A6DF_4FEC_ABA5_3AA7A0F1D82E__INCLUDED_)
#define AFX_F3REPLAY_H__2299CA74_A6DF_4FEC_ABA5_3AA7A0F1D82E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3Setting.h"
#include "f3Map.h"
#include "f3StageFile.h"

enum {
	REPLAYBUFFER = 4096,
};

class Cf3Replay  
{
protected:
	class CKeyState {
	public:
		CKeyState() { ZERO(pushed); ZERO(pressed); }
		// �������Ă�����x�܂Ƃ߂��ق������x�I�ɂ��������I�ɂ���������낵��
		BYTE pushed[REPLAYBUFFER], pressed[REPLAYBUFFER];
	};
	list<CKeyState> m_State;
	list<CKeyState>::iterator m_iPointer;
	int m_nPointer;
	int m_nProgress;
	int m_nSize;
	class Cf3ReplayPlayerState {
	public:
		Cf3ReplayPlayerState() {
			stage=NULL;
			map=NULL;
			oldgravity=theSetting->m_Gravity;
			oldhyper=theSetting->m_Hyper;
		}
		virtual ~Cf3ReplayPlayerState() {
			DELETE_SAFE(stage);
			DELETE_SAFE(map);
			theSetting->m_Gravity=oldgravity;
			theSetting->m_Hyper=oldhyper;
		}
		Cf3StageFile*	stage;
		Cf3Map*			map;
		string			stagetitle;
		string			maptitle;
		long			oldgravity;
		long			oldhyper;
	}*m_pPlayerState;
	string m_FileName;

public:
	// ����
	string GetFileName() { return m_FileName; }
	bool Finished() { return m_nProgress>=m_nSize; }
	void Reset();
	void Seek(int position=0);
	void Progress();
	Cf3Replay();
	virtual ~Cf3Replay();

	// Recorder
	void Save(Cf3StageFile* stage, int map);
	void StartRecording();
	void Record();

	// Player
	void Load(const string& filename);
	void Replay();
	void OnDraw(CDIB32 *lp) { m_pPlayerState->map->OnDraw(lp); }
	Cf3Map* GetMap() { return m_pPlayerState->map; }

};

#endif // !defined(AFX_F3REPLAY_H__2299CA74_A6DF_4FEC_ABA5_3AA7A0F1D82E__INCLUDED_)
