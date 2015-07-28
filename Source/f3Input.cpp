// Cf3Input
// インプット用のクラス
// 継承してキーボード用・マウス用・ジョイスティック用を用意する(つもり)

#include "stdafx.h"
#include "f3Input.h"

Cf3Input f3Input;

const int Cf3Input::m_DefaultButton[F3KEY_BUFSIZE] = {
	VK_ESCAPE,	// F3KEY_EXIT
	VK_RETURN,	// F3KEY_PAUSE
	VK_UP,		// F3KEY_UP
	VK_LEFT,	// F3KEY_LEFT
	VK_RIGHT,	// F3KEY_RIGHT
	VK_DOWN,	// F3KEY_DOWN
	'Z',		// F3KEY_JUMP
	'X',		// F3KEY_ATTACK
	0,			// F3KEY_SMILE
	'F',		// F3KEY_FPS
	'1',		// F3KEY_BGMNONE
	'2',		// F3KEY_BGMDEF
	'3',		// F3KEY_BGMUSER
	'C',		// F3KEY_CAPTURE
	0,			// F3KEY_RECORD
};

const LPSTR Cf3Input::m_KeyName[256] = {
	// 00
	"なし",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	"BackSpace",	"Tab",			"?",			"?",
	"Clear",		"Enter",		"?",			"?",
	// 10
	"Shift",		"Control",		"Alt",			"Pause",
	"Caps Lock",	"?",			"?",			"?",
	"?",			"?",			"?",			"Escape",
	"?",			"?",			"?",			"?",
	// 20
	"Space",		"?",			"?",			"End",
	"Home",			"←",			"↑",			"→",
	"↓",			"?",			"?",			"?",
	"?",			"Insert",		"Delete",		"?",
	// 30
	"0",			"1",			"2",			"3",
	"4",			"5",			"6",			"7",
	"8",			"9",			"?",			"?",
	"?",			"?",			"?",			"?",
	// 40
	"?",			"A",			"B",			"C",
	"D",			"E",			"F",			"G",
	"H",			"I",			"J",			"K",
	"L",			"M",			"N",			"O",
	// 50
	"P",			"Q",			"R",			"S",
	"T",			"U",			"V",			"W",
	"X",			"Y",			"Z",			"Windows(左)",
	"Windows(右)",	"Menu",			"?",			"?",
	// 60
	"0(テンキー)",	"1(テンキー)",	"2(テンキー)",	"3(テンキー)",
	"4(テンキー)",	"5(テンキー)",	"6(テンキー)",	"7(テンキー)",
	"8(テンキー)",	"9(テンキー)",	"*",			"+",
	"?",			"-",			".",			"/",
	// 70
	"F1",			"F2",			"F3",			"F4",
	"F5",			"F6",			"F7",			"F8",
	"F9",			"F10",			"F11",			"F12",
	"F13",			"F14",			"F15",			"F16",
	// 80
	"F17",			"F18",			"F19",			"F20",
	"F21",			"F22",			"F23",			"F24",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	// 90
	"NumLock",		"Scroll",		"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	// A0
	"Shift(左)",	"Shift(右)",	"Control(左)",	"Control(右)",
	"Menu(左)",		"Menu(右)",		"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	// B0
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	// C0
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	// D0
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	// E0
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	// F0
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
	"?",			"?",			"?",			"?",
};

LRESULT Cf3Input::SetButtonState(int button, bool state){
	if (button >= F3KEY_BUFSIZE) return 1;	// そんなボタンないしぃ～
	m_ButtonState[button].Pushed = !m_ButtonState[button].Pressed && state;
	m_ButtonState[button].Released = m_ButtonState[button].Pressed && !state;
	m_ButtonState[button].Pressed = state;
	return 0;
}

void Cf3Input::Input()
{
	for (int i=0; i<F3KEY_BUFSIZE; i++) {
		SetButtonState(i, GetAsyncKeyState(m_ButtonState[i].Button)!=0);
	}
}

void Cf3Input::Init(int *key)
{
	for (int i=0; i<F3KEY_BUFSIZE; i++) {
		m_ButtonState[i].Button = key[i]?key[i]:m_DefaultButton[i];
	}
}
