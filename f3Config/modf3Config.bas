Attribute VB_Name = "modf3Config"
Option Explicit
Public Declare Function GetCompressedStageData Lib "f3sEditDLL.dll" (ByRef lpData As Byte, ByVal dwDataSize As Long, ByVal dwPackSize As Long, ByVal dwType As Long, ByRef lpdwSize As Long) As Long
Public Declare Sub RtlMoveMemory Lib "kernel32" (Destination As Any, Source As Any, ByVal Length As Long)

Public Function GetFullFileName(fn As String) As String
    GetFullFileName = Replace(App.Path & "\" & fn, "\\", "\")
End Function

Public Function GetDataString(aIn() As Byte, Key As Long, Size As Long, PackSize As Long)
    Dim d() As Byte
    Dim Addr As Long
    Dim DataSize As Long
    Addr = GetCompressedStageData(aIn(0), Size, PackSize, Key, DataSize)
    If Addr Then
        ReDim d(DataSize - 1)
        RtlMoveMemory d(0), ByVal Addr, DataSize
        GetDataString = StrConv(d, vbUnicode)
    Else
        GetDataString = ""
    End If
End Function

