#include "stdafx.h"
#include "yaneLZSS.h"

const int NIL = LZSS_RING_BUFFER;	// �؂̖��[

///////////////////////////////////////////////////////////////////////////////
LRESULT CLZSS::Encode(BYTE* lpSrc,BYTE*& lpDst,DWORD  dwSize,DWORD& dwDstSize,bool bDst){

	int i, c, len, r, s, lastmatchlen, codeptr;
	UCHAR code[17], mask;

	if (dwSize<=16) {
	//	�T�C�Y���������Ȃ�A���k�ł��񂼂�O�O�G
		return 1;
	}

	DWORD dwSize2 = dwSize-8; // ���������Ȃ�Έ��k���鉿�l������

	init_tree();  /* �؂������� */
	code[0] = 0;  codeptr = mask = 1;
	s = 0;	r = LZSS_RING_BUFFER - LZSS_LONGEST_MATCH;
	for (i = s; i < r; i++) m_szText[i] = 0;  /* �o�b�t�@�������� */
	for (len = 0; len < LZSS_LONGEST_MATCH ; len++) {
		c = *(lpSrc++);
		if (--dwSize <= 0) break;
		m_szText[r + len] = c;
	}
	if (len == 0) return 1;
	for (i = 1; i <= LZSS_LONGEST_MATCH; i++) insert_node(r - i);
	insert_node(r);

	dwDstSize = 0;
	BYTE* lpDs;
	if (bDst) {
		lpDst = new BYTE[dwSize2];	//	�Ƃ肠�����A���ꂾ���m�ۂ��đ���񂩂�������:p
		lpDs = lpDst;
	}
	
	do {
		if (m_matchlen > len) m_matchlen = len;
		if (m_matchlen < 3) {
			m_matchlen = 1;	 code[0] |= mask;	 code[codeptr++] = m_szText[r];
		} else {
			code[codeptr++] = (BYTE) m_matchpos;
			code[codeptr++] = (BYTE)
				(((m_matchpos >> 4) & 0xf0) | (m_matchlen - 3));
		}
		if ((mask <<= 1) == 0) {
			dwDstSize += codeptr;
			if (dwSize2 <= dwDstSize) goto ErrorEnd;	//	�W�J��o�b�t�@���:p
			for (i = 0; i < codeptr; i++) {
				if (bDst) *(lpDs++) = code[i];
			}
			code[0] = 0;  codeptr = mask = 1;
		}
		lastmatchlen = m_matchlen;
		for (i = 0; i < lastmatchlen; i++) {
			if (dwSize == 0) break;
			dwSize--;
			c = *(lpSrc++);
			delete_node(s);	 m_szText[s] = c;
			if (s < LZSS_LONGEST_MATCH - 1) m_szText[s + LZSS_RING_BUFFER] = c;
			s = (s + 1) & (LZSS_RING_BUFFER - 1);	r = (r + 1) & (LZSS_RING_BUFFER - 1);
			insert_node(r);
		}
		while (i++ < lastmatchlen) {
			delete_node(s);
			s = (s + 1) & (LZSS_RING_BUFFER - 1);	r = (r + 1) & (LZSS_RING_BUFFER - 1);
			if (--len) insert_node(r);
		}
	} while (len > 0);
	if (codeptr > 1) {
		dwDstSize += codeptr;
		//	�W�J��o�b�t�@���:p
		if (!(dwSize2 <= dwDstSize)) {
			for (i = 0; i < codeptr; i++) {
				if (bDst) *(lpDs++) = code[i];
			}
		}
	}

ErrorEnd:;
	//	���k�䂪������Έ��k�����
	if (dwSize2 <= dwDstSize){
		if (bDst) DELETEPTR_SAFE(lpDst);
		return 1;
	}
	return 0;
}

LRESULT CLZSS::Decode(BYTE* lpSrc,BYTE*& lpDst,DWORD  dwSize,bool bDst){

	if (dwSize==0) return 1;	//	�Ȃ񂶂Ⴑ���:p

	BYTE* lpDs;
	if (bDst) {	//	������Ŋm�ۂ���̂��H�����łȂ���Ώ���������Ă���Ɖ��肷�邼
		lpDst = new BYTE[dwSize];	//	�W�J����m�ۂ���B
	}
	lpDs  = lpDst;

	int i, j, k, r, c;
	r = LZSS_RING_BUFFER - LZSS_LONGEST_MATCH;

	DWORD dwFlags = 0;

	ZERO(m_szText);

	for ( ; ; ) {
		if (((dwFlags >>= 1) & 256) == 0) {
			c = *(lpSrc++);
			dwFlags = c | 0xff00;
		}
		if (dwFlags & 1) {
			c = *(lpSrc++);
			*(lpDs++) = c;
			if (--dwSize == 0) return 0;
			m_szText[r++] = c;	r &= (LZSS_RING_BUFFER - 1);
		} else {
			i = *(lpSrc++); j = *(lpSrc++);
			i |= ((j & 0xf0) << 4);	 j = (j & 0x0f) + 2;
			for (k = 0; k <= j; k++) {
				c = m_szText[(i + k) & (LZSS_RING_BUFFER - 1)];
				*(lpDs++) = c;
				if (--dwSize == 0) return 0;
				m_szText[r++] = c;	r &= (LZSS_RING_BUFFER - 1);
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////

void CLZSS::init_tree(void)	 /* �؂̏����� */
{
	int i;

	for (i = LZSS_RING_BUFFER + 1; i <= LZSS_RING_BUFFER + 256; i++) m_rson[i] = NIL;
	for (i = 0; i < LZSS_RING_BUFFER; i++) m_dad[i] = NIL;
}

void CLZSS::insert_node(int r)	/* �� r ��؂ɑ}�� */
{
	int i, p, cmp;
	BYTE* key;

	cmp = 1;  key = &m_szText[r];  p = LZSS_RING_BUFFER + 1 + key[0];
	m_rson[r] = m_lson[r] = NIL;  m_matchlen = 0;
	for ( ; ; ) {
		if (cmp >= 0) {
			if (m_rson[p] != NIL) p = m_rson[p];
			else {	m_rson[p] = r;	m_dad[r] = p;  return;	}
		} else {
			if (m_lson[p] != NIL) p = m_lson[p];
			else {	m_lson[p] = r;	m_dad[r] = p;  return;	}
		}
		for (i = 1; i < LZSS_LONGEST_MATCH; i++)
			if ((cmp = key[i] - m_szText[p + i]) != 0)	break;
		if (i > m_matchlen) {
			m_matchpos = p;
			if ((m_matchlen = i) >= LZSS_LONGEST_MATCH)	 break;
		}
	}
	m_dad[r] = m_dad[p];   m_lson[r] = m_lson[p];  m_rson[r] = m_rson[p];
	m_dad[m_lson[p]] = r;  m_dad[m_rson[p]] = r;
	if (m_rson[m_dad[p]] == p) m_rson[m_dad[p]] = r;
	else					   m_lson[m_dad[p]] = r;
	m_dad[p] = NIL;	 /* p ���O�� */
}

void CLZSS::delete_node(int p)	/* �� p ��؂������ */
{
	int	 q;

	if (m_dad[p]  == NIL) return;  /* ������Ȃ� */
	if (m_rson[p] == NIL) q = m_lson[p];
	else if (m_lson[p] == NIL) q = m_rson[p];
	else {
		q = m_lson[p];
		if (m_rson[q] != NIL) {
			do {  q = m_rson[q];  } while (m_rson[q] != NIL);
			m_rson[m_dad[q]] = m_lson[q];  m_dad[m_lson[q]] = m_dad[q];
			m_lson[q] = m_lson[p];	m_dad[m_lson[p]] = q;
		}
		m_rson[q] = m_rson[p];	m_dad[m_rson[p]] = q;
	}
	m_dad[q] = m_dad[p];
	if (m_rson[m_dad[p]] == p)	m_rson[m_dad[p]] = q;
	else						m_lson[m_dad[p]] = q;
	m_dad[p] = NIL;
}