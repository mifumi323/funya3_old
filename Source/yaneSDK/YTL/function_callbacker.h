// FunctionCallbacker.h

#ifndef __YTLFunctionCallbacker_h__
#define __YTLFunctionCallbacker_h__

class CFunctionCallbacker {
public:
	virtual LRESULT Call(DWORD dwParam) { return 0; }

	virtual ~CFunctionCallbacker(){}
};

//	�����o�֐��|�C���^�̃T�C�Y�̓N���X�ˑ��Ȃ̂ŁA�ėp�^�̂悤�Ȃ��̂�
//	���݂��Ȃ��B�����ŁA���̂悤�Ȃ��Ƃ����Ȃ��Ă͂Ȃ�Ȃ��Ȃ�B
template <class T>
class CTFunctionCallbacker : public CFunctionCallbacker {
public:
	CTFunctionCallbacker(void){
		m_pThis = NULL;
		m_pFunc = NULL;
	}

	virtual LRESULT Call(DWORD dwParam) {
		if (m_pThis) return (m_pThis->*m_pFunc)(dwParam);
		return 0;
	}
	virtual void SetFunction(T* pThis,LRESULT (T::*pFunc)(DWORD)){
		m_pThis = pThis;
		m_pFunc = pFunc;
	}
	//	���̋t�ϊ�
	virtual void GetFunction(T*&pThis,LRESULT (T::*&pFunc)(DWORD)){
		pThis = m_pThis;
		pFunc = m_pFunc;
	}
private:
	T* m_pThis;
	LRESULT (T::*m_pFunc)(DWORD);
};

#endif