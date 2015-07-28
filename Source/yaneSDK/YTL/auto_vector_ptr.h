/*
	auto_vector_ptr	: a vector of auto_ptr
		programmed & desinged by yaneurao(M.Isozaki) '00/09/27
*/

#ifndef __YTLAutoVectorPtr_h__
#define __YTLAutoVectorPtr_h__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.

#pragma warning(disable:4786)
#include <vector>
using namespace std;

#include "auto_ptrEx.h"

template<class T> class auto_vector_ptr : public vector<auto_ptrEx<T> > {
public:
	// new�����|�C���^�̏��L�����ڂ��A�ǉ��o�^����\��
	void	insert(T* pt){
		//	���L����������auto_ptrEx�𐶐����A
		//	auto_ptr�Ԃ̃R�s�[�ŁA���L�����ړ�����
		vector<auto_ptrEx<T> >::push_back(auto_ptrEx<T>(pt));
	}
	void	insert(auto_ptrEx<T> t){
		//	���L���̈ړ��𔺂�auto_ptrEx�̒ǉ��o�^�B
		vector<auto_ptrEx<T> >::push_back(t);
	}
	void	insert(){
		//	�������Ēǉ�����B
		vector<auto_ptrEx<T> >::push_back(auto_ptrEx<T>(new T));
	}
};

#endif