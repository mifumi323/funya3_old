// yaneMacro.h :
//	�ȒP�ȃ}�N���W

#ifndef __yaneMacro__h__
#define __yaneMacro__h__

// �[���Ŗ��߂�}�N��
#define ZERO(var) ZeroMemory(&var,sizeof(var));

// �|�C���^����NULL�Ȃ��Release���|����}�N���B
#define RELEASE_SAFE(var)	\
	if (var != NULL) {		\
		var->Release();		\
		var = NULL;			\
	}

// �|�C���^����NULL�Ȃ��delete���|����}�N���B
#define DELETE_SAFE(var)	\
	if (var != NULL) {		\
		delete var;			\
		var = NULL;			\
	}
//	������g�킸��auto_ptr���g���܂��傤�ˁO�O�G

// �z��̃|�C���^��delete����}�N��
#define DELETEPTR_SAFE(var) \
	if (var != NULL) {		\
		delete [] var;		\
		var = NULL;			\
	}
//	������g�킸��auto_array���g���܂��傤�ˁO�O�G

//	else if�}�N���i�ד�����=p�j
#define ef else if

//	WARNING�}�N��
#ifdef _DEBUG
	#define WARNING(b,s)				\
		if(false&&b) {						\
			CHAR buf[256];				\
			wsprintf(buf,"%s�t�@�C����%d�s\n%s",__FILE__,__LINE__,s);	\
			MessageBox(NULL,buf,"WARNING!!",MB_OK);	\
			* LPLONG(0xcdcdcdcd) = 0;	\
		}
	//	�Ō��* LPLONG(0xcdcdcdcd) = 0;�̕����́A�������G���[���킴�ƋN�����R�[�h
#else
	#define WARNING(var,s) {}
	//	{}�ɂ��Ă���̂́Aif���̂���WARNING�����Ă���Ƃ��Ȃǂ̂��߁B
#endif

//	�g���[�X�p�̃}�N��:�t�@�C�����ƃ��C���i���o�[��Error.log�ɏo�͂���
#define TRACE Err.Out("%s(%d)",__FILE__,__LINE__);

//	�z��̗v�f����Ԃ��}�N��( Number of Elements)
#define NELEMS(array) (sizeof(array) / sizeof(array[0]))

#endif