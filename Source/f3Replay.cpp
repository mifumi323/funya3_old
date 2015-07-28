// f3Replay.cpp: Cf3Replay �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "f3Replay.h"
#include "f3MapObjectMain.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

Cf3Replay::Cf3Replay()
{
	m_pPlayerState=NULL;
	Reset();
}

Cf3Replay::~Cf3Replay()
{
	DELETE_SAFE(m_pPlayerState);
}

void Cf3Replay::Progress()
{
	m_nProgress++;
	m_nPointer++;
	if (m_nPointer>=REPLAYBUFFER) {
		if (++m_iPointer==m_State.end()) {
			m_State.push_back(CKeyState());
			m_iPointer = m_State.end();
			m_iPointer--;
		}
		m_nPointer=0;
	}
}

void Cf3Replay::Seek(int position)
{
	m_iPointer = m_State.begin();
	m_nPointer=m_nProgress=0;
	// position!=0�ɃV�[�N���ׂ��ł͂Ȃ��̂����ꉞ
	while(m_nProgress<position) Progress();
}

void Cf3Replay::Reset()
{
	m_State.clear();
	m_State.push_back(CKeyState());
	m_nSize=0;
	Seek();
	DELETE_SAFE(m_pPlayerState);
}

void Cf3Replay::StartRecording()
{
	Reset();
}

void Cf3Replay::Record()
{
	(*m_iPointer).pressed[m_nPointer]=(*m_iPointer).pushed[m_nPointer]=0;
	for (int i=0; i<8; i++) {
		if (f3Input.GetKeyPressed(i+1))	(*m_iPointer).pressed	[m_nPointer] |= (1<<i);
		if (f3Input.GetKeyPushed(i+1))	(*m_iPointer).pushed	[m_nPointer] |= (1<<i);
	}
	Progress();
	m_nSize++;
}

void Cf3Replay::Save(Cf3StageFile *stage, int map)
{
	if (CApp::MakeFileName(m_FileName,"f3r",theSetting->m_RecordNumber,true)) {
		Cf3StageFile data;
		DWORD chunk, size;
		BYTE* ptr;
		// �L�[���͏������[��ۂ��[����(��)
		ptr = new BYTE[m_nSize*2];
		Seek();
		while(!Finished()) {
			ptr[m_nProgress*2	] = (*m_iPointer).pressed	[m_nPointer];
			ptr[m_nProgress*2+1	] = (*m_iPointer).pushed	[m_nPointer];
			Progress();
		}
		data.SetStageData(CT_RPLY, m_nSize*2, ptr);
		delete[]ptr;
		// �K�v�ȃX�e�[�W�����R�s�[����
		if (ptr=stage->GetStageData(chunk=CT_TITL,&size))
			data.SetStageData(chunk,size,ptr);
		if (ptr=stage->GetStageData(chunk=CT_HITS,&size))
			data.SetStageData(chunk,size,ptr);
		if (ptr=stage->GetStageData(chunk=Cf3Map::GetChunkType(CT_TL00,map),&size))
			data.SetStageData(chunk,size,ptr);
		if (ptr=stage->GetStageData(chunk=Cf3Map::GetChunkType(CT_M000,map),&size))
			data.SetStageData(chunk,size,ptr);
		if (ptr=stage->GetStageData(chunk=Cf3Map::GetChunkType(CT_M100,map),&size))
			data.SetStageData(chunk,size,ptr);
		if (ptr=stage->GetStageData(chunk=Cf3Map::GetChunkType(CT_M200,map),&size))
			data.SetStageData(chunk,size,ptr);
		if (ptr=stage->GetStageData(chunk=CT_MCD0|(0<<24),&size))
			data.SetStageData(chunk,size,ptr);
		ef (ptr=stage->GetStageData(chunk=CT_MCF0|(0<<24),&size))
			data.SetStageData(chunk,size,ptr);
		if (ptr=stage->GetStageData(chunk=CT_MCD0|(1<<24),&size))
			data.SetStageData(chunk,size,ptr);
		ef (ptr=stage->GetStageData(chunk=CT_MCF0|(1<<24),&size))
			data.SetStageData(chunk,size,ptr);
		if (ptr=stage->GetStageData(chunk=CT_MCD0|(2<<24),&size))
			data.SetStageData(chunk,size,ptr);
		ef (ptr=stage->GetStageData(chunk=CT_MCF0|(2<<24),&size))
			data.SetStageData(chunk,size,ptr);
		// �ǉ��̏��
		data.SetStageData(CT_STGN, 4, &map);
		data.SetStageData(CT_GRVT, 4, &theSetting->m_Gravity);
		data.SetStageData(CT_HYPR, 4, &theSetting->m_Hyper);
		data.Write(m_FileName);
	}
}

void Cf3Replay::Replay()
{
	if (Finished()) return;
	ReplayInput.pressed	= (*m_iPointer).pressed	[m_nPointer];
	ReplayInput.pushed	= (*m_iPointer).pushed	[m_nPointer];
	m_pPlayerState->map->OnMove();
	m_pPlayerState->map->OnPreDraw();
	Progress();
}

void Cf3Replay::Load(const string& filename)
{
	m_FileName = filename;
	Reset();
	BYTE* ptr;
	DWORD size;
	m_pPlayerState = new Cf3ReplayPlayerState();
	m_pPlayerState->stage = new Cf3StageFile();
	m_pPlayerState->stage->Read(m_FileName);
	// �V�[�N���b�g�̏�ԂƓ��͏����ǂݍ���
	theSetting->m_Gravity = *m_pPlayerState->stage->GetStageData(CT_GRVT, NULL);
	theSetting->m_Hyper = *m_pPlayerState->stage->GetStageData(CT_HYPR, NULL);
	ptr = m_pPlayerState->stage->GetStageData(CT_RPLY, &size);
	m_nSize = size>>1;
	for (int i=0; i<m_nSize; i++) {
		(*m_iPointer).pressed	[m_nPointer] = ptr[m_nProgress*2	];
		(*m_iPointer).pushed	[m_nPointer] = ptr[m_nProgress*2+1	];
		Progress();
	}
	// �Ō�Ƀ}�b�v��ǂݍ���(�ݒ���ɔ��f������K�v������)
	ptr=m_pPlayerState->stage->GetStageData(CT_STGN, NULL);
	m_pPlayerState->map = new Cf3Map(m_pPlayerState->stage, ptr?*ptr:0);
	Seek();
}
