//	yaneLineParser.h

#ifndef __yaneLineParser_h__
#define __yaneLineParser_h__

class CLineParser {
public:

	void	SetLine( LPSTR lpszStr );				//	1���C���̕�������Z�b�g����
	bool	IsMatch( LPSTR lpszCmpStr );			//	�w�肵��������Ɠ��������ۂ���Ԃ�
	LRESULT	GetNum( int& nNo );						//	���l�𓾂�
	LRESULT	GetStr( string& str );					//	"..."�ň͂܂ꂽ������𓾂�

	static void	ConvertCR(LPSTR);					//	�u\n�v��'\n'�ɕϊ�

	CLineParser( void );							//	�R���X�g���N�^
	virtual	~CLineParser();							//	�f�X�g���N�^

protected:
	void	Garbage( void );						//	�S�~��
	LPSTR	m_lpszReadData;							//	1���C����ǂݍ��񂾃f�[�^
};

#endif