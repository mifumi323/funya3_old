// f3MapObjectWind.h: Cf3MapObjectWind クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTWIND_H__1C01A58C_3E34_4050_9076_76112BB2393A__INCLUDED_)
#define AFX_F3MAPOBJECTWIND_H__1C01A58C_3E34_4050_9076_76112BB2393A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3MapObjectBase.h"

class Cf3MapObjectWind : public Cf3MapObjectBase  
{
protected:
	static set<Cf3MapObjectWind*> m_WindList;

	struct tagWindParticle {
		float x;
		int y;
		float dx;
		int color;
	} *m_Particle;
	int m_ParticleCount;

	float m_Left, m_Right;
	float m_Strength;

	static CDIB32 m_Graphic[2];
	static bool m_bGraphicInitialize;

public:
	void OnPreDraw();
	void OnDraw(CDIB32 *lp);
	static void OnPreDrawAll();
	static void OnDrawAll(CDIB32 *lp);
	Cf3MapObjectWind(int x, int y, int w, float s);
	virtual ~Cf3MapObjectWind();

};

#endif // !defined(AFX_F3MAPOBJECTWIND_H__1C01A58C_3E34_4050_9076_76112BB2393A__INCLUDED_)
