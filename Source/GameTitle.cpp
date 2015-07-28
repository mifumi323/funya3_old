// CGameTitle
// ƒ^ƒCƒgƒ‹

#include "stdafx.h"
#include "GameTitle.h"

CGameTitle::CGameTitle(){

}

CGameTitle::~CGameTitle(){

}

void CGameTitle::Run(){
	while (m_app->IsThreadValid()) {
		Sleep(1);
	}
	*m_nextscene = F3CODE_EXIT;
}
