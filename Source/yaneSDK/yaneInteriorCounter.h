//	CInteriorCounter
//		�����J�E���^

#ifndef __yaneInteriorCounter_h__
#define __yaneInteriorCounter_h__

class CInteriorCounter {
public:
	CInteriorCounter();

	//	int�Ƃ̑��ݕϊ�
	operator int () { return m_nNow; }
	const int operator = (int n) { m_nNow = m_nStart = n; m_nFramesNow = 0; return n; }

	void	Inc();		//	���Z
	CInteriorCounter& operator++() { Inc(); return (*this); }
	CInteriorCounter operator++(int) { CInteriorCounter _Tmp = *this; Inc(); return (_Tmp); }

	void	Dec();		//	���Z
	CInteriorCounter& operator--() { Dec(); return (*this); }
	CInteriorCounter operator--(int) { CInteriorCounter _Tmp = *this; Dec(); return (_Tmp); }

	void	Set(int nStart,int nEnd,int nFrames);
	void	Set(int nNow) { *this = nNow; }

	int		GetEnd(){ return m_nEnd;}
	bool	IsEnd(){ return (m_nNow == m_nEnd);}

private:
	int		m_nNow;			//	���݂̒l
	int		m_nStart;		//	�����l
	int		m_nEnd;			//	�I���l
	int		m_nFrames;		//	�t���[���������i�I���l�ɂȂ�܂ŉ���Inc������΂����̂��j
	int		m_nFramesNow;	//	���݁A���t���[���ڂ��H
};

#endif