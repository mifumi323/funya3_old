// BGMBase.cpp: CBGMBase �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BGMBase.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CBGMBase::CBGMBase()
{
	m_PlayNo = BGMN_SIRENT;
}

CBGMBase::~CBGMBase()
{

}

// ����������true
bool CBGMBase::Init()
{
	return true;
}

void CBGMBase::Play(BGMNumber no)
{
	if (m_PlayNo==no) return;
	Stop();
	m_PlayNo = no;
	InnerPlay(no);
}

void CBGMBase::Stop()
{
	InnerStop();
	m_PlayNo = BGMN_SIRENT;
}

BGMNumber CBGMBase::GetBGM()
{
	return m_PlayNo;
}

string CBGMBase::GetClassName()
{
	return "CBGMBase";
}
