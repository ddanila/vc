;---------------------------------------------------------------
;		Keyboard interface
;
;		Version:	16.07.2000
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Get keystroke
;	Return:	NZ - AX = key
;		ZF - none
;---------------------------------------------------------------
		.DATA
ScanCodes	DB	03h,1Eh,30h,3Eh, 20h,12h,21h,22h
		DB	0Eh,0Fh,1Ch,25h, 26h,1Ch,31h,18h
		DB	19h,10h,13h,1Fh, 14h,16h,2Fh,11h
		DB	2Dh,15h,2Ch,01h, 2Bh,1Bh,07h,0Ch

		.CODE	Subs
GetKey		PROC	FAR
		USES	BX,DX
		CMP	ES:WinClose,0
		JE	@@ChkKey
		MOV	DX,100h
		MOV	AX,168Fh
		INT	2Fh
		CMP	AX,1
		MOV	AX,4400h
		JB	@@Exit
  @@ChkKey:	XOR	AX,AX
		XCHG	AX,ES:KeyStroke
		OR	AX,AX
		JNE	@@Ready
		CMP	ES:StdIn,1
		JE	@@GetStd
		JA	@@GetExt
		MOV	AH,ES:ExtKeyb
		INC	AH
		INT	16h
		JZ	@@Exit
		MOV	AH,ES:ExtKeyb
		INT	16h
		CMP	ES:ExtKeyb,0
		JE	@@Ready
		CMP	AL,0E0h
		JNE	@@Ready
		CMP	AH,3Bh
		JB	@@Ready
		MOV	AL,0
		JMP	@@Ready
  @@GetStd:	MOV	DL,0FFh
		MOV	AH,6
		INT	21h
		JZ	@@Exit
		CMP	AL,0
		JE	@@GetExt
		MOV	AH,0
		CMP	AL,20h
		JAE	@@Ready
		MOV	BX,AX
		MOV	AH,CS:ScanCodes[BX]
		JMP	@@Ready
  @@GetExt:	MOV	ES:StdIn,2
		MOV	DL,0FFh
		MOV	AH,6
		INT	21h
		JZ	@@Exit
		MOV	ES:StdIn,1
		MOV	AH,AL
		MOV	AL,0
  @@Ready:	OR	SP,SP
  @@Exit:	PUSHF
		CMP	AX,0E00Dh
		JE	@@Convert
		CMP	AX,0E00Ah
		JNE	@@NoEnter
  @@Convert:	MOV	AH,1Ch
  @@NoEnter:	POPF
		RET
GetKey		ENDP

;---------------------------------------------------------------
;	Keyboard status
;	Return:	   AX - status
;---------------------------------------------------------------
KeybStat	PROC	FAR
		USES	DS
		MOV	DS,CS:DataSegment
		MOV	AL,0
		CMP	AL,DS:StdIn
		JNE	@@Exit
		MOV	AH,2
		INT	16h
  @@Exit:	RET
KeybStat	ENDP

;---------------------------------------------------------------
;	Clear keyboard buffer
;---------------------------------------------------------------
ClrKbd		PROC	FAR
		USES	AX
		CMP	ES:StdIn,0
		JNZ	@@Exit
		CMP	ES:ClrBuf,0
		JZ	@@Exit
  @@GetChar:	CALL	GetKey
		JNZ	@@GetChar
  @@Exit:	RET
ClrKbd		ENDP

;---------------------------------------------------------------
;	Break test
;	Return:	CY=1 - break
;---------------------------------------------------------------
TstBrk		PROC	FAR
		USES	AX,BX
		MOV	BL,1
		CMP	ES:TaskStack,0
		JNE	@@Exit
  @@ChkKbd:	CMP	ES:StdIn,0
		JNZ	@@ChkStdIn
		MOV	AH,ES:ExtKeyb
		INC	AH
		INT	16h
		JMP	@@ChkKey
  @@ChkStdIn:	CMP	ES:StdFile,0
		JZ	@@Exit
		CALL	GetKey
  @@ChkKey:	JZ	@@Exit
		OR	AX,AX
		JE	@@GetChar
		CMP	AX,2E03h
		JE	@@GetChar
		CMP	AX,011Bh
		JE	@@GetChar
		CMP	AX,4400h
		JNE	@@Exit
  @@GetChar:	MOV	BL,0
		CMP	ES:StdIn,0
		JNZ	@@ChkKbd
		MOV	AH,ES:ExtKeyb
		INT	16h
		JMP	@@ChkKbd
  @@Exit:	CMP	BL,1
		RET
TstBrk		ENDP

;---------------------------------------------------------------
;	Keyboard input
;		   AL - Key bar case
;	Return:	   AX - Key code
;		 CF=0 - key code
;		 CF=1 - mouse key bar
;---------------------------------------------------------------
Input		PROC	FAR
		PUSH	AX
		CALL	Input0
		CMP	AX,3B00h
		JNE	@@Exit
;		CALL	Help
		POP	AX
		JMP	Input
  @@Exit:	INC	SP
		INC	SP
		RET
Input		ENDP

Input0		PROC	FAR
		USES	DS,SI,DI,DX,CX,BX
		LOCAL	IKeyBar	:BYTE
		LOCAL	BlMouse	:BYTE
		LOCAL	KbdFlgs	:BYTE
		LOCAL	AltLock	:BYTE
		LOCAL	ClkFlag	:BYTE
		LOCAL	MouseCu	:BYTE
		LOCAL	ClkBuf	:WORD:6
		LOCAL	BlTime	:DWORD
		LOCAL	AltTime	:DWORD
		PUSH	ES
		POP	DS
		MOV	IKeyBar,AL
		CALL	GetKey
		JNZ	@@ExitInput
  @@GetMouse:	XOR	AX,AX
		CALL	GetMous
		MOV	AH,0FCh
		JNZ	@@ExitInput
		MOV	MouseCu,0
		CALL	Timer
		MOV	AltTime.Lo,DX
		MOV	AltTime.Hi,CX
		PUSH	DS
		PUSH	ES
		MOV	BX,0FAh
		ADD	BL,DS:Columns
		CALL	BasAdr
		PUSH	ES
		PUSH	SS
		POP	ES
		POP	DS
		MOV	SI,DI
		LEA	DI,ClkBuf
		MOV	CX,6
		REP	MOVSW
		POP	ES
		POP	DS
		CALL	KeybStat
		MOV	KbdFlgs,AL
		MOV	AltLock,0
  @@InitVars1:	MOV	ClkFlag,0
  @@InitVars2:	MOV	BlMouse,0
  @@SetBlTime:	MOV	CX,0FFFFh
		CMP	DS:BlDelay,0
		JE	@@BlankTime
		CMP	DS:TaskStack,0
		JNE	@@BlankTime
		MOV	AH,2Ch
		INT	21h
		ADD	CL,DS:BlDelay
		CMP	CL,60
		JB	@@BlankTime
		SUB	CL,60
		INC	CH
  @@BlankTime:	MOV	BlTime.Lo,DX
		MOV	BlTime.Hi,CX
  @@KeyBar:	MOV	AL,IKeyBar
		CALL	PutKey
		CALL	LeaveTm
		CMP	DS:TaskStack,0
		JNE	@@AutoEnter
		CMP	DS:BlDelay,0		; Blank screen
		JE	@@PutClock
		MOV	AH,2Ch
		INT	21h
		CMP	CH,0
		JNE	@@ChkBlank
		CMP	BYTE PTR BlTime[3],24
		JB	@@ChkBlank
		MOV	BYTE PTR BlTime[3],CH
  @@ChkBlank:	CMP	CX,BlTime.Hi
		JB	@@PutClock
		JA	@@BlankScr
		CMP	DX,BlTime.Lo
		JB	@@PutClock
  @@BlankScr:	CALL	Blank
		XOR	BX,BX
		MOV	DH,DS:Lines
		MOV	DL,DS:Columns
		CALL	Window
  @@ExitMouse:	XOR	AX,AX
		CALL	GetMous
		JZ	@@InitVars1
		CALL	LeaveTm
		JMP	@@ExitMouse
  @@PutClock:	CMP	DS:Clock,0		; Output clock
		JE	@@AutoEnter
		MOV	BX,0FAh
		ADD	BL,DS:Columns
		CALL	BasAdr
		INC	DI
		INC	DI
		MOV	AH,2Ch
		INT	21h
		MOV	AL,CH
		CBW
		CMP	DS:Country.TimeFmt,0
		JNZ	@@Separator
		DEC	DI
		DEC	DI
		MOV	DH,12
		DIV	DH
		OR	AH,AH
		JNE	@@ClockAP
		MOV	AH,12
  @@ClockAP:	CMP	AL,0
		MOV	AL,AH
		MOV	DH,'a'
		JE	@@Separator
		MOV	DH,'p'
  @@Separator:	CMP	DL,50
		MOV	DL,' '
		JB	@@ChkWrite
		MOV	DL,DS:Country.TimeSep
  @@ChkWrite:	CMP	DL,ClkFlag
		JE	@@AutoEnter
		MOV	ClkFlag,DL
		CALL	HexDec
		XCHG	CX,AX
		CALL	HexDec
		PUSH	AX
		MOV	AH,Clock_C
		CALL	Color
		MOV	AL,CL
		CMP	AL,'0'
		JNE	@@BegSpace
		MOV	AL,' '
  @@BegSpace:	STOSW
		MOV	AL,CH
		STOSW
		POP	CX
		MOV	AL,DL
		STOSW
		MOV	AL,CL
		STOSW
		MOV	AL,CH
		STOSW
		CMP	DS:Country.TimeFmt,0
		JNZ	@@WriteClock
		MOV	AL,DH
		STOSW
  @@WriteClock:	MOV	DX,106h
		CALL	Window
  @@AutoEnter:	PUSH	DS			; Auto Enter
		POP	ES
		CMP	DS:AutoTim.Lo,0FFFFh
		JE	@@ChkKbd
		MOV	AH,2Ch
		INT	21h
		MOV	AX,0FE00h
		CMP	CX,DS:AutoTim.Lo
		JB	@@ChkKbd
		JA	@@ResClk1
		CMP	DX,DS:AutoTim.Hi
		JB	@@ChkKbd
  @@ResClk1:	JMP	@@ResClock
  @@ChkKbd:	CALL	Rnd			; Check keyboard buffer
		CALL	GetKey
		JNZ	@@ResClk1
		CALL	GetMous			; Check mouse
		MOV	AH,0FCh
		JNZ	@@ResClk1
		MOV	SI,OFFSET MouseCur
  @@CurMouse:	LODSW
		INC	AL
		JE	@@SetMouse
		DEC	AX
		MOV	BX,AX
		LODSW
		MOV	DX,AX
		LODSB
		CALL	InWind
		JC	@@CurMouse
  @@SetMouse:	CMP	AL,MouseCu
		JE	@@ChkShift
		MOV	MouseCu,AL
		CALL	TypMous
  @@ChkShift:	CALL	KeybStat		; Check shift status
		MOV	BL,AL
		XCHG	BL,KbdFlgs
		CMP	DS:DesqVie,0
		JNZ	@@BlankMouse
		CMP	DS:AltBar,0
		JZ	@@BlankMouse
		PUSH	BX
		PUSH	AX
		CALL	Timer
		MOV	AX,CX
		MOV	BX,DX
		XCHG	DX,AltTime.Lo
		XCHG	CX,AltTime.Hi
		SUB	BX,DX
		SBB	AX,CX
		CMP	BX,3
		CMC
		ADC	AX,AX
		POP	AX
		POP	BX
		JNE	@@BlankMouse
		MOV	AH,BL
		XOR	AH,AL
		JZ	@@BlankMouse
		CMP	AH,8
		JNE	@@BlankMouse
		MOV	BH,1
		TEST	AL,8
		JNZ	@@AltStatus
		CMP	AltLock,0
		JE	@@BlankMouse
		MOV	AX,0FD00h
		JMP	@@ResClock
  @@BlankMouse:	AND	BL,0F0h
		XOR	AL,BL
		JNZ	@@ChkAlt
		CMP	DS:TaskStack,0
		JNE	@@InitVars2
		CMP	DS:OS2box,0		; Windowed session
		JZ	@@ChkMouse
		MOV	DX,3D6h
		MOV	AL,82h
		OUT	DX,AL
		IN	AL,DX
		OR	AL,AL
		JE	@@InitVars2
  @@ChkMouse:	CALL	GetMous			; Check mouse position
		MOV	DX,102h
		MOV	BX,0FEh
		ADD	BL,DS:Columns
		CALL	InWind
		JC	@@MouseFlag
		CMP	BlMouse,0
		JE	@@NoBlMouse
		JMP	@@BlankScr
  @@MouseFlag:	MOV	BlMouse,1
  @@NoBlMouse:	MOV	BH,DS:Lines
		DEC	BH
		CALL	InWind
		JNC	@@SetBlTim1
		JMP	@@KeyBar
  @@ChkAlt:	TEST	AL,0F7h
		JZ	@@SetBlTim1
		MOV	BH,0
  @@AltStatus:	MOV	AltLock,BH
  @@SetBlTim1:	JMP	@@SetBlTime
  @@ResClock:	CMP	DS:Clock,0		; Restore clock
		JZ	@@ResMouse
		MOV	BX,0FAh
		ADD	BL,DS:Columns
		PUSH	DS
		PUSH	ES
		PUSH	SS
		POP	DS
		LEA	SI,ClkBuf
		CALL	BasAdr
		MOV	CX,6
		REP	MOVSW
		POP	ES
		POP	DS
		MOV	DX,106h
		CALL	Window
  @@ResMouse:	CMP	MouseCu,0		; Restore mouse cursor
		JZ	@@ExitInput
		PUSH	AX
		MOV	AL,0
		CALL	TypMous
		POP	AX
  @@ExitInput:	MOV	DS:AutoTim.Lo,0FFFFh
		CMP	AH,0FCh
		JNE	@@RetKey
		CMP	DS:KeyBar,0
		JZ	@@RetKey
		CALL	GetMous			; Mouse key bar
		MOV	AH,0FCh
		INC	CH
		CMP	CH,DS:Lines
		JNE	@@RetKey
		DEC	CH
		XOR	SI,SI
		XOR	DI,DI
		PUSH	AX
		MOV	AL,DS:Columns
		MOV	AH,0
		MOV	BL,10
		DIV	BL
		MOV	BX,AX
		MOV	AL,CL
		MOV	AH,0
		INC	AX
		SUB	AL,BH
		DIV	BL
		MOV	BL,AL
		MOV	BH,0
		POP	AX
		CMP	BL,9
		JBE	@@InKeyBar
		MOV	BL,9
  @@InKeyBar:	PUSH	BX
		PUSH	AX
		MOV	AL,DS:Columns
		MOV	AH,0
		MOV	BH,10
		DIV	BH
		MOV	BH,AH
		MOV	DL,AL
		MOV	DH,1
		DEC	DX
		CMP	BL,9
		CMC
		ADC	DL,0
		MUL	BL
		ADD	AL,BH
		JE	@@AddrKeyBar
		DEC	AX
		INC	DX
  @@AddrKeyBar:	MOV	BL,AL
		MOV	BH,DS:Lines
		DEC	BH
		POP	AX
		CALL	InWind
		POP	DX
		MOV	BL,DL
		MOV	BH,AL
		MOV	AL,0
		JC	@@MouseCur
		OR	DI,DI
		JZ	@@SetKeyBar
		MOV	AL,DS:Pointers[4]
  @@MouseCur:	MOV	AH,0
		CMP	AX,SI
		JE	@@SetKeyBar
		MOV	SI,AX
		CALL	TypMous
  @@SetKeyBar:	TEST	BH,7
		JZ	@@SetFnKey
		CALL	KeybStat
		MOV	AH,3
		TEST	AL,8
		JNZ	@@PutKeyBar
		MOV	AH,2
		TEST	AL,4
		JNZ	@@PutKeyBar
		MOV	AH,1
		TEST	AL,3
		JNZ	@@PutKeyBar
		MOV	AH,3
		TEST	BH,2
		JNZ	@@PutKeyBar
		MOV	AH,1
		TEST	BH,4
		JNZ	@@PutKeyBar
		MOV	AH,0
  @@PutKeyBar:	MOV	AL,IKeyBar
		CALL	PutKey0
		CALL	GetMous
		OR	DI,DI
		JNZ	@@WaitMouse
		PUSH	ES
		PUSH	BX
		PUSH	AX
		MOV	AL,DS:Columns
		MOV	AH,0
		MOV	BH,10
		DIV	BH
		MOV	BH,AH
		MUL	BL
		INC	AX
		INC	AX
		ADD	AL,BH
		MOV	BL,AL
		MOV	BH,CH
		CALL	BasAdr
		CMP	BYTE PTR ES:[DI],' '
		POP	AX
		POP	BX
		POP	ES
		JNE	@@WaitMouse
		MOV	AH,0FCh
  @@RetKey:	CLC
		JMP	@@Exit
  @@WaitMouse:	CALL	LeaveTm
		JMP	@@InKeyBar
  @@SetFnKey:	CMP	AL,0
		JE	@@GetMous1
		MOV	AL,0
		CALL	TypMous
		CALL	KeybStat
		MOV	AH,68h
		TEST	AL,8
		JNZ	@@SetKey
		MOV	AH,5Eh
		TEST	AL,4
		JNZ	@@SetKey
		MOV	AH,54h
		TEST	AL,3
		JNZ	@@SetKey
		MOV	AH,68h
		TEST	DH,2
		JNZ	@@SetKey
		MOV	AH,54h
		TEST	DH,4
		JNZ	@@SetKey
		MOV	AH,3Bh
		TEST	DH,1
		JNZ	@@SetKey
  @@GetMous1:	JMP	@@GetMouse
  @@SetKey:	ADD	AH,BL
		MOV	AL,0
		STC
  @@Exit:	RET
Input0		ENDP
