//	Dirty Rect (�X�V��`)�@�Ǘ��N���X
//
//		programmed and designed by yaneurao '02/01/16

#ifndef __yaneDirtyRect_h__
#define __yaneDirtyRect_h__

/*
	�g�������B
		Clear�ŃN���A�B
		AddRect�Ȃ�Ƃ��ŁA��`�����Z���Ă���
		�i�`�悲�ƂɊ��x�ƂȂ��J��Ԃ��j
		���̂Ƃ��A���̃N���X�̓����ŁA�ő�X�V��`(dirty rect)��
		�Ȃ�悤�Ɍ�������Ă����B�������A��ʍ���̏�����
		�E���̏������X�V���������Ȃ̂ɉ�ʑS�̂��ő�X�V��`(dirty rect)
		�Ƃ��đI������Ă��܂��̂͂܂����̂ŁA���̂悤�ȃP�[�X�ɂ��Ȃ���
		�ő�5��dirty rect�������Ă��ǂ����Ƃɂ���B

		�Ō�ARefresh���Ăяo�����Ƃɂ��A�d������i���邢�͊��S�Ɋ܂܂��jdirty rect��
		��������B

		�����āAGetRectNum,GetRect�ŁAdirty rect�̐��Ƌ�`��
		�擾�ł���B


		SubRect�Ȃ�Ƃ��ŁA��`�����Z�ł���
		���̌��ʂ́AAddRect�ƈႢ�ARefresh����܂Ŕ��f����Ȃ����Ƃɒ��ӂ���B
*/

class CDirtyRect {
public:

	//	���ׂĂ��N���A����
	void	Clear(){ 
		m_nRectLast = 0;
		m_vSubRect.clear();
	}

	//  �z������T�C�Y����
	void    Resize(int size){
		m_arcRect.resize(size+1);
		m_anRectArea.resize(size+1);
		m_nRectSize = size;
	}

	//	���t���b�V������
	void	Refresh();

	//	��`�̉��Z
		//	�͈͎w��^�C�v
	void	AddRect(int left,int top,int right,int bottom, int nSize=nRectSize);
		//	���W�{�傫���w��^�C�v
	void	AddRectSize(int x,int y,int sx,int sy, int nSize=nRectSize){ AddRect(x,y,x+sx,y+sy,nSize); }
		//	��`�ŉ��Z
	void	AddRect(const RECT& rc, int nSize=nRectSize){ AddRect(rc.left,rc.top,rc.right,rc.bottom,nSize); }

	//	��`�̌��Z
		//	�͈͎w��^�C�v
	void	SubRect(int left,int top,int right,int bottom);
		//	���W�{�傫���w��^�C�v
	void	SubRectSize(int x,int y,int sx,int sy){ SubRect(x,y,x+sx,y+sy); }
		//	��`�ŉ��Z
	void	SubRect(const RECT& rc){ SubRect(rc.left,rc.top,rc.right,rc.bottom); }

	//	--- property

	//	���ʂ̎擾
		//	�������ꂽ��`���v��(0:�Ȃ�)
	int		GetRectNum() { return m_nRectLast; }
		//	�������ꂽ��`�ւ̃|�C���^���擾
	RECT*	GetRect() { return &m_arcRect[0]; }

	//	-----

	CDirtyRect() { Resize(nRectSize); Clear(); }
	virtual ~CDirtyRect(){};

protected:
	enum	{ nRectSize = 5 };
	//	���̐�������`���m��
	//	�܂��A���܂�ׂ����Ƃ�ƁA�K�[�x�W�̌�����������̂�
	//	5���炢�ɂ��Ă����B

	//RECT	m_arcRect[nRectSize+1];	   //	��`���
	//int	m_anRectArea[nRectSize+1]; //   ��`�ʐ�
	smart_array<RECT>	m_arcRect;	   //	��`���
	smart_array<int>	m_anRectArea;  //   ��`�ʐ�
	int		m_nRectLast;	//	���������܂Ŏg���Ă��邩�̃J�E���^
							//	�������==0 : ���g�p
	int     m_nRectSize;	//  ���̃T�C�Y

	//  ���Z���邽�߂ɕK�v
	smart_vector_ptr<RECT> m_vSubRect;
};

#endif