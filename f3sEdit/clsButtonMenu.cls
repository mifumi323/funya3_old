VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "clsButtonMenu"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Option Explicit

Dim m_Count As Integer
Dim m_Button() As CommandButton

Private Sub Class_Initialize()
    m_Count = 0
End Sub

Public Sub Add(B As CommandButton)
    ReDim Preserve m_Button(m_Count)
    Set m_Button(m_Count) = B
    m_Count = m_Count + 1
End Sub

Public Sub Move(Left As Single, Top As Single, Width As Single, Height As Single)
    ReDim Size(m_Count - 1) As Single
    Dim TotalSize As Single
    Dim Offset As Single
    Dim I As Integer
    For I = 0 To m_Count - 1
        Size(I) = Len(m_Button(I).Caption)
        TotalSize = TotalSize + Size(I)
    Next
    For I = 0 To m_Count - 1
        m_Button(I).Move Offset, Top, Width * Size(I) / TotalSize, Height
        Offset = Offset + Width * Size(I) / TotalSize
    Next
End Sub
