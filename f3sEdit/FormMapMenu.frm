VERSION 5.00
Begin VB.Form FormMapMenu 
   Caption         =   "�}�b�v���j���["
   ClientHeight    =   3090
   ClientLeft      =   165
   ClientTop       =   855
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   ScaleHeight     =   3090
   ScaleWidth      =   4680
   StartUpPosition =   3  'Windows �̊���l
   Visible         =   0   'False
   Begin VB.Menu mnu 
      Caption         =   "���j���["
      Begin VB.Menu mnuShiftRight 
         Caption         =   "�E�V�t�g"
      End
      Begin VB.Menu mnuShiftLeft 
         Caption         =   "���V�t�g"
      End
      Begin VB.Menu mnuShiftUp 
         Caption         =   "��V�t�g"
      End
      Begin VB.Menu mnuShiftDown 
         Caption         =   "���V�t�g"
      End
      Begin VB.Menu SSSAS 
         Caption         =   "-"
      End
      Begin VB.Menu mnuLayer0 
         Caption         =   "���w"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnuLayer1 
         Caption         =   "���w"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnuLayer2 
         Caption         =   "��w"
         Checked         =   -1  'True
      End
      Begin VB.Menu ASASASAS 
         Caption         =   "-"
      End
      Begin VB.Menu mnuWholeView 
         Caption         =   "�S�̑�"
      End
      Begin VB.Menu ASS 
         Caption         =   "-"
      End
      Begin VB.Menu mnuTestPlay 
         Caption         =   "�e�X�g�v���C"
      End
   End
End
Attribute VB_Name = "FormMapMenu"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public Parent As FormMap

Private Sub mnuLayer0_Click()
    Parent.Layer0 = Not mnuLayer0.Checked
    Parent.OnDraw
End Sub

Private Sub mnuLayer1_Click()
    Parent.Layer1 = Not mnuLayer1.Checked
    Parent.OnDraw
End Sub

Private Sub mnuLayer2_Click()
    Parent.Layer2 = Not mnuLayer2.Checked
    Parent.OnDraw
End Sub

Private Sub mnuShiftDown_Click()
    Parent.StageData.ShiftMove EditLevel, 0, 1
    Parent.OnDraw
End Sub

Private Sub mnuShiftLeft_Click()
    Parent.StageData.ShiftMove EditLevel, -1, 0
    Parent.OnDraw
End Sub

Private Sub mnuShiftRight_Click()
    Parent.StageData.ShiftMove EditLevel, 1, 0
    Parent.OnDraw
End Sub

Private Sub mnuShiftUp_Click()
    Parent.StageData.ShiftMove EditLevel, 0, -1
    Parent.OnDraw
End Sub

Private Sub mnuTestPlay_Click()
    TestPlay Stages.GetStageNum(Parent.StageData)
End Sub

Private Sub mnuWholeView_Click()
    Set FormWholeView.StageData = Parent.StageData
    FormWholeView.Show vbModal
End Sub
