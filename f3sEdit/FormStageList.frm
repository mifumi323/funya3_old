VERSION 5.00
Begin VB.Form FormStageList 
   Caption         =   "ステージリスト"
   ClientHeight    =   3015
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   4680
   LinkTopic       =   "Form2"
   MDIChild        =   -1  'True
   ScaleHeight     =   3015
   ScaleWidth      =   4680
   Begin VB.CommandButton cmdDown 
      Caption         =   "下へ移動"
      Height          =   375
      Left            =   2400
      TabIndex        =   6
      Top             =   1080
      Width           =   1575
   End
   Begin VB.CommandButton cmdUp 
      Caption         =   "上へ移動"
      Height          =   375
      Left            =   2400
      TabIndex        =   5
      Top             =   600
      Width           =   1575
   End
   Begin VB.CommandButton cmdTestPlay 
      Caption         =   "テストプレイ"
      Height          =   375
      Left            =   2400
      TabIndex        =   4
      Top             =   2520
      Width           =   1575
   End
   Begin VB.CommandButton cmdDelete 
      Caption         =   "削除"
      Height          =   375
      Left            =   2400
      TabIndex        =   3
      Top             =   2040
      Width           =   1575
   End
   Begin VB.CommandButton cmdAdd 
      Caption         =   "追加"
      Height          =   375
      Left            =   2400
      TabIndex        =   2
      Top             =   1560
      Width           =   1575
   End
   Begin VB.CommandButton cmdEdit 
      Caption         =   "編集"
      Height          =   375
      Left            =   2400
      TabIndex        =   1
      Top             =   120
      Width           =   1575
   End
   Begin VB.ListBox lstStage 
      Height          =   2220
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   2175
   End
End
Attribute VB_Name = "FormStageList"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim DoubleClicked As Boolean

Private Sub cmdDown_Click()
    With lstStage
        If .ListIndex >= 0 And .ListIndex < .ListCount - 1 And .ListCount > 1 Then
            Stages.SwapStage .ListIndex, .ListIndex + 1
            .ListIndex = .ListIndex + 1
            RefreshStage
        End If
    End With
End Sub

Private Sub cmdUp_Click()
    With lstStage
        If .ListIndex >= 1 And .ListCount > 1 Then
            Stages.SwapStage .ListIndex, .ListIndex - 1
            .ListIndex = .ListIndex - 1
            RefreshStage
        End If
    End With
End Sub

Private Sub cmdAdd_Click()
    If Stages.AddStage Then
        lstStage.AddItem (lstStage.ListCount + 1) & ":"
        lstStage.ListIndex = lstStage.ListCount - 1
    End If
End Sub

Private Sub cmdDelete_Click()
    If lstStage.ListIndex >= 0 And lstStage.ListCount > 1 Then
        If MsgBox("削除したステージは元に戻せません。" & vbNewLine & "削除しますか？", vbYesNo) = vbYes Then
            Stages.RemoveStage lstStage.ListIndex
            lstStage.RemoveItem lstStage.ListIndex
            RefreshStage
        End If
    End If
End Sub

Private Sub cmdEdit_Click()
    Stages.ShowMapEdit CByte(lstStage.ListIndex And 255)
End Sub

Private Sub cmdTestPlay_Click()
    TestPlay lstStage.ListIndex
End Sub

Private Sub Form_Load()
    Dim I As Byte
    For I = 0 To Stages.GetStageCount - 1
        lstStage.AddItem (I + 1) & ":" & Stages.GetStageTitle(I)
    Next
    DoubleClicked = False
End Sub

Private Sub Form_Resize()
    On Error Resume Next
    lstStage.Move 120, 120, ScaleWidth - 360 - cmdAdd.Width, ScaleHeight - 240
    cmdAdd.Left = lstStage.Width + 240
    cmdUp.Left = lstStage.Width + 240
    cmdDown.Left = lstStage.Width + 240
    cmdDelete.Left = lstStage.Width + 240
    cmdEdit.Left = lstStage.Width + 240
    cmdTestPlay.Left = lstStage.Width + 240
End Sub

Private Sub Form_Unload(Cancel As Integer)
    MDIFormMain.mnuViewStage.Checked = False
End Sub

Public Sub RefreshStage()
    Dim I As Integer
    With lstStage
        For I = 0 To .ListCount - 1
            .List(I) = (I + 1) & ":" & Stages.GetStageTitle(CByte(I))
        Next
    End With
End Sub

Private Sub lstStage_DblClick()
    DoubleClicked = True    ' ダブルクリックの動作をボタンを離すまで遅延
End Sub

Private Sub lstStage_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If DoubleClicked And Button = vbLeftButton Then
        cmdEdit_Click
        DoubleClicked = False
    End If
End Sub
