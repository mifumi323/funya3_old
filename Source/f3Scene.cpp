// Cf3Scene
// 自前シーンクラス
// 継承して使うべし

#include "stdafx.h"
#include "f3Scene.h"

Cf3Scene::Cf3Scene(){

}

Cf3Scene::~Cf3Scene(){

}

void Cf3Scene::Init(CApp *app, int *nextscene, void *status, void *ret){
	m_app = app;
	m_nextscene = nextscene;
	m_status = status;
	m_ret = ret;
}
