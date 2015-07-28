VERSION 5.00
Begin VB.MDIForm MDIFormMain 
   BackColor       =   &H8000000C&
   Caption         =   "ふにゃさんステージエディタ"
   ClientHeight    =   5340
   ClientLeft      =   165
   ClientTop       =   735
   ClientWidth     =   7050
   Icon            =   "MDIFormMain.frx":0000
   LinkTopic       =   "MDIForm1"
   StartUpPosition =   3  'Windows の既定値
   Begin VB.PictureBox picToolBar 
      Align           =   1  '上揃え
      BorderStyle     =   0  'なし
      Height          =   2415
      Left            =   0
      ScaleHeight     =   2415
      ScaleWidth      =   7050
      TabIndex        =   0
      Top             =   0
      Width           =   7050
      Begin VB.PictureBox picChipMask 
         AutoRedraw      =   -1  'True
         BackColor       =   &H00FFFFFF&
         Height          =   7260
         Index           =   2
         Left            =   6600
         ScaleHeight     =   480
         ScaleMode       =   3  'ﾋﾟｸｾﾙ
         ScaleWidth      =   512
         TabIndex        =   6
         Top             =   720
         Visible         =   0   'False
         Width           =   7740
      End
      Begin VB.PictureBox picChipMask 
         AutoRedraw      =   -1  'True
         BackColor       =   &H00FFFFFF&
         Height          =   7260
         Index           =   1
         Left            =   6480
         ScaleHeight     =   480
         ScaleMode       =   3  'ﾋﾟｸｾﾙ
         ScaleWidth      =   512
         TabIndex        =   7
         Top             =   600
         Visible         =   0   'False
         Width           =   7740
      End
      Begin VB.PictureBox picChipMask 
         AutoRedraw      =   -1  'True
         BackColor       =   &H00FFFFFF&
         Height          =   7260
         Index           =   0
         Left            =   6360
         ScaleHeight     =   480
         ScaleMode       =   3  'ﾋﾟｸｾﾙ
         ScaleWidth      =   512
         TabIndex        =   8
         Top             =   480
         Visible         =   0   'False
         Width           =   7740
      End
      Begin VB.PictureBox picChip 
         AutoRedraw      =   -1  'True
         BackColor       =   &H00000000&
         Height          =   7260
         Index           =   2
         Left            =   6240
         ScaleHeight     =   480
         ScaleMode       =   3  'ﾋﾟｸｾﾙ
         ScaleWidth      =   512
         TabIndex        =   5
         Top             =   360
         Visible         =   0   'False
         Width           =   7740
      End
      Begin VB.PictureBox picChip 
         AutoRedraw      =   -1  'True
         BackColor       =   &H00000000&
         Height          =   7260
         Index           =   1
         Left            =   6120
         ScaleHeight     =   480
         ScaleMode       =   3  'ﾋﾟｸｾﾙ
         ScaleWidth      =   512
         TabIndex        =   4
         Top             =   240
         Visible         =   0   'False
         Width           =   7740
      End
      Begin VB.PictureBox picChip 
         AutoRedraw      =   -1  'True
         BackColor       =   &H00000000&
         Height          =   7260
         Index           =   0
         Left            =   6000
         ScaleHeight     =   480
         ScaleMode       =   3  'ﾋﾟｸｾﾙ
         ScaleWidth      =   512
         TabIndex        =   3
         Top             =   120
         Visible         =   0   'False
         Width           =   7740
      End
      Begin VB.PictureBox PicChara 
         AutoRedraw      =   -1  'True
         AutoSize        =   -1  'True
         Height          =   1020
         Left            =   120
         Picture         =   "MDIFormMain.frx":030A
         ScaleHeight     =   960
         ScaleWidth      =   7680
         TabIndex        =   2
         Top             =   1200
         Visible         =   0   'False
         Width           =   7740
      End
      Begin VB.PictureBox picDefChip 
         AutoRedraw      =   -1  'True
         AutoSize        =   -1  'True
         Height          =   7260
         Left            =   120
         Picture         =   "MDIFormMain.frx":1834C
         ScaleHeight     =   7200
         ScaleWidth      =   7680
         TabIndex        =   1
         Top             =   600
         Visible         =   0   'False
         Width           =   7740
      End
   End
   Begin VB.Menu mnuFile 
      Caption         =   "ファイル"
      Begin VB.Menu mnuFileNew 
         Caption         =   "新規作成"
      End
      Begin VB.Menu mnuFileOpen 
         Caption         =   "開く"
      End
      Begin VB.Menu mnuFileSave 
         Caption         =   "保存"
      End
      Begin VB.Menu SSSSSSSSSSSSSSSSSSSSSSSSSSSSS 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileExit 
         Caption         =   "終了"
      End
   End
   Begin VB.Menu mnuView 
      Caption         =   "表示"
      WindowList      =   -1  'True
      Begin VB.Menu mnuViewInfo 
         Caption         =   "全般設定"
      End
      Begin VB.Menu mnuViewStage 
         Caption         =   "ステージ"
      End
      Begin VB.Menu mnuViewMapChip 
         Caption         =   "マップチップ"
      End
   End
   Begin VB.Menu mnuOption 
      Caption         =   "オプション"
      Begin VB.Menu mnuOptionViewHit 
         Caption         =   "テストプレイ時当たり判定を表示"
      End
   End
End
Attribute VB_Name = "MDIFormMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub MDIForm_Load()
    Set Stages = New clsStageFile

    EditLevel = 1
    Stages.CreateNewFile
    
    picToolBar.Height = 480
    picToolBar.Visible = False
End Sub

Private Sub mnuFileExit_Click()
    Unload Me
End Sub

Private Sub mnuFileNew_Click()
    CloseAllWindow
    Stages.CreateNewFile
End Sub

Private Sub mnuFileOpen_Click()
    CloseAllWindow
    FormLoad.Show vbModal, Me
End Sub

Private Sub mnuFileSave_Click()
    Stages.Save
End Sub

Private Sub mnuOptionViewHit_Click()
    mnuOptionViewHit.Checked = Not mnuOptionViewHit.Checked
End Sub

Private Sub mnuViewInfo_Click()
    mnuViewInfo.Checked = Not mnuViewInfo.Checked
    If mnuViewInfo.Checked Then
        FormStageInfo.Show
    Else
        Unload FormStageInfo
    End If
End Sub

Private Sub mnuViewMapChip_Click()
    mnuViewMapChip.Checked = Not mnuViewMapChip.Checked
    If mnuViewMapChip.Checked Then
        FormMapChip.Show vbModeless, Me
    Else
        Unload FormMapChip
    End If
End Sub

Private Sub mnuViewStage_Click()
    mnuViewStage.Checked = Not mnuViewStage.Checked
    If mnuViewStage.Checked Then
        FormStageList.Show
    Else
        Unload FormStageList
    End If
End Sub
