#include "stdafx.h"
#include "yaneCriticalSection.h"

//	new,delete���ƂɃ��O�L�^���c�����H
#ifdef	USE_MEMORY_CHECK

/*
	new��delete�ŁA���O��f���o���B���̓f���o���ꂽ���O�̂����A
	new����delete���Ă��Ȃ�������˂��~�߂�ɂ́A���Ƃ���Perl��
	���̂悤�ȃv���O�����������Ηǂ��B

	# memcheck.pl
	while (<>) {
		if		(m/new[\s]*([a-f\d]+)[\s]*:[\s]*([\d]+)/){
			$dat{$1} = $2;
		} elsif (m/del[\s]*([a-f\d]+)/){
			delete $dat{$1};
		}
	}

	foreach (keys %dat) {
		print "$_ : $dat{$_}\n";
	}

	//	��L�̃v���O�����́A�W�����͂���e�L�X�g�𓾂āA
	//	�W�����͂ɏo�͂��Ă���̂ŁA���s�́A

	jperl memcheck.pl < debug_memory.txt > result.txt

	�̂悤�ɂ��čs�Ȃ��B

	�����āAnew����delete����Ă��Ȃ��������^�T�C�Y������ł���΁A
	���x�́A�ȉ���operator new�̊֐����ŁA���̃������ł��邩��
	�`�F�b�N����if���������āA���̏����������̂Ƃ���Ƀu���[�N
	�|�C���g���d�|���đ��点�Ă݂�B��������΁A�ǂ��Ŋm�ۂ��ꂽ
	���������������Ă��Ȃ����������킩��B

*/

void* operator new (size_t t){
	void *p = ::malloc(t);

	FILE*fp=fopen("debug_memory.txt","aw+");
	fprintf(fp,"new %.8x :%d\n",p,t);
	fclose(fp);

	return p;
}

void operator delete(void*p){
	if (p==NULL) return ;

	FILE*fp=fopen("debug_memory.txt","aw+");
	fprintf(fp,"del %.8x\n",p);
	fclose(fp);

	::free(p);
}

#endif

///////////////////////////////////////////////////////////////////////////////

#ifdef USE_YANE_NEWDELETE	//	new,delete�̃J�X�^�}�C�Y�o�[�W����(����)

//	��ĂɊm�ۂ��郁����
static	BYTE* g_lpaMemory = 0;		//	��Ċm�ۂ��郁����
static bool g_bInitialize = false;	//	��̃������͏���������Ă��邩�H
const	int g_nBlockSize = 16;		//	1�u���b�N�̃T�C�Y(byte)
const	int g_nBlockNum = 4096;		//	���̐�������C�Ƀu���b�N�m��
//	16*4096 = 64K�@���ꂭ�炢�Ȃ�΋����邾�낤

static int  g_anBlank[g_nBlockNum];				//	�󂫃G���A���L������
static int  g_nMax;

// �N���e�B�J���Z�N�V����
static CCriticalSection* g_pCS = NULL;
// g_pCS��NULL�ɂ���(atexit�֐��Ŏ��s�����)
void _cdecl InvalidateCriticalSectionObject(void)
{
	g_pCS = NULL;
}

void* operator new (size_t t)
{
	static BYTE aanHeap[g_nBlockNum][g_nBlockSize];	//	�q�[�v������
	//	���[�J���Ŋm�ۂ���̂́A�I�����Ɏ����I�ɉ�������悤��

	static CCriticalSection cs;
	if ( !g_bInitialize ) {	//	DoubleChecking
		cs.Enter();
		if ( !g_bInitialize ) {
			g_nMax = g_nBlockNum;				//	���ׂċ�
			for (int i=0;i<g_nBlockNum;++i) {
				g_anBlank[i] = i;
			}
			g_lpaMemory = (BYTE*)aanHeap;	// ������O���[�o���ϐ��ɓ`�d�����Ă���
			g_bInitialize = true;	//	�������I��
			g_pCS = &cs;	//	�O���[�o���ϐ��ɓ`��
			atexit(&InvalidateCriticalSectionObject);	//	���Ȃ��Ƃ��I�����ɂ�SingleThread�Ɖ��肷��
		}
		cs.Leave();
	}

	cs.Enter();
	if ( g_nMax==0 || g_nBlockSize<t ) {
		//	�󂫃G���A����ւ� or �K��T�C�Y�ȏ�̃������m�ۗv��
		cs.Leave();
		return (void *)::malloc(t);
	}

	//	�󂫃u���b�N��Ԃ�
	void* p = (void*)&aanHeap[g_anBlank[--g_nMax]];
	cs.Leave();
	return p;
}

void operator delete(void*p)
{
	//	new����x���Ăяo����Ă��Ȃ��̂�delete���Ăяo����邱�Ƃ͖���
	//	�i�Ɖ��肵�Ă���j
	int n = (BYTE*)p - (BYTE*)g_lpaMemory;

	if (g_pCS!=NULL) g_pCS->Enter();
	//	�����̃q�[�v��̃��������H
	if ((0 <= n) && (n < g_nBlockNum*g_nBlockSize)) {
		n /= g_nBlockSize;
		g_anBlank[g_nMax++] = n;	//	�������̉��
		if (g_pCS!=NULL) g_pCS->Leave();
		return ;
	}

	if (g_pCS!=NULL) g_pCS->Leave();
	::free(p);
}

#endif