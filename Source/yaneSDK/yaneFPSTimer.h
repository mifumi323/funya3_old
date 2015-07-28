// yaneFPSTimer.h
//
//	���m�����ʂȂ��E�F�C�g���|����
//
//	SetFPS��FPS���Z�b�g���āA���Ƃ́A�`��O��ɂł��AWait()���邾���ŗǂ��B

#ifndef __yaneFPSTimer_h__
#define __yaneFPSTimer_h__

class CFPSTimer {
public:
	void SetFPS(DWORD fps);	 //	 FPS�̐ݒ�i�C�j�V�����C�Y�����˂�j
	DWORD GetFPS(void);		 //	 FPS�̎擾
	DWORD GetRealFPS(void);	 //	 FPS�̎擾�i����l�j
	DWORD GetCPUPower(void); //	 CPU�ғ����̎擾�i����l�j
	DWORD GetSkipFrame(void) const;
	void ResetTimer(void);	 //	 �҂����Ԃ̒[���̃��Z�b�g�i���ʁA�g���K�v�͂Ȃ��j
	void WaitFrame(void);	 //	 �P�t���[�����̎��Ԃ�����܂ő҂�
	bool ToBeSkip(void) const;	 //	 ���̃t���[�����X�L�b�v���ׂ����H

	void ResetElapseTimeCounter(void);	// ����Ԃ̃��Z�b�g
	DWORD GetElapseTimeCounter(void);	// ����Ԃ̎擾
	void ResetCallCounter(void);  // �ďo���J�E���^�̃��Z�b�g
	DWORD GetCallCounter(void);	  // �ďo���J�E���^�̎擾

	CFPSTimer(void);
	~CFPSTimer();

protected:
	DWORD m_dwFPS;		  //  FPS(�f�B�t�H���g��60)
	DWORD m_dwFPSWait;	  // =1000*0x10000/FPS; // 60FPS�Ɋ�Â��E�F�C�g
	DWORD m_dwLastDraw;	  // �O��̕`�掞��
	DWORD m_dwFPSWaitTT;  // �O�񔭐������[�����Ԃ̕ێ��̂���
	DWORD m_dwElapsedTime;	  // �O�񃊃Z�b�g����Ă���Elapse���ꂽ����
	int	  m_nUnuseSleep;	  // Wait�֐�����Sleep�𗘗p���邩
	DWORD m_dwDrawTime[32];	   // FPS����p�̕`�掞�Ԍv�Z�p
	DWORD m_dwElapseTime[32];  // CPU Power����p
	DWORD m_nDrawCount;		   // �i����32��̌ďo�����Ԃ̕��ς���FPS�����߂�j
	DWORD m_dwCallCount;	// WaitFrame���Ăяo���ꂽ��

	bool	m_bFrameSkip;		 //	���̃t���[���̓X�L�b�v����̂��H
	DWORD	m_dwFrameSkipCounter; // �@�t���[���X�L�b�v�J�E���^
	DWORD	m_dwFrameSkipCounterN; // �v�����̃t���[���X�L�b�v�J�E���^
};

#endif