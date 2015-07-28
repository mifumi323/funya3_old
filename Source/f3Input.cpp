// Cf3Input
// �C���v�b�g�p�̃N���X
// �p�����ăL�[�{�[�h�p�E�}�E�X�p�E�W���C�X�e�B�b�N�p��p�ӂ���(����)

#include "stdafx.h"
#include "f3Input.h"

Cf3Input f3Input;

const LPSTR Cf3Input::m_KeyName[256] = {
	// 00
	"?",			"?",			"?",			"?",
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
	"Home",			"��",			"��",			"��",
	"��",			"?",			"?",			"?",
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
	"X",			"Y",			"Z",			"Windows(��)",
	"Windows(�E)",	"Menu",			"?",			"?",
	// 60
	"0(�e���L�[)",	"1(�e���L�[)",	"2(�e���L�[)",	"3(�e���L�[)",
	"4(�e���L�[)",	"5(�e���L�[)",	"6(�e���L�[)",	"7(�e���L�[)",
	"8(�e���L�[)",	"9(�e���L�[)",	"*",			"+",
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
	"Shift(��)",	"Shift(�E)",	"Control(��)",	"Control(�E)",
	"Menu(��)",		"Menu(�E)",		"?",			"?",
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
	if (button >= F3KEY_BUFSIZE) return 1;	// ����ȃ{�^���Ȃ������`
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
	int def[F3KEY_BUFSIZE] = {0};
	def[F3KEY_EXIT]		= VK_ESCAPE;
	def[F3KEY_PAUSE]	= VK_RETURN;
	def[F3KEY_UP]		= VK_UP;
	def[F3KEY_LEFT]		= VK_LEFT;
	def[F3KEY_RIGHT]	= VK_RIGHT;
	def[F3KEY_DOWN]		= VK_DOWN;
	def[F3KEY_JUMP]		= 'Z';
	def[F3KEY_ATTACK]	= 'X';
	def[F3KEY_SMILE]	= 0;
	def[F3KEY_FPS]		= 'F';
	def[F3KEY_BGMNONE]	= '1';
	def[F3KEY_BGMDEF]	= '2';
	def[F3KEY_BGMUSER]	= '3';
	def[F3KEY_CAPTURE]	= 'C';
	def[F3KEY_RECORD]	= 0;
	for (int i=0; i<F3KEY_BUFSIZE; i++) {
		m_ButtonState[i].Button = key[i]?key[i]:def[i];
	}
}
