VERSION 5.00
Begin VB.Form FormMapChip 
   BorderStyle     =   5  '可変ﾂｰﾙ ｳｨﾝﾄﾞｳ
   Caption         =   "マップチップ"
   ClientHeight    =   3090
   ClientLeft      =   60
   ClientTop       =   615
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3090
   ScaleWidth      =   4680
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'ｵｰﾅｰ ﾌｫｰﾑの中央
   Begin VB.PictureBox picChara 
      Appearance      =   0  'ﾌﾗｯﾄ
      BackColor       =   &H00FF80FF&
      BorderStyle     =   0  'なし
      ForeColor       =   &H80000008&
      Height          =   480
      Left            =   0
      ScaleHeight     =   32
      ScaleMode       =   3  'ﾋﾟｸｾﾙ
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
      Appearance      =   0  'ﾌﾗｯﾄ
      BackColor       =   &H00FF80FF&
      BorderStyle     =   0  'なし
      ForeColor       =   &H80000008&
      Height          =   735
      Left            =   600
      ScaleHeight     =   49
      ScaleMode       =   3  'ﾋﾟｸｾﾙ
      ScaleWidth      =   89
      TabIndex        =   3
      Top             =   480
      Width           =   1335
   End
   Begin VB.CommandButton cmdReset 
      Caption         =   "標準に戻す"
      Height          =   495
      Left            =   3120
      TabIndex        =   2
      Top             =   1440
      Width           =   1215
   End
   Begin VB.CommandButton cmdLoad 
      Caption         =   "素材読み込み"
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
      Style           =   2  'ﾄﾞﾛｯﾌﾟﾀﾞｳﾝ ﾘｽﾄ
      TabIndex        =   0
      Top             =   1320
      Width           =   2055
   End
   Begin VB.Menu mnu 
      Caption         =   "メニュー"
      Begin VB.Menu mnuHit 
         Caption         =   "地面(上に乗れる)"
         Index           =   0
         Tag             =   "1"
      End
      Begin VB.Menu mnuHit 
         Caption         =   "天井(下から通り抜けられない)"
         Index           =   1
         Tag             =   "2"
      End
      Begin VB.Menu mnuHit 
         Caption         =   "左壁(左から通り抜けられない)"
         Index           =   2
         Tag             =   "4"
      End
      Begin VB.Menu mnuHit 
         Caption         =   "右壁(右から通り抜けられない)"
         Index           =   3
         Tag             =   "8"
      End
      Begin VB.Menu mnuHit 
         Caption         =   "とげ(触れるとミス)"
         Index           =   4
         Tag             =   "16"
      End
      Begin VB.Menu mnuS 
         Caption         =   "-"
      End
      Begin VB.Menu mnuF 
         Caption         =   "摩擦"
         Begin VB.Menu mnuFriction 
            Caption         =   "1.00 全く滑らない"
            Index           =   0
            Tag             =   "224"
         End
         Begin VB.Menu mnuFriction 
            Caption         =   "0.20 ほとんど滑らない"
            Index           =   1
            Tag             =   "192"
         End
         Begin VB.Menu mnuFriction 
            Caption         =   "0.15 普通より滑らない"
            Index           =   2
            Tag             =   "160"
         End
         Begin VB.Menu mnuFriction 
            Caption         =   "0.10 普通"
            Index           =   3
            Tag             =   "0"
         End
         Begin VB.Menu mnuFriction 
            Caption         =   "0.07 普通より滑る"
            Index           =   4
            Tag             =   "128"
         End
         Begin VB.Menu mnuFriction 
            Caption         =   "0.05 よく滑る"
            Index           =   5
            Tag             =   "96"
         End
         Begin VB.Menu mnuFriction 
            Caption         =   "0.02 すごく滑る"
            Index           =   6
            Tag             =   "64"
         End
         Begin VB.Menu mnuFriction 
            Caption         =   "0.00 摩擦なし"
            Index           =   7
            Tag             =   "32"
         End
      End
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
    mnu.Visible = False
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

Private Sub mnuFriction_Click(Index As Integer)
    Dim O
    For Each O In mnuFriction
        O.Checked = O.Index = Index
    Next
End Sub

Private Sub mnuHit_Click(Index As Integer)
    mnuHit(Index).Checked = Not mnuHit(Index).Checked
End Sub

Private Sub picChara_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    Dim SX As Integer, SY As Integer
    Dim CX As Integer, CY As Integer
    SX = hscX.Value: SY = vscY.Value
    CX = (X + SX) \ 32: CY = 15
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
        BitBlt picChara.hdc, I * 32 - SX, 0, 32, 32, Stages.GetMapChipDC(1), 0, 0, vbSrcCopy
    Next
    BitBlt picChara.hdc, 0, 0, 512 - SX, 32, MDIFormMain.picChara.hdc, SX, 32, vbSrcAnd
    BitBlt picChara.hdc, 0, 0, 512 - SX, 32, MDIFormMain.picChara.hdc, SX, 0, vbSrcPaint
    picChara.Line (CX + 0, CY + 0 - 480)-(CX + 31, CY + 31 - 480), &HFFFFFF, B
    picChara.Line (CX + 1, CY + 1 - 480)-(CX + 30, CY + 30 - 480), &H0, B
    picChara.Line (CX + 2, CY + 2 - 480)-(CX + 29, CY + 29 - 480), &HFFFFFF, B
    picChara.Line (CX + 3, CY + 3 - 480)-(CX + 28, CY + 28 - 480), &H0, B
End Sub

Private Sub picChip_DblClick()
    If EditLevel = 1 Then Stages.EditHit SelectedChip
End Sub

Private Sub picChip_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    Dim SX As Integer, SY As Integer
    Dim CX As Integer, CY As Integer
    SX = hscX.Value: SY = vscY.Value
    CX = (X + SX) \ 32: CY = (Y + SY) \ 32
    If CX < 16 And CY < 15 Then SelectedChip = CX + CY * 16
    picChip_Paint
    picChara_Paint
    If Button = vbRightButton And EditLevel = 1 Then Stages.EditHit SelectedChip, True
End Sub

Private Sub picChip_Paint()
    Dim SX As Long, SY As Long
    Dim CX As Long, CY As Long
    SX = hscX.Value: SY = vscY.Value
    CX = (SelectedChip Mod 16) * 32 - SX
    CY = (SelectedChip \ 16) * 32 - SY
    picChip.Cls
    GIFBlt picChip.hdc, 0, 0, 512 - SX, 480 - SY, Stages.GetMapChipDC(EditLevel), SX, SY, Stages.GetMapChipMaskDC(EditLevel)
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
