//	yaneGraphicLoader.h
//
//		BMP,JPEG,GIF,...��K�v�ɉ�����Susie-Plug in�������I�ɓǂݍ��ރN���X
//		programmed by yaneurao(M.Isozaki)	'00/05/15
//		reprogrammed by yaneurao(M.Isozaki) '00/06/21
//
//			SPI�ǂݍ��ݕ����̃T���v���\�[�X�������������A�ɂ����邳��Ɋ��ӁI

#ifndef __yaneGraphicLoader_h__
#define __yaneGraphicLoader_h__

#include <olectl.h>	//	IPicture

#include "yaneFile.h"
#include "YTL/prototype_factory.h"

///////////////////////////////////////////////////////////////////////////

typedef struct SpiPictureInfo {
	long left,top;					//	�摜��W�J����ʒu
	long width;						//	�摜�̕�(pixel)
	long height;					//	�摜�̍���(pixel)
	WORD x_density;					//	��f�̐����������x
	WORD y_density;					//	��f�̐����������x
	short colorDepth;				//	�P��f�������bit��
	HLOCAL hInfo;					//	�摜���̃e�L�X�g���
} SpiPictureInfo, *LPSpiPictureInfo;

typedef	int (PASCAL *SpiFuncIsSupported)(LPSTR, DWORD);
typedef	int (PASCAL *SpiFuncGetPictureInfo)(LPSTR, long, unsigned int, SpiPictureInfo*);
typedef	int (PASCAL *SpiFuncGetPicture)(LPSTR, long, unsigned int, HLOCAL*, HLOCAL*, FARPROC, long);

///////////////////////////////////////////////////////////////////////////

class CGraphicLoader {
public:
	virtual LRESULT LoadPicture(CFile& file);			//	����œǂݍ��ޕԂ��l��0�Ȃ�ΐ���

	virtual LRESULT GetSize(LONG& x,LONG& y);			//	����œǂݍ��񂾉摜�̃T�C�Y���擾
	virtual LRESULT Render(DWORD* p,LONG lPitch);		//	CDIB32�̃T�[�t�F�[�X�ւ̃|�C���^��n���āA�����ɕ`�悵�Ă��炤
	virtual LRESULT	Render(HDC hdc);					//	hdc��n���āA�����ɕ`�悵�Ă��炤
	virtual LRESULT	ReleasePicture();					//	����ŉ������

	virtual LRESULT	GetPalette(PALETTEENTRY pal[256]);	//	�p���b�g�̎擾

	//	���Ђ�[�B�������傦���`�O�O�G
	static PrototypeFactory<CGraphicLoader>* GetPrototypeFactory() { return& m_factory; }

	//	PrototypeFactory�Ŗ����I�ɍ���ĂˁO�O�G
	CGraphicLoader();
	virtual ~CGraphicLoader();

protected:

	virtual LRESULT LoadPictureByOwn(CFile& file);	//	bmp
	virtual LRESULT LoadPictureByOLE(CFile& file);	//	jpeg,gif
	virtual LRESULT LoadPictureBySPI(CFile& file);	//	using by SPI

	//	for LoadPictureByOwn
	BITMAPFILEHEADER*	m_lpBF;
	BITMAPINFOHEADER*	m_lpBI;
	void*				m_lpBits;

	//	for OLELoadPicture
	IPicture*	m_lpiPicture;

	//	for SPI
	HMODULE		m_hDLL;
	SpiFuncIsSupported		m_pIsSupported;
	SpiFuncGetPictureInfo	m_pGetPictureInfo;
	SpiFuncGetPicture		m_pGetPicture;
	CFile*		m_lpFile;
	HLOCAL		m_hBm;
	HLOCAL		m_hBmInfo;
	virtual void	FreeLocalMemory();

	//	���Ђ�[�B�������傦���`�O�O�G
	static PrototypeFactory<CGraphicLoader> m_factory;
};

#endif