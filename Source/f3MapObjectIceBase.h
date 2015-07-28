// f3MapObjectIceBase.h: Cf3MapObjectIceBase クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_F3MAPOBJECTICEBASE_H__229E0C2F_D836_4CA2_B7B1_A101E778BB4C__INCLUDED_)
#define AFX_F3MAPOBJECTICEBASE_H__229E0C2F_D836_4CA2_B7B1_A101E778BB4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "f3MapObjectBase.h"

class Cf3MapObjectIceBase : public Cf3MapObjectBase  
{
protected:
	CDIB32* m_Graphic;
public:
	Cf3MapObjectIceBase(f3MapObjectType eType);
	virtual ~Cf3MapObjectIceBase();

};

#endif // !defined(AFX_F3MAPOBJECTICEBASE_H__229E0C2F_D836_4CA2_B7B1_A101E778BB4C__INCLUDED_)
