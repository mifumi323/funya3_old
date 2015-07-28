/*
	auto_ptrDx	:
		programmed by yaneurao(M.Isozaki) '01/01/16
*/

#ifndef __YTLAutoPtrDx_h__
#define __YTLAutoPtrDx_h__

#include "auto_ptrEx.h"
#include "prototype_factory.h"

//	T�����ۃN���X�łȂ��Ƃ��p
template <class T> class auto_ptrDx : public auto_ptrEx<T> {
public:
	T* get() {
		if (m_lp == NULL)
			m_lp.Add(m_ProtoType.CreateInstance());
		//	�����Am_lp�����݂��Ȃ���΁A�v���g�^�C�v�t�@�N�g���[�Ő������ĕԂ�

		return m_lp;
	}
	void SetFactory(auto_ptrEx<TFactoryBase> p) { m_pProtoType.SetFactory(p); }
	//	�����Factory��ύX���Ă��΁A�f�B�t�H���g�Ƃ���T�h���N���X��ݒ肵�Ă�邱�Ƃ��\�ł���B

protected:
	PrototypeFactory<T> m_ProtoType;
};

//	T�����ۃN���X�ł���Ƃ��p
template <class T> class auto_ptrDxI : public auto_ptrEx<T> {
public:
	T* get() {
		if (m_lp == NULL)
			m_lp.Add(m_ProtoType.CreateInstance());
		//	�����Am_lp�����݂��Ȃ���΁A�v���g�^�C�v�t�@�N�g���[�Ő������ĕԂ�

		return m_lp;
	}
	void SetFactory(auto_ptrEx<TFactoryBase> p) { m_pProtoType.SetFactory(p); }
	//	�����Factory��ύX���Ă��΁A�f�B�t�H���g�Ƃ���T�h���N���X��ݒ肵�Ă�邱�Ƃ��\�ł���B

protected:
	PrototypeFactoryI<T> m_ProtoType;
	//	�����AT�����ۃN���X�ł���ꍇ�A����Factory������
	//	�R���p�C���G���[�ƂȂ�̂ŁA������邽�߂�PrototypeFactoryI���g��
};

#endif