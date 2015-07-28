Attribute VB_Name = "modStageEdit"
Option Explicit

Public Declare Function GetStageData Lib "f3sEditDLL.dll" (ByRef lpData As Byte, ByVal dwDataSize As Long, ByVal dwType As Long, ByRef dwSize As Long) As Long
Public Declare Function CompileCompress Lib "f3sEditDLL.dll" (ByVal fileIn As String, ByVal fileOut As String) As Long
Public Declare Sub RtlMoveMemory Lib "kernel32" (Destination As Any, Source As Any, ByVal Length As Long)

Public Stages As clsStageFile

Public EditLevel As Integer
Public SelectedChip As Byte

Public Function GetFullFileName(fn As String) As String
    GetFullFileName = Replace(App.Path & "\" & fn, "\\", "\")
End Function

Public Function GetDataString(aIn() As Byte, Key As Long, Size As Long)
    Dim d() As Byte
    Dim Addr As Long
    Dim DataSize As Long
    Addr = GetStageData(aIn(0), Size, Key, DataSize)
    If Addr Then
        ReDim d(DataSize - 1)
        RtlMoveMemory d(0), ByVal Addr, DataSize
        GetDataString = StrConv(d, vbUnicode)
    Else
        GetDataString = ""
    End If
End Function

Public Sub CreateMask(Src As PictureBox, Mask As PictureBox)
    With Mask
        .Cls
        .Picture = Src.Picture
        BitBlt .hdc, 0, 0, .ScaleWidth, .ScaleHeight, Src.hdc, 0, 0, SRCINVERT
    End With
End Sub

Public Sub GIFBlt(DestDC As Long, DestX As Long, DestY As Long, DestW As Long, DestH As Long, SrcDC As Long, SrcX As Long, SrcY As Long, MaskDC As Long)
    BitBlt DestDC, DestX, DestY, DestW, DestH, MaskDC, SrcX, SrcY, SRCAND
    BitBlt DestDC, DestX, DestY, DestW, DestH, SrcDC, SrcX, SrcY, SRCPAINT
End Sub

Public Sub CloseAllWindow()
    Dim N
    For Each N In Forms
        If N.Name <> "MDIFormMain" Then Unload N
    Next
End Sub

Public Function GetGraphicFile(Default As String) As String
    Load FormGraphic
    FormGraphic.File = Default
    FormGraphic.Show vbModal
    GetGraphicFile = FormGraphic.File
    Unload FormGraphic
End Function

Public Sub TestPlay(ByVal num As Integer)
    If num < 0 Or Stages.GetStageCount <= num Then Exit Sub
    If MsgBox("ステージをセーブした後テストプレイを始めます。", vbOKCancel) = vbCancel Then Exit Sub
    Stages.Save
    If Stages.GetFileName = "" Then Exit Sub
    Shell "funya3.exe TEST """ & Stages.GetFileName & """ " & num, vbNormalFocus
End Sub
