#ifndef __yaneStringMap_h__
#define __yaneStringMap_h__

#include "yaneSerialize.h"

//	string to string�̘A�z�L���^�W��
class CStringMap : public CArchive {
public:

	//	�L�[�ɑ΂���f�[�^�̏�������
	void	Write(string szKey,string szData);
	void	Write(string szKey,LONGLONG nData);

	//	�L�[�ɑ΂���f�[�^�̒ǉ���������
	void	Append(string szKey,string szData);
	void	Append(string szKey,LONGLONG nData);

	//	�L�[�ɑ΂���f�[�^�̓ǂݏo��
	LRESULT	Read(string szKey,string& szData);
	LRESULT	Read(string szKey,LONGLONG& nData);

	//	�L�[�ɑ΂���f�[�^�̒��ڎ擾
	//	(���̃L�[�����݂��Ȃ��Ƃ��́AszDefault or lDefault���Ԃ�)
	string	GetStr(string szKey,string szDefault="");
	int		GetNum(string szKey,int nDefault=0);
	//	�������LONGLONG�̂������̂����A����́A�Ƃ��ǂ�
	//		VC++��release�o�[�W�������Ɠ����R���p�C���G���[��
	//		�Ȃ�Ƃ�������B�����炭�ALONGLONG�̍œK���̃o�O�B
	LONGLONG	GetLongNum(string szKey,LONGLONG nDefault=0);

	//	�L�[�ɑ΂���f�[�^�̍폜
	bool	Delete(string szKey);	//	�폜�����Ƃ��ɂ�true

	//	�܂邲�ƍ폜
	void	Clear();	//	clear all data..

	//	��`�t�@�C������f�[�^��ǂݏo���āA�i�[����@�\
	//	(�V���A���C�Y�Ƃ͈Ⴂ�A���̂悤�ȃt�H�[�}�b�g�̃t�@�C������ǂݏo��)
	/*
		// �_�u���X���b�V���s�̓R�����g
		#string1 string2 // ���̂悤��#�ŊJ�n���āA������P�C�Q���Ȃ�ׂ�
		//	���̂Ƃ��A�X�y�[�X���Z�p���[�^�B
		��������ƁAstring1���AszKey,string2��szData�Ƃ��āA�ۑ�(Write)�����B
		����āA��`�t�@�C������ǂݍ��݌�AGetStr("string1");�̂悤�ɂ����
		string2���擾�ł���ƌ����킯�I��(���ށ�)o
	*/
	LRESULT ReadFromConfigFile(string szFileName);

	//	�f�[�^�܂邲�ƕ\��(�f�o�b�O�p)
	void	Out(){
		map<string,string>::iterator it;
		printf ("StringMap::Out() \n");
		for(it = GetMap()->begin();it!=GetMap()->end();it++){
			printf("%s => %s\n",(static_cast<string>(it->first).c_str())
							   ,(static_cast<string>(it->second).c_str()));
		}
	}

	//	����������̘A�z�L���Œu������
	void	Replace(string& s,bool bCaseSensitive=false);

	map<string,string>* GetMap() { return& m_vStringMap; }

/*	//	����ς��A����Ȃ̗p�ӂ�����A�z��A�N�Z�X����₱�����Ȃ��ł�߁A�A
	//	�A�z�L�[�̒��ڎw��I�y���[�^
	const string operator [] (string szKey);
	string operator [] (string szKey);
*/

protected:
	map<string,string> m_vStringMap;

	//	override from CArchive
	virtual void Serialize(CSerialize&s);
};

#endif