//	LZSS�ɂ�鈳�k���[�`��
//
//	���@�Q�l����
//	  �������F���wC����ɂ��ŐV�A���S���Y�����T�x�^�Z�p�]�_��

#ifndef __yaneLZSS_h__
#define __yaneLZSS_h__

const int LZSS_RING_BUFFER		= 4096; // ��o�b�t�@�̑傫��
const int LZSS_LONGEST_MATCH	= 18;	// �Œ���v��

class CLZSS {
public:
	//	Decode����BDst��̃o�b�t�@��bDst��true�Ȃ�Ίm�ۂ����
	LRESULT Decode(BYTE* lpSrc,BYTE*& lpDst,DWORD  dwSize,bool bDst=true);

	//	Encode����BDst��̃o�b�t�@��bDst��true�Ȃ�Ίm�ۂ����
	LRESULT Encode(BYTE* lpSrc,BYTE*& lpDst,DWORD  dwSize,DWORD& dwDstSize,bool bDst=true);

private:
	//	�o�̓o�C�g���J�E���^
	DWORD	m_dwOutCount;

	// �e�L�X�g�p�o�b�t�@
	UCHAR	m_szText[LZSS_RING_BUFFER+LZSS_LONGEST_MATCH-1];

	//	��
	int		m_dad[LZSS_RING_BUFFER+1];
	int		m_lson[LZSS_RING_BUFFER+1];
	int		m_rson[LZSS_RING_BUFFER+257];

	void	init_tree(void);	//	�؂̏�����
	void	insert_node(int r);	//	�� r ��؂ɑ}��
	void	delete_node(int p);	//	�� p ��؂������

	int		m_matchpos, m_matchlen;	 /* �Œ���v�ʒu, ��v�� */

};

#endif