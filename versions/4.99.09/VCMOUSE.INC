;---------------------------------------------------------------
;		Mouse interface
;
;		Version:	04.05.1998
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Init mouse
;---------------------------------------------------------------
		.CODE	Subs
IniMous		PROC	FAR
		CMP	ES:MousRes,0
		JZ	@@Reset
		MOV	AX,21h
		INT	33h
		CMP	AX,21h
		JNE	@@SkipReset
  @@Reset:	XOR	AX,AX
		INT	33h
  @@SkipReset:	CMP	AX,0FFFFh
		CMC
		MOV	AL,0
		ADC	AL,AL
		MOV	ES:Mouse,AL
		JNZ	@@SetType
		MOV	AX,0FFFFh
		MOV	ES:MouseX,AX
		MOV	ES:MouseY,AX
		RET
  @@SetType:	MOV	AL,0
		CALL	TypMous
		MOV	DL,ES:Lines
		MOV	DH,0
		MOV	CL,3
		SHL	DX,CL
		DEC	DX
		XOR	CX,CX
		MOV	AX,8
		INT	33h
		MOV	DL,ES:Columns
		MOV	DH,0
		MOV	CL,3
		SHL	DX,CL
		DEC	DX
		XOR	CX,CX
		MOV	AX,7
		INT	33h
		MOV	CX,ES:MouseX
		MOV	DX,ES:MouseY
		MOV	AX,CX
		AND	AX,DX
		INC	AX
		JE	@@MouseOn
		MOV	AX,4
		INT	33h
  @@MouseOn:	MOV	AL,1
		CALL	CurMous
		RET
IniMous		ENDP

;---------------------------------------------------------------
;	Set type of mouse sursor
;		   AL - cursor type
;---------------------------------------------------------------
TypMous		PROC	FAR
		USES	AX,BX,CX,DX
		MOV	CX,0FFFFh
		MOV	DX,7700h
		CMP	AL,0
		JE	@@SetCurs
		CMP	AL,' '
		JE	@@SetCurs
		MOV	CL,0
		MOV	DL,AL
  @@SetCurs:	XOR	BX,BX
		MOV	AX,0Ah
		INT	33h
		RET
TypMous		ENDP

;---------------------------------------------------------------
;	Save mouse cursor
;---------------------------------------------------------------
SavMous		PROC	FAR
		USES	AX,BX,CX,DX
		CMP	ES:Mouse,0
		JZ	@@Exit
		MOV	AX,3
		INT	33h
		MOV	ES:MouseX,CX
		MOV	ES:MouseY,DX
  @@Exit:	RET
SavMous		ENDP

;---------------------------------------------------------------
;	Set mouse cursor
;		   AL - set/reset
;---------------------------------------------------------------
CurMous		PROC	FAR
		USES	AX
		CMP	ES:Mouse,0
		JZ	@@Exit
		CMP	AL,1
		MOV	AX,1
		ADC	AL,0
		INT	33h
  @@Exit:	RET
CurMous		ENDP

;---------------------------------------------------------------
;	Get mouse cursor
;	Return:	CH,CL - mouse cursor
;		   AX - mouse buttons
;---------------------------------------------------------------
GetMous		PROC	FAR
		USES	BX,DX
		XOR	CX,CX
		XOR	AX,AX
		CMP	ES:Mouse,AL
		STC
		JZ	@@Exit ; no mouse
		MOV	AX,3
		INT	33h ; get mouse position and buttons status
		MOV	AX,CX
		MOV	CL,3
		SHR	AX,CL
		SHR	DX,CL
		MOV	CL,AL ; column = cell_x / 8
		MOV	CH,DL ; row = cell_y / 8
		MOV	AX,BX ; buttons status
		CMP	ES:LftMous,0
		JZ	@@ClearKbd
		MOV	BH,BL ; swap L and R buttons
		SHR	BL,1
		SHL	BH,1
		AND	BX,201h
		AND	AL,4
		OR	AL,BL
		OR	AL,BH
  @@ClearKbd:	AND	AX,7 ; only L,R,M buttons
		JZ	@@Exit
		PUSHF
		CALL	ClrKbd ; discard all typed-ahead keys if the corresponding option is set
		POPF
  @@Exit:	RET
GetMous		ENDP

;---------------------------------------------------------------
;	Mouse Ctrl-O
;	Return:	 CY=1 - Ok
;		 CY=0 - Exit
;---------------------------------------------------------------
MouseCtrlO	PROC	FAR
		MOV	BL,0
  @@SetCursor:	MOV	AL,ES:Pointers[6]
  @@CmpCursor:	CMP	AL,BL
		JE	@@LeaveTime
		MOV	BL,AL
		CALL	TypMous
		JMP	@@GetMouse
  @@LeaveTime:	CALL	LeaveTm
  @@GetMouse:	CALL	GetMous
		JZ	@@ChkExit
		CMP	CX,2
		JB	@@SetCursor
		MOV	AL,0
		JMP	@@CmpCursor
  @@ChkExit:	OR	BL,BL
		JE	@@Exit
		MOV	AL,0
		CALL	TypMous
		STC
  @@Exit:	RET
MouseCtrlO	ENDP

;---------------------------------------------------------------
;	Reset mouse command and restore cursor
;---------------------------------------------------------------
ResMouseCmd	PROC	FAR
		USES	AX
		XOR	AX,AX
		CMP	ES:MouseCmd,AX
		JE	@@Exit
		MOV	ES:MouseCmd,AX
		CALL	TypMous
  @@Exit:	RET
ResMouseCmd	ENDP

;---------------------------------------------------------------
;	Mouse commend repeat
;		   SI - command
;	Return:	 CY=1 - do command
;		 CY=0 - wait
;---------------------------------------------------------------
MouseRepeat	PROC	FAR
		USES	AX,BX,CX,DX
		CALL	Timer
		MOV	AL,0
		CMP	SI,ES:MouseCmd
		JNE	@@DoCmd
		MOV	AX,CX
		MOV	BX,DX
		SUB	BX,ES:MouseTime.Lo
		SBB	AX,ES:MouseTime.Hi
		MOV	AL,1
		JNE	@@DoCmd
		MOV	DX,9
		CMP	ES:MouseRep,0
		JZ	@@CmpDelay
		MOV	DL,1
  @@CmpDelay:	CMP	BX,DX
		JAE	@@AddTime
  @@LeaveTime:	CALL	LeaveTm
		CLC
		JMP	@@Exit
  @@AddTime:	XOR	CX,CX
		ADD	DX,ES:MouseTime.Lo
		ADC	CX,ES:MouseTime.Hi
  @@DoCmd:	MOV	ES:MouseTime.Lo,DX
		MOV	ES:MouseTime.Hi,CX
		MOV	ES:MouseCmd,SI
		MOV	ES:MouseRep,AL
		STC
  @@Exit:	RET
MouseRepeat	ENDP

;---------------------------------------------------------------
;	Mouse double click
;		BH,BL - base of selected point
;		DH,DL - size of selected point
;		   SI - cursor
;	Return:	   CY - continue
;		   NZ - action
;---------------------------------------------------------------
MouseDblClick	PROC	FAR
		PUSH	DX
		CMP	ES:MouseEnter,0
		JNZ	@@Continue
		MOV	ES:MouseEnter,1
		MOV	AH,2Ch
		INT	21h
		CMP	ES:MouseTime.Hi,0FFFFh
		JE	@@AddTime
		CMP	SI,ES:MousePos
		JE	@@SecClick
  @@AddTime:	ADD	DL,MoDelay
		CMP	DL,100
		JB	@@SaveTime
		INC	DH
		CMP	DH,60
		JB	@@SaveTime
		INC	CX
		CMP	CL,60
		JB	@@SaveTime
		INC	CH
  @@SaveTime:	MOV	ES:MouseTime.Lo,DX
		MOV	ES:MouseTime.Hi,CX
		MOV	ES:MousePos,SI
  @@Continue:	POP	DX
		STC
		RET
  @@SecClick:	CMP	CX,ES:MouseTime.Hi
		JA	@@AddTime
		JB	@@InTime
		CMP	DX,ES:MouseTime.Lo
		JA	@@AddTime
  @@InTime:	POP	DX
		XOR	CX,CX
  @@WaitMouse:	PUSH	CX
		CALL	GetMous
		JZ	@@Exit
		CALL	InWind
		POP	CX
		MOV	CL,AL
		MOV	AL,0
		JC	@@CmpCur
		MOV	AL,ES:Pointers[4]
  @@CmpCur:	CMP	AL,CH
		JE	@@LeaveTime
		MOV	CH,AL
		CALL	TypMous
		JMP	@@WaitMouse
  @@LeaveTime:	CALL	LeaveTm
		JMP	@@WaitMouse
  @@Exit:	MOV	AL,0
		CALL	TypMous
		POP	AX
		OR	AH,AH
		RET
MouseDblClick	ENDP

;---------------------------------------------------------------
;	Check cursor in window
;		CH,CL - cursor
;		BH,BL - begin of window
;		DH,DL - size of window
;	Return:	CY=0  - cursor inside the window
;---------------------------------------------------------------
InWind		PROC	FAR
		USES	CX,DX
		SUB	CH,BH
		JB	@@Exit
		SUB	CL,BL
		JB	@@Exit
		DEC	DH
		CMP	DH,CH
		JB	@@Exit
		DEC	DL
		CMP	DL,CL
  @@Exit:	RET
InWind		ENDP
