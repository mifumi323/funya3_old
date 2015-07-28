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
' 概要：    ホイールのイベントをつかまえるクラス
'
' 備考：    宣言文：        本クラスは、WithEventsをつけて、メンバ変数で宣言して使用します。
'                           ex) Private WithEvents m_CWheel As clsWheelMouse
'
'           初期化の手順：  Form_Loadにて本クラスのインスタンスを作成し、
'                           Initializeプロシージャを呼び出し、ボタンを捕まえたいコントロールのハンドルを渡す
'                           ex) Set m_CWheel = New clsWheelMouse
'                               Call m_CWheel.Initialize(picView.hWnd)
'
'           後始末の手順：  Form_UnloadにてTerminateプロシージャを呼び出し、インスタンスの削除を行う
'                           ex) m_CWheel.Terminate
'                               Set m_CWheel = Nothing
'

Option Explicit

' [イベント]
Public Event MouseWheel(ByVal iniVector As Integer, ByVal inShift As Integer)
    '' iniVector:   -1;ホイールが奥に回転した場合
    ''               1;ホイールが手前に回転した場合
    '' inShift:     KeyDownイベントのShiftと同様にvbShiftMask, vbCtrlMaskとビット判定して使う
    ''              ex) if (inShift And vbShiftMask) then MsgBox("Shiftキーが押されている")

' [変数]
Private m_OwnerhWnd As Long '' イベントを捕まえるフォーム、もしくはコントロール
Private m_Initlalized As Boolean    '' 初期化されたか?

Public Property Get OwnerhWnd() As Long
    OwnerhWnd = m_OwnerhWnd
End Property

Public Function Initialize(ByVal inlOwnerhWnd As Long) As Boolean
    
    Initialize = False
    
    '' ガード条件
    If m_Initlalized Then Exit Function
    If inlOwnerhWnd = 0 Then Exit Function
    
    m_OwnerhWnd = inlOwnerhWnd
    If Not WheelClassInitialise(Me, m_OwnerhWnd) Then Exit Function
    
    '' 初期化成功
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
    
    '' 終了処理が終わっていない場合には、終了処理を呼び出す
    Call Terminate

End Sub

Public Sub raiseMyEvent(ByVal iniVector As Integer, ByVal inShift As Integer)

    RaiseEvent MouseWheel(iniVector, inShift)

End Sub

