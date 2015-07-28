/*
	prototype_factory	: FactoryMethod + PrototypePattern w/z auto_ptrEx
		programmed by yaneurao(M.Isozaki) '00/10/14
*/

#ifndef __YTLPrototypeFactory_h__
#define __YTLPrototypeFactory_h__

#include "auto_ptrEx.h"

//	Factory�e���v���[�g
class TFactoryBase {
public:
	virtual void* CreateInstance() { return NULL; }
};	//	���N���X��p�ӂ���̂́A
	//	�p���֌W�̂���Factory�N���X�Ԃł̃A�b�v�L���X�g���s�ɂȂ邽��
	//	�����o�֐��e���v���[�g���g����΂���ɉz�������Ƃ��Ȃ��̂͌����܂ł��Ȃ��B

template<class T> class Factory : public TFactoryBase {
public:
	virtual void* CreateInstance() { return new T; }
};

//	FactoryMethod + PrototypePattern�e���v���[�g
template<class T> class PrototypeFactory {
public:
	T* CreateInstance() { return reinterpret_cast<T*>(m_lpFactory->CreateInstance()); }
	void SetFactory(auto_ptrEx<TFactoryBase> lp) { m_lpFactory = lp; }

	//	�R���X�g���N�^�ł́A�f�B�t�H���gFactory�𐶐�����
	PrototypeFactory() : m_lpFactory(new Factory<T>) {}

	//	�����AT�����ۃN���X�ł���ꍇ�A���́�Factory������
	//	�R���p�C���G���[�ƂȂ�̂ŁA������邽�߂̎�i��񋟂���
	//	PrototypeFactory(void*) {}

	virtual ~PrototypeFactory() {} // place holder

protected:
	auto_ptrEx<TFactoryBase> m_lpFactory;
};

	//	�����AT�����ۃN���X�ł���ꍇ�A���́�Factory������
	//	�R���p�C���G���[�ƂȂ�̂ŁA������邽�߂̎�i��񋟂���

template<class T> class PrototypeFactoryI {
public:
	T* CreateInstance() { return reinterpret_cast<T*>(m_lpFactory->CreateInstance()); }
	void SetFactory(auto_ptrEx<TFactoryBase> lp) { m_lpFactory = lp; }

	//	�R���X�g���N�^�ł́A�f�B�t�H���gFactory�𐶐��g���Ȃ��h
	PrototypeFactoryI() {}
	virtual ~PrototypeFactoryI() {} // place holder

protected:
	auto_ptrEx<TFactoryBase> m_lpFactory;
};

#endif