#include "stdafx.h"

#ifdef USE_DIB32

#include "yaneTextDIB32.h"

CTextDIB32::CTextDIB32(void){
	m_nTextX = 0;
	m_nTextY = 0;
	m_bVertical = false;
	UseDIBSection(true);
}

void CTextDIB32::SetVertical(bool bVertical){
	m_bVertical = bVertical;
}

LRESULT	CTextDIB32::UpdateText(void){
	LRESULT lr;
	if (!IsVertical()) {	// �������̏ꍇ
		lr = UpdateTextHorizontal();
	} else {				// �c�����̏ꍇ
		const string font(m_Font.GetFont());
		if (!font.empty() && font[0]=='@') {
			// ������c�����t�H���g�����牽�����Ȃ�
		} else {
			// �ꎞ�I�ɏc�����t�H���g�ɕύX����
			m_Font.SetFont('@'+font);
		}
		// �`��
		lr = UpdateTextVertical();
		// �t�H���g�����ɖ߂�
		m_Font.SetFont(font);
	}
	return lr;
}

LRESULT	CTextDIB32::UpdateTextA(void){	//	�A���`�F���t���`��
	LRESULT lr;
	if (!IsVertical()) {	// �������̏ꍇ
		lr = UpdateTextHorizontalA();
	} else {				// �c�����̏ꍇ
		const string font(m_Font.GetFont());
		if (!font.empty() && font[0]=='@') {
			// ������c�����t�H���g�����牽�����Ȃ�
		} else {
			// �ꎞ�I�ɏc�����t�H���g�ɕύX����
			m_Font.SetFont('@'+font);
		}
		// �`��
		lr = UpdateTextVerticalA();
		// �t�H���g�����ɖ߂�
		m_Font.SetFont(font);
	}
	return lr;
}

//	����̃R�s�y�����O�O�G
LRESULT	CTextDIB32::UpdateTextAA(void){	//	�A���`�F���t���`��
	LRESULT lr;
	if (!IsVertical()) {	// �������̏ꍇ
		lr = UpdateTextHorizontalAA();
	} else {				// �c�����̏ꍇ
		const string font(m_Font.GetFont());
		if (!font.empty() && font[0]=='@') {
			// ������c�����t�H���g�����牽�����Ȃ�
		} else {
			// �ꎞ�I�ɏc�����t�H���g�ɕύX����
			m_Font.SetFont('@'+font);
		}
		// �`��
		lr = UpdateTextVerticalAA();
		// �t�H���g�����ɖ߂�
		m_Font.SetFont(font);
	}
	return lr;
}


//////////////////////////////////////////////////////////
// ������

inline LRESULT CTextDIB32::UpdateTextHorizontal(void){
	int sx,sy;

	if (!IsLoad()) {	//	�r�b�g�}�b�v�ǂݍ���łȂ��Ȃ�΁D�D�D
		m_Font.GetSize(sx,sy);
		// m_Font�̃e�L�X�g����ł��v���[�������D
		if ((sx == 0) || (sy == 0)) {
			sx = sy = 1;
		}
		if (CreateSurface(sx,sy)) return 1;
	}
	m_Font.OnDraw(GetDC(),m_nTextX,m_nTextY);

	*GetYGA() = false;	//	for BltNatural
	return 0;
}

inline LRESULT CTextDIB32::UpdateTextHorizontalA(void){	//	�A���`�F���t���`��
	int sx,sy;
	m_Font.GetSize(sx,sy);
	// m_Font�̃e�L�X�g����ł��v���[�������D
	if ((sx == 0) || (sy == 0)) {
		sx = sy = 1;
	}
	if (!IsLoad()) {	//	�r�b�g�}�b�v�ǂݍ���łȂ��Ȃ�΁D�D�D
		if (CreateSurface(sx,sy)) return 1;
	} else {
		GetSize(sx,sy);	//	�v���[���̃T�C�Y���T�C�Y�Ƃ���O�O�G
	}

	COLORREF rgb = m_Font.GetColor();
	COLORREF bk	 = m_Font.GetBackColor();

	m_Font.SetBackColor(CLR_INVALID);
	m_Font.SetColor(RGB(255,255,255));	//	���ɂ���
	int	nQuality = m_Font.GetQuality();
	m_Font.SetQuality(4);	//	�A���`�F�������ŏ���
	int nFontSize = m_Font.GetSize();
	int nFontHeight = m_Font.GetHeight();
	m_Font.SetSize(nFontSize<<1);	//	2�{�T�C�Y�ŕ`��
	m_Font.SetHeight(nFontHeight<<1); //�@�s�Ԃ�2�{�T�C�Y�ŕ`��
	//	SetSize�́A�s�Ԃ��ύX���Ă��܂��̂ŁA
	//	SetSize����O�ɁA�s�Ԃ��擾���Ă����A������Q�{���Ȃ���΂Ȃ�Ȃ�

	CDIB32 d_dib;
	d_dib.UseDIBSection(true);
	int sx2,sy2;
	m_Font.GetSize(sx2,sy2);
	// m_Font�̃e�L�X�g����ł��v���[�������D
	if ((sx2 == 0) || (sy2 == 0)) {
		sx2 = sy2 = 1;
	}
	if (d_dib.CreateSurface(sx2,sy2)) return 2;
	m_Font.OnDraw(d_dib.GetDC(),m_nTextX<<1,m_nTextY<<1);	//	2�{�̃t�H���g�T�C�Y�ŕ`��

/*
	//	���{�̂Ƃ��̓]���͂���ŗǂ����D�D�D
	DWORD *lpdw = GetPtr();
	DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	�]�u����
	//	���邳�����`�����l���Ɏ����Ă���
	int nSize = GetRect()->right * GetRect()->bottom;
	for (int i=0;i<nSize;i++){
		*lpdw = ((*lpdw << 8)&0xff000000) + dwColor;	//	�F�́Argb�Ŏw�肳�ꂽ�F
		lpdw++;
	}
*/

	//	�Q�~�Q�����{�ɏk��
	{
	  sx2>>=1; sy2>>=1;
	  if (sx2<sx) { sx = sx2; }	//�@�Ȃ��H�������ꂽ�v���[���^�Q�̂ق�����������...
	  if (sy2<sy) { sy = sy2; }
	  DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	�]�u����
	  for(int y=0;y<sy;y++){
		DWORD *lpdw = GetPtr() + (GetRect()->right * y);
		DWORD *lpdw2a = d_dib.GetPtr() + (d_dib.GetRect()->right * (y*2	 ));
		DWORD *lpdw2b = lpdw2a		   +  d_dib.GetRect()->right; // �����C��
		//	����ꂽ���邳�����`�����l���Ɏ����Ă���
		for(int x=0;x<sx;x++){
			DWORD dwAlpha;
			dwAlpha =  ((*lpdw2a)&0xff) + ((*(lpdw2a+1))&0xff);
			dwAlpha += ((*lpdw2b)&0xff) + ((*(lpdw2b+1))&0xff);
			*lpdw = ((dwAlpha << (24-2)) & 0xff000000) + dwColor;	//	�F�́Argb�Ŏw�肳�ꂽ�F
			lpdw++; lpdw2a+=2; lpdw2b+=2;
		}
	  }
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�i�΁j
	m_Font.SetSize(nFontSize);
	m_Font.SetHeight(nFontHeight);
	//	�������Ŗ߂��Ȃ��ƁA�����̉e�𐶐�����Ƃ��ɉR�̃t�H���g�T�C�Y�ɂȂ����܂��O�O�G

	//	�����̉e
	if (bk!=CLR_INVALID) {
	//	�e��������񂩁D�D�D��炵���Ȃ��O�O�G
		CTextDIB32 dib;
		*dib.GetFont() = *GetFont();	//	�t�H���g�܂邲�ƃR�s�[���Ă܂��[�I�I�O�O�G
		dib.GetFont()->SetColor(bk);
		dib.GetFont()->SetBackColor(CLR_INVALID);	//	�����CLR_INVALID�ɂ��Ă����΁A���̍ċA�͂Q�x�ڂŒ�~����
//		dib.GetFont()->SetText(m_Font.GetText());
//		dib.SetTextPos(2,2);	//	���炵�ĕ`��I
		{
			int nOx, nOy;
			dib.GetFont()->GetShadowOffset(nOx,nOy);
			dib.SetTextPos(nOx,nOy); // ���炵�ĕ`��I
		}
		dib.UpdateTextA();
		//	PhotoShop�̃��C���I�s�����x�t������

		int alpha;
		int beta;
		int total;
		DWORD dwRGB;

		//	����dib�̃T�C�Y�́A���T�C�Y�Ɖ���ł��Ȃ������m��Ȃ��D�D
//		int nSize = GetRect()->right*GetRect()->bottom;
//		for (int i=0;i<nSize;i++){

		//	dib�ɂ��炽�ɕ`�悳������ƃT�C�Y���قȂ邩���m��Ȃ��D�D
		for(int y=0;y<sy;y++){
			DWORD *lpdw = GetPtr() + (GetRect()->right * y);
			DWORD *lpdw2 = dib.GetPtr() + (dib.GetRect()->right * y);
			//	����ꂽ���邳�����`�����l���Ɏ����Ă���
			for(int x=0;x<sx;x++){
				/*
				���̃A���S���Y���ɂ��ẮA��˂��炨�̃z�[���y�[�W��
				�X�[�p�[�v���O���}�[�ւ̓��@�u��CB��@PhotoShop�̃��C�������A���S���Y���ɂ��āiYGA�t�H�[�}�b�g��񏥂���) 00/10/18�v
				���Q�Ƃ̂���
				*/
				alpha = *lpdw >> 24;
				beta  = *lpdw2 >> 24;
				beta  = (255-alpha)*beta / 255;	//	�G�l���M�[�c�ʂ̌v�Z
				//	���z�䗦�����܂����̂ŁA����Ɋ�Â��ĕ��z
				total = alpha + beta;
				if (total != 0){	//	�[���̂Ƃ��́A���R�������Ȃ��ėǂ�
					if (beta == 0){		//�]���悪�O�Ȃ�A���̂܂ܓ]��
						*lpdw2 = *lpdw;
					}else{
						dwRGB = ((*lpdw & 0xff) * alpha + (*lpdw2 & 0xff) * beta) / total;
						dwRGB += (((*lpdw & 0xff00) * alpha + (*lpdw2 & 0xff00) * beta) / total) & 0xff00;
						dwRGB += (((*lpdw & 0xff0000) * alpha + (*lpdw2 & 0xff0000) * beta) / total) & 0xff0000;
						dwRGB += total << 24;	//	alpha�l�́A�G�l���M�[����
						*lpdw = dwRGB;
					}
				}
				lpdw++; lpdw2++;
			}
		}
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�Q�i�΁j
	m_Font.SetQuality(nQuality);
	m_Font.SetColor(rgb);
	m_Font.SetBackColor(bk);

	*GetYGA() = true;	//	for BltNatural

	return 0;
}

//	����̃R�s�y�����O�O�G
inline LRESULT CTextDIB32::UpdateTextHorizontalAA(void){	//	�A���`�F���t���`��
	int sx,sy;
	m_Font.GetSize(sx,sy);
	// m_Font�̃e�L�X�g����ł��v���[�������D
	if ((sx == 0) || (sy == 0)) {
		sx = sy = 1;
	}
	if (!IsLoad()) {	//	�r�b�g�}�b�v�ǂݍ���łȂ��Ȃ�΁D�D�D
		if (CreateSurface(sx,sy)) return 1;
	} else {
		GetSize(sx,sy);	//	�v���[���̃T�C�Y���T�C�Y�Ƃ���O�O�G
	}

	COLORREF rgb = m_Font.GetColor();
	COLORREF bk	 = m_Font.GetBackColor();

	m_Font.SetBackColor(CLR_INVALID);
	m_Font.SetColor(RGB(255,255,255));	//	���ɂ���
	int	nQuality = m_Font.GetQuality();
	m_Font.SetQuality(4);	//	�A���`�F�������ŏ���
	int nFontSize = m_Font.GetSize();
	int nFontHeight = m_Font.GetHeight();
	m_Font.SetSize(nFontSize<<2);	//	4�{�T�C�Y�ŕ`��
	m_Font.SetHeight(nFontHeight<<2); //�@�s�Ԃ�4�{�T�C�Y�ŕ`��

	CDIB32 d_dib;
	d_dib.UseDIBSection(true);
	int sx2,sy2;
	m_Font.GetSize(sx2,sy2);
	// m_Font�̃e�L�X�g����ł��v���[�������D
	if ((sx2 == 0) || (sy2 == 0)) {
		sx2 = sy2 = 1;
	}
	if (d_dib.CreateSurface(sx2,sy2)) return 2;
	m_Font.OnDraw(d_dib.GetDC(),m_nTextX<<2,m_nTextY<<2);	//	4�{�̃t�H���g�T�C�Y�ŕ`��

	//	�S�~�S�����{�ɏk��
	{
	  sx2>>=2; sy2>>=2;
	  if (sx2<sx) { sx = sx2; }	//�@�Ȃ��H�������ꂽ�v���[���^�Q�̂ق�����������...
	  if (sy2<sy) { sy = sy2; }
	  DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	�]�u����
	  for(int y=0;y<sy;y++){
		DWORD *lpdw = GetPtr() + (GetRect()->right * y);
		DWORD *lpdw2a = d_dib.GetPtr() + (d_dib.GetRect()->right * (y*4	 ));
		DWORD *lpdw2b = lpdw2a		   +  d_dib.GetRect()->right; // �����C��
		DWORD *lpdw2c = lpdw2b		   +  d_dib.GetRect()->right; // �����C��
		DWORD *lpdw2d = lpdw2c		   +  d_dib.GetRect()->right; // �����C��
		//	����ꂽ���邳�����`�����l���Ɏ����Ă���
		for(int x=0;x<sx;x++){
			DWORD dwAlpha;
			dwAlpha =	((*lpdw2a)&0xff) + ((*(lpdw2a+1))&0xff) + ((*(lpdw2a+2))&0xff) + ((*(lpdw2a+3))&0xff);
			dwAlpha +=	((*lpdw2b)&0xff) + ((*(lpdw2b+1))&0xff) + ((*(lpdw2b+2))&0xff) + ((*(lpdw2b+3))&0xff);
			dwAlpha +=	((*lpdw2c)&0xff) + ((*(lpdw2c+1))&0xff) + ((*(lpdw2c+2))&0xff) + ((*(lpdw2c+3))&0xff);
			dwAlpha +=	((*lpdw2d)&0xff) + ((*(lpdw2d+1))&0xff) + ((*(lpdw2d+2))&0xff) + ((*(lpdw2d+3))&0xff);
			*lpdw = ((dwAlpha << (24-4)) & 0xff000000) + dwColor;	//	�F�́Argb�Ŏw�肳�ꂽ�F
			lpdw++; lpdw2a+=4; lpdw2b+=4; lpdw2c+=4; lpdw2d+=4;
		}
	  }
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�i�΁j
	m_Font.SetSize(nFontSize);
	m_Font.SetHeight(nFontHeight);
	//	�������Ŗ߂��Ȃ��ƁA�����̉e�𐶐�����Ƃ��ɉR�̃t�H���g�T�C�Y�ɂȂ����܂��O�O�G

	//	�����̉e
	if (bk!=CLR_INVALID) {
	//	�e��������񂩁D�D�D��炵���Ȃ��O�O�G
		CTextDIB32 dib;
		*dib.GetFont() = *GetFont();	//	�t�H���g�܂邲�ƃR�s�[���Ă܂��[�I�I�O�O�G
		dib.GetFont()->SetColor(bk);
		dib.GetFont()->SetBackColor(CLR_INVALID);	//	�����CLR_INVALID�ɂ��Ă����΁A���̍ċA�͂Q�x�ڂŒ�~����
//		dib.GetFont()->SetText(m_Font.GetText());
//		dib.SetTextPos(2,2);	//	���炵�ĕ`��I
		{
			int nOx, nOy;
			dib.GetFont()->GetShadowOffset(nOx,nOy);
			dib.SetTextPos(nOx,nOy); // ���炵�ĕ`��I
		}
		dib.UpdateTextAA();

		//	PhotoShop�̃��C���I�s�����x�t������
		int alpha;
		int beta;
		int total;
		DWORD dwRGB;

		//	dib�ɂ��炽�ɕ`�悳������ƃT�C�Y���قȂ邩���m��Ȃ��D�D
		for(int y=0;y<sy;y++){
			DWORD *lpdw = GetPtr() + (GetRect()->right * y);
			DWORD *lpdw2 = dib.GetPtr() + (dib.GetRect()->right * y);
			//	����ꂽ���邳�����`�����l���Ɏ����Ă���
			for(int x=0;x<sx;x++){
				/*
				���̃A���S���Y���ɂ��ẮA��˂��炨�̃z�[���y�[�W��
				�X�[�p�[�v���O���}�[�ւ̓��@�u��CB��@PhotoShop�̃��C�������A���S���Y���ɂ��āiYGA�t�H�[�}�b�g��񏥂���) 00/10/18�v
				���Q�Ƃ̂���
				*/
				alpha = *lpdw >> 24;
				beta  = *lpdw2 >> 24;
				beta  = (255-alpha)*beta / 255;	//	�G�l���M�[�c�ʂ̌v�Z
				//	���z�䗦�����܂����̂ŁA����Ɋ�Â��ĕ��z
				total = alpha + beta;
				if (total != 0){	//	�[���̂Ƃ��́A���R�������Ȃ��ėǂ�
					if (beta == 0){		//�]���悪�O�Ȃ�A���̂܂ܓ]��
						*lpdw2 = *lpdw;
					}else{
						dwRGB = ((*lpdw & 0xff) * alpha + (*lpdw2 & 0xff) * beta) / total;
						dwRGB += (((*lpdw & 0xff00) * alpha + (*lpdw2 & 0xff00) * beta) / total) & 0xff00;
						dwRGB += (((*lpdw & 0xff0000) * alpha + (*lpdw2 & 0xff0000) * beta) / total) & 0xff0000;
						dwRGB += total << 24;	//	alpha�l�́A�G�l���M�[����
						*lpdw = dwRGB;
					}
				}
				lpdw++; lpdw2++;
			}
		}
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�Q�i�΁j
	m_Font.SetQuality(nQuality);
	m_Font.SetColor(rgb);
	m_Font.SetBackColor(bk);

	*GetYGA() = true;	//	for BltNatural

	return 0;
}


//////////////////////////////////////////////////////////
// �c����

inline LRESULT CTextDIB32::UpdateTextVertical(void){
	int sx,sy;

	if (!IsLoad()) {	//	�r�b�g�}�b�v�ǂݍ���łȂ��Ȃ�΁D�D�D
		m_Font.GetSize(sx,sy);
		// m_Font�̃e�L�X�g����ł��v���[�������D
		if ((sx == 0) || (sy == 0)) {
			sx = sy = 1;
		}
//----------------------------�ύX
	//	if (CreateSurface(sx,sy)) return 1;
		if (CreateSurface(sy,sx)) return 1;	// �t�ɂȂ�
	} else {
		GetSize(sx,sy);	//	�v���[���̃T�C�Y���T�C�Y�Ƃ���O�O�G
	}

	// �e�̌������c�����p�ɕύX
	int nOx, nOy;
	m_Font.GetShadowOffset(nOx, nOy);
	m_Font.SetShadowOffset(nOy, -nOx);

	// �c�����ϊ��p�ɂ�����T�[�t�F�X���쐬
	CDIB32 d_dib2;
	d_dib2.UseDIBSection(true);
	if (d_dib2.CreateSurface(sx, sy)) return 2;

 //	m_Font.OnDraw(GetDC(),m_nTextX,m_nTextY);
	m_Font.OnDraw(d_dib2.GetDC(),m_nTextX,m_nTextY);

	// �����c�ϊ�
	{
		int width, height;
		GetSize(width, height);
		for (int y=0;y<sy;y++) {
			DWORD *lpdw = GetPtr() + (width - y -1);	// �������̏と���͏c�����̉E����
			DWORD *lpdw2 = d_dib2.GetPtr() + (d_dib2.GetRect()->right * y);
			for (int x=0;x<sx;x++) {
				*lpdw = *lpdw2;
				lpdw+=width;	// �P����
				lpdw2++;
			}
		}
	}

	// �e�̌��������ɖ߂�
	m_Font.SetShadowOffset(nOx, nOy);
//----------------------------


	*GetYGA() = false;	//	for BltNatural
	return 0;
}

inline LRESULT CTextDIB32::UpdateTextVerticalA(void){	//	�A���`�F���t���`��
	int sx,sy;
	m_Font.GetSize(sx,sy);
	// m_Font�̃e�L�X�g����ł��v���[�������D
	if ((sx == 0) || (sy == 0)) {
		sx = sy = 1;
	}
	if (!IsLoad()) {	//	�r�b�g�}�b�v�ǂݍ���łȂ��Ȃ�΁D�D�D
//----------------------------�ύX
	//	if (CreateSurface(sx,sy)) return 1;
		if (CreateSurface(sy,sx)) return 1;	// �t�ɂȂ�
//----------------------------
	} else {
		GetSize(sx,sy);	//	�v���[���̃T�C�Y���T�C�Y�Ƃ���O�O�G
	}

	COLORREF rgb = m_Font.GetColor();
	COLORREF bk	 = m_Font.GetBackColor();

	m_Font.SetBackColor(CLR_INVALID);
	m_Font.SetColor(RGB(255,255,255));	//	���ɂ���
	int	nQuality = m_Font.GetQuality();
	m_Font.SetQuality(4);	//	�A���`�F�������ŏ���
	int nFontSize = m_Font.GetSize();
	int nFontHeight = m_Font.GetHeight();
	m_Font.SetSize(nFontSize<<1);	//	2�{�T�C�Y�ŕ`��
	m_Font.SetHeight(nFontHeight<<1); //�@�s�Ԃ�2�{�T�C�Y�ŕ`��
	//	SetSize�́A�s�Ԃ��ύX���Ă��܂��̂ŁA
	//	SetSize����O�ɁA�s�Ԃ��擾���Ă����A������Q�{���Ȃ���΂Ȃ�Ȃ�

	CDIB32 d_dib;
	d_dib.UseDIBSection(true);
	int sx2,sy2;
	m_Font.GetSize(sx2,sy2);
	// m_Font�̃e�L�X�g����ł��v���[�������D
	if ((sx2 == 0) || (sy2 == 0)) {
		sx2 = sy2 = 1;
	}
	if (d_dib.CreateSurface(sx2,sy2)) return 2;
	m_Font.OnDraw(d_dib.GetDC(),m_nTextX<<1,m_nTextY<<1);	//	2�{�̃t�H���g�T�C�Y�ŕ`��


//----------------------------�ǉ�
	// �e�̌������c�����p�ɕύX
	int nOx, nOy;
	m_Font.GetShadowOffset(nOx, nOy);
	m_Font.SetShadowOffset(nOy, -nOx);

	// �c�����ϊ��p�ɂ�����T�[�t�F�X���쐬
	CDIB32 d_dib2;
	d_dib2.UseDIBSection(true);
	if (d_dib2.CreateSurface(sx, sy)) return 2;
//----------------------------


/*
	//	���{�̂Ƃ��̓]���͂���ŗǂ����D�D�D
	DWORD *lpdw = GetPtr();
	DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	�]�u����
	//	���邳�����`�����l���Ɏ����Ă���
	int nSize = GetRect()->right * GetRect()->bottom;
	for (int i=0;i<nSize;i++){
		*lpdw = ((*lpdw << 8)&0xff000000) + dwColor;	//	�F�́Argb�Ŏw�肳�ꂽ�F
		lpdw++;
	}
*/

	//	�Q�~�Q�����{�ɏk��
	{
	  sx2>>=1; sy2>>=1;
	  if (sx2<sx) { sx = sx2; }	//�@�Ȃ��H�������ꂽ�v���[���^�Q�̂ق�����������...
	  if (sy2<sy) { sy = sy2; }
	  DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	�]�u����
	  for(int y=0;y<sy;y++){
//----------------------------�ύX
	//	DWORD *lpdw = GetPtr() + (GetRect()->right * y);
		DWORD *lpdw = d_dib2.GetPtr() + (d_dib2.GetRect()->right * y);	// �c�����ϊ��p�̂��̂��g��
//----------------------------
		DWORD *lpdw2a = d_dib.GetPtr() + (d_dib.GetRect()->right * (y*2	 ));
		DWORD *lpdw2b = lpdw2a		   +  d_dib.GetRect()->right; // �����C��
		//	����ꂽ���邳�����`�����l���Ɏ����Ă���
		for(int x=0;x<sx;x++){
			DWORD dwAlpha;
			dwAlpha =  ((*lpdw2a)&0xff) + ((*(lpdw2a+1))&0xff);
			dwAlpha += ((*lpdw2b)&0xff) + ((*(lpdw2b+1))&0xff);
			*lpdw = ((dwAlpha << (24-2)) & 0xff000000) + dwColor;	//	�F�́Argb�Ŏw�肳�ꂽ�F
			lpdw++; lpdw2a+=2; lpdw2b+=2;
		}
	  }
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�i�΁j
	m_Font.SetSize(nFontSize);
	m_Font.SetHeight(nFontHeight);
	//	�������Ŗ߂��Ȃ��ƁA�����̉e�𐶐�����Ƃ��ɉR�̃t�H���g�T�C�Y�ɂȂ����܂��O�O�G

	//	�����̉e
	if (bk!=CLR_INVALID) {
	//	�e��������񂩁D�D�D��炵���Ȃ��O�O�G
		CTextDIB32 dib;
		*dib.GetFont() = *GetFont();	//	�t�H���g�܂邲�ƃR�s�[���Ă܂��[�I�I�O�O�G
		dib.GetFont()->SetColor(bk);
		dib.GetFont()->SetBackColor(CLR_INVALID);	//	�����CLR_INVALID�ɂ��Ă����΁A���̍ċA�͂Q�x�ڂŒ�~����
//		dib.GetFont()->SetText(m_Font.GetText());
//		dib.SetTextPos(2,2);	//	���炵�ĕ`��I
		{
			int nOx, nOy;
			dib.GetFont()->GetShadowOffset(nOx,nOy);
			dib.SetTextPos(nOx,nOy); // ���炵�ĕ`��I
		}
		dib.UpdateTextA();
		//	PhotoShop�̃��C���I�s�����x�t������

		int alpha;
		int beta;
		int total;
		DWORD dwRGB;

		//	����dib�̃T�C�Y�́A���T�C�Y�Ɖ���ł��Ȃ������m��Ȃ��D�D
//		int nSize = GetRect()->right*GetRect()->bottom;
//		for (int i=0;i<nSize;i++){

		//	dib�ɂ��炽�ɕ`�悳������ƃT�C�Y���قȂ邩���m��Ȃ��D�D
		for(int y=0;y<sy;y++){
//----------------------------�ύX
	//		DWORD *lpdw = GetPtr() + (GetRect()->right * y);
			DWORD *lpdw = d_dib2.GetPtr() + (d_dib2.GetRect()->right * y);	// �c�����ϊ��p�̂��̂��g��
//----------------------------
			DWORD *lpdw2 = dib.GetPtr() + (dib.GetRect()->right * y);
			//	����ꂽ���邳�����`�����l���Ɏ����Ă���
			for(int x=0;x<sx;x++){
				/*
				���̃A���S���Y���ɂ��ẮA��˂��炨�̃z�[���y�[�W��
				�X�[�p�[�v���O���}�[�ւ̓��@�u��CB��@PhotoShop�̃��C�������A���S���Y���ɂ��āiYGA�t�H�[�}�b�g��񏥂���) 00/10/18�v
				���Q�Ƃ̂���
				*/
				alpha = *lpdw >> 24;
				beta  = *lpdw2 >> 24;
				beta  = (255-alpha)*beta / 255;	//	�G�l���M�[�c�ʂ̌v�Z
				//	���z�䗦�����܂����̂ŁA����Ɋ�Â��ĕ��z
				total = alpha + beta;
				if (total != 0){	//	�[���̂Ƃ��́A���R�������Ȃ��ėǂ�
					if (beta == 0){		//�]���悪�O�Ȃ�A���̂܂ܓ]��
						*lpdw2 = *lpdw;
					}else{
						dwRGB = ((*lpdw & 0xff) * alpha + (*lpdw2 & 0xff) * beta) / total;
						dwRGB += (((*lpdw & 0xff00) * alpha + (*lpdw2 & 0xff00) * beta) / total) & 0xff00;
						dwRGB += (((*lpdw & 0xff0000) * alpha + (*lpdw2 & 0xff0000) * beta) / total) & 0xff0000;
						dwRGB += total << 24;	//	alpha�l�́A�G�l���M�[����
						*lpdw = dwRGB;
					}
				}
				lpdw++; lpdw2++;
			}
		}
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�Q�i�΁j
	m_Font.SetQuality(nQuality);
	m_Font.SetColor(rgb);
	m_Font.SetBackColor(bk);


//----------------------------�ǉ�
	// �����c�ϊ�
	{
		int width, height;
		GetSize(width, height);
		for (int y=0;y<sy;y++) {
			DWORD *lpdw = GetPtr() + (width - y -1);	// �������̏と���͏c�����̉E����
			DWORD *lpdw2 = d_dib2.GetPtr() + (d_dib2.GetRect()->right * y);
			for (int x=0;x<sx;x++) {
				*lpdw = *lpdw2;
				lpdw+=width;	// �P����
				lpdw2++;
			}
		}
	}

	// �e�̌��������ɖ߂�
	m_Font.SetShadowOffset(nOx, nOy);
//----------------------------


	*GetYGA() = true;	//	for BltNatural

	return 0;
}

//	����̃R�s�y�����O�O�G
inline LRESULT CTextDIB32::UpdateTextVerticalAA(void){	//	�A���`�F���t���`��
	int sx,sy;
	m_Font.GetSize(sx,sy);
	// m_Font�̃e�L�X�g����ł��v���[�������D
	if ((sx == 0) || (sy == 0)) {
		sx = sy = 1;
	}
	if (!IsLoad()) {	//	�r�b�g�}�b�v�ǂݍ���łȂ��Ȃ�΁D�D�D
//----------------------------�ύX
	//	if (CreateSurface(sx,sy)) return 1;
		if (CreateSurface(sy,sx)) return 1;	// �t�ɂȂ�
//----------------------------
	} else {
		GetSize(sx,sy);	//	�v���[���̃T�C�Y���T�C�Y�Ƃ���O�O�G
	}

	COLORREF rgb = m_Font.GetColor();
	COLORREF bk	 = m_Font.GetBackColor();

	m_Font.SetBackColor(CLR_INVALID);
	m_Font.SetColor(RGB(255,255,255));	//	���ɂ���
	int	nQuality = m_Font.GetQuality();
	m_Font.SetQuality(4);	//	�A���`�F�������ŏ���
	int nFontSize = m_Font.GetSize();
	int nFontHeight = m_Font.GetHeight();
	m_Font.SetSize(nFontSize<<2);	//	4�{�T�C�Y�ŕ`��
	m_Font.SetHeight(nFontHeight<<2); //�@�s�Ԃ�4�{�T�C�Y�ŕ`��

	CDIB32 d_dib;
	d_dib.UseDIBSection(true);
	int sx2,sy2;
	m_Font.GetSize(sx2,sy2);
	// m_Font�̃e�L�X�g����ł��v���[�������D
	if ((sx2 == 0) || (sy2 == 0)) {
		sx2 = sy2 = 1;
	}
	if (d_dib.CreateSurface(sx2,sy2)) return 2;
	m_Font.OnDraw(d_dib.GetDC(),m_nTextX<<2,m_nTextY<<2);	//	4�{�̃t�H���g�T�C�Y�ŕ`��


//----------------------------�ǉ�
	// �e�̌������c�����p�ɕύX
	int nOx, nOy;
	m_Font.GetShadowOffset(nOx, nOy);
	m_Font.SetShadowOffset(nOy, -nOx);

	// �c�����ϊ��p�ɂ�����T�[�t�F�X���쐬
	CDIB32 d_dib2;
	d_dib2.UseDIBSection(true);
	if (d_dib2.CreateSurface(sx, sy)) return 2;
//----------------------------


	//	�S�~�S�����{�ɏk��
	{
	  sx2>>=2; sy2>>=2;
	  if (sx2<sx) { sx = sx2; }	//�@�Ȃ��H�������ꂽ�v���[���^�Q�̂ق�����������...
	  if (sy2<sy) { sy = sy2; }
	  DWORD dwColor = ((rgb & 0xff) << 16) + (rgb & 0xff00) + ((rgb & 0xff0000)>>16);	//	�]�u����
	  for(int y=0;y<sy;y++){
//----------------------------�ύX
	//	DWORD *lpdw = GetPtr() + (GetRect()->right * y);
		DWORD *lpdw = d_dib2.GetPtr() + (d_dib2.GetRect()->right * y);	// �c�����ϊ��p�̂��̂��g��
//----------------------------
		DWORD *lpdw2a = d_dib.GetPtr() + (d_dib.GetRect()->right * (y*4	 ));
		DWORD *lpdw2b = lpdw2a		   +  d_dib.GetRect()->right; // �����C��
		DWORD *lpdw2c = lpdw2b		   +  d_dib.GetRect()->right; // �����C��
		DWORD *lpdw2d = lpdw2c		   +  d_dib.GetRect()->right; // �����C��
		//	����ꂽ���邳�����`�����l���Ɏ����Ă���
		for(int x=0;x<sx;x++){
			DWORD dwAlpha;
			dwAlpha =	((*lpdw2a)&0xff) + ((*(lpdw2a+1))&0xff) + ((*(lpdw2a+2))&0xff) + ((*(lpdw2a+3))&0xff);
			dwAlpha +=	((*lpdw2b)&0xff) + ((*(lpdw2b+1))&0xff) + ((*(lpdw2b+2))&0xff) + ((*(lpdw2b+3))&0xff);
			dwAlpha +=	((*lpdw2c)&0xff) + ((*(lpdw2c+1))&0xff) + ((*(lpdw2c+2))&0xff) + ((*(lpdw2c+3))&0xff);
			dwAlpha +=	((*lpdw2d)&0xff) + ((*(lpdw2d+1))&0xff) + ((*(lpdw2d+2))&0xff) + ((*(lpdw2d+3))&0xff);
			*lpdw = ((dwAlpha << (24-4)) & 0xff000000) + dwColor;	//	�F�́Argb�Ŏw�肳�ꂽ�F
			lpdw++; lpdw2a+=4; lpdw2b+=4; lpdw2c+=4; lpdw2d+=4;
		}
	  }
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�i�΁j
	m_Font.SetSize(nFontSize);
	m_Font.SetHeight(nFontHeight);
	//	�������Ŗ߂��Ȃ��ƁA�����̉e�𐶐�����Ƃ��ɉR�̃t�H���g�T�C�Y�ɂȂ����܂��O�O�G

	//	�����̉e
	if (bk!=CLR_INVALID) {
	//	�e��������񂩁D�D�D��炵���Ȃ��O�O�G
		CTextDIB32 dib;
		*dib.GetFont() = *GetFont();	//	�t�H���g�܂邲�ƃR�s�[���Ă܂��[�I�I�O�O�G
		dib.GetFont()->SetColor(bk);
		dib.GetFont()->SetBackColor(CLR_INVALID);	//	�����CLR_INVALID�ɂ��Ă����΁A���̍ċA�͂Q�x�ڂŒ�~����
//		dib.GetFont()->SetText(m_Font.GetText());
//		dib.SetTextPos(2,2);	//	���炵�ĕ`��I
		{
			int nOx, nOy;
			dib.GetFont()->GetShadowOffset(nOx,nOy);
			dib.SetTextPos(nOx,nOy); // ���炵�ĕ`��I
		}
		dib.UpdateTextAA();

		//	PhotoShop�̃��C���I�s�����x�t������
		int alpha;
		int beta;
		int total;
		DWORD dwRGB;

		//	dib�ɂ��炽�ɕ`�悳������ƃT�C�Y���قȂ邩���m��Ȃ��D�D
		for(int y=0;y<sy;y++){
//----------------------------�ύX
		//	DWORD *lpdw = GetPtr() + (GetRect()->right * y);
			DWORD *lpdw = d_dib2.GetPtr() + (d_dib2.GetRect()->right * y);	// �c�����ϊ��p�̂��̂��g��
//----------------------------
			DWORD *lpdw2 = dib.GetPtr() + (dib.GetRect()->right * y);
			//	����ꂽ���邳�����`�����l���Ɏ����Ă���
			for(int x=0;x<sx;x++){
				/*
				���̃A���S���Y���ɂ��ẮA��˂��炨�̃z�[���y�[�W��
				�X�[�p�[�v���O���}�[�ւ̓��@�u��CB��@PhotoShop�̃��C�������A���S���Y���ɂ��āiYGA�t�H�[�}�b�g��񏥂���) 00/10/18�v
				���Q�Ƃ̂���
				*/
				alpha = *lpdw >> 24;
				beta  = *lpdw2 >> 24;
				beta  = (255-alpha)*beta / 255;	//	�G�l���M�[�c�ʂ̌v�Z
				//	���z�䗦�����܂����̂ŁA����Ɋ�Â��ĕ��z
				total = alpha + beta;
				if (total != 0){	//	�[���̂Ƃ��́A���R�������Ȃ��ėǂ�
					if (beta == 0){		//�]���悪�O�Ȃ�A���̂܂ܓ]��
						*lpdw2 = *lpdw;
					}else{
						dwRGB = ((*lpdw & 0xff) * alpha + (*lpdw2 & 0xff) * beta) / total;
						dwRGB += (((*lpdw & 0xff00) * alpha + (*lpdw2 & 0xff00) * beta) / total) & 0xff00;
						dwRGB += (((*lpdw & 0xff0000) * alpha + (*lpdw2 & 0xff0000) * beta) / total) & 0xff0000;
						dwRGB += total << 24;	//	alpha�l�́A�G�l���M�[����
						*lpdw = dwRGB;
					}
				}
				lpdw++; lpdw2++;
			}
		}
	}

	//	���A������O�O�G�@�Ȃ񂬂�ȁ[�Q�i�΁j
	m_Font.SetQuality(nQuality);
	m_Font.SetColor(rgb);
	m_Font.SetBackColor(bk);


//----------------------------�ǉ�
	// �����c�ϊ�
	{
		int width, height;
		GetSize(width, height);
		for (int y=0;y<sy;y++) {
			DWORD *lpdw = GetPtr() + (width - y -1);	// �������̏と���͏c�����̉E����
			DWORD *lpdw2 = d_dib2.GetPtr() + (d_dib2.GetRect()->right * y);
			for (int x=0;x<sx;x++) {
				*lpdw = *lpdw2;
				lpdw+=width;	// �P����
				lpdw2++;
			}
		}
	}

	// �e�̌��������ɖ߂�
	m_Font.SetShadowOffset(nOx, nOy);
//----------------------------


	*GetYGA() = true;	//	for BltNatural

	return 0;
}

#endif