/*
	auto_array	:	auto_ptr array
		programmed by yaneurao(M.Isozaki) '00/09/28
*/

#ifndef __YTLAutoArray_h__
#define __YTLAutoArray_h__

template<class T> class auto_array {
public:
	typedef T element_type;

	explicit auto_array(const T* p){
		m_lp	= const_cast<T*>(p);
		m_bOwner= true;
	}
	auto_array(){
		m_lp	 = NULL;
		m_bOwner = false;
	}
	auto_array(auto_array<T>&rhs){
		m_lp	= rhs.m_lp;
		m_bOwner= rhs.m_bOwner;
		rhs.m_bOwner = false;
	}
	//	auto_array�Ԃ̑�����Z�q
	auto_array<T>& operator = (auto_array<T>& rhs){
		if (this!=&rhs){
			clear();
			m_lp	= rhs.m_lp;
			m_bOwner= rhs.m_bOwner;
			rhs.m_bOwner = false;
		}
		return *this;
	}
	//	������������������`���Ƃ����`�O�O
	auto_array<T>& operator=(const T* _P) {
		//	�E�Ӓl�̓|�C���^�ł����āA���̏��L�҂ɂ��Ă͔���Ȃ�
		if (m_lp != _P) {
			clear();
			SetOwner(false);	//	����āA���L���͖������̂Ƃ���
			m_lp	= const_cast<T*>(_P);
			//	VC++6.0�ł͏o���邪�A�{����const T*��T*�ɑ���ł��Ȃ��̂ŃL���X�g����
		}
		return *this;
	}

	virtual ~auto_array(){
		clear();
	}

	T* get(int n=0) const {	//	n�Ԗڂ̗v�f�ʒu��Ԃ�
		return &m_lp[n];
	}
	T* release() {
		T* tmp	= m_lp;
		m_lp	= NULL;
		SetOwner(false);	//	���L���̉��
		return	tmp;
	}

	//	���������̂͂��܂�������ׂ��łȂ����A
	//	�ꉞT*�ƃR���p�`�r���e�B��ۂ������B
	operator T* () const { return m_lp; }

	/////////////////////////////////////////////////////////////////
	//	���Ƃ́Aauto_array�I���W�i��
	//	std::vector�ɏ������Ă����ق����������ꂽ�\���Z�b�g���낤

	void clear(){	//	�o�b�t�@���̂̉��
		if (m_lp!=NULL && m_bOwner){
			delete [] m_lp;
			SetOwner(false);
		}
		m_lp = NULL;
	}

	//	�C���X�^���X�̐����ǉ��\��
	void resize(int n){
		*this = auto_array<T>(new T[n]);
	}

	//	�ȉ��́A���܂�^^;

	void Add(const T* _P){
		clear();
		m_lp	= const_cast<T*>(_P);
		SetOwner(true);
	}

	//	�T�C�Y�w��ɂ��R���X�g���N�g(��������������(new T[n])�Ȃ�ď����̂̓_�T�C�I)
	auto_array(int n){
		m_lp	= new T[n];
		SetOwner(true);
	}

	//	�I�[�i�[�ł��邩��Ԃ��֐�
	bool	IsOwner() const { return m_bOwner; }
	//	�I�[�i�[���̐ݒ�
	void	SetOwner(bool b) { m_bOwner = b; }

private:
	T*		m_lp;		//	�z��̐擪�������|�C���^
	bool	m_bOwner;	//	���̃|�C���^�̏��L�ҁH
};

#endif