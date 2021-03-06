#include "stdafx.h"
#include "yaneStringMap.h"
#include "yaneStringScanner.h"
#include "yaneFile.h"

void	CStringMap::Serialize(CSerialize&s){
	//	m_vStringMapをまるごとserialize
	int n;
	if (s.IsStoring()){
		n = GetMap()->size();
		s << n; // サイズを突っ込むなりよー＾＾

		map<string,string>::iterator it;
		for(it = GetMap()->begin();it!=GetMap()->end();it++){
			s << static_cast<string>(it->first);
			s << static_cast<string>(it->second);
		}

	} else {
		s << n;		//	長さを復元するなりよ
		GetMap()->clear();	//	いったんクリアするなりよ

		for(int i = 0; i<n ; i++){
			string s1,s2;
			s << s1;
			s << s2;
			GetMap()->insert(pair<string,string>(s1,s2));
		}
	}
}


//	キーに対するデータの書き込み
void	CStringMap::Write(string szKey,string szData){
	map<string,string>::iterator it;
	it = GetMap()->find(szKey);
	if (it==GetMap()->end()){
		//	そのkeyは存在しない
		//	ほな、挿入すっかー
		GetMap()->insert(pair<string,string>(szKey,szData));
	} else {
		//	上書き上書き上書き〜○(≧∇≦)o
		it->second = szData;
	}
}

void	CStringMap::Write(string szKey,LONGLONG nData){
	string s;
	CStringScanner::NumToString(nData,s);
	Write(szKey,s);
}

//	キーに対するデータの追加書き込み
void	CStringMap::Append(string szKey,string szData){
	map<string,string>::iterator it;
	it = GetMap()->find(szKey);
	if (it==GetMap()->end()){
		//	そのkeyは存在しない
		//	ほな、挿入すっかー
		GetMap()->insert(pair<string,string>(szKey,szData));
	} else {
		//	追加追加追加〜○(≧∇≦)o　夏はスイカ〜○(≧∇≦)o
		it->second += szData;
		//	stringの追加なんで、これでええんでしょ？
	}
}

void	CStringMap::Append(string szKey,LONGLONG nData){
	string s;
	CStringScanner::NumToString(nData,s);
	Write(szKey,s);
}

//	キーに対するデータの読み出し
LRESULT	CStringMap::Read(string szKey,string& szData){
	map<string,string>::iterator it;
	it = GetMap()->find(szKey);
	if (it==GetMap()->end()){
		//	そのkeyは存在しない
		szData.erase();
		return -1;
	}
	szData = it->second;
	return 0;
}

LRESULT	CStringMap::Read(string szKey,LONGLONG& nData){
	string s;
	LRESULT lr = Read(szKey,s);
	if (lr!=0) {
		nData = 0; // fail safe対策
		return lr;
	}
	return CStringScanner::StringToNum(s,nData);
}

//	キーに対するデータの直接取得
	//	(そのキーが存在しないときは、szDefault or lDefaultが返る)
string	CStringMap::GetStr(string szKey,string szDefault){
	string s;
	Read(szKey,s);
	if (s.empty()) {
		return szDefault; // エラー時はszDefaultを返す
	}
	return s;
}

int CStringMap::GetNum(string szKey,int nDefault){
	LONGLONG l;
	if (Read(szKey,l)!=0) return nDefault; // error時はnDefaultを返す
	return l;
}

LONGLONG	CStringMap::GetLongNum(string szKey,LONGLONG nDefault){
	LONGLONG l;
	if (Read(szKey,l)!=0) return nDefault; // error時はnDefaultを返す
	return l;
}

//	キーに対するデータの削除
bool	CStringMap::Delete(string szKey){
	//	削除したときにはtrue
	return GetMap()->erase(szKey)!=0;
}

//	まるごと削除
void	CStringMap::Clear(){
	//	clear all data..
	GetMap()->clear();
}


//	定義ファイルからデータを読み出して、格納する機能
//	(シリアライズとは違い、特殊なフォーマットのファイルから読み出す)
LRESULT	CStringMap::ReadFromConfigFile(string szFileName){
	CFile file;
	LRESULT lr = file.Read(szFileName);
	if (lr!=0) return lr;	//	読み込めて無いじゃん..

	Clear();	//	データのクリア

	string buf;
	string szKey,szData;
	while (true){
		LRESULT lr = file.ReadLine(buf);
		if (lr==1) break;
		if (buf.empty()) continue;
		if (buf.size()>=2 && buf[0] == '/' && buf[1] =='/') continue; // コメント行やん..
		if (buf.size()>=1 && buf[0]=='#') {	//	設定行やん..
			LPSTR psz = (LPSTR)(buf.c_str()+1);
			if (CStringScanner::SkipSpace(psz)!=0) continue;
			szKey = CStringScanner::GetStr(psz);
			if (CStringScanner::SkipSpace(psz)!=0) continue;
			szData = CStringScanner::GetStr(psz);
			if (szData.empty()) continue ; // データあらへんで？
			Write(szKey,szData);
		}
	}
	return 0;
}

//	文字列をこの連想記憶で置換する
void	CStringMap::Replace(string& s,bool bCaseSensitive){
	//	置換しながらの表示
	map<string,string>::iterator it;
	for(it = GetMap()->begin();it!=GetMap()->end();it++){
		CStringScanner::Replace(s,
			static_cast<string>(it->first).c_str(),
			static_cast<string>(it->second).c_str(),
			bCaseSensitive);
	}
}