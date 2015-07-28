// yaneError.h	:
//	�G���[���O�o�͗p�N���X
//
//	programmed by yaneurao		'99/06/21
//	reprogrammed by yaneurao	'00/02/25

#ifndef __yaneError_h__
#define __yaneError_h__

#ifdef _DEBUG // debug�V���{������`����Ă���Ȃ�A�G���[�o�́I
  #undef USE_ErrorLog
  #define USE_ErrorLog
#endif

#ifdef USE_ErrorLog

class CErrorLog {					//	�f�o�b�O�p�G���[���O�o��
public:
	virtual LRESULT	Out(string);			//	�����G���[�o�͗p�B
	virtual LRESULT	Out(int);				//	�����G���[�o�͗p�B
	virtual LRESULT __cdecl Out(LPSTR fmt, ... ); // �����G���[�o�͗p
	virtual LRESULT	SelectDevice(int);		//	�G���[���O�o�͐�B(0:�Ȃ� 1:File)
	virtual string	GetError(void) const;			//	�O��G���[�o�͂������̂��擾����B

	CErrorLog(void);
	virtual ~CErrorLog();

protected:
	int		m_nDevice;			//	�o�͐�f�o�C�X
	string	m_oErrorMes;		//	�O��o�͂̃G���[���b�Z�[�W
};

#else	// USE_ErrorLog || _DEBUG
//	��̃N���X��`�ɂ���

class CErrorLog {					//	�f�o�b�O�p�G���[���O�o��
public:
	LRESULT	Out(string){ return 0; }
	LRESULT	Out(int){ return 0; }
	LRESULT __cdecl Out(LPSTR fmt, ... ){ return 0;}
	LRESULT	SelectDevice(int){ return 0;}
	string	GetError(void) const{ return string("");}
};

#endif  // USE_ErrorLog || _DEBUG

extern CErrorLog Err;			//	��������o�͂���

#endif