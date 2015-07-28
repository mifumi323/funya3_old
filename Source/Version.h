// Version.h: CVersion クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERSION_H__1CD9C020_E7C3_4B7D_81AA_FB1F86E44359__INCLUDED_)
#define AFX_VERSION_H__1CD9C020_E7C3_4B7D_81AA_FB1F86E44359__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVersion  
{
public:
	inline LPSTR GetVersion() { return m_strVersion; }
	CVersion();
	virtual ~CVersion();
protected:
	int m_nMajor, m_nMinor;
	TCHAR m_strVersion[16];
};

#endif // !defined(AFX_VERSION_H__1CD9C020_E7C3_4B7D_81AA_FB1F86E44359__INCLUDED_)
