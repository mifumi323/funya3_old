// yaneKeyBase.h
//	 This is a based wrapper of input.
//		programmed by yaneurao(M.Isozaki) '99/11/13

#ifndef __yaneKeyBase_h__
#define __yaneKeyBase_h__

// ---------------------------------------------------------------------------
// bufferのflip! (C)yaneurao
#define FlipKeyBuffer(var) \
	var = 1 - var;
// ---------------------------------------------------------------------------

class CKeyBase {
public:
	CKeyBase(void);
	virtual ~CKeyBase();
	//////////////////////////////////////////////////////////////////
	virtual LRESULT	GetKeyState(void) = 0;	// must be override...
	virtual bool	IsKeyPress(int key) const;
	virtual bool	IsKeyPush(int key) const;
	virtual BYTE*	GetKeyData(void) const;
	//////////////////////////////////////////////////////////////////
protected:
	int		m_nKeyBufNo;				//	裏と表をflipして、差分をとるのに利用
	BYTE	m_byKeyBuffer[2][256];		//	key buffer
};

#endif