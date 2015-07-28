// ResultScene.cpp: CResultScene クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResultScene.h"

#include "App.h"
#include "RandomReplay.h"
#include "ResourceManager.h"

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CResultScene::CResultScene()
{
	m_nLines = m_nScroll = m_nFocus = 0;
	m_NavigationBar = NULL;
	m_TextNavi.dib = NULL;
	m_TextTitle = NULL;
	m_TextResult = NULL;
}

CResultScene::~CResultScene()
{
	DELETE_SAFE(m_NavigationBar);
	DELETE_SAFE(m_TextNavi.dib);
	DELETE_SAFE(m_TextTitle);
	DELETE_SAFE(m_TextResult);
}

void CResultScene::OnInit()
{
	m_TextTitle = new CTextDIB32;
	m_TextTitle->GetFont()->SetSize(32);
	m_TextTitle->GetFont()->SetColor(0xffffff);
	m_TextTitle->GetFont()->SetBackColor(0x303030);
	m_TextTitle->GetFont()->SetText("プレイ記録");
	m_TextTitle->UpdateText();
	m_TextResult = new CTextDIB32;
	m_TextResult->GetFont()->SetSize(16);
	m_TextResult->GetFont()->SetColor(0xffffff);
	m_TextResult->GetFont()->SetBackColor(0x303030);
	m_TextResult->GetFont()->SetWeight(FW_BOLD);
	m_TextResult->GetFont()->SetHeight(24);
	// ナビゲーションバー
	m_NavigationBar = new Cf3NavigationBar;
	m_TextNavi.Set(new CTextDIB32);
	m_NavigationBar->Add(&m_TextNavi);
	// プレイ記録
	Add("バナナ取得総数：%d個",
		(string)"今までに取ったバナナの数です。"+
		(theSetting->m_Character==2?
		"　…なに？ブドウだって！？":"コンティニューなどで減ることがあります。"),
		theSetting->m_Banana);
	Add("累計起動時間　："+SecToStr(theSetting->m_PlayTime),
		"前回のゲーム終了までの起動時間の累計です。今回の起動時間は次回起動時に反映されます。");
	Add("累計睡眠時間　："+SecToStr(theSetting->m_SleepTime/40),
		"ふにゃはしばらく放っておくと寝ます。おやすみなさい");
	Add("完全クリア回数：%d回",
		"ステージ集を最後までクリアした回数です。小さいステージだと楽に増えます。",
		theSetting->m_Smiles);
	if (theSetting->m_Smiles)
		Add("Secret1:満面の笑顔で",
			"キー設定→にっこり　どのステージでもいいから最終面までクリアする(他の全てのシークレットも完全クリア時に解禁されます)");
	if (theSetting->m_TimeMaster)
		Add("Secret2:時間を司る者",
			"全般→ゲームテンポ　標準ステージをクリア");
	if (theSetting->m_Eyewitness)
		Add("Secret3:目撃者",
			"全般→温度感知　バナナ1000個以上");
	if (theSetting->m_FeatherIron)
		Add("Secret4:羽のように鉄のように",
			"全般→重力　ノーミスクリア");
	if (theSetting->m_GrapeColored)
		Add("Secret5:ブドウ色のあの子",
			"全般→キャラクター：あんこ　10分以上眠る");
	if (theSetting->m_Esrever)
		Add("Secret6:いたんは",
			"全般→画面表示　10時間以上起動する");
	if (theSetting->m_Outline)
		Add("Secret7:輪郭だけのあいつ",
			"全般→キャラクター：ミスターフレーム　100回以上完全クリア");
	if (theSetting->m_ColdMan)
		Add("Secret8:冷たい人！",
			"全般→氷の息無制限　バナナ10000個以上");
	if (theSetting->m_AndBalloon)
		Add("Secret9:そして風船のように",
			"全般→重力：なし　起動時間100時間以上");
//	if (theSetting->m_)
//		Add("Secret:",
//			"　");
	m_itBegin = m_Lines.begin();
	m_nLines = m_Lines.size();
	Update();
	// リプレイ関係
	f3Replay.Enter();
	// その他
	m_Icon = ResourceManager.Get(RID_MAIN);
}

void CResultScene::OnDraw(CDIB32 *lp)
{
	lp->Clear(0);
	if (f3Input.GetKeyPushed(F3KEY_PAUSE)) {
		f3Input.Input();
		JumpScene(FIRST_SCENE);
	}ef (f3Input.GetKeyPushed(F3KEY_UP)) {
		if (m_nFocus<=0) {
			m_nFocus = m_nLines;
		}
		m_nFocus--;
		Update();
	}ef (f3Input.GetKeyPushed(F3KEY_DOWN)) {
		m_nFocus++;
		if (m_nFocus>=m_nLines) {
			m_nFocus = 0;
		}
		Update();
	}
	f3Replay.Replay(lp);
	if (!f3Input.GetKeyPressed(F3KEY_SMILE)) {
		lp->Blt(m_TextTitle,80,32);
		RECT rc = { 320, 96, 352, 128 };
		lp->Blt(m_Icon,48,32,&rc);
		lp->Blt(m_TextResult,32,80);
		SetRect(&rc, 352+16*(m_nIcon%4), 96+16*(m_nIcon/4), 368+16*(m_nIcon%4), 112+16*(m_nIcon/4));
		lp->Blt(m_Icon,16,80+24*(m_nFocus-m_nScroll),&rc);
	}
	m_NavigationBar->OnDraw(lp);
}

void __cdecl CResultScene::Add(string summary, string description, ...)
{
	char buf[100];
	wvsprintf(buf, summary.c_str(), (LPSTR)(&description+1));
	m_Lines.push_back(strstrpair(buf, description));
}

void CResultScene::Update()
{
	m_nIcon = CApp::random(8);
	string str="";
	if (m_nLines>6) {
		if (m_nScroll>=m_nFocus) m_nScroll = m_nFocus-1;
		if (m_nScroll+5<=m_nFocus) m_nScroll = m_nFocus-4;
		Saturate(0, m_nScroll, m_nLines-6);
	}
	int i=0;
	for (strstriterator it=m_Lines.begin(); it!=m_Lines.end()&&i<m_nLines&&i<m_nScroll+6; it++, i++) {
		if (i>=m_nScroll) str += (*it).first + "\n";
		if (i==m_nFocus) {
			m_TextNavi.text->GetFont()->SetText((*it).second);
			m_TextNavi.Update();
		}
	}
	m_TextResult->GetFont()->SetText(str);
	m_TextResult->UpdateText();
}

string CResultScene::SecToStr(int sec)
{
	if (sec<=0) return "-";
	string ret = "";
	char buf[32];
    int h, m, s;
    h = sec / 3600;
    m = (sec % 3600) / 60;
    s = sec % 60;
    if (h) ret = ret + itoa(h, buf, 10) + "時間";
    if (m) ret = ret + itoa(m, buf, 10) + "分";
    if (s) ret = ret + itoa(s, buf, 10) + "秒";
	return ret;
}
