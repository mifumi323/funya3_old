#include "stdafx.h"
#include "yaneSerialize.h"
#include "yaneFile.h"
	//	クラスの独立性が低下するから、
	//	こういうの用意したくないんだがな、、、

void	CSerialize::Clear(){
	m_abyData.clear();	//	ストリームのクリア
	m_nDataPos = 0;		//	先頭を指し示す
	m_bStoring = true;	//	格納方向へ
}

//	こんなんでええんかいな．．
CSerialize& CSerialize::operator << (CArchive& vData){
	vData.Serialize(*this);
	return *this;
}

//	--- for storing or unstoring primitive data
CSerialize& CSerialize::operator << (BYTE& byData){
	if (IsStoring()){
	//	保存するんかいな？
		GetData()->push_back(byData);
	} else {
	//	取り出すんかいな？
		byData = (*GetData())[m_nDataPos];
		m_nDataPos++;
	}
	return *this;
}

CSerialize& CSerialize::operator << (int& nData){
	if (IsStoring()){
	//	保存するんかいな？
		//	サイズ不明と仮定したコーディング＾＾；
		int nSize = sizeof(int);
		BYTE* pByte = (BYTE*)&nData;
		for(int i=0;i<nSize;i++){
			GetData()->push_back(pByte[i]);
		}
	} else {
	//	ストリームから取り出すんかいな？
		//	サイズ不明と仮定したコーディング＾＾；
		int nSize = sizeof(int);
		BYTE* pByte = (BYTE*)&nData;
		for(int i=0;i<nSize;i++){
			pByte[i] = (*GetData())[m_nDataPos++];
		}
	}
	return *this;
}

CSerialize& CSerialize::operator << (bool& bData){
	if (IsStoring()){
	//	保存するんかいな？
		//	サイズ不明と仮定したコーディング＾＾；
		int nSize = sizeof(bool);
		BYTE* pByte = (BYTE*)&bData;
		for(int i=0;i<nSize;i++){
			GetData()->push_back(pByte[i]);
		}
	} else {
	//	ストリームから取り出すんかいな？
		//	サイズ不明と仮定したコーディング＾＾；
		int nSize = sizeof(bool);
		BYTE* pByte = (BYTE*)&bData;
		for(int i=0;i<nSize;i++){
			pByte[i] = (*GetData())[m_nDataPos++];
		}
	}
	return *this;
}

CSerialize& CSerialize::operator << (string& szData){
	if (IsStoring()){
	//	保存するんかいな？
		//	サイズ不明と仮定したコーディング＾＾；
		BYTE* pByte = (BYTE*)szData.c_str();
		int nSize = szData.size() + 1;
		//	stringが風に実装されてるとNULLは含まへんねや..
		//	これしかし、stringのエレメントをcharだと仮定しているのは
		//	本当は問題がある..このへんunicode対応のときに修正すべき

		//	NULLのバイトも含める必要がある
		for(int i=0;i<nSize;i++){
			GetData()->push_back(pByte[i]);
		}
	} else {
	//	ストリームから取り出すんかいな？
		//	サイズ不明と仮定したコーディング＾＾；
		int nSize = ::strlen((LPCSTR)&(*GetData())[m_nDataPos]) + 1;
		szData = (LPCSTR)(&(*GetData())[m_nDataPos]);
		m_nDataPos += nSize;
	}
	return *this;
}

CSerialize& CSerialize::operator << (vector<int>& anData){
	int n;
	if (IsStoring()) {
		n = anData.size();
		(*this) << n;	//	サイズもついでに保存しなくては！
	} else {
		(*this) << n;	//	長さを復元
		anData.resize(n);
	}
	for(int i=0;i<n;i++){
		(*this) << anData[i];
	}
	return *this;
}

CSerialize& CSerialize::operator << (vector<bool>& abData){
	int n;// = abData.size();
// 追加
	if (IsStoring()) {
		n = abData.size();
		(*this) << n;	//	サイズもついでに保存しなくては！
	} else {
		(*this) << n;	//	長さを復元
		abData.resize(n);
	}
// 追加終わり
	for(int i=0;i<n;i++){
		(*this) << abData[i];
	}
	return *this;
}

CSerialize& CSerialize::operator << (vector<BYTE>& abyData){
	int n;
	if (IsStoring()) {
		n = abyData.size();
		(*this) << n;	//	サイズもついでに保存しなくては！
	} else {
		(*this) << n;	//	長さを復元
		abyData.resize(n);
	}
	for(int i=0;i<n;i++){
		*this << abyData[i];
	}
	return *this;
}

CSerialize& CSerialize::operator << (vector<string>& szData){
	int n;
	if (IsStoring()) {
		n = szData.size();
		(*this) << n;	//	サイズもついでに保存しなくては！
	} else {
		(*this) << n;	//	長さを復元
		szData.resize(n);
	}
	for(int i=0;i<n;i++){
		*this << szData[i];
	}
	return *this;
}

// 追加
CSerialize& CSerialize::operator << (vector<WORD>& awData){
	int n;
	if (IsStoring()) {
		n = awData.size();
		(*this) << n;	//	サイズもついでに保存しなくては！
	} else {
		(*this) << n;	//	長さを復元
		awData.resize(n);
	}
	for(int i=0;i<n;i++){
		*this << awData[i];
	}
	return *this;
}

CSerialize& CSerialize::operator << (vector<DWORD>& adwData){
	int n;
	if (IsStoring()) {
		n = adwData.size();
		(*this) << n;	//	サイズもついでに保存しなくては！
	} else {
		(*this) << n;	//	長さを復元
		adwData.resize(n);
	}
	for(int i=0;i<n;i++){
		*this << adwData[i];
	}
	return *this;
}
// 追加終わり

//	自分自身も！＾＾；
CSerialize& CSerialize::operator << (CSerialize& vData){
	//	vector<BYTE>なので、何なくシリアライズできる
	*this << *vData.GetData();
	*this << vData.m_bStoring;	//	格納方向
	*this << vData.m_nDataPos;	//	ポジション
	return *this;
}

//	そして、シリアライズ間のコピー！
CSerialize& CSerialize::operator = (CSerialize& vSeri){
	//	vectorのコピーなので、std::vectorにお任せ＾＾；
	m_abyData  = vSeri.m_abyData;
	m_bStoring = vSeri.m_bStoring;
	m_nDataPos = vSeri.m_nDataPos;
//	↑これコピーせんほうがいいような気もするが、、、
//	m_bStoring = true;
//	m_nDataPos = 0;
//	↑リセットしておくのだ．．
	return *this;
}

LRESULT CSerialize::Save(string filename){
	//	ストリームのファイルへの保存
	CFile file;
	return file.Save(filename,GetData());
	//	return CFile().Save(...)のように
	//	テンポラリオブジェクトで書いても良いが、
	//	VC++6.0って、returnのところでこれやったら、
	//	バグってたような気がする＾＾； > サービスパック当ててないと
}

LRESULT CSerialize::Load(string filename){	//	ストリームのファイルからの復元
	//	ストリームからのファイルの読み出し
	CFile file;
	return file.Load(filename,GetData());
}