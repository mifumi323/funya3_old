//	CPlaneBase
//		CPlane��CDIB32�Γ��Ɉ������߂̃M�~�b�N
//		��CPlane��CDIB32�̊֐����T�|�[�g����K�v������Ƃ��Ɏg��

#ifndef __yanePlaneBase_h__
#define __yanePlaneBase_h__

//	DIB32RGB�Ǝd�l�͓���
//	#define PlaneRGB(r,g,b) ( ((DWORD)r)<<16 | ((DWORD)g)<<8 | b )
//	�����ꂾ���Ar,g,b<=255���`�F�b�N�����ق������S�Ȃ̂ŁA���������R�[�h�ɂ���B
inline DWORD PlaneRGB(DWORD r,DWORD g,DWORD b){
	WARNING(r>=256 || g>=256 || b>=256,"PlaneRGB�Œl���I�[�o�[���Ă��܂�");
	return ( ((DWORD)r)<<16 | ((DWORD)g)<<8 | b );
}

//	CPlaneBase::CreateFactory�Ő������邽�߂�factory
class CPlaneBase;
class CPlaneBaseFactory {
public:
	virtual CPlaneBase* Create() = 0;
};

#ifdef USE_DirectDraw
class CPlaneBaseFactoryForCPlane : public CPlaneBaseFactory {
public:
	virtual CPlaneBase* Create();
};
#endif

#ifdef USE_FastDraw
class CPlaneBaseFactoryForCFastPlane : public CPlaneBaseFactory {
public:
	virtual CPlaneBase* Create();
};
#endif

#ifdef USE_DIB32
class CPlaneBaseFactoryForCDIB32 : public CPlaneBaseFactory {
public:
	virtual CPlaneBase* Create();
};
#endif

//	CPlaneBase::GetID��RTTI�̐^�������o����
enum EDrawType {
	eDraw_NullPlane,	//	��@�ȃv���[��
	eDraw_CPlane,		//	CPlane
	eDraw_CDIB32,		//	CDIB32
	eDraw_CFastPlane	//	CFastPlane
};

class CPlaneBase {
public:
	virtual ~CPlaneBase() {}	//	merely place holder

	//	CPlaneBase�̔h���N���X�ł́A������I�[�o�[���C�h���ׂ�
	virtual EDrawType GetID() const { return eDraw_NullPlane; }

	//	��`�`��
	virtual LRESULT Blt(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BltFast(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;

	//	�u�����h�]��(CDIB32,CPlane�ŃT�|�[�g�BCFastPlane�ł͔�T�|�[�g)
	virtual LRESULT BlendBlt(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;
	virtual LRESULT BlendBltFast(CPlaneBase* lpSrc,int x,int y,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) = 0;

	//	�u�����h�]���A�u�����h�䗦�Œ�n(CFastPlane�̂ݓ������ꂽ���[�`���L��)
	virtual LRESULT BlendBlt(CPlaneBase* lpSrc,int x,int y,BYTE byFadeRate
		,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return BlendBlt(lpSrc,x,y
			,PlaneRGB(255-byFadeRate,255-byFadeRate,255-byFadeRate)
			,PlaneRGB(byFadeRate,byFadeRate,byFadeRate)
			,lpSrcRect,lpDstSize,lpClipRect);
	}
	virtual LRESULT BlendBltFast(CPlaneBase* lpSrc,int x,int y,BYTE byFadeRate
		,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL){
		return BlendBltFast(lpSrc,x,y
			,PlaneRGB(255-byFadeRate,255-byFadeRate,255-byFadeRate)
			,PlaneRGB(byFadeRate,byFadeRate,byFadeRate)
			,lpSrcRect,lpDstSize,lpClipRect);
	}

	//	���t���摜�]���n(������CDIB32,CFastPlane�ł�����������Ă��炸)
	virtual LRESULT BlendBltFastAlpha(CPlaneBase* lpSrc,int x,int y
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) { return 0; }
		//	CDIB32�ł�����������Ă��炸
	virtual LRESULT BlendBltFastAlpha(CPlaneBase* lpSrc,int x,int y
				,DWORD dwDstRGBRate,DWORD dwSrcRGBRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) { return 0; }
		//	���Œ�nYGA�摜�]���BCFastPlane�ł̂݁A�������ꂽ���[�`���L��
	virtual LRESULT BlendBltFastAlpha(CPlaneBase* lpSrc,int x,int y,BYTE byFadeRate
				,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		return BlendBltFastAlpha(lpSrc,x,y
			,PlaneRGB(255-byFadeRate,255-byFadeRate,255-byFadeRate)
			,PlaneRGB(byFadeRate,byFadeRate,byFadeRate)
			,lpSrcRect,lpDstSize,lpClipRect);
	}
		//	CDIB32,CFastPlane�ł�����������Ă��炸
	virtual LRESULT FadeBltAlpha(CPlaneBase* lpSrc,int x,int y,int nFadeRate)=0;

	//	Mosaic�i���̃v���[���ɑ΂���G�t�F�N�g�j
	virtual LRESULT MosaicEffect(int d, LPRECT lpRect=NULL) = 0;

	//	Flush �i���̃v���[���ɑ΂���G�t�F�N�g�j
	virtual LRESULT FlushEffect(LPRECT lpRect=NULL) = 0;

	//	 �T�C�Y�擾
	virtual	void	GetSize(int &x,int &y) = 0;

	//	�ǂݍ��܂�Ă��邩�ǂ���
	virtual	bool	IsLoaded(void) const = 0;
	//	YGA�摜�Ȃ̂��ǂ���
	virtual	bool	IsYGA(void) { return false; }

	//	��ʃN���A
	virtual LRESULT	ClearRect(LPRECT lpRect=NULL) = 0;

	//	���R�ȓ]��(CPlane�Ȃ��Blt
	//		CDIB32�Ȃ��Blt,CDIB32��yga�摜�Ȃ��BlendBltFastAlpha,CFastDraw�����l)
	virtual LRESULT BltNatural(CPlaneBase* lpSrc,int x,int y,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		if (!lpSrc->IsYGA()) {
			return Blt(lpSrc,x,y,lpSrcRect,lpDstSize,lpClipRect);
		} else {
			return BlendBltFastAlpha(lpSrc,x,y,lpSrcRect,lpDstSize,lpClipRect);
		}
	}
	virtual LRESULT BltNatural(CPlaneBase* lpSrc,int x,int y,int nFadeRate,LPRECT lpSrcRect=NULL,LPSIZE lpDstSize=NULL,LPRECT lpClipRect=NULL) {
		if (nFadeRate>=255) {
			return BltNatural(lpSrc,x,y,lpSrcRect,lpDstSize,lpClipRect);
		}
		if (!lpSrc->IsYGA()) {
			return BlendBlt(lpSrc,x,y,nFadeRate,lpSrcRect,lpDstSize,lpClipRect);
		} else {
			return BlendBltFastAlpha(lpSrc,x,y,nFadeRate,lpSrcRect,lpDstSize,lpClipRect);
		}
	}
	virtual LRESULT BltNaturalPos(CPlaneBase* lpSrc,int x,int y,int nMode,int nFade=256);
	//	nMode == �x�[�X�ʒu(0:�摜���S 1:���� 2:�E�� 3:���� 4:�E��)

	//////////////////////////////////////////////////////////////////////////
	// �r�b�g�}�b�v�֘A
	virtual LRESULT Load(string szBitmapFileName,bool bLoadPalette=false) = 0;
	// bLoadPalette==false���ƁA���݂̃p���b�g�J���[�ɏ�����SetDIBitsToDevice��
	// �ǂݍ��܂��BWM_PALETTECHANGED�ɉ�������A�v���̏ꍇ�A����œǂݍ��ޕK�v����

	virtual LRESULT LoadW(string szBitmapFileName256,string szBitmapFileNameElse
			,bool bLoadPalette=true) = 0;
	// 256�F���[�h�Ȃ�΁A�ʂ̃t�@�C����ǂޏꍇ�́A������I�[�o�[���C�h

	virtual LRESULT	Release(void) = 0;
	virtual LRESULT SetColorKey(int x,int y) = 0;	// (x,y)�̓_�𓧉߃L�[�ɐݒ肷��
	virtual LRESULT SetColorKey(int r,int g,int b) = 0;

	//	�T�[�t�F�[�X�̐���(CFastPlane�́AbYGA���w�肵�Ȃ��Ƃ����Ȃ�)
	virtual LRESULT CreateSurface(int sx,int sy,bool bYGA=false) { return 1; } // not support

	//	(x,y)�̃��l��Ԃ�(DIB32,CFastPlane�̂�)
	//	�񃿃T�[�t�F�[�X�Ȃ�Δ����F�Ɠ����ł����0,�Ⴄ�̂Ȃ��255��Ԃ�
	virtual int	GetPixelAlpha(int x,int y){
		//	�f�B�t�H���g�ł́A�͈͊O�ł��邩�ǂ����Ŕ���
		int sx,sy;
		GetSize(sx,sy);
		if (x<0 || x>=sx || y<0 || y>=sy) return 0;
		return 255;
	}

	//	�T�[�t�F�[�X�̃o�b�N�A�b�v���쐬���ĕԂ�
	smart_ptr<CPlaneBase> GetBackup(void);

	static CPlaneBase* CreatePlane(void);
	//	�����֐��B����CDirectDraw���g���Ă���̂�CDIBDraw���g���Ă���̂��ɉ�����
	//	CPlane,CDIB32,CFastPlane�𐶐�����B

	//	CreatePlane�̂��߂�Factory��ݒ肷��
	static void SetPlaneBaseFactory(smart_ptr<CPlaneBaseFactory> pv) {
		m_pvPlaneFactory = pv;
	}

private:
	static smart_ptr<CPlaneBaseFactory> m_pvPlaneFactory;

};	//	�Ƃ肠�����A���ꂾ������Ώ\��������O�O�G

#endif