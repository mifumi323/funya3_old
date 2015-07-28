VERSION 5.00
Begin VB.Form FormHit 
   BorderStyle     =   3  '固定ﾀﾞｲｱﾛｸﾞ
   Caption         =   "当たり判定"
   ClientHeight    =   1935
   ClientLeft      =   2760
   ClientTop       =   3750
   ClientWidth     =   6135
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1935
   ScaleWidth      =   6135
   ShowInTaskbar   =   0   'False
   Begin VB.ListBox lstFriction 
      Height          =   1500
      ItemData        =   "FormHit.frx":0000
      Left            =   2880
      List            =   "FormHit.frx":001C
      TabIndex        =   7
      Top             =   360
      Width           =   1815
   End
   Begin VB.CheckBox chkDeath 
      Caption         =   "とげ(触れるとミス)"
      Height          =   255
      Left            =   120
      TabIndex        =   6
      Top             =   1560
      Width           =   2655
   End
   Begin VB.CheckBox chkRight 
      Caption         =   "右壁(右から通り抜けられない)"
      Height          =   255
      Left            =   120
      TabIndex        =   5
      Top             =   1200
      Width           =   2655
   End
   Begin VB.CheckBox chkLeft 
      Caption         =   "左壁(左から通り抜けられない)"
      Height          =   255
      Left            =   120
      TabIndex        =   4
      Top             =   840
      Width           =   2655
   End
   Begin VB.CheckBox chkBottom 
      Caption         =   "天井(下から通り抜けられない)"
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   480
      Width           =   2655
   End
   Begin VB.CheckBox chkTop 
      Caption         =   "地面(上に乗れる)"
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   120
      Width           =   2655
   End
   Begin VB.CommandButton CancelButton 
      Caption         =   "ｷｬﾝｾﾙ"
      Height          =   375
      Left            =   4800
      TabIndex        =   1
      Top             =   600
      Width           =   1215
   End
   Begin VB.CommandButton OKButton 
      Caption         =   "OK"
      Height          =   375
      Left            =   4800
      TabIndex        =   0
      Top             =   120
      Width           =   1215
   End
   Begin VB.Label lblFriction 
      Caption         =   "摩擦"
      Height          =   255
      Left            =   2880
      TabIndex        =   8
      Top             =   120
      Width           =   1095
   End
End
Attribute VB_Name = "FormHit"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public Hit As Byte

Public Sub SetHit(h As Byte)
    Hit = h
    If h And 1 Then chkTop.Value = 1
    If h And 2 Then chkBottom.Value = 1
    If h And 4 Then chkLeft.Value = 1
    If h And 8 Then chkRight.Value = 1
    If h And 16 Then chkDeath.Value = 1
    Select Case h \ 32
    Case 0: lstFriction.ListIndex = 3
    Case 1: lstFriction.ListIndex = 7
    Case 2: lstFriction.ListIndex = 6
    Case 3: lstFriction.ListIndex = 5
    Case 4: lstFriction.ListIndex = 4
    Case 5: lstFriction.ListIndex = 2
    Case 6: lstFriction.ListIndex = 1
    Case 7: lstFriction.ListIndex = 0
    End Select
End Sub

Private Sub CancelButton_Click()
    Hide
End Sub

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
    If UnloadMode <> VbMethod Then
        Hide
        Cancel = True
    End If
End Sub

Private Sub OKButton_Click()
    Hit = chkTop.Value Or chkBottom.Value * 2 Or chkLeft.Value * 4 Or chkRight.Value * 8 Or chkDeath.Value * 16
    Select Case lstFriction.ListIndex
    Case 0: Hit = Hit Or 7 * 32
    Case 1: Hit = Hit Or 6 * 32
    Case 2: Hit = Hit Or 5 * 32
    Case 3: Hit = Hit Or 0 * 32
    Case 4: Hit = Hit Or 4 * 32
    Case 5: Hit = Hit Or 3 * 32
    Case 6: Hit = Hit Or 2 * 32
    Case 7: Hit = Hit Or 1 * 32
    End Select
    Hide
End Sub
