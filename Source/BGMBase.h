// BGMBase.h: CBGMBase �N���X�̃C���^�[�t�F�C�X
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
	BGMN_SIZE,				// ���̔ԍ���BGM�͑��݂��Ȃ��B�z��̃T�C�Y�����߂邽�ߎg��
};

enum MENumber {
	MEN_SLEEP,
	MEN_AWAKE,
	MEN_GEASPRIN,
	MEN_BANANADISTANCE,
	MEN_BANANAPOSITION,
	MEN_SIZE,				// ���̔ԍ���BGM�͑��݂��Ȃ��B�z��̃T�C�Y�����߂邽�ߎg��
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
