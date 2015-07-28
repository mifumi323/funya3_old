Attribute VB_Name = "BitBlt_API"
'BitBlt
'グラフィックの転送などには必須のAPI
'ラスター操作でキャラクターの背景を透過させて転送する等
'VBゲームでの使用率はきわめて高い
'背景透過させるには、SRCANDでマスク画像を転送し、その後
'SRCINVERTでソース画像を転送すればいい。
Declare Sub BitBlt Lib "gdi32" (ByVal hDestDC As Long, ByVal X As Long, ByVal Y As Long, ByVal nWidth As Long, ByVal nHeight As Long, ByVal hSrcDC As Long, ByVal xSrc As Long, ByVal ySrc As Long, ByVal dwRop As Long)
Declare Sub StretchBlt Lib "gdi32" (ByVal hDestDC As Long, ByVal nDestLeft As Long, ByVal nDestTop As Long, ByVal nDestWidth As Long, ByVal nDestHeight As Long, ByVal hSrcDC As Long, ByVal nSrcLeft As Long, ByVal nSrcTop As Long, ByVal nSrcWidth As Long, ByVal nSrcHeight As Long, ByVal dwRop As Long)
Declare Sub SetStretchBltMode Lib "gdi32" (ByVal hdc As Long, ByVal iStretchMode As Long)

'dwRopに指定する定数
Public Const BLACKNESS = &H42
Public Const DSTINVERT = &H550009
Public Const MERGECOPY = &HC000CA
Public Const MERGEPAINT = &HBB0226
Public Const NOTSRCCOPY = &H330008
Public Const NOTSRCERASE = &H1100A6
Public Const PATCOPY = &HF00021
Public Const PATINVERT = &H5A0049
Public Const PATPAINT = &HFB0A09
Public Const SRCAND = &H8800C6
Public Const SRCCOPY = &HCC0020
Public Const SRCERASE = &H440328
Public Const SRCINVERT = &H660046
Public Const SRCPAINT = &HEE0086
Public Const WHITENESS = &HFF0062

Public Const BLACKONWHITE = 1
Public Const WHITEONBLACK = 2
Public Const COLORONCOLOR = 3
Public Const HALFTONE = 4

Public Declare Sub Rectangle Lib "gdi32" (ByVal hdc As Long, ByVal X1 As Long, ByVal Y1 As Long, ByVal X2 As Long, ByVal Y2 As Long)
