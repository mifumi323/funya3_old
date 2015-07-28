// f3MIDIOutput.cpp: Cf3MIDIOutput �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3MIDIOutput.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

Cf3MIDIOutput::Cf3MIDIOutput()
{
	m_bSecondary = false;
	m_Option = DMUS_SEGF_MEASURE;
}

Cf3MIDIOutput::~Cf3MIDIOutput()
{

}

LRESULT Cf3MIDIOutput::SetSecondary()
{
	if (!GetDirectMusic()->CanUseDirectMusic()) return -1;
	if (m_lpDMSegment==NULL) return -2;

//	LRESULT hr = m_lpDMSegment->SetParam(GUID_StandardMIDIFile,0x00000000,0,0,
//		(void*)GetDirectMusic()->GetDirectMusicPerformance());
//	if (FAILED(hr)) return 3;
	m_bSecondary = true;
	return 0;
}

LRESULT Cf3MIDIOutput::Play()
{
	if (!m_bSecondary) {
		return CMIDIOutputDM::Play();
	}else{
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

		// ���������ύX
		HRESULT hr = GetDirectMusic()->GetDirectMusicPerformance()->PlaySegment(
			m_lpDMSegment,DMUS_SEGF_SECONDARY|m_Option,0,&m_lpDMSegmentState);

		if (FAILED(hr)) return 1;

		return 0;
	}
}

LRESULT Cf3MIDIOutput::SetLoopPos(DWORD start, DWORD end)
{
	if (m_lpDMSegment==NULL) return -2;
	if (FAILED(m_lpDMSegment->SetLoopPoints(start, end))) return 1;
	return 0;
}

LRESULT Cf3MIDIOutput::Open(string pFileName, bool secondary)
{
	if (!secondary) {
		return CMIDIOutputDM::Open(pFileName.c_str());
	}else {
		if (!GetDirectMusic()->CanUseDirectMusic()) return -1;

		Close();

		//	���̃t�@�C����ǂݍ���
		if(m_File.Read(pFileName.c_str())!=0)	{
			Err.Out("CMIDIOutputDM::Open�œǂݍ��ރt�@�C��������"+pFileName);
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
//		hr = m_lpDMSegment->SetParam(GUID_StandardMIDIFile,0xffffffff,0,0,
//			(void*)GetDirectMusic()->GetDirectMusicPerformance());
		if (FAILED(hr)) return 3;
		hr = m_lpDMSegment->SetParam(GUID_Download,0xffffffff,0,0,
			(void*)GetDirectMusic()->GetDirectMusicPerformance());
		if (FAILED(hr)) return 4;

		return SetSecondary();
	}
}

void Cf3MIDIOutput::SetPlayOption(DWORD option)
{
	m_Option = option;
}
