#include "stdafx.h"

#include "yaneFindWindow.h"
#include "yaneFile.h"

LRESULT CFindWindow::Find(LPCSTR szModuleName) {
	if (InitProcessFunction()!=0) return 1; // ���������s
	m_szModuleName = CFile::GetPureFileNameOf(szModuleName);
	return FindSameWindow();
}

LRESULT		CFindWindow::FindSameWindow( void )
{
// �E�B���h�E�̗񋓗v��
	if ( !::EnumWindows( (WNDENUMPROC)CFindWindow::EnumWndProc, (LONG)this ) )
						// �����̃L���X�g�����Ȃ���VC++5�Œʂ�Ȃ��̂��O�O�G�G
		return 0;
	return 1;
}

int	CALLBACK	CFindWindow::EnumWndProc( HWND hWnd, LPARAM lParam ){
	return ((CFindWindow*)lParam) ->InnerEnumWndProc(hWnd);
}

int		CFindWindow::InnerEnumWndProc( HWND hWnd ){

// �C���X�^���X�ɑ΂���t�@�C�����̎擾
	CHAR	szFileName[_MAX_PATH];
	if (GetWindowModuleName( hWnd , szFileName, _MAX_PATH )!=0)
		return 1;

	// �t�@�C���������������ۂ��̔���
	if ( ::lstrcmp(m_szModuleName.c_str(),szFileName) == 0 ) {
		// �ȑO�ɍ쐬�����E�B���h�E���A�N�e�B�u�ɂ���

		::ShowWindow( hWnd ,SW_SHOWNORMAL);
		::SetForegroundWindow( hWnd );

		return 0;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////
//	�v���Z�X�̃X�i�b�v���Ƃ�
//		�������Ȃ��ƁA���m�ȃn���h�����������Ȃ�

LRESULT	CFindWindow::InitProcessFunction(void)
{
	HANDLE hKernel;
	hKernel = ::GetModuleHandle("KERNEL32.DLL");

	if ( hKernel ) {
		m_lpCreateToolhelp32Snapshot = (CREATESNAPSHOT)GetProcAddress(
			(HINSTANCE)hKernel, "CreateToolhelp32Snapshot");
		m_lpProcess32First =
			(PROCESSWALK)::GetProcAddress((HINSTANCE)hKernel, "Process32First");
		m_lpProcess32Next =
			(PROCESSWALK)::GetProcAddress((HINSTANCE)hKernel, "Process32Next");
	}
	if ( m_lpProcess32First && m_lpProcess32Next && m_lpCreateToolhelp32Snapshot )
		return 0;
	return 1;
}

LRESULT	CFindWindow::GetWindowModuleName( HWND hWnd , LPTSTR szFileName, DWORD dwSize )
{
	DWORD pid = 0;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	LRESULT Ret = 1;

	::GetWindowThreadProcessId(hWnd, &pid);
	hProcessSnap = m_lpCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if ( hProcessSnap == (HANDLE)-1 )
		return 1;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if ( m_lpProcess32First(hProcessSnap, &pe32) ) {
		do {
			if ( pid == pe32.th32ProcessID ) {
				::lstrcpyn(szFileName, pe32.szExeFile, dwSize);
				Ret = 0;
				break;
			}
		} while ( m_lpProcess32Next(hProcessSnap, &pe32) );
	}
	::CloseHandle(hProcessSnap);
	return Ret;
}