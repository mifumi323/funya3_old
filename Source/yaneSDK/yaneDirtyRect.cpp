#include "stdafx.h"
#include "yaneDirtyRect.h"
#include <limits.h>

//	���Z�F�͈͎w��^�C�v
void	CDirtyRect::AddRect(int left,int top,int right,int bottom, int nSize){
	// �z��̗v�f��������Ȃ��჊�T�C�Y
	if(m_nRectSize < nSize) {
		Resize(nSize);
	}

	int nRectLast = m_nRectLast;
	::SetRect(&m_arcRect[nRectLast],left,top,right,bottom);
	m_anRectArea[nRectLast] = (right-left)*(bottom-top);

	if (nRectLast!=nSize/*nRectSize*/){
		//	�܂�����܂���̂ŁA���̂܂܉��Z�����̂ŏI���ł���
			//	�ʐς������Ă���
		m_nRectLast++;
		return ;
	}

	//	��������ȏ�A����܂�����B
	//	�K�[�x�W���܂���

	//	�s���j�t
	//		�݂��Ɍ������邱�Ƃɂ���āA��������ʐς��ŏ��ƂȂ���̂�I�����A�폜����
	int min_area = INT_MAX;			//	�����ɂ���đ�������ʐς̍ŏ��l
	int	min_a;						//	���������Ƃ��̖ʐ�
	int min_i,min_j;				// ���̂Ƃ���i,j�̒l
	int	nLeft,nTop,nRight,nBottom;	// ���̂Ƃ��̋�`
	
	for(int i=0;i<nRectLast;i++){
		for(int j=i+1;j<nRectLast+1;j++){
			//	��`i�Ƌ�`j���������Ă݂�

			//	�����O�̖ʐ�
			int nAI = m_anRectArea[i];
			int nAJ = m_anRectArea[j];
			//	��`�̊O�����擾
			int left	= m_arcRect[i].left    < m_arcRect[j].left   ? m_arcRect[i].left   : m_arcRect[j].left;
			int top		= m_arcRect[i].top     < m_arcRect[j].top    ? m_arcRect[i].top    : m_arcRect[j].top;
			int right	= m_arcRect[i].right   > m_arcRect[j].right  ? m_arcRect[i].right  : m_arcRect[j].right;
			int bottom	= m_arcRect[i].bottom  > m_arcRect[j].bottom ? m_arcRect[i].bottom : m_arcRect[j].bottom;
			//	�������ꂽ�ʐς��擾
			int nAC = (right - left) * (bottom-top);
			//	�������邱�Ƃɂ���ĕω����鍷�����擾
			int nADiff = nAC - (nAI + nAJ);
			if (nADiff < min_area) {
				min_a    = nAC;
				min_area = nADiff;
				min_i = i; min_j=j;
				nLeft = left; nTop = top; nRight = right; nBottom = bottom;
			}
		}
	}

	//	min_i��min_j����������
		//	min_i�ɑ��
	m_anRectArea[min_i] = min_a;
	::SetRect(&m_arcRect[min_i],nLeft,nTop,nRight,nBottom);
		//	min_j������
	if (min_j!=nRectLast) {
		//	�R�s��K�v������Ƃ������R�s��
		m_anRectArea[min_j] = m_anRectArea[nRectLast];
		m_arcRect[min_j] = m_arcRect[nRectLast];
	}
}

//	���Z�F�͈͎w��^�C�v
void	CDirtyRect::SubRect(int left,int top,int right,int bottom){
	smart_ptr<RECT> r(new RECT,true);
	::SetRect(r, left, top, right, bottom);
	m_vSubRect.insert( r );
}

void	CDirtyRect::Refresh(){
	//	�s���j�t
	//		�P�D�݂��Ɍ������邱�Ƃɂ���āA��������ʐς��O�ȉ��Ȃ�΁A�������č폜����
	//		�Q�D�������������ɓ��荞��ł���̂Ȃ�΁A������ؒf����

	int nRectLast = m_nRectLast;

	for(int i=0;i<nRectLast-1;i++){
		for(int j=i+1;j<nRectLast;){
			//	��`i�Ƌ�`j���������Ă݂�

			//	�����O�̖ʐ�
			int nAI = m_anRectArea[i];
			int nAJ = m_anRectArea[j];
			//	��`�̊O�����擾
			int left	= m_arcRect[i].left    < m_arcRect[j].left   ? m_arcRect[i].left   : m_arcRect[j].left;
			int top		= m_arcRect[i].top     < m_arcRect[j].top    ? m_arcRect[i].top    : m_arcRect[j].top;
			int right	= m_arcRect[i].right   > m_arcRect[j].right  ? m_arcRect[i].right  : m_arcRect[j].right;
			int bottom	= m_arcRect[i].bottom  > m_arcRect[j].bottom ? m_arcRect[i].bottom : m_arcRect[j].bottom;
			//	�������ꂽ�ʐς��擾
			int nAC = (right - left) * (bottom-top);
			//	�������邱�Ƃɂ���ĕω����鍷�����擾
			int nADiff = nAC - (nAI + nAJ);
			if (nADiff <= 0) {	//	�������邱�Ƃɂ���ď������Ȃ�
				//	i��j�͌�������
				m_anRectArea[i] = nAC;
				::SetRect(&m_arcRect[i],left,top,right,bottom);
				//	j������
				if (j!=nRectLast-1) {
				//	�R�s��K�v������Ƃ������R�s��
					m_anRectArea[j] = m_anRectArea[nRectLast-1];
					m_arcRect[j] = m_arcRect[nRectLast-1];
				}
				nRectLast--;
			} else {
				//	�폜���������Ȃ������Ƃ��̂�j�����Z
				j++;
			}
		}
	}
	m_nRectLast = nRectLast;

	////////////////////////////////////////////////////////////////////////
	// ���Z����
	////////////////////////////////////////////////////////////////////////

	int nSize = m_vSubRect.size();
	if(nSize > 0) { // ���Z����K�v�����邩�H
		// ���Z���Ă΂�΂�ɂȂ�����`���i�[����vector
		smart_vector_ptr<RECT> rects;

		// ���ꂶ�Ⴀ���Z���Ă݂邩�`
		for(int i=0;i<nSize;i++) {
			// ���Z�����`�����߂�
			int div_l = m_vSubRect[i]->left;
			int div_t = m_vSubRect[i]->top;
			int div_r = m_vSubRect[i]->right;
			int div_b = m_vSubRect[i]->bottom;

			// ���Z��̋�`�͕����̋�`�ɕ����ł���̂őS�Ē��ׂĂ���ēxAddRect����
			for(int j=0;j<nRectLast;j++){
				int base_l = m_arcRect[j].left;
				int base_t = m_arcRect[j].top;
				int base_r = m_arcRect[j].right;
				int base_b = m_arcRect[j].bottom;

				////////////////////////////////////////////////////////////////////////
				// ���O����̃��t�`�F�b�N
				////////////////////////////////////////////////////////////////////////

				// �d�Ȃ�͈͖���
				if( div_b <= base_t || base_b <  div_t || div_r <= base_l || base_r <  div_l ){
					// ���̋�`��S�̈�o�^
					smart_ptr<RECT> r(new RECT,true);
					::SetRect(r, base_l, base_t, base_r, base_b);
					rects.insert( r );
					continue;
				}
				// �����킳���Ă���
				if( div_l <= base_l && div_t <= base_t && base_r <= div_r && base_b <= div_b ){
					continue; // ���̋�`�͖���
				}

				////////////////////////////////////////////////////////////////////////
				// �����܂ł���ƁA�d�Ȃ�͈͂������I�ɂ���
				// �ȉ��̔ԍ��͐}���Q�l�ɁE�E�E
				////////////////////////////////////////////////////////////////////////

				if( div_t <= base_t ){ // 1.2.3.4.5.6.10.
					if( base_b <= div_b ){ // 4.5.6.
						if( base_l < div_l ){ // 5.6.
							smart_ptr<RECT> r(new RECT,true);
							::SetRect(r, base_l,base_t,div_l,base_b);
							rects.insert( r );
						}
						if( div_r < base_r ){ // 4.5.
							smart_ptr<RECT> r(new RECT,true);
							::SetRect(r, div_r,base_t,base_r,base_b);
							rects.insert( r );
						}
					}
					ef( div_b < base_b ){ // 1.2.3.10.
						if( base_l < div_l ){ // 2.3.
							smart_ptr<RECT> r(new RECT,true);
							::SetRect(r, base_l,base_t,div_l,div_b);
							rects.insert( r );
						}
						if( div_r < base_r ){ // 1.2.
							smart_ptr<RECT> r(new RECT,true);
							::SetRect(r, div_r,base_t,base_r,div_b);
							rects.insert( r );
						}
						// 1.2.3.10.
						{
							smart_ptr<RECT> r(new RECT,true);
							::SetRect(r, base_l,div_b,base_r,base_b);
							rects.insert( r );
						}
					}
				}
				ef( base_t < div_t ){ // 7.8.9.11.12.13.14.15.
					// 7.8.9.11.12.13.14.15.
					{
						smart_ptr<RECT> r(new RECT,true);
						::SetRect(r, base_l,base_t,base_r,div_t);
						rects.insert( r );
					}

					if( base_b <= div_b ){ // 7.8.9.11.
						if( div_r < base_r ){ // 7.8.
							smart_ptr<RECT> r(new RECT,true);
							::SetRect(r, div_r,div_t,base_r,base_b);
							rects.insert( r );
						}
						ef( base_l < div_l ){ // 8.9.
							smart_ptr<RECT> r(new RECT,true);
							::SetRect(r, base_l,div_t,div_l,base_b);
							rects.insert( r );
						}
					}
					ef( div_b < base_b ){ // 12.13.14.15.
						if( base_l < div_l ){ // 14.15.
							smart_ptr<RECT> r(new RECT,true);
							::SetRect(r, base_l,div_t,div_l,div_b);
							rects.insert( r );
						}
						if( div_r < base_r ){ // 13.15.
							smart_ptr<RECT> r(new RECT,true);
							::SetRect(r, div_r,div_t,base_r,div_b);
							rects.insert( r );
						}
						// 12.13.14.15.
						{
							smart_ptr<RECT> r(new RECT,true);
							::SetRect(r, base_l,div_b,base_r,base_b);
							rects.insert( r );
						}
					}
				}

			} // for
		}

		// �� ���Z�����`���폜�������ʂ̐V������`�W�����g����DirtyRect���č\�z
		int nNewSize = rects.size();

		Clear(); // ��U������

		// �z��̗v�f��������Ȃ��჊�T�C�Y
		if(m_nRectSize < nNewSize) {
			Resize(nNewSize);
		}

		// �������������̂ōč\�z���Ċ���
		for(int j = 0; j < nNewSize; j++) {
			AddRect( *rects[j], nNewSize );
		}
	}
}