VERSION 5.00
Begin VB.Form FormGraphic 
   BorderStyle     =   3  '固定ﾀﾞｲｱﾛｸﾞ
   Caption         =   "ファイルを開く"
   ClientHeight    =   3015
   ClientLeft      =   2760
   ClientTop       =   3750
   ClientWidth     =   4935
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3015
   ScaleWidth      =   4935
   ShowInTaskbar   =   0   'False
   Begin VB.HScrollBar HScroll1 
      Height          =   255
      LargeChange     =   32
      Left            =   2280
      TabIndex        =   5
      Top             =   2160
      Width           =   2295
   End
   Begin VB.VScrollBar VScroll1 
      Height          =   2055
      LargeChange     =   32
      Left            =   4560
      TabIndex        =   4
      Top             =   120
      Width           =   255
   End
   Begin VB.Frame Frame1 
      BorderStyle     =   0  'なし
      Height          =   2055
      Left            =   2280
      TabIndex        =   3
      Top             =   120
      Width           =   2295
      Begin VB.Image Image1 
         Height          =   735
         Left            =   0
         Top             =   0
         Width           =   1095
      End
   End
   Begin VB.FileListBox File1 
      Height          =   2790
      Left            =   120
      Pattern         =   "*.bmp;*.gif;*.jpg"
      TabIndex        =   2
      Top             =   120
      Width           =   2055
   End
   Begin VB.CommandButton CancelButton 
      Caption         =   "ｷｬﾝｾﾙ"
      Height          =   375
      Left            =   3600
      TabIndex        =   1
      Top             =   2520
      Width           =   1215
   End
   Begin VB.CommandButton OKButton 
      Caption         =   "OK"
      Height          =   375
      Left            =   2280
      TabIndex        =   0
      Top             =   2520
      Width           =   1215
   End
End
Attribute VB_Name = "FormGraphic"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public File As String

Private Sub CancelButton_Click()
    Hide
End Sub

Private Sub File1_Click()
    Image1.Picture = LoadPicture(File1.FileName)
End Sub

Private Sub File1_DblClick()
    OKButton_Click
End Sub

Private Sub Form_Load()
    HScroll1.Max = 32 * 16 - Frame1.Width / Screen.TwipsPerPixelX
    VScroll1.Max = 32 * 15 - Frame1.Height / Screen.TwipsPerPixelY
End Sub

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
    If UnloadMode <> VbMethod Then
        Hide
        Cancel = True
    End If
End Sub

Private Sub HScroll1_Change()
    Image1.Left = -HScroll1.Value * Screen.TwipsPerPixelX
End Sub

Private Sub HScroll1_Scroll()
    HScroll1_Change
End Sub

Private Sub OKButton_Click()
    If File1.ListIndex >= 0 Then
        File = File1.FileName
        Hide
    End If
End Sub

Private Sub VScroll1_Change()
    Image1.Top = -VScroll1.Value * Screen.TwipsPerPixelY
End Sub

Private Sub VScroll1_Scroll()
    VScroll1_Change
End Sub
