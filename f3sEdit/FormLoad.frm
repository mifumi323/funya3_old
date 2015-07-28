VERSION 5.00
Begin VB.Form FormLoad 
   Caption         =   "ステージを開く"
   ClientHeight    =   3090
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3090
   ScaleWidth      =   4680
   StartUpPosition =   1  'ｵｰﾅｰ ﾌｫｰﾑの中央
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "キャンセル"
      Height          =   255
      Left            =   2160
      TabIndex        =   3
      Top             =   1560
      Width           =   1215
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   255
      Left            =   840
      TabIndex        =   2
      Top             =   1560
      Width           =   1215
   End
   Begin VB.FileListBox filStage 
      Height          =   450
      Left            =   3240
      Pattern         =   "*.f3d"
      TabIndex        =   1
      Top             =   120
      Visible         =   0   'False
      Width           =   1215
   End
   Begin VB.ListBox lstStage 
      Height          =   420
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   1215
   End
End
Attribute VB_Name = "FormLoad"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub cmdCancel_Click()
    Unload Me
End Sub

Private Sub cmdOK_Click()
    If lstStage.ListIndex >= 0 Then
        Stages.LoadStage filStage.List(lstStage.ListIndex)
        Unload Me
    End If
End Sub

Private Sub Form_Load()
    Dim I As Integer
    Dim fn As String
    For I = 0 To filStage.ListCount - 1
        fn = filStage.List(I)
        lstStage.AddItem GetStageName(fn) & "(" & Left(fn, Len(fn) - 4) & ")"
    Next
End Sub

Private Function GetStageName(fn As String)
    GetStageName = ""
End Function

Private Sub Form_Resize()
    On Error Resume Next
    With cmdOK
        .Move 120, ScaleHeight - .Height - 120, ScaleWidth / 2 - 180
        cmdCancel.Move ScaleWidth / 2 + 60, .Top, .Width
        lstStage.Move 120, 120, ScaleWidth - 240, .Top - 240
    End With
End Sub

Private Sub lstStage_DblClick()
    cmdOK_Click
End Sub
