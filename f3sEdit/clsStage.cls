VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "clsStage"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Dim Title As String

Dim MapData0() As Byte
Dim Width0 As Byte
Dim Height0 As Byte
Dim MapData1() As Byte
Dim Width1 As Byte
Dim Height1 As Byte
Dim MapData2() As Byte
Dim Width2 As Byte
Dim Height2 As Byte

Dim MapData(2) As New clsMapData

Dim CharaX As Integer, CharaY As Integer

Dim MapEdit As FormMap

Private Sub Class_Initialize()
    Set MapEdit = Nothing
    MapData(1).CharaExist = True
End Sub

Private Sub Class_Terminate()
    HideMapEdit
End Sub

Public Sub ShowMapEdit()
    If MapEdit Is Nothing Then
        Set MapEdit = New FormMap
        MapEdit.SetStageData Me
        MapEdit.Show
    Else
        MapEdit.ZOrder
    End If
End Sub

Public Sub PlainMap()
    Title = ""
    ResizeMap 0, 10, 7
    ResizeMap 1, 10, 7
    ResizeMap 2, 10, 7
End Sub

Public Function GetTitle() As String
    GetTitle = Title
End Function

Public Function GetMapChip(ByVal Level As Integer, ByVal X As Integer, ByVal Y As Integer) As Byte
    If 0 <= Level And Level <= 2 Then GetMapChip = MapData(Level).GetMapChip(X, Y)
    Exit Function
    If Level = 1 Then
        If 0 <= X And X < Width1 And 0 <= Y And Y < Height1 Then
            GetMapChip = MapData1(X + Y * Width1)
        End If
    End If
End Function

Public Function SetMapChip(ByVal Level As Integer, ByVal X As Integer, ByVal Y As Integer, ByVal Chip As Byte) As Boolean
    SetMapChip = False
    If 0 <= Level And Level <= 2 Then
        SetMapChip = MapData(Level).SetMapChip(X, Y, Chip)
    End If
    Exit Function
    If Level = 1 Then
        If 0 <= X And X < Width1 And 0 <= Y And Y < Height1 Then
            If MapData1(X + Y * Width1) <> Chip And MapData1(X + Y * Width1) <> &HF0 Then
                If Chip = &HF0 Then
                    If Y = Height1 - 1 Then Exit Function
                    MapData1(CharaX + CharaY * Width1) = 0
                    CharaX = X: CharaY = Y
                End If
                MapData1(X + Y * Width1) = Chip
                SetMapChip = True
            End If
        End If
    End If
End Function

Public Function GetWidth(Level As Integer)
    If 0 <= Level And Level <= 2 Then GetWidth = MapData(Level).GetWidth()
    Exit Function
    If Level = 1 Then
        GetWidth = Width1
    End If
End Function

Public Function GetHeight(Level As Integer)
    If 0 <= Level And Level <= 2 Then GetHeight = MapData(Level).GetHeight()
    Exit Function
    If Level = 1 Then
        GetHeight = Height1
    End If
End Function

Public Sub HideMapEdit()
    If Not MapEdit Is Nothing Then
        Unload MapEdit
        Set MapEdit = Nothing
    End If
End Sub

Public Sub SetTitle(t As String)
    Title = t
    If Not MapEdit Is Nothing Then
        MapEdit.Caption = Title
    End If
    FormStageList.RefreshStage
End Sub

Public Sub Save(ByVal N As Integer, ByVal I As Long)
    Dim d() As Byte, h() As Byte
    'ステージタイトル
    If Title <> "" Then
        h = StrConv("TL" & Right$("00" & Hex$(I), 2), vbFromUnicode)
        d = StrConv(Title, vbFromUnicode)
        Put N, , CLng(UBound(d) + 1)
        Put N, , GetChunkType("TL", I)
        Put N, , d
    End If
    'マップデータ
    If Stages.GetMapChipFile(0) <> "" Then MapData(0).Save N, GetChunkType("M0", I)
    MapData(1).Save N, GetChunkType("M1", I)
    If Stages.GetMapChipFile(2) <> "" Then MapData(2).Save N, GetChunkType("M2", I)
End Sub

Public Sub ResizeMap(ByVal L As Integer, ByVal W As Byte, ByVal h As Byte, Optional a As Boolean = False)
    If 0 <= L And L <= 2 Then MapData(L).ResizeMap W, h, a
    Exit Sub
    Dim OldWidth As Byte, OldHeight As Byte
    OldWidth = Width1
    OldHeight = Height1
    Width1 = W
    Height1 = h
    If a Then
        Dim OldMapData() As Byte
        ReDim OldMapData(CLng(OldWidth) * CLng(OldHeight) - 1)
        OldMapData = MapData1
        ReDim MapData1(CLng(Width1) * CLng(Height1) - 1)
        Dim I As Long, J As Long, k As Long
        For J = 0 To OldHeight - 1
            For I = 0 To OldWidth - 1
                SetMapChip 1, CByte(I), CByte(J), OldMapData(k)
                k = k + 1
            Next
        Next
        If CharaX >= Width1 Then CharaX = Width1 - 1
        If CharaY >= Height1 - 1 Then CharaY = Height1 - 2
        SetMapChip 1, CharaX, CharaY, &HF0
    Else
        ReDim MapData1(CLng(Width1) * CLng(Height1) - 1)
        SetMapChip 1, 0, 0, &HF0
    End If
End Sub

Public Sub Load(aIn() As Byte, I As Long, Size As Long)
    Title = GetDataString(aIn, GetChunkType("TL", I), Size)
    MapData(0).Load aIn, Size, GetChunkType("M0", I)
    MapData(1).Load aIn, Size, GetChunkType("M1", I)
    MapData(2).Load aIn, Size, GetChunkType("M2", I)
    Exit Sub
    Dim d() As Byte
    Dim Addr As Long
    Dim DataSize As Long
    Addr = GetStageData(aIn(0), Size, GetChunkType("M1", I), DataSize)
    If Addr Then
        RtlMoveMemory Width1, ByVal Addr, 1
        RtlMoveMemory Height1, ByVal Addr + 1, 1
        ReDim MapData1(1& * Width1 * Height1 - 1)
        RtlMoveMemory MapData1(0), ByVal Addr + 2, 1& * Width1 * Height1
        Dim X As Long, Y As Long, J As Long
        CharaX = -1: CharaY = -1
        For Y = 0 To Height1 - 1
            For X = 0 To Width1 - 1
                If MapData1(J) = &HF0 Then
                    CharaX = X: CharaY = Y
                    GoTo 2
                End If
                J = J + 1
            Next
        Next
2:
    Else
        Width1 = 10: Height1 = 7
        ReDim MapData1(Width1 * Height1 - 1)
        SetMapChip 1, 0, 0, &HF0
    End If
End Sub

Public Function GetChunkType(Prefix As String, I As Long) As Long
    Dim h() As Byte
    h = LeftB(StrConv(Prefix & "  ", vbFromUnicode), 2) & StrConv(Right$("00" & Hex$(I), 2), vbFromUnicode)
    GetChunkType = ((h(3) * 256& + h(2)) * 256& + h(1)) * 256& + h(0)
End Function

Public Function GetMagnificationX(Level As Integer) As Single
    GetMagnificationX = 1
    If Not (Level = 0 Or Level = 2) Then Exit Function
    If MapData(1).GetWidth() <= 10 Then Exit Function
    GetMagnificationX = (MapData(Level).GetWidth() - 10) / (MapData(1).GetWidth() - 10)
End Function

Public Function GetMagnificationY(Level As Integer) As Single
    GetMagnificationY = 1
    If Not (Level = 0 Or Level = 2) Then Exit Function
    If MapData(1).GetHeight() <= 7 Then Exit Function
    GetMagnificationY = (MapData(Level).GetHeight() - 7) / (MapData(1).GetHeight() - 7)
End Function

Public Sub ShiftMove(ByVal Level As Integer, ByVal X As Long, ByVal Y As Long)
    If Not (0 <= Level And Level <= 2) Then Exit Sub
    MapData(Level).ShiftMove X, Y
End Sub
