VERSION 5.00
Begin VB.Form FormWholeView 
   AutoRedraw      =   -1  'True
   BorderStyle     =   0  '‚È‚µ
   Caption         =   "‘S‘Ì‘œ"
   ClientHeight    =   5280
   ClientLeft      =   0
   ClientTop       =   0
   ClientWidth     =   7560
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   352
   ScaleMode       =   3  'Ëß¸¾Ù
   ScaleWidth      =   504
   ShowInTaskbar   =   0   'False
   StartUpPosition =   3  'Windows ‚ÌŠù’è’l
   WindowState     =   2  'Å‘å‰»
   Begin VB.PictureBox picBuf 
      AutoRedraw      =   -1  'True
      BackColor       =   &H00000000&
      BorderStyle     =   0  '‚È‚µ
      Height          =   7680
      Left            =   1440
      ScaleHeight     =   512
      ScaleMode       =   3  'Ëß¸¾Ù
      ScaleWidth      =   512
      TabIndex        =   0
      Top             =   1200
      Visible         =   0   'False
      Width           =   7680
   End
End
Attribute VB_Name = "FormWholeView"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public StageData As clsStage

Public Sub OnDraw()
    Dim X As Integer, Y As Integer
    Dim StartX As Integer, StartY As Integer, EndX As Integer, EndY As Integer
    Dim MX As Long, MY As Long
    Dim CX As Long, CY As Long
    Dim VX As Long, VY As Long
    Dim DestDC As Long, ChipDC As Long, MaskDC As Long
    Dim Size As Long
    Size = 32
    If Size * StageData.GetWidth(1) > ScaleWidth Then Size = ScaleWidth \ StageData.GetWidth(1)
    If Size * StageData.GetHeight(1) > ScaleHeight Then Size = ScaleHeight \ StageData.GetHeight(1)
    StartX = (ScaleWidth - Size * StageData.GetWidth(1)) \ 2
    StartY = (ScaleHeight - Size * StageData.GetHeight(1)) \ 2
    EndX = StageData.GetWidth(1) - 1: EndY = StageData.GetHeight(1) - 1
    ChipDC = Stages.GetMapChipDC(1)
    SetStretchBltMode picBuf.hdc, HALFTONE
    StretchBlt picBuf.hdc, 0, 0, Size * 16, Size * 16, ChipDC, 0, 0, 512, 512, SRCCOPY
    picBuf.Refresh
    ChipDC = picBuf.hdc
    Cls
    DestDC = hdc
    SetStretchBltMode DestDC, COLORONCOLOR
    For MX = 0 To EndX
        For MY = 0 To EndY
            CX = StageData.GetMapChip(1, MX, MY) Mod 16
            CY = StageData.GetMapChip(1, MX, MY) \ 16
            VX = StartX + MX * Size: VY = StartY + MY * Size
            If CY < 15 Then
                BitBlt DestDC, VX, VY, Size, Size, ChipDC, CX * Size, CY * Size, SRCCOPY
            Else
                BitBlt DestDC, VX, VY, Size, Size, ChipDC, 0, 0, SRCCOPY
                StretchBlt DestDC, VX, VY, Size, Size, MDIFormMain.picChara.hdc, CX * 32, 32, 32, 32, SRCAND
                StretchBlt DestDC, VX, VY, Size, Size, MDIFormMain.picChara.hdc, CX * 32, 0, 32, 32, SRCPAINT
            End If
        Next
    Next
    Refresh
End Sub

Private Sub Form_Activate()
    OnDraw
End Sub

Private Sub Form_Click()
    Unload Me
End Sub
