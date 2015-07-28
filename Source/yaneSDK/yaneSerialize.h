//	�V���A���C�Y�p�N���X
//			programmed by yaneurao	'01/07/16
//
//		�ЂƂ̃V���A���C�Y�֐��ŁA�X�g���[���ւ̕ۑ��^������
//		���p���邠���肪�A�V���Ȕ����̂悤�ȋC������̂����A
//		�����ł������̂��낤���D�D�O�O�G

#ifndef __yaneSerialize_h__
#define __yaneSerialize_h__

//	�V���A���C�Y�p�A�[�J�C�u�N���X
class CArchive {
public:
	friend class CSerialize;

	virtual ~CArchive() {} // place holder

protected:
	virtual void Serialize(CSerialize&)=0;
	//	�������I�[�o�[���C�h���Ďg���Ƃ����̂�

	/*
		��j
			void Serialize(CSerialize&s){
				s << nData << szData;
			}
			//	����Ȋ����ŕK�v�ȃ����o�����ׂď����o���R�[�h�������ׂ�
	*/
};

/*
		�ˁ@CSerialize�̃V���A���C�Y�͌��݂̃X�g�A����(IsStoring)
			�Q�ƈʒu(GetPos)���A�ꏏ�ɃV���A���C�Y����̂ŁA
			�������ɂ́A�����𒍈ӂ���K�v������B
*/

//	�V���A���C�Y�p�N���X
class CSerialize {
public:

	//	�f�[�^�̏������B
	//	�����̃X�g���[�����N���A���āAIsStoring()��true��Ԃ��悤�ɂȂ�
	void	Clear();

	bool	IsStoring() { return m_bStoring; }
	void	SetStoring(bool b) { m_bStoring = b; m_nDataPos = 0; if (b) GetData()->clear(); }
	//	�������false��ݒ肵�āA�擾�i�����j�����ɂ����Ƃ��A
	//	�f�[�^�̂ǂ�����ǂݏo���Ă��邩������
	//	�f�[�^�|�W�V�����|�C���^���N���A����
	//	�f�[�^��ۑ������ɂ����Ƃ��́A�f�[�^���N���A����
	//	�i�������Ȃ��ƁA�O��̃f�[�^�ɏ㏑�����Ă��܂��j
	void	InnerSetStoring(bool b) { m_bStoring = b; }
	//	�f�[�^�|�W�V�����̃��Z�b�g�E�f�[�^�N���A�����s�Ȃ킸�Ɋi�[��������
	//	�ύX����Ȃ�΁A���������g���ׂ�

	//	�X�g���[���̎擾
	vector<BYTE>* GetData() { return& m_abyData; }

	//	------ �e��f�[�^�̊i�[�p�I�y���[�^
	//	�A�[�J�C�u�I
	CSerialize& operator << (CArchive& vData);

	//	�����o�֐��e���v���[�g�ł̎���
	//	�������ACArchive�h���N���X�ŁA�����炪
	//	�D�悳���ƁA���܂��̂ŁAoperator�͂��
	template <class T>
//	CSerialize& operator << (T& vData){
	CSerialize& SerializeT (T& vData){
	//	����A�C�����C���ŏ����Ȃ��ƁA�R���p�C���ʂ�Ȃ��A�A
	//	VC++6.0�̃����o�֐��e���v���[�g�́A�ǂ����o�O���Ă���悤�ȋC������A�A
		if (IsStoring()){
		//	�ۑ�����񂩂��ȁH
			//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
			int nSize = sizeof(T);
			BYTE* pByte = (BYTE*)&vData;
			for(int i=0;i<nSize;i++){
				GetData()->push_back(pByte[i]);
			}
		} else {
		//	�X�g���[��������o���񂩂��ȁH
			//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
			int nSize = sizeof(T);
			BYTE* pByte = (BYTE*)&vData;
			for(int i=0;i<nSize;i++){
				pByte[i] = (*GetData())[m_nDataPos++];
			}
		}
		return *this;
	}

	//	primitive data
	CSerialize& operator << (int& nData);
	CSerialize& operator << (bool& bData);
	CSerialize& operator << (BYTE& byData);
	CSerialize& operator << (string& szData);
	CSerialize& operator << (WORD& wData){// �ǉ�
		return SerializeT(wData);
	}
	CSerialize& operator << (DWORD& dwData){
		return SerializeT(dwData);
	}

	//	vector���I
	CSerialize& operator << (vector<int>& anData);
	CSerialize& operator << (vector<bool>& abData);
	CSerialize& operator << (vector<BYTE>& abyData);
	CSerialize& operator << (vector<string>& szData);
	CSerialize& operator << (vector<WORD>& awData);// �ǉ�
	CSerialize& operator << (vector<DWORD>& adwData);// �ǉ�

	//	���邢�́AArray���A�A
	//	�����o�֐��e���v���[�g�ł̎���
	template <class T>
	CSerialize& Store(T* pavData,int nSize){
		if (IsStoring()){
			//	�ۑ�����񂩂��ȁH
			//	this << nSize;		//	�T�C�Y���i�[����K�v�͖���
			for(int i=0;i<nSize;i++){
				*this << *pavData;
				pavData++;
			}
		} else {
			//	���o���񂩂��ȁH
			//	*this << nSize;		//�@�T�C�Y�͖��m�ɂ킩���Ă���
			for(int i=0;i<nSize;i++){
				*this << *pavData;
				pavData++;
			}
		}
		return *this;
	}


	//	�������g���I�O�O�G
	CSerialize& operator << (CSerialize& vData);


	//	-----------------------------------------------------------

	//	�����āA�V���A���C�Y�Ԃ̃R�s�[�I
	CSerialize& operator = (CSerialize& vSeri);

	//	���܂�A���������̗p�ӂ������Ȃ����D�D
	LRESULT Save(string filename);	//	�X�g���[���̃t�@�C���ւ̕ۑ�
	LRESULT Load(string filename);	//	�X�g���[���̃t�@�C������̕���

	//	�g���ق����������ǃf�o�b�O�p��
	int		GetPos() { return m_nDataPos; }

	//	�R���X�g���N�^
	CSerialize() { Clear(); }
	virtual ~CSerialize() {}

protected:
	vector<BYTE> m_abyData;	//	�����Ƀf�[�^�͕ۑ������(������X�g���[���ƌĂ�)
	bool		m_bStoring; //�@�X�g���[���ɕۑ����Ȃ̂��A���o�����Ȃ̂��H
	int			m_nDataPos;	//�@�X�g���[������f�[�^�����o���Ƃ��A
							//	���݉��o�C�g�ڂ��w���Ă��邩������
};

#define ArraySerialize(n) &n[0],NELEMS(n)

#endif