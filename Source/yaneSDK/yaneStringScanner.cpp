#include "stdafx.h"
#include "yaneStringScanner.h"
#include  <mbctype.h> // _ismbblead

bool	CStringScanner::IsToken(LPCSTR &lp,LPCSTR lp2){
// �}���`�o�C�g���ӎ�����K�v�͂Ȃ�
	if (lp2==NULL) return false;

	LPCSTR lpx = lp;
	//	�����ŁA�X�y�[�X�A�^�u���΂��ėǂ��Ǝv����
	SkipSpace(lpx);
	while (true){
		if (*lp2=='\0') {
			//	���ׂĈ�v�����I
			lp = lpx;	// �|�C���^��i�߂�I
			return true;
		}
		if (toupper(*(lpx++))!=toupper(*(lp2++))) return false;	//	�_�������...
	}
}

LRESULT	CStringScanner::SkipTo(LPCSTR &pSrcStr, LPCSTR pTokenStr){
// �}���`�o�C�g���ӎ�����K�v�����遨�Ή�^^
	LPCSTR pSrcStr_org = pSrcStr;
	bool bKanji = false;
	while (true){
		if (bKanji) {	// �Q�o�C�g�����̂Q�o�C�g�ڂ�
			bKanji = false;
			pSrcStr++;
		}
		if (_ismbblead(*pSrcStr)) {	// �Q�o�C�g�����̂P�o�C�g�ڂ�
			bKanji = true;
		}
		if (*pSrcStr=='\0') { //	EOF�ɏo���킵���I
			pSrcStr = pSrcStr_org;
			return 1;
		}

		//	if (IsToken(lp,lp2)) break;	//	����A�C�����C���W�J�����ق��������Ȃ�...
		//	�x�����Ȃ̂Ł����蓮�ŃC�����C���W�J����O�O�G
		{
			LPCSTR lp1 = pSrcStr;
			LPCSTR lp2 = pTokenStr;
			while (true){
				if (*lp2=='\0') {
					pSrcStr = lp1;	// �|�C���^��i�߂�I
					return 0;	//	��v������ŏI��
				}
				if (*(lp1++)!=*(lp2++)) break;	//	�_�������...
			}
		}
		pSrcStr++;
	}
	return 0;
}

//	���́A��v���Ɉ�v����܂łɃX�L�b�v����������𕶎��o�b�t�@�ɃR�s�[�����
LRESULT	CStringScanner::SkipTo2(LPCSTR &pSrcStr, LPCSTR pTokenStr, LPSTR pSkipedStr){
// �}���`�o�C�g���ӎ�����K�v�����遨�Ή�^^
	LPCSTR pSrcStr_org = pSrcStr;
	bool bKanji = false;
	while (true){
		if (bKanji) {	// �Q�o�C�g�����̂Q�o�C�g�ڂ�
			bKanji = false;
			pSrcStr++;
		}
		if (_ismbblead(*pSrcStr)) {	// �Q�o�C�g�����̂P�o�C�g�ڂ�
			bKanji = true;
		}
		if (*pSrcStr=='\0') { //	EOF�ɏo���킵���I
			pSrcStr = pSrcStr_org;
			return 1;
		}

		//	if (IsToken(lp,lp2)) break;	//	����A�C�����C���W�J�����ق��������Ȃ�...
		//	�x�����Ȃ̂Ł����蓮�ŃC�����C���W�J����O�O�G
		{
			LPCSTR lp1 = pSrcStr;
			LPCSTR lp2 = pTokenStr;
			while (true){
				if (*lp2=='\0') {
					{	//	�����o�b�t�@�ɃR�s�[���Ă����B
						const int nSize = pSrcStr - pSrcStr_org;	// �g�[�N���̑O�܂�
						::CopyMemory(pSkipedStr,pSrcStr_org,nSize);
						pSkipedStr[nSize] = '\0';
					}
					pSrcStr = lp1;	// �|�C���^��i�߂�I
					return 0;	//	��v������ŏI��
				}
				if (*(lp1++)!=*(lp2++)) break;	//	�_�������...
			}
		}
		pSrcStr++;
	}
	return 0;
}

LRESULT CStringScanner::SkipSpace(LPCSTR &lp){
// �}���`�o�C�g���ӎ�����K�v�͂Ȃ�
	//	�X�y�[�X�A�^�u�A���s��؂�l�߂�
	while (*lp==0x0a || *lp==0x0d || *lp==' ' || *lp=='\t'){
		lp++;
	}
	if (*lp=='\0') return 1;
	return 0;
}

LRESULT CStringScanner::SkipSpace2(LPCSTR &lp){
// �}���`�o�C�g���ӎ�����K�v�͂Ȃ�
	//	�X�y�[�X�A�^�u�A�J���}�A���s��؂�l�߂�
	//	csv�ǂݍ��݂Ɏg���ƕ֗�
	while (*lp==0x0a || *lp==0x0d || *lp==' ' || *lp=='\t' || *lp==','){
		lp++;
	}
	if (*lp=='\0') return 1;
	return 0;
}

LRESULT	CStringScanner::GetStrNum(LPCSTR &lp,int& nRate){
// �}���`�o�C�g���ӎ�����K�v�����遨���Ή�--;
	//	"+1"�Ƃ���������Ȃ��nRate�ɂ�1���Ԃ�
	if (SkipSpace(lp)!=0) return -1;	//	�_�����

	if (*lp!='"') return 3;	//	�������[���I
	lp++;
	nRate = 0;
	int nSign = 1;
	if (*lp=='+') { lp++; }
	ef (*lp=='-') { nSign = -1; lp++; }
	LRESULT lr = 1;
	while (*lp>='0' && *lp<='9') {
		lr = 0;
		nRate = nRate*10+(*lp-'0');
		lp++;
	}
	nRate *= nSign;
	if (*lp!='"') return 3; // ���ĂȂ����
	lp++;
	return lr;
}

LRESULT	CStringScanner::GetNum(LPCSTR &lp,int& nVal){
// �}���`�o�C�g���ӎ�����K�v�����遨���Ή�--;
	//	+1�Ƃ���������Ȃ��nVal�ɂ�1���Ԃ�

	if (SkipSpace(lp)!=0) return -1;	//	�_�����

	nVal = 0;
	int nSign = 1;
	if (*lp=='+') { lp++; }
	ef (*lp=='-') { nSign = -1; lp++; }
	LRESULT lr = 1;
	while (*lp>='0' && *lp<='9') {
		lr = 0;
		nVal = nVal*10+(*lp-'0');
		lp++;
	}
	nVal *= nSign;
	return lr;
}

LRESULT CStringScanner::GetStrColor(LPCSTR &lp,COLORREF& nFontColor){
// �}���`�o�C�g���ӎ�����K�v�����遨���Ή�--;
	// "#ff0000"�Ƃ���������Ȃ��RGB(255,0,0)���Ԃ�
	if (SkipSpace(lp)!=0) return -1;

	if (*lp!='"') return 3;	//	�������[���I
	lp++;
	if (*lp!='#') return 3; // #�����T�|�[�g���ĂȂ��̂����O�O�G
	lp++;
	LRESULT lr = 1;
	DWORD dw = 0;
	while ((*lp>='0' && *lp<='9')||(*lp>='a' && *lp<='f')||(*lp>='A' && *lp<='F')){
		lr = 0;
		int c;
		if (*lp>='0' && *lp<='9') {
			c = *lp - '0';
		} else {
			c = toupper(*lp) - 'A'+10;
		}
		dw = (dw << 4) + c;
		lp++;
	}
	if (*lp!='"') return 3; // ���ĂȂ����
	lp++;
	//	RGB�͓]�u���V�n����
	nFontColor = ((dw & 0xff)<<16) + (dw & 0xff00) + ((dw & 0xff0000)>>16);
	return lr;
}

string CStringScanner::GetStr(LPCSTR &lp,CHAR c){
// �}���`�o�C�g���ӎ�����K�v�����遨�Ή�^^
	//	c�ɑ�������Ƃ���܂ł̕������Ԃ�
	string str;
	bool bKanji=false;
	while (true) {
		if (bKanji) {
			str += *(lp++);
			bKanji = false;
			continue;
		}
		if ( *lp==c || *lp=='\0' ) break;
		if (_ismbblead(*lp)) { bKanji = true; }		// MultiByte
		str += *(lp++);
	}
	return str;
}

string CStringScanner::GetStr(LPCSTR &lp){
// �}���`�o�C�g���ӎ�����K�v�����遨�Ή�^^
	//	�X�y�[�X�A�^�u�A���s�ɑ�������Ƃ���܂ł̕������Ԃ�
	string str;
	bool bKanji=false;
	while (true) {
		CHAR c = *lp;
		if (bKanji) {
			str += c;
			bKanji = false;
			lp++;
			continue;
		}
		if (c==' ' || c=='\t' || c=='\0' || c=='\n' || c=='\r') break;
		if (_ismbblead(c)) { bKanji = true; }		// MultiByte
		str += c;
		lp++;
	}
	return str;
}

string CStringScanner::GetStrFileName(LPCSTR &lp){
// �}���`�o�C�g���ӎ�����K�v�����遨���Ή�--;
	//	�^�u�A���s�ɑ�������Ƃ���܂ł̕������Ԃ�
	string str;
	while (true) {
		CHAR c = *lp;
		if (c=='\t' || c=='\0' || c=='\n') break;
		str += c;
		lp++;
	}
	return str;
}

string CStringScanner::GetNextStr(LPCSTR &lp){
// �}���`�o�C�g���ӎ�����K�v�����遨�Ή�^^
	//	�X�y�[�X�A�^�u�A���s�ɑ�������Ƃ���܂ł̕������Ԃ�
	bool bKanji=false;
	string str;
	if (SkipSpace(lp)!=0) return str;	//	�_���ˁ`�O�O�G

	bool bDC = (*lp == '"');	//	�_�u���R�[�e�Ŏn�܂��Ă邩�H
	if (bDC) lp++;

	while (true) {
		CHAR c = *lp;
		if (bKanji) {
			bKanji = false;
			str += c;
			lp++;
			continue;
		}
		if (bDC) {	//	�_�u���R�[�e�X�^�[�g�Ȃ�΃X�y�[�X�C�^�u�͖���
			if (c=='"' ) { lp++;break;};
			if (c=='\0') break;
		} else {
			if (c==' ' || c=='\t' || c=='\0' || c=='\n' || c=='\r') break;
		}
		if (_ismbblead(c)) { bKanji = true; }		// MultiByte
		str += c;
		lp++;
	}
	return str;
}

LRESULT CStringScanner::GetNumFromCsv(LPCSTR &lp,int& nVal,bool b){
// �}���`�o�C�g���ӎ�����K�v�����遨���Ή�--;
	//	�J���}�A�X�y�[�X�𖳎����Ȃ��琔�l��ǂݍ��ނ̂�
	if ( !b ){
		if (SkipSpace2(lp)!=0) return 1;
		if (GetNum(lp,nVal)!=0) return 2;
	}else{
		if ( *lp==',' || *lp== '\t' ) {
			lp++;
			nVal = 0;
		}else{
			LRESULT lr = GetNum(lp,nVal);
			if ( *lp==',' || *lp== '\t' ) {
				lp++;
			}
			if ( lr != 0 ) return 2;
		}
	}
	
	return 0;
}


string CStringScanner::GetStrFromCsv(LPCSTR &lp){
// �}���`�o�C�g���ӎ�����K�v�����遨�Ή�^^
	string str;
	str = GetStr(lp,',');
	SkipTo(lp,",");
	return str;
}


void	CStringScanner::Replace(string& buf,LPCSTR pSrc,LPCSTR pDst,bool bCaseSensitive){
// �}���`�o�C�g���ӎ�����K�v�����遨�Ή�^^
	//	string���Ɋ܂܂��pSrc�ƈ�v���镔����pDst�ɒu������
	//	���{��Ή��BbCaseSensitive==true�̂Ƃ��́A
	//	�A���t�@�x�b�g�̑啶������������ʂ���B

	//	�u�������񂪋�H
	if (buf.empty()) return ;

	//	�u���\�[�X���󔒁H
	if (*pSrc == '\0') return ; // ����͂�������D�D
	
	LPCSTR pSz = buf.c_str();
	LPCSTR pSzOrg = pSz; // �I���W�i���|�W�V����
	while (*pSz!='\0'){
		//	�啶���Ə���������ʂ���Ƃ�
		if ((bCaseSensitive && *pSz == *pSrc) ||
			(!bCaseSensitive && toupper(*pSz)==toupper(*pSrc))){
			//	�擪����v������ŁA������������v���邩���ׂ�ɂ�
			LPCSTR p1 = pSz+1;
			LPCSTR p2 = pSrc+1;
			while(true) {
				if (*p2=='\0'){
				//	��v����������O�O�G
					goto Found;
				}
				//	�啶���Ə���������ʂ���Ƃ�
				if (bCaseSensitive){
					if (*p1!=*p2) break;
					p1++; p2++;
				} else {
				//	�啶���Ə���������ʂ��Ȃ��Ƃ�
					int c = *p1;
					if (toupper(c)!=toupper(*p2)) break;
					if (_ismbblead(c)){
						p1++; p2++;
						if (*p1!=*p2) break;
						//	�����R�[�h�̂Q�o�C�g�ڂɂ�CaseInsensitivity��K�p���Ȃ�
						p1++; p2++;
					} else {
						p1++; p2++;
					}
				}
			}
		}
		//	�}���`�o�C�g�΍�
		if (_ismbblead(*pSz)){
			pSz+=2;
		} else {
			pSz++;
		}
	}
	return ;

Found:;
	vector <int> an;
	an.push_back(pSz-pSzOrg); //	���݈ʒu(0�I���W��)
	int nL1 = ::strlen(pSrc);
	int nL2 = ::strlen(pDst);
	pSz+= nL1; // ��v���������񕪂������Z

	//	����Ɍ����𑱂���
	while (*pSz!=0){
		//	�啶���Ə���������ʂ���Ƃ�
		if ((bCaseSensitive && *pSz == *pSrc) ||
			(!bCaseSensitive && toupper(*pSz)==toupper(*pSrc))){
			//	�擪����v������ŁA������������v���邩���ׂ�ɂ�
			LPCSTR p1 = pSz+1;
			LPCSTR p2 = pSrc+1;
			while(true) {
				if (*p2=='\0'){
				//	��v����������O�O�G
					an.push_back(pSz-pSzOrg); //	���݈ʒu(0�I���W��)
					pSz+= nL1; // ��v���������񕪂������Z
					goto SearchNext;
				}
				//	�啶���Ə���������ʂ���Ƃ�
				if (bCaseSensitive){
					if (*p1!=*p2) break;
					p1++; p2++;
				} else {
				//	�啶���Ə���������ʂ��Ȃ��Ƃ�
					int c = *p1;
					if (toupper(c)!=toupper(*p2)) break;
					if (_ismbblead(c)){
						p1++; p2++;
						if (*p1!=*p2) break;
						//	�����R�[�h�̂Q�o�C�g�ڂɂ�CaseInsensitive��K�p���Ȃ�
						p1++; p2++;
					} else {
						p1++; p2++;
					}
				}
			}
		}
		//	�}���`�o�C�g�΍�
		if (_ismbblead(*pSz)){
			pSz+=2;
		} else {
			pSz++;
		}
SearchNext:;
	}

	//	���ۂ̏k���g����
	int n = pSz-pSzOrg; // ���̕�����
	int m = an.size();
	int nL = n + m*(nL2-nL1) + 1; // �u�����鐔�~�P������̕ψ� + 1(�I�[��NULL)
	LPSTR pTmp = new CHAR[nL];
	//	nL1==nL2�̂Ƃ��́A���̃o�b�t�@�쐬��Ƃ͂͂�����̂����A
	//	string::c_str�́Aconst�ȃo�b�t�@�Ȃ��߁A�ύX�͋�����Ă��Ȃ�
	LPSTR pTmpt = pTmp;
	pSz = pSzOrg;
	int k=0;
	for(int i=0;i<n;i++){
		if ((k<m) && (i==an[k])){ // if �̕]�����͍�����Ɖ��肵�Ă���
			LPCSTR pDstt = pDst;
			while (*pDstt!='\0'){
				*(pTmpt++) = *(pDstt++);
			}
			i += nL1-1;
			pSz += nL1; // �\�[�X�����Z
			k++;
		} else {
			*(pTmpt++) = *(pSz++);
		}
	}
	*pTmpt = '\0'; // �I�[����
	buf = pTmp; // ������̑�����
	delete [] pTmp;
}

void	CStringScanner::NumToString(LONGLONG l,string& s){
// �}���`�o�C�g���ӎ�����K�v�͂Ȃ�
	if (l==0) {
		s = "0";
		return ;
	}
	bool bMinus;
	if (l<0) {
		bMinus = true;
		l = -l;
	} else {
		bMinus = false;
	}
	s.erase();
	while (l!=0){
		s = (CHAR)((l%10) + '0') + s;
		//	���������������ꂽ�R�[�h��������̂���..
		l/=10;
	}
	if (bMinus) {
		s = '-' + s;
	}
}

LRESULT	 CStringScanner::StringToNum(string s,LONGLONG& l){
// �}���`�o�C�g���ӎ�����K�v�͂Ȃ�
	LPCSTR psz = s.c_str();
	bool bMinus;
	if (*psz == '-') {
		psz++;
		bMinus = true;
	} else {
		bMinus = false;
	}
	l=0;
	bool bFirst = true;
	while (true){
		CHAR c = *psz;
		if (c>='0' && c<='9'){
			l = l*10 + c-'0';
			bFirst = false;
		} else {
			if (bFirst){
			//	���l������P�x���s�Ȃ��Ă��Ȃ�
				return 1; // error
			}
			break;
		}
		psz++;
	}
	if (bMinus) l=-l;
	return 0;
}

void CStringScanner::NumToStringZ(LONGLONG l,string& s,int n){
// �}���`�o�C�g���ӎ�����K�v�͂Ȃ�
	NumToString(l,s);
	int nLength = s.length();
	if (nLength < n){
		//	����������Ȃ��̂ŁA���̌��Ƀ[����ǉ��B
		string t;
		for(int i=nLength;i<n;i++){ t+='0'; }
		s = t+s;
	} ef (nLength > n){
		//	��������̂ŁA���̌����J�b�g����i����ȕK�v�����񂩁H�j
		s = s.substr(nLength-n,n);
	}
}