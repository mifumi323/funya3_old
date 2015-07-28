/*
	smart_vector_ptr	: a vector of smart_ptr
		programmed & desinged by yaneurao(M.Isozaki) '01/02/09
*/

#ifndef __YTLSmartVectorPtr_h__
#define __YTLSmartVectorPtr_h__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.

#pragma warning(disable:4786)
#include <vector>
using namespace std;

#include "smart_ptr.h"

template<class T> class smart_vector_ptr : public vector<smart_ptr<T> > {
public:
	// new�����|�C���^�̏��L�����ڂ��A�ǉ��o�^����\��
	void	insert(T* pt){
		//	���L����������smart_ptr�𐶐����A
		//	smart_ptr�Ԃ̃R�s�[�ŁA���L�����ړ�����
		smart_ptr<T> t;
		t.Add(pt);
		vector<smart_ptr<T> >::push_back(t);
	}
	void	insert(smart_ptr<T> t){
		//	���L���̈ړ��𔺂�smart_ptr�̒ǉ��o�^�B
		vector<smart_ptr<T> >::push_back(t);
	}
	void	insert(){
		//	�������Ēǉ�����B
		smart_ptr<T> t;
		t.Add(new T);
		vector<smart_ptr<T> >::push_back(t);
	}
};

#endif