// BWHTMLDIB32.h: CBWHTMLDIB32 �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

// BWHTMLDIB32��08/09/17�ł���ɍ쐬

#ifndef __F3HTMLDIB32_H__
#define __F3HTMLDIB32_H__

#include "YANESDK\yaneDIB32.h"

// �����ƕ�����̃Z�b�g
struct Tf3HTMLPiece
{
	string		data;			// �\������e�L�X�g�܂��͉摜�t�@�C����
	RECT		rcClip;			// �N���b�s���O�͈�
	SIZE		szImage;		// �C���[�W�̃T�C�Y

	int			fontSize;		// �t�H���g�T�C�Y
	COLORREF	rgbColor;		// �����F

	bool		bBold;			// ����
	bool		bItalic;		// �C�^���b�N��
	bool		bUnderLine;		// �A���_�[���C��
	bool		bStrikeOut;		// ��������

	bool		bImage;			// �C���[�W���ǂ���
	bool		bBreak;			// ���s���ǂ���
};

enum HTMLAlign { AlignLeft, AlignCenter, AlignRight };
enum HTMLVAlign { AlignBottom, AlignMiddle, AlignTop };

class Cf3HTMLDIB32 : public CDIB32  
{
	friend class Parser;
	friend class Renderer;
public:
	void SetVAlign(HTMLVAlign valign) { m_DefaultVAlign = valign; }
	HTMLVAlign GetVAlign() { return m_DefaultVAlign; }
	void SetAlign(HTMLAlign align) { m_DefaultAlign = align; }
	HTMLAlign GetAlign() { return m_DefaultAlign; }
	void OnDraw(CDIB32* lp, int x, int y, int length=INT_MAX);
	COLORREF GetColorByName(LPCSTR strName);
	void SetMaxWidth(int w);
	int GetFontSize(int relative);
	void SetFontSize(int nSize);
	void UpdateCondition();
	void Update();
	void __cdecl SetText(LPSTR fmt, ... );
	void SetText(const string &s);
	Cf3HTMLDIB32();
	virtual ~Cf3HTMLDIB32();

protected:
	void InnerBlt(CDIB32*lp, int x, int y, LPRECT rc=NULL);
	bool m_bDirty;				// �ĕ`�悪�K�v��

	string m_String;				// HTML�^�O���܂ޕ�����
	string m_PureText;				// �^�O���܂܂Ȃ�������
	list<Tf3HTMLPiece> m_Pieces;	// �����ƕ�����̃Z�b�g
	vector<RECT> m_Rects;			// �ꕶ�����Ƃ̕`��G���A

	int m_nFontSize;
	int m_nMaxWidth;
	int m_nLength;

	HTMLAlign m_Align, m_DefaultAlign;
	HTMLVAlign m_VAlign, m_DefaultVAlign;
};

#endif
