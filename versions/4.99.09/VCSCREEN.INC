;---------------------------------------------------------------
;		Screen interface
;
;		Version:	31.12.1998
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Set screen parameters
;---------------------------------------------------------------
		.CODE	Subs
ScreenParms	PROC	FAR
		MOV	BH,0			; ColorMono
		CMP	DS:ScrMod,7
		JNE	@@MonoEGA
		MOV	BH,1
  @@MonoEGA:	CMP	DS:TstEGA,0
		JZ	@@ColorMono
		MOV	BL,10h
		MOV	AH,12h
		INT	10h
  @@ColorMono:	MOV	DS:ColorMono,BH
		MOV	AH,1			; Snow
		CMP	DS:ColorMono,0
		JNZ	@@Snow3
		CMP	DS:SnowMod,1
		JE	@@Snow3
		MOV	DX,03DAh
		MOV	BX,8000h
		JB	@@Snow1
		CMP	DS:TstEGA,BL
		JNE	@@Snow3
  @@Snow1:	STI
		DEC	BX
		JE	@@Snow3
		MOV	CX,7			; Const2_1 ???
		CLI
  @@Snow2:	IN	AL,DX
		TEST	AL,1
		JZ	@@Snow1
		LOOP	@@Snow2
		MOV	AH,0
  @@Snow3:	STI
		MOV	DS:Snow,AH
		XOR	DI,DI			; Screen Offset
		MOV	AH,0Fh
		INT	10h
		OR	BH,BH
		JE	@@ScrOffs
		PUSH	DS
		MOV	DS,DI
		MOV	DI,DS:[44Eh]
		POP	DS
  @@ScrOffs:	MOV	DS:Screen.Offs,DI
		MOV	BX,0B800h		; Screen Segment
		CMP	DS:ColorMono,0
		JZ	@@TopView
		MOV	BH,0B0h
  @@TopView:	MOV	ES,BX			; Virtual screen buffer
		MOV	AH,0FEh
		INT	10h
		MOV	CX,ES
		PUSH	DS
		POP	ES
		MOV	AL,0
		CMP	CX,BX
		JE	@@SetSegm
		MOV	AL,1
		MOV	DS:Snow,AL
  @@SetSegm:	MOV	DS:Screen.Segm,CX
		MOV	DS:TopView,AL
		RET
ScreenParms	ENDP

;---------------------------------------------------------------
;	Print string
;		DS:SI - string
;---------------------------------------------------------------
PrintStr	PROC	FAR
  @@GetChar:	LODSB
		CMP	AL,0
		JE	@@Exit
		MOV	BL,7
		MOV	AH,0Eh
		INT	10h
		JMP	@@GetChar
  @@Exit:	RET
PrintStr	ENDP

;---------------------------------------------------------------
;	Set cursor
;		   AL - column
;		   AH - line
;---------------------------------------------------------------
Cursor		PROC	FAR
		USES	AX,BX,DX
		MOV	DX,AX
		MOV	AH,0Fh
		INT	10h
		MOV	AH,2
		INT	10h
		RET
Cursor		ENDP

;---------------------------------------------------------------
;	Set cursor on command prompt
;---------------------------------------------------------------
Cursor1		PROC	FAR
		USES	AX
		MOV	AL,ES:DosCol
		SUB	AL,ES:OutCol
		ADD	AL,ES:BegCol
		MOV	AH,ES:DosLin
		CALL	Cursor
		RET
Cursor1		ENDP

;---------------------------------------------------------------
;	Get cursor size & set it
;	Return:	CX - cursor size
;---------------------------------------------------------------
ChgCur		PROC	FAR
		USES	BX,DX,BP
		MOV	BH,0
		MOV	AH,3
		INT	10h
		PUSH	CX
		MOV	CX,0607h
		CMP	ES:ScrMod,7
		JNE	@@LapTop
		MOV	CX,0B0Ch
  @@LapTop:	CMP	ES:ColTyp,2
		JNE	@@SetCur
		MOV	CH,0
  @@SetCur:	MOV	AH,1
		INT	10h
		POP	CX
		RET
ChgCur		ENDP

;---------------------------------------------------------------
;	Hide cursor
;---------------------------------------------------------------
HidCur		PROC	FAR
		USES	AX
		MOV	AX,7F00h
		IFDEF	OFFICIAL
		CALL	Cursor
		ENDIF
		RET
HidCur		ENDP

;---------------------------------------------------------------
;	Beep sound
;---------------------------------------------------------------
Beep		PROC	FAR
		USES	AX,BP,DS
		MOV	DS,CS:DataSegment
		CMP	DS:ErrSnd,0
		JE	@@Exit
		MOV	AX,0E07h
		INT	10h
  @@Exit:	RET
Beep		ENDP

;---------------------------------------------------------------
;	INT 10h with mouse
;---------------------------------------------------------------
Int10m		PROC	FAR
		USES	BP
		PUSH	AX
		MOV	AL,0
		CALL	CurMous
		POP	AX
		INT	10h
		PUSH	AX
		MOV	AL,1
		CALL	CurMous
		POP	AX
		RET
Int10m		ENDP

;---------------------------------------------------------------
;	Convert coordinars to address
;		BH,BL - coordinats
;	Return:	ES:DI - address
;---------------------------------------------------------------
BasAdr		PROC	FAR
		USES	AX,BX
		MOV	ES,CS:DataSegment
		MOV	AL,ES:Columns
		MUL	BH
		MOV	BH,0
		ADD	AX,BX
		ADD	AX,AX
		MOV	DI,AX
		MOV	ES,ES:ScreenBuf
		RET
BasAdr		ENDP

;---------------------------------------------------------------
;	Shift windows in EGA lines
;		   BH - line number
;	Return:	   BH - corrected line number
;---------------------------------------------------------------
SftEGA		PROC	FAR
		USES	AX
		MOV	AL,ES:Lines
		SUB	AL,25
		JBE	@@Exit
		SHR	AL,1
		ADD	BH,AL
  @@Exit:	RET
SftEGA		ENDP

;---------------------------------------------------------------
;	Save window
;		BH,BL - begin
;		DH,DL - size
;---------------------------------------------------------------
SavWin		PROC	FAR
		USES	AX,CX,DX,SI,DI,ES,DS
		CLD
		MOV	DS,CS:DataSegment
		MOV	DS:Quote,0
		MOV	AL,DL
		MUL	DH
		INC	AX
		ADD	AX,AX
		INC	AX
		MOV	CL,4
		SHR	AX,CL
		INC	AX
		MOV	CX,DS:WindowsPoint
		SUB	CX,AX			; ???
		MOV	DS:WindowsPoint,CX
		MOV	AL,DS:Columns
		MOV	AH,0
		ADD	AX,AX
		CALL	BasAdr
		PUSH	ES
		POP	DS
		MOV	SI,DI
		MOV	ES,CX
		MOV	ES:[0],BX
		MOV	ES:[2],DX
		MOV	DI,4
		MOV	CL,DH
		MOV	CH,0
		MOV	DH,0
  @@NextLine:	PUSH	CX
		PUSH	SI
		MOV	CX,DX
		REP	MOVSW
		POP	SI
		POP	CX
		ADD	SI,AX
		LOOP	@@NextLine
		RET
SavWin		ENDP

;---------------------------------------------------------------
;	Restore window
;	Return:	BH,BL - begin
;		DH,DL - size
;---------------------------------------------------------------
ResWin		PROC	FAR
		USES	AX,CX,SI,DI,DS,ES
		CLD
		MOV	ES,CS:DataSegment
		MOV	DS,ES:WindowsPoint
		MOV	ES:Quote,0
		MOV	BX,DS:[0]
		MOV	AX,DS:[2]
		PUSH	AX
		MUL	AH
		INC	AX
		ADD	AX,AX
		INC	AX
		MOV	CL,4
		SHR	AX,CL
		INC	AX
		ADD	ES:WindowsPoint,AX
		MOV	AL,ES:Columns
		MOV	AH,0
		ADD	AX,AX
		MOV	DL,DS:[2]
		MOV	DH,0
		MOV	CL,DS:[3]
		MOV	CH,0
		CALL	BasAdr
		MOV	SI,4
  @@NextLine:	PUSH	CX
		PUSH	DI
		MOV	CX,DX
		REP	MOVSW
		POP	DI
		POP	CX
		ADD	DI,AX
		LOOP	@@NextLine
		POP	DX
		RET
ResWin		ENDP

;---------------------------------------------------------------
;	Restore user screen
;		BH,BL - begin
;		DH,DL - size
;---------------------------------------------------------------
ResUser		PROC	FAR
		USES	AX,CX,DX,SI,DI,DS,ES
		MOV	AL,ES:Columns
		MOV	AH,0
		ADD	AX,AX
		MOV	DS,ES:UserScreen
		CALL	BasAdr
		MOV	SI,DI
		MOV	CL,DH
		MOV	CH,0
		MOV	DH,0
  @@Next:	PUSH	SI
		PUSH	DI
		PUSH	CX
		MOV	CX,DX
		REP	MOVSW
		POP	CX
		POP	DI
		POP	SI
		ADD	SI,AX
		ADD	DI,AX
		LOOP	@@Next
		RET
ResUser		ENDP

;---------------------------------------------------------------
;	Output window
;		BH,BL - begin
;		DH,DL - size
;---------------------------------------------------------------
Window		PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,DS,ES
		MOV	ES,CS:DataSegment
		PUSH	ES
		MOV	AL,0
		CALL	CurMous
		PUSH	ES
		CALL	BasAdr
		MOV	SI,DI
		PUSH	ES
		POP	DS
		POP	ES
		MOV	AL,ES:Snow
		MOV	CL,DL
		MOV	DL,ES:Columns
		LES	DI,ES:Screen
		ADD	DI,SI
		CMP	DH,1
		JE	@@OneLine
		MOV	BH,0
		ADD	BX,BX
		SUB	SI,BX
		SUB	DI,BX
		PUSH	DI
		PUSH	DX
		CALL	MovScr
		POP	AX
		MUL	AH
		MOV	CX,AX
		POP	DI
		JMP	@@ShadowBuf
  @@OneLine:	MOV	CH,0
		PUSH	DI
		PUSH	CX
		MOV	AH,AL
		MOV	DX,03DAh
  @@Next:	OR	AH,AH
		JNZ	@@Move
  @@ChkSnow1:	IN	AL,DX
		TEST	AL,1
		JNZ	@@ChkSnow1
		CLI
  @@ChkSnow2:	IN	AL,DX
		TEST	AL,1
		JZ	@@ChkSnow2
  @@Move:	MOVSW
		STI
		LOOP	@@Next
		POP	CX
		POP	DI
  @@ShadowBuf:	POP	ES
		CMP	ES:TopView,0
		JZ	@@MouseOn
		MOV	AH,0FFh
		INT	10h
  @@MouseOn:	MOV	AL,1
		CALL	CurMous
		RET
Window		ENDP

;---------------------------------------------------------------
;	Save screen
;---------------------------------------------------------------
SavScr		PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,DS,ES
		MOV	DS,ES:Screen.Segm
		MOV	SI,ES:Screen.Offs
		MOV	DH,ES:Lines
		MOV	DL,ES:Columns
		MOV	AL,ES:Snow
		MOV	ES,ES:ScreenBuf
		XOR	DI,DI
		CALL	MovScr
		RET
SavScr		ENDP

;---------------------------------------------------------------
;	Move screen bytes
;		   DH - number of lines
;		   DL - width of screen
;		   AL - snow
;		DS:SI => ES:DI
;---------------------------------------------------------------
MovScr		PROC	NEAR
		CLD
		MOV	BL,AL
		MOV	BH,7			; Const1 ???
  @@NextBlock:	CMP	DH,BH
		JAE	@@BlockSize
		MOV	BH,DH
  @@BlockSize:	MOV	AL,DL
		MUL	BH
		PUSH	AX
		OR	BL,BL
		JNZ	@@MoveBlock
		PUSH	DX
		MOV	DX,03DAh
  @@ChkSnow1:	IN	AL,DX
		TEST	AL,1
		JNZ	@@ChkSnow1
  @@ChkSnow2:	STI
		MOV	CX,10			; Const2 ???
		CLI
  @@ChkSnow3:	IN	AL,DX
		TEST	AL,1
		JZ	@@ChkSnow2
		LOOP	@@ChkSnow3
		POP	DX
  @@MoveBlock:	POP	CX
		REP	MOVSW
		STI
		SUB	DH,BH
		JNE	@@NextBlock
		RET
MovScr		ENDP

;---------------------------------------------------------------
;	Get color
;		   AH - color number
;	Return:	   AH - color
;---------------------------------------------------------------
Color		PROC	FAR
		USES	DI,ES
		MOV	ES,CS:DataSegment
		CALL	ColAddr
		MOV	AH,ES:[DI]
		RET
Color		ENDP

ColAddr		PROC	FAR
		USES	AX
		MOV	AL,6
		MUL	AH
		MOV	DI,AX
		MOV	AL,ES:ColTyp
		CMP	AL,3
		JB	@@ColOffs
		MOV	AL,1
  @@ColOffs:	CBW
		ADD	AX,AX
		CMP	ES:ColorMono,0
		JZ	@@ColAdr
		INC	AX
  @@ColAdr:	ADD	DI,AX
		ADD	DI,OFFSET ColTab
		RET
ColAddr		ENDP

;---------------------------------------------------------------
;	Shadow
;		ES:DI - address of screen buffer
;---------------------------------------------------------------
Shadow		PROC	FAR
		USES	AX
		INC	DI
		MOV	AH,ShadFl
		CALL	Color
		MOV	AL,ES:[DI]
		CMP	AH,1
		JB	@@Write
		PUSHF
		AND	AL,8Fh
		XOR	AL,8
		POPF
		JE	@@Write
		MOV	AL,7
  @@Write:	STOSB
		RET
Shadow		ENDP

;---------------------------------------------------------------
;	Fill color
;		   AL - color
;		ES:DI - buffer to fill
;---------------------------------------------------------------
MvsColr		PROC	FAR
		INC	DI
		STOSB
		LOOP	MvsColr
		RET
MvsColr		ENDP

;---------------------------------------------------------------
;	Move ASCIIZ line to screen buffer
;		DS:SI - line
;		BH,BL - position to move
;		   AH - color
;---------------------------------------------------------------
WriteCoLnBX	PROC	FAR
		CALL	BasAdr
WriteCoLnBX	ENDP

WriteCoLine	PROC	FAR
		LODSB
		CMP	AL,0
		JE	@@Exit
		STOSW
		JMP	WriteCoLine
  @@Exit:	RET
WriteCoLine	ENDP

;---------------------------------------------------------------
;	Clear screen line
;		   BH - line
;---------------------------------------------------------------
ClsLine		PROC	FAR
		USES	CX,DI,ES
		MOV	BL,0
		CALL	BasAdr
		MOV	CL,DS:Columns
		MOV	CH,0
		MOV	AH,DS:CrntCol
		MOV	AL,' '
		REP	STOSW
		RET
ClsLine		ENDP

;---------------------------------------------------------------
;	Blank screen
;---------------------------------------------------------------
Blank		PROC	NEAR
		USES	AX,BX,CX,DX,SI,DI,BP
		MOV	AL,0
		CALL	CurMous
		MOV	AH,0Fh
		INT	10h
		MOV	AH,3
		INT	10h
		PUSH	CX
		PUSH	DX
		MOV	CX,2000h
		MOV	AH,1
		INT	10h
		XOR	CX,CX
		MOV	DH,DS:Lines
		MOV	DL,DS:Columns
		DEC	DH
		DEC	DL
		MOV	AH,Sky1_C
		CALL	Color
		MOV	BH,AH
		MOV	AX,600h
		INT	10h
		MOV	DI,OFFSET Stars
		MOV	CX,NmbStrs*SIZE STAR/2
		XOR	AX,AX
		REP	STOSW
		XOR	CX,CX
		CALL	GetMous
		MOV	SI,CX
		CALL	KeybStat
		MOV	CL,AL
		XOR	DI,DI
  @@RndPos:	PUSH	SI
		PUSH	CX
		CALL	Rnd
		MOV	BL,DS:Lines
		MOV	BH,0
		MUL	BX
		MOV	CL,DL
		CALL	Rnd
		MOV	BL,DS:Columns
		MUL	BX
		MOV	DH,CL
		CALL	Rnd
		MOV	CL,3
		ROR	AX,CL
		CMP	AL,100
		JB	@@ChkStars
		MOV	BL,AL
		CALL	Rnd
		MOV	CL,5
		ROR	AL,CL
		CMP	AL,50
		JB	@@ChkStars
		OR	AL,AH
		AND	AL,1
		MOV	BH,AL
		MOV	SI,OFFSET Stars
		MOV	CX,NmbStrs
		XOR	BP,BP
  @@FindFree:	LODSW
		OR	AX,AX
		LODSW
		JNE	@@CmpPos
		OR	BP,BP
		JNE	@@FindNext
		MOV	BP,SI
		JMP	@@FindNext
  @@CmpPos:	CMP	AX,DX
		JE	@@ChkStars
  @@FindNext:	LOOP	@@FindFree
		OR	BP,BP
		JE	@@ChkStars
		MOV	DS:[BP-SIZE STAR].StarMod,BX
		MOV	DS:[BP-SIZE STAR].StarPos,DX
		MOV	AH,0Fh
		INT	10h
		MOV	AH,2
		INT	10h
		MOV	AH,Sky1_C
		CALL	Color
		MOV	BL,AH
		MOV	CX,1
		MOV	AL,DS:StarTab[0]
		MOV	AH,9
		INT	10h
  @@ChkStars:	MOV	SI,OFFSET Stars
		MOV	CX,NmbStrs
  @@NextStar:	PUSH	CX
		LODSW
		MOV	BX,AX
		LODSW
		OR	BX,BX
		JE	@@SkipStar
		MOV	DX,AX
		DEC	BX
		MOV	DS:[SI-SIZE STAR].StarClk,BL
		MOV	AL,DS:StarTab[1]
		CMP	BL,12
		JE	@@PutStar
		MOV	AL,DS:StarTab[2]
		CMP	BL,9
		JNE	@@ChgStar
		OR	BH,BH
		JE	@@PutStar
		MOV	BL,0
  @@ChgStar:	MOV	AL,DS:StarTab[3]
		CMP	BL,6
		JE	@@PutStar
		MOV	AL,DS:StarTab[4]
		CMP	BL,3
		JE	@@PutStar
		OR	BL,BL
		JNE	@@SkipStar
		XOR	AX,AX
		MOV	DS:[SI-SIZE STAR].StarMod,AX
		MOV	DS:[SI-SIZE STAR].StarPos,AX
		MOV	AL,' '
  @@PutStar:	PUSH	AX
		MOV	AH,0Fh
		INT	10h
		MOV	AH,2
		INT	10h
		POP	AX
		MOV	AH,Sky2_C
		CALL	Color
		MOV	BL,AH
		MOV	CX,1
		MOV	AH,9
		INT	10h
  @@SkipStar:	POP	CX
		LOOP	@@NextStar
		POP	CX
		POP	SI
  @@ChkExit:	CALL	GetKey
		JNZ	@@Exit
		CALL	KeybStat
		XCHG	AL,CL
		AND	AL,0F0h
		XOR	AL,CL
		JNZ	@@Exit
		MOV	BX,CX
		XOR	AX,AX
		XOR	CX,CX
		CALL	GetMous
		JNZ	@@Exit
		PUSH	BX
		MOV	BX,0FCh
		ADD	BL,DS:Columns
		MOV	DX,204h
		CALL	InWind
		POP	BX
		JC	@@MouseExit
		MOV	SI,CX
  @@MouseExit:	CMP	CX,SI
		JNE	@@Exit
		CALL	Timer
		MOV	CX,BX
		XCHG	DX,DI
		CMP	DX,DI
		JNE	@@RndPos
		CALL	LeaveTm
		JMP	@@ChkExit
  @@Exit:	MOV	AH,0Fh
		INT	10h
		POP	DX
		MOV	AH,2
		INT	10h
		POP	CX
		MOV	AH,1
		INT	10h
		MOV	AL,1
		CALL	CurMous
		RET
Blank		ENDP

Rnd		PROC	NEAR
		USES	BX,CX,DX
		MOV	BX,3
		MOV	AX,ES:Rand1
		MUL	BX
		MOV	CX,AX
		MOV	AX,ES:Rand2
		MOV	ES:Rand1,AX
		MUL	BX
		ADD	AX,CX
		ADD	AX,23457
		MOV	ES:Rand2,AX
		RET
Rnd		ENDP
