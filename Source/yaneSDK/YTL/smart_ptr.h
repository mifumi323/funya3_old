#ifndef __YTLSmartPtr_h__
#define __YTLSmartPtr_h__

//	�x�[�X�ƂȂ�I�u�W�F�N�g
//	�i�ǂ�ȃX�}�[�g�|�C���^�������ɕϊ��ł���j
class smart_obj {
public:
	virtual ~smart_obj() {};
};

template<class T> class smart_ptr : public smart_obj {
public:
	smart_ptr(T *_P = NULL,bool bOwner = false)
		:  m_lp(_P) {
			if (bOwner) {
				m_lpPrev = m_lpNext = this;
			} else {
				m_lpPrev = m_lpNext = NULL;
			}
		}
	//	�f�B�t�H���g�ł͏��L���������Ȃ�
	//	�iauto_ptr�Ƃ̈Ⴂ�ɒ��ӁI�j

	smart_ptr(const smart_ptr<T>& _Y) {
		//	���L�������Ƃ�̂��H
		if (_Y.m_lpPrev!=NULL) {
			//	���L���`�F�C���ɒǉ��B
			m_lpPrev = const_cast<smart_ptr<T>*>(&_Y);
			m_lpNext = const_cast<smart_ptr<T>*>(&_Y)->m_lpNext;
			const_cast<smart_ptr<T>*>(&_Y)->m_lpNext->m_lpPrev = this;
			const_cast<smart_ptr<T>*>(&_Y)->m_lpNext = this;
		} else {
			m_lpPrev = NULL;
			m_lpNext = NULL;
		}
		m_lp = _Y.m_lp;
	}
	smart_ptr<T>& operator=(const smart_ptr<T>& _Y) {
		if (this != &_Y) {
			Delete();
			//	���L�������Ƃ�̂��H
			if (_Y.m_lpPrev!=NULL) {
				//	���L���`�F�C���ɒǉ��B
				m_lpPrev = const_cast<smart_ptr<T>*>(&_Y);
				m_lpNext = const_cast<smart_ptr<T>*>(&_Y)->m_lpNext;
				const_cast<smart_ptr<T>*>(&_Y)->m_lpNext->m_lpPrev = this;
				const_cast<smart_ptr<T>*>(&_Y)->m_lpNext = this;
			}
			m_lp = _Y.m_lp;
		}
		return (*this);
	}
	smart_ptr<T>& operator=(const T* _P) {
		//	�E�Ӓl�̓|�C���^�ł����āA���̏��L�҂ɂ��Ă͔���Ȃ�

		//	����|�C���^���ǂ����̃`�F�b�N�͂��Ȃ����c
		Delete();
		m_lp = const_cast<T*>(_P);
		m_lpPrev = m_lpNext = NULL;
		return (*this);
	}

	virtual ~smart_ptr() { Delete(); }
	T& operator*() const  {return *get(); }
	T* operator->() const {return get();  }
	T* get() const { return m_lp; }

	//	�i�ʗ�A�����I�ȃ����[�X�͍s�Ȃ�Ȃ����j
	//	����͊֘A����smart_ptr���珊�L���𔍒D���AT*��Ԃ�
	T* release() {
		//	���ׂẴ`�F�C�����珊�L���𔍒D����
		smart_ptr<T>* lpNext = m_lpNext;
		while (lpNext!=NULL) {
			lpNext->m_lpPrev = NULL;
			smart_ptr<T>* tmp = lpNext->m_lpNext;
			lpNext->m_lpNext = NULL;
			if (lpNext==this) break;
			lpNext = tmp;
		}
		return m_lp;
	}

	//	�i�ʗ�A�����I�ȉ���͍s�Ȃ�Ȃ����j
	//�@������́A�����̏��L���݂̂��������
	virtual void Delete() {
		//	�N�������L���������Ă���̂��H
		if (m_lpPrev) {
#ifdef _DEBUG
			if ( m_lpPrev->m_lp == (LPVOID)0xdddddddd ){
				WARNING("true","����ς݂̃`�F�C���ɃA�N�Z�X");
				m_lpPrev = m_lpNext = this;
			}
#endif
			//	���L�������Ă�̉��������H
			if (m_lpPrev==this) {
				delete m_lp;
			} else {
				//	���L���`�F�C������؂藣��
				m_lpPrev->m_lpNext = m_lpNext;
				m_lpNext->m_lpPrev = m_lpPrev;
			}
			m_lpPrev = m_lpNext = NULL;
		}
		m_lp = NULL;
	}

	//	���L�����������C���X�^���X�̒ǉ������\��
	void Add(){
		Delete();
		m_lp = new T;		//	�x������
		m_lpPrev = m_lpNext = this;
	}
	//	���L�����������C���X�^���X�̒ǉ������\��
	void Add(T*_P){			//	�|�����[�t�B�b�N�Ȍ^�ł���
		Delete();
		if (_P!=NULL) {
			m_lp	= _P;
			m_lpPrev = m_lpNext = this;
		}
	}
	//	�����̂Q�̊֐����P�ɂ܂Ƃ߂��
	//	new T�̕����ŁAT �����ۃN���X���ƃR���p�C���G���[�ɂȂ�B

	//	�I�[�i�[�ł��邩��Ԃ��֐�
	bool	IsOwner() const { return m_lpPrev!=NULL; }
	//	�������A�\����A���Ƃ���I�[�i�[����ݒ肷�邱�Ƃ͏o���Ȃ�

	//	���������̂͂��܂�������ׂ��łȂ����A
	//	�ꉞT*�ƃR���p�`�r���e�B��ۂ������B
	operator T* () const { return get(); }

	//�@�������yaneSDK3rd�Ǝ����Ă�����
	bool	isNull() const { return get()==NULL; }
/*
	//	smart_ptr�Ԃ̔�r�\��(���ۂɎw���Ă���I�u�W�F�N�g���r����)

	bool operator==(const smart_ptr<T>& _X) const
		{return (m_lp == _X.m_lp); }
	bool operator!=(const smart_ptr<T>& _X) const
		{return (!(*this == _X)); }
	bool operator<(const smart_ptr<T>&  _X) const
		{return (m_lp < _X.m_lp);}
	bool operator>(const smart_ptr<T>&  _X) const
		{return (_X < *this); }
	bool operator<=(const smart_ptr<T>& _X) const
		{return (!(_X < *this)); }
	bool operator>=(const smart_ptr<T>& _X) const
		{return (!(*this < _X)); }

	//	�R���X�g���N�^��explicit�ɂ��Ă��Ȃ��̂ŁA
	//	�Öق̕ϊ��ɂ��operator==���B���ɂȂ�Ȃ��悤��
	bool operator==(const T* lp) const
		{return (m_lp == lp); }
	bool operator!=(const T* lp) const
		{return (!(*this == lp)); }
	bool operator<(const T*  lp) const
		{return (m_lp < lp);}
	bool operator>(const T* lp) const
		{return (lp < *this); }
	bool operator<=(const T* lp) const
		{return (!(lp < *this)); }
	bool operator>=(const T* lp) const
		{return (!(*this < lp)); }
*/

protected:
	T		*m_lp;

	//	�`�F�C���i�o�������X�g�j�������Ƃŏ��L���𖾂炩�ɂ���
	//	�iNULL�Ȃ�΁A���L���������Ă��Ȃ��j
	smart_ptr<T>* m_lpPrev;
	smart_ptr<T>* m_lpNext;
};

#endif