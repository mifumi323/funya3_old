Attribute VB_Name = "modCommonDialog"
Option Explicit

Type OPENFILENAME
    lStructSize As Long             '構造体のサイズ
    hwndOwner As Long               'ウインドウのハンドル
    hInstance As Long               'インスタンスハンドル
    lpstrFilter As String           'フィルター
    lpstrCustomFilter As String     'カスタムフィルター
    nMaxCustFilter As Long          'カスタムフィルターのサイズ
    nFilterIndex As Long            'フィルタのインデックス
    lpstrFile As String             'ファイル名のバッファ
    nMaxFile As Long                'ファイル名のバッファのサイズ
    lpstrFileTitle As String        'フルパス用のバッファ
    nMaxFileTitle As Long           'フルパス用のバッファのサイズ
    lpstrInitialDir As String       'ディレクトリを指定
    lpstrTitle As String            'ダイヤログボックスのタイトル
    flags As Long                   '定数（OFN＿××参照）
    nFileOffset As Integer          'フルパスの中のファイル名までのオフセット
    nFileExtension As Integer       '拡張子までのオフセット
    lpstrDefExt As String           'デフォルトの拡張子
    lCustData As Long               'lpfnHookで渡すデータ
    lpfnHook As Long                'フック関数のポインタ
    lpTemplateName As String        'テンプレート名
End Type

Declare Function GetOpenFileName Lib "comdlg32.dll" Alias "GetOpenFileNameA" (lpofn As OPENFILENAME) As Long
Declare Function GetSaveFileName Lib "comdlg32.dll" Alias "GetSaveFileNameA" (lpofn As OPENFILENAME) As Long

Public Const OFN_ALLOWMULTISELECT = &H200       '複数ファイルを選択可能にする
Public Const OFN_CREATEPROMPT = &H2000          '指定のファイル名が存在しない時にメッセージボックスを表示
Public Const OFN_FILEMUSTEXIST = &H1000         '存在しないファイル名は入力不可
Public Const OFN_HIDEREADONLY = &H4             '読み取り専用のチェックボックスを非表示
Public Const OFN_NOCHANGEDIR = &H8              '他のサブディレクトリから選択不可
Public Const OFN_NOREADONLYRETURN = &H8000      '読み込み専用ファイルと書きこみ禁止ディレクトリの選択不可
Public Const OFN_NOVALIDATE = &H100             'ファイル名の有効性をチェックしない
Public Const OFN_OVERWRITEPROMPT = &H2          '既存のファイル名を指定した時にメッセージを出す
Public Const OFN_PATHMUSTEXIST = &H800          '有効なパスだけをうけつける
Public Const OFN_READONLY = &H1                 '読み取り専用のチェックボックスをチェック
Public Const OFN_SHOWHELP = &H10                'ヘルプボタンを表示


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
