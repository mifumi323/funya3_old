Attribute VB_Name = "modWheelMouse"
' @(s)
'
' �T�v�F    �z�C�[���W�����W���[���B�z�C�[���N���X�ƃZ�b�g�Ŏg�p
'

Option Explicit

Private Declare Function CallWindowProc Lib "user32" Alias "CallWindowProcA" _
    (ByVal lpPrevWndFunc As Long, ByVal hWnd As Long, ByVal Msg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
Public Declare Function GetWindowLong Lib "user32" Alias "GetWindowLongA" _
    (ByVal hWnd As Long, ByVal nIndex As Long) As Long
Public Declare Function SetWindowLong Lib "user32" Alias "SetWindowLongA" (ByVal hWnd As Long, ByVal nIndex As Long, ByVal dwNewLong As Long) As Long
Private Declare Function GetKeyState Lib "user32" _
    (ByVal nVirtKey As Long) As Integer


Public Const GWL_WNDPROC = (-4)
Public Const GWL_USERDATA = (-21)

Private Const WM_LBUTTONDOWN = &H201
Private Const WM_LBUTTONUP = &H202
Private Const WM_MBUTTONDOWN = &H207
Private Const WM_MBUTTONUP = &H208
Private Const WM_RBUTTONDOWN = &H204
Private Const WM_RBUTTONUP = &H205
Private Const WM_MOUSEWHEEL = &H20A

Private Const VK_SHIFT = &H10
Private Const VK_CONTROL = &H11

Private m_ColCWhell As Collection

Public Function WheelClassInitialise(ByRef inoWheel As clsWheelMouse, ByVal inlOwnerhWnd As Long) As Boolean
    Dim i As Long
    
    
    WheelClassInitialise = False
    
    If m_ColCWhell Is Nothing Then
        Set m_ColCWhell = New Collection
    End If
    
    '' �K�[�h�����F�n���h����0�̏ꍇ�͏I��
    If inlOwnerhWnd = 0 Then Exit Function
    '' �K�[�h�����F���ɓ����n���h�����o�^����Ă���ꍇ�ɂ́A�I��
    For i = 1 To m_ColCWhell.Count
        If m_ColCWhell(i).OwnerhWnd = inlOwnerhWnd Then
            Exit Function
        End If
    Next i
    
    m_ColCWhell.Add inoWheel
    
    SetWindowLong inlOwnerhWnd, GWL_USERDATA, SetWindowLong(inlOwnerhWnd, GWL_WNDPROC, AddressOf SubClassProc)
    
    WheelClassInitialise = True

End Function

Public Sub WheelClassTerminate(ByVal inlOwnerhWnd As Long)
    Dim i As Long
    
    
    If m_ColCWhell Is Nothing Then Exit Sub
    
    For i = 1 To m_ColCWhell.Count
        If m_ColCWhell(i).OwnerhWnd = inlOwnerhWnd Then
            m_ColCWhell.Remove i
            Exit For
        End If
    Next i

End Sub

Private Function SubClassProc(ByVal hwndx As Long, ByVal uMsg As Long, ByVal wParam As Long, ByVal lParam As Long) As Long
    Dim i As Long
    Dim Index As Long
    Dim zDelta As Integer
    Dim intShift As Integer
    
    
    If m_ColCWhell Is Nothing Then Exit Function
    If m_ColCWhell.Count = 0 Then Exit Function
    
    Static Calling As Boolean
    If Not Calling Then
        Index = 0
        For i = 1 To m_ColCWhell.Count
            If m_ColCWhell(i).OwnerhWnd = hwndx Then
                Index = i
                Exit For
            End If
        Next i
        If Index <> 0 Then
            Calling = True
            Select Case uMsg
            Case WM_MOUSEWHEEL  '' Wheel���񂵂�
                intShift = 0
                If (GetKeyState(VK_SHIFT) < 0) Then
                    intShift = intShift Or vbShiftMask
                End If
                If (GetKeyState(VK_CONTROL) < 0) Then
                    intShift = intShift Or vbCtrlMask
                End If
                
                zDelta = CInt(wParam / 2 ^ 16)
                If zDelta < 0 Then
                    '' �z�C�[�������ɉ�]�����ꍇ
                    m_ColCWhell(i).raiseMyEvent -1, intShift
                Else
                    '' �z�C�[������O�ɉ�]�����ꍇ
                    m_ColCWhell(i).raiseMyEvent 1, intShift
                End If
            End Select
        End If
        Calling = False
    End If
    SubClassProc = CallWindowProc(GetWindowLong(hwndx, GWL_USERDATA), hwndx, uMsg, wParam, lParam)
End Function

