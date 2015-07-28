#ifndef __YTLAutoPtrEx_h__
#define __YTLAutoPtrEx_h__

template<class T> class auto_ptrEx {
public:
	typedef T element_type;
	explicit auto_ptrEx(T *_P = 0)
		: m_bOwner(_P != 0), m_lp(_P) {}
	auto_ptrEx(const auto_ptrEx<T>& _Y) {
		SetOwner(_Y.IsOwner());
		const_cast<auto_ptrEx<T>*>(&_Y)->SetOwner(false);
		m_lp = _Y.m_lp;
	}
	auto_ptrEx<T>& operator=(const auto_ptrEx<T>& _Y) {
		if (this != &_Y) {
			if (m_lp != _Y.m_lp) {
				if (IsOwner()) delete m_lp;	//	���������L���������Ă���΂�������
				SetOwner(_Y.IsOwner());	//	������ɏ��L��������Ȃ�΁A������ړ�
				const_cast<auto_ptrEx<T>*>(&_Y)->SetOwner(false);
				m_lp = _Y.m_lp;
			} else {
				SetOwner(_Y.IsOwner());	//	������ɏ��L��������Ȃ�΁A������ړ�
				const_cast<auto_ptrEx<T>*>(&_Y)->SetOwner(false);
			}
		}
		return (*this);
	}
	~auto_ptrEx() { if (IsOwner()) delete m_lp;	}
	T& operator*() const  {return *get(); }
	T* operator->() const {return get();  }
	T* get() const { return m_lp; }
	T* release() const {
		//	���́��L���X�g�����Ȃ���this��const����������̂�m_bOwner�ɃA�N�Z�X�ł��Ȃ�(fake this)
		((auto_ptrEx<T> *)this)->m_bOwner = false;
		T* tmp = m_lp;
		((auto_ptrEx<T> *)this)->m_lp	  = NULL;
		return tmp;
	}

	////////////////////////////////////////////////////////
	//	auto_ptrEx�I���W�i��

	//	���L���̔j���ƃI�u�W�F�N�g�̉���\��
	void Delete(){
		if (IsOwner()) {
			delete release();	//	fixed '00/11/29
		} else {
			((auto_ptrEx<T> *)this)->m_bOwner = false;
			((auto_ptrEx<T> *)this)->m_lp	  = NULL;
		}
	}

	//	���L�����������C���X�^���X�̒ǉ������\��
	void Add(){
		Delete();
		m_lp = new T;		//	�x������
		SetOwner(true);
	}
	void Add(T*_P){			//	�|�����[�t�B�b�N�Ȍ^�ł���
		Delete();
		if (_P) {
			m_lp	= _P;
			SetOwner(true);
		}
	}
	//	�����̂Q�̊֐����P�ɂ܂Ƃ߂��
	//	new T�̕����ŁAT �����ۃN���X���ƃR���p�C���G���[�ɂȂ�B

	auto_ptrEx<T>& operator=(const T* _P) {
		//	�E�Ӓl�̓|�C���^�ł����āA���̏��L�҂ɂ��Ă͔���Ȃ�
		if (m_lp != _P) {
			Delete();	//	����āA������A���L���͖������̂Ƃ���
			m_lp = const_cast<T*>(_P);
		}
		return (*this);
	}

	//	�I�[�i�[�ł��邩��Ԃ��֐�
	bool	IsOwner() const { return m_bOwner; }
	//	�I�[�i�[���̐ݒ�
	void	SetOwner(bool b) { m_bOwner = b; }
	//	���@������炪��������auto_ptr���炱�̃N���X��h�����Ȃ�����

	//	���������̂͂��܂�������ׂ��łȂ����A
	//	�ꉞT*�ƃR���p�`�r���e�B��ۂ������B
	operator T* () const { return get(); }

private:
	bool	m_bOwner;
	T		*m_lp;
};

#endif