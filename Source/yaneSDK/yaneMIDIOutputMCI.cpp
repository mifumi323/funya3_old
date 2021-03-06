#include "stdafx.h"

#ifdef USE_MIDIOutputMCI

#include "yaneMIDIOutputMCI.h"
#include "yaneFile.h"
#include "yaneAppInitializer.h"

#define sign(val) (val>0?1:(val<0?-1:0))	//	符号を返す * yane

////////////////
//	コンストラクタ・デストラクタ
////////////////
CMIDIOutputMCI::CMIDIOutputMCI(void)
{
	m_wDeviceID = -1;	//	本来これはwordなので、(DWORD)-1はありえない
	m_bPaused = 0;
	m_bLoopPlay = false;
	m_bNowPlay = false;
	m_lLength = 0;
	CAppInitializer::Hook(this);		//	フックを開始する
}

CMIDIOutputMCI::~CMIDIOutputMCI()
{
	Close();
	CAppInitializer::Unhook(this);		//	フックを解除する。
}

////////////////
//	オープン
///////////////
LRESULT CMIDIOutputMCI::Open(LPCTSTR pName)
{
	Close();

	//	元のファイルを読み込み
	if(m_File.Read((LPSTR)pName)!=0)	{
		Err.Out("CMIDIOutputMCI::Openで読み込むファイルが無い"+string(pName));
		return 1;
	}

	if (m_File.CreateTemporary()!=0)	{
		Err.Out("CMIDIOutputMCI::OpenでCreateTemporaryに失敗");
		return 2;
	}

	return 0;
}

LRESULT	CMIDIOutputMCI::InnerOpen(void){	//	MCIをオープンする
	if (m_wDeviceID!=-1) return 0;	//	open済み

	//	出力ファイルでＭＩＤＩデバイスオープン
	DWORD	dwOpenFlags = MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT;
	MCI_OPEN_PARMS open;
	ZERO(open);
	open.lpstrDeviceType = (LPTSTR)MCI_DEVTYPE_SEQUENCER;
//	open.lpstrElementName = (LPTSTR)(CFile::MakeFullName(m_File.GetName()).c_str());
	//	↑これではテンポラリオブジェクト消滅しうる＾＾；
	string s;
	s = CFile::MakeFullName(m_File.GetName());
	open.lpstrElementName = (LPTSTR)(s.c_str());

	if (::mciSendCommand(0, MCI_OPEN, dwOpenFlags, (DWORD)&open)) {
		Err.Out("CMIDIOutput::MIDIデバイスがオープンできない。");
		return 1;
	}

	m_wDeviceID = open.wDeviceID;

	// 単位をmsにする '01/04/09 add kaine
	MCI_SET_PARMS set;
	set.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	mciSendCommand(m_wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID)&set);


	return 0;
}

//////////
//	クローズ
//////////
LRESULT CMIDIOutputMCI::Close(void)
{
	m_File.Close();	//	delete temporary file
	if (m_wDeviceID == -1) return 0; // not opened
	if (m_bNowPlay) Stop();	// stopしないとCloseできないよーん

	if (::mciSendCommand(m_wDeviceID, MCI_CLOSE, 0, (DWORD)NULL)) {
		Err.Out("CMIDIOutput::MIDIデバイスがクローズできない。");
		return 1;
	}

	m_wDeviceID = -1;
	m_dwPosition = 0;
	return 0;
}

//////////
//	演奏処理
/////////

LRESULT CMIDIOutputMCI::Play(void)
{
	m_bPaused = 0;
	if (InnerOpen()!=0) return 1;

	Stop();
	//	再生開始
	MCI_PLAY_PARMS play;
	play.dwFrom = 0;
	play.dwCallback = (DWORD)CAppInitializer::GetHWnd();
	if (mciSendCommand(m_wDeviceID, MCI_PLAY, MCI_FROM|MCI_NOTIFY,(DWORD)&play)) {
		return 2;
	}
	m_bNowPlay = true;
	return 0;
}

LRESULT CMIDIOutputMCI::Replay(void)
{
	if (m_bPaused==0) return 0;		//	pauseしてへんて！
	if (--m_bPaused!=0) return 0;	//	参照カウント方式

	if (m_wDeviceID == -1) return 1;	//	オープンしてないよ！
	if (InnerOpen()!=0) return 1;

	//	なぜか再生中なので何もせずに帰る
	if (IsPlay()) return 0;

	//	再生再開
	MCI_PLAY_PARMS play;
	play.dwFrom = m_dwPosition;
	play.dwCallback = (DWORD) (DWORD)CAppInitializer::GetHWnd();
	if (::mciSendCommand(m_wDeviceID, MCI_PLAY, MCI_FROM|MCI_NOTIFY,(DWORD)&play)) {
		return 2;
	}
	m_bPaused = 0;
	m_bNowPlay = true;
	return 0;
}

LRESULT CMIDIOutputMCI::Stop(void)
{
	m_bPaused += sign(m_bPaused);	//	必殺技:p
	if (m_wDeviceID == -1) return 1;	//	not open
	if (!IsPlay()) return 0;			//	すでに停止している

	//	再生中のをpauseしたならば
	m_bPaused = 1;

	//	現在状態読み出して停止
	MCI_STATUS_PARMS status;
	ZERO(status);
	status.dwItem = MCI_STATUS_POSITION;
	if (::mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&status)) {
		Err.Out("CMIDIOutput::MIDIデバイスステータスが読み出せない。");
		m_dwPosition = 0;
		if (::mciSendCommand(m_wDeviceID, MCI_STOP, 0, (DWORD)NULL)) {
			Err.Out("CMIDIOutput::MIDIデバイスが停止できない。");
		}
		return 2;
	}
	m_dwPosition = status.dwReturn;
	if (::mciSendCommand(m_wDeviceID, MCI_STOP, 0, (DWORD)NULL)) {
		Err.Out("CMIDIOutput::MIDIデバイスが停止できない。");
		return 2;
	}
	m_bNowPlay = false;
	return 0;
}


LONG	CMIDIOutputMCI::GetCurrentPos(void){
	if (m_wDeviceID == -1) return -1;

	// 現在の再生ポジションを得る
	MCI_STATUS_PARMS status;
	status.dwItem = MCI_STATUS_POSITION;
	if (mciSendCommand(m_wDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&status)){
		Err.Out("CMIDIOutputMCI::GetCurrentPosでMCI_STATUS(MCI_STATUS_POSITION)に失敗");
		return -1;	// エラー
	}
//	return status.dwReturn * 100; // .1秒単位で取得できる
	return status.dwReturn; // [ms]単位で取得できる
}

LRESULT CMIDIOutputMCI::SetCurrentPos(LONG lPos){	// [ms]
	if ( m_wDeviceID == -1) return -1;

	MCI_SEEK_PARMS seek;
	seek.dwTo = lPos;
	seek.dwCallback = (DWORD) (DWORD)CAppInitializer::GetHWnd();
	if ( mciSendCommand(m_wDeviceID,MCI_SEEK, MCI_TO|MCI_WAIT , (DWORD)&seek)){
		Err.Out("Err SetCurrentPos");
		return -1;
	}

	if ( m_bNowPlay ){
		MCI_PLAY_PARMS play;
		play.dwCallback = (DWORD)CAppInitializer::GetHWnd();
		if (mciSendCommand(m_wDeviceID, MCI_PLAY, MCI_NOTIFY,(DWORD)&play)) {
			return 2;
		}
	}

	return 0;
}

LRESULT CMIDIOutputMCI::SetLoopMode(bool bLoop){
	m_bLoopPlay = bLoop;
	return 0;
}

bool CMIDIOutputMCI::IsPlay(void){
	return m_bNowPlay;
}

LRESULT	CMIDIOutputMCI::LoopPlay(void){
	if (m_bLoopPlay) {
		return Play();
	} else {
		return Stop();
	}
}

// add '01/04/09 kaine
LONG	CMIDIOutputMCI::GetLength(void){
	if (m_wDeviceID == -1) return -1;

	// 長さを得る
	MCI_STATUS_PARMS status;
	status.dwItem = MCI_STATUS_LENGTH;
	if (mciSendCommand(m_wDeviceID,MCI_STATUS,MCI_STATUS_ITEM,(DWORD)&status)){
		Err.Out("CMIDIOutputMCI::GetCurrentPosでMCI_STATUS(MCI_STATUS_LENGTH)に失敗");
		return -1;	// エラー
	}
	return status.dwReturn; // [ms]単位で取得できる
}


//////////////////////////////////////////////////////////////////////////////

// WM_MCINOTIFYの通知を受け取る必要あり
LRESULT CMIDIOutputMCI::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
	switch(uMsg){
	case MM_MCINOTIFY : {
		// このメッセージはCDのんか？
		if (lParam!=(LPARAM)m_wDeviceID) return 0; // 違うやん！

		// Stopで停止させたときは、MCI_NOTIFY_ABORTED
		if (wParam==MCI_NOTIFY_SUCCESSFUL){
			LoopPlay();
			return 1;
		}
	}

	} // end switch

	return 0;
}

#endif