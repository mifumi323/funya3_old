#include "stdafx.h"

//	�����ɑ��݂���֐���CDIB32��alpha�t�̏����֐��ł����āA
//	class CDIB32Alpha�̂��߂̏����֐��ł͂Ȃ�

#ifdef USE_DIB32

#include "yaneDIB32.h"

//	���`�����l���t���G�t�F�N�g�n
LRESULT CDIB32P5::FadeAlpha(int nFade,LPRECT lpRect){
	//	�v���[���̃��l�𗎂Ƃ��@�\	added '00/11/07
	//	�蔲�������ǁA����Ȃ�ɑ����̂ŁA������[�O�O�G

	WARNING(m_lpdwSrc == NULL,"CDIB32P5::FadeAlpha��m_lpdwSrc == NULL");

	if (nFade==256) return 0;	//	�����Ȃ�

	RECT r = GetClipRect(lpRect);
	LONG lPitch	 = GetRect()->right;
	DWORD* pSurface = GetPtr();

	static DWORD dwTable[256];
	static int nSelectTable = -1;
	if (nSelectTable != nFade){
	//	�e�[�u���̍��Ȃ���
		nSelectTable = nFade;
		nFade <<= 16;	//�@�Q�^����
		DWORD dw = 0;
		for(int i=0;i<256;++i,dw+=nFade)
			dwTable[i] = dw & 0xff000000;
	}

	for(int y=r.top;y<r.bottom;y++){
		DWORD *p = pSurface + y*lPitch + r.left;
		for(int x=r.left;x<r.right;x++){
			DWORD dw = *p;
			//	���������e�[�u���Q�Ƃŕϊ����āA����24�r�b�g�͖��ύX
			dw =  (dwTable[(dw & 0xff000000)>>24])
				+ (dw & 0xffffff);
			*(p++) = dw;
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
LRESULT CDIB32P5::BltToAlpha(CDIB32* lpSrc,int nSrcMin,int nSrcMax,int nDstMin,int nDstMax,LPRECT lpRect){
	//	�v���[���̃��l�𗎂Ƃ��@�\	added '00/11/07
	//	�蔲�������ǁA����Ȃ�ɑ����̂ŁA������[�O�O�G

	if (lpSrc->GetRect()->right!=GetRect()->right ||
		lpSrc->GetRect()->bottom!=GetRect()->bottom){
		//	�]�����Ɠ]����Ƃ̃v���[���̃T�C�Y���Ⴄ���I
		return 1;
	}

	WARNING(m_lpdwSrc == NULL,"CDIB32P5::BltToAlpha��m_lpdwSrc == NULL");
	{
		CDIB32Base* p = lpSrc->GetDIB32BasePtr();
		WARNING(p->GetPtr() == NULL,"CDIB32P5::BltToAlpha��p->GetPtr() == NULL");
	}

	RECT r = GetClipRect(lpRect);
	LONG lPitch	 = GetRect()->right;
	DWORD* pSurface = lpSrc->GetPtr();
	DWORD* pSurface2 = GetPtr();

	static DWORD dwTable[256];
	//	�e�[�u���͖�����Ȃ���
	{
		int i,j;
		DWORD dw;
		dw = nDstMin << 24;
		for(i=0;i<=nSrcMin && i<256;++i)
			dwTable[i] = dw;
		i = nSrcMin+1; if (i<0) i=0;
		for(j=1;i<nSrcMax && i<256;++i,++j) {
			dw = (DWORD)(nDstMin + j * (nDstMax-nDstMin) / (nSrcMax - nSrcMin));
			dwTable[i] = (dw<<24) & 0xff000000;
		}
		dw = nDstMax << 24;
		i=nSrcMax; if (i<0) i=0;
		for(;i<256;++i)
			dwTable[i] = dw;

	}

	for(int y=r.top;y<r.bottom;y++){
		DWORD *p = pSurface + y*lPitch + r.left;	// src
		DWORD *q = pSurface2+ y*lPitch + r.left;	// dst
		for(int x=r.left;x<r.right;x++){
			DWORD dw = *(p++);
			//	B�������e�[�u���Q�Ƃŕϊ����āA����
			dw =  dwTable[dw & 0xff];
			//	dst�̉���24�r�b�g�͖��ύX
			*q = (*q & 0xffffff) + dw;
			q++;
		}
	}

	return 0;
}
///////////////////////////////////////////////////////////////////////////////
LRESULT CDIB32P5::FadeBltAlpha(CDIB32* lpSrc,int nFade,LPRECT lpRect){
	//	�v���[���̃��l�𗎂Ƃ��Ȃ���]������@�\	added '00/11/08
	//	�蔲�������ǁA����Ȃ�ɑ����̂ŁA������[�O�O�G

	if (lpSrc->GetRect()->right!=GetRect()->right ||
		lpSrc->GetRect()->bottom!=GetRect()->bottom){
		//	�]�����Ɠ]����Ƃ̃v���[���̃T�C�Y���Ⴄ���I
		return 1;
	}

	WARNING(m_lpdwSrc == NULL,"CDIB32P5::FadeBltAlpha��m_lpdwSrc == NULL");
	{
		CDIB32Base* p = lpSrc->GetDIB32BasePtr();
		WARNING(p->GetPtr() == NULL,"CDIB32P5::FadeBltAlpha��p->GetPtr() == NULL");
	}

	RECT r = GetClipRect(lpRect);
	LONG lPitch	 = GetRect()->right;
	DWORD* pSurface = lpSrc->GetPtr();
	DWORD* pSurface2 = GetPtr();

	static DWORD dwTable[256];
	static int nSelectTable = -1;
	if (nSelectTable != nFade){
	//	�e�[�u���̍��Ȃ���
		nSelectTable = nFade;
		nFade <<= 16;	//�@�Q�^����
		DWORD dw = 0;
		for(int i=0;i<256;++i,dw+=nFade)
			dwTable[i] = dw & 0xff000000;
	}

	for(int y=r.top;y<r.bottom;y++){
		DWORD *p = pSurface + y*lPitch + r.left;	// src
		DWORD *q = pSurface2+ y*lPitch + r.left;	// dst
		for(int x=r.left;x<r.right;x++){
			DWORD dw = *(p++);
			//	�]�������������e�[�u���Q�Ƃŕϊ����āA�]���惿��
			*(q++) = (dw & 0xffffff) + dwTable[dw >> 24];
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//	���`�����l���t���]���n

//////////////////////////////////////
//	BltClearAlpha
//	�����F�L��̓]��(���l������)
//////////////////////////////////////
LRESULT CDIB32P5::BltClearAlpha(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32P5::BltClearAlpha��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32P5::BltClearAlpha��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;
	DWORD	lPitchSrc = p->m_lPitch;
	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth =	  m_lPitch - (nWidth<<2);									// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����

	DWORD	nonAlphaMask = 0x00ffffff;

// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:��	���l:��	 --------------
	if ( m_bActualSize == true )
	{
		DWORD		colKey = p->m_dwColorKey;


		_asm
		{
			MOV		ECX, nWidth
			MOV		EDX, nHeight

			MOV		EDI, lpDst
			MOV		ESI, lpSrc

			MOV		EBX, colKey				// UnPair

		LoopX_22:	// 6(5)�N���b�N��T�C�N��
			MOV		EAX, [ESI]
			ADD		ESI, 4

			AND		EAX, nonAlphaMask		//	UnPair			//	���l���}�X�N����

			CMP		EAX, EBX
			JE		Skip_22

			MOV		[EDI], EAX				// UnPair

		Skip_22:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		ECX

			JNZ		LoopX_22				// UnPair

			MOV		ECX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			DEC		EDX

			JNZ		LoopX_22				// UnPair
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:�L	���l:��	 --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		int		i, j;
		BYTE*	lpSrcBack;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;
		LoopY_23:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_23: // 8(7)�N���b�N��T�C�N��
			MOV		EAX, [ESI]									// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			AND		EAX, nonAlphaMask		//	UnPair			//	���l���}�X�N����

			CMP		EAX, colKey
			JE		SkipColKey_23

			MOV		[EDI], EAX				// UnPair

		SkipColKey_23:
			ADD		EDI, 4					// UnPair			// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_23									// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_23:
			DEC		i
			JNZ		LoopX_23

			MOV		ESI, lpSrcBack;								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_23				// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc
			SUB		EDX, EY2				// UnPair			// Y�̕␳
			
		SkipY_23:
			DEC		j
			JNZ		LoopY_23
		}
	} // if

	
	return 0;
} // BltClearAlpha


//////////////////////////////////////
//	BltFastClearAlpha
//	�����F�����̓]�� (���l�͖���)
//////////////////////////////////////
LRESULT CDIB32P5::BltFastClearAlpha(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32P5::BltFastClearAlpha��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32P5::BltFastClearAlpha��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);				// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����

	DWORD	nonAlphaMask = 0x00ffffff;


// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:��	���l:��	 --------------
	if ( m_bActualSize == true )
	{
		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, nHeight
			MOV		ECX, nWidth

		LoopX_24:	// 4�N���b�N��T�C�N��
			MOV		EAX, [ESI]
			ADD		EDI, 4										// EDI���ɐi�߂�

			AND		EAX, nonAlphaMask							//	���l���}�X�N����
			ADD		ESI, 4

			MOV		[EDI-4], EAX
			DEC		ECX

			JNZ		LoopX_24				// UnPair

			MOV		ECX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			DEC		EDX

			JNZ		LoopX_24				// UnPair
		}
	} // if
// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:�L	���l:��	 --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		int		i, j;
		BYTE*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;
		

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_25:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX									// EY = InitializeX;

		LoopX_25: // 6�N���b�N��T�C�N��
			MOV		EAX, [ESI]									// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			AND		EAX, nonAlphaMask		//	UnPair			//	���l���}�X�N����

			MOV		[EDI], EAX
			ADD		EDI, 4										// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_25									// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_25:
			DEC		i
			JNZ		LoopX_25

			MOV		ESI, lpSrcBack								// Src�����C���̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_25					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc			// 1���C�������Z���āA���̍s��
			SUB		EDX, EY2									// Y�̕␳

		SkipY_25:
			DEC		j
			JNZ		LoopY_25
		}
	} // if

	
	return 0;
} // BltFastClearAlpha


///////////////////////////////////////////////
//	BlendBltFastAlpha
//	�����F�֌W�Ȃ��̃��l���f�u�����h�]��
///////////////////////////////////////////////
LRESULT CDIB32P5::BlendBltFastAlpha(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32P5::BlendBltFastAlpha��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32P5::BlendBltFastAlpha��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);			// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����

	DWORD	alphaMask = 0xff000000;

// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:��	 ���l:���f	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

		LoopX_10: // 35�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������59�N���b�N)
			MOV		EAX, [ESI]
			ADD		ESI, 4

			MOV		EBX, EAX				// UnPair

			AND		EBX, alphaMask			// UnPair			// Src�̃��l�𓾂�

			MOV		ECX, EBX
			MOV		EDX, EBX

			SHR		EBX, 24					// UnPair

			SHR		ECX, 16					// UnPair

			SHR		EDX, 8
			ADD		EBX, ECX

			ADD		EBX, EDX				// UnPair

			MOV		EDX, EBX				// UnPair

			AND		EDX, 0x00ff00ff			// UnPair			// bgRateSrc�́A0x00aa00aa�ɂ���
			// �␮
			CMP		EDX, 0x00ff00ff								//	255���ۂ��̔���
			JNE		Skip_ManiA10

			MOV		EDX, 0x01000100			// UnPair			//	255��256�ɕ␮����

		Skip_ManiA10:
			MOV		bgRateSrc, EDX								//	Src�̃��l�Z�o����

			NOT		EBX						// UnPair

			AND		EBX, 0x00ff00ff			// bgRateDst�́A0x00aa00aa�ɂ���
			AND		EDX, 0x000001ff
			// �␮
			CMP		EBX, 0x00ff00ff								//	255���ۂ��̔���
			JNE		Skip_ManiB10

			MOV		EBX, 0x01000100			// UnPair			//	255��256�ɕ␮����

		Skip_ManiB10:
			MOV		bgRateDst, EBX								//	Dst�̃��l�Z�o����
			AND		EBX, 0x01ff0000

			OR		EBX, EDX
			MOV		ECX, [EDI]				// 2

			MOV		rRate, EBX				// rRate�́A0x00da00sa������
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 16
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0x00ff0000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			MOV		SrcBackup, EAX			// 1
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		ECX, EDX				// 3
			MOV		EAX, EBX				// 2

			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 16
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_10				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_10
		}
	}
// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:�L	 ���l:���f	  --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;


		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EDI, lpDst
			MOV		EAX, EIY

			MOV		ESI, lpSrc
			MOV		nEYCnt, EAX									// EY = InitializeY;

		LoopY_13:
			MOV		lpSrcBack, ESI
			MOV		EAX, EIX				// UnPair

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_13: // 37�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������61�N���b�N)
			MOV		EAX, [ESI]									// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			MOV		EBX, EAX				// UnPair

			AND		EBX, alphaMask			// UnPair			// Src�̃��l�𓾂�

			MOV		ECX, EBX
			MOV		EDX, EBX

			SHR		EBX, 24					// UnPair

			SHR		ECX, 16					// UnPair

			SHR		EDX, 8
			ADD		EBX, ECX

			ADD		EBX, EDX				// UnPair

			MOV		EDX, EBX				// UnPair

			AND		EDX, 0x00ff00ff			// UnPair			// bgRateSrc�́A0xaa0000aa�ɂ���
			// �␮
			CMP		EDX, 0x00ff00ff								//	255���ۂ��̔���
			JNE		Skip_ManiA13

			MOV		EDX, 0x01000100			// UnPair			//	255��256�ɕ␮����

		Skip_ManiA13:
			MOV		bgRateSrc, EDX								//	Src�̃��l�Z�o����

			NOT		EBX						// UnPair

			AND		EBX, 0x00ff00ff			// bgRateDst�́A0xaa0000aa�ɂ���
			AND		EDX, 0x000001ff
			// �␮
			CMP		EBX, 0x00ff00ff								//	255���ۂ��̔���
			JNE		Skip_ManiB13

			MOV		EBX, 0x01000100			// UnPair			//	255��256�ɕ␮����

		Skip_ManiB13:
			MOV		bgRateDst, EBX								//	Dst�̃��l�Z�o����
			AND		EBX, 0x1ff0000

			OR		EBX, EDX
			MOV		ECX, [EDI]				// 2

			MOV		rRate, EBX				// rRate�́A0xda0000sa������
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 16
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0x00ff0000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			MOV		SrcBackup, EAX			// 1
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		ECX, EDX				// 3
			MOV		EAX, EBX				// 2// UnPair

//			MUL		bgRateSrc				// UnPair
			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 16
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// Dst�����ɐi�߂�
			MOV		EAX, EX

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_13										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_13:
			DEC		i
			JNZ		LoopX_13

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_13									// if ( EY >= 0 )

			ADD		ESI, lPitchSrc				// 1���C�������Z���āA���̍s��
			MOV		EAX, EY2

			SUB		nEYCnt, EAX									// Y�̕␳

		SkipY_13:
			DEC		j
			JNZ		LoopY_13
		}
	} // if


	
	return 0;
} // BlendBltFastAlpha

// kaine 2001/2/28
///////////////////////////////////////////////
//	BlendBltFastAlpha
//	�����F�֌W�Ȃ��̃��l���f�u�����h�䗦�w��]��
///////////////////////////////////////////////
LRESULT CDIB32P5::BlendBltFastAlpha(CDIB32* lpDIB32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
							   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BlendBltFast��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p == NULL || p->GetPtr() == NULL,"CDIB32Base::BlendBltFast��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);			// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����

	DWORD	alphaMask = 0xff000000;

	// (E-dwDstRGBRate)[alpha] = (E-dwDstRGBRate) * alpha
	DWORD	dwDstR = 255 - (   dwDstRGBRate >> 16) ;
	DWORD	dwDstG = 255 - ( ( dwDstRGBRate & 0x0000ff00) >> 8 ) ;
	DWORD	dwDstB = 255 - (   dwDstRGBRate & 0x000000ff) ;
	DWORD	dwSrcR = dwSrcRGBRate >> 16 ;
	DWORD	dwSrcG = ( dwSrcRGBRate & 0x0000ff00) >> 8	;
	DWORD	dwSrcB = dwSrcRGBRate & 0x000000ff ;
	static DWORD	dwDstGBRate[256];
	static DWORD	dwSrcGBRate[256];
	static DWORD	dwRRate[256];
	DWORD	dR,dG,dB,sR,sG,sB;
	dR = dG = dB = sR =sG=sB=0;
	for ( int k = 0	 ; k < 256 ; k++, 
					dR+=dwDstR,dG+=dwDstG,dB+=dwDstB,
					sR+=dwSrcR,sG+=dwSrcG,sB+=dwSrcB
		){
		if ( k == 255 ) {
			dR+=dwDstR;dG+=dwDstG;dB+=dwDstB;
			sR+=dwSrcR;sG+=dwSrcG;sB+=dwSrcB;}
		dwDstGBRate[k]	= ( (~ ( dB >> 8 ) << 24 & 0xff000000) ) | ( ~( dG >> 8 ) & 0x000000ff) ;
		dwSrcGBRate[k]	= ( ( sB >> 8 ) << 24 ) | ( sG >> 8 ) ;
		dwRRate[k]		= ( ~( dR >> 8 ) << 24 ) | ( sR >> 8 ) ;
	}
	
// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:��	 RGB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
//		DWORD	DstBackup;
		int		i, j;
		DWORD	*lpDstGBRate = &dwDstGBRate[0];
		DWORD	*lpSrcGBRate = &dwSrcGBRate[0];
		DWORD	*lpRRate = &dwRRate[0];
	
		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

		LoopX_12: // ??�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������ �N���b�N)
			mov		eax, [ESI]
			mov		ebx, lpSrcGBRate

			and		eax, 0xff000000
			mov		ecx, lpDstGBRate

			shr		eax,22
			ADD		EDI,4

			mov		edx, lpRRate // rate0
			ADD		ESI,8

			mov		ebx,[ebx+eax] // Src2
			mov		ecx,[ecx+eax] // Dst2
			
			mov		bgRateSrc,ebx // Src3
			ADD		EDI,-4

			mov		edx,[edx+eax] // rate2
			mov		bgRateDst,ecx // Dst3

			MOV		EAX, [ESI-8]
			mov		rRate,edx //rate3

			ADD		ESI,-4
//------------------------------
			MOV		ECX, [EDI]				// 2
			MOV		EBX, EAX				// 1

			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����
			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0xff000000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			SHR		EDX, 8					// 3
			MOV		SrcBackup, EAX			// 1

			MOV		ECX, EDX				// 3
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		EAX, EBX				// 2// UnPair

			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_12				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_12
		}
	}// if
// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:�L	 RGB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;
		DWORD	*lpDstGBRate = &dwDstGBRate[0];
		DWORD	*lpSrcGBRate = &dwSrcGBRate[0];
		DWORD	*lpRRate = &dwRRate[0];


		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EDI, lpDst

			MOV		ESI, lpSrc
			MOV		EAX, EIY

			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_13:
			MOV		lpSrcBack, ESI
			MOV		EAX, EIX				// UnPair

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_13: // 37�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������61�N���b�N)
			MOV		EAX, [ESI]									// �s�N�Z���̃R�s�[
			mov		ebx, lpSrcGBRate

			and		eax, 0xff000000
			mov		ecx, lpDstGBRate

			shr		eax,22
			ADD		EDI,2

			mov		edx, lpRRate // rate0
			ADD		EDI,2

			mov		ebx,[ebx+eax] // Src2
			mov		ecx,[ecx+eax] // Dst2
			
			mov		bgRateSrc,ebx // Src3
			ADD		EDI,-4

			mov		edx,[edx+eax] // rate2
			mov		bgRateDst,ecx // Dst3

			mov		rRate,edx //rate3
//---------
			MOV		EAX, [ESI]
			ADD		ESI, AddSrcPixel							// �������̉��Z
			
			MOV		ECX, [EDI]				// 2
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0xff000000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			SHR		EDX, 8					// 3
			MOV		SrcBackup, EAX			// 1

			MOV		ECX, EDX				// 3
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		EAX, EBX				// 2// UnPair

			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// Dst�����ɐi�߂�
			MOV		EAX, EX

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_13										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_13:
			DEC		i
			JNZ		LoopX_13

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_13									// if ( EY >= 0 )

			ADD		ESI, lPitchSrc				// 1���C�������Z���āA���̍s��
			MOV		EAX, EY2

			SUB		nEYCnt, EAX									// Y�̕␳

		SkipY_13:
			DEC		j
			JNZ		LoopY_13
		}
	} // if

	
	return 0;
} // BlendBltFastAlpha

//////////////////////////////////////
//	BltClearAlphaM
//	�����F�L��̓]�� (���l������)
//////////////////////////////////////
LRESULT CDIB32P5::BltClearAlphaM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32P5::BltClearAlphaM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32P5::BltClearAlphaM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����

	DWORD	nonAlphaMask = 0x00ffffff;

// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ECX, nWidth
			MOV		EDX, nHeight

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EBX, colKey				// UnPair

		LoopY_1:

		LoopX_1:	// 5(4)�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			AND		EAX, nonAlphaMask		//	UnPair			//	���l���}�X�N����

			CMP		EAX, EBX
			JE		Skip_1

			MOV		[EDI], EAX				// UnPair

		Skip_1:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		ECX

			JNZ		LoopX_1					// UnPair

			MOV		ECX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			DEC		EDX

			JNZ		LoopY_1					// UnPair
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		int		i, j;
		BYTE*	lpSrcBack;
		DWORD	lPitchSrc =	 p->m_lPitch;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_2:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_2: // 7(6)�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]								// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			AND		EAX, nonAlphaMask		//	UnPair			//	���l���}�X�N����

			CMP		EAX, colKey
			JE		SkipColKey_2

			MOV		[EDI], EAX				// UnPair

		SkipColKey_2:
			ADD		EDI, 4					// UnPair			// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_2										// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_2:
			DEC		i
			JNZ		LoopX_2

			MOV		ESI, lpSrcBack;								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_2					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc
			SUB		EDX, EY2									// Y�̕␳

		SkipY_2:
			DEC		j
			JNZ		LoopY_2
		}
	} // if


	return 0;
} // BltClearAlphaM


//////////////////////////////////////
//	BltFastClearAlphaM
//	�����F�����̓]�� (���l����)
//////////////////////////////////////
LRESULT CDIB32P5::BltFastClearAlphaM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32P5::BltFastClearAlphaM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32P5::BltFastClearAlphaM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����

	DWORD	nonAlphaMask = 0x00ffffff;

// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, nHeight
			MOV		ECX, nWidth

		LoopX_3: // 3�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]
			ADD		EDI, 4

			AND		EAX, nonAlphaMask							//	���l���}�X�N����
			ADD		ESI, -4

			MOV		[EDI-4], EAX 
			DEC		ECX

			JNZ		LoopX_3					// UnPair

			MOV		ECX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			DEC		EDX

			JNZ		LoopX_3					// UnPair
		}
	} // if
// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:�L	 --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_4:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_4: // 5�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]								// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			AND		EAX, nonAlphaMask		//	UnPair			//	���l���}�X�N����

			MOV		[EDI], EAX
			ADD		EDI, 4										// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_4										// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_4:
			DEC		i
			JNZ		LoopX_4

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_4					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc
			SUB		EDX, EY2									// Y�̕␳

		SkipY_4:
			DEC		j
			JNZ		LoopY_4
		}
	} // if

	
	return 0;
} // BltFastClearAlphaM


//////////////////////////////////////
//	BlendBltFastAlphaM
//	�����F�֌W�Ȃ��̃��l���f�u�����h�]��
//////////////////////////////////////
LRESULT CDIB32P5::BlendBltFastAlphaM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32P5::BlendBltFastAlphaM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32P5::BlendBltFastAlphaM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����

	DWORD	alphaMask = 0xff000000;

// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:��	 RGB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX


		LoopX_9: // 35�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������59�N���b�N)
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			MOV		EBX, EAX				// UnPair

			AND		EBX, alphaMask			// UnPair			// Src�̃��l�𓾂�

			MOV		ECX, EBX
			MOV		EDX, EBX

			SHR		EBX, 24					// UnPair

			SHR		ECX, 16					// UnPair
			
			SHR		EDX, 8
			ADD		EBX, ECX

			ADD		EBX, EDX				// UnPair

			MOV		EDX, EBX				// UnPair

			AND		EDX, 0x00ff00ff			// UnPair			// bgRateSrc�́A0x00aa00aa�ɂ���
			// �␮
			CMP		EDX, 0x00ff00ff								//	255���ۂ��̔���
			JNE		Skip_ManiA9

			MOV		EDX, 0x01000100			// UnPair			//	255��256�ɕ␮����

		Skip_ManiA9:
			MOV		bgRateSrc, EDX								//	Src�̃��l�Z�o����

			NOT		EBX						// UnPair

			AND		EBX, 0x00ff00ff			// bgRateDst�́A0x00aa00aa�ɂ���
			AND		EDX, 0x000001ff
			// �␮
			CMP		EBX, 0x00ff00ff								//	255���ۂ��̔���
			JNE		Skip_ManiB9

			MOV		EBX, 0x01000100			// UnPair			//	255��256�ɕ␮����

		Skip_ManiB9:
			MOV		bgRateDst, EBX								//	Dst�̃��l�Z�o����
			AND		EBX, 0x01ff0000

			OR		EBX, EDX
			MOV		ECX, [EDI]				// 2

			MOV		rRate, EBX				// rRate�́A0x00da00sa������
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 16
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0x00ff0000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			MOV		SrcBackup, EAX			// 1
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		ECX, EDX				// 3
			MOV		EAX, EBX				// 2// UnPair

//			MUL		bgRateSrc				// UnPair
			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 16
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_9				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_9
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 RGB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EDI, lpDst
			MOV		EAX, EIY

			MOV		ESI, lpSrc
			MOV		nEYCnt, EAX									// EY = InitializeY;

		LoopY_10:
			MOV		lpSrcBack, ESI
			MOV		EAX, EIX				// UnPair			// �y�A�ɂȂ�Ȃ����ǁE�E�E�܂��Ay���[�v�������炢����:p

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_10: // 34�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������58�N���b�N)
			MOV		EAX, [ESI-4]								// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			MOV		EBX, EAX				// UnPair

			AND		EBX, alphaMask			// UnPair			// Src�̃��l�𓾂�

			MOV		ECX, EBX
			MOV		EDX, EBX

			SHR		EBX, 24					// UnPair

			SHR		ECX, 16					// UnPair
			
			SHR		EDX, 8
			ADD		EBX, ECX

			ADD		EBX, EDX				// UnPair

			MOV		EDX, EBX				// UnPair

			AND		EDX, 0x00ff00ff			// UnPair			// bgRateSrc�́A0x00aa00aa�ɂ���
			// �␮
			CMP		EDX, 0x00ff00ff								//	255���ۂ��̔���
			JNE		Skip_ManiA10

			MOV		EDX, 0x01000100			// UnPair			//	255��256�ɕ␮����

		Skip_ManiA10:
			MOV		bgRateSrc, EDX								//	Src�̃��l�Z�o����

			NOT		EBX						// UnPair

			AND		EBX, 0x00ff00ff			// bgRateDst�́A0x00aa00aa�ɂ���
			AND		EDX, 0x000001ff
			// �␮
			CMP		EBX, 0x00ff00ff								//	255���ۂ��̔���
			JNE		Skip_ManiB10

			MOV		EBX, 0x01000100			// UnPair			//	255��256�ɕ␮����

		Skip_ManiB10:
			MOV		bgRateDst, EBX								//	Dst�̃��l�Z�o����
			AND		EBX, 0x01ff0000

			OR		EBX, EDX
			MOV		ECX, [EDI]				// 2

			MOV		rRate, EBX				// rRate�́A0xda0000sa������
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 16
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0x00ff0000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			MOV		SrcBackup, EAX			// 1
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		ECX, EDX				// 3
			MOV		EAX, EBX				// 2// UnPair

//			MUL		bgRateSrc				// UnPair
			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 16
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// Dst�����ɐi�߂�
			MOV		EAX, EX

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_10									// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_10:
			DEC		i
			JNZ		LoopX_10

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY										// Y�̑��������Z

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_10									// if ( EY >= 0 )

			MOV		EAX, EY2									// Y�̕␳
			ADD		ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

			SUB		nEYCnt, EAX				// UnPair			// Y�̕␳

		SkipY_10:
			DEC		j
			JNZ		LoopY_10
		}
	} // if

	
	return 0;
} // BlendBltFastAlphaM

// kaine 2001/2/28
///////////////////////////////////////////////
//	BlendBltFastAlphaM
//	�����F�֌W�Ȃ��̃��l���f�u�����h�䗦�w��]��
///////////////////////////////////////////////
LRESULT CDIB32P5::BlendBltFastAlphaM(CDIB32* lpDIB32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
							   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32P5::BlendBltFastAlphaM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32P5::BlendBltFastAlphaM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����

	DWORD	alphaMask = 0xff000000;

	// (E-dwDstRGBRate)[alpha] = (E-dwDstRGBRate) * alpha
	DWORD	dwDstR = 255 - (   dwDstRGBRate >> 16) ;
	DWORD	dwDstG = 255 - ( ( dwDstRGBRate & 0x0000ff00) >> 8 ) ;
	DWORD	dwDstB = 255 - (   dwDstRGBRate & 0x000000ff) ;
	DWORD	dwSrcR = dwSrcRGBRate >> 16 ;
	DWORD	dwSrcG = ( dwSrcRGBRate & 0x0000ff00) >> 8	;
	DWORD	dwSrcB = dwSrcRGBRate & 0x000000ff ;
	static DWORD	dwDstGBRate[256];
	static DWORD	dwSrcGBRate[256];
	static DWORD	dwRRate[256];
	DWORD	dR,dG,dB,sR,sG,sB;
	dR = dG = dB = sR =sG=sB=0;
	for ( int k = 0	 ; k < 256 ; k++, 
					dR+=dwDstR,dG+=dwDstG,dB+=dwDstB,
					sR+=dwSrcR,sG+=dwSrcG,sB+=dwSrcB
		){
		if ( k == 255 ) {
			dR+=dwDstR;dG+=dwDstG;dB+=dwDstB;
			sR+=dwSrcR;sG+=dwSrcG;sB+=dwSrcB;}
		dwDstGBRate[k]	= ( (~ ( dB >> 8 ) << 24 & 0xff000000) ) | ( ~( dG >> 8 ) & 0x000000ff) ;
		dwSrcGBRate[k]	= ( ( sB >> 8 ) << 24 ) | ( sG >> 8 ) ;
		dwRRate[k]		= ( ~( dR >> 8 ) << 24 ) | ( sR >> 8 ) ;
	}
	
// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:��	 RGB�u�����h:�L	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
//		DWORD	DstBackup;
		int		i, j;
		DWORD	*lpDstGBRate = &dwDstGBRate[0];
		DWORD	*lpSrcGBRate = &dwSrcGBRate[0];
		DWORD	*lpRRate = &dwRRate[0];
		nSrcWidth = nSrcWidth + p->m_lPitch;

		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

		LoopX_9: // ??�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������ �N���b�N)
			mov		eax, [ESI-4]
			mov		ebx, lpSrcGBRate

			and		eax, 0xff000000
			mov		ecx, lpDstGBRate

			shr		eax,22
			ADD		EDI,2

			add		ebx,eax // Src1
			add		ecx,eax // Dst1

			mov		edx, lpRRate // rate0
			ADD		ESI,-2

			mov		ebx,[ebx] // Src2
			mov		ecx,[ecx] // Dst2
			
			mov		bgRateSrc,ebx // Src3
			add		edx,eax	 // rate1

			MOV		EAX, [ESI-4+2]
			ADD		EDI,-2

			mov		edx,[edx] // rate2
			mov		bgRateDst,ecx // Dst3
		
			mov		rRate,edx //rate3
			ADD		ESI,-2
//------------------------------
			MOV		ECX, [EDI]				// 2
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0xff000000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			SHR		EDX, 8					// 3
			MOV		SrcBackup, EAX			// 1

			MOV		ECX, EDX				// 3
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		EAX, EBX				// 2// UnPair

			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		i

			JNZ		LoopX_9				// UnPair

			MOV		EAX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			MOV		i, EAX

			DEC		j
			JNZ		LoopX_9
		}
	}// if
// -----------	 Pentium   �J���[�L�[:��   �~���[:�L  �g�k:�L  RGB�u�����h:�L	  --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;

		DWORD	feMask = 0x00fefefe;
		DWORD	ovMask = 0x01010100;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD	bgRateSrc;
		DWORD	bgRateDst;
		DWORD	rRate;
		DWORD	SrcBackup;
		int		nEXCnt, nEYCnt;
		int		i, j;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		DWORD	*lpDstGBRate = &dwDstGBRate[0];
		DWORD	*lpSrcGBRate = &dwSrcGBRate[0];
		DWORD	*lpRRate = &dwRRate[0];


		_asm
		{
			MOV		EAX, nHeight
			MOV		EBX, nWidth

			MOV		j, EAX
			MOV		i, EBX

			MOV		EDI, lpDst

			MOV		ESI, lpSrc
			MOV		EAX, EIY

			MOV		nEYCnt, EAX				// UnPair			// EY = InitializeY;

		LoopY_10:
			MOV		lpSrcBack, ESI
			MOV		EAX, EIX				// UnPair

			MOV		nEXCnt, EAX				// UnPair			// EX = InitializeX;

		LoopX_10: // 37�N���b�N��T�C�N�� (MUL�� 9�N���b�N�Ɛ������61�N���b�N)
			MOV		EAX, [ESI-4]									// �s�N�Z���̃R�s�[
			mov		ebx, lpSrcGBRate

			and		eax, 0xff000000
			mov		ecx, lpDstGBRate

			shr		eax,22
			ADD		EDI,2

			add		ebx,eax // Src1
			add		ecx,eax // Dst1

			mov		edx, lpRRate // rate0
			ADD		EDI,2

			mov		ebx,[ebx] // Src2
			mov		ecx,[ecx] // Dst2
			
			mov		bgRateSrc,ebx // Src3
			add		edx,eax	 // rate1

			MOV		EAX, [ESI-4]
			SUB		EDI,4

			mov		edx,[edx] // rate2
			mov		bgRateDst,ecx // Dst3
		
			mov		rRate,edx //rate3
			ADD		ESI, AddSrcPixel							// �������̉��Z
			
			MOV		ECX, [EDI]				// 2
			MOV		EBX, EAX				// 1

			ROR		EAX, 8					// 1				// ���q�f�a���a���q�f�ɂ���
			AND		ECX, 0x00ff0000			// 2				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHL		ECX, 8					// 2
			AND		EBX, 0x00ff0000			// 1				// �O�q�O�O�ɂ��邽�߂Ƀ}�X�N�����

			SHR		EBX, 16					// 1
			AND		EAX, 0xff0000ff			// 1				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MUL		bgRateSrc				// UnPair

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			OR		EBX, ECX									// dR�O�OsR���ł���

			MOV		ECX, EAX									// Src��0x0000GGBB���ł���
			MOV		EAX, EBX

			MUL		rRate					// UnPair			// EAX:Src�̂q�AEDX:Dst�̂q

			AND		EAX, 0x0000ff00			// 1
			MOV		EBX, [EDI]				// 2

			SHL		EAX, 8					// 1
			AND		EDX, 0xff000000			// 3

			ROR		EBX, 8					// 2				// ���q�f�a���a���q�f�ɂ���
			OR		EAX, ECX				// 1				// Src��0x00RRGGBB�̊���

			SHR		EDX, 8					// 3
			MOV		SrcBackup, EAX			// 1

			MOV		ECX, EDX				// 3
			AND		EBX, 0xff0000ff			// 2				// �a�O�O�f�ɂ��邽�߂Ƀ}�X�N�����

			MOV		EAX, EBX				// 2// UnPair

			MUL		bgRateDst				// UnPair			//	�Ԉ���Ă� by Tia

			SHR		EDX, 24
			AND		EAX, 0x0000ff00

			OR		EAX, EDX
			MOV		EBX, SrcBackup

			OR		EAX, ECX				// UnPair			// Dst��0x00RRGGBB�̊���

			// ---------- �T�`�����[�V����ADD ------------
			MOV		EDX, EAX									// DST�̕ۑ�
			MOV		ECX, EAX

			AND		EAX, EBX				// UnPair			// (src&dst)

			SHL		EAX, 1										// <<1
			XOR		ECX, EBX									// (src^dst)

			AND		ECX, feMask				// UnPair			// & feMask

			ADD		EAX, ECX				// UnPair			// ()+()

			AND		EAX, ovMask				// UnPair			// & ovMask
			
			SHR		EAX, 8										//	(c >> 8)
			MOV		EBX, SrcBackup

			ADD		EAX, Mask7f									// + 0x7f7f7f
			MOV		ECX, EDX

			XOR		EAX, Mask7f									//	^ 0x7f7f7f
			ADD		EBX, ECX									// src + dst

			SUB		EBX, EAX				// UnPair			// ( - c)

			OR		EBX, EAX				// UnPair			// () | mask

			MOV		[EDI], EBX				// UnPair

			ADD		EDI, 4										// Dst�����ɐi�߂�
			MOV		EAX, EX

			ADD		nEXCnt, EAX									// X�̑��������Z
			JNB		SkipX_10									// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			MOV		EAX, EX2

			SUB		nEXCnt, EAX				// UnPair			// X�̕␳

		SkipX_10:
			DEC		i
			JNZ		LoopX_10

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			MOV		EAX, EY										// Y�̑��������Z

			ADD		nEYCnt, EAX									// Y�̑��������Z
			JNB		SkipY_10									// if ( EY >= 0 )

			MOV		EAX, EY2									// Y�̕␳
			ADD		ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

			SUB		nEYCnt, EAX				// UnPair			// Y�̕␳

		SkipY_10:
			DEC		j
			JNZ		LoopY_10
		}
	} // if


	
	return 0;
} // BlendBltFastAlphaM blend


//////////////////////////////////////
//	BltClearAlpha
//	�����F�L��̓]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BltClearAlpha(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BltClearAlpha��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32PMMX::BltClearAlpha��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;
	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);			// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����

	DWORD	nonAlphaMask = 0x00ffffff;


// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:��	  ���l:����	 --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOVD		MM6, nonAlphaMask						//	���l�擾�p�}�X�N

			MOVQ		MM1, MM7
			MOVQ		MM2, MM6

			PUNPCKLDQ	MM7, MM1								//	ColKey
			MOV			EDI, lpDst

			PUNPCKLDQ	MM6, MM2								//	AlphaMask
			MOV			ESI, lpSrc

		LoopY_1:
			SHR			ECX, 1
			JNB			Skip_1									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM2, [EDI]
			PAND		MM0, MM6								//	���l���}�X�N���Ă���

			MOVQ		MM1, MM0			// UnPair

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1								// & mask
			ADD			EDI, 4

			PXOR		MM0, MM2			// UnPair			// src ^ ()
			NOP

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_1

		Skip_1:
			SHR			ECX, 1
			JNB			LoopX_1									//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			MOVQ		MM2, [EDI]
			PAND		MM0, MM6								//	���l���}�X�N���Ă���

			MOVQ		MM1, MM0			// UnPair

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask
			ADD			EDI, 8

			PXOR		MM0, MM2			// UnPair			// src ^ ()
			NOP

			ADD			ESI, 8				// AGI���

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_1

		LoopX_1: // 12�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// 1// UnPair

			MOVQ		MM3, [ESI+8]		// 2
			PAND		MM0, MM6			// 1				//	���l���}�X�N���Ă���

			MOVQ		MM1, MM0			// 1
			PAND		MM3, MM6			// 2				//	���l���}�X�N���Ă���

			MOVQ		MM4, MM3			// 2
			PCMPEQD		MM1, MM7			// 1				// ColKey:0xffffffff NonColKey:0x00000000

			MOVQ		MM2, [EDI]			// 1
			PCMPEQD		MM4, MM7			// 2				// ColKey:0xffffffff NonColKey:0x00000000

			MOVQ		MM5, [EDI+8]		// 2
			PXOR		MM2, MM0			// 1				// (Src ^ Dst)

			PXOR		MM5, MM3			// 2				// (Src ^ Dst)
			PAND		MM2, MM1			// 1				// & mask

			PAND		MM5, MM4			// 2				// &mask
			PXOR		MM0, MM2			// 1				// Src ^ ()

			PXOR		MM3, MM5			// 2				// Src ^ ()
			ADD			ESI, 16

			MOVQ		[EDI], MM0			// UnPair

			MOVQ		[EDI+8], MM3		// UnPair

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_1				// UnPair

		EndLoop_1:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_1				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:�L	  ���l:����	 --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD	lPitchSrc = p->m_lPitch;
		DWORD*	lpSrcBack;
	
		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOVD		MM6, nonAlphaMask						//	���l�擾�p�}�X�N

			MOVQ		MM1, MM7
			MOVQ		MM2, MM6

			PUNPCKLDQ	MM7, MM1								// ColKey
			MOV			ECX, nWidth

			PUNPCKLDQ	MM6, MM2								// AlphaMask
			MOV			EDX, nHeight

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_2:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX								// nExCnt = EIX;

		LoopX_2: // 10�N���b�N��T�C�N��
			MOVD		MM2, [ESI]			// UnPair			// *lpDst = *lpSrc;

			MOVD		MM4, [EDI]
			PAND		MM2, MM6								//	���l���}�X�N���Ă���

			MOVQ		MM3, MM2			// UnPair

			PCMPEQD		MM3, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM4, MM2								// (Src ^ Dst)

			PAND		MM4, MM3								// & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM2, MM4								// Src ^ ()
			ADD			EDI, 4									// lpDst++;
			
			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_2									// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_2:
			MOVD		[EDI-4], MM2		// UnPair

			DEC			ECX
			JNZ			LoopX_2

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_2									// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l

		SkipY_2:
			DEC			EDX
			JNZ			LoopY_2

			EMMS
		}
	} // if


	return 0;
} // BltClearAlpha


//////////////////////////////////////
//	BltFastClearAlpha
//	�����F�����̓]�� (���l����)
//////////////////////////////////////
LRESULT CDIB32PMMX::BltFastClearAlpha(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BltFastClearAlpha��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32PMMX::BltFastClearAlpha��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����

	DWORD	nonAlphaMask = 0x00ffffff;

// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	  ���l:����	 --------------
	if ( m_bActualSize == true )
	{
		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM6, nonAlphaMask						//	���l�擾�p�}�X�N
			MOV			EDI, lpDst

			MOVQ		MM0, MM6
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM6, MM0								//	alphaMask


		LoopY_3:
			SHR			ECX, 1
			JNB			Skip_3									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI]			// UnPair

			PAND		MM0, MM6								//	���l���}�X�N���Ă���
			ADD			EDI, 4									// lpDst++;

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair
			
			OR			ECX, ECX
			JZ			EndLoop_3

		Skip_3:
			SHR			ECX, 1
			JNB			LoopX_3									//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			PAND		MM0, MM6								//	���l���}�X�N���Ă���
			ADD			EDI, 8

			ADD			ESI, 8				// AGI���

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_3

		LoopX_3: // 6�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// UnPair

			MOVQ		MM3, [ESI+8]		// UnPair
			PAND		MM0, MM6								//	���l���}�X�N���Ă���

			PAND		MM3, MM6								//	���l���}�X�N���Ă���
			ADD			EDI, 16

			ADD			ESI, 16				// UnPair

			MOVQ		[EDI-16], MM0		// UnPair

			MOVQ		[EDI-8], MM3		// UnPair

			DEC			ECX
			JNZ			LoopX_3

		EndLoop_3:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_3				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:�L	  ���l:����	 --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			PXOR		MM1, MM1
			PXOR		MM0, MM0

			MOVD		MM6, nonAlphaMask						//	���l�擾�p�}�X�N
			MOV			ECX, nWidth

			MOVQ		MM2, MM6
			MOV			EDX, nHeight

			PUNPCKLDQ	MM6, MM2								//	alphaMask
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_4:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_4: // 7�N���b�N��T�C�N��
			MOVD		MM0, [ESI]			// UnPair			// *lpDst = *lpSrc;

			ADD			EDI, 4									// lpDst++;
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PAND		MM0, MM6								//	���l���}�X�N���Ă���
			ADD			EAX, EX									// EX += 2*DX;

			JNB			SkipX_4				// Unpair			// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_4:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_4

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_4									// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l
		SkipY_4:
			DEC			EDX
			JNZ			LoopY_4

			EMMS
		}
	} // if

	
	return 0;
} // BltFastClearAlpha


////////////////////////////////////////////
//	BlendBltFastAlpha
//	�����F�֌W�Ȃ��̃��l���f�u�����h�]��
////////////////////////////////////////////
LRESULT CDIB32PMMX::BlendBltFastAlpha(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BlendBltFastAlpha��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32PMMX::BlendBltFastAlpha��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����

	DWORD	alphaMask	= 0xff000000;
	DWORD	manipuMask	= 0x00010101;


// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	 ���l:���f	  --------------
	if ( m_bActualSize == true )
	{
		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			PXOR		MM7, MM7
			PXOR		MM4, MM4

			MOVD		MM7, alphaMask							//	0x00000000ff000000���쐬
			MOV			EDI, lpDst

			MOV			ESI, lpSrc			// UnPair

		LoopY_11:
		LoopX_11: // 26�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 30�N���b�N)
			MOVD		MM0, [ESI]

			MOVQ		MM2, MM0
			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�

			PAND		MM2, MM7			// UnPair			//	0xff000000ff000000�Ń��l�𓾂�

			MOVQ		MM1, MM2
			MOVQ		MM3, MM2

			PSRLD		MM2, 24
			PSRLD		MM1, 16

			PSRLD		MM3, 8
			PADDD		MM2, MM1

			PADDD		MM2, MM3			// UnPair			//	0x0000000000aaaaaa�Ń\�[�X�̃��l�͓���ꂽ

			PUNPCKLBW	MM2, MM4								//	WORD�P�ʂŏ�Z����̂�
			// �␮
			PCMPEQD		MM1, MM1								//	�̈ӂ�0xffffffffffffffff�����o��

			MOVQ		MM3, MM7								//	0xff000000
			MOVD		MM6, manipuMask							//	0x00010101

			PUNPCKLBW	MM6, MM4								//	0x0000000100010001���쐬
			PANDN		MM3, MM1								//	0x0000000000ffffff���쐬

			PUNPCKLBW	MM3, MM4			// UnPair			//	0x000000ff00ff00ff�ɂȂ�

			PCMPEQD		MM3, MM2			// UnPair			//	MM2=0x000000ff00ff00ff��������MM3=0xffffffffffffffff�ɂȂ�

			PAND		MM3, MM6			// UnPair			//	MM3=0xffffffffffffffff�Ȃ�MM3=0x0000000100010001�ɂȂ�

			PADDD		MM2, MM3			// UnPair			//	MM3=0x0000000000000000��MM3=0x0000000100010001�𑫂�

			// Dst
			MOVQ		MM5, MM2
			MOVD		MM6, manipuMask							//	0x00010101

			PUNPCKLBW	MM4, MM6			// MM4�؂�܂���	//	256 = 0x00 00 01 00 | 01 00 01 00

			PSUBW		MM4, MM5			// UnPair			//	256 - alpha
			
			MOVQ		MM5, MM4
			PXOR		MM4, MM4
			// �␮�I��
			MOVD		MM1, [EDI]			// UnPair

			PMULLW		MM0, MM2
			PUNPCKLBW	MM1, MM4

			PMULLW		MM1, MM5
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_11

			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_11			// UnPair

			EMMS
		}
	}
// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:�L	 ���l:���f	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			ECX, nWidth
			MOV			EDX, nHeight

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			PXOR		MM7, MM7
			PXOR		MM4, MM4

			MOVD		MM7, alphaMask							//	0x00000000ff000000���쐬
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_12:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_12: // 14�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 18�N���b�N)
			MOVD		MM0, [ESI]

			MOVQ		MM2, MM0
			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�

			PAND		MM2, MM7			// UnPair			//	0xff000000ff000000�Ń��l�𓾂�

			MOVQ		MM1, MM2
			MOVQ		MM3, MM2

			PSRLD		MM2, 24
			PSRLD		MM1, 16

			PSRLD		MM3, 8
			PADDD		MM2, MM1

			PADDD		MM2, MM3			// UnPair			//	0x0000000000aaaaaa�Ń\�[�X�̃��l�͓���ꂽ

			PUNPCKLBW	MM2, MM4								//	WORD�P�ʂŏ�Z����̂�
			// �␮
			PCMPEQD		MM1, MM1								//	�̈ӂ�0xffffffffffffffff�����o��

			MOVQ		MM3, MM7								//	0xff000000
			MOVD		MM6, manipuMask							//	0x00010101

			PUNPCKLBW	MM6, MM4								//	0x0000000100010001���쐬
			PANDN		MM3, MM1								//	0x0000000000ffffff���쐬

			PUNPCKLBW	MM3, MM4			// UnPair			//	0x000000ff00ff00ff�ɂȂ�

			PCMPEQD		MM3, MM2			// UnPair			//	MM2=0x000000ff00ff00ff��������MM3=0xffffffffffffffff�ɂȂ�

			PAND		MM3, MM6			// UnPair			//	MM3=0xffffffffffffffff�Ȃ�MM3=0x0000000100010001�ɂȂ�

			PADDD		MM2, MM3			// UnPair			//	MM3=0x0000000000000000��MM3=0x0000000100010001�𑫂�

			// Dst
			MOVQ		MM5, MM2
			MOVD		MM6, manipuMask							//	0x00010101

			PUNPCKLBW	MM4, MM6			// MM4�؂�܂���	//	256 = 0x00 00 01 00 | 01 00 01 00

			PSUBW		MM4, MM5			// UnPair			//	256 - alpha
			
			MOVQ		MM5, MM4
			PXOR		MM4, MM4
			// �␮�I��
			MOVD		MM1, [EDI]			// UnPair

			PMULLW		MM0, MM2
			PUNPCKLBW	MM1, MM4

			PMULLW		MM1, MM5
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4
			ADD			ESI, AddSrcPixel						// �������̉��Z

			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_12								// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_12:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_12

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_12								// if ( EY >= 0 )

			ADD			ESI, lPitchSrc							// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l

		SkipY_12:
			DEC			EDX
			JNZ			LoopY_12

			EMMS
		}
	} // if


	return 0;
} // BlendBltFastAlpha

// kaine 2001/2/28
////////////////////////////////////////////
//	BlendBltFastAlpha
//	�����F�֌W�Ȃ��̃��l���f�u�����h�䗦�w��]��
////////////////////////////////////////////
LRESULT CDIB32PMMX::BlendBltFastAlpha(CDIB32* lpDIB32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
							   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BlendBltFastAlpha��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32PMMX::BlendBltFastAlpha��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����

	DWORD	alphaMask	= 0xff000000;
	DWORD	manipuMask	= 0x00010101;

	// (E-dwDstRGBRate)[alpha] = (E-dwDstRGBRate) * alpha
	DWORD	dwDstR = 255 - (   dwDstRGBRate >> 16) ;
	DWORD	dwDstG = 255 - ( ( dwDstRGBRate & 0x0000ff00) >> 8 ) ;
	DWORD	dwDstB = 255 - (   dwDstRGBRate & 0x000000ff) ;
	DWORD	dwSrcR = dwSrcRGBRate >> 16 ;
	DWORD	dwSrcG = ( dwSrcRGBRate & 0x0000ff00) >> 8	;
	DWORD	dwSrcB = dwSrcRGBRate & 0x000000ff ;
	static DWORD	dwDstRGBRateT[256];
	static DWORD	dwSrcRGBRateT[256];
	DWORD	dR,dG,dB,sR,sG,sB;
	dR = dG = dB = sR =sG=sB=0;
	for ( int k = 0	 ; k < 256 ; k++, 
					dR+=dwDstR,dG+=dwDstG,dB+=dwDstB,
					sR+=dwSrcR,sG+=dwSrcG,sB+=dwSrcB
		){
		if ( k == 255 ) {
			dR+=dwDstR;dG+=dwDstG;dB+=dwDstB;
			sR+=dwSrcR;sG+=dwSrcG;sB+=dwSrcB;}
		dwDstRGBRateT[k] =	( ~( dR >> 8 ) << 16 & 0x00ff0000) |	
							( ~( dG >> 8 ) <<  8 & 0x0000ff00) |
							( ~( dB >> 8 )		 & 0x000000ff);
		dwSrcRGBRateT[k] =	(  ( sR >> 8 ) << 16 & 0x00ff0000) |	
							(  ( sG >> 8 ) <<  8 & 0x0000ff00) |
							(  ( sB >> 8 )		 & 0x000000ff);
	}
	

// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	 ���l:���f	RGB�u�����h:�L	 --------------
	if ( m_bActualSize == true )
	{
		
		DWORD	*lpDstRGBRate = &dwDstRGBRateT[0];
		DWORD	*lpSrcRGBRate = &dwSrcRGBRateT[0];

		_asm
		{
			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			MOV			EDX, nHeight
			MOV			ECX, nWidth

		LoopY_11:
		LoopX_11: // 26�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 30�N���b�N)

			PXOR		MM0, MM0
			mov			eax,[ESI]
			
			and			eax,0xff000000
			mov			ebx,lpSrcRGBRate

			shr			eax,22
			PXOR		MM1, MM1

			movd		mm0,[ESI]
			PXOR		MM2, MM2

			movd		mm2,[ebx+eax]
			PXOR		MM4, MM4

			mov			ebx,lpDstRGBRate
			PUNPCKLBW	MM2, MM4				// WORD�P�ʂŏ�Z����̂�

			MOVD		MM1, [EDI]			
			PUNPCKLBW	MM0, MM4					

			MOVD		MM3, [ebx+eax]		// UnPair

			PUNPCKLBW	MM3, MM4			
			NOP
// ------------			

			PMULLW		MM0, MM2
			PUNPCKLBW	MM1, MM4

			PMULLW		MM1, MM3
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_11

			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_11			// UnPair

			EMMS
		}
	}
// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:�L	 ���l:���f	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;
		DWORD	*lpDstRGBRate = &dwDstRGBRateT[0];
		DWORD	*lpSrcRGBRate = &dwSrcRGBRateT[0];

		_asm
		{
			MOV			ECX, nWidth
			MOV			EDX, nHeight

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			PXOR		MM7, MM7
			PXOR		MM4, MM4

			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_12:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_12: // 14�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 18�N���b�N)
			push		ebx
			push		eax

			pxor		mm0,mm0
			mov			eax,[ESI]
			
			and			eax,0xff000000
			mov			ebx,lpSrcRGBRate

			shr			eax,22
			PXOR		MM1, MM1

			movd		mm0,[ESI]
			pxor		mm2,mm2

			movd		mm2,[ebx+eax]
			PXOR		MM4, MM4

			mov			ebx,lpDstRGBRate
			PUNPCKLBW	MM2, MM4				// WORD�P�ʂŏ�Z����̂�

			MOVD		MM1, [EDI]			
			PUNPCKLBW	MM0, MM4					

			MOVD		MM3, [ebx+eax]		// UnPair

			PUNPCKLBW	MM3, MM4			
			nop

			pop			eax
			pop			ebx
//----
			PMULLW		MM0, MM2
			PUNPCKLBW	MM1, MM4

			PMULLW		MM1, MM3
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4
			ADD			ESI, AddSrcPixel						// �������̉��Z

			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_12								// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_12:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_12

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_12								// if ( EY >= 0 )

			ADD			ESI, lPitchSrc							// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l

		SkipY_12:
			DEC			EDX
			JNZ			LoopY_12

			EMMS
		}
	} // if


	return 0;
} // BlendBltFastAlpha blend



//////////////////////////////////////
//	BltClearAlphaM
//	�����F�L��̓]�� (���l������)
//////////////////////////////////////
LRESULT CDIB32PMMX::BltClearAlphaM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BltClearAlphaM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32PMMX::BltClearAlphaM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����

	DWORD	nonAlphaMask = 0x00ffffff;


// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOVD		MM6, nonAlphaMask						//	���l�擾�p�}�X�N

			MOVQ		MM0, MM7
			MOVQ		MM1, MM6

			PUNPCKLDQ	MM7, MM0								// ColKey
			MOV			EDI, lpDst

			PUNPCKLDQ	MM6, MM1								//	AlphaMask
			MOV			ESI, lpSrc

		LoopY_1:
			SHR			ECX, 1
			JNB			Skip_1									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]
			PAND		MM0, MM6								//	���l���}�X�N���Ă���

			MOVQ		MM1, MM0			// UnPair

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 4

			ADD			ESI, -4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_1

		Skip_1:
			SHR			ECX, 1
			JNB			LoopX_1									//	4�̔{�����ۂ��̔���
			// Step2
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [ESI-8]		// UnPair

			MOVQ		MM2, [EDI]
			PUNPCKLDQ	MM0, MM1								// �\�[�X�� 2�s�N�Z�����~���[����

			PAND		MM0, MM6			// UnPair			//	���l���}�X�N���Ă���

			MOVQ		MM1, MM0			// UnPair

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 8

			ADD			ESI, -8				// AGI���

			MOVQ		[EDI-8], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_1

		LoopX_1: // 15�N���b�N��T�C�N��
			// Step4
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM1, [ESI-8]		// 1// UnPair

			MOVD		MM3, [ESI-12]		// 2
			PUNPCKLDQ	MM0, MM1			// 1				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVD		MM4, [ESI-16]		// 2
			PAND		MM0, MM6			// 1				//	���l���}�X�N���Ă���

			MOVQ		MM2, [EDI]			// 1
			PUNPCKLDQ	MM3, MM4			// 2				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM5, [EDI+8]		// 2
			PAND		MM3, MM6			// 2				//	���l���}�X�N���Ă���

			MOVQ		MM1, MM0			// 1
			MOVQ		MM4, MM3			// 2

			PCMPEQD		MM1, MM7			// 1				// ColKey:0xffffffff NonColKey:0x00000000
			PCMPEQD		MM4, MM7			// 2				// ColKey:0xffffffff NonColKey:0x00000000

			PXOR		MM2, MM0			// 1				// (src ^ dst)
			PXOR		MM5, MM3			// 2				// (src ^ dst)

			PAND		MM2, MM1			// 1				// & mask
			PAND		MM5, MM4			// 2				// & mask

			PXOR		MM0, MM2			// 1				// src ^ ()
			PXOR		MM3, MM5			// 2				// src ^ ()

			ADD			ESI, -16			// AGI���

			MOVQ		[EDI], MM0			// 1// UnPair

			MOVQ		[EDI+8], MM3		// 2// UnPair

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_1				// UnPair

		EndLoop_1:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_1				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOVD		MM6, nonAlphaMask						//	���l�擾�p�}�X�N

			MOVQ		MM0, MM7
			MOVQ		MM1, MM6

			PUNPCKLDQ	MM7, MM0								// ColKey
			MOV			ECX, nWidth

			PUNPCKLDQ	MM6, MM1								//	AlphaMask
			MOV			EDX, nHeight

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_2:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_2: // 10�N���b�N��T�C�N��
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]
			PAND		MM0, MM6								//	���l���}�X�N���Ă���

			MOVQ		MM1, MM0			// UnPair

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1								// & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_2									// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_2:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_2

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_2										// if ( EY >= 0 )

			ADD			ESI, lPitchSrc
			SUB			EBX, EY2									// Y�̕␳

		SkipY_2:
			DEC			EDX
			JNZ			LoopY_2

			EMMS
		}
	} // if


	return 0;
} // BltClearAlphaM


//////////////////////////////////////
//	BltFastClearAlphaM
//	�����F�����̓]�� (���l����)
//////////////////////////////////////
LRESULT CDIB32PMMX::BltFastClearAlphaM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BltFastClearAlphaM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32PMMX::BltFastClearAlphaM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����

	DWORD	nonAlphaMask = 0x00ffffff;

// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM6, nonAlphaMask						//	���l�擾�p�}�X�N
			MOV			EDI, lpDst

			MOVQ		MM1, MM6
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM6, MM1								//	AlphaMask

		LoopY_3:
			SHR			ECX, 1
			JNB			Skip_3									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair
			ADD			EDI, 4

			PAND		MM0, MM6								//	���l���}�X�N���Ă���
			ADD			ESI, -4				// AGI���

			MOVD		[EDI-4], MM0		// UnPair

			OR			ECX, ECX
			JZ			EndLoop_3

		Skip_3:
			SHR			ECX, 1
			JNB			LoopX_3									//	4�̔{�����ۂ��̔���
			// Step2
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [ESI-8]		// UnPair

			PUNPCKLDQ	MM0, MM1			// UnPair			// �\�[�X�� 2�s�N�Z�����~���[����

			PAND		MM0, MM6								//	���l���}�X�N���Ă���
			ADD			EDI, 8

			ADD			ESI, -8				// AGI���

			MOVQ		[EDI], MM0			// UnPair

			OR			ECX, ECX
			JZ			EndLoop_3

		LoopX_3: // 9�N���b�N��T�C�N��							// ����������ƃA�����[�����������ǁE�E�E�܁A������:p
			// Step4
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM1, [ESI-8]		// 1// UnPair

			MOVD		MM3, [ESI-12]		// 2
			PUNPCKLDQ	MM0, MM1			// 1				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVD		MM4, [ESI-16]		// 2// UnPair
			PAND		MM0, MM6			// 1				//	���l���}�X�N���Ă���

			PUNPCKLDQ	MM3, MM4			// 2				// �\�[�X�� 2�s�N�Z�����~���[����
			ADD			EDI, 16

			PAND		MM3, MM6			// 2				//	���l���}�X�N���Ă���
			ADD			ESI, -16

			MOVQ		[EDI-16], MM0		// 1// UnPair

			MOVQ		[EDI-8], MM3		// 2// UnPair

			DEC			ECX
			JNZ			LoopX_3

		EndLoop_3:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_3				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:�L	 --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			MOV			ECX, nWidth
			MOV			EDX, nHeight

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_4:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_4: // 7�N���b�N��T�C�N��
			MOVD		MM0, [ESI-4]		// UnPair

			ADD			ESI, AddSrcPixel						// �������̉��Z
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_4									// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_4:
			MOVD		[EDI-4], MM0			// UnPair

			DEC			ECX
			JNZ			LoopX_4

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_4										// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_4:
			DEC			EDX
			JNZ			LoopY_4

			EMMS
		}
	} // if


	return 0;
} // BltFastClearAlphaM


///////////////////////////////////////////
//	BlendBltFastAlphaM
//	�����F�֌W�Ȃ��̃��l���f�u�����h�]��
///////////////////////////////////////////
LRESULT CDIB32PMMX::BlendBltFastAlphaM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BlendBltFastAlphaM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32PMMX::BlendBltFastAlphaM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����

	DWORD	alphaMask	= 0xff000000;
	DWORD	manipuMask	= 0x00010101;

// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:��	 ���l:���f	  --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			PXOR		MM7, MM7
			PXOR		MM4, MM4

			MOVD		MM7, alphaMask							//	0x00000000ff000000���쐬
			MOV			EDI, lpDst

			MOV			ESI, lpSrc			// UnPair

		LoopY_9:
		LoopX_9: // 26�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 30�N���b�N)
			MOVD		MM0, [ESI-4]

			MOVQ		MM2, MM0
			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�

			PAND		MM2, MM7			// UnPair			//	0xff000000ff000000�Ń��l�𓾂�

			MOVQ		MM1, MM2
			MOVQ		MM3, MM2

			PSRLD		MM2, 24
			PSRLD		MM1, 16

			PSRLD		MM3, 8
			PADDD		MM2, MM1

			PADDD		MM2, MM3			// UnPair			//	0x0000000000aaaaaa�Ń\�[�X�̃��l�͓���ꂽ

			PUNPCKLBW	MM2, MM4								//	WORD�P�ʂŏ�Z����̂�
			// �␮
			PCMPEQD		MM1, MM1								//	�̈ӂ�0xffffffffffffffff�����o��

			MOVQ		MM3, MM7								//	0xff000000
			MOVD		MM6, manipuMask							//	0x00010101

			PUNPCKLBW	MM6, MM4								//	0x0000000100010001���쐬
			PANDN		MM3, MM1								//	0x0000000000ffffff���쐬

			PUNPCKLBW	MM3, MM4			// UnPair			//	0x000000ff00ff00ff�ɂȂ�

			PCMPEQD		MM3, MM2			// UnPair			//	MM2=0x000000ff00ff00ff��������MM3=0xffffffffffffffff�ɂȂ�

			PAND		MM3, MM6			// UnPair			//	MM3=0xffffffffffffffff�Ȃ�MM3=0x0000000100010001�ɂȂ�

			PADDD		MM2, MM3			// UnPair			//	MM3=0x0000000000000000��MM3=0x0000000100010001�𑫂�

			// Dst
			MOVQ		MM5, MM2
			MOVD		MM6, manipuMask							//	0x00010101

			PUNPCKLBW	MM4, MM6			// MM4�؂�܂���	//	256 = 0x00 00 01 00 | 01 00 01 00

			PSUBW		MM4, MM5			// UnPair			//	256 - alpha
			
			MOVQ		MM5, MM4
			PXOR		MM4, MM4
			// �␮�I��
			MOVD		MM1, [EDI]			// UnPair

			PMULLW		MM0, MM2
			PUNPCKLBW	MM1, MM4

			PMULLW		MM1, MM5
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4
			ADD			EDI, 4

			ADD			ESI, -4

			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_9

			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_9				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 ���l:���f	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			PXOR		MM7, MM7
			PXOR		MM4, MM4

			MOVD		MM6, manipuMask							//	0x0000000100010001���쐬
			MOVD		MM7, alphaMask							//	0x00000000ff000000���쐬

			PUNPCKLBW	MM6, MM4								//	0x0000000100010001
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_10:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_10: // 18�N���b�N��T�C�N�� (PMUL�� 3�N���b�N���� 22�N���b�N)
			MOVD		MM0, [ESI-4]

			MOVQ		MM2, MM0
			PUNPCKLBW	MM0, MM4								// WORD�P�ʂŏ�Z����̂�

			PAND		MM2, MM7			// UnPair			//	0xff000000ff000000�Ń��l�𓾂�

			MOVQ		MM1, MM2
			MOVQ		MM3, MM2

			PSRLD		MM2, 24
			PSRLD		MM1, 16

			PSRLD		MM3, 8
			PADDD		MM2, MM1

			PADDD		MM2, MM3			// UnPair			//	0x0000000000aaaaaa�Ń\�[�X�̃��l�͓���ꂽ

			PUNPCKLBW	MM2, MM4								//	WORD�P�ʂŏ�Z����̂�
			// �␮
			PCMPEQD		MM1, MM1								//	�̈ӂ�0xffffffffffffffff�����o��

			MOVQ		MM3, MM7								//	0xff000000
			MOVD		MM6, manipuMask							//	0x00010101

			PUNPCKLBW	MM6, MM4								//	0x0000000100010001���쐬
			PANDN		MM3, MM1								//	0x0000000000ffffff���쐬

			PUNPCKLBW	MM3, MM4			// UnPair			//	0x000000ff00ff00ff�ɂȂ�

			PCMPEQD		MM3, MM2			// UnPair			//	MM2=0x000000ff00ff00ff��������MM3=0xffffffffffffffff�ɂȂ�

			PAND		MM3, MM6			// UnPair			//	MM3=0xffffffffffffffff�Ȃ�MM3=0x0000000100010001�ɂȂ�

			PADDD		MM2, MM3			// UnPair			//	MM3=0x0000000000000000��MM3=0x0000000100010001�𑫂�

			// Dst
			MOVQ		MM5, MM2
			MOVD		MM6, manipuMask							//	0x00010101

			PUNPCKLBW	MM4, MM6			// MM4�؂�܂���	//	256 = 0x00 00 01 00 | 01 00 01 00

			PSUBW		MM4, MM5			// UnPair			//	256 - alpha
			
			MOVQ		MM5, MM4
			PXOR		MM4, MM4
			// �␮�I��
			MOVD		MM1, [EDI]			// UnPair

			PMULLW		MM0, MM2
			PUNPCKLBW	MM1, MM4

			PMULLW		MM1, MM5
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4
			ADD			ESI, AddSrcPixel						// �������̉��Z

			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_10								// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_10:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_10

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_10									// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_10:
			DEC			EDX
			JNZ			LoopY_10

			EMMS
		}
	} // if

	
	return 0;
} // BlendBltFastAlphaM


// kaine 2001/2/28
////////////////////////////////////////////
//	BlendBltFastAlphaM blend
//	�����F�֌W�Ȃ��̃��l���f�u�����h�䗦�w��]��
////////////////////////////////////////////
LRESULT CDIB32PMMX::BlendBltFastAlphaM(CDIB32* lpDIB32,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
							   ,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BlendBltFastAlpha��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32PMMX::BlendBltFastAlpha��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����

	DWORD	alphaMask	= 0xff000000;
	DWORD	manipuMask	= 0x00010101;

	// (E-dwDstRGBRate)[alpha] = (E-dwDstRGBRate) * alpha
	DWORD	dwDstR = 255 - (   dwDstRGBRate >> 16) ;
	DWORD	dwDstG = 255 - ( ( dwDstRGBRate & 0x0000ff00) >> 8 ) ;
	DWORD	dwDstB = 255 - (   dwDstRGBRate & 0x000000ff) ;
	DWORD	dwSrcR = dwSrcRGBRate >> 16 ;
	DWORD	dwSrcG = ( dwSrcRGBRate & 0x0000ff00) >> 8	;
	DWORD	dwSrcB = dwSrcRGBRate & 0x000000ff ;
	static DWORD	dwDstRGBRateT[256];
	static DWORD	dwSrcRGBRateT[256];
	DWORD	dR,dG,dB,sR,sG,sB;
	dR = dG = dB = sR =sG=sB=0;
	for ( int k = 0	 ; k < 256 ; k++, 
					dR+=dwDstR,dG+=dwDstG,dB+=dwDstB,
					sR+=dwSrcR,sG+=dwSrcG,sB+=dwSrcB)
	{
		dwDstRGBRateT[k] =	( ~( dR >> 8 ) << 16 & 0x00ff0000) |	
							( ~( dG >> 8 ) <<  8 & 0x0000ff00) |
							( ~( dB >> 8 )		 & 0x000000ff);
		dwSrcRGBRateT[k] =	(  ( sR >> 8 ) << 16 & 0x00ff0000) |	
							(  ( sG >> 8 ) <<  8 & 0x0000ff00) |
							(  ( sB >> 8 )		 & 0x000000ff);
	}

	dwDstRGBRateT[255] =	( ~( dR >> 8 ) << 16 & 0x00ff0000) |	
							( ~( dG >> 8 ) <<  8 & 0x0000ff00) |
							( ~( dB >> 8 )		 & 0x000000ff);
	dwSrcRGBRateT[255] =	(  ( sR >> 8 ) << 16 & 0x00ff0000) |	
							(  ( sG >> 8 ) <<  8 & 0x0000ff00) |
							(  ( sB >> 8 )		 & 0x000000ff);


// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	 ���l:���f	RGB�u�����h:�L	 --------------
	if ( m_bActualSize == true )
	{
		nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����

		DWORD	*lpDstRGBRate = &dwDstRGBRateT[0];
		DWORD	*lpSrcRGBRate = &dwSrcRGBRateT[0];

		_asm
		{
			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			MOV			EDX, nHeight
			MOV			ECX, nWidth

		LoopY_9:
		LoopX_9: // 26�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 30�N���b�N)

			PXOR		MM0, MM0
			mov			eax,[ESI-4]
			
			and			eax,0xff000000
			mov			ebx,lpSrcRGBRate

			shr			eax,22
			PXOR		MM1, MM1

			movd		mm0,[ESI-4]
			PXOR		MM2, MM2

			movd		mm2,[ebx+eax]
			PXOR		MM4, MM4

			mov			ebx,lpDstRGBRate
			PUNPCKLBW	MM2, MM4				// WORD�P�ʂŏ�Z����̂�

			MOVD		MM1, [EDI]			
			PUNPCKLBW	MM0, MM4					

			MOVD		MM3, [ebx+eax]		// UnPair

			PUNPCKLBW	MM3, MM4			
			NOP
// ------------			

			PMULLW		MM0, MM2
			PUNPCKLBW	MM1, MM4

			PMULLW		MM1, MM3
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4
			ADD			EDI, 4

			ADD			ESI, -4

			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_9

			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_9				// UnPair

			EMMS
		}
	}
// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:�L	 ���l:���f	  --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	Mask7f = 0x007f7f7f;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;
		DWORD	*lpDstRGBRate = &dwDstRGBRateT[0];
		DWORD	*lpSrcRGBRate = &dwSrcRGBRateT[0];

		_asm
		{
			MOV			ECX, nWidth
			MOV			EDX, nHeight

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			PXOR		MM7, MM7
			PXOR		MM4, MM4

			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_10:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_10: // 14�N���b�N��T�C�N��(��Z�� 3�N���b�N�� 18�N���b�N)
			push		ebx
			push		eax

			pxor		mm0,mm0
			mov			eax,[ESI-4]
			
			and			eax,0xff000000
			mov			ebx,lpSrcRGBRate

			shr			eax,22
			PXOR		MM1, MM1

			movd		mm0,[ESI-4]
			pxor		mm2,mm2

			movd		mm2,[ebx+eax]
			PXOR		MM4, MM4

			mov			ebx,lpDstRGBRate
			PUNPCKLBW	MM2, MM4				// WORD�P�ʂŏ�Z����̂�

			MOVD		MM1, [EDI]			
			PUNPCKLBW	MM0, MM4					

			MOVD		MM3, [ebx+eax]		// UnPair

			PUNPCKLBW	MM3, MM4			
			nop

			pop			eax
			pop			ebx
//----
			PMULLW		MM0, MM2
			PUNPCKLBW	MM1, MM4

			PMULLW		MM1, MM3
			NOP

			NOP			// PMUL�X�g�[����� (2���ߒǉ�)
			NOP

			NOP
			NOP

			PADDUSB		MM0, MM1			// UnPair

			PSRLW		MM0, 8				// UnPair

			PACKUSWB	MM0, MM4
			ADD			ESI, AddSrcPixel						// �������̉��Z

			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_10								// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_10:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_10

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_10									// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_10:
			DEC			EDX
			JNZ			LoopY_10

			EMMS
		}
	} // if


	return 0;
} // BlendBltFastAlphaM blend


/*----------------------------------------------------------------------------
//						following lines are added by yaneurao				//
----------------------------------------------------------------------------*/


//////////////////////////////////////
//	BltWithoutAlpha(for stencil alpha effect)
//	�����F�L��̓]��
//////////////////////////////////////

LRESULT CDIB32P5::BltWithoutAlpha(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BltWithoutAlpha��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32Base::BltWithoutAlpha��p->GetPtr() == NULL");

// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;
	DWORD	lPitchSrc = p->m_lPitch;
	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth =	  m_lPitch - (nWidth<<2);									// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�
	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		DWORD		colKey = p->m_dwColorKey;


		_asm
		{
			MOV		ECX, nWidth
			MOV		EDX, nHeight

			MOV		EDI, lpDst
			MOV		ESI, lpSrc

			MOV		EBX, colKey				// UnPair

		LoopX_1:	// 5(4)�N���b�N��T�C�N��
			MOV		EAX, [ESI]
			ADD		ESI, 4

			CMP		EAX, EBX
			JE		Skip_1

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOV		[EDI], EAX				// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			mov		ebx,[edi]
			and		eax,0x00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			and		ebx,0xff000000	//	���l�����c��
			or		eax,ebx
			mov		[edi],eax

		Skip_1:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		ECX

			JNZ		LoopX_1					// UnPair

			MOV		ECX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			DEC		EDX

			JNZ		LoopX_1					// UnPair
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:��   �g�k:�L	 --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		int		i, j;
		BYTE*	lpSrcBack;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;
		LoopY_2:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_2: // 7(6)�N���b�N��T�C�N��
			MOV		EAX, [ESI]									// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			CMP		EAX, colKey
			JE		SkipColKey_2

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOV		[EDI], EAX				// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			mov		ebx,[edi]
			and		eax,0x00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			and		ebx,0xff000000	//	���l�����c��
			or		eax,ebx
			mov		[edi],eax

		SkipColKey_2:
			ADD		EDI, 4					// UnPair			// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_2										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_2:
			DEC		i
			JNZ		LoopX_2

			MOV		ESI, lpSrcBack;								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_2					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc
			SUB		EDX, EY2				// UnPair			// Y�̕␳
			
		SkipY_2:
			DEC		j
			JNZ		LoopY_2
		}
	} // if

	
	return 0;
} // BltWithoutAlpha


//////////////////////////////////////
//	BltFastWithoutAlpha
//	�����F�����̓]��
//////////////////////////////////////
LRESULT CDIB32P5::BltFastWithoutAlpha(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BltFastWithoutAlpha��m_lpdwSrc == NULL");
	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32Base::BltFastWithoutAlpha��p->GetPtr() == NULL");

// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);				// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		_asm {
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, nHeight
			MOV		ECX, nWidth

		LoopX_3: // ???�N���b�N�E�T�C�N��
			mov		eax,[esi]
			add		esi,4
		
			mov		ebx,[edi]
			and		eax,0x00ffffff
			and		ebx,0xff000000
			or		eax,ebx
			mov		[edi],eax

			add		edi,4

			dec		ecx
			jnz		LoopX_3

			MOV		ECX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nAddSrcWidth							// �N���b�v�����̈敪���΂�
			DEC		EDX

			JNZ		LoopX_3					// UnPair

		}
	} // if
// -----------	 Pentium   �J���[�L�[:��   �~���[:��   �g�k:�L	 --------------
	else
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		int		i, j;
		BYTE*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;
		

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_5:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX									// EY = InitializeX;

		LoopX_5: // 5�N���b�N��T�C�N��
			MOV		EAX, [ESI]									// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOV		[EDI], EAX				// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			mov		ebx,[edi]
			and		eax,0x00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			and		ebx,0xff000000	//	���l�����c��
			or		eax,ebx
			mov		[edi],eax

			ADD		EDI, 4										// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_5										// if ( EX >= 0)

			ADD		ESI, 4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_5:
			DEC		i
			JNZ		LoopX_5

			MOV		ESI, lpSrcBack								// Src�����C���̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_5					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc			// 1���C�������Z���āA���̍s��
			SUB		EDX, EY2									// Y�̕␳

		SkipY_5:
			DEC		j
			JNZ		LoopY_5
		}
	} // if

	
	return 0;
} // BltFastWithoutAlpha

////////////////////////////////////////////////////////////////////
//	�~���[�L���`�]��
////////////////////////////////////////////////////////////////////
//////////////////////////////////////
//	BltWithoutAlphaM
//	�����F�L��̓]��
//////////////////////////////////////
LRESULT CDIB32P5::BltWithoutAlphaM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BltWithoutAlphaM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32Base::BltWithoutAlphaM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ECX, nWidth
			MOV		EDX, nHeight

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EBX, colKey				// UnPair

		LoopY_1:

		LoopX_1:	// 5(4)�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			CMP		EAX, EBX
			JE		Skip_1

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOV		[EDI], EAX				// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			mov		ebx,[edi]
			and		eax,0x00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			and		ebx,0xff000000	//	���l�����c��
			or		eax,ebx
			mov		[edi],eax

		Skip_1:
			ADD		EDI, 4										// EDI���ɐi�߂�
			DEC		ECX

			JNZ		LoopX_1					// UnPair

			MOV		ECX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			DEC		EDX

			JNZ		LoopY_1					// UnPair
		}
	}// if
// -----------	 Pentium   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		int		i, j;
		BYTE*	lpSrcBack;
		DWORD	lPitchSrc =	 p->m_lPitch;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_2:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_2: // 7(6)�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]								// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			CMP		EAX, colKey
			JE		SkipColKey_2

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOV		[EDI], EAX				// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			mov		ebx,[edi]
			and		eax,0x00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			and		ebx,0xff000000	//	���l�����c��
			or		eax,ebx
			mov		[edi],eax

		SkipColKey_2:
			ADD		EDI, 4					// UnPair			// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_2										// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_2:
			DEC		i
			JNZ		LoopX_2

			MOV		ESI, lpSrcBack;								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_2					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc
			SUB		EDX, EY2									// Y�̕␳

		SkipY_2:
			DEC		j
			JNZ		LoopY_2
		}
	} // if


	return 0;
} // BltWithoutAlphaM


//////////////////////////////////////
//	BltFastWithoutAlphaM
//	�����F�����̓]��
//////////////////////////////////////
LRESULT CDIB32P5::BltFastWithoutAlphaM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32Base::BltFastWithoutAlphaM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32Base::BltFastWithoutAlphaM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;


	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����


		_asm
		{
			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, nHeight
			MOV		ECX, nWidth

		LoopX_3: // 3�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]
			ADD		ESI, -4

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOV		[EDI], EAX				// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			mov		ebx,[edi]
			and		eax,0x00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			and		ebx,0xff000000	//	���l�����c��
			or		eax,ebx
			mov		[edi],eax

			ADD		EDI, 4

			DEC		ECX
			JNZ		LoopX_3

			MOV		ECX, nWidth
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			ADD		ESI, nSrcWidth								// �����C���ւ̈ړ�
			DEC		EDX

			JNZ		LoopX_3					// UnPair
		}
	} // if
// -----------	 Pentium   �J���[�L�[:��   �~���[:�L   �g�k:�L	 --------------
	else
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		int		i, j;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV		EAX, nWidth
			MOV		EBX, nHeight

			MOV		i, EAX
			MOV		j, EBX

			MOV		ESI, lpSrc
			MOV		EDI, lpDst

			MOV		EDX, EIY				// UnPair			// EX = InitializeY;

		LoopY_4:
			MOV		lpSrcBack, ESI
			MOV		ECX, EIX				// UnPair			// EY = InitializeX;

		LoopX_4: // 5�N���b�N��T�C�N��
			MOV		EAX, [ESI-4]								// �s�N�Z���̃R�s�[
			ADD		ESI, AddSrcPixel							// �������̉��Z

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOV		[EDI], EAX				// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			mov		ebx,[edi]
			and		eax,0x00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			and		ebx,0xff000000	//	���l�����c��
			or		eax,ebx
			mov		[edi],eax

			ADD		EDI, 4										// Dst�����ɐi�߂�

			ADD		ECX, EX										// X�̑��������Z
			JNB		SkipX_4										// if ( EX >= 0)

			ADD		ESI, -4										// Src�����ɐi�߂�
			SUB		ECX, EX2									// X�̕␳

		SkipX_4:
			DEC		i
			JNZ		LoopX_4

			MOV		ESI, lpSrcBack								// Src�̐擪�ɖ߂�
			ADD		EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV		EAX, nWidth
			ADD		ESI, AddWidthSrc							// �������̉��Z '00.09.12.byTia

			MOV		i, EAX
			ADD		EDX, EY										// Y�̑��������Z

			JNB		SkipY_4					// UnPair			// if ( EY >= 0 )

			ADD		ESI, lPitchSrc
			SUB		EDX, EY2									// Y�̕␳

		SkipY_4:
			DEC		j
			JNZ		LoopY_4
		}
	} // if

	return 0;
} // BltFastWithoutAlphaM

////////////////////////////////////////////////////////////////////////////
//	PentiumMMX�p�̃��[�`��
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
//	�~���[������`�]��
////////////////////////////////////////////////////////////////////
//////////////////////////////////////
//	BltWithoutAlpha
//	�����F�L��̓]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BltWithoutAlpha(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BltWithoutAlpha��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32PMMX::BltWithoutAlpha��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;
	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);			// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;

			//	mask data setting by yane
			static DWORD mask1[2] = {0x00ffffff, 0x00ffffff};
			static DWORD mask2[2] = {0xff000000, 0xff000000};

		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM1, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM1			// UnPair			// ColKey

		LoopY_1:
			SHR			ECX, 1
			JNB			Skip_1									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI]			// UnPair

			MOVD		MM2, [EDI]
			MOVQ		MM1, MM0

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 4

			ADD			ESI, 4				// AGI���

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOVD		[EDI-4], MM0			// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movd	mm1,[edi-4]
			pand	mm0,mask1	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mask2	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movd	[edi-4],mm0

			OR			ECX, ECX
			JZ			EndLoop_1

		Skip_1:
			SHR			ECX, 1
			JNB			LoopX_1									//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			MOVQ		MM2, [EDI]
			MOVQ		MM1, MM0

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 8

			ADD			ESI, 8				// UnPair

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOVQ		[EDI-8], MM0			// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movq	mm1,[edi-8]
			pand	mm0,mask1	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mask2	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movq	[edi-8],mm0

			OR			ECX, ECX
			JZ			EndLoop_1

		LoopX_1: // 12�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// 1// UnPair

			MOVQ		MM3, [ESI+8]		// 2
			MOVQ		MM1, MM0			// 1

			MOVQ		MM4, MM3			// 2
			PCMPEQD		MM1, MM7			// 1				// ColKey:0xffffffff NonColKey:0x00000000

			MOVQ		MM2, [EDI]			// 1
			PCMPEQD		MM4, MM7			// 2				// ColKey:0xffffffff NonColKey:0x00000000

			MOVQ		MM5, [EDI+8]		// 2
			PXOR		MM2, MM0			// 1				// (Src ^ Dst)

			PXOR		MM5, MM3			// 2				// (Src ^ Dst)
			PAND		MM2, MM1			// 1				// & mask

			PAND		MM5, MM4			// 2				// &mask
			PXOR		MM0, MM2			// 1				// Src ^ ()

			PXOR		MM3, MM5			// 2				// Src ^ ()
			ADD			ESI, 16

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//	MOVQ		[EDI], MM0			// UnPair
			//	MOVQ		[EDI+8], MM3		// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movq	mm1,[edi]
			pand	mm0,mask1	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mask2	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movq	[edi],mm0
			movq	mm1,[edi+8]
			pand	mm3,mask1	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mask2	//	0xff000000ff000000	//	���l�����c��
			por		mm3,mm1
			movq	[edi+8],mm3

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_1				// UnPair

		EndLoop_1:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_1				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:��   �g�k:�L	 --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD	lPitchSrc = p->m_lPitch;
		DWORD*	lpSrcBack;
	
			//	mask data setting by yane
			static DWORD mask1[2] = {0x00ffffff, 0x00ffffff};
			static DWORD mask2[2] = {0xff000000, 0xff000000};

		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			PXOR		MM1, MM1

			MOVQ		MM2, MM7
			PXOR		MM0, MM0

			PUNPCKLDQ	MM7, MM2								// ColKey
			MOV			ECX, nWidth

			MOV			EDX, nHeight
			MOV			EBX, EIY								// nEyCnt = EIY;

		LoopY_2:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX								// nExCnt = EIX;

		LoopX_2: // 10�N���b�N��T�C�N��
			MOVD		MM2, [ESI]			// UnPair			// *lpDst = *lpSrc;

			MOVD		MM4, [EDI]
			MOVQ		MM3, MM2

			PCMPEQD		MM3, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM4, MM2								// (Src ^ Dst)

			PAND		MM4, MM3								// & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM2, MM4								// Src ^ ()
			ADD			EDI, 4									// lpDst++;

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_2									// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_2:
			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOVD		[EDI-4], MM2		// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movd	mm1,[edi-4]
			pand	mm0,mask1	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mask2	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movd	[edi-4],mm0

			DEC			ECX
			JNZ			LoopX_2

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_2									// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l

		SkipY_2:
			DEC			EDX
			JNZ			LoopY_2

			EMMS
		}
	} // if


	return 0;
} // BltWithoutAlpha


//////////////////////////////////////
//	BltFastWithoutAlpha
//	�����F�����̓]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BltFastWithoutAlpha(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BltFastWithoutAlpha��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32PMMX::BltFastWithoutAlpha��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( Clipping( lpDIB32, x, y, lpSrcRect, lpDstSize,	 lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nAddSrcWidth = p->m_lPitch - ((m_rcSrcRect.right - m_rcSrcRect.left)<<2);	// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);										// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc +(m_rcSrcRect.left<<2)+m_rcSrcRect.top*p->m_lPitch );		// �N���b�s���O�����̃J�b�g
	DWORD*	lpDst = (DWORD*)((BYTE*)   m_lpdwSrc +(m_rcDstRect.left<<2)+m_rcDstRect.top*   m_lPitch );		// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
			//	mask data setting by yane
			static DWORD mask1[2] = {0x00ffffff, 0x00ffffff};
			static DWORD mask2[2] = {0xff000000, 0xff000000};

		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			//	mask data setting by yane
			movq		mm3,mask1
			movq		mm4,mask2

		LoopY_3:
			SHR			ECX, 1
			JNB			Skip_3									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI]			// UnPair

			ADD			ESI, 4				// AGI���


			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOVD		[EDI], MM0			// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movd	mm1,[edi]
			pand	mm0,mm3	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mm4	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movd	[edi],mm0

			
			ADD			EDI, 4
			OR			ECX, ECX

			JZ			EndLoop_3			// UnPair

		Skip_3:
			SHR			ECX, 1
			JNB			LoopX_3									//	4�̔{�����ۂ��̔���
			// Step2
			MOVQ		MM0, [ESI]			// UnPair

			ADD			ESI, 8				// AGI���

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOVQ		[EDI], MM0			// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movq	mm1,[edi]
			pand	mm0,mm3	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mm4	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movq	[edi],mm0

			ADD			EDI, 8
			OR			ECX, ECX

			JZ			EndLoop_3			// UnPair

		LoopX_3: // 6�N���b�N��T�C�N��
			// Step4
			MOVQ		MM0, [ESI]			// UnPair

			MOVQ		MM2, [ESI+8]		// UnPair

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//	MOVQ		[EDI], MM0			// UnPair
			//	MOVQ		[EDI+8], MM2		// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movq	mm1,[edi]
			pand	mm0,mm3	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mm4	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movq	[edi],mm0
			movq	mm1,[edi+8]
			pand	mm2,mm3	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mm4	//	0xff000000ff000000	//	���l�����c��
			por		mm2,mm1
			movq	[edi+8],mm2

			ADD			ESI, 16
			ADD			EDI, 16

			DEC			ECX
			JNZ			LoopX_3

		EndLoop_3:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nAddSrcWidth						// �N���b�v�����̈敪���΂�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_3				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:��   �g�k:�L	 --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = 4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			PXOR		MM1, MM1
			PXOR		MM0, MM0

			MOV			ECX, nWidth
			MOV			EDX, nHeight

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;

		LoopY_4:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_4: // 7�N���b�N��T�C�N��
			MOVD		MM0, [ESI]			// UnPair			// *lpDst = *lpSrc;

			ADD			EDI, 4									// lpDst++;
			ADD			ESI, AddSrcPixel							// �������̉��Z

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_4									// if ( EX >= 0)

			ADD			ESI, 4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_4:
			MOVD		[EDI-4], MM0		// UnPair

			DEC			ECX
			JNZ			LoopX_4

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY									// Y�̑���
			JNB			SkipY_4									// if ( EY >= 0 )

			ADD			ESI, lPitchSrc						// 1���C�������Z���āA���̍s��
			SUB			EBX, EY2								// Y�̕␳�l
		SkipY_4:
			DEC			EDX
			JNZ			LoopY_4

			EMMS
		}
	} // if

	
	return 0;
} // BltFastWithoutAlpha

////////////////////////////////////////////////////////////////////
//	�~���[�L���`�]��
////////////////////////////////////////////////////////////////////
//////////////////////////////////////
//	BltWithoutAlphaM
//	�����F�L��̓]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BltWithoutAlphaM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BltWithoutAlphaM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32PMMX::BltWithoutAlphaM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����


// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
		DWORD	colKey = p->m_dwColorKey;

				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����

			//	mask data setting by yane
			static DWORD mask1[2] = {0x00ffffff, 0x00ffffff};
			static DWORD mask2[2] = {0xff000000, 0xff000000};

		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			EDI, lpDst

			MOVQ		MM0, MM7
			MOV			ESI, lpSrc

			PUNPCKLDQ	MM7, MM0			// UnPair			// ColKey

		LoopY_1:
			SHR			ECX, 1
			JNB			Skip_1									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]
			MOVQ		MM1, MM0

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 4

			ADD			ESI, -4				// AGI���

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOVD		[EDI-4], MM0		// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movd	mm1,[edi-4]
			pand	mm0,mask1	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mask2	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movd	[edi-4],mm0

			OR			ECX, ECX
			JZ			EndLoop_1

		Skip_1:
			SHR			ECX, 1
			JNB			LoopX_1									//	4�̔{�����ۂ��̔���
			// Step2
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM1, [ESI-8]		// UnPair

			MOVQ		MM2, [EDI]
			PUNPCKLDQ	MM0, MM1								// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM1, MM0			// UnPair

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1			// UnPair			// & mask

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 8

			ADD			ESI, -8				// AGI���

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOVQ		[EDI-8], MM0			// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movq	mm1,[edi-8]
			pand	mm0,mask1	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mask2	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movq	[edi-8],mm0

			OR			ECX, ECX
			JZ			EndLoop_1

		LoopX_1: // 15�N���b�N��T�C�N��
			// Step4
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM1, [ESI-8]		// 1// UnPair

			MOVD		MM3, [ESI-12]		// 2
			PUNPCKLDQ	MM0, MM1			// 1				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVD		MM4, [ESI-16]		// 2
			MOVQ		MM1, MM0			// 1

			MOVQ		MM2, [EDI]			// 1
			PUNPCKLDQ	MM3, MM4			// 2				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVQ		MM5, [EDI+8]		// 2
			MOVQ		MM4, MM3			// 2

			PCMPEQD		MM1, MM7			// 1				// ColKey:0xffffffff NonColKey:0x00000000
			PCMPEQD		MM4, MM7			// 2				// ColKey:0xffffffff NonColKey:0x00000000

			PXOR		MM2, MM0			// 1				// (src ^ dst)
			PXOR		MM5, MM3			// 2				// (src ^ dst)

			PAND		MM2, MM1			// 1				// & mask
			PAND		MM5, MM4			// 2				// & mask

			PXOR		MM0, MM2			// 1				// src ^ ()
			PXOR		MM3, MM5			// 2				// src ^ ()

			ADD			ESI, -16			// AGI���

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//	MOVQ		[EDI], MM0			// UnPair
			//	MOVQ		[EDI+8], MM3		// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movq	mm1,[edi]
			pand	mm0,mask1	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mask2	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movq	[edi],mm0
			movq	mm1,[edi+8]
			pand	mm3,mask1	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mask2	//	0xff000000ff000000	//	���l�����c��
			por		mm3,mm1
			movq	[edi+8],mm3

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_1				// UnPair

		EndLoop_1:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_1				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:�L   �~���[:�L   �g�k:�L	 --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD	colKey = p->m_dwColorKey;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

			//	mask data setting by yane
			static DWORD mask1[2] = {0x00ffffff, 0x00ffffff};
			static DWORD mask2[2] = {0xff000000, 0xff000000};

		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			MOVD		MM7, colKey								//	�J���[�L�[��ݒ肷��
			MOV			ECX, nWidth

			MOVQ		MM0, MM7
			MOV			EDX, nHeight

			PUNPCKLDQ	MM7, MM0								// ColKey
			MOV			EBX, EIY								// nEyCnt = EIY;
			//	mask data setting by yane
			movq		mm3,mask1
			movq		mm4,mask2

		LoopY_2:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_2: // 10�N���b�N��T�C�N��
			MOVD		MM0, [ESI-4]		// UnPair

			MOVD		MM2, [EDI]
			MOVQ		MM1, MM0

			PCMPEQD		MM1, MM7								// ColKey:0xffffffff NonColKey:0x00000000
			PXOR		MM2, MM0								// (src ^ dst)

			PAND		MM2, MM1								// & mask
			ADD			ESI, AddSrcPixel						// �������̉��Z

			PXOR		MM0, MM2								// src ^ ()
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_2									// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_2:
			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOVD		[EDI-4], MM0		// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movd	mm1,[edi-4]
			pand	mm0,mm3		//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mm4		//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movd	[edi-4],mm0

			DEC			ECX
			JNZ			LoopX_2

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_2										// if ( EY >= 0 )

			ADD			ESI, lPitchSrc
			SUB			EBX, EY2									// Y�̕␳

		SkipY_2:
			DEC			EDX
			JNZ			LoopY_2

			EMMS
		}
	} // if


	return 0;
} // BltWithoutAlphaM


//////////////////////////////////////
//	BltFastWithoutAlphaM
//	�����F�����̓]��
//////////////////////////////////////
LRESULT CDIB32PMMX::BltFastWithoutAlphaM(CDIB32* lpDIB32,int x,int y,LPRECT lpSrcRect,LPSIZE lpDstSize,LPRECT lpClipRect)
{
	WARNING(m_lpdwSrc == NULL,"CDIB32PMMX::BltFastWithoutAlphaM��m_lpdwSrc == NULL");

	CDIB32Base* p = lpDIB32->GetDIB32BasePtr();
	WARNING(p->GetPtr() == NULL,"CDIB32PMMX::BltFastWithoutAlphaM��p->GetPtr() == NULL");


// �N���b�s���O���̏���
	// �]���͈͂Ȃ��̎��́A���̂܂܋A��
	if ( ClippingM( lpDIB32, x, y, lpSrcRect, lpDstSize,  lpClipRect ) != 0 )
		return 0;

	// �]����̉����Əc���̐ݒ�
	int		nWidth = m_rcDstRect.right - m_rcDstRect.left;
	int		nHeight = m_rcDstRect.bottom - m_rcDstRect.top;

	DWORD	nSrcWidth = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;	// �]�����鉡���̌v�Z
	DWORD	nAddSrcWidth = p->m_lPitch - nSrcWidth;					// �N���b�s���O���Ĕ�΂����̎Z�o
	DWORD	nAddDstWidth = m_lPitch - (nWidth<<2);					// ASM�Ŏg�p���� 1���C���̃o�C�g���̐ݒ�

	DWORD*	lpSrc = (DWORD*)((BYTE*)p->m_lpdwSrc + (m_rcSrcRect.right<<2) + m_rcSrcRect.top*p->m_lPitch);
	DWORD*	lpDst = (DWORD*)((BYTE*)m_lpdwSrc + (m_rcDstRect.left<<2) + m_rcDstRect.top*m_lPitch );	// �w�肳�ꂽx,y�̈ʒu����

// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:��	 --------------
	if ( m_bActualSize == true )
	{
				nSrcWidth = nSrcWidth + p->m_lPitch;				// �����C���ւ̈ړ����� = �]�����鉡�� + Src�̑S����

			//	mask data setting by yane
			static DWORD mask1[2] = {0x00ffffff, 0x00ffffff};
			static DWORD mask2[2] = {0xff000000, 0xff000000};

		_asm
		{
			MOV			EDX, nHeight
			MOV			ECX, nWidth

			MOV			EDI, lpDst
			MOV			ESI, lpSrc

			//	mask data setting by yane
			movq		mm5,mask1
			movq		mm6,mask2

		LoopY_3:
			SHR			ECX, 1
			JNB			Skip_3									//	�������ۂ��̔���
			// Step	1
			MOVD		MM0, [ESI-4]		// UnPair

			ADD			ESI, -4				// AGI���

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOVD		[EDI], MM0			// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movd	mm1,[edi]
			pand	mm0,mm5	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mm6	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movd	[edi],mm0

			ADD			EDI, 4
			OR			ECX, ECX

			JZ			EndLoop_3			// UnPair

		Skip_3:
			SHR			ECX, 1
			JNB			LoopX_3									//	4�̔{�����ۂ��̔���
			// Step2
			MOVD		MM0, [ESI-4]		// UnPair
			MOVD		MM1, [ESI-8]		// UnPair
			PUNPCKLDQ	MM0, MM1								// �\�[�X�� 2�s�N�Z�����~���[����
			ADD			EDI, 8

			ADD			ESI, -8				// AGI���

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOVQ		[EDI], MM0			// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movq	mm1,[edi]
			pand	mm0,mm5	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mm6	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movq	[edi],mm0

			OR			ECX, ECX
			JZ			EndLoop_3

		LoopX_3: // 9�N���b�N��T�C�N��							// ����������ƃA�����[�����������ǁE�E�E�܁A������:p
			// Step4
			MOVD		MM0, [ESI-4]		// 1// UnPair

			MOVD		MM1, [ESI-8]		// 1// UnPair

			MOVD		MM3, [ESI-12]		// 2
			PUNPCKLDQ	MM0, MM1			// 1				// �\�[�X�� 2�s�N�Z�����~���[����

			MOVD		MM4, [ESI-16]		// 2// UnPair

			PUNPCKLDQ	MM3, MM4			// 2				// �\�[�X�� 2�s�N�Z�����~���[����
			ADD			ESI, -16

			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOVQ		[EDI], MM0			// 1// UnPair
			//		MOVQ		[EDI+8], MM3		// 2// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movq	mm1,[edi]
			pand	mm0,mm5	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mm6	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movq	[edi],mm0
			movq	mm1,[edi+8]
			pand	mm3,mm5	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mm6	//	0xff000000ff000000	//	���l�����c��
			por		mm3,mm1
			movq	[edi+8],mm3

			ADD			EDI, 16
			DEC			ECX

			JNZ			LoopX_3				// UnPair

		EndLoop_3:
			ADD			EDI, nAddDstWidth						// Dst�̐擪�����̃��C���Ɉړ�������
			ADD			ESI, nSrcWidth							// �����C���ւ̈ړ�

			MOV			ECX, nWidth								//	ECX�̍X�V
			DEC			EDX

			JNZ			LoopY_3				// UnPair

			EMMS
		}
	} // if
// -----------	 MMX   �J���[�L�[:��   �~���[:�L   �g�k:�L	 --------------
	else if ( 1 )
	{
		DWORD	AddSrcPixel = -4 * m_nStepsX;
//		DWORD	AddWidthSrc = ((p->m_rcRect.right - p->m_rcRect.left)<<2) * m_nStepsY;
		DWORD	AddWidthSrc = p->m_lPitch * m_nStepsY;
//		DWORD	AddWidthSrc2 = (m_rcSrcRect.right - m_rcSrcRect.left)<<2;
		int		EIX= m_nInitialX;
		int		EIY= m_nInitialY;
		int		EX = m_nStepX;
		int		EY = m_nStepY;
		int		EX2= m_nCmpX;
		int		EY2= m_nCmpY;
		DWORD*	lpSrcBack;
		DWORD	lPitchSrc = p->m_lPitch;

			//	mask data setting by yane
			static DWORD mask1[2] = {0x00ffffff, 0x00ffffff};
			static DWORD mask2[2] = {0xff000000, 0xff000000};

		_asm
		{
			MOV			ESI, lpSrc
			MOV			EDI, lpDst

			MOV			ECX, nWidth
			MOV			EDX, nHeight

			MOV			EBX, EIY			// UnPair			// nEyCnt = EIY;
			//	mask data setting by yane
			movq		mm5,mask1
			movq		mm6,mask2

		LoopY_4:
			MOV			lpSrcBack, ESI
			MOV			EAX, EIX			// UnPair			// nExCnt = EIX;

		LoopX_4: // 7�N���b�N��T�C�N��
			MOVD		MM0, [ESI-4]		// UnPair

			ADD			ESI, AddSrcPixel						// �������̉��Z
			ADD			EDI, 4

			ADD			EAX, EX									// EX += 2*DX;
			JNB			SkipX_4									// if ( EX >= 0)

			ADD			ESI, -4									// lpSrc++;
			SUB			EAX, EX2								// X�̕␳�l

		SkipX_4:
			//	���ꂪ�������݂��Ă���ꏊ�Ȃ̂ŕύX����(by yane)
			//	Blt�̃R�[�h�͂��ꁫ
			//		MOVD		[EDI-4], MM0			// UnPair
			//	�����dst = (dst&0xff000000) | (src&0xffffff);�ɂ���
			movd	mm1,[edi-4]
			pand	mm0,mm5	//	0x00ffffff00ffffff	//	�ŏ�ʃo�C�g�̃}�X�N(�ꉞ��)
			pand	mm1,mm6	//	0xff000000ff000000	//	���l�����c��
			por		mm0,mm1
			movd	[edi-4],mm0

			DEC			ECX
			JNZ			LoopX_4

			MOV			ESI, lpSrcBack							// Src�̐擪�ɖ߂�
			ADD			EDI, nAddDstWidth							// Dst�̐擪�����̃��C���Ɉړ�������

			MOV			ECX, nWidth
			ADD			ESI, AddWidthSrc						// �������̉��Z '00.09.12.byTia

			ADD			EBX, EY										// Y�̑��������Z
			JNB			SkipY_4										// if ( EY >= 0 )

			SUB			EBX, EY2									// Y�̕␳
			ADD			ESI, lPitchSrc								// �N���b�v�����̈敪���΂�

		SkipY_4:
			DEC			EDX
			JNZ			LoopY_4

			EMMS
		}
	} // if


	return 0;
} // BltFastWithoutAlphaM

///////////////////////////////////////////////////////////////////////////////

//	���`�����l���t���G�t�F�N�g�n
LRESULT CDIB32P5::FlushAlpha(LPRECT lpRect){

	WARNING(m_lpdwSrc == NULL,"CDIB32P5::FlushAlpha��m_lpdwSrc == NULL");

	RECT r = GetClipRect(lpRect);
	LONG lPitch	 = GetRect()->right;
	DWORD* pSurface = GetPtr();

	for(int y=r.top;y<r.bottom;y++){
		DWORD *p = pSurface + y*lPitch + r.left;
		for(int x=r.left;x<r.right;x++){
			//	�������𔽓]�A����24�r�b�g�͖��ύX
			*(p++) = *p ^ 0xff000000;
		}
	}

	return 0;
}

#endif