//	�Q�[���p�A�ėp�J�E���^
//		�i�v���X�e�J���L�b�g�ł������炵���j

#ifndef __yaneRootCounter_h__
#define __yaneRootCounter_h__

class CRootCounter {
public:
	//	�ݒ�
	void	Set(int nStart,int nEnd,int nStep=1);
	void	SetStep(int nStep);
	void	SetStart(int nStart);
	void	SetEnd(int nEnd);

	//	�擾
	int		GetStep(void) { return m_nStep; }
	int		GetStart(void) { return m_nStart; }
	int		GetEnd(void) { return m_nEnd; }
	////	�I�[�g���o�[�X�J�E���^
	void	SetReverse(bool bReverse){ m_bReverse = bReverse;}	//	���o�[�X�J�E���^�ɂ���
	bool*	GetReversing(void){ return &m_bReversing; }			//	���]�J�E���g�����H
	////	�����l
	void	SetInit(int n) { m_bInit = true; m_nInit = n; }

	//	�J�E���^�̃��Z�b�g
	void	Reset(void);

	//	�J�E���^�̃C���N�������g(�I�[�܂ŒB����ƁA�ēx�A�����l�ɖ߂�)
	void	Inc(void);		//	���Z

	CRootCounter& operator++() { Inc(); return (*this); }
	CRootCounter operator++(int) { CRootCounter _Tmp = *this; Inc(); return (_Tmp); }

	//	�J�E���^�̃T�`�����[�V�����C���N�������g�i�I�[�܂ŒB����ƁA�����Œ�~����j
	void	IncS(void);

	//	int�Ƃ̑��ݕϊ�
	operator int (void) { return m_nRootCount; }
	const int operator = (int n) { m_nRootCount = n; return n; }
	int		Get (void) { return m_nRootCount; }

	//	Inc�������ʁA���񂵂����H�^IncS�������ʁA�I�l�ɂȂ������H
	bool	IsLapAround(void) { return m_bLapAround; }
	//	Inc�������ʁA�����l�ɖ߂������H
	bool	IsLapAroundI(void) { return m_bLapAroundI; }

	CRootCounter(void);
	CRootCounter(int nEnd);
	CRootCounter(int nStart,int nEnd,int nStep=1);

	virtual ~CRootCounter();

protected:
	int		m_nRootCount;
	int		m_nStart;
	int		m_nEnd;
	int		m_nStep;
	int		m_nRate;	//	nStep<0�̂Ƃ��́A�����Inc()��+1�����
	bool	m_bLapAround;
	bool	m_bLapAroundI;

	bool	m_bReverse;		//	���o�[�X�J�E���^
	bool	m_bReversing;	//	���o�[�X�����H
	bool	m_bInit;		//	�����l���ݒ肳��Ă��邩�H
	int		m_nInit;		//	�ݒ肳��Ă��鏉���l
};

//	������́AnStart��nEnd�łȂ��ėǂ�
class CRootCounterS {
public:
	//	nStep�͈��̑����̐�Βl�B�}�C�i�X��1/nStep�̈Ӗ�
	void	Set(int nStart,int nEnd,int nStep=1) { m_nStart=nStart; m_nEnd=nEnd; m_nStep=nStep; Reset(); }
	void	SetStep(int nStep) { m_nStep = nStep; }
	void	SetStart(int nStart) { m_nStart = nStart; }
	void	SetEnd(int nEnd) { m_nEnd = nEnd; }

	//	�擾
	int		GetStep(void) const { return m_nStep; }
	int		GetStart(void) const { return m_nStart; }
	int		GetEnd(void) const { return m_nEnd; }

	//	�J�E���^�̃��Z�b�g
	void	Reset(void) { m_nRootCount= m_nStart; m_nRate=0; }

	//	property..
	bool	IsEnd(void) const { return m_nRootCount == m_nEnd; }

	CRootCounterS(void);
	CRootCounterS(int nEnd);
	CRootCounterS(int nStart,int nEnd,int nStep=1);

	virtual ~CRootCounterS() {}

	//	int�Ƃ̑��ݕϊ�
	operator int (void) { return m_nRootCount; }
	const int operator = (int n) { m_nRootCount = n; return n; }
	int		Get (void) { return m_nRootCount; }

	//	�J�E���^�̃C���N�������g(�I�[�܂ŒB����ƁA�����Œ�~����)
	void	Inc(bool bAdd=true);
	//	���Z�i��End�����փC���N�������g�j�^���Z�i��Start�����ւ̃C���N�������g�j

	CRootCounterS& operator++() { Inc(true); return (*this); }
	CRootCounterS operator++(int) { CRootCounterS _Tmp = *this; Inc(true); return (_Tmp); }
	CRootCounterS& operator--() { Inc(false); return (*this); }
	CRootCounterS operator--(int) { CRootCounterS _Tmp = *this; Inc(false); return (_Tmp); }

protected:
	int		m_nRootCount;
	int		m_nStart;
	int		m_nEnd;
	int		m_nStep;
	int		m_nRate;	//	nStep<0�̂Ƃ��́A�����Inc()��+1�����
};

#endif