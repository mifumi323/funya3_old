VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "clsWheelMouse"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
'@(s)
'
' �T�v�F    �z�C�[���̃C�x���g�����܂���N���X
'
' ���l�F    �錾���F        �{�N���X�́AWithEvents�����āA�����o�ϐ��Ő錾���Ďg�p���܂��B
'                           ex) Private WithEvents m_CWheel As clsWheelMouse
'
'           �������̎菇�F  Form_Load�ɂĖ{�N���X�̃C���X�^���X���쐬���A
'                           Initialize�v���V�[�W�����Ăяo���A�{�^����߂܂������R���g���[���̃n���h����n��
'                           ex) Set m_CWheel = New clsWheelMouse
'                               Call m_CWheel.Initialize(picView.hWnd)
'
'           ��n���̎菇�F  Form_Unload�ɂ�Terminate�v���V�[�W�����Ăяo���A�C���X�^���X�̍폜���s��
'                           ex) m_CWheel.Terminate
'                               Set m_CWheel = Nothing
'

Option Explicit

' [�C�x���g]
Public Event MouseWheel(ByVal iniVector As Integer, ByVal inShift As Integer)
    '' iniVector:   -1;�z�C�[�������ɉ�]�����ꍇ
    ''               1;�z�C�[������O�ɉ�]�����ꍇ
    '' inShift:     KeyDown�C�x���g��Shift�Ɠ��l��vbShiftMask, vbCtrlMask�ƃr�b�g���肵�Ďg��
    ''              ex) if (inShift And vbShiftMask) then MsgBox("Shift�L�[��������Ă���")

' [�ϐ�]
Private m_OwnerhWnd As Long '' �C�x���g��߂܂���t�H�[���A�������̓R���g���[��
Private m_Initlalized As Boolean    '' ���������ꂽ��?

Public Property Get OwnerhWnd() As Long
    OwnerhWnd = m_OwnerhWnd
End Property

Public Function Initialize(ByVal inlOwnerhWnd As Long) As Boolean
    
    Initialize = False
    
    '' �K�[�h����
    If m_Initlalized Then Exit Function
    If inlOwnerhWnd = 0 Then Exit Function
    
    m_OwnerhWnd = inlOwnerhWnd
    If Not WheelClassInitialise(Me, m_OwnerhWnd) Then Exit Function
    
    '' ����������
    m_Initlalized = True
    Initialize = True
    
End Function

Public Function Terminate()

    Call WheelClassTerminate(m_OwnerhWnd)
    SetWindowLong m_OwnerhWnd, GWL_WNDPROC, GetWindowLong(m_OwnerhWnd, GWL_USERDATA)
    m_Initlalized = False

End Function

Private Sub Class_Initialize()
    
    m_OwnerhWnd = 0&
    m_Initlalized = False

End Sub

Private Sub Class_Terminate()
    
    If Not m_Initlalized Then Exit Sub
    
    '' �I���������I����Ă��Ȃ��ꍇ�ɂ́A�I���������Ăяo��
    Call Terminate

End Sub

Public Sub raiseMyEvent(ByVal iniVector As Integer, ByVal inShift As Integer)

    RaiseEvent MouseWheel(iniVector, inShift)

End Sub
