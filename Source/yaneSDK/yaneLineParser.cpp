// CLineParser.cpp

#include "stdafx.h"
#include "yaneLineParser.h"

CLineParser::CLineParser( void ) {
	m_lpszReadData = NULL;										// 1���C����ǂݍ��񂾃f�[�^
}

CLineParser::~CLineParser() {
}

void	CLineParser::SetLine( LPSTR lpszStr ){
	m_lpszReadData = lpszStr;
}

bool	CLineParser::IsMatch( LPSTR lpszCmpStr ) {

	if ( m_lpszReadData == NULL ) return false;

	Garbage();				//	�S�~���폜����

	LPSTR lpOrg = m_lpszReadData;
	while ( true ) {
		if ( *lpszCmpStr == '\0' ) return true;
		if ( ::toupper( *(lpszCmpStr++) ) != ::toupper( *(m_lpszReadData++) ) ) {	//	�w�肵��������Ɠ��������ۂ��̔���
			m_lpszReadData = lpOrg;
			return false;
		}
	}
}


LRESULT		CLineParser::GetNum( int& nNo )
{
	if ( m_lpszReadData == NULL ) return 1;

	int		c;
	int		nRetNum = 0;
	bool	bSigned = false;

	Garbage();

	// �w0�`9�x���w-�x��T��
	// �ŏ����w-�x���w0�`9�x�ȊO�̎���GetNum���s
	c = *m_lpszReadData;
	if ( (c < '0' || c > '9') && (c != '-') ) return 1;

	if (c=='-') {
		bSigned = true;
		m_lpszReadData++;
	}

	while ( true ) {
		c = *m_lpszReadData;
		if ( (c >= '0') && (c <= '9') ) {
			nRetNum = nRetNum*10 + (c -'0');	//	������𐔒l�ɂ��āA�����グ��
		} else {
			break;
		}
		m_lpszReadData++;
	}

	if ( bSigned )												//	�������}�C�i�X���ۂ��̔���
		nRetNum = -nRetNum;

	nNo = nRetNum;												//	���ʂ�n��
	return 0;
} // GetNum


LRESULT		CLineParser::GetStr( string& str )
{
	if ( m_lpszReadData == NULL ) return 1;

	Garbage();
	
	// �w"�x��T��
	// �ŏ����w"�x�ȊO�̎���GetStr���s
	if ( *m_lpszReadData!= '\"' ) return 2;

	m_lpszReadData++;

	// "����"�܂ł̕������n��
	// �I���́w"�x��T��
	CHAR	szStr[256];
	int		k=0;
	while (true) {
		int c = *(m_lpszReadData++);
		if (c=='\0') return 3;	//	unexpectedly terminate
		
		if (c=='\"') {
			szStr[k] = '\0';
			break;
		} else {
			szStr[k] = c;
		}
		k++;
	}

	str = (LPSTR) szStr;
	return 0;
} // GetStr


/////////////////////////////////////////

void	CLineParser::Garbage( void ) {
	// �S�~�ȊO��������܂Ń��[�v����
	// �S�~�w�蕶�� �w (Space)�x�w	(Tab)�x�w,�x
	while ( true )
	{
		int c = *m_lpszReadData;
		if ( c != ' ' && c != '\t' && c != ',' ) break;
		m_lpszReadData++;
	}
}

void	CLineParser::ConvertCR(LPSTR lpsz){
	LPSTR lp = lpsz;
	for(;;) {
		*lpsz = *lp;
		if (*lpsz == '\0') break;
		if((*lp == '\\') && (*(lp+1)=='n')) {
			*lpsz = '\n';
			lpsz++;
			lp+=2;
			continue;
		}
		lpsz++;
		lp++;
	}
}