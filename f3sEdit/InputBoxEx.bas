Attribute VB_Name = "modInputBoxEx"
' InputBoxEx
'  VBに標準で入っているInputBox関数は挙動がおかしいので自作したもの
'  ヘルプ関係が未対応だが、
'  入力文字数の制限や、キャンセルが押されたときの戻り値の設定もできる。
Option Explicit

Type InputBoxInfo
    Prompt As Variant
    Title As Variant
    Default As Variant
    XPos As Variant
    YPos As Variant
    HelpFile As Variant
    Context As Variant
    Length As Variant
    MultiLine As Variant
End Type

Public Function InputBoxEx(Prompt, Optional Title, Optional Default, Optional XPos, Optional YPos, Optional HelpFile, Optional Context, Optional Length) As String
    Dim info As InputBoxInfo
    With info
        .Prompt = Prompt
        If Not IsMissing(Title) Then .Title = Title
        If Not IsMissing(Default) Then .Default = Default
        If Not IsMissing(XPos) Then .XPos = XPos
        If Not IsMissing(YPos) Then .YPos = YPos
        If Not IsMissing(HelpFile) Then .HelpFile = HelpFile
        If Not IsMissing(Context) Then .Context = Context
        If Not IsMissing(Length) Then .Length = Length
        InputBoxEx = InputBoxEx2(info)
    End With
End Function

Public Function InputBoxEx2(info As InputBoxInfo) As String
    Dim d As String

    With FormInputBoxEx
        .lblMessage.Caption = CStr(info.Prompt)
        If Not IsEmpty(info.Title) Then .Caption = CStr(info.Title)
        If Not IsEmpty(info.Default) Then d = CStr(info.Default)
        If Not IsEmpty(info.XPos) Then .Left = CSng(info.XPos)
        If Not IsEmpty(info.YPos) Then .Top = CSng(info.YPos)
        If Not IsEmpty(info.MultiLine) Then .target = IIf(CBool(info.MultiLine), 1, 0)
        If Not IsEmpty(info.Length) Then .txtInput(.target).MaxLength = CInt(info.Length)
        .txtInput(.target).Text = d
        .m = 1
        .Show vbModal
        If .m = 2 Then InputBoxEx2 = .txtInput(.target).Text Else InputBoxEx2 = d
        .m = 0
        Unload FormInputBoxEx
    End With
End Function

