// f3StageFile.h: Cf3StageFile クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3STAGEFILE_H__3ACC1873_531E_4731_8140_8ABE157984FF__INCLUDED_)
#define AFX_F3STAGEFILE_H__3ACC1873_531E_4731_8140_8ABE157984FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct tagf3StageHeader {
	char	ident[8];		// "funya3s1"
	long	datasize;		// 展開サイズ
	long	packsize;		// 圧縮サイズ(datasizeと同じ場合無圧縮)
};

enum {
	CT_TITL=0x4C544954,
	CT_AUTH=0x48545541,
	CT_DESC=0x43534544,
	CT_ENDM=0x4D444E45,
	CT_STGC=0x43475453,
	CT_REST=0x54534552,
	CT_PNLT=0x544C4E50,
	CT_HITS=0x53544948,
	CT_TL00=0x30304C54,
	CT_M000=0x3030304D,
	CT_M100=0x3030314D,
	CT_M200=0x3030324D,
	CT_MCF0=0x3046434D,
	CT_MCD0=0x3044434D,

	CT_RPLY=0x594C5052,
	CT_STGN=0x4E475453,
	CT_GRVT=0x54565247,
	CT_HYPR=0x52505948,
};

class Cf3StageFile  
{
protected:
	void ClearData();
	void AnalyzeData(BYTE* data);
	tagf3StageHeader	m_StageHeader;
	map<DWORD,HGLOBAL>	m_Data;
public:
	LRESULT Write(string filename);
	void SetStageData(DWORD dwType, DWORD dwSize, void* lpData);
	BYTE* GetStageData(const DWORD dwType, DWORD*dwSize=NULL);
	LRESULT Read(string filename);
	Cf3StageFile();
	virtual ~Cf3StageFile();

};

#endif // !defined(AFX_F3STAGEFILE_H__3ACC1873_531E_4731_8140_8ABE157984FF__INCLUDED_)
