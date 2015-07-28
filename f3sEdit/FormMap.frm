VERSION 5.00
Begin VB.Form FormMap 
   Caption         =   "マップ"
   ClientHeight    =   3090
   ClientLeft      =   60
   ClientTop       =   450
   ClientWidth     =   4680
   LinkTopic       =   "Form1"
   MDIChild        =   -1  'True
   ScaleHeight     =   3090
   ScaleWidth      =   4680
   Begin VB.CommandButton cmdOther 
      Caption         =   "その他"
      Height          =   255
      Left            =   3480
      TabIndex        =   6
      Top             =   1440
      Width           =   975
   End
   Begin VB.CommandButton cmdSize 
      Caption         =   "サイズ変更"
      Height          =   255
      Left            =   3000
      TabIndex        =   5
      Top             =   1080
      Width           =   1215
   End
   Begin VB.CommandButton cmdTitle 
      Caption         =   "タイトル"
      Height          =   255
      Left            =   2160
      TabIndex        =   4
      Top             =   600
      Width           =   1215
   End
   Begin VB.CommandButton cmdMapChip 
      Caption         =   "マップチップ"
      Height          =   255
      Left            =   1440
      TabIndex        =   3
      Top             =   120
      Width           =   1455
   End
   Begin VB.HScrollBar hscX 
      Height          =   255
      LargeChange     =   16
      Left            =   1680
      TabIndex        =   2
      Top             =   1440
      Width           =   1335
   End
   Begin VB.VScrollBar vscY 
      Height          =   1215
      LargeChange     =   16
      Left            =   2160
      TabIndex        =   1
      Top             =   960
      Width           =   255
   End
   Begin VB.PictureBox picMap 
      Appearance      =   0  'ﾌﾗｯﾄ
      AutoRedraw      =   -1  'True
      BackColor       =   &H00FF80FF&
      BorderStyle     =   0  'なし
      DrawStyle       =   6  '実線 (ふちどり)
      FillColor       =   &H00FF80FF&
      FillStyle       =   0  '塗りつぶし
      ForeColor       =   &H00FF80FF&
      Height          =   495
      Left            =   120
      ScaleHeight     =   33
      ScaleMode       =   3  'ﾋﾟｸｾﾙ
      ScaleWidth      =   81
      TabIndex        =   0
      Top             =   360
      Width           =   1215
   End
End
Attribute VB_Name = "FormMap"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Public StageData As clsStage
Dim Buttons As New clsButtonMenu
Dim MapMenu As New FormMapMenu

Public Layer0 As Boolean
Public Layer1 As Boolean
Public Layer2 As Boolean

Private Sub cmdMapChip_Click()
    MDIFormMain.mnuViewMapChip.Checked = True
    FormMapChip.Show vbModeless, MDIFormMain
End Sub

Private Sub cmdOther_Click()
    MapMenu.mnuLayer0.Checked = Layer0
    MapMenu.mnuLayer1.Checked = Layer1
    MapMenu.mnuLayer2.Checked = Layer2
    PopupMenu MapMenu.mnu, 0, cmdOther.Left, cmdOther.Top + cmdOther.Height
End Sub

Private Sub cmdOther_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
'    PopupMenu MapMenu.mnu, 0, cmdOther.Left, cmdOther.Top + cmdOther.Height
End Sub

Private Sub cmdSize_Click()
    On Error Resume Next
    Dim W As Long, h As Long
    W = CLng(Val(InputBoxEx("幅？(10～255)", "", StageData.GetWidth(EditLevel))))
    If W < 10 Then W = 10
    If W > 255 Then W = 255
    h = CLng(Val(InputBoxEx("高さ？(7～255)", "", StageData.GetHeight(EditLevel))))
    If h < 7 Then h = 7
    If h > 255 Then h = 255
    StageData.ResizeMap EditLevel, CByte(W), CByte(h), MsgBox("現状維持？", vbOKCancel) = vbOK
    hscX.Max = IIf(picMap.ScaleWidth < StageData.GetWidth(1) * 32, StageData.GetWidth(1) * 32 - picMap.ScaleWidth, 0)
    vscY.Max = IIf(picMap.ScaleHeight < StageData.GetHeight(1) * 32, StageData.GetHeight(1) * 32 - picMap.ScaleHeight, 0)
    OnDraw
End Sub

Private Sub cmdTitle_Click()
    Dim info As InputBoxInfo
    info.Prompt = "ステージ名を入力してください。"
    info.Title = "ふにゃさん"
    info.Default = Replace(StageData.GetTitle, "\n", vbNewLine)
    info.MultiLine = True
    StageData.SetTitle Replace(InputBoxEx2(info), vbNewLine, "\n")
End Sub

Private Sub Form_Load()
    Layer0 = True
    Layer1 = True
    Layer2 = True
    Buttons.Add cmdMapChip
    Buttons.Add cmdTitle
    Buttons.Add cmdSize
    Buttons.Add cmdOther
    Load MapMenu
    MapMenu.Visible = False
    Set MapMenu.Parent = Me
End Sub

Private Sub Form_Resize()
    On Error Resume Next
    If WindowState <> vbMinimized Then
        With cmdMapChip
            Buttons.Move 0, 0, ScaleWidth, .Height
            picMap.Move 0, .Height, ScaleWidth - vscY.Width, ScaleHeight - .Height - hscX.Height
            hscX.Move 0, .Height + picMap.Height, picMap.Width
            vscY.Move picMap.Width, .Height, vscY.Width, picMap.Height
            hscX.Max = IIf(picMap.ScaleWidth < StageData.GetWidth(1) * 32, StageData.GetWidth(1) * 32 - picMap.ScaleWidth, 0)
            vscY.Max = IIf(picMap.ScaleHeight < StageData.GetHeight(1) * 32, StageData.GetHeight(1) * 32 - picMap.ScaleHeight, 0)
        End With
    End If
End Sub

Private Sub Form_Unload(Cancel As Integer)
    StageData.HideMapEdit
    Unload MapMenu
End Sub

Private Sub hscX_Change()
    OnDraw
End Sub

Private Sub hscX_Scroll()
    OnDraw
End Sub

Private Sub picMap_KeyDown(KeyCode As Integer, Shift As Integer)
    If Shift = vbShiftMask Then
        Select Case KeyCode
        Case vbKeyLeft: StageData.ShiftMove EditLevel, -1, 0
        Case vbKeyRight: StageData.ShiftMove EditLevel, 1, 0
        Case vbKeyUp: StageData.ShiftMove EditLevel, 0, -1
        Case vbKeyDown: StageData.ShiftMove EditLevel, 0, 1
        End Select
        OnDraw
    End If
End Sub

Private Sub picMap_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    With StageData
        Dim MX As Integer, MY As Integer
        MX = (X + hscX.Value * .GetMagnificationX(EditLevel)) \ 32: MY = (Y + vscY.Value * .GetMagnificationY(EditLevel)) \ 32
        If Button = 1 Then
            If .SetMapChip(EditLevel, MX, MY, SelectedChip) Then OnDraw
        ElseIf Button = 2 Then
            If SelectedChip <> .GetMapChip(EditLevel, MX, MY) Then
                SelectedChip = .GetMapChip(EditLevel, MX, MY)
                If MDIFormMain.mnuViewMapChip.Checked Then
                    FormMapChip.picChip.Refresh
                    FormMapChip.picChara.Refresh
                End If
            End If
        End If
    End With
End Sub

Private Sub picMap_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    picMap_MouseDown Button, Shift, X, Y
End Sub

Public Sub OnDraw()
    With picMap
        Dim X As Integer, Y As Integer
        Dim StartX As Integer, StartY As Integer, EndX As Integer, EndY As Integer
        Dim MX As Long, MY As Long
        Dim CX As Long, CY As Long
        Dim VX As Long, VY As Long
        Dim DestDC As Long, ChipDC As Long, MaskDC As Long
        DestDC = picMap.hdc
        '下層
        ChipDC = Stages.GetMapChipDC(0)
        MaskDC = Stages.GetMapChipMaskDC(0)
        StartX = hscX.Value * StageData.GetMagnificationX(0): StartY = vscY.Value * StageData.GetMagnificationY(0)
        EndX = StartX + .ScaleWidth + 32: EndY = StartY + .ScaleHeight + 32
        For X = StartX To EndX Step 32
            For Y = StartY To EndY Step 32
                MX = X \ 32: MY = Y \ 32
                If MX < StageData.GetWidth(0) And MY < StageData.GetHeight(0) And Layer0 Then
                    CX = StageData.GetMapChip(0, MX, MY) Mod 16
                    CY = StageData.GetMapChip(0, MX, MY) \ 16
                    VX = MX * 32 - StartX
                    VY = MY * 32 - StartY
                    If CY < 15 Then
                        GIFBlt DestDC, VX, VY, 32, 32, ChipDC, CX * 32, CY * 32, MaskDC
                    End If
                Else
                    Rectangle DestDC, MX * 32 - StartX, MY * 32 - StartY, MX * 32 - StartX + 32, MY * 32 - StartY + 32
                End If
            Next
        Next
        '中層
        If Layer1 Then
            ChipDC = Stages.GetMapChipDC(1)
            MaskDC = Stages.GetMapChipMaskDC(1)
            StartX = hscX.Value: StartY = vscY.Value
            EndX = StartX + .ScaleWidth + 32: EndY = StartY + .ScaleHeight + 32
            For X = StartX To EndX Step 32
                For Y = StartY To EndY Step 32
                    MX = X \ 32: MY = Y \ 32
                    If MX < StageData.GetWidth(1) And MY < StageData.GetHeight(1) Then
                        CX = StageData.GetMapChip(1, MX, MY) Mod 16
                        CY = StageData.GetMapChip(1, MX, MY) \ 16
                        VX = MX * 32 - StartX
                        VY = MY * 32 - StartY
                        If CY < 15 Then
                            GIFBlt DestDC, VX, VY, 32, 32, ChipDC, CX * 32, CY * 32, MaskDC
                        Else
                            GIFBlt DestDC, VX, VY, 32, 32, ChipDC, 0, 0, MaskDC
                            BitBlt DestDC, VX, VY, 32, 32, MDIFormMain.picChara.hdc, CX * 32, 32, vbSrcAnd
                            BitBlt DestDC, VX, VY, 32, 32, MDIFormMain.picChara.hdc, CX * 32, 0, vbSrcPaint
                        End If
                    End If
                Next
            Next
        End If
        '上層
        If Layer2 Then
            ChipDC = Stages.GetMapChipDC(2)
            MaskDC = Stages.GetMapChipMaskDC(2)
            StartX = hscX.Value * StageData.GetMagnificationX(2): StartY = vscY.Value * StageData.GetMagnificationY(2)
            EndX = StartX + .ScaleWidth + 32: EndY = StartY + .ScaleHeight + 32
            For X = StartX To EndX Step 32
                For Y = StartY To EndY Step 32
                    MX = X \ 32: MY = Y \ 32
                    If MX < StageData.GetWidth(2) And MY < StageData.GetHeight(2) Then
                        CX = StageData.GetMapChip(2, MX, MY) Mod 16
                        CY = StageData.GetMapChip(2, MX, MY) \ 16
                        VX = MX * 32 - StartX
                        VY = MY * 32 - StartY
                        If CY < 15 Then
                            GIFBlt DestDC, VX, VY, 32, 32, ChipDC, CX * 32, CY * 32, MaskDC
                        End If
                    End If
                Next
            Next
        End If
        .Refresh
    End With
End Sub

Private Sub picMap_Resize()
    OnDraw
End Sub

Private Sub vscY_Change()
    OnDraw
End Sub

Private Sub vscY_Scroll()
    OnDraw
End Sub

Public Sub SetStageData(Data As clsStage)
    Set StageData = Data
    Caption = StageData.GetTitle
End Sub
