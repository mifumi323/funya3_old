VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "clsLineParser"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Dim m_ReadData As String
Dim m_Offset As Integer

Public Sub SetLine(s As String)
    m_ReadData = s
    m_Offset = 0
End Sub

Public Function IsMatch(CmpStr As String) As Boolean

    If m_ReadData = "" Or m_Offset >= Len(m_ReadData) Then Exit Function
    Garbage

    Dim I As Integer
    For I = 1 To Len(CmpStr)
        If StrConv(Mid$(m_ReadData, m_Offset + I, 1), vbUpperCase) <> StrConv(Mid$(CmpStr, I, 1), vbUpperCase) Then
            Exit Function
        End If
    Next
    m_Offset = m_Offset + I
    IsMatch = True
End Function


Public Function GetNum(nNo As Long) As Integer
    If m_ReadData = "" Or m_Offset >= Len(m_ReadData) Then
        GetNum = 1
        Exit Function
    End If

    Dim c As String
    Dim nRetNum As Long
    Dim bSigned As Boolean
    Dim I As Integer

    Garbage

    ' 『0～9』か『-』を探す
    ' 最初が『-』か『0～9』以外の時はGetNum失敗
    c = Mid$(m_ReadData, m_Offset + 1, 1)
    If (Not IsNumeric(c)) And (c <> "-") Then
        GetNum = 1
        Exit Function
    End If

    I = 1
    If c = "-" Then
        bSigned = True
        I = I + 1
    End If

    Do
        c = Mid$(m_ReadData, m_Offset + I, 1)
        If IsNumeric(c) Then
            nRetNum = nRetNum * 10 + Val(c) '  文字列を数値にして、桁を上げる
        Else
            Exit Do
        End If
        I = I + 1
    Loop

    If bSigned Then nRetNum = -nRetNum

    nNo = nRetNum                           '  結果を渡す
    m_Offset = m_Offset + I
End Function


Public Function GetStr(s As String) As Integer
    If m_ReadData = "" Or m_Offset >= Len(m_ReadData) Then
        GetStr = 1
        Exit Function
    End If

    Garbage
    
    ' 『"』を探す
    ' 最初が『"』以外の時はGetStr失敗
    If Mid$(m_ReadData, m_Offset + 1, 1) <> """" Then
        GetStr = 2
        Exit Function
    End If

    Dim I As Integer
    I = I + 1

    ' "から"までの文字列を渡す
    ' 終了の『"』を探す
    Dim szStr As String
    Dim k As Integer
    Dim c As String
    k = 2
    Do
        If m_Offset + k > Len(m_ReadData) Then
            GetStr = 3
            Exit Function
        End If
        c = Mid$(m_ReadData, m_Offset + k, 1)

        If c = """" Then
            Exit Do
        Else
            szStr = szStr + c
        End If
        k = k + 1
    Loop

    s = szStr
    m_Offset = m_Offset + k
End Function


Private Sub Garbage()
    ' ゴミ以外が見つかるまでループする
    ' ゴミ指定文字 『 (Space)』『  (Tab)』『,』
    Dim c As String
    Do
        c = Mid$(m_ReadData, m_Offset + 1, 1)
        If c <> " " And c <> vbTab And c <> "," Then Exit Do
        m_Offset = m_Offset + 1
    Loop
End Sub
