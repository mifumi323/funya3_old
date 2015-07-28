Attribute VB_Name = "modf3Config"
Option Explicit

Public Function GetFullFileName(fn As String) As String
    GetFullFileName = Replace(App.Path & "\" & fn, "\\", "\")
End Function
