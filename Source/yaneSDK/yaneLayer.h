// yaneLayer.h
//	yaneWinHook����̉���:p
//	
//	'00/08/04	sohei

#ifndef __yaneLayer_h__
#define __yaneLayer_h__

#ifdef USE_DirectDraw
class CDirectDraw;
#endif

#ifdef USE_FastDraw
class CFastDraw;
#endif

#ifdef USE_DIB32
class CDIBDraw;
#endif

#ifdef USE_SAVER
class CSaverDraw;
#endif

class CPlaneBase;

//	CLayer�́A�R���X�g���N�^�Ŏ����o�^�C�f�X�g���N�^�Ŏ����폜
class CLayerBase {
public:
#ifdef USE_DirectDraw
	virtual	void	OnDraw(CDirectDraw*lpDraw);
#endif

#ifdef USE_FastDraw
	virtual	void	OnDraw(CFastDraw*lpDraw);
#endif

#ifdef USE_DIB32
	virtual	void	OnDraw(CDIBDraw*lpDraw);
#endif

#ifdef USE_SAVER
	virtual	void	OnDraw(CSaverDraw*lpDraw);
#endif

	virtual	void	OnDraw(HDC) { }

	void	SetPos(int x,int y)	  { m_nX = x; m_nY = y; }	//	�|�W�V�����w��
	void	GetPos(int& x,int& y) { x = m_nX; y = m_nY; }	//	�|�W�V�����擾
	void	Enable(bool bEnable) { m_bEnable = bEnable; }	//	�`���L���ɂ���i�f�B�t�H���g�ŗL���j
	bool	IsEnable() { return m_bEnable; }

	CLayerBase();
	virtual ~CLayerBase();

protected:
	virtual void	InnerOnDraw(CPlaneBase*) { }
	int		m_nX,m_nY;	//	�I�t�Z�b�g���W
	bool	m_bEnable;
};

class CLayer : public CLayerBase {
public:
	CLayer();
	virtual ~CLayer();
};

class CAfterLayer : public CLayerBase {
public:
	CAfterLayer();
	virtual ~CAfterLayer();
};

class CHDCLayer : public CLayerBase {
public:
	CHDCLayer();
	virtual ~CHDCLayer();
};

class CLayerList {
public:
	void	Add(CLayerBase*);			//	�������g���t�b�N�ɉ�����
	void	Del(CLayerBase*);			//	�������g���t�b�N����O��
	void	Clear();					//	���ׂĂ��N���A����
	bool	IsEmpty();					//	��ł��邩�H

	//	���b�Z�[�W��Dispatcher
#ifdef USE_DirectDraw
	void	OnDraw(CDirectDraw*);
#endif

#ifdef USE_FastDraw
	void	OnDraw(CFastDraw*);
#endif

#ifdef USE_DIB32
	void	OnDraw(CDIBDraw*);
#endif

#ifdef USE_SAVER
	void	OnDraw(CSaverDraw*);
#endif

	void	OnDraw(HDC);

protected:
	list<CLayerBase*>	m_LayerPtrList;	//	�t�b�N���X�g
};

#endif