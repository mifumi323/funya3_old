#ifndef __STDAFX_H__
#define __STDAFX_H__

//	�ŏ��ɐݒ��ǂݍ���ǂ����O�O�G
#include "config/yaneConfig.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// DirectSound���g�����B�����̎g���w�b�_�[���炢������include���[��...
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

//	mpeg�p�̍\���̂�����̂Ńv���b�g�t�H�[��SDK����ŐV�̂ƍ����ւ��ĂˁI
#include <mmreg.h>

//	DirectX�֌W��GUID�̐ݒ�̂���
#pragma comment(lib,"dxguid.lib")

// �m�s�œ��삳����̂ŁA�o�[�W������DirectX3���I�I
#define DIRECTDRAW_VERSION	0x0300
#define DIRECTSOUND_VERSION 0x0300
//	��DirectX7����DSBUFFERDESC�\���̂̃T�C�Y���傫���Ȃ��Ă���̂ł��̑΍�

//#include <dinput.h>			// ������Ȃ�w�b�_�[�ǂ�ǂ����[
//#pragma comment(lib,"dinput.lib")
//	�����N�������Ȃ����ǁAdinput.lib�̃O���[�o���ϐ����L�[�f�o�C�X�擾��
//	�K�v�Ȃ̂ŁA�d�����Ȃ��B�Ȃ�ł���Ȑ݌v�ɂȂ��Ă�񂩂ȁ[�BBCC5.5�Ȃ�ΊO���āB

#include <ddraw.h>			// DirectDraw header
#include <dsound.h>			// DirectSound header

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#pragma warning(disable:4786)
#pragma warning(disable:4503)

// STL�֘A
#include <set>
#include <list>
#include <vector>
#include <string>
#include <stack>
#include <map>
using namespace std;

#include <stdlib.h>		// _MAX_PATH,qsort,rand..
#include <math.h>

//	��������֗����ȁ`�O�O
#include "yaneError.h"
#include "YTL/yaneMacro.h"
#include "YTL/auto_ptrEx.h"
#include "YTL/auto_array.h"
#include "YTL/auto_vector_ptr.h"
#include "YTL/vectorEx.h"
#include "YTL/swap.h"
#include "YTL/auto_ptrDx.h"
#include "YTL/function_callbacker.h"
#include "YTL/smart_ptr.h"
#include "YTL/smart_array.h"
#include "YTL/smart_vector_ptr.h"
#include "YTL/mediator.h"

// �����̂��ǂ�ǂ��ā[��
#include "../f3TL.h"

#endif