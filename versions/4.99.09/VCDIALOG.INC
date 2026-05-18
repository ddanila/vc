;---------------------------------------------------------------
;		Boxes and dialogs
;
;		Version:	12.07.2000
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Zooming box
;		BH,BL - begin
;		DH,DL - size
;		   AH - color
;---------------------------------------------------------------
		.CODE	Subs
MinBox		PROC	FAR
		USES	AX,CX,DI
		MOV	ES,CS:DataSegment
		MOV	AL,0
		MOV	ES:Quote,AL
		CMP	AL,ES:ZoomWin
		JZ	@@PutBox
		PUSH	AX
		MOV	AH,ShadFl
		CALL	ColAddr
		POP	AX
		XCHG	AL,ES:[DI]
		PUSH	DI
		PUSH	AX
		XOR	DI,DI
		CALL	MinBxTick
		MOV	CL,ES:ZoomStages
		MOV	CH,0
		DEC	CX
  @@NextZoom:	PUSH	DX
		PUSH	CX
		PUSH	BX
		PUSH	AX
		MOV	AL,DL
		MOV	AH,BL
		MOV	CH,11
		CALL	MinBxCorr
		MOV	DL,AL
		MOV	BL,AH
		MOV	AL,DH
		MOV	AH,BH
		MOV	CH,5
		CALL	MinBxCorr
		MOV	DH,AL
		MOV	BH,AH
		POP	AX
		CALL	ShadBox
		CALL	MinBxTick
		CALL	Window
		POP	BX
		POP	CX
		POP	DX
		LOOP	@@NextZoom
		CALL	MinBxTick
		POP	AX
		POP	DI
		MOV	ES:[DI],AL
  @@PutBox:	CALL	ShadBox
		RET
MinBox		ENDP

MinBxTick	PROC	NEAR			; Wait for tick
		USES	CX,DX
  @@WaitTick:	CALL	Timer
		CMP	DX,DI
		JE	@@WaitTick
		MOV	DI,DX
		RET
MinBxTick	ENDP

MinBxCorr	PROC	NEAR
		PUSH	AX
		SUB	AL,CH
		SHR	AL,1
		MUL	CL
		MOV	CH,ES:ZoomStages
		DEC	CH
		DIV	CH
		MOV	CH,AL
		POP	AX
		ADD	AH,CH
		SUB	AL,CH
		SUB	AL,CH
		RET
MinBxCorr	ENDP

;---------------------------------------------------------------
;	Box with shadow
;		BH,BL - begin
;		DH,DL - size
;		   AH - color
;---------------------------------------------------------------
ShadBox		PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,ES
		MOV	CL,DH
		MOV	CH,0
		MOV	DI,CX
		SUB	DL,10
		MOV	DH,0
		MOV	SI,3
		MOV	CX,'  '
		MOV	AL,' '
		CALL	BoxLine
		MOV	CX,WORD PTR ES:Borders[0]
		MOV	AL,ES:Borders[2]
		CALL	ShadLin
		SUB	DI,5
		JBE	@@SkipLines
		MOV	CX,WORD PTR ES:Borders[6]
		MOV	AL,' '
  @@NextLine:	CALL	ShadLin
		DEC	DI
		JNE	@@NextLine
  @@SkipLines:	MOV	CX,WORD PTR ES:Borders[3]
		MOV	AL,ES:Borders[5]
		CALL	ShadLin
		MOV	CX,'  '
		MOV	AL,' '
		CALL	ShadLin
		CALL	BasAdr
		ADD	DI,2*2
		MOV	CX,DX
		ADD	CL,8
  @@Shadow:	CALL	Shadow
		LOOP	@@Shadow
		RET
ShadBox		ENDP

;---------------------------------------------------------------
;	Box
;		BH,HL - coordinates
;		   AH - color
;		    ╔═════════════════╗
;		    ║                 ║   ┐
;		    ║                 ║   ├ DI
;		    ║                 ║   ┘
;		    ╚═════════════════╝
;		 └┬┘ └───────────────┘ └┬┘
;		  CX         DX         CX
;---------------------------------------------------------------
BoxDubl		PROC	FAR
		USES	AX,CX,SI,DI
		MOV	SI,CX
		MOV	CX,WORD PTR ES:Borders[0]
		MOV	AL,ES:Borders[2]
		CALL	BoxLine
		MOV	CX,WORD PTR ES:Borders[6]
		MOV	AL,' '
  @@Next:	CALL	BoxLine
		DEC	DI
		JNE	@@Next
		MOV	CX,WORD PTR ES:Borders[3]
		MOV	AL,ES:Borders[5]
		CALL	BoxLine
		RET
BoxDubl		ENDP

;---------------------------------------------------------------
;	One line of box
;		BH,BL - coordinates
;		   AH - color
;		│   ╟───────────────────────╢   │
;		 └┬┘|└──────────┬──────────┘|└┬┘
;		 SI CL         DX(AL)      CH SI
;---------------------------------------------------------------
ShadLin		PROC	FAR
		USES	DI,ES
		CALL	BasAdr
		CALL	BoxLn0
		CALL	Shadow
		CALL	Shadow
		INC	BH
		RET
ShadLin		ENDP

BoxLine		PROC	FAR
		USES	DI,ES
		CALL	BasAdr
		CALL	BoxLn0
		INC	BH
		RET
BoxLine		ENDP

BoxLn0		PROC	FAR
		USES	AX,CX
		PUSH	AX
		PUSH	CX
		MOV	CX,SI
		JCXZ	@@Line
		MOV	AL,' '
		REP	STOSW
  @@Line:	POP	CX
		MOV	AL,CL
		STOSW
		POP	AX
		PUSH	CX
		MOV	CX,DX
		REP	STOSW
		POP	CX
		MOV	AL,CH
		STOSW
		MOV	CX,SI
		JCXZ	@@Exit
		MOV	AL,' '
		REP	STOSW
  @@Exit:	RET
BoxLn0		ENDP

;---------------------------------------------------------------
;	Zoom box only once
;		BH,BL - begin-center
;			(BL = 0 - cnt of scr, 1 - cnt of panel)
;			(BH > 127 - center of screen)
;		CS:DX - table of pointers
;		CS:BP - pointer:
;			DB  NormCol,RevCol,BoldCol,UndrCol
;			DW  OFFSET Line1
;			. . .
;			DW  OFFSET LineX
;			DW  0
;	Return:	BH,BL - begin
;		DH,DL - size
;---------------------------------------------------------------
		.CODE	Subs
OncZoom		PROC	FAR
		USES	AX
		MOV	AL,ES:ZoomWin
		CMP	ES:ZoomOne,0
		JZ	@@DialBox
		PUSH	DX
		PUSH	BX
		CALL	ResWin
		POP	BX
		POP	DX
		MOV	ES:ZoomWin,0
  @@DialBox:	CALL	DialBox
		CALL	Window
		MOV	ES:ZoomWin,AL
		MOV	ES:ZoomOne,1
		RET
OncZoom		ENDP

;---------------------------------------------------------------
;	Put window
;		BH,BL - begin-center
;			(BL = 0 - cnt of scr, 1 - cnt of panel)
;			(BH > 127 - center of screen)
;		CS:DX - table of pointers
;		CS:BP - pointer:
;			DB  NormCol,RevCol,BoldCol,UndrCol
;			DW  OFFSET Line1
;			. . .
;			DW  OFFSET LineX
;			DW  0
;	Return:	BH,BL - begin
;		DH,DL - size
;---------------------------------------------------------------
DialBox		PROC	FAR
		USES	AX,CX,SI,DI,DS
		CALL	HidCur
		PUSH	DX
		CMP	BL,1
		JA	@@BoxSize
		CALL	SizDX
		CMP	BL,0
		MOV	BL,DL
		JE	@@BoxSize
		MOV	DH,BH
		CALL	BegBX
		SHR	DL,1
		ADD	BL,DL
		MOV	BH,DH
  @@BoxSize:	POP	DX
		PUSH	CS			; Calculate size of window
		POP	DS
		PUSH	BX
		LEA	DI,[BP+4]
		PUSH	DI
		XOR	BX,BX
  @@LenLine:	MOV	SI,DS:[DI]
		INC	DI
		INC	DI
		INC	BH
		INC	SI
		JE	@@LenLine
		INC	SI
		JNE	@@ChkLine
		DEC	BH
		ADD	BH,ES:ListWindow.Y
		MOV	AL,ES:ListWindow.X
		JMP	@@ChkWidth
  @@ChkLine:	DEC	SI
		DEC	SI
		JE	@@EndCheck
		MOV	AL,0
		CALL	NgLine
  @@ChkWidth:	CMP	AL,BL
		JBE	@@LenLine
		MOV	BL,AL
		JMP	@@LenLine
  @@EndCheck:	MOV	DI,DX
		LEA	DX,[BX+30Ch]
		POP	CX
		POP	BX
		TEST	BH,80h			; Put box
		JZ	@@ShiftEGA
		MOV	AL,ES:Lines
		INC	AX
		SUB	AL,DH
		SHR	AL,1
		AND	BH,7Fh
		CALL	SftEGA
		CMP	BH,AL
		JBE	@@PutBox
		MOV	BH,AL
		JMP	@@PutBox
  @@ShiftEGA:	CALL	SftEGA
  @@PutBox:	PUSH	BX
		MOV	AL,DL
		DEC	AX
		SHR	AL,1
		SUB	BL,AL
		CALL	SavWin
		MOV	AH,CS:[BP]
		CALL	Color
		CALL	MinBox
		POP	AX
		PUSH	BX			; Output lines
		MOV	BX,AX
		MOV	SI,CX
  @@WriteLine:	INC	BH
		PUSH	SI
		PUSH	DI
		PUSH	DX
		MOV	SI,DS:[SI]
		INC	SI
		JE	@@Separator
		INC	SI
		JE	@@ListWin
		DEC	SI
		DEC	SI
		JE	@@Exit
		MOV	DX,DI
		CALL	CntLine
		JMP	@@NextLine
  @@Separator:	PUSH	BX
		MOV	DH,0
		MOV	AX,DX
		DEC	AX
		SHR	AX,1
		SUB	BL,AL
		SUB	DL,10
		MOV	SI,3
		MOV	CX,WORD PTR ES:Borders[8]
		MOV	AL,ES:Borders[10]
		MOV	AH,DS:[BP]
		CALL	Color
		CALL	BoxLine
		POP	BX
		JMP	@@NextLine
  @@ListWin:	DEC	BH
		ADD	BH,ES:ListWindow.Y
  @@NextLine:	POP	DX
		POP	DI
		POP	SI
		INC	SI
		INC	SI
		JMP	@@WriteLine
  @@Exit:	POP	DX
		POP	DI
		POP	SI
		POP	BX
		RET
DialBox		ENDP

;---------------------------------------------------------------
;	Output NG line
;		BH,BL - coordinates
;		DS:SI - address of line
;		CS:DX - address of points table
;		CS:BP - address of color table
;		   AL - limit (0 - check length)
;			Alailabe macros:
;			0FFh,n		- insert n spaces (n<128)
;			0FFh,n,Char	- duplicate Char n-128 times (n>128)
;			^^		- insert ^
;			^N		- normal text
;			^R		- reverced text
;			^B		- bold text
;			^U		- underlined text
;			^Axx		- insert ASCII char (xx - hex)
;			^Cxx		- set color xx (xx - hex)
;			^nnnC[N|P][mm]	- insert line from CS:
;			^nnnE[N|P][mm]	- insert line from ES:
;			^nnnI[mm]	- insert integer number
;			^nnnL[mm]	- insert long integer number
;			^nnnB		- insert hex byte
;			^nnnW		- insert hex word
;			^nnnPs1/s2/	- insert string s1 if 1, or s2 if not
;			^nnnQs1/s2/	- insert string s1 if 1, or s2 if not
;	Return:	   AX - length of string (if AL was 0)
;---------------------------------------------------------------
NgLine		PROC	FAR
		LOCAL	StringBuf:BYTE:128
		USES	BX,CX,DI,ES
		PUSH	AX
		CALL	BasAdr
		CMP	AL,0
		JNE	@@Begin
		XOR	DI,DI
		MOV	AL,0FFh
  @@Begin:	MOV	BL,AL
		MOV	CL,0
  @@SetColor:	MOV	CH,0
		MOV	BH,CL
		PUSH	SI
		MOV	SI,SS:[BP]
		ADD	SI,CX
		MOV	AH,CS:[SI]
		POP	SI
		CALL	Color
  @@NextChar:	LODSB
		CMP	AL,0
		JE	@@Exit
		CMP	AL,'^'
		JE	@@GetMacro
		CMP	AL,0FFh
		JE	@@Duplicate
  @@SaveChar:	CALL	NgLnSaveChar
		JMP	@@NextChar
  @@Duplicate:	LODSB				; Duplicate space or char
		MOV	CL,AL
		TEST	AL,80h
		MOV	AL,' '
		JZ	@@ChkZero
		LODSB
  @@ChkZero:	CALL	NgLnFillChar
		JMP	@@NextChar
  @@GetMacro:	LODSB				; Get macro
		CMP	AL,'^'
		JE	@@SaveChar
		CALL	UpCase
		MOV	CL,0			; Color macros
		CMP	AL,'N'
		JE	@@RestrCol
		MOV	CL,1
		CMP	AL,'R'
		JE	@@RestrCol
		MOV	CL,2
		CMP	AL,'B'
		JE	@@RestrCol
		MOV	CL,3
		CMP	AL,'U'
		JNE	@@HexMacros
  @@RestrCol:	CMP	CL,BH
		JNE	@@SetColor
		MOV	CL,0
		JMP	@@SetColor
  @@HexMacros:	CMP	AL,'A'			; ^Axx and ^Cxx macros
		JE	@@GetHex
		CMP	AL,'C'
		JNE	@@NumMacros
  @@GetHex:	MOV	CL,AL
		LODSB
		CALL	TstHex
		MOV	CH,AL
		LODSB
		JC	@@NextChar
		CALL	TstHex
		JC	@@NextChar
		PUSH	CX
		MOV	CL,4
		SHL	CH,CL
		OR	AL,CH
		POP	CX
		CMP	CL,'C'
		JNE	@@SaveChar
		MOV	AH,AL
		MOV	BH,0
		JMP	@@NextChar
  @@NumMacros:	DEC	SI			; Numeric macros
		PUSH	AX
		CALL	TxtNum
		MOV	CX,AX
		POP	AX
		JC	@@NextChar
		LODSB
		CALL	UpCase
		PUSH	DS
		PUSH	SI
		MOV	SI,CX
		ADD	SI,SI
		ADD	SI,DX
		MOV	SI,CS:[SI]
		MOV	DS,CS:DataSegment
		CMP	AL,'C'
		JE	@@StringCS
		CMP	AL,'E'
		JE	@@StringES
		CMP	AL,'I'
		JE	@@Integer
		CMP	AL,'L'
		JE	@@LongInt
		CMP	AL,'B'
		JE	@@HexByte
		CMP	AL,'W'
		JE	@@HexWord
		CMP	AL,'P'
		JE	@@PluralInt
		CMP	AL,'Q'
		JE	@@PluralLong
  @@GoNextChar:	POP	SI
		POP	DS
		JMP	@@NextChar
  @@StringCS:	PUSH	CS			; ^nnnC - string ftom CS:
		POP	DS
  @@StringES:	PUSH	ES			; ^nnnE - string ftom ES:
		PUSH	DI
		PUSH	SS
		POP	ES
		LEA	DI,StringBuf
		MOV	CX,127
		REP	MOVSB
		MOV	AL,0
		STOSB
		POP	DI
		POP	ES
  @@CutString:	POP	SI
		POP	DS
		PUSH	AX
		LODSB
		DEC	SI
		CALL	UpCase
		CMP	AL,'N'
		JE	@@PathLin
		CMP	AL,'P'
		JNE	@@NoPathLin
  @@PathLin:	PUSH	ES
		PUSH	SI
		PUSH	SS
		POP	ES
		LEA	SI,StringBuf
		CMP	AL,'N'
		JE	@@LetLin
		CALL	PathLin
		JMP	@@ExPathLin
  @@LetLin:	CALL	LetLin
  @@ExPathLin:	POP	SI
		POP	ES
		INC	SI
  @@NoPathLin:	CALL	TxtNum
		MOV	CX,AX
		POP	AX
		JC	@@SkipCut
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	AX
		PUSH	SS
		POP	ES
		LEA	SI,StringBuf
		MOV	AX,CX
		CALL	ShrtLin
		MOV	DI,SI
		PUSH	AX
		CALL	StrLen
		POP	AX
		DEC	CX
		SUB	AX,CX
		MOV	CX,AX
		POP	AX
		POP	SI
		POP	DI
		POP	ES
		LODSB
		CMP	AL,'>'
		JE	@@SkipCut
		XCHG	CL,CH
		CMP	AL,'<'
		JE	@@SkipCut
		DEC	SI
		XOR	CX,CX
  @@SkipCut:	PUSH	DS
		PUSH	SI
		MOV	AL,' '
		CALL	NgLnFillChar
		PUSH	SS
		POP	DS
		LEA	SI,StringBuf
  @@MoveChar:	LODSB
		CMP	AL,0
		JE	@@EndSpace
		CALL	NgLnSaveChar
		JMP	@@MoveChar
  @@EndSpace:	XCHG	CL,CH
		MOV	AL,' '
		CALL	NgLnFillChar
		JMP	@@GoNextChar
  @@Integer:	PUSH	BX			; ^nnnI - integer number
		XOR	BX,BX
		JMP	@@Number
  @@LongInt:	PUSH	BX			; ^nnnL - long integer number
		MOV	BX,DS:[SI].Hi
  @@Number:	MOV	CX,DS:[SI].Lo
		PUSH	ES
		PUSH	DI
		PUSH	SS
		POP	ES
		LEA	DI,StringBuf
		PUSH	ES
		PUSH	DI
		MOV	AL,1
		CALL	Number
		POP	SI
		POP	DS
		POP	DI
		POP	ES
		POP	BX
		JMP	@@CutString
  @@HexWord:	MOV	AL,DS:[SI+1]		; ^nnnW - hex word
		CALL	NgLnHexByte
  @@HexByte:	MOV	AL,DS:[SI]		; ^nnnB - hex byte
		CALL	NgLnHexByte
		JMP	@@GoNextChar
  @@PluralLong:	CMP	WORD PTR DS:[SI].Hi,0	; ^nnnQ - single or plural
		JNE	@@Plural
  @@PluralInt:	CMP	WORD PTR DS:[SI].Lo,1	; ^nnnP - single or plural
  @@Plural:	POP	SI
		POP	DS
		MOV	CX,2
		JE	@@WritePart
		NOT	CH
  @@WritePart:	LODSB
		CMP	AL,0
		JE	@@Exit
		CMP	AL,'/'
		JE	@@NextPart
		OR	CH,CH
		JNE	@@WritePart
		CALL	NgLnSaveChar
		JMP	@@WritePart
  @@NextPart:	NOT	CH
		DEC	CL
		JNE	@@WritePart
		JMP	@@NextChar
  @@Exit:	POP	AX
		CMP	BL,0FFh
		JNE	@@NoReturn
		MOV	AX,DI
  @@NoReturn:	RET
NgLine		ENDP

NgLnSaveChar	PROC	NEAR			; Save Char
		INC	DI
		CMP	BL,0FFh
		JE	@@Exit
		DEC	DI
		OR	BL,BL
		JE	@@Exit
		STOSW
		DEC	BL
  @@Exit:	RET
NgLnSaveChar	ENDP

NgLnHexByte	PROC	NEAR			; Write hex byte
		PUSH	AX
		CALL	HexByt
		MOV	CX,AX
		PUSH	AX
		MOV	AL,CL
		CALL	NgLnSaveChar
		MOV	AL,CH
		CALL	NgLnSaveChar
		RET
NgLnHexByte	ENDP

NgLnFillChar	PROC	NEAR
		USES	CX
		AND	CX,7Fh
		JE	@@Exit
  @@NextDup:	CALL	NgLnSaveChar
		LOOP	@@NextDup
  @@Exit:	RET
NgLnFillChar	ENDP

;---------------------------------------------------------------
;	Center line
;		BH,BL - center
;		DS:SI - address of line
;		CS:DX - address of points table
;		CS:BP - address of color table
;---------------------------------------------------------------
CntLine		PROC	FAR
		USES	AX,BX
		PUSH	SI
		MOV	AL,0
		CALL	NgLine
		POP	SI
		PUSH	AX
		INC	AX
		SHR	AX,1
		SUB	BL,AL
		POP	AX
		CALL	NgLine
		RET
CntLine		ENDP

;---------------------------------------------------------------
;	Dialog
;		CS:DX - address of points table
;		CS:BP - pointer:
;			DW  BoxPoint, BegCenter, SecondBox
;			DB  KeyBar, Menu, HlpPage, Status
;				DB  Type
;				DB  Up, Down, Left, Right
;				DW  Begin, Point, Control
;				...
;			DB  0
;			Status:	b0 - ClrKbd
;				b1 - Beep
;				b2 - SftErr
;				b3 - F10-Tree
;				b4 - disable DialBox
;---------------------------------------------------------------
		.DATA
Dial1_T		DW	3B00h, DialogHelp
		DW	1C0Dh, DialogEnter
		DW	1C0Ah, DialogEnter
		DW	001Bh, DialogEsc
		DW	4400h, DialogEsc
		DW	0
Dial2_T		DW	0009h, DialogTab
		DW	0F00h, DialogShTab
		DW	4800h, DialogUp
		DW	5000h, DialogDown
		DW	4B00h, DialogLeft
		DW	4D00h, DialogRight
		DW	4700h, DialogHome
		DW	4F00h, DialogEnd
		DW	4900h, DialogHome
		DW	5100h, DialogPgDn
		DW	0
Dial3_T		DW	0005h, DialogHistUp
		DW	0018h, DialogHistDn
		DW	0

		.CODE	Subs
Dialog		PROC	FAR
		USES	BX,CX,DX,SI,DI
		MOV	AL,0
		MOV	ES:Quote,AL
		XCHG	ES:MinMnuFlag,AL
		PUSH	AX
		CALL	ChgCur
		PUSH	CX
		PUSH	BP			; Output box
		MOV	AL,CS:[BP].bMenu
		TEST	CS:[BP].bFlags,10h
		JNZ	@@PutMenu
		MOV	BX,CS:[BP].BoxBase
		CMP	AL,1
		JNE	@@PutMenu
		MOV	CX,DS
		CMP	CX,ES:WCB1seg
		JE	@@PutMenu
		MOV	AL,5
  @@PutMenu:	CMP	AL,Def_M
		JE	@@PutKeyBar
		AND	AL,7Fh
		CALL	PutMnu
  @@PutKeyBar:	MOV	AL,CS:[BP].bKeyBar
		CALL	PutKey
		TEST	CS:[BP].bFlags,4
		JZ	@@PutDialBox
		ADD	BH,ES:SftErr
  @@PutDialBox:	TEST	CS:[BP].bFlags,10h
		JNZ	@@SkipBox
		MOV	BP,CS:[BP].pDiBox
		CALL	DialBox
  @@SkipBox:	POP	BP
		MOV	SI,BP
		MOV	AX,0FFFEh
		CALL	DialogSubFunc
		PUSH	BP			; Init
		LEA	SI,[BP]+SIZE BoxHd
		MOV	BP,SP
  @@InitItem:	MOV	AL,CS:[SI].mType
		CMP	AL,1			; InpStr
		JNE	@@InitSel
		MOV	AX,CS:[SI].mCtrl
		MOV	CX,AX
		MOV	AH,0
		AND	CL,7Fh
		OR	CH,CH
		JE	@@InitInLen2
		CMP	CL,InpModePoint
		JA	@@InitInLen1
		MOV	CH,0
		MOV	AH,1
		JMP	@@InitInLen2
  @@InitInLen1:	CMP	CH,CL
		MOV	CL,0
		JBE	@@InitInLen2
		XCHG	CL,CH
  @@InitInLen2:	SUB	SP,CX
		SUB	SP,7
		MOV	DI,SP
		PUSH	ES
		PUSH	SI
		PUSH	BX
		CALL	DialogBase
		MOV	SI,CS:[SI].mPoint
		PUSH	AX
		PUSH	SS
		POP	ES
		MOV	AL,1
		STOSB
		MOV	AL,0
		STOSB
		STOSB
		STOSB
		STOSB
		MOV	AL,AH
		STOSB
		PUSH	SI
		REP	MOVSB
		MOV	AL,0
		STOSB
		POP	SI
		MOV	DI,[BP]
		MOV	DI,CS:[DI].pDiBox
		MOV	AH,CS:[DI+1]
		POP	CX
		AND	CX,7Fh
		PUSH	BP
		XOR	BP,BP
		CALL	InpStrOut
		POP	BP
		JMP	@@InitNext
  @@InitSel:	CMP	AL,2			; Select
		JNE	@@InitFlip
		DEC	SP
		DEC	SP
		MOV	DI,SP
		PUSH	ES
		PUSH	SI
		PUSH	BX
		CALL	DialogBase
		MOV	SI,CS:[SI].mPoint
		LODSB
		MOV	AH,AL
		MOV	SS:[DI],AX
		ADD	BH,AL
		CALL	BasAdr
		MOV	BYTE PTR ES:[DI],''
		JMP	@@InitNext
  @@InitFlip:	CMP	AL,3			; FlipFlop
		JNE	@@InitMenu
		DEC	SP
		MOV	DI,SP
		PUSH	ES
		PUSH	SI
		PUSH	BX
		CALL	DialogBase
		MOV	SI,CS:[SI].mPoint
		LODSB
		MOV	SS:[DI],AL
		CMP	AL,0
		JZ	@@InitNext
		CALL	BasAdr
		MOV	BYTE PTR ES:[DI],'x'
		JMP	@@InitNext
  @@InitMenu:	CMP	AL,4			; MinMnu
		JNE	@@InitEnd
		DEC	SP
		MOV	DI,SP
		PUSH	ES
		PUSH	SI
		PUSH	BX
		CALL	DialogBase
		DEC	BH
		MOV	AL,DL
		DEC	AX
		SHR	AL,1
		CBW
		ADD	BX,AX
		MOV	CX,CS:[SI].mCtrl
		CMP	CL,0FFh
		JNE	@@MinMnuDef
		MOV	CL,ES:MinMnuDefault
  @@MinMnuDef:	MOV	SS:[DI],CL
		MOV	SI,CS:[SI].mPoint
		CALL	BasAdr
		MOV	BX,[BP]
		MOV	BX,CS:[BX].pDiBox
		MOV	AH,CS:[BX+2]
		CALL	MnMn41
  @@InitNext:	POP	BX
		POP	SI
		POP	ES
		ADD	SI,SIZE BoxMn
		JMP	@@InitItem
  @@InitEnd:	CALL	Window			; Output window
		MOV	SI,[BP]
		MOV	AX,0FFFFh
		CALL	DialogSubFunc
		MOV	AL,CS:[SI].bHelp
		CMP	AL,0
		JE	@@ClrKeyb
		MOV	ES:HlpPage,AL
  @@ClrKeyb:	MOV	AL,CS:[SI].bFlags
		TEST	AL,1
		JZ	@@Beep
		CALL	ClrKbd
  @@Beep:	TEST	AL,2
		JZ	@@Dialogs
		CALL	Beep

  @@Dialogs:	LEA	DI,[SI]+SIZE BoxHd	; Menus
  @@DialNext:	PUSH	DI
		PUSH	BX
		PUSH	BP
		PUSH	DS
		PUSH	SS
		POP	DS
		MOV	SI,DI
		CALL	DialogBase
		CALL	DialogBuf
		MOV	BP,[BP]
		MOV	AL,CS:[BP].bKeyBar
		MOV	BP,CS:[BP].pDiBox
		MOV	CL,CS:[DI].mType
		CMP	CL,1			; InpStr
		JNE	@@DialSel
		ADD	SI,6
  @@DialInpStr:	PUSH	DI
		MOV	CX,CS:[DI].mCtrl
		AND	CL,7Fh
		MOV	AH,CS:[BP+1]
		MOV	DI,DS:[SI-5]
		CALL	InpStr
		MOV	DS:[SI-5],DI
		POP	DI
		CMP	CL,InpModePoint
		JBE	@@DialEnd
		PUSH	SI
		MOV	SI,OFFSET Dial3_T
		CALL	Case
		POP	SI
		JMP	@@DialEnd
DialogHistUp:	POP	SI			; History Up
		POP	DS
		POP	BP
		MOV	AL,2
		MOV	AH,SS:[SI-6]
		INC	AH
		JNE	@@DialHist1
		DEC	AH
		JMP	@@DialHist1
DialogHistDn:	POP	SI			; History Down
		POP	DS
		POP	BP
		MOV	AL,0
		MOV	AH,SS:[SI-6]
		OR	AH,AH
		JE	@@DialHist1
		DEC	AH
  @@DialHist1:	PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	DS
		POP	ES
		PUSH	CS:[DI].mCtrl
		MOV	DI,CS:[DI].mPoint
		CMP	AH,1
		JNE	@@DialHist2
		CMP	BYTE PTR DS:[DI],0
		JNE	@@DialHist2
		MOV	AH,AL
  @@DialHist2:	MOV	AL,0
		MOV	CL,AH
		MOV	CH,0
		OR	CX,CX
		JE	@@DialHist4
		DEC	CX
		JE	@@DialHist4
  @@DialHist3:	MOV	SI,DI
		PUSH	CX
		MOV	CH,0FFh
		REPNE	SCASB
		POP	CX
		CMP	AL,DS:[DI]
		LOOPNE	@@DialHist3
		JNE	@@DialHist4
		MOV	DI,SI
		DEC	AH
  @@DialHist4:	POP	CX
		POP	SI
		PUSH	SI
		PUSH	SS
		POP	ES
		XCHG	SI,DI
		MOV	ES:[DI-6],AH
		SUB	DI,5
		STOSB
		STOSB
		STOSB
		STOSB
		STOSB
		OR	AH,AH
		JE	@@DialHist6
		AND	CL,7Fh
		OR	CH,CH
		JE	@@DialHist5
		CMP	CH,CL
		MOV	CL,0
		JBE	@@DialHist5
		XCHG	CL,CH
  @@DialHist5:	REP	MOVSB
  @@DialHist6:	STOSB
		POP	SI
		POP	DI
		POP	ES
		PUSH	BP
		PUSH	DS
		PUSH	SS
		POP	DS
		MOV	BP,[BP]
		MOV	AL,CS:[BP].bKeyBar
		MOV	BP,CS:[BP].pDiBox
		JMP	@@DialInpStr
  @@DialSel:	CMP	CL,2			; Select
		JNE	@@DialFlip
		MOV	AH,CS:[DI].mSelTot
		MOV	CX,DS:[SI]
		CALL	Select
		MOV	DS:[SI],CX
		JMP	@@DialEnd
  @@DialFlip:	CMP	CL,3			; FlipFlop
		JNE	@@DialMenu
		MOV	CL,DS:[SI]
		CALL	FlipFl
		MOV	DS:[SI],CL
		JMP	@@DialEnd
  @@DialMenu:	CMP	CL,4			; MinMnu
		JNE	@@DialEmpty
		DEC	BH
		MOV	CL,DL
		MOV	CH,0
		DEC	CX
		SHR	CX,1
		ADD	BX,CX
		OR	AL,80h
		MOV	CL,DS:[SI]
		PUSH	SI
		PUSH	DX
		MOV	SI,CS:[DI].mPoint
		CALL	MnMn60
		CALL	MinMnu
		POP	DX
		POP	SI
		MOV	DS:[SI],CL
		JMP	@@DialEnd
  @@DialEmpty:	CALL	Input0
  @@DialEnd:	POP	DS			; Exit
		POP	BP
		POP	BX
		POP	DI
  @@GoToCase:	MOV	CL,0
		CMP	CS:[DI].mType,4
		JNE	@@CallSub
		CALL	DialogBuf
		MOV	CL,SS:[SI]
  @@CallSub:	MOV	SI,[BP]
		CALL	DialogSubFunc
		CMP	AH,0FCh
		JE	@@Mouse
  @@Case:	MOV	SI,OFFSET Dial1_T
		CALL	Case
		CMP	CS:[DI].mType,0
		JE	@@DialNext
		MOV	SI,OFFSET Dial2_T
		CALL	Case
		JMP	@@DialNext
DialogHelp:
;		CALL	Help			; Help
		JMP	@@DialNext
DialogTab:	ADD	DI,SIZE BoxMn		; Tab
		CMP	CS:[DI].mType,0
		JNE	@@DialNext
		MOV	DI,[BP]
		ADD	DI,SIZE BoxHd
		JMP	@@DialNext
DialogShTab:	SUB	DI,SIZE BoxMn		; Shift-Tab
		MOV	SI,[BP]
		ADD	SI,SIZE BoxHd
		CMP	DI,SI
		JAE	@@DialNext
  @@LastItem:	ADD	DI,SIZE BoxMn
		CMP	CS:[DI].mType,0
		JNE	@@LastItem
		SUB	DI,SIZE BoxMn
		JMP	@@DialNext

  @@Mouse:	XOR	SI,SI			; Mouse
  @@GetMouse:	CALL	GetMous
		JZ	@@ExitMouse
		OR	SI,SI
		JNE	@@SkipSubMo
		MOV	AH,0FCh
		PUSH	SI
		MOV	SI,[BP]
		CALL	DialogSubFunc
		POP	SI
		CMP	AH,0FCh
		JNE	@@Case
  @@SkipSubMo:	PUSH	SI
		CMP	AL,1
		JNE	@@MouseCur
		PUSH	DX
		SUB	DX,102h
		CALL	InWind
		POP	DX
		MOV	AH,0FFh
		JC	@@CmpMouse
		MOV	SI,[BP]
		ADD	SI,SIZE BoxHd
  @@ChkItems:	MOV	AL,CS:[SI].mType
		CBW
		CMP	AL,0
		JE	@@CmpMouse
		MOV	AH,DL
		PUSH	DX
		PUSH	BX
		CALL	DialogBase
		MOV	DX,CS:[SI].mCtrl
		CMP	AL,1
		JE	@@InInpStr
		CMP	AL,3
		JA	@@InMenu
		DEC	BX
		MOV	DH,DL
		CALL	LenItem
		CMP	AL,2
		JE	@@InItem
		DEC	DX
  @@InInpStr:	INC	DX
		AND	DL,7Fh
		MOV	DH,1
		JMP	@@InItem
  @@MouseCur:	MOV	AH,1
		CMP	AL,2
		JE	@@CmpMouse
		MOV	AH,2
  @@CmpMouse:	POP	SI
		MOV	AL,AH
		MOV	AH,0
		CMP	AX,SI
		JE	@@LeaveTime
		MOV	SI,AX
		CMP	AL,0FFh
		CMC
		ADC	AL,0
		CMP	AL,1
		JB	@@TypMouse
		MOV	AL,ES:Pointers[4]
		JE	@@TypMouse
		MOV	AL,ES:Pointers[5]
  @@TypMouse:	CALL	TypMous
		JMP	@@GetMouse
  @@LeaveTime:	CALL	LeaveTm
		JMP	@@GetMouse
  @@InMenu:	DEC	BH
		MOV	AL,AH
		DEC	AX
		SHR	AL,1
		CBW
		ADD	BX,AX
		PUSH	SI
		MOV	SI,CS:[SI].mPoint
		CALL	MnMn60
		POP	SI
  @@InItem:	CALL	InWind
		POP	BX
		POP	DX
		JNC	@@GoToDial
		ADD	SI,SIZE BoxMn
		JMP	@@ChkItems
  @@GoToDial:	CMP	CS:[DI].mType,4
		JNE	@@GoNextDial
		PUSH	SI
		PUSH	DX
		PUSH	BX
		MOV	SI,DI
		CALL	DialogBase
		DEC	BH
		MOV	AL,DL
		DEC	AX
		SHR	AL,1
		CBW
		ADD	BX,AX
		CALL	DialogBuf
		MOV	CL,SS:[SI]
		MOV	SI,CS:[DI].mPoint
		MOV	DI,SI
		CALL	MnMn60
		MOV	SI,[BP]
		MOV	SI,CS:[SI].pDiBox
		MOV	AH,CS:[SI+0]
		MOV	AL,CS:[SI+2]
		CALL	MnMn40
		POP	BX
		POP	DX
		POP	SI
  @@GoNextDial:	MOV	DI,SI
		POP	SI
		OR	SI,SI
		JE	@@DialNext
		MOV	AL,0
		CALL	TypMous
		JMP	@@DialNext
  @@ExitMouse:	MOV	AL,0
		CALL	TypMous
		MOV	AX,1C0Dh
		CMP	SI,1
		JE	@@GoToCas1
		MOV	AX,011Bh
		CMP	SI,2
		JE	@@GoToCas1
		CMP	SI,0FFh
		JE	@@GoToCas1
		JMP	@@DialNext
  @@GoToCas1:	JMP	@@GoToCase

DialogUp:	MOV	AL,CS:[DI].mUp		; Up
		JMP	@@SetNext
DialogDown:	MOV	AL,CS:[DI].mDown	; Down
		JMP	@@SetNext
DialogLeft:	MOV	AL,CS:[DI].mLeft	; Left
		JMP	@@SetNext
DialogRight:	MOV	AL,CS:[DI].mRight	; Right
		JMP	@@SetNext
DialogHome:	MOV	AL,80h			; Home, PgUp
  @@SetNext:	PUSH	AX
		AND	AL,3Fh
		MOV	AH,SIZE BoxMn
		MUL	AH
		ADD	AX,[BP]
		ADD	AX,SIZE BoxHd
		MOV	DI,AX
		POP	AX
		CMP	CS:[DI].mType,2
		JNE	@@DialNext
		TEST	AL,80h
		JZ	@@DialNext
		TEST	AL,40h
		MOV	AL,0
		JZ	@@SetVar
		MOV	AL,CS:[DI].mSelTot
		DEC	AX
  @@SetVar:	CALL	DialogBuf
		MOV	SS:[SI+1],AL
		JMP	@@DialNext
DialogEnd:	CALL	DialogLast		; End
		CMP	CS:[DI].mType,4
		JNE	DialogPgDn
		CALL	DialogBuf
		MOV	BYTE PTR SS:[SI],0
DialogPgDn:	CALL	DialogLast		; PgDn
		JMP	@@DialNext

DialogEnter:	XOR	AX,AX			; Enter
		CMP	CS:[DI].mType,4
		JNE	@@SaveVars
		CALL	DialogBuf
		MOV	AL,SS:[SI]
		CMP	AL,0
		JNE	@@Exit
  @@SaveVars:	MOV	DI,[BP]
		ADD	DI,SIZE BoxHd
		MOV	SI,BP
  @@SaveVar:	MOV	AL,CS:[DI].mType
		CMP	AL,1
		JB	@@Exit
		PUSH	DI
		JNE	@@SaveSel
		MOV	CX,CS:[DI].mCtrl
		MOV	AL,CL
		AND	CL,7Fh
		OR	CH,CH
		JE	@@SaveInLen2
		CMP	CL,InpModePoint
		JA	@@SaveInLen1
		MOV	CH,0
		JMP	@@SaveInLen2
  @@SaveInLen1:	CMP	CH,CL
		MOV	CL,0
		JBE	@@SaveInLen2
		XCHG	CL,CH
  @@SaveInLen2:	INC	CX
		SUB	SI,CX
		PUSH	DS
		PUSH	ES
		PUSH	SI
		PUSH	SS
		POP	ES
		TEST	AL,80h
		JZ	@@SkipNorm
		CALL	NormStr
  @@SkipNorm:	PUSH	DS
		POP	ES
		MOV	DI,CS:[DI].mPoint
		CMP	BYTE PTR DS:[DI],0
		JNE	@@SaveHist
		PUSH	DI
		PUSH	SI
		PUSH	CX
		MOV	SI,DI
		INC	SI
		REP	MOVSB
		POP	CX
		POP	SI
		POP	DI
  @@SaveHist:	PUSH	SS
		POP	DS
		INC	CX
		CALL	SaveHistory
		POP	SI
		POP	ES
		POP	DS
		SUB	SI,6
		JMP	@@SaveNext
  @@SaveSel:	CMP	AL,3
		JAE	@@SaveFlip
		DEC	SI
		CMP	AL,AL
  @@SaveFlip:	JNE	@@SaveNext
		DEC	SI
		MOV	DI,CS:[DI].mPoint
		MOV	AL,SS:[SI]
		MOV	DS:[DI],AL
  @@SaveNext:	POP	DI
		ADD	DI,SIZE BoxMn
		JMP	@@SaveVar
DialogEsc:	MOV	AL,0FFh			; Esc
  @@Exit:	MOV	SP,BP
		POP	BP
		CALL	HidCur
		TEST	CS:[BP].bFlags,10h	; Restore window
		JNZ	@@RestCur
		CALL	ResWin
		CALL	Window
  @@RestCur:	POP	CX
		PUSH	BP
		PUSH	AX
		MOV	AH,1
		INT	10h
		POP	AX
		POP	BP
		CMP	AL,0
		POP	BX
		MOV	ES:MinMnuFlag,BL
		RET
Dialog		ENDP

DialogBase	PROC	NEAR			; Set base coordinates
		ADD	BX,CS:[SI].mBase
		TEST	CS:[SI].mBase.Y,80h
		JZ	@@Exit
		ADD	BH,DH
  @@Exit:	RET
DialogBase	ENDP

DialogBuf	PROC	NEAR			; Find buffer
		USES	AX,CX
		MOV	AX,BP
		MOV	SI,[BP]
		ADD	SI,SIZE BoxHd
  @@FndBuf:	DEC	AX
		CMP	CS:[SI].mType,2
		JA	@@CmpBuf
		PUSHF
		DEC	AX
		POPF
		JE	@@CmpBuf
		MOV	CX,CS:[SI].mCtrl
		AND	CL,7Fh
		OR	CH,CH
		JE	@@InLen2
		CMP	CL,InpModePoint
		JA	@@InLen1
		MOV	CH,0
		JMP	@@InLen2
  @@InLen1:	CMP	CH,CL
		MOV	CL,0
		JBE	@@InLen2
		XCHG	CL,CH
  @@InLen2:	SUB	AX,CX
		SUB	AX,5
  @@CmpBuf:	CMP	SI,DI
		JAE	@@DialInit
		ADD	SI,SIZE BoxMn
		JMP	@@FndBuf
  @@DialInit:	MOV	SI,AX
		RET
DialogBuf	ENDP

DialogLast	PROC	NEAR
  @@Next:	ADD	DI,SIZE BoxMn
		CMP	CS:[DI].mType,0
		JNE	@@Next
		SUB	DI,SIZE BoxMn
		RET
DialogLast	ENDP

DialogSubFunc	PROC	NEAR
		USES	BX,CX,DX,SI,DI,BP,DS,ES
		CMP	CS:[SI].SubFunc,0
		JE	@@Exit
		MOV	DI,BP
		MOV	BP,CS:[SI]
		CALL	CS:[SI].SubFunc
  @@Exit:	RET
DialogSubFunc	ENDP

;---------------------------------------------------------------
;	Dialog List
;---------------------------------------------------------------
ListData	STRUC
		DB	TmpBuf+486 DUP (?)
ListCurFlag	DB	?
ListMoveFlag	DB	?
ListKey		DW	?
ListLoop	DB	?
ListShift	DW	?
ListScrollType	DB	?
ListBase	DW	?
ListWindow	DW	?
ListCursor	DW	?
ListFirst	DW	?
ListTotal	DW	?
ListScrollBar	DW	?
ListLine	DW	?
ListHotKey	DW	?
ListSelect	DW	?
ListData	ENDS

		.DATA
DialList_T	DW	0FFFEh, ListInit1
		DW	0FFFFh, ListInit2
		DW	0FFFDh,	ListScrlBar
		DW	0FFFCh, ListScroll
		DW	 4800h, ListUp
		DW	 5000h, ListDown
		DW	 4700h, ListHome
		DW	 4F00h, ListEnd
		DW	 4900h, ListPgUp
		DW	 5100h, ListPgDn
		DW	 5200h, ListIns
		DW	 4E2Bh, ListGreyPlus
		DW	 4A2Dh, ListGreyMinus
		DW	 1C0Dh, ListEnter
		DW	 0

		.CODE	Subs
DialogList	PROC	NEAR
		MOV	DI,DS:ListFirst
		CMP	AH,0FCh
		JE	@@Mouse
		MOV	DS:MouseCmd,0
		MOV	DS:MouseTime.Hi,0FFFFh
  @@Case:	MOV	SI,OFFSET DialList_T
		CALL	Case
		CMP	DS:ListHotKey,0		; Hot keys
		JE	@@CtrlEnter
		PUSH	DI
		CALL	DS:ListHotKey
		POP	DI
		JNC	@@SetMove
  @@CtrlEnter:	CMP	AX,1C0Ah
		JE	ListEnter
		JMP	@@Exit
  @@Mouse:	MOV	DI,DS:ListFirst		; Mouse support
		CALL	GetMous
		JZ	@@Clear
		MOV	AH,0FCh
		TEST	AL,4
		JNZ	@@Exit
		PUSH	DX
		PUSH	BX
		ADD	BX,DS:ListBase
		MOV	DX,DS:ListWindow
		TEST	DS:MouseWin,1
		JNZ	@@MousePos
		TEST	DS:MouseWin,2
		JNZ	@@ScrollUp
		TEST	DS:MouseWin,4
		JNZ	@@ScrollDn
		TEST	DS:MouseWin,8
		JNZ	@@SetBar
		CALL	InWind
		JNC	@@MousePos
		CMP	DS:ListScrollBar,0
		JE	@@MouseExit
		ADD	BL,DL
		INC	BX
		MOV	DX,101h
		CALL	InWind
		JNC	@@ScrollUp
		PUSH	BX
		ADD	BH,DS:ListWindow.Y
		DEC	BH
		CALL	InWind
		POP	BX
		JNC	@@ScrollDn
		MOV	DS:ListScrollType,0FFh
		MOV	DH,DS:ListWindow.Y
		PUSH	DX
		PUSH	BX
		SUB	DH,2
		INC	BH
		CALL	InWind
		POP	BX
		POP	DX
		JNC	@@SetBar
  @@MouseExit:	POP	BX
		POP	DX
		JMP	@@Exit
  @@ScrollUp:	MOV	DS:MouseWin,2		; Scroll arrows
		MOV	AL,0
		MOV	SI,4800h
		JMP	@@ScrollUpDn
  @@ScrollDn:	MOV	DS:MouseWin,4
		MOV	AL,DS:ListWindow.Y
		DEC	AX
		MOV	SI,5000h
  @@ScrollUpDn:	POP	BX
		POP	DX
		PUSH	DX
		PUSH	BX
		ADD	BX,DS:ListBase
		ADD	BL,DS:ListWindow.X
		ADD	BH,AL
		INC	BX
		MOV	DX,101h
		CALL	InWind
		POP	BX
		POP	DX
		JNC	@@ScrRepeat
		MOV	DS:MouseCmd,0
  @@LeaveTime:	CALL	LeaveTm
		JMP	@@Mouse
  @@ScrRepeat:	CALL	MouseRepeat
		JNC	@@Mouse
		MOV	AX,SI
		JMP	@@Case
  @@SetBar:	MOV	DS:ListMoveFlag,1	; Scroll bar
		MOV	DS:MouseWin,8
		MOV	AX,DS:ListScrollBar
		MOV	CL,DS:ListScrollType
		MOV	DL,DH
		SUB	DH,2
		INC	BH
		SUB	CH,BH
		JAE	@@SetBar1
		XOR	CX,CX
		JMP	@@SetBar5
  @@SetBar1:	CMP	CH,DH
		JB	@@SetBar2
		MOV	CH,DH
		SUB	CH,AL
		MOV	CL,AL
		DEC	CX
		JMP	@@SetBar5
  @@SetBar2:	CMP	CL,0FFh
		JNE	@@SetBar3
		MOV	CL,0
		CMP	CH,AH
		JB	@@SetBar3
		MOV	CL,AL
		DEC	CX
		MOV	BL,AH
		ADD	BL,AL
		CMP	CH,BL
		JAE	@@SetBar3
		MOV	CL,CH
		SUB	CL,AH
  @@SetBar3:	SUB	CH,CL
		JAE	@@SetBar4
		ADD	CL,CH
		MOV	CH,0
		JMP	@@SetBar5
  @@SetBar4:	MOV	BL,CH
		ADD	BL,AL
		SUB	BL,DH
		JBE	@@SetBar5
		ADD	CL,BL
		SUB	CH,BL
  @@SetBar5:	MOV	DS:ListScrollType,CL
		MOV	CL,DH
		SUB	CL,AL
		MOV	AX,DS:ListTotal
		MOV	DH,0
		SUB	AX,DX
		MOV	DL,CH
		MUL	DX
		MOV	CH,0
		DIV	CX
		OR	DX,DX
		JE	@@SetBar6
		INC	AX
  @@SetBar6:	MOV	DI,AX
		POP	BX
		POP	DX
		CMP	DI,DS:ListFirst
		JE	@@LeaveTime
		MOV	SI,DS:ListCursor
		CMP	SI,DI
		JAE	@@SetBar7
		MOV	SI,DI
  @@SetBar7:	MOV	AL,DS:ListWindow.Y
		MOV	AH,0
		ADD	AX,DI
		CMP	SI,AX
		JB	@@CorParam
		MOV	SI,AX
		DEC	SI
		JMP	@@CorParam
  @@MousePos:	MOV	DS:ListMoveFlag,1	; Set cursor
		AND	DS:MouseWin,0F0h
		OR	DS:MouseWin,11h
		TEST	AL,2
		JNZ	@@ChkZone
		AND	DS:MouseWin,0Fh
  @@ChkZone:	SUB	CH,BH
		JB	@@MouseUp
		CMP	CH,DH
		JAE	@@MouseDown
		CALL	ResMouseCmd
		SUB	CL,BL
		POP	BX
		POP	DX
		JB	@@LeaveTime
		CMP	CL,DS:ListWindow.X
		JAE	@@LeaveTime
		MOV	CL,CH
		MOV	CH,0
		MOV	SI,DI
		ADD	SI,CX
		CMP	SI,DS:ListTotal
		JAE	@@MouseLast
		CMP	DS:ListSelect,0
		JE	@@DblClick
		TEST	DS:MouseWin,10h
		JNZ	@@MouseSet
  @@DblClick:	PUSH	DX			; Double click
		PUSH	BX
		PUSH	AX
		ADD	BX,DS:ListBase
		ADD	BH,CL
		MOV	DL,DS:ListWindow.X
		MOV	DH,1
		CALL	MouseDblClick
		POP	AX
		POP	BX
		POP	DX
		JC	@@MouseSet
		JE	@@Clear
		TEST	AL,2
		MOV	AX,1C0Ah
		JNZ	ListEnter
		MOV	AX,1C0Dh
		JMP	ListEnter
  @@MouseLast:	MOV	SI,DS:ListTotal
		DEC	SI
  @@MouseSet:	CMP	SI,DS:ListCursor
		JNE	@@CorParam
		MOV	AH,1
		JMP	@@MouseSel
  @@MouseUp:	MOV	SI,4800h		; Scroll
		MOV	AL,DS:Pointers[0]
		JMP	@@MouseUpDn
  @@MouseDown:	MOV	SI,5000h
		MOV	AL,DS:Pointers[1]
  @@MouseUpDn:	POP	BX
		POP	DX
		CMP	DS:MouseCmd,0
		JNE	@@MouseRep
		CALL	TypMous
  @@MouseRep:	CALL	MouseRepeat
		JNC	@@Mouse
		CMP	SI,4800h
		MOV	SI,DI
		JNE	@@MouseDn
		OR	SI,SI
		JE	@@MouseCor
		JMP	@@Up1
  @@MouseDn:	MOV	AL,DS:ListWindow.Y
		MOV	AH,0
		ADD	SI,AX
		CMP	SI,DS:ListTotal
		JAE	ListEnd
  @@MouseCor:	JMP	@@CorParam
ListInit1:	MOV	DS:ListShift,0		; Init
		MOV	DS:ListLoop,0
		MOV	DS:ListScrollBar,0
		MOV	DS:ListMoveFlag,0
		MOV	DS:ListCurFlag,0
		MOV	DS:ListSelect,0
		JMP	@@Clear
ListInit2:	MOV	DI,8000h
		XCHG	DI,DS:ListFirst
		MOV	SI,DS:ListCursor
		JMP	@@CorParam
ListScroll:	MOV	SI,DS:ListTotal		; Scroll
		DEC	SI
		JMP	@@CorParam
ListUp:		MOV	SI,DS:ListCursor	; Up
		OR	SI,SI
		JNE	@@Up1
		CMP	DS:ListLoop,0
		JNE	@@SetMove
		CMP	DS:ListScrollBar,0
		JE	ListEnd
		JMP	@@SetMove
  @@Up1:	DEC	SI
		JMP	@@SetMove
ListDown:	MOV	SI,DS:ListCursor	; Down
		INC	SI
		CMP	SI,DS:ListTotal
		JB	@@SetMove
  @@Down1:	CMP	DS:ListLoop,0
		JNE	@@Up1
		CMP	DS:ListScrollBar,0
		JNE	@@Up1
ListHome:	XOR	SI,SI			; Home
		JMP	@@SetMove
ListEnd:	MOV	SI,DS:ListTotal		; End
		JMP	@@Up1
ListPgUp:	MOV	SI,DS:ListCursor	; PgUp
		MOV	AL,DS:ListWindow.Y
		MOV	AH,0
		CALL	PageUp
		JMP	@@SetMove
ListPgDn:	PUSH	BP			; PgDn
		MOV	BP,DS:ListTotal
		MOV	SI,DS:ListCursor
		MOV	AL,DS:ListWindow.Y
		MOV	AH,0
		CALL	PageDown
		POP	BP
		JMP	@@SetMove
ListIns:	CMP	DS:ListSelect,0		; Ins
		JE	@@Exit
		MOV	SI,DS:ListCursor
		CMP	SI,DS:ListTotal
		JAE	@@Exit
		PUSH	DX
		PUSH	BX
		MOV	AL,0FFh
		CALL	DS:ListSelect
		JC	@@ChkDown
		XOR	AL,1
		CALL	DS:ListSelect
		JC	@@ChkDown
		PUSH	BP
		INC	BP
		CALL	DS:ListLine
		POP	BP
		JC	@@ChkDown
		CALL	Window
  @@ChkDown:	POP	BX
		POP	DX
		CMP	DS:InsDwn,0
		JE	@@Exit
		INC	SI
		CMP	SI,DS:ListTotal
		JAE	@@Down1
		PUSH	DS
		XOR	AX,AX
		MOV	DS,AX
		AND	BYTE PTR DS:[418h],7Fh
		POP	DS
		JMP	@@SetMove
ListGreyPlus:	MOV	AL,1			; Grey +
		JMP	@@SelAll
ListGreyMinus:	MOV	AL,0			; Grey -
  @@SelAll:	CMP	DS:ListSelect,0
		JE	@@Exit
		XOR	SI,SI
		MOV	AH,0
  @@ChkLine:	CMP	SI,DS:ListTotal
		JAE	@@SelDisplay
		CALL	DS:ListSelect
		JC	@@Next
		MOV	AH,1
  @@Next:	INC	SI
		JMP	@@ChkLine
  @@SelDisplay:	OR	AH,AH
		JE	@@Exit
		MOV	SI,DS:ListCursor
		MOV	DI,8000h
		XCHG	DI,DS:ListFirst
		JMP	@@CorParam
  @@SetMove:	MOV	DS:ListMoveFlag,1
  @@CorParam:	MOV	CX,DI			; Test parameters
		CMP	SI,DI
		JAE	@@CorPar1
		MOV	DI,SI
  @@CorPar1:	MOV	AL,DS:ListWindow.Y
		MOV	AH,0
		ADD	AX,DI
		CMP	SI,AX
		JB	@@Output
		SUB	AX,DI
		MOV	DI,SI
		SUB	DI,AX
		INC	DI
  @@Output:	MOV	AX,DI
		XCHG	SI,DS:ListCursor
		XCHG	DI,DS:ListFirst
		SUB	AX,DI
		JNE	@@ResCur
		CMP	SI,DS:ListCursor
		JNE	@@ResCur
		CMP	DS:ListCurFlag,0
		JE	@@ChkRet
		CMP	DS:ListMoveFlag,0
		JE	@@ChkRet
  @@ResCur:	PUSH	AX			; Reset cursor
		PUSH	DX
		PUSH	BX
		CMP	DI,8000h
		JE	@@NoOut1
		CMP	SI,DS:ListTotal
		JAE	@@NoOut1
		CMP	DS:ListCurFlag,0
		JNE	@@NoOut1
		CALL	DS:ListLine
		JC	@@NoOut1
		CALL	Window
  @@NoOut1:	POP	BX
		POP	DX
		POP	CX
		CMP	DS:ListShift,8000h
		JE	@@OutLines
		JCXZ	@@ChkFlag
  @@OutLines:	PUSH	CX			; Output lines
		MOV	SI,DS:ListFirst
		MOV	DI,SI
		MOV	CL,DS:ListWindow.Y
		MOV	CH,0
  @@WriteLine:	CMP	SI,DS:ListTotal
		JAE	@@ExitLines
		PUSH	DX
		PUSH	BX
		CALL	DS:ListLine
		POP	BX
		POP	DX
		INC	SI
		LOOP	@@WriteLine
  @@ExitLines:	POP	CX
		PUSH	DX			; Output screen
		PUSH	CX
		PUSH	BX
		ADD	BX,DS:ListBase
		MOV	DX,DS:ListWindow
		CMP	DS:ListShift,8000h
		JE	@@WriteWin
		MOV	AL,DH
		DEC	AX
		MOV	AH,6
		CMP	CX,1
		JE	@@Scroll
		CMP	CX,-1
		JNE	@@WriteWin
		MOV	AX,700h
  @@Scroll:	PUSH	DX
		PUSH	BX
		MOV	CX,BX
		ADD	DX,CX
		SUB	DX,101h
		PUSH	AX
		MOV	AH,CS:[BP]
		CALL	Color
		MOV	BH,AH
		POP	AX
		PUSH	AX
		MOV	AL,1
		CALL	Int10m
		POP	AX
		POP	BX
		POP	DX
		ADD	BH,AL
		MOV	DH,1
  @@WriteWin:	CALL	Window
		POP	BX
		POP	CX
		POP	DX
  @@ChkFlag:	CMP	DS:ListMoveFlag,0
		JE	@@SetCur
		MOV	DS:ListCurFlag,0
  @@SetCur:	PUSH	DX			; Set cursor
		PUSH	BX
		MOV	SI,DS:ListCursor
		MOV	DI,DS:ListFirst
		CMP	DS:ListCurFlag,0
		JNE	@@NoOut2
		CMP	SI,DS:ListTotal
		JAE	@@NoOut2
		INC	BP
		CALL	DS:ListLine
		DEC	BP
		JC	@@NoOut2
		CALL	Window
  @@NoOut2:	POP	BX
		POP	DX
		JCXZ	@@ChkRet
ListScrlBar:	MOV	AL,DS:ListWindow.Y	; Scroll bar
		MOV	AH,0
		CMP	AX,DS:ListTotal
		JAE	@@ChkRet
		PUSH	DX
		PUSH	BX
		MOV	DX,101h
		ADD	BX,DS:ListBase
		ADD	BL,DS:ListWindow.X
		INC	BX
		CMP	DS:ListScrollBar,0
		JNE	@@ScrBar1
		PUSH	ES
		MOV	AH,CS:[BP+1]
		CALL	Color
		CALL	BasAdr
		MOV	AL,DS:ScrollBars[0]
		STOSW
		CALL	Window
		PUSH	BX
		ADD	BH,DS:ListWindow.Y
		DEC	BH
		CALL	BasAdr
		MOV	AL,DS:ScrollBars[1]
		STOSW
		CALL	Window
		POP	BX
		POP	ES
  @@ScrBar1:	MOV	AL,DS:ListWindow.Y
		MOV	AH,0
		MOV	SI,AX
		DEC	SI
		DEC	SI
		MUL	SI
		MOV	CX,DS:ListTotal
		DIV	CX
		OR	AX,AX
		JNE	@@ScrBar2
		INC	AX
  @@ScrBar2:	MOV	DI,AX
		MOV	AX,SI
		SUB	AX,DI
		MUL	DS:ListFirst
		SUB	CX,SI
		DEC	CX
		DEC	CX
		DIV	CX
		SUB	SI,AX
		SUB	SI,DI
		MOV	CX,DI
		MOV	CH,AL
		MOV	DS:ListScrollBar,CX
		INC	BH
		MOV	CX,AX
		MOV	AL,DS:ScrollBars[2]
		PUSH	AX
		CALL	ListScrBar
		MOV	CX,DI
		MOV	AL,DS:ScrollBars[3]
		CALL	ListScrBar
		POP	AX
		MOV	CX,SI
		CALL	ListScrBar
		POP	BX
		POP	DX
  @@ChkRet:	CMP	DS:MouseWin,0
		JZ	@@Clear
		MOV	AH,0
  @@MouseSel:	CMP	DS:ListSelect,0		; Select by mouse
		JE	@@NoSelect
		TEST	DS:MouseWin,10h
		JZ	@@NoSelect
		MOV	SI,DS:ListCursor
		CMP	SI,DS:ListTotal
		JAE	@@NoSelect
		MOV	DI,DS:ListFirst
		TEST	DS:MouseWin,20h
		JNZ	@@SelUnsel
		MOV	AL,0FFh
		CALL	DS:ListSelect
		JC	@@NoSelect
		OR	DS:MouseWin,20h
		CMP	AL,0
		JNE	@@SelUnsel
		OR	DS:MouseWin,40h
  @@SelUnsel:	MOV	AL,0
		TEST	DS:MouseWin,40h
		JZ	@@CallSel
		INC	AX
  @@CallSel:	CALL	DS:ListSelect
		JC	@@NoSelect
		PUSH	DX
		PUSH	BX
		PUSH	BP
		INC	BP
		CALL	DS:ListLine
		POP	BP
		JC	@@ExitSel
		CALL	Window
  @@ExitSel:	POP	BX
		POP	DX
		JMP	@@Mouse
  @@NoSelect:	OR	AH,AH
		JE	@@Mouse
		JMP	@@LeaveTime
  @@Clear:	XOR	AX,AX
		JMP	@@Exit
ListEnter:	MOV	DS:ListKey,AX
  @@Exit:	MOV	DS:MouseWin,0
		MOV	DS:MouseEnter,0
		CALL	ResMouseCmd
		RET
DialogList	ENDP

ListScrBar	PROC	NEAR
		USES	DI,ES
		JCXZ	@@Exit
		MOV	DX,101h
  @@Write:	CALL	BasAdr
		CMP	AL,ES:[DI]
		JE	@@Next
		MOV	ES:[DI],AL
		CALL	Window
  @@Next:	INC	BH
		LOOP	@@Write
  @@Exit:	RET
ListScrBar	ENDP

;---------------------------------------------------------------
;	Input string
;		BH,BL - begin
;		DS:SI - string
;		   DI - cursor
;		   CL - visible length
;		   CH - string length
;		   AH - number of color
;		   AL - key bar
;	Return:	   DI - cursor
;		   AX - key code
;---------------------------------------------------------------
		.DATA
InSt_T		DW	0009h, InpStrExit
		DW	0F00h, InpStrExit
		DW	4B00h, InpStrLeft
		DW	4D00h, InpStrRight
		DW	4800h, InpStrExit
		DW	5000h, InpStrExit
		DW	4900h, InpStrExit
		DW	5100h, InpStrExit
		DW	0011h, InpStrEdit
		DW	000Dh, InpStrExit
		DW	011Bh, InpStrExit
		DW	4200h, InpStrExit
		DW	4400h, InpStrExit
		DW	7100h, InpStrExit
		DW	0005h, InpStrExit
		DW	0018h, InpStrExit
		DW	000Ah, InpStrFile
		DW	001Bh, InpStrPath1
		DW	001Dh, InpStrPath2
		DW	0

		.CODE	Subs
InpStr		PROC	NEAR
		USES	DX,SI,BP
		MOV	DX,AX
  @@ResetMouse:	MOV	ES:MouseCmd,0
  @@PutStr:	MOV	AH,DH
		MOV	BP,DS:[SI-3]
		CALL	InpStrOut
		PUSH	DX
		MOV	DL,CL
		MOV	DH,1
		CALL	Window
		POP	DX
		LEA	AX,[BX+DI]
		SUB	AX,DS:[SI-3]
		CALL	Cursor
		MOV	AL,DL
		CALL	Input
		CMP	AH,0FCh
		JE	@@Mouse
		MOV	BP,SI
		CMP	ES:Quote,0
		JNZ	@@ChkClr
		MOV	SI,OFFSET InSt_T
		CALL	Case
  @@ChkClr:	CMP	BYTE PTR DS:[BP-1],0
		JNE	InpStrEdit
		MOV	BYTE PTR DS:[BP-1],1
		CMP	ES:Quote,0
		JNZ	@@ClrStr
		OR	AH,AH
		JE	@@ClrStr
		CMP	AL,' '
		JB	InpStrEdit
		CMP	AL,7Fh
		JE	InpStrEdit
  @@ClrStr:	MOV	BYTE PTR DS:[BP],0
InpStrEdit:	PUSH	CX
		OR	CH,CH
		JE	@@InLen2
		CMP	CL,InpModePoint
		JA	@@InLen1
		MOV	CH,80h
		JMP	@@InLen2
  @@InLen1:	CMP	CH,CL
		MOV	CL,0
		JBE	@@InLen2
		XCHG	CL,CH
  @@InLen2:	MOV	SI,BP
		XCHG	DI,DX
		CALL	EdLin
		XCHG	DI,DX
		MOV	AX,CX
		AND	AX,7FFFh
		POP	CX
  @@CorCur0:	OR	CH,CH
		JE	@@PutStr
		CMP	CL,InpModePoint
		JBE	@@PutStr
		CMP	DI,DS:[SI-3]
		JA	@@CorCur1
		MOV	DS:[SI-3],DI
		OR	DI,DI
		JE	@@CorCur1
		DEC	WORD PTR DS:[SI-3]
  @@CorCur1:	MOV	AL,CL
		MOV	AH,0
		DEC	AX
		ADD	AX,DS:[SI-3]
		CMP	DI,AX
		JB	@@PutStr
		MOV	AX,DI
		SUB	AL,CL
		SBB	AH,0
		INC	AX
		MOV	DS:[SI-3],AX
		JMP	@@PutStr
InpStrFile:	PUSH	DS			; Insert file
		MOV	DS,ES:WCBcrn
		CMP	DS:Visible,0
		JZ	@@File2
		CMP	DS:WinTyp,p_File
		JNE	@@File2
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	@@File2
		CMP	DS:[SI].LenLongNamF,0
		JE	@@File1
		CMP	ES:OS2box,0
		JNE	@@File1
		CALL	KeybStat
		TEST	AL,3
		JNZ	@@File1
		ADD	SI,DS:[SI].LenShortNamF
  @@File1:	LEA	AX,[SI].NameF
		JMP	@@Path2
  @@File2:	POP	DS
		MOV	SI,BP
		JMP	@@PutStr
InpStrPath1:	MOV	AX,ES:WCB1seg		; Insert path
		JMP	@@Path1
InpStrPath2:	MOV	AX,ES:WCB2seg
  @@Path1:	PUSH	DS
		MOV	DS,AX
		CMP	DS:WinTyp,p_Tree
		JA	@@File2         	; Info or QuickView panel
		MOV	AX,OFFSET CrnPath
		JE	@@Path2
		MOV	AX,OFFSET ArcPath	; File panel
		CMP	DS:ArcPnt,0
		JNE	@@Path2
		CALL	KeybStat
		TEST	AL,3
		MOV	AX,OFFSET WinLongPath
		JZ	@@Path2
		MOV	AX,OFFSET WinShortPath
  @@Path2:	CMP	CL,InpModePoint
		JBE	@@File2
		MOV	SI,DS
		POP	DS
		PUSH	CX
  		PUSH	BX
		MOV	BX,SI
		MOV	SI,BP
		MOV	BP,SP
		SUB	SP,512
		PUSH	ES
		PUSH	DS
		PUSH	DI
		PUSH	SI
		PUSH	SS
		POP	ES
		LEA	DI,[BP-512]
		MOV	DS,BX
		MOV	BX,DI
		MOV	SI,AX
		PUSH	DI
		PUSH	SI
		CALL	strcpy
		POP	SI
		POP	DI
		CMP	SI,OFFSET ArcFiles
		JAE	@@Path4
		CMP	BYTE PTR DS:[SI],0
		JE	@@Path4
		CMP	SI,OFFSET CrnPath
		JNE	@@Path3
		CALL	KeybStat
		TEST	AL,3
		JNZ	@@Path3
		CALL	LongTreePath
  @@Path3:	PUSH	CX
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		CALL	AddPathSep
		MOV	AL,0
		STOSB
		POP	CX
  @@Path4:	POP	SI
		POP	DI
		POP	DS
		POP	ES
		XCHG	DI,DX
		CMP	BYTE PTR DS:[SI-1],0
		JNE	@@Path5
		MOV	WORD PTR DS:[SI-1],1
  @@Path5:	OR	CH,CH
		JE	@@Path6
		CMP	CH,CL
		MOV	CL,0
		JBE	@@Path6
		XCHG	CL,CH
  @@Path6:	MOV	AL,SS:[BX]
		CMP	AL,0
		JE	@@Path7
		INC	BX
		MOV	AH,0
		CALL	EdLin
		JC	@@Path6
  @@Path7:	XCHG	DI,DX
		MOV	AX,CX
		MOV	SP,BP
		POP	BX
		POP	CX
		JMP	@@CorCur0
  @@Mouse:	XCHG	BP,CX			; Mouse
		CALL	GetMous
		MOV	AH,0FCh
		XCHG	BP,CX
		JZ	@@ResetMouse
		CMP	AL,1
		JNE	InpStrExit
		PUSH	DX
		MOV	DL,CL
		MOV	DH,1
		INC	DX
		XCHG	BP,CX
		CALL	InWind
		XCHG	BP,CX
		POP	DX
		JC	InpStrExit
		MOV	BYTE PTR DS:[SI-1],1
		SUB	BP,BX
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	CX
		PUSH	DS
		POP	ES
		MOV	CH,0
		INC	CX
		ADD	SI,DS:[SI-3]
		MOV	DI,SI
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		SUB	DI,SI
		MOV	AX,DI
		POP	CX
		POP	SI
		POP	DI
		POP	ES
		OR	CH,CH
		JE	@@CmpCur
		CMP	CL,InpModePoint
		JBE	@@CmpCur
		OR	BP,BP
		JNE	@@Right0
		MOV	DI,DS:[SI-3]
		OR	DI,DI
		JE	@@Left
		INC	DI
  @@Left:	MOV	AX,4B00h
		JMP	@@Repeat
  @@Right0:	PUSH	CX
		MOV	CH,0
		DEC	CX
		CMP	BP,CX
		POP	CX
		JB	@@CmpCur
		CMP	AX,BP
		JBE	@@Right1
		MOV	AX,BP
  @@Right1:	PUSH	CX
		MOV	CH,0
		DEC	CX
		CMP	AX,CX
		JBE	@@Right2
		MOV	AX,CX
  @@Right2:	POP	CX
		ADD	AX,DS:[SI-3]
		MOV	DI,AX
		MOV	AX,4D00h
  @@Repeat:	PUSH	SI
		MOV	SI,AX
		CALL	MouseRepeat
		POP	SI
		JNC	@@Mouse
		MOV	BP,SI
		JMP	InpStrEdit
  @@CmpCur:	MOV	ES:MouseCmd,0
		CMP	BP,AX
		JBE	@@SetPos
		MOV	BP,AX
  @@SetPos:	ADD	BP,DS:[SI-3]
		CMP	BP,DI
		JE	@@LeaveTime
		MOV	DI,BP
		JMP	@@PutStr
  @@LeaveTime:	CALL	LeaveTm
		JMP	@@Mouse
InpStrLeft:	OR	DI,DI
		JNE	@@ChkClr
		JMP	InpStrExit
InpStrRight:	CMP	BYTE PTR DS:[BP+DI],0
		JNE	@@ChkClr
InpStrExit:	RET
InpStr		ENDP

InpStrOut	PROC	FAR
		USES	CX,DX,SI,DI,ES
		MOV	DX,WORD PTR ES:Scrolls
		MOV	CH,0
		CALL	BasAdr
		CALL	Color
		OR	BP,BP
		JE	@@GetChar
		ADD	SI,BP
		INC	SI
		MOV	AL,DL
		JMP	@@PutChar
  @@GetChar:	LODSB
		CMP	AL,0
		JE	@@FillSpc
  @@PutChar:	STOSW
		LOOP	@@GetChar
		LODSB
		CMP	AL,0
		JE	@@Exit
		DEC	DI
		DEC	DI
		MOV	AL,DH
		STOSW
		JMP	@@Exit
  @@FillSpc:	MOV	AL,' '
		REP	STOSW
  @@Exit:	RET
InpStrOut	ENDP

;---------------------------------------------------------------
;	Select case
;		BH,BL - begin
;		   AL - key bar
;		   AH - total
;		   CL - current
;		   CH - cursor
;	Return:	   CL - current
;		   CH - cursor
;		   AX - key code
;---------------------------------------------------------------
		.DATA
Slct_T		DW	0020h, SelectSpace
		DW	0009h, SelectExit
		DW	0F00h, SelectExit
		DW	4B00h, SelectExit
		DW	4D00h, SelectExit
		DW	4800h, SelectUp
		DW	5000h, SelectDown
		DW	4700h, SelectPgUp
		DW	4F00h, SelectPgDn
		DW	4900h, SelectPgUp
		DW	5100h, SelectPgDn
		DW	000Dh, SelectEnter
		DW	001Bh, SelectExit
		DW	4400h, SelectExit
		DW	0

		.CODE	Subs
Select		PROC	NEAR
		USES	DX,SI,DI
		MOV	DX,AX
		MOV	AL,0
  @@SetCur:	PUSH	AX			; Cursor
		MOV	AX,BX
		ADD	AH,CH
		CALL	Cursor
		POP	AX
		CMP	AL,0
		JZ	@@Input
		PUSH	CX
  @@WaitMouse:	CALL	LeaveTm
		CALL	GetMous
		JNZ	@@WaitMouse
		POP	CX
  @@Input:	MOV	AL,DL			; Input
		CALL	Input
		CMP	AH,0FCh
		JE	@@Mouse
		MOV	SI,OFFSET Slct_T
		CALL	Case
		JMP	@@Input
  @@Mouse:	XCHG	DI,CX			; Mouse
		CALL	GetMous
		MOV	AH,0FCh
		XCHG	DI,CX
		JZ	@@Input
		CMP	AL,1
		JNE	SelectExit
		PUSH	DX
		DEC	BX
		CALL	LenItem
		XCHG	DI,CX
		CALL	InWind
		XCHG	DI,CX
		INC	BX
		POP	DX
		JC	SelectExit
		MOV	AX,DI
		SUB	AH,BH
		CMP	AH,CL
		JE	@@LeaveTime
		MOV	CH,AH
		JMP	@@ChgPos
  @@LeaveTime:	CALL	LeaveTm
		JMP	@@Mouse
SelectEnter:	CMP	CH,CL			; Enter
		JNE	@@ChgPos
		JMP	SelectExit
SelectSpace:	CMP	CH,CL			; Space
		JNE	@@ChgPos
		INC	CH
		CMP	CH,DH
		JB	@@ChgPos
		MOV	CH,0
  @@ChgPos:	PUSH	ES
		PUSH	DX
		PUSH	BX
		MOV	AH,CH
		MOV	DX,101h
		ADD	BH,CL
		CALL	BasAdr
		MOV	BYTE PTR ES:[DI],' '
		CALL	Window
		SUB	BH,CL
		ADD	BH,AH
		CALL	BasAdr
		MOV	BYTE PTR ES:[DI],''
		CALL	Window
		POP	BX
		POP	DX
		POP	ES
		MOV	CL,CH
		JMP	@@SetCur
SelectUp:	OR	CH,CH			; Up
		JE	SelectExit
		DEC	CH
		JMP	@@SetCur
SelectDown:	INC	CH			; Down
		CMP	CH,DH
		JB	@@SetCur
SelectPgDn:	MOV	CH,DH			; End, PgDn
		DEC	CH
		JMP	SelectExit
SelectPgUp:	MOV	CH,0			; Home, PgUp
SelectExit:	RET
Select		ENDP

LenItem		PROC	NEAR
		USES	DI,ES
		MOV	DL,0
		CALL	BasAdr
  @@Check:	CMP	BYTE PTR ES:[DI],' '
		JNE	@@Next
		CMP	BYTE PTR ES:[DI+2],' '
		JE	@@Exit
  @@Next:	INC	DI
		INC	DI
		INC	DL
		JNE	@@Check
  @@Exit:	RET
LenItem		ENDP

;---------------------------------------------------------------
;	Flip Flop
;		BH,BL - begin
;		   AL - key bar
;		   CL - paremeter
;	Return:	   CL - paremeter
;		   AX - key code
;---------------------------------------------------------------
FlipFl		PROC	NEAR
		USES	DX,DI
		MOV	CH,AL
		MOV	AX,BX
		CALL	Cursor
  @@Input:	MOV	AL,CH
		CALL	Input
		CMP	AL,' '
		JE	@@Flip
		CMP	AH,0FCh
		JNE	@@Exit
		XCHG	DI,CX			; Mouse
		CALL	GetMous
		MOV	AH,0FCh
		XCHG	DI,CX
		JZ	@@Input
		CMP	AL,1
		JNE	@@Exit
		DEC	BX
		CALL	LenItem
		MOV	DH,1
		XCHG	DI,CX
		CALL	InWind
		XCHG	DI,CX
		INC	BX
		JC	@@Exit
  @@Flip:	PUSH	ES
		CALL	BasAdr
		MOV	BYTE PTR ES:[DI],' '
		OR	CL,CL
		MOV	CL,0
		JNE	@@PutFlip
		MOV	BYTE PTR ES:[DI],'x'
		MOV	CL,1
  @@PutFlip:	POP	ES
		MOV	DX,101h
		CALL	Window
		CMP	AL,' '
		JE	@@Input
		PUSH	CX
  @@WaitMouse:	CALL	LeaveTm
		CALL	GetMous
		JNZ	@@WaitMouse
		POP	CX
		JMP	@@Input
  @@Exit:	RET
FlipFl		ENDP

;---------------------------------------------------------------
;	Menu
;		CS:SI - control line
;		BH,BL - begin
;		DH,DL - size
;		   BP - color table
;		   AL - key bar
;		   CL - case
;	Return:	   CL - case
;		   AX - key code
;---------------------------------------------------------------
		.DATA
MnMn1_T		DW	4700h, MnMnHome
		DW	4F00h, MnMnEnd
MnMn2_T		DW	4B00h, MnMnLeft
		DW	4D00h, MnMnRight
		DW	0E08h, MnMnLeft
		DW	0020h, MnMnRight
		DW	0

		.CODE	Subs
MinMnu		PROC	NEAR
		USES	SI,DI
		CALL	HidCur
		MOV	DI,SI
		MOV	CH,AL
  @@Input:	MOV	AH,CS:[BP+0]
		MOV	AL,CS:[BP+1]
		CALL	MnMn40
		MOV	AL,CH
		AND	AL,7Fh
		CALL	Input
		CALL	MnMn10
		JC	@@Input
		CMP	AH,0FCh
		JE	@@Exit
		PUSH	AX
		MOV	AH,CS:[BP+0]
		MOV	AL,CS:[BP+2]
		CALL	MnMn40
		POP	AX
  @@Exit:	RET
MinMnu		ENDP

MnMn10		PROC	NEAR
		CMP	AH,0FCh
		JNE	@@Keyb
  @@ChkMouse:	XCHG	CX,SI			; Mouse
		CALL	GetMous
		MOV	AH,0FCh
		XCHG	CX,SI
		JZ	@@ExitC
		CMP	AL,1
		JNE	@@ExitNC
		PUSH	DX
		PUSH	CX
		MOV	CX,SI
		MOV	SI,DI
		CALL	MnMn60
		CALL	InWind
		MOV	SI,CX
		POP	CX
		POP	DX
		JC	@@ExitNC
		MOV	AX,SI
		SUB	AX,BX
		MOV	SI,DI
  @@WhatItem:	CMP	BYTE PTR CS:[SI],0
		JE	@@LeaveTm1
		MOV	AH,CS:[SI+1]
		CMP	AL,AH
		JB	@@LeaveTm1
		ADD	AH,CS:[SI]
		CMP	AL,AH
		JB	@@CmpItem
		INC	SI
		INC	SI
		JMP	@@WhatItem
  @@LeaveTm1:	CALL	LeaveTm
		JMP	@@ChkMouse
  @@CmpItem:	MOV	AX,SI
		SUB	AX,DI
		SHR	AX,1
		CMP	AL,CL
		MOV	CL,AL
		JNE	@@ExitC
		PUSH	DX
		PUSH	CX
		PUSH	BX
		ADD	BL,CS:[SI+1]
		MOV	DL,CS:[SI]
		MOV	DH,1
		XOR	SI,SI
  @@WaitMouse:	CALL	GetMous
		JZ	@@RelMouse
		CALL	InWind
		MOV	AL,ES:Pointers[4]
		JNC	@@CurMouse
		MOV	AL,0
  @@CurMouse:	MOV	AH,0
		CMP	AX,SI
		JE	@@LeaveTm2
		MOV	SI,AX
		CALL	TypMous
		JMP	@@WaitMouse
  @@LeaveTm2:	CALL	LeaveTm
		JMP	@@WaitMouse
  @@RelMouse:	MOV	AL,0
		CALL	TypMous
		POP	BX
		POP	CX
		POP	DX
		OR	SI,SI
		JE	@@ExitC
		JMP	@@SetEnter
  @@Keyb:	MOV	SI,OFFSET MnMn1_T	; Keyboard control
		CMP	ES:MinMnuFlag,0
		JE	@@Case
		MOV	SI,OFFSET MnMn2_T
  @@Case:	CALL	Case
		CALL	UpCase			; Hot Key
		CMP	AL,'A'
		JB	@@ExitNC
		CMP	AL,'Z'
		JA	@@ExitNC
		MOV	SI,DI
  @@NextHotKey:	CMP	BYTE PTR CS:[SI],0
		JE	@@ExitC
		PUSH	ES
		PUSH	DI
		PUSH	CX
		MOV	CX,CS:[SI]
		PUSH	BX
		ADD	BL,CH
		CALL	BasAdr
		POP	BX
		MOV	CH,0
		DEC	DI
  @@FndHotKey:	INC	DI
		SCASB
		LOOPNE	@@FndHotKey
		POP	CX
		POP	DI
		POP	ES
		JE	@@SetHotKey
		INC	SI
		INC	SI
		JMP	@@NextHotKey
  @@SetHotKey:	MOV	CX,SI
		SUB	CX,DI
		SHR	CX,1
  @@SetEnter:	MOV	AX,1C0Dh
  @@ExitNC:	CLC
		RET
MnMnLeft:	OR	CL,CL			; Left
		JE	@@LastItem
		DEC	CX
  @@ExitC:	STC
		RET
  @@LastItem:	CALL	MnMn50
		JE	@@ExitC
		INC	CX
		JMP	@@LastItem
MnMnRight:	CALL	MnMn50			; Right
		JE	@@FirstItem
		INC	CX
		JMP	@@ExitC
  @@FirstItem:	MOV	CL,0
		JMP	@@ExitC
MnMnHome:	TEST	CH,80h			; Home
		JZ	@@ExitNC
		MOV	AX,4900h
		MOV	CL,0
		JMP	@@ExitNC
MnMnEnd:	TEST	CH,80h			; End
		JZ	@@ExitNC
		MOV	AX,5100h
  @@ExitLast:	CALL	MnMn50
		JE	@@ExitNC
		INC	CX
		JMP	@@ExitLast
MnMn10		ENDP

MnMn40		PROC	NEAR			; Show menus
		USES	ES
		PUSH	CX
		MOV	SI,DI
		CALL	BasAdr
		CALL	Color
		XCHG	AL,AH
		MOV	CL,DL
		MOV	CH,0
		PUSH	DI
		CALL	MvsColr
		POP	DI
		POP	CX
		PUSH	CX
		PUSH	SI
		CALL	MnMn41
		POP	SI
		POP	CX
		CALL	Window
		MOV	DI,SI
		RET
MnMn40		ENDP

MnMn41		PROC	NEAR			; Show current menu
		CALL	Color
		MOV	AL,AH
		MOV	CH,0
		ADD	CX,CX
		ADD	SI,CX
		MOV	CL,CS:[SI+1]
		ADD	CX,CX
		ADD	DI,CX
		MOV	CL,CS:[SI]
		CALL	MvsColr
		RET
MnMn41		ENDP

MnMn50		PROC	NEAR			; Check last menu
		USES	CX,DI
		MOV	CH,0
		ADD	CX,CX
		ADD	DI,CX
		CMP	BYTE PTR CS:[DI+2],0
		RET
MnMn50		ENDP

MnMn60		PROC	NEAR			; Size of window
		USES	SI
  @@Next:	MOV	DX,CS:[SI]
		INC	SI
		INC	SI
		CMP	BYTE PTR CS:[SI],0
		JNE	@@Next
		ADD	DL,DH
		MOV	DH,1
		RET
MnMn60		ENDP

;---------------------------------------------------------------
;	Save line in history buffer
;		DS:SI - line
;		ES:DI - history buffer
;		   CX - length of history buffer
;---------------------------------------------------------------
SaveHistory	PROC	FAR
		USES	AX,BX,CX,DX,SI,DI
		MOV	BX,DI
		MOV	DX,CX
		PUSH	ES
		PUSH	DI
		PUSH	DS
		POP	ES
		MOV	DI,SI
		CALL	StrLen
		POP	DI
		POP	ES
		MOV	AL,0
  @@CompLine:	SCASB
		JE	@@Compress
		DEC	DI
		PUSH	DI
		PUSH	SI
		PUSH	CX
		REPE	CMPSB
		POP	CX
		POP	SI
		POP	DI
		JE	@@Compress
		PUSH	CX
		MOV	CH,0FFh
		REPNE	SCASB
		POP	CX
		JMP	@@CompLine
  @@Compress:	PUSH	SI
		PUSH	CX
		MOV	SI,DI
		ADD	DI,CX
		MOV	AX,DI
		ADD	DX,BX
		SUB	AX,DX
		JBE	@@MoveHist
		SUB	SI,AX
		MOV	DI,DX
  @@MoveHist:	MOV	CX,SI
		SUB	CX,BX
		DEC	SI
		DEC	DI
		STD
		PUSH	DS
		PUSH	ES
		POP	DS
		REP	MOVSB
		POP	DS
		CLD
		POP	CX
		POP	SI
		MOV	DI,BX
		REP	MOVSB
		MOV	AL,0
  @@FindEnd:	MOV	SI,DI
		SCASB
		JE	@@Exit
		MOV	CH,0FFh
		REPNE	SCASB
		CMP	DI,DX
		JB	@@FindEnd
		MOV	ES:[SI],AL
  @@Exit:	RET
SaveHistory	ENDP
