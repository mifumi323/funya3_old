VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "clsStageFile"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Dim FileName As String
Dim TempFile As String

Dim MapChipFile(2) As String
Dim MapChipData(2) As IPictureDisp

Dim StageCount As Byte
Dim Stage() As clsStage

Private Type tagStageInfo
    CHeader As Long
    CName As String
    CDescription As String
    CDefault As Variant
    CMin As Variant
    CMax As Variant
    Value As Variant
End Type

Dim StageInfo() As tagStageInfo

Const SCH_TITLE As Long = &H4C544954
Const SCH_AUTHOR As Long = &H48545541
Const SCH_DESCRIPTION As Long = &H43534544
Const SCH_ENDING As Long = &H4D444E45
Const SCH_REST As Long = &H54534552
Const SCH_PENALTY As Long = &H544C4E50
Const SCH_STAGECOUNT As Long = &H43475453
Const SCH_CHIPFILE As Long = &H3046434D
Const SCH_CHIPDATA As Long = &H3044434D
Const SCH_HITDATA As Long = &H53544948

Public Sub CreateNewFile()
    ChangeFileName ""
    MapChipFile(0) = ""
    MapChipFile(1) = ""
    MapChipFile(2) = ""
    SetMapChipFile 0, ""
    SetMapChipFile 1, ""
    SetMapChipFile 2, ""
    StageCount = 0
    AddStage
    InitInfo
End Sub

Private Sub Class_Initialize()
    StageCount = 0
    ReDim Stage(0)
    TempFile = GetFullFileName("Temp.dat")
    ReDim StageInfo(0)
    DefineInfo SCH_TITLE, "�X�e�[�W�t�@�C����", "�X�e�[�W�Z���N�g�ŕ\������閼�O�ł��B", ""
    DefineInfo SCH_AUTHOR, "�X�e�[�W���", "�X�e�[�W��������l�A�܂肠�Ȃ��̖��O�ł��B", ""
    DefineInfo SCH_DESCRIPTION, "����", "�X�e�[�W�̐����ł��B���̂Ƃ���g���Ă��܂���B", ""
    DefineInfo SCH_ENDING, "�G���f�B���O", "�G���f�B���O���b�Z�[�W�ł��B���̂Ƃ���g���Ă��܂���B", ""
    DefineInfo SCH_REST, "�c��l��", "���̉񐔂����ƃQ�[���I�[�o�[�ł��B", CByte(4), CByte(1), CByte(100)
    DefineInfo SCH_PENALTY, "�y�i���e�B", "�Q�[���I�[�o�[��ɃR���e�B�j���[���邽�߂ɏ�����A�C�e���̐��ł��B", CInt(100), CInt(10), CInt(999)
End Sub

Public Sub ShowMapEdit(num As Byte)
    If 0 <= num And num < StageCount Then Stage(num).ShowMapEdit
End Sub

Public Function GetStageCount() As Byte
    GetStageCount = StageCount
End Function

Public Function GetStageTitle(num As Byte) As String
    If 0 <= num And num < StageCount Then GetStageTitle = Stage(num).GetTitle
End Function

Public Function GetMapChipDC(Level As Integer) As Long
    If 0 <= Level And Level <= 2 Then GetMapChipDC = MDIFormMain.picChip(Level).hDC
End Function

Public Function GetMapChipMaskDC(Level As Integer) As Long
    If 0 <= Level And Level <= 2 Then GetMapChipMaskDC = MDIFormMain.picChipMask(Level).hDC
End Function

Public Function GetTitle() As String
    GetTitle = GetInfo(SCH_TITLE)
End Function

Public Sub SetTitle(t As String)
    SetInfo SCH_TITLE, t
End Sub

Public Function GetAuthor() As String
    GetAuthor = GetInfo(SCH_AUTHOR)
End Function

Public Sub SetAuthor(a As String)
    SetInfo SCH_AUTHOR, a
End Sub

Public Function GetComment() As String
    GetComment = GetInfo(SCH_DESCRIPTION)
End Function

Public Sub SetComment(C As String)
    SetInfo SCH_DESCRIPTION, C
End Sub

Public Function GetFileName() As String
    GetFileName = FileName
End Function

Public Sub Save()
    Dim fn As String
    fn = FileName
    If fn = "" Then
        fn = InputBoxEx("�Z�[�u����t�@�C�������w�肵�Ă��������B" & vbNewLine & "�g���q�͂���܂���(�����ő}������܂�)�B", "�Z�[�u", "")
    End If
    If fn = "" Then Exit Sub
    ChangeFileName fn
    Dim N As Integer
    N = FreeFile()
    Open TempFile For Output As #N
    Close N
    Open TempFile For Binary Access Write Lock Read Write As N
        InnerSave N
    Close N
    CompileNoCompress
    Dim msg As String
    Select Case CompileCompress(TempFile, fn & ".f3s")
    Case 0: msg = FileName & "�̕ۑ��ɐ������܂����B"
    Case 1: msg = TempFile & "�̓ǂݍ��݂Ɏ��s���܂����B"
    Case 2: msg = TempFile & "�̈��k�Ɏ��s���܂����B"
    Case 3: msg = FileName & "�̏������݂Ɏ��s���܂����B"
    End Select
    Kill TempFile
    MsgBox msg
End Sub

Private Sub ChangeFileName(fn As String)
    FileName = fn
    MDIFormMain.Caption = App.Title & " [" & IIf(fn = "", "����", fn) & "]"
End Sub

Private Sub InnerSave(ByVal N As Integer, Optional ByVal C As Boolean = False)
    Dim d() As Byte
    Dim m As Integer
    Dim I As Long
    For I = 0 To UBound(StageInfo) - 1
        With StageInfo(I)
            If .Value <> .CDefault Then
                Select Case VarType(.CDefault)
                Case vbString
                    d = StrConv(CStr(.Value), vbFromUnicode)
                    Put N, , CLng(UBound(d) + 1)
                    Put N, , .CHeader
                    Put N, , d
                Case vbLong
                    Put N, , 4&
                    Put N, , .CHeader
                    Put N, , CLng(.Value)
                Case vbInteger
                    Put N, , 2&
                    Put N, , .CHeader
                    Put N, , CInt(.Value)
                Case vbByte
                    Put N, , 1&
                    Put N, , .CHeader
                    Put N, , CByte(.Value)
                End Select
            End If
        End With
    Next
    If StageCount <> 1 Then
        Put N, , 1&
        Put N, , SCH_STAGECOUNT
        Put N, , StageCount
    End If
    For I = 0 To StageCount - 1
        Stage(I).Save N, I
    Next
    For I = 0 To 2
        If Len(MapChipFile(I)) Then
            d = StrConv(MapChipFile(I), vbFromUnicode)
            Put N, , CLng(UBound(d) + 1)
            Put N, , SCH_CHIPFILE Or &H1000000 * I
            Put N, , d
            m = FreeFile
            Open MapChipFile(I) For Binary As #m
                ReDim d(LOF(m) - 1)
                Get #m, , d
            Close #m
            Put N, , CLng(UBound(d) + 1)
            Put N, , SCH_CHIPDATA Or &H1000000 * I
            Put N, , d
        End If
    Next
    If Len(MapChipFile(1)) Then
        Dim N2 As Integer
        Dim B As Byte
        Dim hf As String
        Put N, , 240&
        Put N, , SCH_HITDATA
        hf = GetMapChipFile(1) & ".f3h"
        N2 = FreeFile
        If Dir(hf) = "" Then
            Open hf For Output As #N2
            Close
        End If
        Open hf For Binary As #N2
            For I = 0 To 239
                If I < LOF(N) Then
                    Get #N2, , B
                Else
                    If (I And &HF) = 0 Then
                        B = &H0
                    ElseIf (I And &HF) = 1 Then
                        B = &HF
                    Else
                        B = &H10
                    End If
                End If
                Put N, , B
            Next
        Close
    End If
End Sub

Private Sub CompileNoCompress()
    Dim N As Integer, fs As Long
    Dim d() As Byte, s() As Byte
    N = FreeFile
    fs = FileLen(TempFile)
    Open TempFile For Binary Access Read As N
        ReDim s(fs - 1)
        Get N, , s
    Close N
    Open GetFullFileName(FileName & ".f3d") For Output As #N
    Close N
    Open GetFullFileName(FileName & ".f3d") For Binary Access Write Lock Read Write As N
        d = StrConv("funya3s1", vbFromUnicode)
        Put N, , d
        Put N, , fs
        Put N, , fs
        Put N, , s
    Close N
End Sub

Public Sub LoadStage(fn As String)
    Dim Size1 As Long, Size2 As Long
    Dim N As Integer, fs As Long
    Dim d() As Byte, s() As Byte
    Dim str As String
    Dim Addr As Long, DataSize As Long
    Dim I As Long
    InitInfo
    N = FreeFile
    If LCase(Right(fn, 4)) = ".f3d" Then fn = Left(fn, Len(fn) - 4)
    Open GetFullFileName(fn & ".f3d") For Binary Access Read Lock Read Write As N
        ReDim d(7)
        Get N, , d
        If StrConv(d, vbUnicode) = "funya3s1" Then
            str = "�w�b�_�ƍ������I"
            Get N, , Size1
            Get N, , Size2
            If Size1 = Size2 Then
                str = "�T�C�Y�m�F�����I"
                ReDim s(Size1 - 1)
                Get N, , s
                str = "�f�[�^�i�[�����I"
                '�S�ʐݒ���Q�b�g�ł�
                For I = 0 To UBound(StageInfo) - 1
                    With StageInfo(I)
                        Select Case VarType(.CDefault)
                        Case vbString
                            .Value = GetDataString(s, .CHeader, Size1)
                        Case vbLong
                            Addr = GetStageData(s(0), Size1, .CHeader, fs)
                            If Addr Then
                                Dim lv As Long
                                RtlMoveMemory lv, ByVal Addr, fs
                                .Value = lv
                            Else
                                .Value = .CDefault
                            End If
                        Case vbInteger
                            Addr = GetStageData(s(0), Size1, .CHeader, fs)
                            If Addr Then
                                Dim iv As Integer
                                RtlMoveMemory iv, ByVal Addr, fs
                                .Value = iv
                            Else
                                .Value = .CDefault
                            End If
                        Case vbByte
                            Addr = GetStageData(s(0), Size1, .CHeader, fs)
                            If Addr Then
                                Dim bv As Byte
                                RtlMoveMemory bv, ByVal Addr, fs
                                .Value = bv
                            Else
                                .Value = .CDefault
                            End If
                        End Select
                    End With
                Next
'                Title = GetDataString(s, SCH_TITLE, Size1)
'                Author = GetDataString(s, SCH_AUTHOR, Size1)
'                Comment = GetDataString(s, SCH_DESCRIPTION, Size1)
'                Ending = GetDataString(s, SCH_ENDING, Size1)
                '�X�e�[�W���Q�b�g�ł�
                Addr = GetStageData(s(0), Size1, SCH_STAGECOUNT, fs)
                If Addr Then
                    RtlMoveMemory StageCount, ByVal Addr, fs
                Else
                    StageCount = 1
                End If
                ReDim Stage(StageCount - 1)
                '�X�e�[�W�f�[�^�Q�b�g�ł�
                For I = 0 To StageCount - 1
                    Set Stage(I) = New clsStage
                    Stage(I).Load s, I, Size1
                Next
                '�}�b�v�`�b�v�Q�b�g�ł�
                For I = 0 To 2
                    SetMapChipFile CInt(I), GetDataString(s, SCH_CHIPFILE Or &H1000000 * I, Size1)
                Next
            End If
        End If
    Close N
    ChangeFileName fn
End Sub

Public Sub SetMapChipFile(Level As Integer, fn As String)
    MapChipFile(Level) = fn
    If Len(fn) Then
        Set MapChipData(Level) = LoadPicture(GetFullFileName(fn))
    Else
        If Level = 1 Then
            Set MapChipData(Level) = MDIFormMain.picDefChip.Picture
        Else
            Set MapChipData(Level) = LoadPicture()
        End If
    End If
    MDIFormMain.picChip(Level).Picture = MapChipData(Level)
    CreateMask MDIFormMain.picChip(Level), MDIFormMain.picChipMask(Level)
    MapChipFile(Level) = fn
End Sub

Public Function GetMapChipFile(Level As Integer)
    GetMapChipFile = MapChipFile(Level)
End Function

Public Sub SetEnding(e As String)
    SetInfo SCH_ENDING, e
End Sub

Public Function GetEnding() As String
    GetEnding = GetInfo(SCH_ENDING)
End Function

Public Function AddStage() As Boolean
    If StageCount < 100 Then
        ReDim Preserve Stage(StageCount)
        Set Stage(StageCount) = New clsStage
        Stage(StageCount).PlainMap
        StageCount = StageCount + 1
        AddStage = True
    Else
        AddStage = False
    End If
End Function

Private Sub DefineInfo(CHeader As Long, CName As String, CDesc As String, CDef As Variant, Optional CMin As Variant, Optional CMax As Variant)
    With StageInfo(UBound(StageInfo))
        .CHeader = CHeader
        .CName = CName
        .CDescription = CDesc
        .CDefault = CDef
        .CMin = CMin
        .CMax = CMax
    End With
    ReDim Preserve StageInfo(UBound(StageInfo) + 1)
End Sub

Public Sub InitInfo()
    Dim I As Integer
    For I = 0 To UBound(StageInfo) - 1
        StageInfo(I).Value = StageInfo(I).CDefault
    Next
End Sub

Public Sub SetInfo(CHeader As Long, Value As Variant)
    Dim I As Integer
    For I = 0 To UBound(StageInfo) - 1
        If StageInfo(I).CHeader = CHeader Then
            If Not IsMissing(StageInfo(I).CMin) Then
                If Value - StageInfo(I).CMin < 0 Or StageInfo(I).CMax - Value < 0 Then Exit Sub
            End If
            Select Case VarType(StageInfo(I).CDefault)
            Case vbString: StageInfo(I).Value = CStr(Value)
            Case vbLong: StageInfo(I).Value = CLng(Value)
            Case vbInteger: StageInfo(I).Value = CInt(Value)
            Case vbByte: StageInfo(I).Value = CByte(Value)
            End Select
            Exit Sub
        End If
    Next
End Sub

Public Function GetInfo(CHeader As Long) As Variant
    Dim I As Integer
    For I = 0 To UBound(StageInfo) - 1
        If StageInfo(I).CHeader = CHeader Then
            GetInfo = StageInfo(I).Value
            Exit Function
        End If
    Next
End Function

Public Function GetInfoMax(CHeader As Long) As Variant
    Dim I As Integer
    For I = 0 To UBound(StageInfo) - 1
        If StageInfo(I).CHeader = CHeader Then
            GetInfoMax = StageInfo(I).CMax
            Exit Function
        End If
    Next
End Function

Public Function GetInfoMin(CHeader As Long) As Variant
    Dim I As Integer
    For I = 0 To UBound(StageInfo) - 1
        If StageInfo(I).CHeader = CHeader Then
            GetInfoMin = StageInfo(I).CMin
            Exit Function
        End If
    Next
End Function

Public Function GetInfoName(CHeader As Long) As String
    Dim I As Integer
    For I = 0 To UBound(StageInfo) - 1
        If StageInfo(I).CHeader = CHeader Then
            GetInfoName = StageInfo(I).CName
            Exit Function
        End If
    Next
End Function

Public Function GetInfoDescription(CHeader As Long) As String
    Dim I As Integer
    For I = 0 To UBound(StageInfo) - 1
        If StageInfo(I).CHeader = CHeader Then
            GetInfoDescription = StageInfo(I).CDescription
            Exit Function
        End If
    Next
End Function

Public Function GetHeaderList() As Variant
    Dim hl() As Long
    ReDim hl(UBound(StageInfo) - 1)
    Dim I As Long
    For I = 0 To UBound(hl)
        hl(I) = StageInfo(I).CHeader
    Next
    GetHeaderList = hl
End Function

Public Sub RemoveStage(num As Byte)
    Dim I As Integer
    For I = num To UBound(Stage) - 1
        Set Stage(I) = Stage(I + 1)
    Next
    ReDim Preserve Stage(UBound(Stage) - 1)
    StageCount = StageCount - 1
End Sub

Public Function GetStageNum(stg As clsStage) As Integer
    Dim I As Integer
    For I = 0 To UBound(Stage)
        If stg Is Stage(I) Then
            GetStageNum = I
            Exit Function
        End If
    Next
    GetStageNum = -1
End Function

Public Sub EditHit(ByVal Chip As Byte)
    If GetMapChipFile(1) = "" Then Exit Sub
    Dim hf As String
    Dim B(239) As Byte
    hf = GetMapChipFile(1) & ".f3h"
    Dim I As Integer, N As Integer
    '�ǂݍ���
    N = FreeFile
    If Dir(hf) = "" Then
        Open hf For Output As #N
        Close
    End If
    Open hf For Binary As #N
        For I = 0 To 239
            If I < LOF(N) Then
                Get #N, , B(I)
            Else
                If (I And &HF) = 0 Then
                    B(I) = &H0
                ElseIf (I And &HF) = 1 Then
                    B(I) = &HF
                Else
                    B(I) = &H10
                End If
            End If
        Next
    Close
    '�ҏW
    Load FormHit
    FormHit.SetHit B(Chip)
    FormHit.Show vbModal
    B(Chip) = FormHit.Hit
    Unload FormHit
    '�����o��
    Open hf For Output As #N
    Close
    Open hf For Binary As #N
        Put #N, , B
    Close
End Sub

Public Sub SwapStage(N As Integer, m As Integer)
    Dim L As clsStage
    Set L = Stage(N)
    Set Stage(N) = Stage(m)
    Set Stage(m) = L
End Sub