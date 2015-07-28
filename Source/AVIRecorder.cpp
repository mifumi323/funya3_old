// AVIRecorder.cpp: CAVIRecorder クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVIRecorder.h"
#include "yaneSDK/yaneAppInitializer.h"
#include "yaneSDK/yaneFileDialog.h"
#include "yaneSDK/yanePlaneEffectBlt.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CAVIRecorder::CAVIRecorder()
{
	m_bInitialized = false;

	char filename[256];
	CFileDialog fd;
	fd.SetExt("avi");
	if (fd.GetSaveAsFile(filename)) return;

	ZERO(si);
	si.fccType = streamtypeVIDEO;
	si.fccHandler = comptypeDIB;
	si.dwScale = 1;
	si.dwRate = 40;
	si.dwQuality = (DWORD)-1;
	SetRect(&si.rcFrame, 0, 0, 320, 240);
	strcpy(si.szName, "funya3Video");
	BITMAPINFOHEADER bmih={
		sizeof(BITMAPINFOHEADER),
		320, 240,	// 画像サイズ
		1, 32,		// プレーン数(1固定)とビット深度(CDIB32なので32)
		BI_RGB,		// 圧縮なしのデータを渡すのでRGBのまま
		320*240*4,	// データサイズ(0でもいいらしいが…)
		0, 0, 0, 0	// 重要じゃないデータっぽい
	};

	AVIFileInit();

	ZERO(cv);
	cv.cbSize=sizeof(COMPVARS);
	cv.dwFlags=ICMF_COMPVARS_VALID;
	cv.fccHandler=comptypeDIB;
	cv.lQ=ICQUALITY_DEFAULT;
	if (!ICCompressorChoose(CAppInitializer::GetHWnd(),
		ICMF_CHOOSE_DATARATE | ICMF_CHOOSE_KEYFRAME, &bmih, NULL, &cv, NULL)) return;
	si.fccHandler=cv.fccHandler;
	opt.fccType=streamtypeVIDEO;
	opt.fccHandler=cv.fccHandler;
	opt.dwKeyFrameEvery=cv.lKey;
	opt.dwQuality=cv.lQ;
	opt.dwBytesPerSecond=cv.lDataRate;
	opt.dwFlags=(cv.lDataRate>0?AVICOMPRESSF_DATARATE:0)
		|(cv.lKey>0?AVICOMPRESSF_KEYFRAMES:0);
	opt.lpFormat=NULL;
	opt.cbFormat=0;
	opt.lpParms=cv.lpState;
	opt.cbParms=cv.cbState;
	opt.dwInterleaveEvery=0;

	if (AVIFileOpen(&pavi, filename, OF_CREATE | OF_WRITE | OF_SHARE_DENY_NONE,NULL)!=0)
		return;
	if (AVIFileCreateStream(pavi, &pstm, &si)!=0) return;
	if (AVIMakeCompressedStream(&ptmp, pstm, &opt, NULL)!=AVIERR_OK) return;
	if (AVIStreamSetFormat(ptmp, 0, &bmih, sizeof(BITMAPINFOHEADER))!=0) return;

	m_DIB.CreateSurface(320, 240);
	m_bInitialized = true;
}

CAVIRecorder::~CAVIRecorder()
{
	if (!m_bInitialized) return;
	AVIStreamRelease(ptmp);
	AVIStreamRelease(pstm);
	AVIFileRelease(pavi);
	ICCompressorFree(&cv);
	AVIFileExit();
}

void CAVIRecorder::Write(CDIB32* lpDIB)
{
	if (!m_bInitialized) return;
	CPlaneTransBlt::MirrorBlt2(&m_DIB, lpDIB, 0, 0, 1);
	AVIStreamWrite(ptmp, si.dwLength++, 1, m_DIB.GetPtr(), 320*240*4, AVIIF_KEYFRAME, NULL, NULL);
}
