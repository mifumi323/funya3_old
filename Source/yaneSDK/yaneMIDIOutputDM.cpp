#include "stdafx.h"

#ifdef USE_MIDIOutputDM

#include "yaneMIDIOutputDM.h"
#include "yaneFile.h"
#include "yaneCOMBase.h"
#include "yaneDirectSound.h"

#define sign(val) (val>0?1:(val<0?-1:0))	//	������Ԃ� * yane

///////////////////////////////////////////////////////////////////////////////
bool CDirectMusic::m_bFirst = true;
bool CDirectMusic::m_bCanUseDirectMusic = true;
int		CDirectMusic::m_nRef = 0;
auto_ptrEx<CDirectMusic> CDirectMusic::m_lpCDirectMusic;

bool	CDirectMusic::CanUseDirectMusic(void) const {
	return m_bCanUseDirectMusic;
}

CDirectMusic::CDirectMusic(void){
	HRESULT hr;

	//	DirectMusic���g���邩��Ԃ�
	CCOMBase::AddRef();
	if (m_bFirst) {
		m_bFirst = false;
		hr = ::CoCreateInstance(CLSID_DirectMusicPerformance,NULL,CLSCTX_INPROC,
			IID_IDirectMusicPerformance,(void**)&m_lpDMPerf);
		if (FAILED(hr)) {
			m_bCanUseDirectMusic = false;
			m_lpDMPerf = NULL;	//	NULL��ۏ؂���
			goto ErrEnd;
		}

		//	�����DirectSound���珉��������
		CDirectSound::AddRef();
		m_lpDMusic = NULL;	//	���������Ă����Ȃ���bug��B
							//	�Ԃ��l�̂��߂ɓn���ϐ��̏�������v�����邾�Ȃ�āA
							//	�Ƃ�ł��Ȃ���>DirectMusic
		hr = m_lpDMPerf->Init(  &m_lpDMusic /* NULL*/,CDirectSound::GetDirectSound(),NULL);
		if (FAILED(hr)) {
			m_bCanUseDirectMusic = false;
			m_lpDMusic = NULL;	//	NULL��ۏ؂���
			CDirectSound::DelRef();
			goto ErrEnd;
		}

		//	�������A�|�[�g�i�V���Z�T�C�U�j�̑I���͍s�Ȃ�
		hr = m_lpDMPerf->AddPort(NULL);
		//	���m_lpDMusic���󂯂Ă���ꍇ�́A�������������|�[�g�łȂ���΂Ȃ�Ȃ�
		if (FAILED(hr)) {
//			m_bCanUseDirectMusic = false;
			//	�T�[�h�p�[�e�B���̃\�t�g�V���Z�̓f�B�t�H���g��
			//	�I������邱�Ƃ͖����̂ł��̏ꍇ�A���s����
//			goto ErrEnd;
		//	�����ǃh���}�C�i�΁j
		}

		hr = ::CoCreateInstance(CLSID_DirectMusicLoader,NULL,CLSCTX_INPROC,
			IID_IDirectMusicLoader,(void**)&m_lpDMLoader);

		if (FAILED(hr)) {
			m_bCanUseDirectMusic = false;
			m_lpDMLoader = NULL;
			CDirectSound::DelRef();
			goto ErrEnd;
		}

		//	�������Đ����s�Ȃ��̂ŃZ�O�����g�̃L���b�V���ݒ��؂�
		//	�����؂��Ă��o�O��悤���O�O�G
		m_lpDMLoader->EnableCache(GUID_DirectMusicAllTypes, false);
		m_lpDMLoader->EnableCache(CLSID_DirectMusicSegment, false);

		//	���ׂẴe�X�g�ɍ��i�����Ƃ��̂�true�ɂ���
		m_bCanUseDirectMusic = true;
	}

ErrEnd:;
}

CDirectMusic::~CDirectMusic(){
	RELEASE_SAFE(m_lpDMLoader);
	
	if (m_bCanUseDirectMusic) {
		CDirectSound::DelRef();
	}

	//		m_lpDMusic->Release();

	if (m_lpDMPerf!=NULL){
		m_lpDMPerf->CloseDown();
		m_lpDMPerf->Release();
	}
	CCOMBase::Release();
}

///////////////////////////////////////////////////////////////////////////////

CMIDIOutputDM::CMIDIOutputDM(void)
{
	CDirectMusic::AddRef();
	m_lpDMSegment = NULL;
	m_lpDMSegmentState = NULL;
	m_bPaused = 0;
	m_bLoopPlay = false;
	m_mtPosition = 0;
}

CMIDIOutputDM::~CMIDIOutputDM()
{
	Close();
	CDirectMusic::DelRef();
}

////////////////
//	�I�[�v��
///////////////
LRESULT CMIDIOutputDM::Open(LPCTSTR pName)
{
	if (!GetDirectMusic()->CanUseDirectMusic()) return -1;

	Close();

	//	���̃t�@�C����ǂݍ���
	if(m_File.Read((LPSTR)pName)!=0)	{
		Err.Out("CMIDIOutputDM::Open�œǂݍ��ރt�@�C��������"+string(pName));
		return 1;
	}

/*	�_�������O�O�G
	//	��������΁A�O��̃���������������O�ɐV������������new�����
	//	���Ƃ��ۏ؂����B���Ȃ킿�A�O��Đ��������̂Ƃ͈Ⴄ�������A�h���X��
	//	�n�邱�Ƃ��ۏ؂����
	auto_array<BYTE> cache(m_File.GetSize());
	::CopyMemory((void*)cache,m_File.GetMemory(),m_File.GetSize());
	Err.Out((int)(BYTE*)cache);
	m_alpMIDICache = cache;	//	�j��I�R�s�[
*/

/*
	//	�d���Ȃ��̂Ńe���|�����t�@�C���𐶐�
	if (m_File.CreateTemporary()!=0)	{
		Err.Out("CMIDIOutputDM::Open��CreateTemporary�Ɏ��s");
		return 2;
	}
*/

	DMUS_OBJECTDESC desc;
	desc.dwSize = sizeof(DMUS_OBJECTDESC);
	desc.guidClass = CLSID_DirectMusicSegment;
	desc.pbMemData = (BYTE*)m_File.GetMemory(); // (BYTE*)m_alpMIDICache;
	desc.llMemLength = m_File.GetSize();
	desc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_MEMORY | DMUS_OBJ_LOADED;

/*
	#define MULTI_TO_WIDE( x,y )	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, y, -1, x, _MAX_PATH );
	WCHAR	szFilename[_MAX_PATH];
	MULTI_TO_WIDE(szFilename,CFile::MakeFullName(m_File.GetName()).c_str());
	::wcscpy( desc.wszFileName,szFilename);				// �t�@�C�����w��
	desc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;	// �\���̓��̗L���ȏ��������t���O��ݒ�
*/

	HRESULT hr;
	hr = GetDirectMusic()->GetDirectMusicLoader()->GetObject(
			&desc,IID_IDirectMusicSegment,(void**)&m_lpDMSegment);
	if (FAILED(hr)){
		m_lpDMSegment = NULL;
		return 2;
	}
	hr = m_lpDMSegment->SetParam(GUID_StandardMIDIFile,0xffffffff,0,0,
		(void*)GetDirectMusic()->GetDirectMusicPerformance());
	if (FAILED(hr)) return 3;
	hr = m_lpDMSegment->SetParam(GUID_Download,0xffffffff,0,0,
		(void*)GetDirectMusic()->GetDirectMusicPerformance());
	if (FAILED(hr)) return 4;

	return 0;
}

//////////
//	�N���[�Y
//////////
LRESULT CMIDIOutputDM::Close(void)
{
	m_File.Close();	//	delete temporary file
	Stop();	// stop���Ȃ���Close�ł��Ȃ���[��

	RELEASE_SAFE(m_lpDMSegmentState);

	if (m_lpDMSegment!=NULL) {
		//	�Q�Ƃ̉��
		//	GetDirectMusic()->GetDirectMusicLoader()->ReleaseObject((IDirectMusicObject*)m_lpDMSegment);
		//	���ꂪ�����ƃo�O��O�O�G
		GetDirectMusic()->GetDirectMusicLoader()->ClearCache(CLSID_DirectMusicSegment);

		//	DLS�̃A�����[�h
		m_lpDMSegment->SetParam(GUID_Unload,0xffffffff,0,0,(void*)GetDirectMusic()->GetDirectMusicPerformance());
		m_lpDMSegment->Release();
	}

	return 0;
}

//////////
//	���t����
/////////

LRESULT CMIDIOutputDM::Play(void)
{
	if (!GetDirectMusic()->CanUseDirectMusic()) return -1;
	if (m_lpDMSegment==NULL) return -2;

	m_bPaused = 0;

	Stop();

	//	�Đ��J�n
	if (!m_bLoopPlay) {
		m_lpDMSegment->SetRepeats(0);	//	�J��Ԃ��Ȃ�
	} else {
		m_lpDMSegment->SetRepeats(-1);	//	�񐔁�
	}
	m_mtPosition = 0;
	m_lpDMSegment->SetStartPoint(m_mtPosition);

	HRESULT hr = GetDirectMusic()->GetDirectMusicPerformance()->PlaySegment(
		m_lpDMSegment,DMUS_SEGF_DEFAULT,0,&m_lpDMSegmentState);

	if (FAILED(hr)) return 1;

	return 0;
}

LRESULT CMIDIOutputDM::Replay(void)
{
	if (!GetDirectMusic()->CanUseDirectMusic()) return -1;
	if (m_lpDMSegment==NULL) return -2;

	if (m_bPaused==0) return 0;		//	pause���Ăւ�āI
	if (--m_bPaused!=0) return 0;	//	�Q�ƃJ�E���g����

	//	�Ȃ����Đ����Ȃ̂ŉ��������ɋA��
	if (IsPlay()) return 0;

	Stop();

	//	�Đ��ĊJ
	m_lpDMSegment->SetStartPoint(m_mtPosition);	//	m_mtPosition�͑O��Pause�����ʒu
	if (!m_bLoopPlay) {
		m_lpDMSegment->SetRepeats(0);	//	�J��Ԃ��Ȃ�
	} else {
		m_lpDMSegment->SetRepeats(-1);	//	�񐔁�
	}
	HRESULT hr = GetDirectMusic()->GetDirectMusicPerformance()->PlaySegment(
		m_lpDMSegment,DMUS_SEGF_DEFAULT,0,&m_lpDMSegmentState);

	if (FAILED(hr)) return 1;

	m_bPaused = 0;
	return 0;
}

LRESULT CMIDIOutputDM::Stop(void)
{
	if (!GetDirectMusic()->CanUseDirectMusic()) return -1;

	m_bPaused += sign(m_bPaused);	//	�K�E�Z:p
	if (!IsPlay()) return 0;			//	���łɒ�~���Ă���

	//	�Đ����̂�pause�����Ȃ��
	m_bPaused = 1;

	//	���݂̏�ԓǂݏo���Ē�~
	if (m_lpDMSegment!=NULL && m_lpDMSegmentState!=NULL) {
		GetDirectMusic()->GetDirectMusicPerformance()->Stop(m_lpDMSegment,m_lpDMSegmentState,0,0);
		if (FAILED(m_lpDMSegmentState->GetSeek(&m_mtPosition))) {	//	m_mtPosition�͑O��Pause�����ʒu
			m_mtPosition = 0;
		}
		RELEASE_SAFE(m_lpDMSegmentState);
	}

	return 0;
}

LONG	CMIDIOutputDM::GetCurrentPos(void){
	if (!GetDirectMusic()->CanUseDirectMusic()) return -1;

	if (m_lpDMSegmentState==NULL) return -2;

	// ���݂̍Đ��|�W�V�����𓾂�
	MUSIC_TIME mt;
	if (FAILED(m_lpDMSegmentState->GetSeek(&mt))) return -3;
	//	�������œ����̂́A�e���|�ɂ�鑊�Ύ����Ȃ̂Ł��ŕϊ����Ă��K�v������

	REFERENCE_TIME rt;
	if (FAILED(GetDirectMusic()->GetDirectMusicPerformance()->MusicToReferenceTime(
		mt,&rt))) return -4;
	REFERENCE_TIME rt2;
	if (FAILED(GetDirectMusic()->GetDirectMusicPerformance()->MusicToReferenceTime(
		0,&rt2))) return -4;
	rt-=rt2;	//	���낵�����ƂɁA�x�[�X����̒l�ł͂Ȃ��̂�
				//	���̂悤�ȕ␳�����K�v�ɂȂ�DirectMusic�̃o�O�̂悤�ȋC�����Ȃ����Ȃ��B

	return rt/10000;	//	�����[ms]�P�ʁiREFERENCE_TIME�͕���\���肷���I�i�΁j�j
}

LRESULT CMIDIOutputDM::SetCurrentPos(LONG lPos){
	HRESULT hr;
	if (!GetDirectMusic()->CanUseDirectMusic()) return -1;
	if ( m_lpDMSegment == NULL ) return -1;
	MUSIC_TIME mt;
	LONG lLen = GetLength();
	hr = m_lpDMSegment->GetLength(&mt);

//	�Ȃ�ł��߂Ȃ񂾂낤�c
//	REFERENCE_TIME rt = lPos*10000;
//	hr = GetDirectMusic()->GetDirectMusicPerformance()->ReferenceToMusicTime(rt,&mt);
	Pause();
	//m_mtPosition;
	m_mtPosition = (DWORDLONG) lPos * mt / lLen;
	Replay();

	return 0;
}

LRESULT CMIDIOutputDM::SetVolume(LONG volume){
	if (!GetDirectMusic()->CanUseDirectMusic()) return -1;
	LONG v = volume;
	GetDirectMusic()->GetDirectMusicPerformance()->SetGlobalParam(GUID_PerfMasterVolume,&v,sizeof(v));
	return 0;
}


LONG	CMIDIOutputDM::GetVolume(void)
{
	if (!GetDirectMusic()->CanUseDirectMusic()) return -1;
	LONG v;
	GetDirectMusic()->GetDirectMusicPerformance()->GetGlobalParam(GUID_PerfMasterVolume,(LPVOID)&v,sizeof(v));
	return v;
}

LRESULT CMIDIOutputDM::SetLoopMode(bool bLoop){
	m_bLoopPlay = bLoop;
	return 0;
}

bool CMIDIOutputDM::IsPlay(void){
	if (!GetDirectMusic()->CanUseDirectMusic()) return false;
	if (m_lpDMSegmentState==NULL) return false;
	return GetDirectMusic()->GetDirectMusicPerformance()->IsPlaying(m_lpDMSegment,m_lpDMSegmentState)==S_OK;
}

LRESULT	CMIDIOutputDM::LoopPlay(void){
	if (m_bLoopPlay) {
		return Play();
	} else {
		return Stop();
	}
}

LONG	CMIDIOutputDM::GetLength(void){
	if (!GetDirectMusic()->CanUseDirectMusic()) return -1;

	if (m_lpDMSegment==NULL) return -2;

	MUSIC_TIME mt;
	if ( FAILED(m_lpDMSegment->GetLength(&mt))) return -3;

	REFERENCE_TIME rt;
	if (FAILED(
		GetDirectMusic()->GetDirectMusicPerformance()->MusicToReferenceTime(mt,&rt)
		)) return -4;
	REFERENCE_TIME rt2;
	if (FAILED(GetDirectMusic()->GetDirectMusicPerformance()->MusicToReferenceTime(
		0,&rt2))) return -4;
	rt-=rt2;
	return rt/10000;
}

#endif