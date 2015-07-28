VERSION 5.00
Begin VB.Form FormStageInfo 
   Caption         =   "ステージ情報"
   ClientHeight    =   3090
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   MDIChild        =   -1  'True
   ScaleHeight     =   3090
   ScaleWidth      =   4680
   Begin VB.ListBox lstStageInfo 
      Height          =   420
      Left            =   0
      TabIndex        =   0
      Top             =   0
      Width           =   1215
   End
End
Attribute VB_Name = "FormStageInfo"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim HeaderList() As Long

Private Sub Form_Load()
    HeaderList = Stages.GetHeaderList()
    With lstStageInfo
        Dim I As Long
        For I = 0 To UBound(HeaderList)
            .AddItem GetString(I)
        Next
    End With
End Sub

Private Sub Form_Resize()
    lstStageInfo.Move 0, 0, ScaleWidth, ScaleHeight
End Sub

Private Sub Form_Unload(Cancel As Integer)
    MDIFormMain.mnuViewInfo.Checked = False
End Sub

Public Function GetString(I As Long) As String
    GetString = Stages.GetInfoName(HeaderList(I)) & " > " & Stages.GetInfo(HeaderList(I))
End Function

Private Sub lstStageInfo_DblClick()
    With lstStageInfo
        Dim I As Long, H As Long
        Dim msg As String
        I = .ListIndex
        H = HeaderList(I)
        msg = Stages.GetInfoName(H) & "を入力してください。" & vbNewLine & Stages.GetInfoDescription(H)
        If Not IsMissing(Stages.GetInfoMin(H)) Then
            msg = msg & vbNewLine & "(" & Stages.GetInfoMin(H) & "～" & Stages.GetInfoMax(H) & ")"
        End If
        Stages.SetInfo H, InputBoxEx(msg, "", Stages.GetInfo(H))
        .List(I) = GetString(I)
    End With
End Sub
