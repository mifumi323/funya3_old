VERSION 5.00
Begin VB.Form FormMain 
   Caption         =   "�ӂɂႳ��ݒ�"
   ClientHeight    =   4455
   ClientLeft      =   165
   ClientTop       =   555
   ClientWidth     =   6735
   Icon            =   "FormMain.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   4455
   ScaleWidth      =   6735
   StartUpPosition =   3  'Windows �̊���l
   Begin VB.Frame fraCategory 
      Caption         =   "�L�[����ݒ�"
      Height          =   1335
      Index           =   20
      Left            =   3360
      TabIndex        =   10
      Top             =   0
      Width           =   1815
      Begin VB.CommandButton cmdKeyReset 
         Caption         =   "���̃L�[�����Z�b�g"
         Height          =   255
         Left            =   120
         TabIndex        =   13
         Top             =   600
         Width           =   1575
      End
      Begin VB.TextBox txtKey 
         Height          =   270
         Left            =   120
         Locked          =   -1  'True
         MultiLine       =   -1  'True
         TabIndex        =   12
         Top             =   960
         Width           =   1575
      End
      Begin VB.ComboBox cmbKey 
         Height          =   300
         Left            =   120
         Style           =   2  '��ۯ���޳� ؽ�
         TabIndex        =   11
         Top             =   240
         Width           =   1575
      End
   End
   Begin VB.Frame fraCategory 
      Caption         =   "���[�U�[��`BGM"
      Height          =   1695
      Index           =   10
      Left            =   0
      TabIndex        =   1
      Top             =   360
      Width           =   2055
      Begin VB.CheckBox chkBGMLoop 
         Caption         =   "���[�v"
         Height          =   255
         Left            =   120
         TabIndex        =   9
         Top             =   960
         Width           =   855
      End
      Begin VB.CommandButton cmdBGMRefference 
         Caption         =   "�Q��(&R)..."
         Height          =   255
         Left            =   1080
         TabIndex        =   5
         Top             =   960
         Width           =   855
      End
      Begin VB.TextBox txtBGM 
         Height          =   270
         Left            =   120
         TabIndex        =   4
         Text            =   "Text1"
         Top             =   600
         Width           =   1815
      End
      Begin VB.ComboBox cmbBGM 
         Height          =   300
         Left            =   120
         Style           =   2  '��ۯ���޳� ؽ�
         TabIndex        =   3
         Top             =   240
         Width           =   1815
      End
      Begin VB.Label lblBGM 
         BackStyle       =   0  '����
         Caption         =   "Label1"
         Height          =   255
         Left            =   120
         TabIndex        =   6
         Top             =   1320
         Width           =   1815
      End
   End
   Begin VB.Frame fraCategory 
      Caption         =   "���v���C�f�[�^"
      Height          =   2895
      Index           =   40
      Left            =   3480
      TabIndex        =   29
      Top             =   1440
      Width           =   3735
      Begin VB.FileListBox filReplay 
         Height          =   270
         Left            =   1560
         Pattern         =   "*.f3r"
         TabIndex        =   40
         Top             =   2040
         Visible         =   0   'False
         Width           =   975
      End
      Begin VB.CommandButton cmdDelete 
         Caption         =   "�폜"
         Height          =   375
         Left            =   1800
         TabIndex        =   39
         Top             =   2400
         Width           =   735
      End
      Begin VB.CommandButton cmdConvert 
         Caption         =   "�^��"
         Height          =   375
         Left            =   960
         TabIndex        =   38
         Top             =   2400
         Width           =   735
      End
      Begin VB.CommandButton cmdReplay 
         Caption         =   "�Đ�"
         Height          =   375
         Left            =   120
         TabIndex        =   37
         Top             =   2400
         Width           =   735
      End
      Begin VB.ListBox lstReplay 
         Height          =   240
         Left            =   120
         TabIndex        =   36
         Top             =   2040
         Width           =   1335
      End
      Begin VB.HScrollBar hscRecordNumber 
         Height          =   255
         LargeChange     =   10
         Left            =   120
         Max             =   1000
         TabIndex        =   35
         Top             =   1680
         Width           =   2175
      End
      Begin VB.HScrollBar hscRecordMiss 
         Height          =   255
         LargeChange     =   10
         Left            =   120
         Max             =   100
         TabIndex        =   33
         Top             =   1080
         Width           =   2175
      End
      Begin VB.HScrollBar hscRecordClear 
         Height          =   255
         LargeChange     =   10
         Left            =   120
         Max             =   100
         TabIndex        =   31
         Top             =   480
         Width           =   2175
      End
      Begin VB.Label lblRecordNumber 
         Caption         =   "�L�^���鐔"
         Height          =   180
         Left            =   120
         TabIndex        =   34
         Top             =   1440
         Width           =   2280
      End
      Begin VB.Label lblRecordMiss 
         Caption         =   "�~�X�����Ƃ������I�ɋL�^�H"
         Height          =   180
         Left            =   120
         TabIndex        =   32
         Top             =   840
         Width           =   2280
      End
      Begin VB.Label lblRecordClear 
         Caption         =   "�N���A�����Ƃ������I�ɋL�^�H"
         Height          =   180
         Left            =   120
         TabIndex        =   30
         Top             =   240
         Width           =   2280
      End
   End
   Begin VB.Frame fraCategory 
      Caption         =   "�S��"
      Height          =   4335
      Index           =   0
      Left            =   1920
      TabIndex        =   2
      Top             =   0
      Width           =   3015
      Begin VB.CheckBox chkBackground 
         Caption         =   "��A�N�e�B�u�ł����삳����"
         Height          =   255
         Left            =   120
         TabIndex        =   43
         Top             =   960
         Width           =   2535
      End
      Begin VB.ComboBox cmbTime 
         Height          =   300
         ItemData        =   "FormMain.frx":0442
         Left            =   120
         List            =   "FormMain.frx":0452
         Style           =   2  '��ۯ���޳� ؽ�
         TabIndex        =   42
         Top             =   2040
         Width           =   1455
      End
      Begin VB.CheckBox chkHyper 
         Caption         =   "�X�̑�������"
         Height          =   255
         Left            =   120
         TabIndex        =   24
         Top             =   3840
         Visible         =   0   'False
         Width           =   1455
      End
      Begin VB.ComboBox cmbEffect 
         Height          =   300
         ItemData        =   "FormMain.frx":04A6
         Left            =   120
         List            =   "FormMain.frx":04C2
         Style           =   2  '��ۯ���޳� ؽ�
         TabIndex        =   23
         Top             =   3480
         Visible         =   0   'False
         Width           =   1455
      End
      Begin VB.OptionButton optChara 
         Height          =   615
         Index           =   71
         Left            =   2280
         Picture         =   "FormMain.frx":056E
         Style           =   1  '���̨���
         TabIndex        =   22
         ToolTipText     =   "�~�X�^�[�t���[��"
         Top             =   1320
         Visible         =   0   'False
         Width           =   615
      End
      Begin VB.OptionButton optChara 
         Height          =   615
         Index           =   45
         Left            =   1560
         Picture         =   "FormMain.frx":065E
         Style           =   1  '���̨���
         TabIndex        =   21
         ToolTipText     =   "����"
         Top             =   1320
         Visible         =   0   'False
         Width           =   615
      End
      Begin VB.OptionButton optChara 
         Height          =   615
         Index           =   1
         Left            =   840
         Picture         =   "FormMain.frx":0739
         Style           =   1  '���̨���
         TabIndex        =   20
         ToolTipText     =   "�T���O���X"
         Top             =   1320
         Width           =   615
      End
      Begin VB.OptionButton optChara 
         Height          =   615
         Index           =   0
         Left            =   120
         Picture         =   "FormMain.frx":081A
         Style           =   1  '���̨���
         TabIndex        =   19
         ToolTipText     =   "�ӂɂ�"
         Top             =   1320
         Value           =   -1  'True
         Width           =   615
      End
      Begin VB.ComboBox cmbGravity 
         Height          =   300
         ItemData        =   "FormMain.frx":08EC
         Left            =   120
         List            =   "FormMain.frx":08F9
         Style           =   2  '��ۯ���޳� ؽ�
         TabIndex        =   18
         Top             =   3120
         Visible         =   0   'False
         Width           =   1455
      End
      Begin VB.CheckBox chkESP 
         Caption         =   "���x���m"
         Height          =   255
         Left            =   120
         TabIndex        =   17
         Top             =   2760
         Visible         =   0   'False
         Width           =   1455
      End
      Begin VB.ComboBox cmbFPS 
         Height          =   300
         ItemData        =   "FormMain.frx":0921
         Left            =   120
         List            =   "FormMain.frx":0942
         Style           =   2  '��ۯ���޳� ؽ�
         TabIndex        =   16
         Top             =   2400
         Visible         =   0   'False
         Width           =   1455
      End
      Begin VB.CheckBox chkFullScreen 
         Caption         =   "�t���X�N���[��"
         Height          =   255
         Left            =   120
         TabIndex        =   8
         Top             =   600
         Width           =   1455
      End
      Begin VB.ComboBox cmbBGMMode 
         Height          =   300
         ItemData        =   "FormMain.frx":0980
         Left            =   120
         List            =   "FormMain.frx":098D
         Style           =   2  '��ۯ���޳� ؽ�
         TabIndex        =   7
         Top             =   240
         Width           =   1455
      End
   End
   Begin VB.Frame fraButton 
      BorderStyle     =   0  '�Ȃ�
      Height          =   1815
      Left            =   120
      TabIndex        =   25
      Top             =   2520
      Width           =   1695
      Begin VB.CommandButton cmdSave 
         Caption         =   "�ݒ�ۑ�"
         Height          =   375
         Left            =   0
         TabIndex        =   41
         Top             =   480
         Width           =   1695
      End
      Begin VB.CommandButton cmdOK 
         Caption         =   "OK"
         Height          =   375
         Left            =   0
         TabIndex        =   28
         Top             =   960
         Width           =   1695
      End
      Begin VB.CommandButton cmdCancel 
         Caption         =   "�L�����Z��"
         Height          =   375
         Left            =   0
         TabIndex        =   27
         Top             =   1440
         Width           =   1695
      End
      Begin VB.CommandButton cmdPlay 
         Caption         =   "�Q�[���N��"
         Height          =   375
         Left            =   0
         TabIndex        =   26
         Top             =   0
         Width           =   1695
      End
   End
   Begin VB.Frame fraCategory 
      Caption         =   "�v���C�L�^"
      Height          =   615
      Index           =   100
      Left            =   5040
      TabIndex        =   14
      Top             =   0
      Width           =   1695
      Begin VB.ListBox lstResult 
         Height          =   240
         Left            =   120
         TabIndex        =   15
         Top             =   240
         Width           =   1455
      End
   End
   Begin VB.ListBox lstCategory 
      Height          =   2220
      ItemData        =   "FormMain.frx":09BD
      Left            =   120
      List            =   "FormMain.frx":09BF
      TabIndex        =   0
      Top             =   120
      Width           =   1695
   End
End
Attribute VB_Name = "FormMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim BGM As New clsBGM

Dim Banana As Long
Dim PlayTime As Long
Dim SleepTime As Long
Dim Smiles As Long
Dim TimeMaster As Long
Dim FPS As Long
Dim Eyewitness As Long
Dim FeatherIron As Long
Dim ESP As Long
Dim Gravity As Long
Dim GrapeColored As Long
Dim Esrever As Long
Dim DrawMethod As Long
Dim Outline As Long
Dim ColdMan As Long
Dim Hyper As Long
Dim AndBalloon As Long
Dim CheckSum As Long
Dim UnknownData As String


Private Type tagKey
    ID  As String
    Key As Long
    Dsc As String
End Type
Dim Key() As tagKey

Const SettingFile As String = "funya3.ini"

Private Sub chkBGMLoop_Click()
    BGM.SetLoopModeByText cmbBGM.Text, chkBGMLoop.Value <> 0
End Sub

Private Sub chkESP_Click()
    ESP = chkESP.Value
End Sub

Private Sub chkHyper_Click()
    Hyper = chkHyper.Value
End Sub

Private Sub cmbBGM_Click()
    txtBGM.Text = BGM.GetFileNameByText(cmbBGM.Text)
    chkBGMLoop.Value = IIf(BGM.GetLoopModeByText(cmbBGM.Text), 1, 0)
End Sub

Private Sub cmbEffect_Click()
    DrawMethod = cmbEffect.ListIndex
End Sub

Private Sub cmbFPS_Click()
    FPS = cmbFPS.ItemData(cmbFPS.ListIndex)
End Sub

Private Sub cmbGravity_Click()
    Gravity = cmbGravity.ListIndex
End Sub

Private Sub cmbKey_Click()
    txtKey_LostFocus
End Sub

Private Sub cmdBGMRefference_Click()
    Dim ofn As OPENFILENAME
    Dim Filter As String
    Dim flags As Long
    Filter = "���f�B�A�t�@�C��|*.mid;*.mp3;*.wma|MIDI�t�@�C��|*.mid|MP3�t�@�C��|*.mp3|WMA�t�@�C��|*.wma|�S�Ẵt�@�C��|*.*"
    Filter = Replace(Filter, "|", Chr(0))
    flags = OFN_FILEMUSTEXIST Or OFN_HIDEREADONLY Or OFN_PATHMUSTEXIST
    SetOPENFILENAME ofn, hWnd, Filter, 255, 255, "", "BGM[" & cmbBGM.Text & "]��T��", flags, ""
    If GetOpenFileName(ofn) Then
        txtBGM.Text = ofn.lpstrFile
    End If
End Sub

Private Sub cmdCancel_Click()
    Unload Me
End Sub

Private Sub cmdConvert_Click()
    If lstReplay.ListIndex < 0 Then Exit Sub
    Shell "funya3.exe RECORD """ & Replace(filReplay.List(lstReplay.ListIndex), ".f3r", "") & """", vbNormalFocus
End Sub

Private Sub cmdDelete_Click()
    Dim li As Integer
    li = lstReplay.ListIndex
    If MsgBox(lstReplay.List(li) & vbNewLine & "���폜���܂��B��낵���ł����H", vbOKCancel, "�m�F") = vbOK Then
        Kill filReplay.List(li)
        RefreshReplay
        lstReplay.ListIndex = li - 1
    End If
End Sub

Private Sub cmdKeyReset_Click()
    Key(GetKeyByDsc(cmbKey.Text)).Key = 0
    txtKey_LostFocus
End Sub

Private Sub cmdOK_Click()
    BGM.SaveFile
    f3SaveSetting
    Unload Me
End Sub

Private Sub cmdPlay_Click()
    BGM.SaveFile
    f3SaveSetting
    ChDir App.Path
    Shell "funya3.exe", vbNormalFocus
    Unload Me
End Sub

Private Sub cmdReplay_Click()
    If lstReplay.ListIndex < 0 Then Exit Sub
    Shell "funya3.exe REPLAY """ & Replace(filReplay.List(lstReplay.ListIndex), ".f3r", "") & """", vbNormalFocus
End Sub

Private Sub cmdSave_Click()
    BGM.SaveFile
    f3SaveSetting
End Sub

Private Sub Form_Load()
    Dim N
    Dim I As Long
    With lstCategory
        For Each N In fraCategory
            .AddItem N.Caption
        Next
        .ListIndex = 0
    End With
    With cmbBGM
        For I = 0 To BGM.GetSize - 1
            .AddItem BGM.GetText(I)
        Next
        .ListIndex = 0
    End With
    With cmbFPS
        For I = 0 To .ListCount - 1
            .List(I) = "�Q�[���e���|�F" & .List(I) & "(" & .ItemData(I) & "FPS)"
        Next
    End With
    ReDim Key(0)
    AddKey "KEY_PAUSE", "�|�[�Y�E�X�^�[�g"
    AddKey "KEY_UP", "��"
    AddKey "KEY_LEFT", "��"
    AddKey "KEY_RIGHT", "�E"
    AddKey "KEY_DOWN", "��"
    AddKey "KEY_JUMP", "�W�����v"
    AddKey "KEY_ATTACK", "�U��"
    AddKey "KEY_SMILE", "�ɂ�����"
    AddKey "KEY_FPS", "FPS�\��"
    AddKey "KEY_BGMNONE", "BGM����"
    AddKey "KEY_BGMDEF", "BGM�W���ɐ؂�ւ�"
    AddKey "KEY_BGMUSER", "BGM���[�U�["
    AddKey "KEY_CAPTURE", "�Q�[����ʕۑ�"
    AddKey "KEY_RECORD", "���v���C�ۑ�"
    AddKey "KEY_EXIT", "�����I��"
    With lstResult
        .AddItem "�Q�[�����F�ӂɂႳ��"
        .AddItem "�o�[�W�����F" & App.Major & "." & Format(App.Minor, "00")
        .AddItem "�Q�[����ҁF����(�Ƃ���)"
        .AddItem "��҃z�[���y�[�W�FTGWS(http://tgws.fromc.jp/)"
        .AddItem ""
    End With
    f3LoadSetting
    For I = 0 To UBound(Key) - 1
        If Key(I).ID <> "KEY_SMILE" Or Smiles Then cmbKey.AddItem Key(I).Dsc
    Next
    cmbKey.ListIndex = 0
    RefreshReplay
End Sub

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
    If UnloadMode <> VbMethod Then
        Select Case MsgBox("�ݒ�Z�[�u�H", vbYesNoCancel)
        Case vbYes
            BGM.SaveFile
            f3SaveSetting
        Case vbCancel
            Cancel = True
        End Select
    End If
End Sub

Private Sub Form_Resize()
    On Error Resume Next
    If WindowState <> vbMinimized Then
        '�����j���[
        lstCategory.Height = ScaleHeight - fraButton.Height - 360
        fraButton.Top = lstCategory.Height + 240
        '�t���[��
        Dim FrameWidth As Single, FrameHeight As Single
        FrameWidth = ScaleWidth - lstCategory.Width - 360
        FrameHeight = ScaleHeight - 240
        Dim N
        For Each N In fraCategory
            N.Move lstCategory.Width + 240, 120, FrameWidth, FrameHeight
        Next
        Dim ItemWidth As Single
        ItemWidth = FrameWidth - 240
        '�S��
        cmbBGMMode.Width = ItemWidth
        chkFullScreen.Width = ItemWidth
        cmbTime.Width = ItemWidth
        cmbFPS.Width = ItemWidth
        chkESP.Width = ItemWidth
        cmbGravity.Width = ItemWidth
        cmbEffect.Width = ItemWidth
        chkHyper.Width = ItemWidth
        lstResult.Width = ItemWidth
        lstResult.Height = FrameHeight - lstResult.Top - 120
        'BGM
        cmbBGM.Width = ItemWidth
        txtBGM.Width = ItemWidth
        chkBGMLoop.Width = ItemWidth / 2 - 60
        cmdBGMRefference.Width = chkBGMLoop.Width
        cmdBGMRefference.Left = ItemWidth / 2 + 180
        lblBGM.Width = ItemWidth
        lblBGM.Height = FrameHeight
        '�L�[
        cmbKey.Width = ItemWidth
        cmdKeyReset.Width = ItemWidth
        txtKey.Width = ItemWidth
        txtKey.Height = FrameHeight - txtKey.Top - 120
        '���v���C
        lblRecordClear.Width = ItemWidth
        hscRecordClear.Width = ItemWidth
        lblRecordMiss.Width = ItemWidth
        hscRecordMiss.Width = ItemWidth
        lblRecordNumber.Width = ItemWidth
        hscRecordNumber.Width = ItemWidth
        lstReplay.Width = ItemWidth
        lstReplay.Height = FrameHeight - lstReplay.Top - cmdReplay.Height - 240
        cmdReplay.Top = lstReplay.Top + lstReplay.Height + 120
        cmdReplay.Width = ItemWidth / 3 - 80
        cmdConvert.Left = cmdReplay.Left + cmdReplay.Width + 120
        cmdConvert.Top = cmdReplay.Top
        cmdConvert.Width = cmdReplay.Width
        cmdDelete.Left = cmdConvert.Left + cmdConvert.Width + 120
        cmdDelete.Top = cmdReplay.Top
        cmdDelete.Width = cmdReplay.Width
    End If
End Sub

Private Sub hscRecordClear_Change()
    Dim str As String
    str = "�N���A�������ŋL�^�F"
    Select Case hscRecordClear.Value
    Case 0: str = str & "���Ȃ�"
    Case 100: str = str & "����"
    Case Else: str = str & hscRecordClear.Value & "%�̊m���ł���"
    End Select
    lblRecordClear.Caption = str
End Sub

Private Sub hscRecordClear_Scroll()
    hscRecordClear_Change
End Sub

Private Sub hscRecordMiss_Change()
    Dim str As String
    str = "�~�X�������ŋL�^�F"
    Select Case hscRecordMiss.Value
    Case 0: str = str & "���Ȃ�"
    Case 100: str = str & "����"
    Case Else: str = str & hscRecordMiss.Value & "%�̊m���ł���"
    End Select
    lblRecordMiss.Caption = str
End Sub

Private Sub hscRecordMiss_Scroll()
    hscRecordMiss_Change
End Sub

Private Sub hscRecordNumber_Change()
    lblRecordNumber.Caption = "�L�^���鐔�F" & hscRecordNumber.Value
End Sub

Private Sub hscRecordNumber_Scroll()
    hscRecordNumber_Change
End Sub

Private Sub lstCategory_Click()
    Dim N
    With lstCategory
        For Each N In fraCategory
            N.Visible = (.List(.ListIndex) = N.Caption)
        Next
    End With
End Sub

Private Sub txtBGM_Change()
    BGM.SetFileNameByText cmbBGM.Text, txtBGM.Text
    lblBGM.Caption = txtBGM.Text
End Sub

Public Sub f3LoadSetting()
    On Error Resume Next
    Dim N As Integer, I As Long
    Dim Buf As String
    Dim l As New clsLineParser
    N = FreeFile
    cmbBGMMode.ListIndex = 0
    chkFullScreen.Value = 0
    cmbTime.ListIndex = 0
    cmbGravity.ListIndex = 0
    cmbEffect.ListIndex = 0
    chkHyper.Value = 0
    hscRecordClear.Value = 0
    hscRecordMiss.Value = 0
    hscRecordNumber.Value = 0
    UnknownData = ""
    Open GetFullFileName(SettingFile) For Input As #N
        If Err.Number = 0 Then
            Do Until EOF(N)
                Line Input #N, Buf
                l.SetLine Buf
                If False Then   '���S���̏�����ElseIf�ŕ\�L���邽�߂̃_�~�[
                
                ElseIf l.IsMatch("BGM") Then
                    l.GetNum I
                    If I < 0 Or I > 2 Then I = 0
                    cmbBGMMode.ListIndex = I
                
                ElseIf l.IsMatch("FULLSCREEN") Then
                    l.GetNum I
                    chkFullScreen.Value = -(I <> 0)
                
                ElseIf l.IsMatch("BACKGROUND") Then
                    l.GetNum I
                    chkBackground.Value = -(I <> 0)
                
                ElseIf l.IsMatch("CHARACTER") Then
                    l.GetNum I
                    optChara(I).Value = True
                
                ElseIf l.IsMatch("VIEWTIME") Then
                    l.GetNum I
                    cmbTime.ListIndex = I And 3
                
                ElseIf l.IsMatch("RECORDCLEAR") Then
                    l.GetNum I
                    hscRecordClear.Value = I
                
                ElseIf l.IsMatch("RECORDMISS") Then
                    l.GetNum I
                    hscRecordMiss.Value = I
                
                ElseIf l.IsMatch("RECORDNUMBER") Then
                    l.GetNum I
                    hscRecordNumber.Value = I
                
                ElseIf l.IsMatch("BANANA") Then
                    l.GetNum I
                    Banana = I
                    lstResult.AddItem "������o�i�i�̐��F" & I
                
                ElseIf l.IsMatch("PLAYTIME") Then
                    l.GetNum I
                    PlayTime = I
                    lstResult.AddItem "�v���C���ԁF" & SecToStr(I)
                
                ElseIf l.IsMatch("SLEEPTIME") Then
                    l.GetNum I
                    SleepTime = I
                    lstResult.AddItem "�������ԁF" & SecToStr(I / 40)
                
                ElseIf l.IsMatch("SMILES") Then
                    l.GetNum I
                    Smiles = I
                    lstResult.AddItem "�X�e�[�W���S�N���A�񐔁F" & I
                    lstResult.AddItem "�V�[�N���b�g�P�F���ʂ̏Ί��(�L�[�ݒ聨�ɂ�����)"
                
                ElseIf l.IsMatch("TIMEMASTER") Then
                    l.GetNum I
                    TimeMaster = I
                    cmbFPS.Visible = True
                    lstResult.AddItem "�V�[�N���b�g�Q�F���Ԃ��i���(�S�ʁ��Q�[���e���|)"
                
                ElseIf l.IsMatch("FPS") Then
                    l.GetNum I
                    FPS = I
                    With cmbFPS
                        For I = 0 To .ListCount - 1
                            If .ItemData(I) = FPS Then
                                .ListIndex = I
                                Exit For
                            End If
                        Next
                    End With
                
                ElseIf l.IsMatch("EYEWITNESS") Then
                    l.GetNum I
                    Eyewitness = I
                    chkESP.Visible = True
                    lstResult.AddItem "�V�[�N���b�g�R�F�ڌ���(�S�ʁ����x���m)"
                
                ElseIf l.IsMatch("ESP") Then
                    l.GetNum I
                    ESP = I
                    chkESP.Value = -(ESP <> 0)
                
                ElseIf l.IsMatch("FEATHERIRON") Then
                    l.GetNum I
                    FeatherIron = I
                    cmbGravity.Visible = True
                    lstResult.AddItem "�V�[�N���b�g�S�F�H�̂悤�ɓS�̂悤��(�S�ʁ��d��)"
                
                ElseIf l.IsMatch("GRAVITY") Then
                    l.GetNum I
                    Gravity = I
                    If 0 <= Gravity And Gravity <= 3 Then
                        If cmbGravity.ListCount <= Gravity Then cmbGravity.AddItem "�d�́F�Ȃ�"
                        cmbGravity.ListIndex = Gravity
                    End If
                
                ElseIf l.IsMatch("GRAPECOLORED") Then
                    l.GetNum I
                    GrapeColored = I
                    optChara(45).Visible = True
                    lstResult.AddItem "�V�[�N���b�g�T�F�u�h�E�F�̂��̎q(�S�ʁ�����)"
                
                ElseIf l.IsMatch("ESREVER") Then
                    l.GetNum I
                    Esrever = I
                    cmbEffect.Visible = True
                    lstResult.AddItem "�V�[�N���b�g�U�F�������(�S�ʁ���ʕ\��)"
                
                ElseIf l.IsMatch("DRAWMETHOD") Then
                    l.GetNum I
                    cmbEffect.ListIndex = I
                
                ElseIf l.IsMatch("OUTLINE") Then
                    l.GetNum I
                    Outline = I
                    optChara(71).Visible = True
                    lstResult.AddItem "�V�[�N���b�g�V�F�֊s�����̂�����(�S�ʁ��~�X�^�[�t���[��)"
                
                ElseIf l.IsMatch("COLDMAN") Then
                    l.GetNum I
                    ColdMan = I
                    chkHyper.Visible = True
                    lstResult.AddItem "�V�[�N���b�g�W�F�₽���l�I(�S�ʁ��X�̑�������)"
                
                ElseIf l.IsMatch("HYPER") Then
                    l.GetNum I
                    Hyper = I
                    chkHyper.Value = -(Hyper <> 0)
                
                ElseIf l.IsMatch("ANDBALLOON") Then
                    l.GetNum I
                    AndBalloon = I
                    If cmbGravity.ListCount <= 3 Then cmbGravity.AddItem "�d�́F�Ȃ�"
                    lstResult.AddItem "�V�[�N���b�g�X�F�����ĕ��D�̂悤��(�S�ʁ��d�́F�Ȃ�)"
                
                ElseIf l.IsMatch("CHECKSUM") Then
                    l.GetNum I
                    CheckSum = I
                
                ElseIf ReadKey(l) Then
                
                Else
                    UnknownData = UnknownData & Buf & vbNewLine
                End If
            Loop
        End If
    Close
    hscRecordClear_Change
    hscRecordMiss_Change
    hscRecordNumber_Change
End Sub

Public Sub f3SaveSetting()
    Dim N As Integer, I As Integer
    N = FreeFile
    Open GetFullFileName(SettingFile) For Output As #N
        Print #N, "BGM" & " " & cmbBGMMode.ListIndex
        Print #N, "FULLSCREEN" & " " & chkFullScreen.Value
        Print #N, "BACKGROUND" & " " & chkBackground.Value
        Print #N, "CHARACTER" & " " & TrueOption(optChara)
        Print #N, "VIEWTIME" & " " & cmbTime.ListIndex
        Print #N, "RECORDCLEAR" & " " & hscRecordClear.Value
        Print #N, "RECORDMISS" & " " & hscRecordMiss.Value
        Print #N, "RECORDNUMBER" & " " & hscRecordNumber.Value
        Print #N, "BANANA" & " " & Banana
        Print #N, "PLAYTIME" & " " & PlayTime
        Print #N, "SLEEPTIME" & " " & SleepTime
        If Smiles Then Print #N, "SMILES" & " " & Smiles
        If TimeMaster Then Print #N, "TIMEMASTER" & " " & TimeMaster
        If FPS Then Print #N, "FPS" & " " & FPS
        If Eyewitness Then Print #N, "EYEWITNESS" & " " & Eyewitness
        If ESP Then Print #N, "ESP" & " " & ESP
        If FeatherIron Then Print #N, "FEATHERIRON" & " " & FeatherIron
        If Gravity Then Print #N, "GRAVITY" & " " & Gravity
        If GrapeColored Then Print #N, "GRAPECOLORED" & " " & GrapeColored
        If Esrever Then Print #N, "ESREVER" & " " & Esrever
        If DrawMethod Then Print #N, "DRAWMETHOD" & " " & DrawMethod
        If Outline Then Print #N, "OUTLINE" & " " & Outline
        If ColdMan Then Print #N, "COLDMAN" & " " & ColdMan
        If Hyper Then Print #N, "HYPER" & " " & Hyper
        If AndBalloon Then Print #N, "ANDBALLOON" & " " & AndBalloon
        Print #N, "CHECKSUM" & " " & CheckSum
        For I = 0 To UBound(Key) - 1
            Print #N, Key(I).ID & " " & Key(I).Key
        Next
        Print #N, UnknownData;
    Close
End Sub

Public Sub AddKey(ID As String, Dsc As String)
    Key(UBound(Key)).ID = ID
    Key(UBound(Key)).Key = 0
    Key(UBound(Key)).Dsc = Dsc
    ReDim Preserve Key(UBound(Key) + 1)
End Sub

Public Function SecToStr(Sec As Long) As String
    Dim r As String
    Dim h As Long, m As Long, s As Long
    h = Sec \ 3600
    m = (Sec Mod 3600) \ 60
    s = Sec Mod 60
    If h Then r = h & "����"
    If m Then r = r & m & "��"
    If s Then r = r & s & "�b"
    SecToStr = r
End Function

Public Function ReadKey(l As clsLineParser) As Boolean
    Dim I As Integer
    For I = 0 To UBound(Key) - 1
        If l.IsMatch(Key(I).ID) Then
            l.GetNum Key(I).Key
            ReadKey = True
        End If
    Next
End Function

Public Function GetKeyByDsc(Dsc As String) As Long
    Dim I As Integer
    For I = 0 To UBound(Key)
        If Key(I).Dsc = Dsc Then
            GetKeyByDsc = I
            Exit Function
        End If
    Next
    GetKeyByDsc = -1
End Function

Private Sub txtKey_GotFocus()
    txtKey.Text = cmbKey.Text & "�L�[�F" & Key(GetKeyByDsc(cmbKey.Text)).Key & vbNewLine & "�L�[�������Đݒ肵�܂��B"
End Sub

Private Sub txtKey_KeyDown(KeyCode As Integer, Shift As Integer)
    Key(GetKeyByDsc(cmbKey.Text)).Key = KeyCode
    txtKey_GotFocus
End Sub

Private Sub txtKey_LostFocus()
    txtKey.Text = cmbKey.Text & "�L�[�F" & Key(GetKeyByDsc(cmbKey.Text)).Key
End Sub

Private Function TrueOption(Options) As Integer
    Dim O
    For Each O In Options
        If O.Value = True Then TrueOption = O.Index
    Next
End Function

Private Sub RefreshReplay()
    Dim I As Integer
    Dim fn As String
    Dim dll As Boolean
    dll = IsDLLEnable()
    filReplay.Refresh
    If filReplay.ListCount = lstReplay.ListCount Then Exit Sub
    lstReplay.Clear
    For I = 0 To filReplay.ListCount - 1
        fn = filReplay.List(I)
        If dll Then
            lstReplay.AddItem Left(fn, Len(fn) - 4) & " - " & GetStageName(fn)
        Else
            lstReplay.AddItem Left(fn, Len(fn) - 4)
        End If
    Next
End Sub

Private Function IsDLLEnable()
    On Error Resume Next
    GetCompressedStageData ByVal 0, 0, 0, 0, 0
    IsDLLEnable = Err.Number = 0
End Function

Private Function GetStageName(fn As String)
    Dim Size1 As Long, Size2 As Long
    Dim N As Integer
    Dim d() As Byte, s() As Byte
    Dim stg As Long, fs As Long, Addr As Long
    Const SCH_REPLAY As Long = &H4E475453
    N = FreeFile
    Open GetFullFileName(fn) For Binary Access Read Lock Read Write As N
        ReDim d(7)
        Get N, , d
        If StrConv(d, vbUnicode) = "funya3s1" Then
            Get N, , Size1
            Get N, , Size2
            ReDim s(Size1 - 1)
            Get N, , s
            '�X�e�[�W���Q�b�g�ł�
            Addr = GetCompressedStageData(s(0), Size1, Size2, SCH_REPLAY, fs)
            If Addr Then
                RtlMoveMemory stg, ByVal Addr, fs
            Else
                stg = 0
            End If
            GetStageName = (stg + 1) & "�� " & GetDataString(s, GetChunkType("TL", stg), Size1, Size2)
        End If
    Close N
End Function

Public Function GetChunkType(Prefix As String, I As Long) As Long
    Dim h() As Byte
    h = LeftB(StrConv(Prefix & "  ", vbFromUnicode), 2) & StrConv(Right$("00" & Hex$(I), 2), vbFromUnicode)
    GetChunkType = ((h(3) * 256& + h(2)) * 256& + h(1)) * 256& + h(0)
End Function

