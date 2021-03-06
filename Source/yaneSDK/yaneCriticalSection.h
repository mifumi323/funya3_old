//	yaneCriticalSection.h :
//		programmed by yaneurao	'00/03/02

#ifndef __yaneCriticalSection_h__
#define __yaneCriticalSection_h__

class CCriticalSection {
public:
	void	Enter(void);	//	CriticalSectionに入る
	void	Leave(void);	//	CriticalSectionから出る


	CCriticalSection(void);
	virtual ~CCriticalSection();
protected:
	CRITICAL_SECTION	m_oCriticalSection;
};

#endif