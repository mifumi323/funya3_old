/*
	yaneSDK configulation

	使わないものについてはここでdefineをコメントアウト
	すればコンパイルされない。
*/

#ifndef __yaneConfig_h__
#define __yaneConfig_h__

// -=-=-=-=-=-=-=-=- 使わないクラスの排除 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// --- MIDI出力系  --------------------------------------------------------

#define USE_MIDIOutputMCI	//	CMIDIOutputMCIを使うか
#define USE_MIDIOutputDM	//	CMIDIOutputDMを使うか
	//	↑DirectMusicを使うには、DirectX6以降が必要なので注意！
	//	ただし、使えない環境ならば、USE_MIDIOutputMCIが有効であれば、
	//	自動的にMCIで再生するように切り替わる

// --- ストリーム再生  ----------------------------------------------------

#define USE_StreamSound		//	CStreamSoundを使うか

// --- 描画	 --------------------------------------------------------------

//#define USE_DirectDraw		//	CPlane,CDirectDrawを使うか
#define USE_DIB32			//	CDIB32,CDIBDrawを使うか

#define USE_YGA				//	YGAを使うか

#define USE_DIRECTX

// --- メモリマネージメント -----------------------------------------------

//#define USE_MEMORY_CHECK	//	new,deleteごとにログ記録を残すか？
#define USE_YANE_NEWDELETE	//	new,deleteのカスタマイズバージョン(超高速)
							//	上のUSE_MEMORY_CHECKと併用は不可

// --- CErrorLog出力系 ----------------------------------------------------

//#define USE_ErrorLog		//	CErrorLogクラスを有効にする。
//	これをdefineしなければ、CErrorLogクラスは空のクラスになり、デバッグ用のエラー
//	文字列等はVC++の最適化によって消滅する。

#endif
