;---------------------------------------------------------------
;		Internal Viewer
;
;		Version:	02.06.2000
;---------------------------------------------------------------

ViewData	STRUC
VwHandle	DW	?		; File handle
VwBase		DW	?		; View window base
VwSize		DW	?		; View window size
VwStatus	DW	?		; Status line procedure
VwSpace		DW	?		; Buffer size
VwLoaded	DW	?		; Bytes loaded in buffer
VwFirst		DW	?		; Offset of first line of screen
VwFirst1	DW	?		; Offset of first line of screen (copy)
VwColText	DW	?		; First column in text mode
VwColHex	DW	?		; First column in hex mode
VwHexSize	DW	?		; Bytes per line in hex mode
VwLen		DD	?		; Size of file
VwOffset	DD	?		; Offset of first loaded byte in buffer
VwTextHex	DB	?		; Text / Hex mode
VwWrap		DB	?		; Wrap / Unwrap
VwShift		DB	?		; Shift 1 byte in DBCS mode
Vw100pc		DB	?		; 100% displayed
VwSrTextHex	DB	?		; String was found in Text/Hex mode
VwSrLen		DW	?		; Length of the found string
VwSearch	DD	?		; Saerch offset
VwTbl		TblData	?		; Translation table
VwBuffer	LABEL	BYTE		; View data
ViewData	ENDS

		.CONST
ReadFile_S	DB	"Reading the file",0
Search_S	DB	"Search for the string",0
SearchCase_S	DB	"[ ] Case sensitive",0
SearchCancel_S	DB	"[ Search ]",0FFh,3,"[ Cancel ]",0
SearchFor_S	DB	"Searching for",0
SearchError_S	DB	"Could not find the string",0
GotoOffset_S	DB	"Go to offset",0
GotoOffsErr_S	DB	"Invalid hexadecimal offset",0
ViewStat1_S	DB	"^3C: ^1E",0FFh,127,0FFh,127,0
ViewStat2_S	DB	"Col^0L21> Byte^0Q /s/",0FFh,6,"%",0
PlainText_S	DB	"Plain text",0

		.DATA
ViewMain_T	DW	000Fh, V01_00		; Ctrl-0
		DW	001Bh, V02_00		; Esc
		DW	3D00h, V02_00		; F3
		DW	4400h, V02_00		; F10
		DW	0
View1_T		DW	3E00h, V11_00		; F4
		DW	4200h, V13_00		; F8
		DW	5B00h, V14_00		; Shift-F8
		DW	4B00h, V15_00		; Left
		DW	4D00h, V17_00		; Right
		DW	4800h, V19_00		; Up
		DW	5000h, V20_00		; Down
		DW	4900h, V21_00		; PgUp
		DW	5100h, V22_00		; PgDn
		DW	4700h, V23_00		; Home
		DW	8400h, V23_00		; Ctrl-PgUp
		DW	4F00h, V24_00		; End
		DW	7600h, V24_00		; Ctrl-PgDn
		DW	3F00h, V25_00		; F5
		DW	4100h, V27_00		; F7
		DW	5A00h, V28_00		; Shift-F7
		DW	6400h, V29_00		; Ctrl-F7
		DW	4000h, V10_21		; F6
		DW	5800h, V10_21		; Shift-F5
		DW	5900h, V10_21		; Shift-F6
		DW	6200h, V10_21		; Ctrl-F5
		DW	6300h, V10_21		; Ctrl-F6
		DW	6500h, V10_21		; Ctrl-F8
		DW	0
View2_T		DW	1C0Dh, V11_00		; Enter
		DW	3C00h, V12_00		; F2
		DW	0F09h, V14_00		; Tab
		DW	0013h, V15_00		; Ctrl-S
		DW	7300h, V16_00		; Ctlr-Left
		DW	0001h, V16_00		; Ctrl-A
		DW	0004h, V17_00		; Ctrl-D
		DW	7400h, V18_00		; Ctrl-Right
		DW	0006h, V18_00		; Ctrl-F
		DW	0005h, V19_00		; Ctrl-E
		DW	0018h, V20_00		; Ctrl-X
		DW	0012h, V21_00		; Ctrl-R
		DW	0003h, V22_00		; Ctrl-C
		DW	7700h, V23_00		; Ctrl-Home
		DW	7500h, V24_00		; Ctrl-End
		DW	0011h, V26_00		; Ctrl-Q
		DW	0
ReadFile_P	DB	Box_C, 0, 0, 0
		DW	Inc0_S
		DW	ReadFile_S
		DW	Line1_S
		DW	0
GotoOffset_P	DB	Box_C, BoxCu_C, 0, 0
		DW	Inc0_S
		DW	GotoOffset_S
		DW	Empty_S
		DW	0
GotoOffset_B	BOXHD	< GotoOffset_P, 500h, V25_30, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 1, 0,0,0,0, 305h, GotoOffset, 80h+8 >
		DB	0
GotoOffsErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	GotoOffsErr_S
		DW	Ok_S
		DW	0
GotoOffsErr_B	BOXHD	< GotoOffsErr_P, 900h, 0, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 3FEh, OkMn_D, 0 >
		DB	0
Search_P	DB	Box_C, BoxCu_C, Box_C, 0
		DW	Inc0_S
		DW	Inc1l_S
		DW	Empty_S
		DW	0FFFFh
		DW	Inc2l_S
		DW	0FFFFh
		DW	SearchCancel_S
		DW	0
Search_B	BOXHD	< Search_P, 500h, V27_20, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 1, 2,1,0,0, 305h, SrchStr, LenStrFull+LenStr >
		BOXMN	< 3, 0,2,1,1, 506h, SrchCase, 0 >
		BOXMN	< 4, 1,0,2,2, 7F4h, Search_D, 0 >
		DB	0
SearchFor_P	DB	Box_C, 0, 0, 0
		DW	Inc0_S
		DW	SearchFor_S
		DW	Quote2_S
		DW	0
SearchError_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	SearchError_S
		DW	Quote2_S
		DW	Ok_S
		DW	0
SearchError_B	BOXHD	< SearchError_P, 900h, 0, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
Status_P	DB	Menu_C
ViewStat_T	DW	TmpBuf
ViewGoto_T	DW	View_S
ViewSearch_T	DW	View_S, Search_S, SearchCase_S
ViewSearching_T	DW	View_S, 0, SrchStr
Search_D	DB	10, 0
		DB	10,13
		DB	0

		.CODE
;---------------------------------------------------------------
;	Load file and init variables
;		DS:BX - translation table
;		   AL - text / hex mode
;	Return:	   DS - view buffer
;---------------------------------------------------------------
ViewLoad	PROC	FAR
		PUSH	ES
		PUSH	DS
		PUSH	BX
		PUSH	AX
		PUSH	ES
		POP	DS
		MOV	BX,500h
		MOV	DX,OFFSET ReadView_T
		MOV	BP,OFFSET ReadFile_P
		CALL	DialBox
		CALL	Window
		MOV	SI,OFFSET SourceFile
		CMP	DS:ArcPathName,0
		JE	@@Init
		MOV	DI,OFFSET TmpBuf
		MOV	SI,OFFSET VcTempArc_S
		MOV	AX,DS:TempDirNumb
		PUSH	DI
		CALL	MakePathName
		CALL	AddFile
		MOV	SI,OFFSET SourceFile
		CALL	CutPath
		CALL	MvzLine
		POP	SI
  @@Init:	MOV	DL,DS:Columns
		MOV	DH,DS:Lines
		SUB	DH,2
		MOV	BX,100h
		MOV	BP,0FFFh
		MOV	AH,0
		CALL	ViewInit
		PUSH	DS
		POP	ES
		POP	AX
		POP	SI
		POP	DS
		JC	@@Restore
		MOV	ES:VwTextHex,AL
		MOV	AX,DS
		OR	AX,AX
		JE	@@Restore
		MOV	DI,OFFSET VwTbl
		MOV	CX,SIZE TblData
		REP	MOVSB
  @@Restore:	PUSH	ES
		POP	DS
		POP	ES
		PUSHF
		CALL	ResWin
		POPF
		JNC	@@Exit
		CALL	Window
		STC
  @@Exit:	RET
ViewLoad	ENDP

;---------------------------------------------------------------
;	View file
;		   DS - view buffer
;		DI SI - offset
;---------------------------------------------------------------
ViewFile	PROC	FAR
		MOV	AX,100h
		XCHG	AL,ES:Clock
		XCHG	AH,ES:KeyBar
		PUSH	AX
		PUSH	DI
		PUSH	SI
		MOV	DS:VwStatus,OFFSET ViewStat0 ; Init status line
		PUSH	DS
		PUSH	DS:VwLen.Hi
		PUSH	DS:VwLen.Lo
		CMP	DS:VwHandle,0FFFEh
		PUSH	ES
		POP	DS
		MOV	DS:HlpPage,ViewKeys_H
		MOV	SI,OFFSET SourceFile
		JB	V00_06
		CALL	CutPath
V00_06:		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		CALL	MvzLine
		POP	SI
		MOV	AL,DS:Columns
		MOV	AH,0
		PUSH	AX
		SUB	AL,47
		CALL	ShrtLin
		POP	AX
		MOV	DS,CS:SubsSeg
		MOV	BP,OFFSET Status_P
		XOR	BX,BX
		MOV	DX,OFFSET ViewFile_T
		MOV	SI,OFFSET ViewStat1_S
		CALL	NgLine
		POP	ES:TmpBuf.Lo
		POP	ES:TmpBuf.Hi
		MOV	BX,AX
		SUB	BX,38
		MOV	DX,OFFSET ViewStat_T
		MOV	SI,OFFSET ViewStat2_S
		CALL	NgLine
		POP	DS
		POP	SI
		POP	BX
		MOV	AX,SI			; Set position
		AND	AX,BX
		INC	AX
		JZ	V00_07
		PUSH	SI
		MOV	AL,DS:VwTextHex
		MOV	SI,OFFSET VwTbl
		CALL	PrepareSearch
		POP	SI
		CALL	V28_50
		CALL	V10_10
		JMP	V00_10
V00_07:		CALL	ViewStatus
		CALL	ViewScreen

V00_10:		MOV	DI,OFFSET MouseCur	; Input
		XOR	AX,AX
		STOSW
		MOV	AX,102h
		STOSW
		MOV	AL,ES:Pointers[6]
		MOV	AH,0FFh
		STOSW
		MOV	AL,ViewFile_K
		CMP	DS:VwTextHex,0
		JNZ	V00_11
		INC	AX
		CMP	DS:VwWrap,0
		JZ	V00_11
		INC	AX
V00_11:		CMP	DS:VwTbl.Translate,OFFSET TransDBCS
		JNE	V00_12
		ADD	AL,3
V00_12:		CMP	DS:VwHandle,0FFFEh
		JAE	V00_13
		ADD	AL,6
V00_13:		CALL	Input
		MOV	ES:MouseCur,0FFh
		CMP	AH,0FCh			; Mouse
		JNE	V00_14
		CALL	GetMous
		JZ	V00_10
		CMP	CX,2
		JB	V00_16
		MOV	AH,0FCh
		MOV	BX,DS:VwBase
		MOV	DX,DS:VwSize
		CALL	InWind
		JNC	V00_15
		CALL	LeaveTm
		JMP	V00_10
V00_14:		CMP	ES:Quote,0
		JNZ	V00_15
		MOV	SI,OFFSET ViewMain_T
		CALL	Case
V00_15:		CALL	ViewMain
		JMP	V00_10
V00_16:		CALL	MouseCtrlO
		JNC	V00_10

V01_00:		XOR	BX,BX			; ^O - user screen
		MOV	DL,ES:Columns
		MOV	DH,1
		CALL	SavWin
		MOV	DH,ES:Lines
		CALL	ResUser
		CALL	PutPrompt
		CALL	Window
		XOR	AX,AX
		MOV	ES:KeyBar,AL
		MOV	DI,OFFSET MouseCur
		STOSW
		MOV	AX,102h
		STOSW
		MOV	AL,ES:Pointers[6]
		MOV	AH,0FFh
		STOSW
V01_01:		MOV	AL,Empty_K
		CALL	Input
		CMP	AH,0FCh
		JNE	V01_03
V01_02:		CALL	GetMous
		JZ	V01_01
		CMP	CX,2
		JAE	V01_02
		CALL	MouseCtrlO
		JNC	V01_01
V01_03:		MOV	ES:KeyBar,1
		MOV	ES:BarPrev,0FFh
		MOV	ES:MouseCur,0FFh
		CALL	ResWin
		CALL	Window
		JMP	V00_07

V02_00:		MOV	BX,DS:VwHandle		; Exit
		CMP	BX,0FFFEh
		JAE	V02_01
		MOV	AH,3Eh
		CALL	Intr21
V02_01:		POP	AX
		MOV	ES:Clock,AL
		MOV	ES:KeyBar,AH
		RET
ViewFile	ENDP

;---------------------------------------------------------------
;	View - main module
;		   DS - view segment
;		   AX - keystroke
;	Return:	   AX - keystroke or 0
;---------------------------------------------------------------
ViewMain	PROC	NEAR
		MOV	ES:MouseWin,0
		MOV	ES:MouseCmd,0
		CALL	ClrKbd
		CMP	AH,0FCh
		JNE	V10_04
V10_00:		CALL	GetMous			; Mouse
		JZ	V10_21
		MOV	BX,DS:VwBase
		MOV	DX,DS:VwSize
		CMP	ES:MouseWin,0
		JNZ	V10_01
		CALL	InWind
		JC	V10_02
V10_01:		MOV	ES:MouseWin,1
		PUSH	BX
		MOV	AL,DH
		CBW
		MOV	BL,3
		DIV	BL
		MOV	SI,4800h
		ADD	BH,AL
		CMP	CH,BH
		POP	BX
		JB	V10_03
		MOV	SI,5000h
		ADD	BH,DH
		SUB	BH,AL
		CMP	CH,BH
		JAE	V10_03
		MOV	AL,DL
		SHR	AL,1
		SHR	AL,1
		MOV	SI,4B00h
		ADD	BL,AL
		CMP	CL,BL
		JB	V10_03
		MOV	SI,4D00h
		SUB	BL,AL
		ADD	BL,DL
		SUB	BL,AL
		CMP	CL,BL
		JAE	V10_03
		MOV	ES:MouseCmd,0
V10_02:		CALL	LeaveTm
		JMP	V10_20
V10_03:		CALL	MouseRepeat
		JNC	V10_20
		MOV	AX,SI
V10_04:		OR	AH,AH			; Commands
		JE	V10_05
		CMP	ES:Quote,0
		JZ	V10_06
V10_05:		MOV	ES:Quote,0
		PUSH	AX
		MOV	AL,' '
		CALL	V26_10
		POP	AX
		CMP	AL,0
		JZ	V10_21
		JMP	V27_10
V10_06:		MOV	SI,OFFSET View1_T
		CMP	DS:VwSize.Y,0
		JNE	V10_09
		OR	AH,AH
		JE	V10_22
		PUSH	DS
		MOV	DS,CS:SubsSeg
V10_07:		MOV	CX,DS:[SI]
		ADD	SI,4
		JCXZ	V10_08
		CMP	AL,CL
		JNE	V10_07
		OR	CH,CH
		JE	V10_08
		CMP	AH,CH
		JNE	V10_07
V10_08:		POP	DS
		JE	V10_21
		JMP	V10_22
V10_09:		CALL	Case
		MOV	CL,DS:VwSize.X
		CMP	CL,ES:Columns
		JB	V10_22
		MOV	SI,OFFSET View2_T
		CALL	Case
		CMP	AL,' '
		JB	V10_22
		JMP	V27_10
V10_10:		CALL	ViewStatus		; Output screen
		CALL	ViewScreen
		JNC	V10_20
V10_11:		CALL	ViewChk100
		JNC	V10_20
		CALL	ViewReadMore
		JNC	V10_12
		CALL	ViewScreen
		JNC	V10_20
		CALL	ViewChk100
		JNC	V10_20
V10_12:		MOV	AX,DS:VwSpace
		SHR	AX,1
		CMP	AX,DS:VwFirst
		JA	V10_20
		CALL	ViewReadNext
		MOV	SI,DS:VwFirst
		CALL	ViewBegLine
		MOV	DS:VwFirst,SI
		MOV	DS:VwFirst1,0FFFFh
		CALL	ViewScreen
		JNC	V10_20
		CALL	ViewChk100
V10_20:		CMP	ES:MouseWin,0
		JNZ	V10_00
V10_21:		XOR	AX,AX
V10_22:		MOV	ES:MouseWin,0
		MOV	ES:MouseCmd,0
		RET
ViewMain	ENDP

V11_00:		CALL	ViewReload		; F4 - ASCII / Hex
		MOV	BX,OFFSET VwTextHex
		JMP	V12_01

V12_00:		CMP	DS:VwTextHex,0		; F2 - Wrap / Unwrap
		JNZ	V10_20
		CALL	ViewReload
		MOV	BX,OFFSET VwWrap
V12_01:		CMP	BYTE PTR DS:[BX],0
		MOV	BYTE PTR DS:[BX],0
		JNZ	V12_02
		MOV	BYTE PTR DS:[BX],1
V12_02:		MOV	SI,DS:VwFirst1
		CMP	SI,0FFFFh
		JNE	V12_03
		MOV	SI,DS:VwFirst
		MOV	DS:VwFirst1,SI
V12_03:		CALL	ViewBegLine
		JNC	V12_04
		CMP	DS:VwTextHex,0
		JZ	V12_04
		CALL	ViewNextLine
V12_04:		MOV	DS:VwFirst,SI
		JMP	V10_10

V13_00:		CALL	ViewReload		; F8 - Table
		MOV	SI,OFFSET VwTbl
		MOV	AL,0
		CALL	TableSelect
		JC	V10_20
		JMP	V12_02

V14_00:		CMP	DS:VwTbl.Translate,OFFSET TransDBCS ; Shift-F8 - Shift
		JNE	V10_20
		CALL	ViewReload
		MOV	BX,OFFSET VwShift
		JMP	V12_01

V15_00:		MOV	BL,1			; Left
		JMP	V16_01

V16_00:		MOV	BL,DS:VwSize.X		; ^Left
		SHR	BL,1
V16_01:		CALL	ViewReload
		MOV	SI,OFFSET VwColText
		CMP	DS:VwTextHex,0
		JZ	V16_02
		MOV	SI,OFFSET VwColHex
V16_02:		MOV	AX,DS:[SI]
		MOV	BH,0
		SUB	AX,BX
		JAE	V16_10
		XOR	AX,AX
V16_10:		CMP	AX,DS:[SI]
		JE	V10_20
		MOV	DS:[SI],AX
		JMP	V10_10

V17_00:		MOV	BL,1			; Right
		JMP	V18_01

V18_00:		MOV	BL,DS:VwSize.X		; ^Right
		SHR	BL,1
V18_01:		CALL	ViewReload
		MOV	SI,OFFSET VwColText
		MOV	CL,DS:VwSize.X
		MOV	CH,0
		NOT	CX
		CMP	DS:VwTextHex,0
		JZ	V18_02
		MOV	SI,OFFSET VwColHex
		MOV	AX,DS:VwHexSize
		MOV	CX,AX
		SHL	CX,1
		SHL	CX,1
		DEC	AX
		SHR	AX,1
		SHR	AX,1
		ADD	CX,AX
		ADD	CX,13
		MOV	AL,DS:VwSize.X
		MOV	AH,0
		SUB	CX,AX
		JAE	V18_02
		XOR	CX,CX
V18_02:		MOV	BH,0
		MOV	AX,DS:[SI]
		ADD	AX,BX
		CMP	AX,CX
		JBE	V16_10
		MOV	AX,CX
		JMP	V16_10

V19_00:		CALL	ViewReload			; Up
		MOV	DS:VwFirst1,0FFFFh
V19_01:		MOV	SI,DS:VwFirst
		CALL	ViewPrevLine
		JNC	V19_02
		MOV	AX,DS:VwOffset.Lo
		OR	AX,DS:VwOffset.Hi
		JE	V10_20
		MOV	AX,DS:VwSpace
		SHR	AX,1
		CMP	AX,DS:VwFirst
		JBE	V19_02
		CALL	ViewReadPrev
		MOV	SI,DS:VwFirst
		CALL	ViewPrevLine
		MOV	DS:VwFirst,SI
		JMP	V10_10
V19_02:		MOV	DS:VwFirst,SI
		CALL	ViewStatus
		MOV	BX,DS:VwBase
		PUSH	BX
		ADD	BH,DS:VwSize.Y
		DEC	BH
		MOV	AH,7
		CALL	ViewScroll
		POP	BX
		CALL	ViewClrLine
		MOV	SI,DS:VwFirst
		MOV	BP,SI
		CALL	ViewNextLine
		XCHG	BP,SI
		MOV	AL,0
		CALL	ViewPutLine
		PUSH	BX
		MOV	BX,DS:VwBase
		MOV	DL,DS:VwSize.X
		MOV	DH,1
		CALL	Window
		POP	BX
		MOV	CL,DS:VwSize.Y
		MOV	CH,0
V19_03:		CALL	ViewNextLine
		JC	V19_04
		LOOP	V19_03
		JMP	V10_20
V19_04:		CALL	ViewChk100
		JMP	V10_20

V20_00:		CALL	ViewReload		; Down
		MOV	DS:VwFirst1,0FFFFh
		CMP	DS:Vw100pc,0
		JNZ	V10_20
V20_01:		MOV	SI,DS:VwFirst
		CALL	ViewNextLine
		JC	V22_10
		MOV	DS:VwFirst,SI
		CALL	ViewStatus
		MOV	BX,DS:VwBase
		MOV	AH,6
		CALL	ViewScroll
		ADD	BH,DS:VwSize.Y
		DEC	BH
		CALL	ViewClrLine
		MOV	AL,DS:VwSize.Y
		CBW
		DEC	AX
		MOV	CX,AX
		MOV	SI,DS:VwFirst
		JCXZ	V20_03
V20_02:		CALL	ViewNextLine
		JC	V10_11
		LOOP	V20_02
V20_03:		MOV	BP,SI
		CALL	ViewNextLine
		XCHG	BP,SI
		PUSHF
		CALL	ViewPutLine
		MOV	BX,DS:VwBase
		ADD	BH,AL
		MOV	DL,DS:VwSize.X
		MOV	DH,1
		CALL	Window
		POPF
		JC	V10_11
		JMP	V10_20

V21_00:		CALL	ViewReload		; PgUp
		MOV	DS:VwFirst1,0FFFFh
		MOV	BL,DS:VwSize.Y
		SUB	BL,3
		JBE	V19_01
		MOV	BH,0
		INC	BX
		MOV	CX,BX
		MOV	SI,DS:VwFirst
		CALL	ViewPrevLines
		JNC	V21_01
		MOV	AX,DS:VwSpace
		SHR	AX,1
		CMP	AX,DS:VwFirst
		JBE	V21_01
		MOV	AX,DS:VwOffset.Lo
		OR	AX,DS:VwOffset.Hi
		JE	V21_01
		CALL	ViewReadPrev
		MOV	CX,BX
		MOV	SI,DS:VwFirst
		CALL	ViewPrevLines
V21_01:		MOV	DS:VwFirst,SI
		JMP	V10_10

V22_00:		CALL	ViewReload		; PgDn
		MOV	DS:VwFirst1,0FFFFh
		CMP	DS:Vw100pc,0
		JNZ	V10_20
		MOV	BL,DS:VwSize.Y
		MOV	BH,0
		MOV	CX,BX
		SUB	CL,3
		JBE	V20_01
		INC	CX
		MOV	SI,DS:VwFirst
		MOV	DI,OFFSET TmpBuf+256-2
V22_01:		CALL	ViewNextLine
		JC	V22_10
		INC	DI
		INC	DI
		MOV	ES:[DI],SI
		LOOP	V22_01
		LEA	CX,[BX-1]
V22_02:		CALL	ViewNextLine
		JC	V22_03
		LOOP	V22_02
		JMP	V22_05
V22_03:		MOV	AX,DS:VwOffset.Lo
		MOV	BX,DS:VwOffset.Hi
		ADD	AX,DS:VwLoaded
		ADC	BX,0
		CMP	BX,DS:VwLen.Hi
		JB	V22_05
		JA	V22_04
		CMP	AX,DS:VwLen.Lo
		JB	V22_05
V22_04:		ADD	CX,CX
		SUB	DI,CX
		CMP	DI,OFFSET TmpBuf+256
		JB	V10_20
V22_05:		MOV	AX,ES:[DI]
		MOV	DS:VwFirst,AX
		JMP	V10_10
V22_10:		CALL	ViewReadNext
		MOV	DS:VwFirst,0
		JMP	V10_10

V23_00:		CALL	ViewReload		; Home
		MOV	DS:VwFirst1,0FFFFh
		XOR	BX,BX
		XOR	CX,CX
		XOR	DX,DX
		XCHG	BX,DS:VwOffset.Lo
		XCHG	CX,DS:VwOffset.Hi
		XCHG	DX,DS:VwFirst
		MOV	AX,DS:VwColText
		CMP	DS:VwTextHex,0
		JZ	V23_01
		MOV	AX,DS:VwColHex
V23_01:		MOV	DS:VwColText,0
		MOV	DS:VwColHex,0
		MOV	SI,BX
		OR	SI,CX
		JNZ	V23_02
		OR	AX,DX
		JE	V10_20
		JMP	V10_10
V23_02:		MOV	AX,DS:VwSpace
		OR	CX,CX
		JNE	V23_03
		CMP	BX,AX
		JAE	V23_03
		MOV	AX,BX
V23_03:		MOV	DS:VwOffset.Lo,AX
		CALL	ViewReadPrev1
		MOV	DS:VwFirst,0
		JMP	V10_10

V24_00:		CALL	ViewReload		; End
		MOV	DS:VwFirst1,0FFFFh
		CMP	DS:Vw100pc,0
		JNZ	V10_20
		MOV	SI,DS:VwLen.Lo
		MOV	DI,DS:VwLen.Hi
		MOV	BX,SI
		MOV	CX,DI
		SUB	BX,DS:VwOffset.Lo
		SBB	CX,DS:VwOffset.Hi
		XOR	AX,AX
		MOV	DX,DS:VwSpace
		SUB	BX,DX
		SBB	CX,AX
		JB	V24_02
		JA	V24_01
		MOV	AX,BX
		CMP	AX,DX
		JB	V24_02
V24_01:		XOR	AX,AX
		SUB	SI,DX
		SBB	DI,AX
		MOV	DS:VwOffset.Lo,SI
		MOV	DS:VwOffset.Hi,DI
		MOV	DS:VwLoaded,AX
V24_02:		CALL	ViewReadNext1
		MOV	SI,DS:VwLoaded
		CMP	DS:VwTextHex,0
		JNZ	V24_03
		INC	SI
V24_03:		MOV	CL,DS:VwSize.Y
		MOV	CH,0
		CALL	ViewPrevLines
		MOV	DS:VwFirst,SI
		JMP	V10_10

VwGotoOffs	EQU	DWORD PTR TmpBuf[0]
VwGotoFlag	EQU	 BYTE PTR TmpBuf[4]
VwGotoBefore	EQU	2048

V25_00:		CMP	DS:VwTextHex,0		; F5 - Goto
		JZ	V10_20
		CALL	ViewReload
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET ViewGoto_T
		MOV	BP,OFFSET GotoOffset_B
		CALL	Dialog
		POP	DS
		JNE	V10_20
		CMP	ES:VwGotoFlag,0
		JE	V10_20
		MOV	AX,ES:VwGotoOffs.Lo
		MOV	BX,ES:VwGotoOffs.Hi
		MOV	CX,DS:VwLen.Lo
		MOV	DX,DS:VwLen.Hi
		CMP	BX,DX
		JB	V25_02
		JA	V25_01
		CMP	AX,CX
		JB	V25_02
V25_01:		MOV	AX,CX
		MOV	BX,DX
		OR	CX,DX
		JE	V25_02
		SUB	AX,1
		SBB	BX,0
V25_02:		CALL	V25_10
		JMP	V10_10

V25_10		PROC	NEAR			; Go to BX AX
		MOV	CX,AX
		MOV	DX,BX
		SUB	AX,DS:VwOffset.Lo
		SBB	BX,DS:VwOffset.Hi
		JB	V25_11
		JA	V25_15
		CMP	AX,DS:VwLoaded
		JAE	V25_15
		MOV	SI,AX
		CALL	ViewBegLine
		JNC	V25_22
		CMP	SI,DS:VwFirst
		JE	V25_22
		CMP	AX,VwGotoBefore
		JAE	V25_22
V25_11:		PUSH	CX
		SUB	CX,VwGotoBefore
		SBB	DX,0
		JAE	V25_12
		XOR	CX,CX
		XOR	DX,DX
V25_12:		MOV	AX,DS:VwSpace
		MOV	SI,DS:VwOffset.Lo
		MOV	DI,DS:VwOffset.Hi
		SUB	SI,CX
		SBB	DI,DX
		JNE	V25_13
		CMP	SI,AX
		JAE	V25_13
		MOV	AX,SI
V25_13:		ADD	CX,AX
		ADC	DX,0
		MOV	DS:VwOffset.Lo,CX
		MOV	DS:VwOffset.Hi,DX
		CALL	ViewReadPrev1
		JMP	V25_21
V25_15:		PUSH	CX
		XOR	AX,AX
		MOV	BX,DS:VwSpace
		MOV	SI,DS:VwLen.Lo
		MOV	DI,DS:VwLen.Hi
		SUB	SI,BX
		SBB	DI,AX
		JB	V25_20
		SUB	CX,VwGotoBefore
		SBB	DX,AX
		JAE	V25_16
		XOR	CX,CX
		XOR	DX,DX
V25_16:		CMP	DX,DI
		JB	V25_18
		JA	V25_17
		CMP	CX,SI
		JBE	V25_18
V25_17:		MOV	CX,SI
		MOV	DX,DI
V25_18:		MOV	SI,CX
		MOV	DI,DX
		SUB	SI,DS:VwOffset.Lo
		SBB	DI,DS:VwOffset.Hi
		JB	V25_20
		JA	V25_19
		MOV	AX,SI
		CMP	AX,BX
		JBE	V25_20
V25_19:		XOR	AX,AX
		MOV	DS:VwLoaded,AX
		MOV	DS:VwOffset.Lo,CX
		MOV	DS:VwOffset.Hi,DX
V25_20:		CALL	ViewReadNext1
V25_21:		POP	AX
		SUB	AX,DS:VwOffset.Lo
		MOV	SI,AX
		CALL	ViewBegLine
V25_22:		MOV	DS:VwFirst1,AX
		MOV	DS:VwFirst,SI
		RET
V25_10		ENDP

		.CODE	Subs
V25_30		PROC	NEAR			; Test offset
		CMP	AH,0FCh
		JAE	@@Exit
		CMP	AL,0Dh
		JE	@@ChkNumb
		CMP	AL,0Ah
		JNE	@@Exit
  @@ChkNumb:	PUSH	SS
		POP	DS
		LEA	SI,[DI-9]
		CALL	FndLet
		MOV	ES:VwGotoFlag,AL
		CMP	AL,0
		JE	@@Enter
		XOR	BX,BX
		XOR	DX,DX
		MOV	CL,4
  @@Char:	LODSB
		CMP	AL,0
		JE	@@Offset
		CMP	AL,' '
		JE	@@Space
		CALL	UpCase
		SUB	AL,'0'
		JB	@@Error
		CMP	AL,9
		JBE	@@AddNum
		SUB	AL,7
		JB	@@Error
		CMP	AL,0Ah
		JB	@@Error
		CMP	AL,0Fh
		JA	@@Error
  @@AddNum:	MOV	AH,BH
		SHL	DX,CL
		SHL	BX,CL
		SHR	AH,CL
		OR	DL,AH
		OR	BL,AL
		JMP	@@Char
  @@Space:	CALL	FndLet
		CMP	AL,0
		JNE	@@Error
  @@Offset:	MOV	ES:VwGotoOffs.Lo,BX
		MOV	ES:VwGotoOffs.Hi,DX
  @@Enter:	MOV	AX,1C0Dh
		JMP	@@Exit
  @@Error:	MOV	DX,OFFSET ViewGoto_T
		MOV	BP,OFFSET GotoOffsErr_B
		CALL	Dialog
		XOR	AX,AX
  @@Exit:	RET
V25_30		ENDP

		.CODE
V26_00:		MOV	ES:Quote,1		; ^Q - quote
		MOV	AL,'"'
		CALL	V26_10
		JMP	V10_20

V26_10		PROC	NEAR
		USES	BX,DX,ES
		CMP	DS:VwStatus,OFFSET ViewStat0
		JNE	@@Exit
		MOV	BH,0
		MOV	BL,ES:Columns
		SUB	BL,40
		CALL	BasAdr
		CMP	AL,ES:[DI]
		STOSB
		JE	@@Exit
		MOV	DX,101h
		CALL	Window
  @@Exit:	RET
V26_10		ENDP

VwSrchSymb	EQU	BYTE PTR TmpBuf[0]

V27_00:		MOV	AL,0			; F7 - search
V27_10:		MOV	ES:VwSrchSymb,AL
		CALL	ViewReload
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET ViewSearch_T
		MOV	BP,OFFSET Search_B
		CALL	Dialog
		POP	DS
		JE	V28_01
		JMP	V10_20

		.CODE	Subs
V27_20		PROC	NEAR
		CMP	AX,0FFFFh
		JNE	@@Exit
		MOV	AL,ES:VwSrchSymb
		CMP	AL,0
		JE	@@Exit
		LEA	SI,[DI-513]
		MOV	AH,0
		MOV	SS:[SI],AX
		MOV	AL,1
		MOV	SS:[SI-1],AL
		MOV	SS:[SI-5],AX
  @@Exit:	RET
V27_20		ENDP

		.CODE
V28_00:		CALL	ViewReload		; Search next
V28_01:		CMP	ES:SrchStr,0
		JE	V10_20
		PUSH	DS:VwSearch.Hi
		PUSH	DS:VwSearch.Lo
		CALL	V28_20
		JC	V28_02
		MOV	AX,CX
V28_02:		ADD	SI,AX
		ADC	DI,0
		MOV	DS:VwSearch.Lo,SI
		MOV	DS:VwSearch.Hi,DI
		MOV	SI,AX			; Search in buffer
V28_03:		MOV	CX,DS:VwLoaded
		SUB	CX,SI
		JB	V28_06
		MOV	DI,SI
		ADD	SI,OFFSET VwBuffer
		CALL	SearchNext
		JNC	V28_10
		CALL	ViewReadMore
		JNC	V28_04
		ADD	DS:VwSearch.Lo,SI
		ADC	DS:VwSearch.Hi,0
		ADD	SI,DI
		JMP	V28_03
V28_04:		CALL	ViewChkEnd
		JNC	V28_07
V28_05:		ADD	DS:VwSearch.Lo,SI	; Search on disk
		ADC	DS:VwSearch.Hi,0
V28_06:		MOV	CX,DS:VwLen.Lo
		MOV	DX,DS:VwLen.Hi
		SUB	CX,DS:VwSearch.Lo
		SBB	DX,DS:VwSearch.Hi
		XOR	BX,BX
		CALL	V28_30
		JC	V28_08
		PUSH	DS
		MOV	DS,AX
		CALL	SearchNext
		POP	DS
		JNC	V28_10
		OR	SI,SI
		JNE	V28_05
V28_07:		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET ViewSearching_T
		MOV	BP,OFFSET SearchError_B
		CALL	Dialog
		POP	DS
V28_08:		POP	DS:VwSearch.Lo
		POP	DS:VwSearch.Hi
		CALL	ResWin
		CALL	Window
		JMP	V10_20
V28_10:		SBB	BX,BX			; Set line
		ADD	SI,DS:VwSearch.Lo
		ADC	BX,DS:VwSearch.Hi
		CALL	V28_50
		POP	AX
		POP	AX
		CALL	ResWin
		CALL	Window
		JMP	V10_10

V28_20		PROC	NEAR
		MOV	AL,DS:VwTextHex
		MOV	DX,OFFSET ViewSearching_T
		CALL	StartSearch
		MOV	CX,DS:VwFirst
		CMP	DS:VwSrLen,0
		JE	V28_22
		CMP	DS:VwTextHex,0
		JNZ	V28_21
		CMP	DS:VwSrTextHex,0
		JNZ	V28_22
V28_21:		MOV	AX,DS:VwSearch.Lo
		MOV	BX,DS:VwSearch.Hi
		MOV	DX,DS:VwSrLen
		ADD	AX,DX
		ADC	BX,0
		SUB	AX,DS:VwOffset.Lo
		SBB	BX,DS:VwOffset.Hi
		JNE	V28_22
		CMP	AX,CX
		JBE	V28_22
		MOV	BX,AX
		SUB	BX,DX
		JB	V28_23
		MOV	SI,CX
		CALL	ViewNextLine
		CMP	BX,SI
		JMP	V28_23
V28_22:		CLC
V28_23:		MOV	SI,DS:VwOffset.Lo
		MOV	DI,DS:VwOffset.Hi
		RET
V28_20		ENDP

V28_30		PROC	NEAR
		USES	DS
		CALL	TstBrk
		JC	V28_33
		MOV	AL,ES:Lines
		MUL	ES:Columns
		MOV	BP,AX
		ADD	BP,BP
		ADD	AX,BP
		OR	DX,DX
		JNE	V28_31
		CMP	AX,CX
		JBE	V28_31
		MOV	AX,CX
V28_31:		MOV	SI,DS:VwSearch.Lo
		MOV	DI,DS:VwSearch.Hi
		AND	BX,AX
		SUB	SI,BX
		SBB	DI,0
		MOV	CX,AX
		MOV	DX,BP
		MOV	BX,DS:VwHandle
		MOV	DS,ES:UserScreen
		CALL	ViewFileRead
		JNC	V28_32
		XOR	AX,53h
		CMP	AX,1
V28_32:		MOV	SI,BP
		MOV	AX,DS
V28_33:		RET
V28_30		ENDP

V28_40		PROC	NEAR
		JCXZ	V28_45
V28_41:		MOV	DI,SI
		LODSW
		DEC	SI
		CMP	DS:VwTbl.Translate,OFFSET TransDBCS
		JNE	V28_42
		INC	SI
		OR	AH,AH
		JNE	V28_43
V28_42:		CMP	AL,13
		JE	V28_44
		CMP	AL,10
		JE	V28_44
		CMP	AL,9
		JNE	V28_43
		OR	BL,7
V28_43:		INC	BX
		LOOP	V28_41
		JMP	V28_45
V28_44:		MOV	SI,DI
V28_45:		RET
V28_40		ENDP

V28_50		PROC	NEAR			; Set position
		MOV	CX,ES:Search.SrLenHex
		CMP	AL,0
		JNZ	V28_51
		MOV	CX,ES:Search.SrLenText
		CMP	DS:VwTbl.Translate,OFFSET TransDBCS
		JNE	V28_51
		ADD	CX,CX
		MOV	DX,SI
		AND	DL,1
		MOV	DS:VwShift,DL
V28_51:		MOV	DS:VwSrLen,CX
		MOV	DS:VwSrTextHex,AL
		MOV	AX,SI
		MOV	DS:VwSearch.Lo,AX
		MOV	DS:VwSearch.Hi,BX
		CALL	V25_10
		CMP	DS:VwTextHex,0		; Set column
		JNZ	V28_59
		MOV	SI,DS:VwFirst
		PUSH	SI
		CALL	ViewNextLine
		MOV	AX,DS:VwFirst1
		SUB	SI,AX
		MOV	DX,DS:VwSrLen
		CMP	DX,SI
		JBE	V28_52
		MOV	DX,SI
V28_52:		MOV	CX,SI
		SUB	CX,DX
		POP	SI
		PUSH	CX
		MOV	CX,AX
		SUB	CX,SI
		XOR	BX,BX
		MOV	BP,OFFSET ViewGetChar1
		CMP	DS:VwTbl.Translate,OFFSET TransDBCS
		JNE	V28_53
		MOV	BP,OFFSET ViewGetChar3
		SHR	DX,1
		SHR	CX,1
		JNC	V28_53
		INC	BX
		INC	SI
V28_53:		ADD	SI,OFFSET VwBuffer
		CALL	V28_40
		MOV	CX,DX
		MOV	DX,BX
		CALL	V28_40
		MOV	DI,DS:VwColText
		MOV	AL,DS:VwSize.X
		MOV	AH,0
		MOV	CX,DI
		ADD	CX,AX
		CMP	BX,CX
		POP	CX
		JB	V28_57
		MOV	DI,BX
		SUB	DI,AX
		JCXZ	V28_57
V28_54:		CALL	BP
		CMP	AL,13
		JE	V28_57
		CMP	AL,10
		JE	V28_57
		CMP	DS:VwWrap,0
		JZ	V28_56
		CMP	AL,' '
		JE	V28_55
		CMP	AL,9
		JNE	V28_56
V28_55:		LOOP	V28_54
		JMP	V28_57
V28_56:		INC	DI
V28_57:		CMP	DX,DI
		JA	V28_58
		MOV	DI,DX
		OR	DI,DI
		JE	V28_58
		DEC	DI
V28_58:		MOV	DS:VwColText,DI
V28_59:		RET
V28_50		ENDP

V29_00:		CALL	ViewReload		; Search previous
		CMP	ES:SrchStr,0
		JE	V10_20
		PUSH	DS:VwSearch.Hi
		PUSH	DS:VwSearch.Lo
		CALL	V28_20
		JC	V29_01
		ADD	SI,CX
		ADC	DI,0
		MOV	DS:VwSearch.Lo,SI
		MOV	DS:VwSearch.Hi,DI
		JMP	V29_02
V29_01:		MOV	CX,DS:VwSearch.Lo
		MOV	DX,DS:VwSearch.Hi
		SUB	CX,SI
		SBB	DX,DI
		JNE	V29_04
V29_02:		MOV	SI,CX			; Search in buffer
		ADD	SI,OFFSET VwBuffer
		CALL	SearchPrev
		JNC	V29_05
		MOV	AX,DS:VwOffset.Lo
		OR	AX,DS:VwOffset.Hi
		JE	V28_07
V29_03:		SUB	DS:VwSearch.Lo,SI	; Search on disk
		SBB	DS:VwSearch.Hi,0
V29_04:		MOV	CX,DS:VwSearch.Lo
		MOV	DX,DS:VwSearch.Hi
		MOV	BX,0FFFFh
		CALL	V28_30
		JC	V28_08
		ADD	SI,CX
		PUSH	DS
		MOV	DS,AX
		CALL	SearchPrev
		POP	DS
		JNC	V29_05
		OR	SI,SI
		JNE	V29_03
		JMP	V28_07
V29_05:		MOV	BX,SI
		XOR	SI,SI
		SUB	SI,BX
		JMP	V28_10

;---------------------------------------------------------------
;	Init viewer
;		DS:SI - file
;		   AL - don't show error
;		   BX - window base
;		   DX - window size
;		   BP - limit in paragraphs
;	Return:	   BP - limit in paragraphs
;		   DS - view segment
;		   BX - handle
;		   CY - error
;---------------------------------------------------------------
ViewInit	PROC	FAR
		MOV	CL,AL
		XCHG	DX,SI
		CALL	ChgDisk
		JC	@@Error
		MOV	AL,40h
		CALL	OpenFil
		JNC	@@Break
		CMP	AX,53h
		JE	@@Error
		OR	CL,CL
		JNE	@@Error
		MOV	DX,OFFSET ReadView_T
		MOV	BP,OFFSET FndEr_B
		CALL	Dialog
  @@Error:	STC
		JMP	@@Exit
  @@Break:	CALL	TstBrk
		JC	@@Error
		MOV	DI,DS:WorkBuffer
		MOV	DS,DI
		MOV	DX,ES
		ADD	DX,ES:SizeDataSeg
		SUB	DX,DI
		CMP	DX,BP
		JBE	@@BufSize
		MOV	DX,BP
  @@BufSize:	MOV	BP,DX
		MOV	CL,4
		SHL	DX,CL
		SUB	DX,OFFSET VwBuffer
		MOV	DS:VwSpace,DX
		MOV	DS:VwHandle,AX
		MOV	DS:VwBase,BX
		MOV	DS:VwSize,SI
		MOV	BX,AX
		XOR	CX,CX
		XOR	DX,DX
		MOV	AX,4202h
		CALL	Intr21
		MOV	DS:VwLen.Lo,AX
		MOV	DS:VwLen.Hi,DX
		XOR	AX,AX
		MOV	DS:VwOffset.Lo,AX
		MOV	DS:VwOffset.Hi,AX
		MOV	DS:VwLoaded,AX
		MOV	DS:VwFirst,AX
		MOV	DS:VwFirst1,0FFFFh
		MOV	DS:VwColText,AX
		MOV	DS:VwColHex,AX
		MOV	DS:VwHexSize,16
		MOV	DS:VwTextHex,AL
		MOV	DS:VwWrap,1
		MOV	DS:VwShift,AL
		MOV	DS:VwTbl.TblNumb,AL
		MOV	DS:VwTbl.Translate,OFFSET TransNone
		MOV	DS:VwSrLen,AX
		MOV	CX,DS:VwSpace
		MOV	AX,4000h
		CMP	AX,CX
		JBE	@@SetLimit
		MOV	AX,CX
  @@SetLimit:	MOV	DS:VwSpace,AX
		CALL	ViewReadMore
		MOV	DS:VwSpace,CX
		CLC
  @@Exit:	RET
ViewInit	ENDP

;---------------------------------------------------------------
;	Output status line
;---------------------------------------------------------------
ViewStatus	PROC	FAR
		USES	AX,BX
		MOV	DS:Vw100pc,0
		MOV	BX,DS:VwStatus
		OR	BX,BX
		JE	@@Exit
		MOV	AL,0
		CALL	BX
  @@Exit:	RET
ViewStatus	ENDP

ViewStat0	PROC	NEAR			; Full screen view
		USES	AX,BX,CX,DX,SI,DI
		MOV	CX,100
		CMP	AL,0
		JNZ	@@PutPerCent
		MOV	CX,DS:VwColText		; Col nnn
		CMP	DS:VwTextHex,0
		JZ	@@Column
		MOV	CX,DS:VwColHex
  @@Column:	MOV	DI,OFFSET TmpBuf
		PUSH	DI
		XOR	BX,BX
		MOV	AL,1
		CALL	Number
		MOV	AL,' '
		MOV	CX,6
		PUSH	CX
		REP	STOSB
		POP	CX
		POP	SI
		MOV	BL,ES:Columns
		SUB	BL,34
		PUSH	DS
		PUSH	ES
		POP	DS
		CALL	BasAdr
  @@WriteCol:	MOVSB
		INC	DI
		LOOP	@@WriteCol
		PUSH	DS
		POP	ES
		POP	DS
		CALL	ViewCalcPerCent		; %
  @@PutPerCent:	MOV	DL,ES:Columns
		MOV	BL,DL
		SUB	BL,5
		MOV	BH,0
		CALL	ViewPutPerCent
		XOR	BX,BX
		CALL	Window
		RET
ViewStat0	ENDP

ViewStat1a	PROC	NEAR			; Quick view
		USES	BX,DX
		CALL	ViewStat1
		RET
ViewStat1a	ENDP

ViewStat1b	PROC	NEAR
		USES	BX,DX
		CALL	ViewStat1
		CALL	Window
		RET
ViewStat1b	ENDP

ViewStat1	PROC	NEAR
		USES	AX,CX,SI,DI
		MOV	CX,100
		CMP	AL,0
		JNZ	@@PutPerCent
		CALL	ViewCalcPerCent
  @@PutPerCent:	MOV	BX,DS:VwBase
		MOV	DX,DS:VwSize
		ADD	BX,DX
		INC	BH
		SUB	BL,4
		MOV	DL,3
		CALL	ViewPutPerCent
		RET
ViewStat1	ENDP

ViewCalcPerCent	PROC	NEAR
		MOV	SI,DS:VwFirst
		MOV	AX,DS:VwOffset.Lo
		MOV	BX,DS:VwOffset.Hi
		ADD	AX,SI
		ADC	BX,0
		MOV	CX,100
		XOR	DX,DX
		XOR	SI,SI
		XOR	DI,DI
  @@Mul:	ADD	DX,AX
		ADC	SI,BX
		ADC	DI,0
		LOOP	@@Mul
		MOV	AX,DS:VwLen.Lo
		MOV	BX,DS:VwLen.Hi
		MOV	CX,AX
		OR	CX,BX
		JE	@@Exit
		MOV	CX,0FFFFh
  @@Div:	INC	CX
		SUB	DX,AX
		SBB	SI,BX
		SBB	DI,0
		JNB	@@Div
  @@Exit:	RET
ViewCalcPerCent	ENDP

ViewPutPerCent	PROC	NEAR
		USES	DS,ES
		PUSH	BX
		XOR	BX,BX
		MOV	DI,OFFSET TmpBuf[0]
		MOV	AL,3
		CALL	Number
		POP	BX
		MOV	SI,OFFSET TmpBuf[10]
		MOV	CX,3
		PUSH	ES
		POP	DS
		CALL	BasAdr
  @@WrPerCent:	MOVSB
		INC	DI
		LOOP	@@WrPerCent
		MOV	DH,1
		RET
ViewPutPerCent	ENDP

;---------------------------------------------------------------
;	Output screen if size changed
;---------------------------------------------------------------
ViewReload	PROC	NEAR
		USES	AX,BX,CX,DX,SI,BP
		MOV	BX,DS:VwHandle
		CMP	BX,0FFFEh
		JAE	@@Exit
		XOR	CX,CX
		XOR	DX,DX
		MOV	AX,4202h
		CALL	Intr21
		MOV	BX,AX
		MOV	CX,DX
		XCHG	BX,DS:VwLen.Lo
		XCHG	CX,DS:VwLen.Hi
		XOR	BX,AX
		XOR	CX,DX
		OR	CX,BX
		JE	@@Exit
		MOV	CL,0
		MOV	BX,AX
		MOV	SI,DX
		SUB	AX,DS:VwOffset.Lo
		SBB	DX,DS:VwOffset.Hi
		JA	@@Status
		JE	@@CmpLoaded
		MOV	DS:VwOffset.Lo,BX
		MOV	DS:VwOffset.Hi,SI
		XOR	AX,AX
		JMP	@@Reduse
  @@CmpLoaded:	MOV	DX,DS:VwLoaded
		CMP	DX,AX
		JBE	@@Status
  @@Reduse:	MOV	DS:VwLoaded,AX
		INC	CX
  @@Status:	PUSH	DS
		MOV	AX,DS:VwLen.Lo
		MOV	BX,DS:VwLen.Hi
		MOV	ES:TmpBuf.Lo,AX
		MOV	ES:TmpBuf.Hi,BX
		MOV	DS,CS:SubsSeg
		MOV	BL,ES:Columns
		MOV	BH,0
		MOV	AX,38
		SUB	BX,AX
		MOV	BP,OFFSET Status_P
		MOV	DX,OFFSET ViewStat_T
		MOV	SI,OFFSET ViewStat2_S
		CALL	NgLine
		POP	DS
		MOV	AL,DS:Vw100pc
		CALL	ViewStatus
		OR	CL,CL
		JNZ	@@SetFirst
		CMP	AL,0
		JZ	@@Exit
		CALL	ViewReadMore
		JMP	@@Screen
  @@SetFirst:	MOV	DS:VwFirst1,0FFFFh
		MOV	SI,DS:VwLoaded
		CMP	SI,DS:VwFirst
		JA	@@Screen
		MOV	DS:VwFirst,SI
		CALL	ViewEndLine
		JNC	@@First
		MOV	AX,DS:VwSpace
		SUB	AX,DS:VwLoaded
		CALL	ViewReadPrev1
		MOV	SI,DS:VwFirst
		CALL	ViewEndLine
  @@First:	MOV	DS:VwFirst,SI
  @@Screen:	CALL	ViewScreen
		JNC	@@Exit
		CALL	ViewChk100
  @@Exit:	RET
ViewReload	ENDP

ViewEndLine	PROC	NEAR
		CMP	DS:VwTextHex,0
		JZ	@@BegLine
		CMP	SI,1
		JB	@@Exit
		DEC	SI
  @@BegLine:	CALL	ViewBegLine
  @@Exit:	RET
ViewEndLine	ENDP

;---------------------------------------------------------------
;	Output screen
;	Return:	   CY - end found
;---------------------------------------------------------------
ViewScreen	PROC	NEAR
		USES	BX,DX
		CALL	ViewScreen0
		PUSHF
		MOV	BX,DS:VwBase
		MOV	DX,DS:VwSize
		CALL	Window
		POPF
		RET
ViewScreen	ENDP

ViewScreen0	PROC	FAR
		USES	AX,BX,CX,DI,BP
		MOV	BX,DS:VwBase		; Clear window
		MOV	CL,DS:VwSize.Y
		MOV	CH,0
		PUSH	CX
		MOV	AL,0
  @@Cls:	CALL	ViewClrLine
		INC	BH
		LOOP	@@Cls
		POP	CX
		MOV	BP,DS:VwFirst
		MOV	AL,0
  @@WriteLine:	MOV	SI,BP
		CALL	ViewNextLine
		XCHG	BP,SI
		JC	@@EndBuffer
		CALL	ViewPutLine
		INC	AX
		LOOP	@@WriteLine
		CLC
		JMP	@@Exit
  @@EndBuffer:	CALL	ViewChk100
		CALL	ViewPutLine
		STC
  @@Exit:	RET
ViewScreen0	ENDP

;---------------------------------------------------------------
;	Clear line
;		BH BL - base
;---------------------------------------------------------------
ViewClrLine	PROC	NEAR
		USES	AX,CX,DI,ES
		MOV	CL,DS:VwSize.X
		MOV	CH,0
		MOV	AH,Back_C
		CALL	Color
		MOV	AL,' '
		CALL	BasAdr
		REP	STOSW
		RET
ViewClrLine	ENDP

;---------------------------------------------------------------
;	Check end of file is loaded
;	Return:	   CY - not loaded
;---------------------------------------------------------------
ViewChkEnd	PROC	NEAR
		USES	AX,BX
		MOV	AX,DS:VwOffset.Lo
		MOV	BX,DS:VwOffset.Hi
		ADD	AX,DS:VwLoaded
		ADC	BX,0
		CMP	BX,DS:VwLen.Hi
		JNE	@@Exit
		CMP	AX,DS:VwLen.Lo
  @@Exit:	RET
ViewChkEnd	ENDP

;---------------------------------------------------------------
;	Check end of file is loaded, output 100%
;	Return:	   CY - not loaded
;---------------------------------------------------------------
ViewChk100	PROC	NEAR
		USES	AX,BX
		CALL	ViewChkEnd
		JC	@@Exit
		MOV	DS:Vw100pc,1
		MOV	BX,DS:VwStatus
		OR	BX,BX
		JE	@@Exit
		MOV	AL,1
		CALL	BX
		CLC
  @@Exit:	RET
ViewChk100	ENDP

;---------------------------------------------------------------
;	Find beginning of line
;		   SI - offset
;	Return:    SI - offset of the beginning of line
;		   CY - not found
;---------------------------------------------------------------
ViewBegLine	PROC	NEAR
		USES	AX,BX,CX,DX,DI,BP
		CMP	DS:VwTextHex,0
		JNZ	@@Hex
		MOV	DH,DS:VwWrap
		MOV	DI,SI
		MOV	CX,SI
		ADD	SI,OFFSET VwBuffer-1
		STD
		JCXZ	@@Found
		MOV	BP,OFFSET ViewGetChar1
		CMP	DS:VwTbl.Translate,OFFSET TransDBCS
		JNE	@@Char
		MOV	BP,OFFSET ViewGetChar2
		MOV	AX,DS:VwOffset.Lo
		ADD	AX,SI
		ADD	AL,DS:VwShift
		AND	AL,1
		JZ	@@Char
		LODSB
		CMP	DI,DS:VwLoaded
		JAE	@@ChkCrLf
		JMP	@@NextChar
  @@Char:	CALL	BP
		JC	@@NextChar
  @@ChkCrLf:	CMP	AL,13
		JE	@@ChkLast
		CMP	AL,10
		JE	@@ChkLast
  @@NextChar:	LOOP	@@Char
		JMP	@@Found
  @@ChkLast:	LEA	BX,[SI+2]
		SUB	BX,OFFSET VwBuffer
		CMP	BP,OFFSET ViewGetChar1
		JE	@@FirstChar
		INC	BX
  @@FirstChar:	CMP	BX,DS:VwLoaded
		JAE	@@Found
		MOV	AH,DS:[BX+VwBuffer]
		CMP	BP,OFFSET ViewGetChar1
		JE	@@Cmp
		INC	BX
		CMP	BX,DS:VwLoaded
		JAE	@@Cmp
		CMP	DS:[BX+VwBuffer],0
		JNE	@@Found
  @@Cmp:	CMP	AH,AL
		JE	@@Found
		CMP	AH,13
		JE	@@NextCrLf
		CMP	AH,10
		JE	@@NextCrLf
		JMP	@@Found
  @@CrLf:	MOV	AH,AL
		CALL	BP
		JC	@@NextChar
		CMP	AL,AH
		JE	@@Found
		CMP	AL,13
		JE	@@NextCrLf
		CMP	AL,10
		JNE	@@NextChar
  @@NextCrLf:	MOV	DH,1
		LOOP	@@CrLf
  @@Found:	CLD
		SUB	SI,OFFSET VwBuffer-1
		JCXZ	@@BegBuf
		INC	SI
		CMP	BP,OFFSET ViewGetChar1
		JE	@@BegBuf
		CMP	SI,DS:VwLoaded
		JAE	@@BegBuf
		INC	SI
  @@BegBuf:	MOV	CX,DI
		MOV	DI,OFFSET TmpBuf+2
		OR	DH,DH
		JZ	@@ChkBegin
  @@NextLine:	MOV	AX,SI
		CALL	ViewNextLine
		JC	@@SetLine
		CMP	SI,CX
		JA	@@SetLine
		JE	@@ChkBegin
		CMP	DI,OFFSET TmpBuf+256
		JBE	@@SavePoint
		PUSH	DS
		PUSH	SI
		PUSH	CX
		PUSH	ES
		POP	DS
		MOV	CX,DI
		MOV	DI,OFFSET TmpBuf+2
		LEA	SI,[DI+64]
		SUB	CX,SI
		SHR	CX,1
		REP	MOVSW
		POP	CX
		POP	SI
		POP	DS
  @@SavePoint:	STOSW
		JMP	@@NextLine
  @@SetLine:	MOV	SI,AX
  @@ChkBegin:	MOV	WORD PTR ES:TmpBuf[0],DI
		OR	SI,SI
		JNE	@@Exit
		MOV	AX,DS:VwOffset.Lo
		OR	AX,DS:VwOffset.Hi
		JE	@@Exit
		JMP	@@Error
  @@Hex:	MOV	WORD PTR ES:TmpBuf[0],0
		MOV	BX,DS:VwOffset.Lo
		MOV	AX,DS:VwOffset.Hi
		ADD	BX,SI
		ADC	AX,0
		XOR	DX,DX
		DIV	DS:VwHexSize
		MOV	AX,BX
		DIV	DS:VwHexSize
		SUB	SI,DX
		JAE	@@Exit
  @@Error:	XOR	SI,SI
		STC
  @@Exit:	RET
ViewBegLine	ENDP

ViewGetChar1	PROC	NEAR
		LODSB
		CLC
		RET
ViewGetChar1	ENDP

ViewGetChar2	PROC	NEAR
		LODSB
		CMP	CX,1
		JE	@@Zero
		DEC	CX
		CMP	AL,0
		LODSB
		JE	@@Exit
  @@Zero:	MOV	AL,0
		STC
  @@Exit:	RET
ViewGetChar2	ENDP

;---------------------------------------------------------------
;	Find next line
;		   SI - offset of line
;	Return:	   SI - offset of next line
;		   CY - not found
;---------------------------------------------------------------
ViewNextLine	PROC	NEAR
		USES	AX,BX,CX,DX,DI,BP
		MOV	DI,SI
		CMP	DS:VwTextHex,0
		JNZ	@@Hex
		MOV	CX,DS:VwLoaded
		SUB	CX,SI
		JBE	@@Error
		XOR	BX,BX
		XOR	DX,DX
		ADD	SI,OFFSET VwBuffer
		MOV	BP,OFFSET ViewGetChar1
		CMP	DS:VwTbl.Translate,OFFSET TransDBCS
		JNE	@@Char
		MOV	BP,OFFSET ViewGetChar3
		MOV	AX,DS:VwOffset.Lo
		ADD	AX,DI
		ADD	AL,DS:VwShift
		AND	AL,1
		JZ	@@Char
		INC	SI
		JMP	@@NotChar
  @@Char:	CALL	BP
		JNC	@@CrLf
  @@NotChar:	CMP	DS:VwWrap,0
		JZ	@@Next
		JMP	@@ChkEnd
  @@CrLf:	CMP	AL,13
		JE	@@FoundCr
		CMP	AL,10
		JE	@@FoundLf
		CMP	DS:VwWrap,0
		JZ	@@Next
		CMP	AL,' '
		JE	@@Space
		CMP	AL,9
		JNE	@@ChkEnd
		OR	BL,7
  @@Space:	INC	BX
		JE	@@Error
		JMP	@@SaveLoc
  @@ChkEnd:	INC	BX
		JE	@@Error
		MOV	AL,ES:Columns
		MOV	AH,0
		CMP	BX,AX
		JB	@@Next
		JE	@@ChkLoc
		MOV	SI,DX
  @@Found:	SUB	SI,OFFSET VwBuffer
		JMP	@@Exit
  @@FoundCr:	MOV	AH,10
		JMP	@@CrLf2
  @@FoundLf:	MOV	AH,13
  @@CrLf2:	CMP	CX,1
		JE	@@LastChar
		PUSH	SI
		CALL	BP
		CMP	AL,AH
		POP	AX
		JE	@@Found
		MOV	SI,AX
		JMP	@@Found
  @@LastChar:	CALL	ViewChkEnd
		JNC	@@Found
		JMP	@@Error
  @@ChkLoc:	OR	DX,DX
		JNE	@@Next
  @@SaveLoc:	MOV	DX,SI
  @@Next:	LOOP	@@Char
		JMP	@@Error
  @@Hex:	ADD	SI,DS:VwHexSize
		CALL	ViewBegLine
		CMP	DS:VwLoaded,SI
		JA	@@Exit
  @@Error:	MOV	SI,DS:VwLoaded
		STC
  @@Exit:	RET
ViewNextLine	ENDP

ViewGetChar3	PROC	NEAR
		LODSB
		CMP	CX,1
		JE	@@Exit
		DEC	CX
		CMP	BYTE PTR DS:[SI],0
		JE	@@SkipHi
		MOV	AL,'.'
		STC
  @@SkipHi:	INC	SI
  @@Exit:	RET
ViewGetChar3	ENDP

;---------------------------------------------------------------
;	Find previous line
;		   SI - offset of line
;	Return:	   SI - offset of previous line
;		   CY - not found
;---------------------------------------------------------------
ViewPrevLine	PROC	NEAR
		CMP	SI,1
		JB	@@Exit
		DEC	SI
		CALL	ViewBegLine
  @@Exit:	RET
ViewPrevLine	ENDP

;---------------------------------------------------------------
;	Find previous lines
;		   CX - number of lines
;		   SI - offset of line
;	Return:	   SI - offset of previous line
;		   CY - not found
;---------------------------------------------------------------
ViewPrevLines	PROC	NEAR
		USES	AX,DI
		MOV	WORD PTR ES:TmpBuf[0],0
  @@Line:	MOV	DI,WORD PTR ES:TmpBuf[0]
		CMP	DI,OFFSET TmpBuf+2
		JBE	@@PrevLine
		DEC	DI
		DEC	DI
		MOV	WORD PTR ES:TmpBuf[0],DI
		MOV	SI,ES:[DI]
		OR	SI,SI
		JNE	@@Next
		MOV	AX,DS:VwOffset.Lo
		OR	AX,DS:VwOffset.Hi
		JE	@@Next
		STC
		JMP	@@Exit
  @@PrevLine:	CALL	ViewPrevLine
		JC	@@Exit
  @@Next:	LOOP	@@Line
  @@Exit:	RET
ViewPrevLines	ENDP

;---------------------------------------------------------------
;	Output line
;		   AL - line number
;		   SI - line offset
;---------------------------------------------------------------
ViewPutLine	PROC	NEAR
		LOCAL	PatStart:WORD
		LOCAL	PatEnd  :WORD
		LOCAL	Limit	:WORD
		USES	AX,BX,CX,DX,SI,DI
		MOV	BX,DS:VwBase
		ADD	BH,AL
		PUSH	ES
		CALL	BasAdr
		POP	ES
		MOV	CX,0FFFFh
		MOV	AX,DS:VwSearch.Lo
		MOV	BX,DS:VwSearch.Hi
		ADD	AX,OFFSET VwBuffer+1
		ADC	BX,0
		SUB	AX,DS:VwOffset.Lo
		SBB	BX,DS:VwOffset.Hi
		JNE	@@InitVars
		MOV	CX,AX
		ADD	AX,DS:VwSrLen
		JNC	@@InitVars
		XOR	AX,AX
  @@InitVars:	DEC	AX
		MOV	PatStart,CX
		MOV	PatEnd,AX
		MOV	AX,[BP]
		ADD	AX,OFFSET VwBuffer
		MOV	Limit,AX
		CMP	DS:VwTextHex,0
		JNZ	@@HexMode
		XOR	DX,DX			; Text view
		MOV	AX,SI
		ADD	SI,OFFSET VwBuffer
		CMP	DS:VwTbl.Translate,OFFSET TransDBCS
		JNE	@@GetChar
		ADD	AX,DS:VwOffset.Lo
		ADD	AL,DS:VwShift
		AND	AL,1
		JZ	@@GetChar
		CMP	SI,Limit
		JAE	@@ExitText
		LODSB
		MOV	AH,AL
		MOV	AL,0
		JMP	@@Decode
  @@GetChar:	CMP	SI,Limit
		JAE	@@ExitText
		LODSW
		DEC	SI
		CMP	DS:VwTbl.Translate,OFFSET TransDBCS
		JNE	@@Decode
		CMP	SI,Limit
		JB	@@HiByte
		MOV	AH,0
  @@HiByte:	INC	SI
  @@Decode:	MOV	BX,OFFSET VwTbl
		CALL	DS:VwTbl.Translate
		CMP	AL,13
		JE	@@ExitText
		CMP	AL,10
		JE	@@ExitText
		MOV	CX,SI
		MOV	BX,DX
		INC	DX
		CMP	AL,9
		JNE	@@SubCol
		MOV	AL,' '
		TEST	DL,7
		JZ	@@SubCol
		DEC	SI
		CMP	DS:VwTbl.Translate,OFFSET TransDBCS
		JNE	@@SubCol
		DEC	SI
  @@SubCol:	SUB	BX,DS:VwColText
		JB	@@GetChar
		OR	BH,BH
		JNE	@@ExitText
		CMP	BL,DS:VwSize.X
		JB	@@PutChar
		CMP	DS:VwWrap,0
		JZ	@@Overflow
		CMP	AL,' '
		JE	@@GetChar
		CMP	AL,9
		JE	@@GetChar
		JMP	@@Overflow
  @@PutChar:	ADD	BX,BX
		PUSH	DS
		MOV	DS,ES:ScreenBuf
		MOV	DS:[DI+BX],AL
		POP	DS
		CMP	CX,PatStart
		JB	@@GetChar
		CMP	CX,PatEnd
		JA	@@GetChar
		CMP	DS:VwSrTextHex,0
		JNZ	@@GetChar
		MOV	AH,Cursr_C
		CALL	Color
		PUSH	DS
		MOV	DS,ES:ScreenBuf
		MOV	DS:[DI+BX],AX
		POP	DS
		JMP	@@GetChar
  @@Overflow:	MOV	CX,DS:VwColText
		JMP	@@RightArrow
  @@ExitText:	MOV	CX,DS:VwColText
		JMP	@@LeftArrow
  @@HexMode:	MOV	BX,1			; Hex view
		MOV	CX,DS:VwOffset.Lo
		MOV	DX,DS:VwOffset.Hi
		ADD	CX,SI
		ADC	DX,0
		MOV	AL,DH
		CALL	ViewHexByte
		MOV	AL,DL
		CALL	ViewHexByte
		MOV	AL,CH
		CALL	ViewHexByte
		MOV	AL,CL
		CALL	ViewHexByte
		ADD	SI,OFFSET VwBuffer	; Bytes
		INC	BX
		INC	BX
		XOR	CX,CX
		MOV	AX,DS:VwHexSize
		MOV	DX,AX
		ADD	DX,AX
		ADD	DX,AX
		DEC	AX
		SHR	AX,1
		SHR	AX,1
		ADD	DX,AX
		ADD	DX,12
		MOV	AH,Cursr_C
		CALL	Color
  @@GetByte:	CMP	SI,Limit
		JAE	@@ExitHex
		LODSB
		CALL	ViewHexByte
		INC	BX
		PUSH	BX
		PUSH	AX
		CMP	DS:VwTbl.Translate,OFFSET TransDBCS
		JNE	@@DecodeHex
		MOV	BX,SI
		SUB	BX,OFFSET VwBuffer
		ADD	BX,DS:VwOffset.Lo
		PUSH	BX
		ADD	BL,DS:VwShift
		AND	BL,1
		POP	BX
		JZ	@@Point
		MOV	AH,0
		CMP	BX,DS:VwLoaded
		JAE	@@DecodeHex
		MOV	AH,DS:[SI]
  @@DecodeHex:	MOV	BX,OFFSET VwTbl
		CALL	DS:VwTbl.Translate
		CMP	AL,0
		JNE	@@WriteByte
  @@Point:	MOV	AL,'.'
  @@WriteByte:	POP	BX
		MOV	AH,BH
		POP	BX
		XCHG	BX,DX
		CALL	ViewOutChar
		XCHG	BX,DX
		CMP	SI,PatStart
		JB	@@ColSpace
		CMP	SI,PatEnd
		JA	@@ColSpace
		PUSH	BX
		CMP	DS:VwSrTextHex,0
		JNZ	@@HexPat
		MOV	BX,DX
		JMP	@@TextPat
  @@HexPat:	DEC	BX
		DEC	BX
		CALL	ViewOutColor
		DEC	BX
		CALL	ViewOutColor
		CMP	SI,PatStart
		JE	@@ExitHexPat
		JCXZ	@@ExitHexPat
		DEC	BX
		CALL	ViewOutColor
  @@TextPat:	DEC	BX
		CALL	ViewOutColor
  @@ExitHexPat:	POP	BX
  @@ColSpace:	INC	CX
		TEST	CL,3
		JNE	@@GetByte
		INC	BX
		JMP	@@GetByte
  @@ExitHex:	MOV	CX,DS:VwColHex
		SUB	DX,CX
		JB	@@LeftArrow
		MOV	AL,DS:VwSize.X
		MOV	AH,0
		CMP	DX,AX
		JBE	@@LeftArrow
  @@RightArrow:	MOV	BL,DS:VwSize.X		; Arrows
		MOV	BH,0
		ADD	BX,BX
		MOV	AH,Arrow_C
		CALL	Color
		MOV	AL,ES:Scrolls[1]
		PUSH	DS
		MOV	DS,ES:ScreenBuf
		MOV	DS:[DI+BX-2],AX
		POP	DS
  @@LeftArrow:	JCXZ	@@Exit
		MOV	AH,Arrow_C
		CALL	Color
		MOV	AL,ES:Scrolls[0]
		PUSH	DS
		MOV	DS,ES:ScreenBuf
		MOV	DS:[DI],AX
		POP	DS
  @@Exit:	RET
ViewPutLine	ENDP

;---------------------------------------------------------------
;	Write hex byte
;		ES:DI - screen buffer
;		   AL - byte
;		   BX - offset
;	Return:	   BX - offset
;---------------------------------------------------------------
ViewHexByte	PROC	NEAR
		USES	AX
		CALL	HexByt
		CALL	ViewOutChar
		MOV	AL,AH
		CALL	ViewOutChar
		RET
ViewHexByte	ENDP

;---------------------------------------------------------------
;	Output character
;		ES:DI - screen buffer
;		   AL - character
;		   BX - offset
;	Return:	   BX - offset
;---------------------------------------------------------------
ViewOutChar	PROC	NEAR
		USES	DS
		PUSH	BX
		SUB	BX,DS:VwColHex
		JBE	@@Exit
		OR	BH,BH
		JNE	@@Exit
		CMP	BL,DS:VwSize.X
		JAE	@@Exit
		ADD	BX,BX
		MOV	DS,ES:ScreenBuf
		MOV	DS:[DI+BX],AL
  @@Exit:	POP	BX
		INC	BX
		RET
ViewOutChar	ENDP

;---------------------------------------------------------------
;	Output color
;		ES:DI - screen buffer
;		   AH - color
;		   BX - offset
;---------------------------------------------------------------
ViewOutColor	PROC	NEAR
		USES	DS
		PUSH	BX
		SUB	BX,DS:VwColHex
		JBE	@@Exit
		OR	BH,BH
		JNE	@@Exit
		CMP	BL,DS:VwSize.X
		JAE	@@Exit
		ADD	BX,BX
		MOV	DS,ES:ScreenBuf
		MOV	DS:[DI+BX+1],AH
  @@Exit:	POP	BX
		RET
ViewOutColor	ENDP

;---------------------------------------------------------------
;	Read file from offset
;		   CX - length
;		   BX - handle
;		DS:DX - buffer
;		DI SI - offset in file
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
ViewFileRead	PROC	NEAR
		USES	BX,DX,BP
		PUSH	ES
		PUSH	DI
		PUSH	CX
		PUSH	DS
		POP	ES
		MOV	DI,DX
		MOV	AL,0
		REP	STOSB
		POP	CX
		POP	DI
		POP	ES
		MOV	BP,DX
		PUSH	BX
		CMP	BX,0FFFEh
		JB	@@Seek
		PUSH	DS
		MOV	DS,ES:ViewSeg
		MOV	DX,OFFSET ViewName1
		JNE	@@Open
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET DirInfoFile
		MOV	AX,DS:DirInfoNumb
		CMP	AX,0FFFFh
		JE	@@Open
		PUSH	DI
		PUSH	SI
		MOV	SI,OFFSET VcTempArc_S
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		CALL	MakePathName
		CALL	AddFile
		MOV	SI,DX
		CALL	MvzLine
		POP	DX
		POP	SI
		POP	DI
  @@Open:	MOV	AL,40h
		CALL	OpenFil
		POP	DS
		JC	@@OpenErr
		MOV	BX,AX
  @@Seek:	PUSH	CX
		MOV	DX,SI
		MOV	CX,DI
		MOV	AX,4200h
		CALL	Intr21
		POP	CX
		JC	@@Close
		XOR	AX,SI
		XOR	DX,DI
		OR	DX,AX
		XOR	AX,AX
		CMP	AX,DX
		JB	@@Close
		MOV	DX,BP
		MOV	AH,3Fh
		CALL	Intr21
  @@Close:	POP	DX
		PUSHF
		PUSH	AX
		CMP	DX,0FFFEh
		JB	@@Exit
		MOV	AH,3Eh
		CALL	Intr21
		JNC	@@Exit
		POP	DX
  @@OpenErr:	POP	DX
		PUSHF
		PUSH	AX
  @@Exit:	POP	AX
		POPF
		RET
ViewFileRead	ENDP

;---------------------------------------------------------------
;	Read more data in buffer
;	Return:	   NC - no data loaded
;---------------------------------------------------------------
ViewReadMore	PROC	NEAR
		USES	AX,BX,CX,DX,SI,DI
		MOV	DX,DS:VwSpace
		MOV	AX,DS:VwLen.Lo
		MOV	BX,DS:VwLen.Hi
		SUB	AX,DS:VwOffset.Lo
		SBB	BX,DS:VwOffset.Hi
		JNE	@@SetSize
		CMP	DX,AX
		JBE	@@SetSize
		MOV	DX,AX
  @@SetSize:	MOV	CX,DX
		XCHG	DX,DS:VwLoaded
		SUB	CX,DX
		JE	@@Exit
		MOV	SI,DS:VwOffset.Lo
		MOV	DI,DS:VwOffset.Hi
		ADD	SI,DX
		ADC	DI,0
		ADD	DX,OFFSET VwBuffer
		MOV	BX,DS:VwHandle
		CALL	ViewFileRead
		STC
  @@Exit:	RET
ViewReadMore	ENDP

;---------------------------------------------------------------
;	Read next part
;---------------------------------------------------------------
ViewReadNext	PROC	NEAR
		USES	AX
		MOV	AX,DS:VwSpace
		SHR	AX,1
		CALL	ViewReadNext1
		RET
ViewReadNext	ENDP

ViewReadNext1	PROC	NEAR
		USES	CX,SI,DI
		MOV	SI,DS:VwLen.Lo
		MOV	DI,DS:VwLen.Hi
		SUB	SI,DS:VwOffset.Lo
		SBB	DI,DS:VwOffset.Hi
		SUB	SI,DS:VwSpace
		SBB	DI,0
		JNE	@@SetOffset
		CMP	AX,SI
		JBE	@@SetOffset
		MOV	AX,SI
  @@SetOffset:	ADD	DS:VwOffset.Lo,AX
		ADC	DS:VwOffset.Hi,0
		SUB	DS:VwFirst,AX
		MOV	CX,DS:VwLoaded
		SUB	CX,AX
		JAE	@@Move
		XOR	CX,CX
  @@Move:	MOV	DS:VwLoaded,CX
		OR	AX,AX
		JE	@@Read
		MOV	DI,OFFSET VwBuffer
		MOV	SI,AX
		ADD	SI,DI
		PUSH	ES
		PUSH	DS
		POP	ES
		REP	MOVSB
		POP	ES
  @@Read:	CALL	ViewReadMore
		RET
ViewReadNext1	ENDP

;---------------------------------------------------------------
;	Read previous part
;---------------------------------------------------------------
ViewReadPrev	PROC	NEAR
		USES	AX
		MOV	AX,DS:VwSpace
		SHR	AX,1
		CALL	ViewReadPrev1
		RET
ViewReadPrev	ENDP

ViewReadPrev1	PROC	NEAR
		USES	BX,CX,DX,SI,DI
		MOV	DX,AX
		CMP	DS:VwOffset.Hi,0
		JNE	@@SetOffset
		MOV	AX,DS:VwOffset.Lo
		CMP	DX,AX
		JBE	@@SetOffset
		MOV	DX,AX
  @@SetOffset:	SUB	DS:VwOffset.Lo,DX
		SBB	DS:VwOffset.Hi,0
		ADD	DS:VwFirst,DX
		MOV	CX,DS:VwSpace
		MOV	AX,DS:VwLoaded
		ADD	AX,DX
		JC	@@SetSize
		CMP	AX,CX
		JAE	@@SetSize
		MOV	CX,AX
  @@SetSize:	MOV	DS:VwLoaded,CX
		MOV	DI,CX
		SUB	CX,DX
		ADD	DI,OFFSET VwBuffer-1
		MOV	SI,DI
		SUB	SI,DX
		PUSH	ES
		PUSH	DS
		POP	ES
		STD
		REP	MOVSB
		CLD
		POP	ES
		MOV	CX,DX
		MOV	SI,DS:VwOffset.Lo
		MOV	DI,DS:VwOffset.Hi
		MOV	DX,OFFSET VwBuffer
		MOV	BX,DS:VwHandle
		CALL	ViewFileRead
		RET
ViewReadPrev1	ENDP

;---------------------------------------------------------------
;	Scroll screen up / down
;		   AH - 6-down, 7-up
;---------------------------------------------------------------
ViewScroll	PROC	NEAR
		USES	AX,BX,CX,DX,SI,DI
		MOV	CL,DS:VwSize.Y
		MOV	CH,0
		DEC	CX
		JCXZ	@@Exit
		MOV	DL,ES:Columns
		MOV	DH,0
		ADD	DX,DX
		CMP	AH,6
		JE	@@Offset
		NEG	DX
  @@Offset:	PUSH	ES
		PUSH	DS
		PUSH	AX
		MOV	AL,DS:VwSize.X
		CALL	BasAdr
		PUSH	ES
		POP	DS
		MOV	SI,DI
		ADD	SI,DX
  @@MoveLine:	PUSH	DI
		PUSH	SI
		PUSH	CX
		MOV	CL,AL
		REP	MOVSW
		POP	CX
		POP	SI
		POP	DI
		ADD	SI,DX
		ADD	DI,DX
		LOOP	@@MoveLine
		MOV	AH,Back_C
		CALL	Color
		MOV	BH,AH
		POP	AX
		POP	DS
		POP	ES
		MOV	CX,DS:VwBase
		MOV	DX,DS:VwSize
		ADD	DX,CX
		SUB	DX,101h
		MOV	AL,1
		CALL	Int10m
  @@Exit:	RET
ViewScroll	ENDP

;---------------------------------------------------------------
;	Start searching
;		   AL - text / hex search
;		CS:DX - pointers table for DialBox
;---------------------------------------------------------------
StartSearch	PROC	NEAR
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	BX,500h
		MOV	BP,OFFSET SearchFor_P
		CALL	DialBox
		CALL	Window
		POP	DS
		MOV	SI,OFFSET VwTbl
		CALL	PrepareSearch
		RET
StartSearch	ENDP
