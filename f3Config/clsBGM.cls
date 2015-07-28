VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "clsBGM"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Private Type tagBGM
    Text As String
    ID As String
    FileName As String
    LoopMode As Boolean
End Type

Private m_BGM() As tagBGM

Const ListFile As String = "BGM.dat"

Private Sub Add(Text As String, ID As String)
    m_BGM(UBound(m_BGM)).Text = Text
    m_BGM(UBound(m_BGM)).ID = ID
    m_BGM(UBound(m_BGM)).FileName = ""
    m_BGM(UBound(m_BGM)).LoopMode = False
    ReDim Preserve m_BGM(UBound(m_BGM) + 1)
End Sub

Private Sub Class_Initialize()
    ReDim m_BGM(0)
    Add "タイトル", "TITLE"
    Add "普通のステージ", "GAMEFUNYA"
    Add "バナナが多いステージ", "GAMEBANANA"
    Add "とげとげだらけのステージ", "GAMENEEDLE"
    Add "ギヤバネのステージ", "GAMEGEASPRIN"
    Add "風の強いステージ", "GAMEWIND"
    Add "冷たいステージ", "GAMEICE"
    Add "熱いステージ", "GAMEFIRE"
    Add "偽者がいるステージ", "GAMEMRFRAME"
    Add "ウナギカズラが密生するステージ", "GAMEEELPITCHER"
    Add "ステージクリア", "CLEAR"
    Add "エンディング", "ENDING"
    Add "ミス", "MISS"
    Add "ゲームオーバー", "GAMEOVER"
    Add "操作説明", "EXPLAIN"
    LoadFile
End Sub

Private Function TextToIndex(Text As String) As Long
    For TextToIndex = 0 To UBound(m_BGM) - 1
        If m_BGM(TextToIndex).Text = Text Then Exit Function
    Next
End Function

Private Function IDToIndex(ID As String) As Long
    For IDToIndex = 0 To UBound(m_BGM) - 1
        If m_BGM(IDToIndex).ID = ID Then Exit Function
    Next
End Function

Public Sub SetFileNameByText(Text As String, FileName As String)
    m_BGM(TextToIndex(Text)).FileName = FileName
End Sub

Public Sub SetFileNameByID(ID As String, FileName As String)
    m_BGM(IDToIndex(ID)).FileName = FileName
End Sub

Public Sub SetLoopModeByText(Text As String, LoopMode As Boolean)
    m_BGM(TextToIndex(Text)).LoopMode = LoopMode
End Sub

Public Sub SetLoopModeByID(ID As String, LoopMode As Boolean)
    m_BGM(IDToIndex(ID)).LoopMode = LoopMode
End Sub

Public Function GetFileNameByText(Text As String) As String
    GetFileNameByText = m_BGM(TextToIndex(Text)).FileName
End Function

Public Function GetFileNameByID(ID As String) As String
    GetFileNameByID = m_BGM(IDToIndex(ID)).FileName
End Function

Public Function GetLoopModeByText(Text As String) As Boolean
    GetLoopModeByText = m_BGM(TextToIndex(Text)).LoopMode
End Function

Public Function GetLoopModeByID(ID As String) As Boolean
    GetLoopModeByID = m_BGM(IDToIndex(ID)).LoopMode
End Function

Public Function GetText(Num As Long) As String
    GetText = m_BGM(Num).Text
End Function

Public Function GetSize() As Long
    GetSize = UBound(m_BGM)
End Function

Private Sub LoadFile()
    On Error Resume Next
    Dim N As Integer, I As Integer
    Dim Buf As String
    Dim l As New clsLineParser
    N = FreeFile
    Open GetFullFileName(ListFile) For Input As #N
        If Err.Number Then
            Close
            Exit Sub
        End If
        Do Until EOF(N)
            Line Input #N, Buf
            l.SetLine Buf
            For I = 0 To UBound(m_BGM) - 1
                If l.IsMatch(m_BGM(I).ID) Then Exit For
            Next
            l.GetStr m_BGM(I).FileName
            If l.IsMatch("LOOP") Then m_BGM(I).LoopMode = True Else m_BGM(I).LoopMode = False
        Loop
    Close
End Sub

Public Sub SaveFile()
    Dim N As Integer, I As Integer
    N = FreeFile
    Open GetFullFileName(ListFile) For Output As #N
        For I = 0 To UBound(m_BGM) - 1
            Print #N, m_BGM(I).ID & " """ & m_BGM(I).FileName & """" & IIf(m_BGM(I).LoopMode, " LOOP", "")
        Next
    Close
End Sub
