/*
	vectorEx	: a extra version of vector
		programmed & desinged by yaneurao(M.Isozaki) '00/10/15
*/

#ifndef __YTLVectorEx_h__
#define __YTLVectorEx_h__

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.

#pragma warning(disable:4786)
#include <vector>
#include <set>		//	pair
using namespace std;

#if _MSC_VER >= 1200	// VC++6.0upper

template<class T> class vectorEx : public vector<T> {
public:
	//	set�I�ǉ��i����v�f�̑��݂�F�߂Ȃ��j
	//	set::insert�Ɠ��@�\ /insert�o�����Ƃ�����<it,true>
	//						/insert�o���Ȃ���������<����v�f�ւ̃C�e���[�^it,false>
	pair<iterator,bool> insert(T t);

	//	set�I�폜(����v�f�̑��݂��폜����)
	//	set::erase�Ɠ��@�\/�폜��������true
	bool	erase(T t);

	//	���Ƃ��炠�����z�͎Օ�����Ă��܂��̂ŁA�Ē�`
	iterator insert(iterator it, const T& x = T()){
		vector<T>::insert(it,x);
	}
	void insert(iterator it, size_type n, const T& x){
		vector<T>::insert(it,n,x);
	}
	void insert(iterator it,
		const_iterator first, const_iterator last){
		vector<T>::insert(it,first,last);
	}
	iterator erase(iterator it){
		return vector<T>::erase(it);
	}
	iterator erase(iterator first, iterator last){
		return vector<T>::erase(first,last);
	}
};

//	���ł�chain(�|�C���^��)����������B
//		���̂́AvectorEx<T*>�ł���
template<class T> class chain : public vectorEx<T*> {};

///////////////////////////////////////////////////////////////////////////////

template<class T> pair<vectorEx<T>::iterator,bool> vectorEx<T>::insert(T t) {
	iterator it = begin();
	while (it!=end()){
		if (*it == t) {	//	����v�f����������
			return pair<vectorEx<T>::iterator,bool>(it,false);
		}
		it ++;
	}
	push_back(t);
	return pair<vectorEx<T>::iterator,bool>(end(),true);
}

template<class T> bool vectorEx<T>::erase(T t) {
	iterator it = begin();
	while (it!=end()){
		if (*it == t) {	//	����v�f����������
			iterator itnext = it;
			itnext++;
			while (itnext!=end()) {
				*it = *itnext;
				it++; itnext++;
			}
			pop_back();	//	�v�f������炷
			return true;
		}
		it ++;
	}
	return false;
}

#else

///////////////////////////////////////////////////////////////////////////////
//	VC++5�̃e���v���[�g�W�J�̃o�O�΍�
#define vectorEx set
template<class T> class chain : public set<T*> {};

#endif // _MSC_VER >= 1200
///////////////////////////////////////////////////////////////////////////////

#endif