VERSION 5.00
Begin VB.Form FormMapChip 
   BorderStyle     =   5  '��°� ����޳
   Caption         =   "�}�b�v�`�b�v"
   ClientHeight    =   3090
   ClientLeft      =   60
   ClientTop       =   330
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3090
   ScaleWidth      =   4680
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  '��Ű ̫�т̒���
   Begin VB.PictureBox picChara 
      Appearance      =   0  '�ׯ�
      BackColor       =   &H00FF80FF&
      BorderStyle     =   0  '�Ȃ�
      ForeColor       =   &H80000008&
      Height          =   480
      Left            =   0
      ScaleHeight     =   32
      ScaleMode       =   3  '�߸��
      ScaleWidth      =   216
      TabIndex        =   6
      Top             =   2280
      Width           =   3240
   End
   Begin VB.HScrollBar hscX 
      Height          =   255
      Left            =   1680
      TabIndex        =   5
      Top             =   1440
      Width           =   1335
   End
   Begin VB.VScrollBar vscY 
      Height          =   1215
      Left            =   2160
      TabIndex        =   4
      Top             =   960
      Width           =   255
   End
   Begin VB.PictureBox picChip 
      Appearance      =   0  '�ׯ�
      BackColor       =   &H00FF80FF&
      BorderStyle     =   0  '�Ȃ�
      ForeColor       =   &H80000008&
      Height          =   735
      Left            =   600
      ScaleHeight     =   49
      ScaleMode       =   3  '�߸��
      ScaleWidth      =   89
      TabIndex        =   3
      Top             =   480
      Width           =   1335
   End
   Begin VB.CommandButton cmdReset 
      Caption         =   "�W���ɖ߂�"
      Height          =   495
      Left            =   3120
      TabIndex        =   2
      Top             =   1440
      Width           =   1215
   End
   Begin VB.CommandButton cmdLoad 
      Caption         =   "�f�ޓǂݍ���"
      Height          =   495
      Left            =   2880
      TabIndex        =   1
      Top             =   360
      Width           =   1215
   End
   Begin VB.ComboBox cmbLevel 
      Height          =   300
      ItemData        =   "FormMapChip.frx":0000
      Left            =   480
      List            =   "FormMapChip.frx":000D
      Style           =   2  '��ۯ���޳� ؽ�
      TabIndex        =   0
      Top             =   1320
      Width           =   2055
   End
End
Attribute VB_Name = "FormMapChip"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub cmbLevel_Click()
    EditLevel = cmbLevel.ListIndex
    picChip_Paint
End Sub

Private Sub cmdLoad_Click()
    Stages.SetMapChipFile EditLevel, GetGraphicFile(Stages.GetMapChipFile(EditLevel))
    picChip.Refresh
End Sub

Private Sub cmdReset_Click()
    Stages.SetMapChipFile EditLevel, ""
    picChip.Refresh
End Sub

Private Sub Form_Load()
    cmbLevel.ListIndex = EditLevel
End Sub

Private Sub Form_Resize()
    On Error Resume Next
    If WindowState <> vbMinimized Then
        With cmbLevel
            .Move 0, 0, ScaleWidth / 4
            cmdLoad.Move .Width, 0, ScaleWidth * 3 / 8, .Height
            cmdReset.Move .Width + cmdLoad.Width, 0, ScaleWidth - .Width - cmdLoad.Width, .Height
            picChip.Move 0, .Height, ScaleWidth - vscY.Width, ScaleHeight - .Height - picChara.Height - hscX.Height
            picChara.Move 0, picChip.Top + picChip.Height, picChip.Width
            hscX.Move 0, .Height + picChip.Height + picChara.Height, picChip.Width
            vscY.Move picChip.Width, .Height, vscY.Width, picChip.Height + picChara.Height
            hscX.Max = IIf(picChip.ScaleWidth < 512, 512 - picChip.ScaleWidth, 0)
            vscY.Max = IIf(picChip.ScaleHeight < 480, 480 - picChip.ScaleHeight, 0)
        End With
    End If
End Sub

Private Sub Form_Unload(Cancel As Integer)
    MDIFormMain.mnuViewMapChip.Checked = False
End Sub

Private Sub hscX_Change()
    picChara_Paint
    picChip_Paint
End Sub

Private Sub hscX_Scroll()
    picChara_Paint
    picChip_Paint
End Sub

Private Sub picChara_MouseDown(Button As Integer, Shift As Integer, x As Single, y As Single)
    Dim SX As Integer, SY As Integer
    Dim CX As Integer, CY As Integer
    SX = hscX.Value: SY = vscY.Value
    CX = (x + SX) \ 32: CY = 15
    If CX < 16 And CY = 15 Then SelectedChip = CX + CY * 16
    picChara_Paint
    picChip_Paint
End Sub

Private Sub picChara_Paint()
    Dim SX As Integer, SY As Integer
    Dim CX As Integer, CY As Integer
    Dim I As Integer
    SX = hscX.Value: SY = vscY.Value * 0
    CX = (SelectedChip Mod 16) * 32 - SX
    CY = (SelectedChip \ 16) * 32 - SY
    For I = 0 To 15
        BitBlt picChara.hDC, I * 32 - SX, 0, 32, 32, Stages.GetMapChipDC(1), 0, 0, vbSrcCopy
    Next
    BitBlt picChara.hDC, 0, 0, 512 - SX, 32, MDIFormMain.picChara.hDC, SX, 32, vbSrcAnd
    BitBlt picChara.hDC, 0, 0, 512 - SX, 32, MDIFormMain.picChara.hDC, SX, 0, vbSrcPaint
    picChara.Line (CX + 0, CY + 0 - 480)-(CX + 31, CY + 31 - 480), &HFFFFFF, B
    picChara.Line (CX + 1, CY + 1 - 480)-(CX + 30, CY + 30 - 480), &H0, B
    picChara.Line (CX + 2, CY + 2 - 480)-(CX + 29, CY + 29 - 480), &HFFFFFF, B
    picChara.Line (CX + 3, CY + 3 - 480)-(CX + 28, CY + 28 - 480), &H0, B
End Sub

Private Sub picChip_DblClick()
    If EditLevel = 1 And SelectedChip < &HF0 Then Stages.EditHit SelectedChip
End Sub

Private Sub picChip_MouseDown(Button As Integer, Shift As Integer, x As Single, y As Single)
    Dim SX As Integer, SY As Integer
    Dim CX As Integer, CY As Integer
    SX = hscX.Value: SY = vscY.Value
    CX = (x + SX) \ 32: CY = (y + SY) \ 32
    If CX < 16 And CY < 15 Then SelectedChip = CX + CY * 16
    picChip_Paint
    picChara_Paint
End Sub

Private Sub picChip_Paint()
    Dim SX As Long, SY As Long
    Dim CX As Long, CY As Long
    SX = hscX.Value: SY = vscY.Value
    CX = (SelectedChip Mod 16) * 32 - SX
    CY = (SelectedChip \ 16) * 32 - SY
    picChip.Cls
    GIFBlt picChip.hDC, 0, 0, 512 - SX, 480 - SY, Stages.GetMapChipDC(EditLevel), SX, SY, Stages.GetMapChipMaskDC(EditLevel)
    picChip.Line (CX + 0, CY + 0)-(CX + 31, CY + 31), &HFFFFFF, B
    picChip.Line (CX + 1, CY + 1)-(CX + 30, CY + 30), &H0, B
    picChip.Line (CX + 2, CY + 2)-(CX + 29, CY + 29), &HFFFFFF, B
    picChip.Line (CX + 3, CY + 3)-(CX + 28, CY + 28), &H0, B
End Sub

Private Sub vscY_Change()
    picChip_Paint
End Sub

Private Sub vscY_Scroll()
    picChip_Paint
End Sub
