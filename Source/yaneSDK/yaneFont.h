//	yaneFont.h :
//
//		Font Wrapper

#ifndef __yaneFont_h__
#define __yaneFont_h__

class CFont {
public:

	//	set text & drawing
	void	SetText(const string& s);	// �\��������̎w��i\n�ŉ��s���g�p�ł���j
	void	__cdecl SetText(LPSTR fmt, ... );	//	���Ɠ����Bprintf�o�͏������g����B
										//	������buf��512�����܂�
	void	SetText(int i);				//	������\��������Ƃ��Ďw��
	LPCSTR	GetText() { return m_String.c_str(); }

	LRESULT	GetSize(int& sx,int& sy);	//	�`��T�C�Y�𓾂�
	void	OnDraw(HDC hdc,int x=0,int y=0);//	HDC��n���āA�����ɕ`�悵�Ă��炤

	//	property
	void	SetQuality(int nQuality);	//	�t�H���g�N�I���e�B				(0�j
	int		GetQuality() const { return m_nQuality; }

	void	SetSize(int nSize);			// Font�T�C�Y��pt���Ŏw��			(16)
	int		GetSize() const { return m_nSize; }

	void	SetColor(COLORREF rgb);		// �e�L�X�g�J���[�w��				(255,255,255)
	COLORREF GetColor() const { return m_nRgb; }

	void	SetBackColor(COLORREF rgb);	// �����̉e��						(64,64,64)
	COLORREF GetBackColor() const { return m_nBkRgb; }

	void	SetBGColor(COLORREF rgb);	// �w�i�F							(CLR_INVALID)
	COLORREF GetBGColor() const { return m_nBGRgb; }

	void	SetHeight(int nHeight);		// ���s�܂ł̍s��					(20)
	int		GetHeight() const { return m_nHeight; }

	void	SetFont(int nFontNo);		// �t�H���g�Z���N�g�i�ԍ��Łj		(0) : MS�S�V�b�N
	void	SetFont(const string& fontname);	// �t�H���g�Z���N�g�i�t�H���g���Łj
	LPCSTR	GetFont(void) { return m_FontName.c_str(); }
	void	SetWeight(int nWeight);		// �����̑���						(300:FW_LIGHT) , FW_BOLD��700
	void	SetItalic(bool b);			// �Α�								(false)
	void	SetUnderLine(bool b);		// �����̗L��						(false)
	void	SetStrikeOut(bool b);		// �ł��������̗L��					(false)
	void	SetShadowOffset(int nOx,int nOy); // �e�̃I�t�Z�b�g				(2,2)
	void	GetShadowOffset(int &nOx,int &nOy) const { nOx=m_nShadowOffsetX; nOy=m_nShadowOffsetY; }


	CFont();
	virtual ~CFont();

protected:
	string	m_String;			//	�\�����镶����
	int		m_nQuality;			//	�t�H���g�N�I���e�B
	COLORREF m_nRgb;			//	�����F
	COLORREF m_nBkRgb;			//	�����̉e
	COLORREF m_nBGRgb;			//	�w�i�F
	int		m_nSize;			//	�I������Ă���t�H���g�T�C�Y
	string	m_FontName;			//	�I������Ă���t�H���g��
	int		m_nHeight;			//	���̍s�܂ł̋���
	int		m_nWeight;			//	�����̑���
	bool	m_bItalic;			//	�Α�
	bool	m_bUnderLine;		//	����
	bool	m_bStrikeOut;		//	�ł�������

	int		m_nShadowOffsetX;	//	�e�̃I�t�Z�b�gX���W
	int		m_nShadowOffsetY;	//	�e�̃I�t�Z�b�gY���W

	//	Auxiliary
	void	TextOut(HDC hdc,int x,int y,const string& s);
};

#endif