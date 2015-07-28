VERSION 5.00
Begin VB.Form FormInputBoxEx 
   Caption         =   "“ü—Í‚µ‚Ä‚­‚¾‚³‚¢"
   ClientHeight    =   1875
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   5535
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1875
   ScaleWidth      =   5535
   StartUpPosition =   1  'µ°Å° Ì«°Ñ‚Ì’†‰›
   Begin VB.TextBox txtInput 
      Height          =   615
      Index           =   1
      Left            =   120
      MultiLine       =   -1  'True
      ScrollBars      =   2  '‚’¼
      TabIndex        =   4
      ToolTipText     =   "Shift‚ð‰Ÿ‚µ‚È‚ª‚ç‰üs‚Å‚«‚Ü‚·‚æ"
      Top             =   1200
      Width           =   1575
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "·¬¾Ù"
      Height          =   255
      Left            =   3960
      TabIndex        =   3
      Top             =   480
      Width           =   975
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   255
      Left            =   3960
      TabIndex        =   2
      Top             =   120
      Width           =   975
   End
   Begin VB.TextBox txtInput 
      Height          =   270
      Index           =   0
      Left            =   120
      TabIndex        =   1
      Top             =   840
      Width           =   4815
   End
   Begin VB.Label lblMessage 
      AutoSize        =   -1  'True
      Height          =   180
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   3735
      WordWrap        =   -1  'True
   End
End
Attribute VB_Name = "FormInputBoxEx"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public m As Integer
Public target As Integer

Private Sub cmdCancel_Click()
    m = 3
End Sub

Private Sub cmdOK_Click()
    m = 2
End Sub

Private Sub Form_Activate()
    If m = 0 Then
        Unload Me
    Else
        Do
            DoEvents
        Loop While m = 1
        Hide
    End If
End Sub

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
    If UnloadMode = vbFormControlMenu Then
        Cancel = True
        m = 3
    End If
End Sub

Private Sub Form_Resize()
    On Error Resume Next
    lblMessage.Move 120, 120, ScaleWidth - cmdOK.Width - 360
    cmdOK.Left = ScaleWidth - cmdOK.Width - 120
    cmdCancel.Left = ScaleWidth - cmdCancel.Width - 120
    txtInput(0).Move 120, ScaleHeight - txtInput(0).Height - 120, ScaleWidth - 240
    txtInput(0).Visible = target = 0
    Dim t As Single
    t = IIf(120 + lblMessage.Height > cmdCancel.Top + cmdCancel.Height, lblMessage.Height + 240, cmdCancel.Top + cmdCancel.Height + 120)
    txtInput(1).Move 120, t, ScaleWidth - 240, ScaleHeight - t - 120
    txtInput(1).Visible = target = 1
End Sub

