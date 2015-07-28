// BGMBase.h: CBGMBase クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BGMBASE_H__4DFF022A_6698_4E43_9138_2DB45DC37368__INCLUDED_)
#define AFX_BGMBASE_H__4DFF022A_6698_4E43_9138_2DB45DC37368__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum BGMMode {
	BGMM_NONE,
	BGMM_DEFAULT,
	BGMM_USER,
};

enum BGMNumber {
	BGMN_SIRENT,
	BGMN_TITLE,
	BGMN_GAMEFUNYA,
	BGMN_GAMEBANANA,
	BGMN_GAMENEEDLE,
	BGMN_GAMEGEASPRIN,
	BGMN_GAMEWIND,
	BGMN_GAMEICE,
	BGMN_GAMEFIRE,
	BGMN_GAMEMRFRAME,
	BGMN_GAMEEELPITCHER,
	BGMN_CLEAR,
	BGMN_ENDING,
	BGMN_MISS,
	BGMN_GAMEOVER,
	BGMN_EXPLAIN,
	BGMN_SIZE,				// この番号のBGMは存在しない。配列のサイズを決めるため使う
};

enum MENumber {
	MEN_SLEEP,
	MEN_AWAKE,
	MEN_GEASPRIN,
	MEN_BANANADISTANCE,
	MEN_BANANAPOSITION,
	MEN_SIZE,				// この番号のBGMは存在しない。配列のサイズを決めるため使う
};

class CBGMBase  
{
protected:
	BGMNumber	m_PlayNo;
	virtual void InnerPlay(BGMNumber no) {}
	virtual void InnerStop() {}
public:
	virtual void MusicEffect(MENumber no, float param=0.0f) {}
	virtual void Update() {}
	virtual string GetClassName();
	BGMNumber GetBGM();
	void Stop();
	void Play(BGMNumber no);
	virtual bool Init();
	CBGMBase();
	virtual ~CBGMBase();

};

#endif // !defined(AFX_BGMBASE_H__4DFF022A_6698_4E43_9138_2DB45DC37368__INCLUDED_)
