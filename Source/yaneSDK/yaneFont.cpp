#include "stdafx.h"

#include "yaneFont.h"

//////////////////////////////////////////////////////////////////////////////

CFont::CFont(void){
	SetSize(16);
	SetHeight(20);
	SetFont(0);	// MS �S�V�b�N
//	SetColor(RGB(128,192,128));
//	SetBackColor(RGB(64,128,64));
	SetColor(RGB(255,255,255));
	SetBackColor(RGB(128,128,128));
	SetBGColor(CLR_INVALID);
	SetQuality(2);
	SetWeight(FW_LIGHT);
	SetItalic(false);
	SetUnderLine(false);
	SetStrikeOut(false);
	SetShadowOffset(2,2);
}

CFont::~CFont(){
}

//////////////////////////////////////////////////////////////////////////////

void	CFont::SetQuality(int nQuality){
	switch(nQuality) {
	case 0: m_nQuality = DEFAULT_QUALITY; break;
	case 1: m_nQuality = DRAFT_QUALITY; break;
	case 2: m_nQuality = PROOF_QUALITY; break;
	case 3: m_nQuality = ANTIALIASED_QUALITY; break;
	case 4: m_nQuality = NONANTIALIASED_QUALITY; break;
	default: WARNING(true,"CFont::SetQuality �K��O�̐��l");
	}
}

void	CFont::SetSize(int nSize){
	m_nSize = nSize;
	m_nHeight = nSize;	//	�������X�V���鉿�l������H
}

void	CFont::SetColor(COLORREF rgb){
	m_nRgb = rgb;
}

void	CFont::SetBackColor(COLORREF rgb){
	m_nBkRgb	= rgb;
}

void	CFont::SetBGColor(COLORREF rgb){
	m_nBGRgb	= rgb;
}

void	CFont::SetHeight(int nHeight){
	m_nHeight = nHeight;
}

void	CFont::SetWeight(int nWeight){
	m_nWeight = nWeight;
}

void	CFont::SetFont(int nFontNo){
	string name;
	switch (nFontNo) {
	case 0: name ="�l�r �S�V�b�N"; break;
	case 1: name ="�l�r �o�S�V�b�N"; break;
	case 2: name ="�l�r ����"; break;
	case 3: name ="�l�r �o����"; break;
	default: name="";
	}
	if (m_FontName==name) return ;
	m_FontName=name;
}
void	CFont::SetFont(const string& fontname){
	if (m_FontName==fontname) return ;
	m_FontName=fontname;
}

void	CFont::SetText(const string& s){
	m_String = s;
}

void	__cdecl CFont::SetText(LPSTR fmt, ... ){
	CHAR buf[512];
	wvsprintf(buf,fmt,(LPSTR)(&fmt+1));
	SetText((string)buf);
}

void CFont::SetText(int i){
	CHAR buf[16];
	wsprintf(buf,"%d",i);
	SetText(buf);
}

void	CFont::SetItalic(bool b){
	m_bItalic = b;
}

void	CFont::SetUnderLine(bool b){
	m_bUnderLine = b;
}

void	CFont::SetStrikeOut(bool b){
	m_bStrikeOut = b;
}

void	CFont::SetShadowOffset(int nOx,int nOy){
	m_nShadowOffsetX = nOx;
	m_nShadowOffsetY = nOy;
}

/////////////////////////////////////////////////////////////////////////////////////////

//	drawing

void	CFont::OnDraw(HDC hdc,int x,int y){

	//	�����񂪐ݒ肳��Ă��Ȃ���΋A��
	if (m_String.empty()) return ;

	HFONT hFont = ::CreateFont(m_nSize,0,0,0,m_nWeight,m_bItalic,m_bUnderLine,m_bStrikeOut,SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, m_nQuality,FF_MODERN,m_FontName.c_str());

	if (hFont==NULL) return ; // �����������̂�����I�H

	HFONT hFontLast = (HFONT)::SelectObject(hdc,hFont);

	//	�����w�i�F�t���ŕ`�悷��Ȃ�΁A
	//	�ŏ��̈���opaque,���ڈȍ~��transparent,
	bool bFirst = true;

	if (m_nBkRgb!=CLR_INVALID){
		//	����A�d�˂鉿�l���Ȃ������D�D
		POINT point[5] = {
			{m_nShadowOffsetX,m_nShadowOffsetY},		// 1
//			{2,2},		// 1
			{0,1},
			{2,1},		// 2
			{1,2},
			{2,2}		// 3
		};

		::SetTextColor(hdc,m_nBkRgb);
		//	�F�̕ύX

		//	�w�i�F�̐ݒ�
		if (m_nBGRgb==CLR_INVALID) {
			::SetBkMode(hdc, TRANSPARENT);
		} else {
			::SetBkMode(hdc, OPAQUE);
			::SetBkColor(hdc,m_nBGRgb);
		}
		bFirst = false;

		for(int i=0;i<1 /* 5 */;i++){
			TextOut(hdc,x + point[i].x,y + point[i].y,m_String);
			//	�����̕\���iWIN32API�ł͂Ȃ��A���̃N���X��static�֐��ł��邱�Ƃɒ��Ӂj
		}
	}
	if (m_nRgb!=CLR_INVALID){
		//	�w�i�F�̐ݒ�
		if (bFirst){
			if (m_nBGRgb==CLR_INVALID) {
				::SetBkMode(hdc, TRANSPARENT);
			} else {
				::SetBkMode(hdc, OPAQUE);
				::SetBkColor(hdc,m_nBGRgb);
			}
		} else if (m_nBGRgb!=CLR_INVALID) {
			//	�Q��ڈȍ~�̕`��Ȃ̂œ��ߐݒ�ɖ߂��Ă����Ȃ��ẮD�D
			::SetBkMode(hdc, TRANSPARENT);
		}

		::SetTextColor(hdc,m_nRgb);
		TextOut(hdc,x,y,m_String);	//	�����̕\��
	}
	::SelectObject(hdc,hFontLast);
	::DeleteObject(hFont);		// ������ł����񂩁H
}

LRESULT	CFont::GetSize(int& sx,int& sy){
	sx = 0; sy = 0;	// fail safe�΍�

	//	�����񂪐ݒ肳��Ă��Ȃ���΋A��
	if (m_String.empty()) return 1;

	HFONT hFont = ::CreateFont(m_nSize,0,0,0,m_nWeight,m_bItalic,m_bUnderLine,m_bStrikeOut,SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, m_nQuality,FF_MODERN,m_FontName.c_str());
	if (hFont==NULL) return 1; // ������[

	SIZE size;
	HDC hdc = ::CreateCompatibleDC(NULL);
	HFONT hFontLast = (HFONT)::SelectObject(hdc,hFont);

	LPCSTR p = m_String.c_str();
	CHAR buf[256];	//	���̒����܂ł����v���s�O�O�G
	LPSTR q=buf;
	size.cx=0; size.cy=0;
	SIZE size2;
	for(;;){
		*q = *(p++);
		if (*q=='\n') {
			*q=0;	// ���s�R�[�h�Ȃ�Βׂ� 
			::GetTextExtentPoint32(hdc,buf,::lstrlen(buf),&size2);
			//GetTextExtentPoint�͓���̏󋵂łP�h�b�g���ꂪ�������邻����^^;
//			size.cy+=size2.cy;
			size.cy+=m_nHeight;	//	fixed by ������
			if (m_bItalic) size2.cx+=m_nSize/4;	//	API���Α̂̃T�C�Y��������bug�΍�
			if (size.cx<size2.cx) size.cx=size2.cx;
			q=buf;
			continue;	//	���̂Ƃ���q�����Z����Ă͍���
		} else if (*q=='\0') {
			::GetTextExtentPoint32(hdc,buf,::lstrlen(buf),&size2);
			//GetTextExtentPoint�͓���̏󋵂łP�h�b�g���ꂪ�������邻����^^;
			size.cy+=size2.cy;
			if (m_bItalic) size2.cx+=m_nSize/4;	//	API���Α̂̃T�C�Y��������bug�΍�
			if (size.cx<size2.cx) size.cx=size2.cx;
			break;	// ����ɂďI���`
		}
		q++;
	}
	sx = size.cx + 3;	//	�����ɂ͉e������̂�
	sy = size.cy + 3;	//

	::SelectObject(hdc,hFontLast);
	::DeleteObject(hFont);	// ������ł����񂩁H

	::DeleteDC(hdc);	//	����Ō�ɂ���Ƃ�����ŁI(by TearDrop_Stone)

	return 0;
}

void	CFont::TextOut(HDC hdc,int x,int y,const string& s){

	//	�����񂪐ݒ肳��Ă��Ȃ���΋A��
	if (m_String.empty()) return ;

	//	�ő�̗��_�́A�����񒆂�\n�𖄂ߍ��ނ��Ƃŉ��s���o���邱�Ƃł���B

	CHAR buf[256];
	LPCSTR p = s.c_str();
	LPSTR q=buf;
	for(;;){
		*q = *(p++);
		if (*q=='\n') {
			*q=0;	// ���s�R�[�h�Ȃ�Βׂ� 
			::TextOut(hdc,x,y,buf,::lstrlen(buf));
			y+=m_nHeight;
			q=buf;
			continue;	//	���̂Ƃ���q�����Z����Ă͍���
		} else if (*q=='\0') {
			::TextOut(hdc,x,y,buf,::lstrlen(buf));
			break;	// ����ɂďI���`
		}
		q++;
	}
}