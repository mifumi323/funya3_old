Attribute VB_Name = "modCommonDialog"
Option Explicit

Type OPENFILENAME
    lStructSize As Long             '�\���̂̃T�C�Y
    hwndOwner As Long               '�E�C���h�E�̃n���h��
    hInstance As Long               '�C���X�^���X�n���h��
    lpstrFilter As String           '�t�B���^�[
    lpstrCustomFilter As String     '�J�X�^���t�B���^�[
    nMaxCustFilter As Long          '�J�X�^���t�B���^�[�̃T�C�Y
    nFilterIndex As Long            '�t�B���^�̃C���f�b�N�X
    lpstrFile As String             '�t�@�C�����̃o�b�t�@
    nMaxFile As Long                '�t�@�C�����̃o�b�t�@�̃T�C�Y
    lpstrFileTitle As String        '�t���p�X�p�̃o�b�t�@
    nMaxFileTitle As Long           '�t���p�X�p�̃o�b�t�@�̃T�C�Y
    lpstrInitialDir As String       '�f�B���N�g�����w��
    lpstrTitle As String            '�_�C�����O�{�b�N�X�̃^�C�g��
    flags As Long                   '�萔�iOFN�Q�~�~�Q�Ɓj
    nFileOffset As Integer          '�t���p�X�̒��̃t�@�C�����܂ł̃I�t�Z�b�g
    nFileExtension As Integer       '�g���q�܂ł̃I�t�Z�b�g
    lpstrDefExt As String           '�f�t�H���g�̊g���q
    lCustData As Long               'lpfnHook�œn���f�[�^
    lpfnHook As Long                '�t�b�N�֐��̃|�C���^
    lpTemplateName As String        '�e���v���[�g��
End Type

Declare Function GetOpenFileName Lib "comdlg32.dll" Alias "GetOpenFileNameA" (lpofn As OPENFILENAME) As Long
Declare Function GetSaveFileName Lib "comdlg32.dll" Alias "GetSaveFileNameA" (lpofn As OPENFILENAME) As Long

Public Const OFN_ALLOWMULTISELECT = &H200       '�����t�@�C����I���\�ɂ���
Public Const OFN_CREATEPROMPT = &H2000          '�w��̃t�@�C���������݂��Ȃ����Ƀ��b�Z�[�W�{�b�N�X��\��
Public Const OFN_FILEMUSTEXIST = &H1000         '���݂��Ȃ��t�@�C�����͓��͕s��
Public Const OFN_HIDEREADONLY = &H4             '�ǂݎ���p�̃`�F�b�N�{�b�N�X���\��
Public Const OFN_NOCHANGEDIR = &H8              '���̃T�u�f�B���N�g������I��s��
Public Const OFN_NOREADONLYRETURN = &H8000      '�ǂݍ��ݐ�p�t�@�C���Ə������݋֎~�f�B���N�g���̑I��s��
Public Const OFN_NOVALIDATE = &H100             '�t�@�C�����̗L�������`�F�b�N���Ȃ�
Public Const OFN_OVERWRITEPROMPT = &H2          '�����̃t�@�C�������w�肵�����Ƀ��b�Z�[�W���o��
Public Const OFN_PATHMUSTEXIST = &H800          '�L���ȃp�X��������������
Public Const OFN_READONLY = &H1                 '�ǂݎ���p�̃`�F�b�N�{�b�N�X���`�F�b�N
Public Const OFN_SHOWHELP = &H10                '�w���v�{�^����\��


Public Sub SetOPENFILENAME(ofn As OPENFILENAME, _
                            hwndOwner As Long, _
                            lpstrFilter As String, _
                            nMaxFile As Long, _
                            nMaxFileTitle As Long, _
                            lpstrInitialDir As String, _
                            lpstrTitle As String, _
                            flags As Long, _
                            lpstrDefExt As String)
    With ofn
        .hwndOwner = hwndOwner
        .hInstance = App.hInstance
        .lpstrFilter = lpstrFilter
        .lpstrFile = String(nMaxFile, Chr(0))
        .nMaxFile = nMaxFile
        .lpstrFileTitle = String(nMaxFileTitle, Chr(0))
        .nMaxFileTitle = nMaxFileTitle
        .lpstrInitialDir = lpstrInitialDir
        .lpstrTitle = lpstrTitle
        .flags = flags
        .lpstrDefExt = lpstrDefExt
        .lStructSize = Len(ofn)
    End With
End Sub
