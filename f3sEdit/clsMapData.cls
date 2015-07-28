VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "clsMapData"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Dim MapData() As Byte
Dim Width As Byte
Dim Height As Byte

Public CharaExist As Boolean
Dim CharaX As Integer, CharaY As Integer

Public Sub ResizeMap(ByVal W As Byte, ByVal h As Byte, Optional a As Boolean = False)
    Dim OldWidth As Byte, OldHeight As Byte
    OldWidth = Width
    OldHeight = Height
    Width = W
    Height = h
    If a Then
        Dim OldMapData() As Byte
        ReDim OldMapData(CLng(OldWidth) * CLng(OldHeight) - 1)
        OldMapData = MapData
        ReDim MapData(CLng(Width) * CLng(Height) - 1)
        Dim I As Long, J As Long, k As Long
        For J = 0 To OldHeight - 1
            For I = 0 To OldWidth - 1
                SetMapChip CByte(I), CByte(J), OldMapData(k)
                k = k + 1
            Next
        Next
        If CharaExist Then
            If CharaX >= Width Then CharaX = Width - 1
            If CharaY >= Height - 1 Then CharaY = Height - 2
            SetMapChip CharaX, CharaY, &HF0
        End If
    Else
        ReDim MapData(CLng(Width) * CLng(Height) - 1)
        If CharaExist Then
            MapData(0) = &HF0
            CharaX = 0
            CharaY = 0
        End If
    End If
End Sub

Public Function SetMapChip(ByVal X As Long, ByVal Y As Long, ByVal Chip As Byte) As Boolean
    SetMapChip = False
    If Not (0 <= X And X < Width And 0 <= Y And Y < Height) Then Exit Function
    If MapData(X + Y * Width) = &HF0 Then Exit Function
    If CharaExist = False And Chip >= &HF0 Then Exit Function
    If MapData(X + Y * Width) = Chip Then Exit Function

    If Chip = &HF0 Then
        If Y = Height - 1 Then Exit Function
        MapData(CharaX + CLng(CharaY) * Width) = 0
        CharaX = X: CharaY = Y
    End If
    MapData(X + Y * Width) = Chip
    SetMapChip = True
End Function

Public Function GetMapChip(ByVal X As Long, ByVal Y As Long) As Byte
    If 0 <= X And X < Width And 0 <= Y And Y < Height Then
        GetMapChip = MapData(X + Y * Width)
    End If
End Function

Public Function GetWidth()
    GetWidth = Width
End Function

Public Function GetHeight()
    GetHeight = Height
End Function

Public Sub Save(ByVal N As Integer, ByVal Chunk As Long)
    Put N, , CLng(Width) * CLng(Height) + 2
    Put N, , Chunk
    Put N, , Width
    Put N, , Height
    Put N, , MapData
End Sub

Public Sub Load(aIn() As Byte, ByVal Size As Long, ByVal Chunk As Long)
    Dim d() As Byte
    Dim Addr As Long
    Dim DataSize As Long
    Addr = GetStageData(aIn(0), Size, Chunk, DataSize)
    If Addr Then
        RtlMoveMemory Width, ByVal Addr, 1
        RtlMoveMemory Height, ByVal Addr + 1, 1
        ReDim MapData(1& * Width * Height - 1)
        RtlMoveMemory MapData(0), ByVal Addr + 2, 1& * Width * Height
        If Not CharaExist Then Exit Sub
        Dim X As Long, Y As Long, J As Long
        CharaX = -1: CharaY = -1
        For Y = 0 To Height - 1
            For X = 0 To Width - 1
                If MapData(J) = &HF0 Then
                    CharaX = X: CharaY = Y
                    Exit Sub
                End If
                J = J + 1
            Next
        Next
    Else
        Width = 10: Height = 7
        ReDim MapData(Width * Height - 1)
        If CharaExist Then SetMapChip 0, 0, &HF0
    End If
End Sub

Public Sub ShiftMove(ByVal X As Long, ByVal Y As Long)
    Dim X2 As Long, Y2 As Long
    Dim XX As Long, YY As Long
    Dim Buf As Byte, Buf2 As Byte
    If X > 0 Then
        For YY = 0 To Height - 1
            Buf = GetMapChip(Width - 1, YY)
            Buf2 = GetMapChip(Width - 2, YY)
            For XX = Width - 2 To 0 Step -1
                SetMapChip XX + 1, YY, GetMapChip(XX, YY)
            Next
            SetMapChip 0, YY, Buf
            SetMapChip Width - 1, YY, Buf2
        Next
        X2 = X - 1
    ElseIf X < 0 Then
        For YY = 0 To Height - 1
            Buf = GetMapChip(0, YY)
            Buf2 = GetMapChip(1, YY)
            For XX = 0 To Width - 2
                SetMapChip XX, YY, GetMapChip(XX + 1, YY)
            Next
            SetMapChip Width - 1, YY, Buf
            SetMapChip 0, YY, Buf2
        Next
        X2 = X + 1
    ElseIf Y > 0 Then
        For XX = 0 To Width - 1
            Buf = GetMapChip(XX, Height - 1)
            Buf2 = GetMapChip(XX, Height - 2)
            For YY = Height - 2 To 0 Step -1
                SetMapChip XX, YY + 1, GetMapChip(XX, YY)
            Next
            SetMapChip XX, 0, Buf
            SetMapChip XX, Height - 1, Buf2
        Next
        Y2 = Y - 1
    ElseIf Y < 0 Then
        For XX = 0 To Width - 1
            Buf = GetMapChip(XX, 0)
            Buf2 = GetMapChip(XX, 1)
            For YY = 0 To Height - 2
                SetMapChip XX, YY, GetMapChip(XX, YY + 1)
            Next
            SetMapChip XX, Height - 1, Buf
            SetMapChip XX, 0, Buf2
        Next
        Y2 = Y + 1
    End If
    If X2 Or Y2 Then ShiftMove X2, Y2   'Ä‹A“I‚É
End Sub
