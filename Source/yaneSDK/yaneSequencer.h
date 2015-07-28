//	yaneSequencer.h :
//
//		�^�C�~���O�����Ȃ���
//		�v���O�������������邽�߂̃A�V�X�g�N���X
//
//		programmed by yaneurao '00/02/29

#ifndef __yaneSequencer_h__
#define __yaneSequencer_h__

class CSequenceInfo {
public:
	int		m_nStart;		//	���̒l����
	int		m_nEnd;			//	���̒l�܂ł͈̔͂Ȃ��
	int		m_nData;		//	���̒l��Ԃ��B�����Ȃ���NULL

	//	STL�̔�r�̂��߂�operator
	bool	operator <(const CSequenceInfo& x) const;
};
//	int �́@�������R�Q�r�b�g�Ɖ��肵�Ă���B

class CSequencer {
public:
	LRESULT Add(int start,int end,int data);	//	�ǉ�
	LRESULT Add(int start,int data);			//	�ǉ�
	LRESULT Del(int start,int end,int data);	//	�폜


	void Clear(void);		//	�S�폜

	LRESULT	Get(int pos,int& data,int &diff);
	//	pos�̒l�ɑΉ�����f�[�^�ɂ��Ė₢���킹��B
	//	���������Ƃ��ɂ�data�ɂ��̒l�Adiff��start����̑��ʂ�����B
protected:
	set<CSequenceInfo>	m_oSequenceList;
};

#endif