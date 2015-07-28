#ifndef __YTLSmartArray_h__
#define __YTLSmartArray_h__

#include "yaneMacro.h"

//	�x�[�X�ƂȂ�I�u�W�F�N�g
//	�i�ǂ�ȃX�}�[�g�|�C���^�������ɕϊ��ł���j
class smart_aobj {
public:
	virtual ~smart_aobj() {};
};

template<class T> class smart_array : public smart_aobj {
public:
	template <class S>
	smart_array(S *_P,int nMax=0,bool bOwner=false)
		: m_lpa(_P) , m_nSize(sizeof(S)) , m_nMax(nMax) {
		if (bOwner) {
			m_lpPrev = m_lpNext = this;
		} else {
			m_lpPrev = m_lpNext = NULL;
		}
	}
	//	�f�B�t�H���g�ł͏��L���������Ȃ�

	//	����������NULL���ƁA�����o�֐��e���v���[�g������ɓ��삵�Ȃ�
	smart_array(void)
		: m_lpa(NULL) , m_nSize(0) , m_nMax(0),
			m_lpPrev(NULL), m_lpNext(NULL){}

	smart_array(const smart_array<T>& _Y) {
		//	���L�������Ƃ�̂��H
		if (_Y.m_lpPrev!=NULL) {
			//	���L���`�F�C���ɒǉ��B
			m_lpPrev = const_cast<smart_array<T>*>(&_Y);
			m_lpNext = const_cast<smart_array<T>*>(&_Y)->m_lpNext;
			const_cast<smart_array<T>*>(&_Y)->m_lpNext->m_lpPrev = this;
			const_cast<smart_array<T>*>(&_Y)->m_lpNext = this;
		} else {
			m_lpPrev = m_lpNext = NULL;
		}
		m_lpa	= _Y.m_lpa;
		m_nSize = _Y.m_nSize;
		m_nMax	= _Y.m_nMax;
	}
	smart_array<T>& operator=(const smart_array<T>& _Y) {
		if (this != &_Y) {
			Delete();
			//	���L�������Ƃ�̂��H
			if (_Y.m_lpPrev!=NULL) {
				//	���L���`�F�C���ɒǉ��B
				m_lpPrev = const_cast<smart_array<T>*>(&_Y);
				m_lpNext = const_cast<smart_array<T>*>(&_Y)->m_lpNext;
				const_cast<smart_array<T>*>(&_Y)->m_lpNext->m_lpPrev = this;
				const_cast<smart_array<T>*>(&_Y)->m_lpNext = this;
			}
			m_lpa	= _Y.m_lpa;
			m_nSize = _Y.m_nSize;
			m_nMax	= _Y.m_nMax;
		}
		return (*this);
	}
	template <class S>
	smart_array<T>& operator=(const S* _P) {
		//	�E�Ӓl�̓|�C���^�ł����āA���̏��L�҂ɂ��Ă͔���Ȃ�

		//	����|�C���^���ǂ����̃`�F�b�N�͂��Ȃ����c
		Delete();
		m_lpa	= const_cast<S*>(_P);	//	�A�b�v�L���X�g�����˂�
		m_nSize = sizeof(S);
		m_nMax	= _Y.m_nMax;
		return (*this);
	}

	virtual ~smart_array() { Delete(); }
	T& operator*() const  {return *get(); }
	T* operator->() const {return get();  }

	//	�z��̗v�f�͈̓`�F�b�N�@�\�t��
	T* get(int n=0) const {	//	n�Ԗڂ̗v�f�ʒu��Ԃ�
		WARNING(m_nMax!=0 && (n>=m_nMax || n<0),"�z��A�N�Z�X���͈͊O�ł�");
		return (T*)((BYTE*)m_lpa + n * m_nSize);
	}

	//	�i�ʗ�A�����I�ȃ����[�X�͍s�Ȃ�Ȃ����j
	//	����͊֘A����smart_array���珊�L���𔍒D���AT*��Ԃ�
	T* release() {
		//	���ׂẴ`�F�C�����珊�L���𔍒D����
		smart_array<T>* lpNext = m_lpNext;
		while (lpNext!=NULL) {
			lpNext->m_lpPrev = NULL;
			smart_array<T>* tmp = lpNext->m_lpNext;
			lpNext->m_lpNext = NULL;
			if (lpNext==this) break;
			lpNext = tmp;
		}
		return m_lpa;
	}

	//	�i�ʗ�A�����I�ȉ���͍s�Ȃ�Ȃ����j
	//�@������́A�����̏��L���݂̂��������
	void Delete() {
		//	�N�������L���������Ă���̂��H
		if (m_lpPrev) {
			//	���L�������Ă�̉��������H
			if (m_lpPrev==this) {
				delete [] m_lpa;
			} else {
				//	���L���`�F�C������؂藣��
				m_lpPrev->m_lpNext = m_lpNext;
				m_lpNext->m_lpPrev = m_lpPrev;
			}
			m_lpPrev = m_lpNext = NULL;
		}
		m_lpa	= NULL;
		m_nMax	= 0;
	}

	//	���L�����������C���X�^���X�̒ǉ������\��
	void Add(int n){
		Delete();
//		if (n!=0) {	//	����ȃ`�F�b�N�͂����C������
		m_lpa = new T[n];		//	�x������
		m_nSize = sizeof(T);
		m_lpPrev = m_lpNext = this;
		m_nMax	= n;
	}
	template <class S>
	void Add(S*_P,int nMax=0){			//	�|�����[�t�B�b�N�Ȍ^�ł���
		Delete();
		if (_P!=NULL) {
			m_lpa	= _P;
			m_nSize = sizeof(S);
			m_lpPrev = m_lpNext = this;
			m_nMax = nMax;
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

	//	���S�Ȕz��A�N�Z�X
	T& operator [] (int n) {
		//	�z��ւ̎Q�Ƃ͔͈̓`�F�b�N���s�Ȃ��A����
		//	�X�}�[�g�|�C���^���쐬���ĕԂ��i�ƈ��S�Ȃ̂���
		//	�������Ƀp�t�H�[�}���X�͈��������j
		return *get(n);
	}

/*
	//	smart_array�Ԃ̔�r�\��(���ۂɎw���Ă���I�u�W�F�N�g���r����)

	bool operator==(const smart_array<T>& _X) const
		{return (m_lpa == _X.m_lpa); }
	bool operator!=(const smart_array<T>& _X) const
		{return (!(*this == _X)); }
	bool operator<(const smart_array<T>&  _X) const
		{return (m_lpa < _X.m_lpa);}
	bool operator>(const smart_array<T>&  _X) const
		{return (_X < *this); }
	bool operator<=(const smart_array<T>& _X) const
		{return (!(_X < *this)); }
	bool operator>=(const smart_array<T>& _X) const
		{return (!(*this < _X)); }

	//	�R���X�g���N�^��explicit�ɂ��Ă��Ȃ��̂ŁA
	//	�Öق̕ϊ��ɂ��operator==���B���ɂȂ�Ȃ��悤��
	bool operator==(const T* lpa) const
		{return (m_lpa == lpa); }
	bool operator!=(const T* lpa) const
		{return (!(*this == lpa)); }
	bool operator<(const T*  lpa) const
		{return (m_lpa < lpa);}
	bool operator>(const T* lpa) const
		{return (lpa < *this); }
	bool operator<=(const T* lpa) const
		{return (!(lpa < *this)); }
	bool operator>=(const T* lpa) const
		{return (!(*this < lpa)); }
*/

	/////////////////////////////////////////////////////////////////
	//	���Ƃ́Asmart_array�I���W�i��
	//	std::vector�ɏ������Ă����ق����������ꂽ�\���Z�b�g���낤

	void clear(){ Delete(); } //	�o�b�t�@���̂̉��

	//	�C���X�^���X�̐����ǉ��\��
	void resize(int n){ m_lpPrev = m_lpNext = NULL; Add(new T[n],n); }

	//	�T�C�Y�w��ɂ��R���X�g���N�g(��������������(new T[n])�Ȃ�ď����̂̓_�T�C�I)
	smart_array(int n) {
		m_lpPrev = m_lpNext = this;
		m_lpa	= new T[n];
		m_nSize = sizeof(T);
		m_nMax = n;
	}

	//--- �ǉ� '01/12/13  by enra ---
	int size() const { return m_nMax; }	// �v�f����Ԃ�
	//-------------------------------

	///////////////////////////////////////////////////////////////////////////////
	//	STL���ɔ����q����`���Ă������[

	class iterator;
	iterator begin() { return iterator(m_lpa,0,m_nSize,m_nMax); }
	iterator end()   { return iterator(m_lpa,m_nMax,m_nSize,m_nMax); }
	//	����end�́A�z��T�C�Y���^�����Ă��Ȃ��Ɛ������l���Ԃ�Ȃ�
	iterator At(int n) { return iterator(m_lpa,n,m_nSize,m_nMax); }

	class iterator {
	public:
		iterator() : m_lpa(NULL),m_nOff(0),m_nSize(0),m_nMax(0){}
		iterator(T*pT,int nOff,int nSize,int nMax) {
			m_lpa = pT; m_nOff = nOff; m_nSize = nSize; m_nMax = nMax;
		}
		T* operator*() const {
			WARNING(m_nMax!=0 && (m_nOff>=m_nMax || m_nOff<0),"iterator�ł̃A�N�Z�X���v�f�O�ł�");
			return (T*)((BYTE*)m_lpa + m_nOff*m_nSize);
		}
		iterator& operator++() { inc(); return (*this); }
		iterator operator++(int) { iterator _Tmp = *this; inc(); return (_Tmp); }
		iterator& operator--() { dec(); return (*this); }
		iterator operator--(int) { iterator _Tmp = *this; dec(); return (_Tmp); }
		iterator& operator+=(int _N)
			{ m_nOff += _N; if (m_nOff>=m_nMax) m_nOff=m_nMax; return (*this); }
		iterator& operator-=(int _N)
			{ m_nOff -= _N; if (m_nOff<0) m_nOff=0; return (*this); }
		iterator operator+(int _N) const
			{iterator _Tmp = *this;
			return (_Tmp += _N); }
		iterator operator-(int _N) const
			{iterator _Tmp = *this;
			return (_Tmp -= _N); }
		T* operator[](int _N) const { return (*(*this + _N)); }
		bool operator==(const iterator& _X) const
			{return (m_lpa == _X.m_lpa && m_nOff == _X.m_nOff); }
		bool operator!=(const iterator& _X) const
			{return (!(*this == _X)); }
		bool operator<(const iterator& _X) const
			{return (m_lpa < _X.m_lpa
				|| m_lpa == _X.m_lpa && m_nOff < _X.m_nOff); }
		bool operator>(const iterator& _X) const
			{return (_X < *this); }
		bool operator<=(const iterator& _X) const
			{return (!(_X < *this)); }
		bool operator>=(const iterator& _X) const
			{return (!(*this < _X)); }
	///////////////////////////////////////////////////////////////////////////////
	protected:
		void dec() { /* if (m_nOff>0) */ m_nOff--; }
		//	�}�C�i�X�����́A��~���Ȃ��ق����f�o�b�O���₷���O�O�G
		void inc() { if (m_nMax==0 || m_nOff<=m_nMax) m_nOff++; }
		//	m_nOff��end()���w������

	private:
		T*	m_lpa;
		int m_nOff;		//	���݃|�C���g���Ă���v�f�C���f�b�N�X
		int	m_nSize;	//	�v�f�T�C�Y
		int m_nMax;		//	�ő�T�C�Y
	};

private:
	T		*m_lpa;		//	�|�C���^
	int		m_nSize;	//	�v�f�T�C�Y
	int		m_nMax;		//	�z��T�C�Y��������Ă�������(0:����`)

	//	�`�F�C���i�o�������X�g�j�������Ƃŏ��L���𖾂炩�ɂ���
	//	�i0�Ȃ�΁A���L���������Ă��Ȃ��j
	smart_array<T>* m_lpPrev;
	smart_array<T>* m_lpNext;
};

#endif