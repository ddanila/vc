;--------------------- Subroutines ---------------------
;		Version :	17.09.1999
;-------------------------------------------------------

;----------------------- Timer -------------------------
Timer	PROC	NEAR
	PUSH	DS
	XOR	CX,CX
	MOV	DS,CX
	CLI
	MOV     DX,DS:[46Ch].Lo
	MOV     CX,DS:[46Ch].Hi
	STI
	POP	DS
	RET
Timer	ENDP

;----------------- Clear keyboard buffer ---------------
ClrKbd	PROC	NEAR
	PUSH	AX
	CMP	ES:ClrBuf,0
	JZ	ClrKb3
	JMP	ClrKb2
ClrKb1:	MOV	AH,0
	INT	16h
ClrKb2:	MOV	AH,1
	INT	16h
	JNZ	ClrKb1
ClrKb3:	POP	AX
	RET
ClrKbd	ENDP

;----------------------- Go to CASE --------------------
;			CS:SI - address of case table
;			   AX - key code
Case	PROC	NEAR
	PUSH	SI
	PUSH	CX
	OR	AH,AH
	JE	Case3
Case1:	MOV	CX,CS:[SI]
	ADD	SI,4
	JCXZ	Case3
	CMP	AL,CL
	JNE	Case1
	OR	CH,CH
	JE	Case2
	CMP	AH,CH
	JNE	Case1
Case2:	POP	CX
	ADD	SP,4
	JMP	WORD PTR CS:[SI-2]
Case3:	POP	CX
	POP	SI
	RET
Case	ENDP

;----------------------- Set cursor --------------------
;			AL,AH - x, y
Cursor	PROC	NEAR
	PUSH	DX
	PUSH	BX
	PUSH	AX
	MOV	DX,AX
	MOV	AH,0Fh
	INT	10h
	MOV	AH,2
	INT	10h
	POP	AX
	POP	BX
	POP	DX
	RET
Cursor	ENDP

;-------------- Get cursor size & set it ---------------
;	Return :	CX - cursor size
ChgCur	PROC	NEAR
	PUSH	BP
	PUSH	DX
	PUSH	BX
	MOV	BH,0
	MOV	AH,3
	INT	10h
	PUSH	CX
	MOV	CX,0607h
	CMP	ES:ScrMod,7
	JNE	ChCur1
	MOV	CX,0B0Ch
ChCur1:	CMP	ES:ColTyp,2
	JNE	ChCur2
	MOV	CH,0
ChCur2:	MOV	AH,1
	INT	10h
	POP	CX
	POP	BX
	POP	DX
	POP	BP
	RET
ChgCur	ENDP

;------------------- Make hidden cursor ----------------
HidCur	PROC	NEAR
	PUSH	AX
	MOV	AX,7F00h
	CALL	Cursor
	POP	AX
	RET
HidCur	ENDP

;--------------------- Beep sound ----------------------
Beep	PROC	NEAR
	CMP	ES:ErrSnd,0
	JE	Beep1
	PUSH	BP
	PUSH	AX
	MOV	AX,0E07h
	INT	10h
	POP	AX
	POP	BP
Beep1:	RET
Beep	ENDP

;------------------------ Snow -------------------------
SetSnow	PROC	NEAR
	MOV	AH,1
	CMP	DS:TopView,0
	JNZ	SetSn3
	CMP	DS:ScrMod,7
	JE	SetSn3
	CMP	DS:SnowMod,1
	JE	SetSn3
	MOV	DX,03DAh
	MOV	BX,7FFFh
	JB	SetSn1
	CMP	DS:TstEGA,0
	JNE	SetSn3
SetSn1:	STI
	DEC	BX
	JE	SetSn3
	MOV	CX,7			; Const2_1 ???
	CLI
SetSn2:	IN	AL,DX
	TEST	AL,1
	JZ	SetSn1
	LOOP	SetSn2
	MOV	AH,0
SetSn3:	STI
	MOV	DS:Snow,AH
	RET
SetSnow	ENDP

;--------------- Set type of mouse sursor --------------
;			   AL - cursor
TypMous	PROC	NEAR
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	MOV	CX,0FFFFh
	MOV	DX,7700h
	CMP	AL,0
	JE	TypMo1
	MOV	CL,0
	MOV	DL,AL
TypMo1:	XOR	BX,BX
	MOV	AX,0Ah
	INT	33h
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	RET
TypMous	ENDP

;------------------- Set mouse cursor ------------------
;			   AL - set/reset
CurMous	PROC	NEAR
	CMP	ES:Mouse,0
	JE	CurMo1
	PUSH	AX
	CMP	AL,1
	MOV	AX,1
	ADC	AL,0
	INT	33h
	POP	AX
CurMo1:	RET
CurMous	ENDP

;------------------- Get mouse cursor ------------------
;	Return :	CH,CL - mouse cursor
;			   AX - mouse buttons
GetMous	PROC	NEAR
	PUSH	DX
	PUSH	BX
	XOR	AX,AX
	CMP	ES:Mouse,0
	STC
	JZ	GetMo1
	MOV	AX,3
	INT	33h
	MOV	AX,CX
	MOV	CL,3
	SHR	AX,CL
	SHR	DX,CL
	MOV	CL,AL
	MOV	CH,DL
	MOV	AX,BX
	CMP	ES:LftMous,0
	JE	GetMo1
	MOV	BH,BL
	SHR	BL,1
	SHL	BH,1
	AND	BX,201h
	AND	AL,4
	OR	AL,BL
	OR	AL,BH
GetMo1:	AND	AX,7
	POP	BX
	POP	DX
	RET
GetMous	ENDP

;----------------- Mouse double click ------------------
;			BH,BL - base of selected point
;			DH,DL - size of selected point
;			   SI - cursor
;	Return :	   CY - continue
;			   NZ - action
DoublMo	PROC	NEAR
	PUSH	DX
	CMP	ES:MousEnt,0
	JNE	DblMo3
	MOV	ES:MousEnt,1
	MOV	AH,2Ch
	INT	21h
	CMP	ES:MousTim.Hi,0FFFFh
	JE	DblMo1
	CMP	SI,ES:MousPos
	JE	DblMo4
DblMo1:	ADD	DL,MoDelay
	CMP	DL,100
	JB	DblMo2
	INC	DH
	CMP	DH,60
	JB	DblMo2
	INC	CL
	CMP	CL,60
	JB	DblMo2
	INC	CH
DblMo2:	MOV	ES:MousTim.Lo,DX
	MOV	ES:MousTim.Hi,CX
	MOV	ES:MousPos,SI
DblMo3:	POP	DX
	STC
	RET
DblMo4:	CMP	CX,ES:MousTim.Hi
	JA	DblMo1
	JB	DblMo5
	CMP	DX,ES:MousTim.Lo
	JA	DblMo1
DblMo5:	POP	DX
	MOV	CH,0
DblMo6:	PUSH	CX
	CALL	GetMous
	CALL	InWind
	POP	CX
	PUSH	AX
	MOV	AL,0
	JC	DblMo7
	MOV	AL,'û'
DblMo7:	CMP	AL,CH
	JE	DblMo8
	MOV	CH,AL
	CALL	TypMous
DblMo8:	POP	AX
	TEST	AL,7
	JNZ	DblMo6
	OR	CH,CH
	RET
DoublMo	ENDP

;--------------- Check cursor in window ----------------
;			CH,CL - cursor
;			BH,BL - begin of window
;			DH,DL - size of window
;	Return :	CY=0  - cursor inside the window
InWind	PROC	NEAR
	PUSH	DX
	PUSH	CX
	SUB	CH,BH
	JB	InWin1
	SUB	CL,BL
	JB	InWin1
	DEC	DH
	CMP	DH,CH
	JB	InWin1
	DEC	DL
	CMP	DL,CL
InWin1:	POP	CX
	POP	DX
	RET
InWind	ENDP

;------------------ INT 10h with mouse -----------------
Int10m	PROC	NEAR
	PUSH	BP
	PUSH	AX
	MOV	AL,0
	CALL	CurMous
	POP	AX
	INT	10h
	PUSH	AX
	MOV	AL,1
	CALL	CurMous
	POP	AX
	POP	BP
	RET
Int10m	ENDP

;------------- Convert coordinars to address -----------
;			BH,BL - coordinats
;	Return :	   DI - address
BasAdr	PROC	NEAR
	PUSH	BX
	PUSH	AX
	MOV	AL,80
	MUL	BH
	MOV	BH,0
	ADD	AX,BX
	ADD	AX,AX
	ADD	AX,OFFSET ScrBuf
	MOV	DI,AX
	POP	AX
	POP	BX
	RET
BasAdr	ENDP

;--------------- Shift windows in EGA lines ------------
SftEGA	PROC	NEAR
	PUSH	AX
	MOV	AL,ES:Lines
	SUB	AL,25
	JBE	SfEGA1
	SHR	AL,1
	ADD	BH,AL
SfEGA1:	POP	AX
	RET
SftEGA	ENDP

;---------------------- Save window --------------------
;			BH,BL - begin
;			DH,DL - size
;			   SI - address of buffer
SavWin	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	CLD
	CALL	BasAdr
	XCHG	SI,DI
	MOV	CL,DH
	MOV	CH,0
	MOV	DH,0
	PUSH	ES
	POP	DS
SavWn1:	PUSH	CX
	PUSH	SI
	MOV	CX,DX
	REP	MOVSW
	POP	SI
	POP	CX
	ADD	SI,2*80
	LOOP	SavWn1
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	DS
	RET
SavWin	ENDP

;--------------------- Restore window ------------------
;			BH,BL - begin
;			DH,DL - size
;			   SI - address of buffer
ResWin	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	CLD
	CALL	BasAdr
	MOV	CL,DH
	MOV	CH,0
	MOV	DH,0
	PUSH	ES
	POP	DS
ResWn1:	PUSH	CX
	PUSH	DI
	MOV	CX,DX
	REP	MOVSW
	POP	DI
	POP	CX
	ADD	DI,2*80
	LOOP	ResWn1
	MOV	DS:Quote,0
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	DS
	RET
ResWin	ENDP

;--------------------- Output window -------------------
;			BH,BL - begin
;			DH,DL - size
Window	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	PUSH	ES
	MOV	AL,0
	CALL	CurMous
	PUSH	ES
	POP	DS
	MOV	ES,DS:Screen.Segm
	CALL	BasAdr
	MOV	SI,DI
	SUB	DI,OFFSET ScrBuf
	ADD	DI,DS:Screen.Offs
	MOV	AL,DS:Snow
	CMP	DH,1
	JE	Wind1
	MOV	BH,0
	ADD	BX,BX
	SUB	SI,BX
	SUB	DI,BX
	PUSH	DI
	PUSH	DX
	CALL	MovScr
	POP	AX
	MOV	AL,80
	MUL	AH
	MOV	CX,AX
	POP	DI
	JMP	Wind6
Wind1:	MOV	CL,DL
	MOV	CH,0
	PUSH	DI
	PUSH	CX
	MOV	AH,AL
	MOV	DX,03DAh
Wind2:	OR	AH,AH
	JNZ	Wind5
Wind3:	IN	AL,DX
	TEST	AL,1
	JNZ	Wind3
	CLI
Wind4:	IN	AL,DX
	TEST	AL,1
	JZ	Wind4
Wind5:	MOVSW
	STI
	LOOP	Wind2
	POP	CX
	POP	DI
Wind6:	POP	ES
	CMP	ES:TopView,0
	JZ	Wind7
	MOV	AH,0FFh
	INT	10h
Wind7:	MOV	AL,1
	CALL	CurMous
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	DS
	RET
Window	ENDP

;------------------------ Save screen ------------------
SavScr	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	MOV	DS,ES:Screen.Segm
	MOV	SI,ES:Screen.Offs
	MOV	DI,OFFSET ScrBuf
	MOV	DH,ES:Lines
	MOV	AL,ES:Snow
	CALL	MovScr
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	DS
	RET
SavScr	ENDP

;-------------------- Move screen bytes ----------------
;			   DH - number of lines
;			   AL - snow
;			DS:SI => ES:DI
MovScr	PROC	NEAR
	CLD
	MOV	BL,AL
	MOV	DL,7			; Const1 ???
MovSc1:	CMP	DH,DL
	JAE	MovSc2
	MOV	DL,DH
MovSc2:	MOV	AX,80
	MUL	DL
	PUSH	AX
	OR	BL,BL
	JNZ	MovSc6
	PUSH	DX
	MOV	DX,03DAh
MovSc3:	IN	AL,DX
	TEST	AL,1
	JNZ	MovSc3
MovSc4:	STI
	MOV	CX,10			; Const2 ???
	CLI
MovSc5:	IN	AL,DX
	TEST	AL,1
	JZ	MovSc4
	LOOP	MovSc5
	POP	DX
MovSc6:	POP	CX
	REP	MOVSW
	STI
	SUB	DH,DL
	JNE	MovSc1
	RET
MovScr	ENDP

;---------------------- Zooming box --------------------
;			BH,BL - begin
;			DH,DL - size
;			   AH - color
Diskret	EQU	5
MinBox	PROC	NEAR
	PUSH	DI
	PUSH	CX
	PUSH	AX
	MOV	AL,0
	MOV	ES:Quote,AL
	CMP	AL,ES:ZoomWin
	JZ	MinBx2
	PUSH	AX
	MOV	AH,ShadFl
	CALL	ColAddr
	POP	AX
	XCHG	AL,ES:[DI]
	PUSH	DI
	PUSH	AX
	XOR	DI,DI
	CALL	MinBx3
	MOV	CX,Diskret-1
MinBx1:	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	MOV	AL,DL
	MOV	AH,BL
	MOV	CH,11
	CALL	MinBx5
	MOV	DL,AL
	MOV	BL,AH
	MOV	AL,DH
	MOV	AH,BH
	MOV	CH,5
	CALL	MinBx5
	MOV	DH,AL
	MOV	BH,AH
	POP	AX
	CALL	ShadBox
	CALL	MinBx3
	CALL	Window
	POP	BX
	POP	CX
	POP	DX
	LOOP	MinBx1
	CALL	MinBx3
	POP	AX
	POP	DI
	MOV	ES:[DI],AL
MinBx2:	CALL	ShadBox
	POP	AX
	POP	CX
	POP	DI
	RET
MinBox	ENDP

MinBx3	PROC	NEAR
	PUSH	DX
	PUSH	CX
	PUSH	AX
MinBx4:	STI
	CALL	Timer
	CMP	DX,DI
	JE	MinBx4
	MOV	DI,DX
	POP	AX
	POP	CX
	POP	DX
	RET
MinBx3	ENDP

MinBx5	PROC	NEAR
	PUSH	AX
	SUB	AL,CH
	SHR	AL,1
	MUL	CL
	MOV	CH,Diskret-1
	DIV	CH
	MOV	CH,AL
	POP	AX
	ADD	AH,CH
	SUB	AL,CH
	SUB	AL,CH
	RET
MinBx5	ENDP

;-------------------- Box with shadow ------------------
;			BH,BL - begin
;			DH,DL - size
;			   AH - color
ShadBox	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	CALL	BasAdr
	MOV	CL,DH
	MOV	CH,0
	MOV	SI,CX
	SUB	DL,10
	MOV	DH,0
	MOV	CL,3
	MOV	BX,'  '
	MOV	AL,' '
	CALL	BoxLine
	MOV	BX,'É»'
	MOV	AL,'Í'
	CALL	ShadLin
	SUB	SI,5
	JBE	ShdBx2
	MOV	BX,'ºº'
	MOV	AL,' '
ShdBx1:	CALL	ShadLin
	DEC	SI
	JNE	ShdBx1
ShdBx2:	MOV	BX,'È¼'
	MOV	AL,'Í'
	CALL	ShadLin
	MOV	BX,'  '
	MOV	AL,' '
	CALL	ShadLin
	ADD	DI,2*2
	MOV	CX,DX
	ADD	CL,8
ShdBx3:	CALL	Shadow
	LOOP	ShdBx3
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	RET
ShadBox	ENDP

;-------------------- One line of box ------------------
;			ES:DI - screen buffer
;			   AH - color
;		³   ÇÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¶   ³
;		 ÀÂÙ|ÀÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÙ|ÀÂÙ
;		 CX BH         DX(AL)      BL CX
ShadLin	PROC	NEAR
	PUSH	DI
	CALL	BoxLn0
	CALL	Shadow
	CALL	Shadow
	POP	DI
	ADD	DI,2*80
	RET
ShadLin	ENDP

BoxLine	PROC	NEAR
	PUSH	DI
	CALL	BoxLn0
	POP	DI
	ADD	DI,2*80
	RET
BoxLine	ENDP

BoxLn0	PROC	NEAR
	PUSH	CX
	PUSH	AX
	PUSH	CX
	PUSH	AX
	JCXZ	BoxLn1
	MOV	AL,' '
	REP	STOSW
BoxLn1:	MOV	AL,BH
	STOSW
	POP	AX
	MOV	CX,DX
	REP	STOSW
	MOV	AL,BL
	STOSW
	POP	CX
	JCXZ	BoxLn2
	MOV	AL,' '
	REP	STOSW
BoxLn2:	POP	AX
	POP	CX
	RET
BoxLn0	ENDP

;---------------------- Put window ---------------------
;			BH,BL - begin-center
;			CS:DX - address of include line
;			CS:BP - pointer:
;				DB  NormCol,RevCol,BoldCol,UndrCol
;				DB  NumLns,NumVars
;				DW  SaveBuf
;				DW  Variable
;				DB  VarMode,VarLimit
;				. . .
;				DW  OFFSET Line1
;				. . .
;	Return:		BH,BL - begin
;			DH,DL - size
;			   SI - address of save buffer
DialBox	PROC	NEAR
	PUSH	DS
	PUSH	DI
	PUSH	CX
	PUSH	AX
	CALL	HidCur
	CALL	SftEGA
	PUSH	BP			; Convert variables
	MOV	AL,CS:[BP+5]
	AND	AX,3
	JE	DiBx01
	ADD	BP,8
	MOV	DI,OFFSET TmpErr+1
	PUSH	AX
	CALL	DiBx30
	POP	AX
	DEC	AX
	JE	DiBx01
	ADD	BP,4
	MOV	DI,OFFSET TmpErr+101
	CALL	DiBx30
DiBx01:	POP	BP
	PUSH	CS			; Calculate size of window
	POP	DS
	MOV	CL,DS:[BP+4]
	MOV	CH,0
	PUSH	BX
	LEA	BX,[BP+8]
	MOV	AL,DS:[BP+5]
	AND	AL,3
	MOV	AH,4
	MUL	AH
	ADD	BX,AX
	PUSH	BX
	XOR	DI,DI
DiBx10:	MOV	SI,DS:[BX]
	INC	BX
	INC	BX
	OR	SI,SI
	JE	DiBx11
	CALL	LnStr
	CMP	AX,DI
	JBE	DiBx11
	MOV	DI,AX
DiBx11:	LOOP	DiBx10
	XCHG	DX,DI
	MOV	DH,DS:[BP+4]
	ADD	DX,40Ch
	POP	CX
	POP	BX
	PUSH	BX			; Put box
	MOV	AL,DL
	DEC	AX
	SHR	AL,1
	SUB	BL,AL
	MOV	SI,DS:[BP+6]
	CALL	SavWin
	MOV	AH,DS:[BP]
	CALL	Color
	CALL	MinBox
	POP	AX
	PUSH	SI			; Output lines
	PUSH	BX
	MOV	BX,AX
	MOV	SI,CX
	MOV	CL,DS:[BP+4]
	MOV	CH,0
DiBx20:	INC	BH
	PUSH	SI
	PUSH	DI
	PUSH	DX
	MOV	SI,DS:[SI]
	OR	SI,SI
	JE	DiBx21
	MOV	DX,DI
	CALL	CntLine
	JMP	DiBx22
DiBx21:	CALL	BasAdr
	PUSH	CX
	PUSH	BX
	MOV	DH,0
	MOV	AX,DX
	DEC	AX
	AND	AX,0FFFEh
	SUB	DI,AX
	SUB	DL,10
	MOV	CX,3
	MOV	BX,'Ç¶'
	MOV	AL,'Ä'
	MOV	AH,DS:[BP]
	CALL	Color
	CALL	BoxLine
	POP	BX
	POP	CX
DiBx22:	POP	DX
	POP	DI
	POP	SI
	INC	SI
	INC	SI
	LOOP	DiBx20
	POP	BX			; Exit
	POP	SI
	POP	AX
	POP	CX
	POP	DI
	POP	DS
	RET
DialBox	ENDP

DiBx30	PROC	NEAR			; Convert variable
	MOV	SI,CS:[BP]
	MOV	CX,LenPath+13
	PUSH	CX
	PUSH	DI
	REP	MOVSB
	POP	SI
	POP	CX
	MOV	AL,CS:[BP+2]
	TEST	AL,4
	JZ	DiBx31
	CALL	NormStr
DiBx31:	TEST	AL,2
	JZ	DiBx32
	CALL	PathLin
	TEST	AL,1
	JZ	DiBx32
	CALL	LetLin
DiBx32:	MOV	AL,CS:[BP+3]
	CBW
	CALL	ShrtLin
	RET
DiBx30	ENDP

;--------------- Calculate string length ---------------
;			DS:SI - address of line
;			CS:DX - address of include line
;	Return :	   AX - length
LnStr	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	BX
	XOR	BX,BX
LnStr0:	LODSW
	CMP	AL,0
	JE	LnStr9
	CMP	AL,'^'
	JE	LnStr3
	CMP	AL,0FFh
	JE	LnStr2
	DEC	SI
LnStr1:	INC	BX
	JMP	LnStr0
LnStr2:	MOV	AL,AH
	AND	AL,7Fh
	DEC	AX
	SHL	AH,1
	CMC
	ADC	BL,AL
	JMP	LnStr0
LnStr3:	MOV	AL,AH
	CMP	AL,'^'
	JE	LnStr1
	CALL	UpCase
	CMP	AL,'V'
	JE	LnStr5
	CMP	AL,'W'
	JE	LnStr6
	CMP	AL,'I'
	JE	LnStr7
	CMP	AL,'A'
	JE	LnStr4
	CMP	AL,'C'
	JNE	LnStr0
	INC	BX
LnStr4:	INC	SI
	INC	SI
	JMP	LnStr0
LnStr5:	PUSH	ES
	MOV	DI,OFFSET TmpErr+1
	JMP	LnStr8
LnStr6:	PUSH	ES
	MOV	DI,OFFSET TmpErr+101
	JMP	LnStr8
LnStr7:	PUSH	ES
	PUSH	CS
	POP	ES
	MOV	DI,DX
LnStr8:	PUSH	DI
	PUSH	CX
	MOV	CX,80
	MOV	AL,0
	REPNE	SCASB
	MOV	AX,DI
	POP	CX
	POP	DI
	POP	ES
	SUB	AX,DI
	DEC	AX
	ADD	BL,AL
	JMP	LnStr0
LnStr9:	MOV	AX,BX
	POP	BX
	POP	DI
	POP	SI
	RET
LnStr	ENDP

;-------------------- Output NG line -------------------
;			DS:SI - address of line
;			ES:DI - address of screen buffer
;			CS:DX - address of include line
;			CS:BP - address of color table
;			   AL - limit
NgLine	PROC	NEAR
	PUSH	CX
	PUSH	BX
	PUSH	AX
	MOV	BL,AL
	MOV	AH,CS:[BP]
	MOV	BH,0
NgLn01:	CALL	Color
NgLn02:	LODSB
	CMP	AL,0
	JE	NgLn04
	CMP	AL,'^'
	JE	NgLn07
	CMP	AL,0FFh
	JE	NgLn05
NgLn03:	CALL	NgLn30
	JMP	NgLn02
NgLn04:	POP	AX
	POP	BX
	POP	CX
	RET
NgLn05:	LODSB
	MOV	CL,AL
	TEST	AL,80h
	MOV	AL,' '
	JZ	NgLn50
	LODSB
NgLn50:	AND	CX,7Fh
	JE	NgLn02
NgLn06:	CALL	NgLn30
	LOOP	NgLn06
	JMP	NgLn02
NgLn07:	LODSB
	CMP	AL,'^'
	JE	NgLn03
	CALL	UpCase
	CMP	AL,'V'
	JNE	NgLn08
	PUSH	SI
	MOV	SI,OFFSET TmpErr+1
	JMP	NgLn09
NgLn08:	CMP	AL,'W'
	JNE	NgLn13
	PUSH	SI
	MOV	SI,OFFSET TmpErr+101
NgLn09:	PUSH	DS
	PUSH	ES
NgLn10:	POP	DS
NgLn11:	LODSB
	CMP	AL,0
	JE	NgLn12
	CALL	NgLn30
	JMP	NgLn11
NgLn12:	POP	DS
	POP	SI
	JMP	NgLn02
NgLn13:	CMP	AL,'I'
	JNE	NgLn14
	PUSH	SI
	MOV	SI,DX
	PUSH	DS
	PUSH	CS
	JMP	NgLn10
NgLn14:	MOV	CL,0
	CMP	AL,'N'
	JE	NgLn15
	MOV	CL,1
	CMP	AL,'R'
	JE	NgLn15
	MOV	CL,2
	CMP	AL,'B'
	JE	NgLn15
	MOV	CL,3
	CMP	AL,'U'
	JNE	NgLn17
NgLn15:	CMP	CL,BH
	JNE	NgLn16
	MOV	CL,0
NgLn16:	MOV	CH,0
	PUSH	BP
	ADD	BP,CX
	MOV	AH,CS:[BP]
	POP	BP
	MOV	BH,CL
	JMP	NgLn01
NgLn17:	CMP	AL,'A'
	JE	NgLn18
	CMP	AL,'C'
	JNE	NgLn20
NgLn18:	MOV	CL,AL
	LODSB
	CALL	TstHex
	MOV	CH,AL
	LODSB
	JC	NgLn20
	CALL	TstHex
	JC	NgLn20
	PUSH	CX
	MOV	CL,4
	SHL	CH,CL
	OR	AL,CH
	POP	CX
	CMP	CL,'C'
	JE	NgLn19
	JMP	NgLn03
NgLn19:	MOV	AH,AL
	MOV	BH,0
NgLn20:	JMP	NgLn02
NgLine	ENDP

NgLn30	PROC	NEAR
	OR	BL,BL
	JE	NgLn31
	STOSW
	DEC	BL
NgLn31:	RET
NgLn30	ENDP

;--------------------- Center line ---------------------
;			BH,BL - center
;			DS:SI - address of line
;			CS:DX - address of include line
;			CS:BP - address of color table
CntLine	PROC	NEAR
	PUSH	DI
	PUSH	AX
	CALL	LnStr
	CALL	BasAdr
	PUSH	AX
	INC	AX
	AND	AX,0FFFEh
	SUB	DI,AX
	POP	AX
	CALL	NgLine
	POP	AX
	POP	DI
	RET
CntLine	ENDP

;------------------------ Dialog -----------------------
;			CS:DX - address of include line
;			CS:BP - pointer:
;				DW  BoxPoint, BegCenter, SecondBox
;				DB  KeyBar, Menu, HlpPage, Status
;					DB  Type
;					DB  Up, Down, Left, Right
;					DW  Begin, Point, Control
;					...
;				DB  0
;				Status:	b0 - Beep
;					b1 - ClrKbd
;					b2 - SftEGA
;					b3 - F10-Tree
;					b4 - disable DialBox
	.DATA
Dial_T	DW	0009h, Dial38
	DW	0F00h, Dial55
	DW	4800h, Dial60
	DW	5000h, Dial61
	DW	4B00h, Dial62
	DW	4D00h, Dial63
	DW	4700h, Dial64
	DW	4F00h, Dial68
	DW	4900h, Dial64
	DW	5100h, Dial69
	DW	000Dh, Dial70
	DW	001Bh, Dial81
	DW	4400h, Dial78
	DW	7100h, Dial80
	DW	0

	.CODE
Dialog	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	MOV	ES:Quote,0
	CALL	ChgCur
	PUSH	CX
	PUSH	BP			; Output box
	MOV	BX,CS:[BP].BoxBase
	MOV	AL,CS:[BP].bKeyBar
	CALL	PutKey
	TEST	CS:[BP].bFlags,10h
	JNZ	Dial04
	MOV	BP,CS:[BP].pDiBox
	CALL	DialBox
Dial04:	POP	BP
	PUSH	SI			; Init
	PUSH	DX
	LEA	SI,[BP]+SIZE BoxHd
Dial11:	PUSH	SI
	PUSH	BX
	MOV	AH,BL
	ADD	BX,CS:[SI].mBase
	CALL	BasAdr
	MOV	AL,CS:[SI].mType
	CMP	AL,1			; InpStr
	JNE	Dial15
	MOV	CX,CS:[SI].mCtrl
	MOV	SI,CS:[SI].mPoint
	MOV	DI,OFFSET TmpErr
	MOV	AL,64
	MUL	CH
	ADD	DI,AX
	MOV	CH,0
	XOR	AX,AX
	STOSW
	STOSB
	PUSH	DI
	PUSH	CX
	REP	MOVSB
	STOSB
	POP	CX
	POP	SI
	MOV	DI,CS:[BP].pDiBox
	MOV	AH,CS:[DI+1]
	CALL	InSt10
	JMP	Dial19
Dial15:	CMP	AL,2			; Select
	JNE	Dial16
	MOV	BX,CS:[SI].mPoint
	MOV	AL,ES:[BX]
	MOV	AH,2*80
	MUL	AH
	ADD	DI,AX
	MOV	BYTE PTR ES:[DI],''
	JMP	Dial19
Dial16:	CMP	AL,3			; FlipFlop
	JNE	Dial17
	MOV	BX,CS:[SI].mPoint
	MOV	SI,CS:[SI].mCtrl
	LODSB
	MOV	ES:[BX],AL
	CMP	AL,0
	JZ	Dial19
	MOV	BYTE PTR ES:[DI],'x'
	JMP	Dial19
Dial17:	CMP	AL,4			; MinMnu
	JNE	Dial20
	SUB	BL,AH
	CMP	AH,40
	JB	Dial18
	ADD	BL,40
Dial18:	CALL	BasAdr
	MOV	BX,CS:[SI].mPoint
	MOV	CL,0
	MOV	ES:[BX],CL
	MOV	SI,CS:[SI].mCtrl
	MOV	BX,CS:[BP].pDiBox
	MOV	AH,CS:[BX+2]
	CALL	MnMn41
Dial19:	POP	BX
	POP	SI
	ADD	SI,SIZE BoxMn
	JMP	Dial11
Dial20:	POP	BX			; Output window
	POP	SI
	CALL	Window
	MOV	AL,CS:[BP].bHelp
	CMP	AL,0
	JE	Dial21
	MOV	ES:HlpPage,AL
Dial21:	MOV	AL,CS:[BP].bFlags
	TEST	AL,1
	JZ	Dial22
	CALL	ClrKbd
Dial22:	TEST	AL,2
	JZ	Dial30
	CALL	Beep
Dial30:	LEA	SI,[BP]+SIZE BoxHd	; Menus
Dial31:	PUSH	BP
	PUSH	SI
	PUSH	BX
	MOV	AH,BL
	ADD	BX,CS:[SI].mBase
	MOV	DI,BP
	MOV	BP,CS:[BP].pDiBox
	MOV	AL,CS:[SI].mType
	CMP	AL,1			; InpStr
	JNE	Dial33
	MOV	CX,CS:[SI].mCtrl
	MOV	SI,OFFSET TmpErr+3
	MOV	AL,64
	MUL	CH
	ADD	SI,AX
	MOV	AL,CS:[DI].bKeyBar
	MOV	CH,0
	MOV	AH,CS:[BP+1]
	MOV	DI,ES:[SI-3]
	CALL	InpStr
	MOV	ES:[SI-3],DI
	JMP	Dial37
Dial33:	CMP	AL,2			; Select
	JNE	Dial34
	MOV	AL,CS:[DI].bKeyBar
	MOV	AH,CS:[SI].mSelTot
	MOV	DI,CS:[SI].mPoint
	MOV	CX,ES:[DI]
	CALL	Select
	MOV	ES:[DI],CX
	JMP	Dial37
Dial34:	CMP	AL,3			; FlipFlop
	JNE	Dial35
	MOV	AL,CS:[DI].bKeyBar
	MOV	DI,CS:[SI].mPoint
	MOV	CL,ES:[DI]
	CALL	FlipFl
	MOV	ES:[DI],CL
	JMP	Dial37
Dial35:	CMP	AL,4			; MinMnu
	JNE	Dial37
	SUB	BL,AH
	CMP	AH,40
	JB	Dial36
	ADD	BL,40
Dial36:	MOV	AL,CS:[DI].bKeyBar
	OR	AL,80h
	MOV	DI,CS:[SI].mPoint
	MOV	CL,ES:[DI]
	MOV	SI,CS:[SI].mCtrl
	CALL	MnMn60
	CALL	MinMnu
	MOV	ES:[DI],CL
Dial37:	POP	BX			; Exit
	POP	DI
	POP	BP
	CMP	AH,0FCh
	JE	Dial40
	MOV	SI,OFFSET Dial_T
	CALL	Case
	JMP	Dial39
Dial38:	ADD	DI,SIZE BoxMn		; Tab
	CMP	CS:[DI].mType,0
	JNE	Dial39
	LEA	DI,[BP]+SIZE BoxHd
Dial39:	MOV	SI,DI
	JMP	Dial31
Dial55:	SUB	DI,SIZE BoxMn		; Shift-Tab
	LEA	SI,[BP]+SIZE BoxHd
	CMP	DI,SI
	JAE	Dial39
Dial56:	ADD	SI,SIZE BoxMn
	CMP	CS:[SI].mType,0
	JNE	Dial56
	SUB	SI,SIZE BoxMn
	JMP	Dial31
Dial40:	POP	DX			; Mouse
	PUSH	DX
	XOR	SI,SI
Dial41:	CALL	GetMous
	JNZ	Dial42
	JMP	Dial52
Dial42:	PUSH	SI
	MOV	AH,0
	CMP	AL,1
	JNE	Dial45
	PUSH	DX
	SUB	DX,102h
	CALL	InWind
	POP	DX
	MOV	AH,0FFh
	JC	Dial46
	LEA	SI,[BP]+SIZE BoxHd
Dial43:	MOV	AL,CS:[SI].mType
	CBW
	CMP	AL,0
	JE	Dial46
	PUSH	DX
	PUSH	BX
	MOV	AH,BL
	ADD	BX,CS:[SI].mBase
	MOV	DX,CS:[SI].mCtrl
	CMP	AL,1
	JE	Dial44
	CMP	AL,3
	JA	Dial47
	MOV	DH,DL
	CALL	Slct20
	CMP	AL,2
	JE	Dial48
Dial44:	MOV	DH,1
	JMP	Dial48
Dial45:	MOV	AH,'û'
	CMP	AL,2
	JE	Dial46
	MOV	AH,'x'
Dial46:	POP	SI
	MOV	AL,AH
	MOV	AH,0
	CMP	AX,SI
	JE	Dial41
	MOV	SI,AX
	CMP	AL,0FFh
	CMC
	ADC	AL,0
	CALL	TypMous
	JMP	Dial41
Dial47:	SUB	BL,AH
	PUSH	SI
	MOV	SI,DX
	CALL	MnMn60
	POP	SI
	CMP	AH,40
	JB	Dial48
	ADD	BL,40
Dial48:	CALL	InWind
	POP	BX
	POP	DX
	JNC	Dial49
	ADD	SI,SIZE BoxMn
	JMP	Dial43
Dial49:	CMP	CS:[DI].mType,4
	JNE	Dial51
	PUSH	SI
	PUSH	BX
	MOV	AL,BL
	ADD	BX,CS:[DI].mBase
	SUB	BL,AL
	CMP	AL,40
	JB	Dial50
	ADD	BL,40
Dial50:	MOV	SI,CS:[DI].mPoint
	MOV	CL,ES:[SI]
	MOV	SI,CS:[DI].mCtrl
	MOV	DI,SI
	CALL	MnMn60
	MOV	SI,CS:[BP].pDiBox
	MOV	AH,CS:[SI+0]
	MOV	AL,CS:[SI+2]
	CALL	MnMn40
	POP	BX
	POP	SI
Dial51:	POP	DI
	JMP	Dial31
Dial52:	MOV	AL,0
	CALL	TypMous
	CMP	SI,'û'
	JE	Dial70
	CMP	SI,'x'
	JE	Dial53
	CMP	SI,0FFh
	JE	Dial53
	MOV	SI,DI
	JMP	Dial31
Dial53:	JMP	Dial81
Dial60:	MOV	AL,CS:[DI].mUp		; Up
	JMP	Dial65
Dial61:	MOV	AL,CS:[DI].mDown	; Down
	JMP	Dial65
Dial62:	MOV	AL,CS:[DI].mLeft	; Left
	JMP	Dial65
Dial63:	MOV	AL,CS:[DI].mRight	; Right
	JMP	Dial65
Dial64:	MOV	AL,80h			; Home, PgUp
Dial65:	PUSH	AX
	AND	AL,3Fh
	MOV	AH,SIZE BoxMn
	MUL	AH
	LEA	SI,[BP]+SIZE BoxHd
	ADD	SI,AX
	POP	AX
	CMP	CS:[SI].mType,2
	JNE	Dial67
	TEST	AL,80h
	JZ	Dial67
	TEST	AL,40h
	MOV	AL,0
	JZ	Dial66
	MOV	AL,CS:[SI].mSelTot
	DEC	AX
Dial66:	MOV	DI,CS:[SI].mPoint
	MOV	ES:[DI+1],AL
Dial67:	JMP	Dial31
Dial68:	CALL	Dial85			; End
	CMP	CS:[DI].mType,4
	JNE	Dial69
	MOV	SI,CS:[DI].mPoint
	MOV	BYTE PTR ES:[SI],0
Dial69:	CALL	Dial85			; PgDn
	MOV	SI,DI
	JMP	Dial31
Dial70:	XOR	AX,AX			; Enter
	CMP	CS:[DI].mType,4
	JNE	Dial71
	MOV	SI,CS:[DI].mPoint
	MOV	AL,ES:[SI]
Dial71:	CMP	CS:[BP].SubFunc,0
	JE	Dial72
	PUSH	BP
	PUSH	DI
	PUSH	BX
	CALL	CS:[BP].SubFunc
	POP	BX
	POP	SI
	POP	BP
	JNC	Dial72
	JMP	Dial31
Dial72:	CMP	AL,0
	JNE	Dial82
	LEA	SI,[BP]+SIZE BoxHd
Dial73:	MOV	AL,CS:[SI].mType
	CMP	AL,0
	JE	Dial82
	PUSH	SI
	CMP	AL,1
	JNE	Dial76
	MOV	DI,CS:[SI].mPoint
	MOV	CX,CS:[SI].mCtrl
	MOV	SI,OFFSET TmpErr+3
	MOV	AL,64
	MUL	CH
	ADD	SI,AX
	MOV	CH,0
	PUSH	DS
	PUSH	ES
	POP	DS
	POP	ES
	REP	MOVSB
	PUSH	DS
	PUSH	ES
	POP	DS
	POP	ES
	MOV	BYTE PTR DS:[DI],0
	JMP	Dial77
Dial76:	CMP	AL,3
	JNE	Dial77
	MOV	DI,CS:[SI].mPoint
	MOV	SI,CS:[SI].mCtrl
	MOV	AL,ES:[DI]
	MOV	DS:[SI],AL
Dial77:	POP	SI
	ADD	SI,SIZE BoxMn
	JMP	Dial73
Dial78: TEST	CS:[BP].bFlags,8	; F10
	JZ	Dial81
	MOV	AH,0
Dial79:	MOV	AL,1
	JMP	Dial71
Dial80:	TEST	CS:[BP].bFlags,8	; Alt-F10
	MOV	AH,1
	JNZ	Dial79
	JMP	Dial39
Dial81:	MOV	AL,0FFh			; Esc
Dial82:	CALL	HidCur
	POP	DX			; Restore window
	POP	SI
	TEST	CS:[BP].bFlags,10h
	JNZ	Dial83
	CALL	ResWin
	CALL	Window
Dial83:	POP	CX
	PUSH	BP
	PUSH	AX
	MOV	AH,1
	INT	10h
	POP	AX
	POP	BP
	CMP	AL,0
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	RET
Dialog	ENDP

Dial85	PROC	NEAR
	ADD	DI,SIZE BoxMn
	CMP	CS:[DI].mType,0
	JNE	Dial85
	SUB	DI,SIZE BoxMn
	RET
Dial85	ENDP

;--------------------- Input string --------------------
;			BH,BL - begin
;			ES:SI - string
;			   DI - cursor
;			   CX - length
;			   AH - number of color
;			   AL - key bar
;	Return :	   DI - cursor
;			   AX - key code
	.DATA
InSt_T	DW	0009h, InSt08
	DW	0F00h, InSt08
	DW	4B00h, InSt07
	DW	4800h, InSt08
	DW	5000h, InSt08
	DW	4900h, InSt08
	DW	5100h, InSt08
	DW	0011h, InSt04
	DW	000Dh, InSt08
	DW	001Bh, InSt08
	DW	4400h, InSt08
	DW	7100h, InSt08
	DW	0

	.CODE
InpStr	PROC	NEAR
	PUSH	BP
	PUSH	SI
	PUSH	DX
	MOV	DX,AX
InSt01:	MOV	AH,DH
	CALL	InSt10
	LEA	AX,[BX+DI]
	CALL	Cursor
	MOV	AL,DL
	CALL	Input
	CMP	AH,0FCh
	JE	InSt05
	MOV	BP,SI
	CMP	ES:Quote,0
	JNZ	InSt02
	MOV	SI,OFFSET InSt_T
	CALL	Case
InSt02:	CMP	BYTE PTR ES:[BP-1],0
	JNE	InSt04
	MOV	BYTE PTR ES:[BP-1],1
	CMP	ES:Quote,0
	JNZ	InSt03
	OR	AH,AH
	JE	InSt03
	CMP	AL,' '
	JB	InSt04
	CMP	AL,7Fh
	JE	InSt04
InSt03:	MOV	BYTE PTR ES:[BP],0
InSt04:	MOV	SI,BP
	XCHG	DI,DX
	CALL	EdLin
	XCHG	DI,DX
	JMP	InSt01
InSt05:	XCHG	BP,CX
	CALL	GetMous
	MOV	AH,0FCh
	XCHG	BP,CX
	JZ	InSt01
	CALL	ClrKbd
	CMP	AL,1
	JNE	InSt08
	PUSH	DX
	MOV	DL,CL
	MOV	DH,1
	XCHG	BP,CX
	CALL	InWind
	XCHG	BP,CX
	POP	DX
	JC	InSt08
	MOV	BYTE PTR ES:[SI-1],1
	SUB	BP,BX
	PUSH	DI
	PUSH	CX
	MOV	DI,SI
	MOV	AL,0
	REPNE	SCASB
	DEC	DI
	SUB	DI,SI
	CMP	BP,DI
	JBE	InSt06
	MOV	BP,DI
InSt06:	POP	CX
	POP	DI
	CMP	BP,DI
	JE	InSt05
	MOV	DI,BP
	JMP	InSt01
InSt07:	OR	DI,DI
	JNE	InSt02
InSt08:	POP	DX
	POP	SI
	POP	BP
	RET
InpStr	ENDP

InSt10	PROC	NEAR
	PUSH	DI
	PUSH	SI
	PUSH	DX
	PUSH	CX
	MOV	DH,1
	MOV	DL,CL
	CALL	BasAdr
	CALL	Color
InSt11:	LODS	ES:Byt
	CMP	AL,0
	JE	InSt12
	STOSW
	LOOP	InSt11
InSt12:	JCXZ	InSt13
	MOV	AL,' '
	REP	STOSW
InSt13:	CALL	Window
	POP	CX
	POP	DX
	POP	SI
	POP	DI
	RET
InSt10	ENDP

;--------------------- Select case ---------------------
;			BH,BL - begin
;			   AL - key bar
;			   AH - total
;			   CL - current
;			   CH - cursor
;	Return :	   CL - current
;			   CH - cursor
;			   AX - key code
	.DATA
Slct_T	DW	0020h, Slct06
	DW	0009h, Slct13
	DW	0F00h, Slct13
	DW	4B00h, Slct13
	DW	4D00h, Slct13
	DW	4800h, Slct09
	DW	5000h, Slct10
	DW	4700h, Slct12
	DW	4F00h, Slct11
	DW	4900h, Slct12
	DW	5100h, Slct11
	DW	000Dh, Slct05
	DW	001Bh, Slct13
	DW	4400h, Slct13
	DW	0

	.CODE
Select	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	DX
	MOV	DX,AX
	MOV	AL,0
Slct01:	PUSH	AX			; Cursor
	MOV	AX,BX
	ADD	AH,CH
	CALL	Cursor
	POP	AX
	CMP	AL,0
	JZ	Slct03
	PUSH	CX
Slct02:	CALL	GetMous
	JNZ	Slct02
	POP	CX
Slct03:	MOV	AL,DL			; Input
	CALL	Input
	CMP	AH,0FCh
	JE	Slct04
	MOV	SI,OFFSET Slct_T
	CALL	Case
	JMP	Slct03
Slct04:	XCHG	DI,CX			; Mouse
	CALL	GetMous
	MOV	AH,0FCh
	XCHG	DI,CX
	JZ	Slct03
	CALL	ClrKbd
	CMP	AL,1
	JNE	Slct13
	PUSH	DX
	DEC	BX
	CALL	Slct20
	XCHG	DI,CX
	CALL	InWind
	XCHG	DI,CX
	INC	BX
	POP	DX
	JC	Slct13
	MOV	AX,DI
	SUB	AH,BH
	CMP	AH,CH
	JE	Slct04
	MOV	CH,AH
	JMP	Slct07
Slct05:	CMP	CH,CL			; Enter
	JNE	Slct07
	JMP	Slct13
Slct06:	CMP	CH,CL			; Space
	JNE	Slct07
	INC	CH
	CMP	CH,DH
	JB	Slct07
	MOV	CH,0
Slct07:	PUSH	DX
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
	MOV	CL,CH
Slct08:	JMP	Slct01
Slct09:	OR	CH,CH			; Up
	JE	Slct13
	DEC	CH
	JMP	Slct01
Slct10:	INC	CH			; Down
	CMP	CH,DH
	JB	Slct08
Slct11:	MOV	CH,DH			; End, PgDn
	DEC	CH
	JMP	Slct13
Slct12:	MOV	CH,0			; Home, PgUp
Slct13:	POP	DX
	POP	DI
	POP	SI
	RET
Select	ENDP

Slct20	PROC	NEAR
	PUSH	DI
	MOV	DL,0
	CALL	BasAdr
Slct21:	CMP	BYTE PTR ES:[DI],' '
	JNE	Slct22
	CMP	BYTE PTR ES:[DI+2],' '
	JE	Slct23
Slct22:	INC	DI
	INC	DI
	INC	DL
	JNE	Slct21
Slct23:	POP	DI
	RET
Slct20	ENDP

;----------------------- Flip Flop ---------------------
;			BH,BL - begin
;			   AL - key bar
;			   CL - paremeter
;	Return :	   CL - paremeter
;			   AX - key code
FlipFl	PROC	NEAR
	PUSH	DI
	PUSH	DX
	MOV	CH,AL
	MOV	AX,BX
	CALL	Cursor
FlipF1:	MOV	AL,CH
	CALL	Input
	CMP	AX,3920h
	JE	FlipF2
	CMP	AH,0FCh
	JNE	FlipF5
	XCHG	DI,CX
	CALL	GetMous
	MOV	AH,0FCh
	XCHG	DI,CX
	JZ	FlipF1
	CALL	ClrKbd
	CMP	AL,1
	JNE	FlipF5
	DEC	BX
	CALL	Slct20
	MOV	DH,1
	XCHG	DI,CX
	CALL	InWind
	XCHG	DI,CX
	INC	BX
	JC	FlipF5
FlipF2:	CALL	BasAdr
	MOV	BYTE PTR ES:[DI],' '
	CMP	CL,0
	MOV	CL,0
	JNE	FlipF3
	MOV	BYTE PTR ES:[DI],'x'
	MOV	CL,1
FlipF3:	MOV	DX,101h
	CALL	Window
	CMP	AL,' '
	JE	FlipF1
	PUSH	CX
FlipF4:	CALL	GetMous
	JNZ	FlipF4
	POP	CX
	JMP	FlipF1
FlipF5:	POP	DX
	POP	DI
	RET
FlipFl	ENDP

;------------------------ Menu -------------------------
;			CS:SI - control line
;			BH,BL - begin
;			DH,DL - size
;			   BP - color table
;			   AL - key bar
;			   CL - case
;	Return :	   CL - case
;			   AX - key code
	.DATA
MnMn_T	DW	4B00h, MnMn24
	DW	4D00h, MnMn26
	DW	0008h, MnMn24
	DW	0020h, MnMn26
	DW	4700h, MnMn28
	DW	4F00h, MnMn29
	DW	0

	.CODE
MinMnu	PROC	NEAR
	PUSH	SI
	PUSH	DI
	CALL	HidCur
	MOV	DI,SI
	MOV	CH,AL
MnMn01:	MOV	AH,CS:[BP+0]
	MOV	AL,CS:[BP+1]
	CALL	MnMn40
	MOV	AL,CH
	AND	AL,7Fh
	CALL	Input
	CALL	MnMn10
	JC	MnMn01
	CMP	AH,0FCh
	JE	MnMn02
	PUSH	AX
	MOV	AH,CS:[BP+0]
	MOV	AL,CS:[BP+2]
	CALL	MnMn40
	POP	AX
MnMn02:	POP	DI
	POP	SI
	RET
MinMnu	ENDP

MnMn10	PROC	NEAR
	CMP	AH,0FCh
	JE	MnMn11
	JMP	MnMn19
MnMn11:	XCHG	CX,SI
	CALL	GetMous
	MOV	AH,0FCh
	XCHG	CX,SI
	JZ	MnMn16
	CMP	AL,1
	JNE	MnMn18
	PUSH	DX
	PUSH	CX
	MOV	CX,SI
	MOV	SI,DI
	CALL	MnMn60
	CALL	InWind
	MOV	SI,CX
	POP	CX
	POP	DX
	JC	MnMn18
	MOV	AX,SI
	SUB	AX,BX
	MOV	SI,DI
MnMn12:	CMP	BYTE PTR CS:[SI],0
	JE	MnMn11
	MOV	AH,CS:[SI+1]
	CMP	AL,AH
	JB	MnMn11
	ADD	AH,CS:[SI]
	CMP	AL,AH
	JB	MnMn13
	INC	SI
	INC	SI
	JMP	MnMn12
MnMn13:	MOV	AX,SI
	SUB	AX,DI
	SHR	AX,1
	CMP	AL,CL
	MOV	CL,AL
	JNE	MnMn16
	PUSH	DX
	PUSH	CX
	PUSH	BX
	ADD	BL,CS:[SI+1]
	MOV	DL,CS:[SI]
	MOV	DH,1
	XOR	SI,SI
MnMn14:	CALL	GetMous
	JZ	MnMn17
	CALL	InWind
	MOV	AL,'û'
	JNC	MnMn15
	MOV	AL,0
MnMn15:	MOV	AH,0
	CMP	AX,SI
	JE	MnMn14
	MOV	SI,AX
	CALL	TypMous
	JMP	MnMn14
MnMn16:	STC
	RET
MnMn17:	MOV	AL,0
	CALL	TypMous
	POP	BX
	POP	CX
	POP	DX
	OR	SI,SI
	JE	MnMn16
	MOV	AX,1C0Dh
MnMn18:	JMP	MnMn23
MnMn19:	MOV	SI,OFFSET MnMn_T
	CALL	Case
	CALL	UpCase
	CMP	AL,'A'
	JB	MnMn23
	CMP	AL,'Z'
	JA	MnMn23
	MOV	SI,DI
MnMn20:	CMP	BYTE PTR CS:[SI],0
	JE	MnMn16
	PUSH	DI
	PUSH	CX
	MOV	CX,CS:[SI]
	PUSH	BX
	ADD	BL,CH
	CALL	BasAdr
	POP	BX
	MOV	CH,0
	DEC	DI
MnMn21:	INC	DI
	SCASB
	LOOPNE	MnMn21
	POP	CX
	POP	DI
	JE	MnMn22
	INC	SI
	INC	SI
	JMP	MnMn20
MnMn22:	MOV	CX,SI
	SUB	CX,DI
	SHR	CX,1
	MOV	AX,1C0Dh
MnMn23:	CLC
	RET
MnMn24:	OR	CL,CL
	JE	MnMn25
	DEC	CX
	JMP	MnMn16
MnMn25:	CALL	MnMn50
	JE	MnMn16
	INC	CX
	JMP	MnMn25
MnMn26:	CALL	MnMn50
	JE	MnMn27
	INC	CX
	JMP	MnMn16
MnMn27:	MOV	CL,0
	JMP	MnMn16
MnMn28:	TEST	CH,80h
	JZ	MnMn23
	MOV	AX,4900h
	MOV	CL,0
	JMP	MnMn23
MnMn29:	TEST	CH,80h
	JZ	MnMn23
	MOV	AX,5100h
MnMn30:	CALL	MnMn50
	JE	MnMn23
	INC	CX
	JMP	MnMn30
MnMn10	ENDP

MnMn40	PROC	NEAR
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
MnMn40	ENDP

MnMn41	PROC	NEAR
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
MnMn41	ENDP

MnMn50	PROC	NEAR
	PUSH	DI
	PUSH	CX
	MOV	CH,0
	ADD	CX,CX
	ADD	DI,CX
	CMP	BYTE PTR CS:[DI+2],0
	POP	CX
	POP	DI
	RET
MnMn50	ENDP

MnMn60	PROC	NEAR
	PUSH	SI
MnMn61:	MOV	DX,CS:[SI]
	INC	SI
	INC	SI
	CMP	BYTE PTR CS:[SI],0
	JNE	MnMn61
	ADD	DL,DH
	MOV	DH,1
	POP	SI
	RET
MnMn60	ENDP

;-------------------- Output key bar -------------------
;			   AL - Case
	.DATA
KeyB_T	DB	 1, 0, 0, 0		;  0
	DB	 2, 0, 0, 0		;  1
	DB	 3, 0, 0, 0		;  2
	DB	 4, 0, 0, 0		;  3
LenKeyB	=	$ - KeyB_T

KeyC_T	DB	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0	;  0
	DB	 1,  2,  3,  4,  5,  6,  7,  8,  9, 10	;  1
	DB	 1,  0,  0,  0,  0,  0,  0,  0,  0, 10	;  2
	DB	 1,  0,  0,  0,  0,  0,  0,  8,  0, 10	;  3
	DB	 0,  0,  0,  0,  0,  0,  0,  0,  0, 10	;  4

	.CONST
Key_S	DB	'      '		;  0
	DB	'Help  '		;  1
	DB	'Config'		;  2
	DB	'Screen'		;  3
	DB	'Panels'		;  4
	DB	'B&W   '		;  5
	DB	'Color '		;  6
	DB	'Laptop'		;  7
	DB	'Reset '		;  8
	DB	'Cntry '		;  9
	DB	'Quit  '		; 10

	.CODE
PutKey	PROC	NEAR
	CMP	ES:KeyBar,0
	JNZ	PutKy1
	MOV	ES:BarPrev,0FFh
	RET
PutKy1:	PUSH	BX
	PUSH	AX
	MOV	BL,AL
	MOV	AH,2
	INT	16h
	MOV	AH,3
	TEST	AL,8
	JNZ	PutKy2
	DEC	AH
	TEST	AL,4
	JNZ	PutKy2
	DEC	AH
	TEST	AL,3
	JNZ	PutKy2
	DEC	AH
PutKy2:	MOV	AL,BL
	CALL	PutKey0
	POP	AX
	POP	BX
	RET
PutKey	ENDP

PutKey0	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	SHL	AL,1
	SHL	AL,1
	OR	AL,AH
	MOV	AH,0
	MOV	BX,AX
	MOV	AL,0
	CMP	BL,LenKeyB
	JAE	PutKy3
	MOV	AL,CS:KeyB_T[BX]
PutKy3:	CMP	AL,ES:BarPrev
	JE	PutKy7
	MOV	ES:BarPrev,AL
	MOV	AH,10
	MUL	AH
	MOV	SI,AX
	MOV	BH,ES:Lines
	DEC	BH
	MOV	BL,0
	CALL	BasAdr
	MOV	AH,KeyTi_C
	CALL	Color
	MOV	DH,AH
	MOV	AH,KeyNu_C
	CALL	Color
	MOV	DL,'1'
PutKy4:	MOV	AL,DL
PutKy5:	STOSW
	XCHG	AH,DH
	PUSH	SI
	PUSH	AX
	MOV	CX,6
	MOV	AL,CS:KeyC_T[SI]
	MUL	CL
	ADD	AX,OFFSET Key_S
	MOV	SI,AX
	POP	AX
	CALL	MvsBwc
	POP	SI
	INC	SI
	XCHG	AH,DH
	MOV	AL,' '
	STOSW
	INC	DX
	CMP	DL,'9'+1
	JB	PutKy4
	JA	PutKy6
	MOV	AL,'1'
	STOSW
	MOV	AL,'0'
	JMP	PutKy5
PutKy6:	MOV	DX,150h
	CALL	Window
PutKy7:	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	RET
PutKey0	ENDP

;------------------- Keyboard input --------------------
;			   AL - Key bar case
;	Return:		   AX - Key code
Input	PROC	NEAR
	PUSH	AX
	CALL	Input0
	CMP	AX,3B00h
	JNE	Input1
	CALL	Help
	POP	AX
	JMP	Input
Input1:	INC	SP
	INC	SP
	RET
Input	ENDP

Input0	PROC	C NEAR
	USES	DS,SI,DI,DX,CX,BX
	LOCAL	IKeyBar	:BYTE	,\
		BlMouse	:BYTE	,\
		KbdFlgs	:BYTE	,\
		AltLock	:BYTE	,\
		ClkFlag	:BYTE	,\
		ClkBuf	:WORD:6	,\
		BlTime	:DWORD	,\
		AltTime	:DWORD
	PUSH	ES
	POP	DS
	MOV	IKeyBar,AL
	MOV	AH,1
	INT	16h
	JZ	Inpt02
	MOV	AH,0
	INT	16h
Inpt01:	JMP	Inpt41
Inpt02:	XOR	AX,AX
	CALL	GetMous
	MOV	AH,0FCh
	TEST	AL,7
	JNZ	Inpt01
	CALL	Timer
	MOV	AltTime.Lo,DX
	MOV	AltTime.Hi,CX
	PUSH	ES
	PUSH	SS
	POP	ES
	MOV	SI,OFFSET ScrBuf+2*74
	LEA	DI,ClkBuf
	MOV	CX,6
	REP	MOVSW
	POP	ES
	MOV	AH,2
	INT	16h
	MOV	KbdFlgs,AL
	MOV	AltLock,0
Inpt03:	MOV	ClkFlag,0
	MOV	BlMouse,0
Inpt04:	MOV	CX,0FFFFh
	CMP	DS:BlDelay,0
	JE	Inpt05
	MOV	AH,2Ch
	INT	21h
	ADD	CL,DS:BlDelay
	CMP	CL,60
	JB	Inpt05
	SUB	CL,60
	INC	CH
Inpt05:	MOV	BlTime.Lo,DX
	MOV	BlTime.Hi,CX
Inpt10:	MOV	AL,IKeyBar
	CALL	PutKey
	CALL	LeaveTm
	CMP	DS:BlDelay,0
	JE	Inpt20
	MOV	AH,2Ch
	INT	21h
	CMP	CH,0
	JNE	Inpt11
	CMP	BYTE PTR BlTime[3],24
	JB	Inpt11
	MOV	BYTE PTR BlTime[3],CH
Inpt11:	CMP	CX,BlTime.Hi
	JB	Inpt20
	JA	Inpt12
	CMP	DX,BlTime.Lo
	JB	Inpt20
Inpt12:	CALL	Blank
	XOR	BX,BX
	MOV	DH,DS:Lines
	MOV	DL,80
	CALL	Window
Inpt13:	XOR	AX,AX
	CALL	GetMous
	TEST	AL,7
	JNZ	Inpt13
	JMP	Inpt03
Inpt20:	CMP	DS:Clock,0
	JE	Inpt30
	MOV	DI,OFFSET ScrBuf+2*75
	MOV	AH,2Ch
	INT	21h
	MOV	AL,CH
	MOV	AH,0
	CMP	DS:Country.TimeFmt,0
	JNZ	Inpt22
	DEC	DI
	DEC	DI
	MOV	BL,12
	DIV	BL
	CMP	AH,0
	JNE	Inpt21
	MOV	AH,12
Inpt21:	CMP	AL,0
	MOV	AL,AH
	MOV	BH,'a'
	JE	Inpt22
	MOV	BH,'p'
Inpt22:	MOV	BL,' '
	CMP	DL,50
	JB	Inpt23
	MOV	BL,DS:Country.TimeSep
Inpt23:	CMP	BL,ClkFlag
	JE	Inpt30
	MOV	ClkFlag,BL
	CALL	HexDec
	MOV	DX,AX
	MOV	AL,CL
	CALL	HexDec
	MOV	CX,AX
	MOV	AH,Clock_C
	CALL	Color
	MOV	AL,DL
	CMP	AL,'0'
	JNE	Inpt24
	MOV	AL,' '
Inpt24:	STOSW
	MOV	AL,DH
	STOSW
	MOV	AL,BL
	STOSW
	MOV	AL,CL
	STOSW
	MOV	AL,CH
	STOSW
	CMP	DS:Country.TimeFmt,0
	JNZ	Inpt25
	MOV	AL,BH
	STOSW
Inpt25:	MOV	BX,74
	MOV	DX,106h
	CALL	Window
Inpt30:	CMP	DS:AutoTim.Lo,0FFFFh
	JE	Inpt31
	MOV	BX,DX
	MOV	AH,2Ch
	INT	21h
	XCHG	BX,DX
	MOV	AX,0FE00h
	CMP	CX,DS:AutoTim.Lo
	JB	Inpt31
	JA	Inpt32
	CMP	BX,DS:AutoTim.Hi
	JAE	Inpt32
Inpt31:	CALL	Rnd
	MOV	AH,1
	INT	16h
	JZ	Inpt33
	MOV	AH,0
	INT	16h
Inpt32:	JMP	Inpt40
Inpt33:	XOR	AX,AX
	CALL	GetMous
	MOV	AH,0FCh
	TEST	AL,7
	JNZ	Inpt32
	MOV	AH,2
	INT	16h
	MOV	BL,AL
	XCHG	BL,KbdFlgs
	CMP	DS:DesqVie,0
	JNZ	Inpt34
	CMP	DS:AltBar,0
	JZ	Inpt34
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
	JNE	Inpt34
	MOV	AH,BL
	XOR	AH,AL
	JZ	Inpt34
	CMP	AH,8
	JNE	Inpt34
	MOV	BH,1
	TEST	AL,8
	JNZ	Inpt38
	CMP	AltLock,0
	JE	Inpt34
	MOV	AX,0FD00h
	JMP	Inpt40
Inpt34:	AND	BL,0F0h
	XOR	AL,BL
	JNZ	Inpt37
	XOR	CX,CX
	CALL	GetMous
	MOV	DX,102h
	MOV	BX,78
	CALL	InWind
	JC	Inpt35
	CMP	BlMouse,0
	JE	Inpt36
	JMP	Inpt12
Inpt35:	MOV	BlMouse,1
Inpt36:	MOV	BH,DS:Lines
	DEC	BH
	CALL	InWind
	JNC	Inpt39
	JMP	Inpt10
Inpt37:	TEST	AL,0F7h
	JZ	Inpt39
	MOV	BH,0
Inpt38:	MOV	AltLock,BH
Inpt39:	JMP	Inpt04
Inpt40:	CMP	DS:Clock,0
	JZ	Inpt41
	PUSH	DS
	PUSH	SS
	POP	DS
	LEA	SI,ClkBuf
	MOV	DI,OFFSET ScrBuf+2*74
	MOV	CX,6
	REP	MOVSW
	POP	DS
	MOV	BX,74
	MOV	DX,106h
	CALL	Window
Inpt41:	MOV	DS:AutoTim.Lo,0FFFFh
	CMP	AH,0FCh
	JNE	Inpt55
	CMP	DS:KeyBar,0
	JZ	Inpt55
	CALL	GetMous
	MOV	AH,0FCh
	INC	CH
	CMP	CH,DS:Lines
	JNE	Inpt55
	DEC	CH
	MOV	BL,CL
	AND	BX,0F8h
	XOR	SI,SI
	XOR	DI,DI
Inpt50:	PUSH	BX
	MOV	BH,DS:Lines
	DEC	BH
	MOV	DX,108h
	CALL	InWind
	POP	DX
	MOV	BH,AL
	MOV	AL,0
	JC	Inpt51
	OR	DI,DI
	JZ	Inpt52
	MOV	AL,'û'
Inpt51:	MOV	AH,0
	CMP	AX,SI
	JE	Inpt52
	MOV	SI,AX
	CALL	TypMous
Inpt52:	MOV	AH,3
	TEST	BH,2
	JNZ	Inpt54
	MOV	AH,1
	TEST	BH,4
	JNZ	Inpt54
	MOV	AH,0
	TEST	BH,1
	JZ	Inpt56
Inpt54:	MOV	AL,IKeyBar
	CALL	PutKey0
	CALL	GetMous
	OR	DI,DI
	JNZ	Inpt50
	PUSH	BX
	CMP	BL,72
	CMC
	ADC	BL,1
	MOV	BH,CH
	CALL	BasAdr
	POP	BX
	CMP	BYTE PTR DS:[DI],' '
	JNE	Inpt50
	MOV	AH,0FCh
Inpt55:	CLC
	JMP	Inpt59
Inpt56:	CMP	AL,0
	JE	Inpt57
	MOV	AL,0
	CALL	TypMous
	MOV	AH,68h
	TEST	DH,2
	JNZ	Inpt58
	MOV	AH,54h
	TEST	DH,4
	JNZ	Inpt58
	MOV	AH,3Bh
	TEST	DH,1
	JNZ	Inpt58
Inpt57:	JMP	Inpt02
Inpt58:	MOV	CL,3
	SHR	BL,CL
	ADD	AH,BL
	MOV	AL,0
	STC
Inpt59:	RET
Input0	ENDP

LeaveTm	PROC	NEAR
	INT	28h
	CMP	DS:DESQvie,0
	JZ	LvTm1
	MOV	AX,101Ah
	INT	15h
	MOV	AX,1000h
	INT	15h
	MOV	AX,1025h
	INT	15h
LvTm1:	RET
LeaveTm	ENDP

;--------------------- Blank screen --------------------
Blank	PROC	NEAR
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
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
	DEC	DH
	MOV	DL,79
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
	MOV	AH,2
	INT	16h
	MOV	CL,AL
	XOR	DI,DI
Blnk01:	PUSH	SI
	PUSH	CX
	CALL	Rnd
	MOV	BL,ES:Lines
	MOV	BH,0
	MUL	BX
	MOV	CL,DL
	CALL	Rnd
	MOV	BX,80
	MUL	BX
	MOV	DH,CL
	CALL	Rnd
	MOV	CL,3
	ROR	AX,CL
	CMP	AL,100
	JB	Blnk05
	MOV	BL,AL
	CALL	Rnd
	MOV	CL,5
	ROR	AL,CL
	CMP	AL,50
	JB	Blnk05
	OR	AL,AH
	AND	AL,1
	MOV	BH,AL
	MOV	SI,OFFSET Stars
	MOV	CX,NmbStrs
	XOR	BP,BP
Blnk02:	LODSW
	OR	AX,AX
	LODSW
	JNE	Blnk03
	OR	BP,BP
	JNE	Blnk04
	MOV	BP,SI
	JMP	Blnk04
Blnk03:	CMP	AX,DX
	JE	Blnk05
Blnk04:	LOOP	Blnk02
	OR	BP,BP
	JE	Blnk05
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
	MOV	AL,ES:StarTab[0]
	MOV	AH,9
	INT	10h
Blnk05:	MOV	SI,OFFSET Stars
	MOV	CX,NmbStrs
Blnk06:	PUSH	CX
	LODSW
	MOV	BX,AX
	LODSW
	OR	BX,BX
	JE	Blnk09
	MOV	DX,AX
	DEC	BX
	MOV	DS:[SI-SIZE STAR].StarClk,BL
	MOV	AL,ES:StarTab[1]
	CMP	BL,12
	JE	Blnk08
	MOV	AL,ES:StarTab[2]
	CMP	BL,9
	JNE	Blnk07
	OR	BH,BH
	JE	Blnk08
	MOV	BL,0
Blnk07:	MOV	AL,ES:StarTab[3]
	CMP	BL,6
	JE	Blnk08
	MOV	AL,ES:StarTab[4]
	CMP	BL,3
	JE	Blnk08
	OR	BL,BL
	JNE	Blnk09
	XOR	AX,AX
	MOV	DS:[SI-SIZE STAR].StarMod,AX
	MOV	DS:[SI-SIZE STAR].StarPos,AX
	MOV	AL,' '
Blnk08:	PUSH	AX
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
Blnk09:	POP	CX
	LOOP	Blnk06
	POP	CX
	POP	SI
Blnk10:	MOV	AH,1
	INT	16h
	JNZ	Blnk13
	MOV	AH,2
	INT	16h
	XCHG	AL,CL
	AND	AL,0F0h
	XOR	AL,CL
	JNZ	Blnk14
	MOV	BX,CX
	XOR	AX,AX
	XOR	CX,CX
	CALL	GetMous
	TEST	AL,7
	JNZ	Blnk14
	PUSH	BX
	MOV	BX,76
	MOV	DX,204h
	CALL	InWind
	POP	BX
	JC	Blnk11
	MOV	SI,CX
Blnk11:	CMP	CX,SI
	JNE	Blnk14
	CALL	Timer
	MOV	CX,BX
	XCHG	DX,DI
	CMP	DX,DI
	JE	Blnk12
	JMP	Blnk01
Blnk12:	CALL	LeaveTm
	JMP	Blnk10
Blnk13:	MOV	AH,0
	INT	16h
Blnk14:	MOV	AH,0Fh
	INT	10h
	POP	DX
	MOV	AH,2
	INT	10h
	POP	CX
	MOV	AH,1
	INT	10h
	MOV	AL,1
	CALL	CurMous
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	RET
Blank	ENDP

Rnd	PROC	NEAR
	PUSH	DX
	PUSH	CX
	PUSH	BX
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
	POP	BX
	POP	CX
	POP	DX
	RET
Rnd	ENDP

;----------------------- Edit line ---------------------
;			ES:SI - begin of line
;			   DX - cursor
;			   CX - length of line
;			   AX - key code
;	Return :	 CY=0 - no change
;			 CY=1 - change
;			   DX - cursor
	.DATA
EdKeys	DW	0013h, E01_00		; ^S
	DW	4B00h, E01_00		; Left
	DW	0004h, E02_00		; ^D
	DW	4D00h, E02_00		; Right
	DW	0001h, E03_00		; ^A
	DW	7300h, E03_00		; ^Left
	DW	0006h, E04_00		; ^F
	DW	7400h, E04_00		; ^Right
	DW	4700h, E05_00		; Home
	DW	4F00h, E06_00		; End
	DW	7700h, E05_00		; ^Home
	DW	7500h, E06_00		; ^End
	DW	0008h, E07_00		; BackSpace
	DW	0007h, E08_00		; ^G
	DW	5300h, E08_00		; Del
	DW	0019h, E09_00		; ^Y
	DW	000Bh, E10_00		; ^K
	DW	0017h, E11_00		; ^W
	DW	007Fh, E11_00		; ^BackSpace
	DW	0014h, E12_00		; ^T
	DW	0011h, E13_00		; ^Q
	DW	0

	.CODE
EdLin	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	DI
	PUSH	CX
	PUSH	AX
	PUSH	ES
	POP	DS
	MOV	DI,SI
	CMP	DS:Quote,0
	JNZ	E00_03
	MOV	SI,OFFSET EdKeys
	CALL	Case
	OR	AX,AX
	JE	E00_04
	OR	AH,AH
	JE	E00_01
	CMP	AL,' '
	JB	E00_04
E00_01:	CMP	DX,CX			; Symbols
	JB	E00_02
	CALL	Beep
	JMP	E00_04
E00_02:	PUSH	DI
	ADD	DI,CX
	MOV	SI,DI
	DEC	SI
	PUSH	DI
	SUB	CX,DX
	STD
	REP	MOVSB
	CLD
	POP	DI
	MOV	BYTE PTR DS:[DI],0
	POP	SI
	ADD	SI,DX
	MOV	DS:[SI],AL
	INC	DX
	JMP	E00_06
E00_03:	MOV	DS:Quote,0
	CMP	AL,0
	JNE	E00_01
E00_04:	CLC
	JMP	E00_07
E00_05:	LODSB
	STOSB
	CMP	AL,0
	JNE	E00_05
E00_06:	STC
E00_07:	POP	AX
	POP	CX
	POP	DI
	POP	SI
	POP	DS
	RET

E01_00:	OR	DX,DX			; Left,   ^S
	JE	E00_04
	DEC	DX
	JMP	E00_06
E02_00:	ADD	DI,DX			; Right,  ^D
	CMP	BYTE PTR DS:[DI],0
	JE	E00_04
	INC	DX
	JMP	E00_06
E03_00:	CALL	EdSub1			; ^Left,  ^A
	JMP	E00_06
E04_00:	CALL	EdSub2			; ^Right, ^F
	JMP	E00_06
E05_00:	XOR	DX,DX			; Home
	JMP	E00_06
E06_00:	MOV	SI,DI			; End
	MOV	AL,0
	INC	CX
	REPNE	SCASB
	DEC	DI
	MOV	DX,DI
	SUB	DX,SI
	JMP	E00_06
E07_00:	OR	DX,DX			; BackSpace
	JE	E00_04
	DEC	DX
E08_00:	ADD	DI,DX			; Del, ^G
	CMP	BYTE PTR DS:[DI],0
	JE	E00_04
	LEA	SI,[DI+1]
	JMP	E00_05
E09_00:	XOR	DX,DX			; ^Y
E10_00:	ADD	DI,DX			; ^K
	MOV	BYTE PTR ES:[DI],0
	JMP	E00_06
E11_00:	CALL	EdSub1			; ^BackSpace, ^W
	JMP	E00_05
E12_00:	PUSH	DX			; ^T
	CALL	EdSub2
	POP	DX
	JMP	E00_05
E13_00:	MOV	DS:Quote,1		; ^Q
	JMP	E00_04
EdLin	ENDP

EdSub1	PROC	NEAR
	ADD	DI,DX
	MOV	SI,DI
ES1_01:	OR	DX,DX
	JE	ES1_03
	DEC	DX
	DEC	DI
	MOV	AL,ES:[DI]
	CALL	CmpSym
	JE	ES1_01
ES1_02:	OR	DX,DX
	JE	ES1_03
	DEC	DX
	DEC	DI
	MOV	AL,ES:[DI]
	CALL	CmpSym
	JNE	ES1_02
	INC	DX
	INC	DI
ES1_03:	CMP	SI,DI
	RET
EdSub1	ENDP

EdSub2	PROC	NEAR
	ADD	DI,DX
	MOV	SI,DI
ES2_01:	CMP	BYTE PTR ES:[SI],0
	JE	ES2_03
	INC	SI
	INC	DX
	MOV	AL,ES:[SI]
	CALL	CmpSym
	JE	ES2_01
ES2_02:	CMP	BYTE PTR ES:[SI],0
	JE	ES2_03
	INC	SI
	INC	DX
	MOV	AL,ES:[SI]
	CALL	CmpSym
	JNE	ES2_02
ES2_03:	CMP	SI,DI
	RET
EdSub2	ENDP

;-------------------- Compare symbol -------------------
;			   AL - Symbol
	.DATA
CtEn_T	DB	' +:.,><|\',9

	.CODE
CmpSym	PROC	NEAR
	PUSH	ES
	PUSH	DI
	PUSH	CX
	PUSH	CS
	POP	ES
	MOV	DI,OFFSET CtEn_T
	MOV	CX,10
	REPNE	SCASB
	POP	CX
	POP	DI
	POP	ES
	RET
CmpSym	ENDP

;----------------------- Get color ---------------------
;			   AH - color number
;	Return :	   AH - color
Color	PROC	NEAR
	PUSH	DI
	CALL	ColAddr
	MOV	AH,ES:[DI]
	POP	DI
	RET
Color	ENDP

ColAddr	PROC	NEAR
	PUSH	AX
	MOV	AL,6
	MUL	AH
	MOV	DI,AX
	MOV	AL,ES:ColTyp
	CMP	AL,3
	JB	ColAd1
	MOV	AL,1
ColAd1:	CBW
	ADD	AX,AX
	CMP	ES:ScrMod,7
	JNE	ColAd2
	INC	AX
ColAd2:	ADD	DI,AX
	ADD	DI,OFFSET ColTab
	POP	AX
	RET
ColAddr	ENDP

;------------------------- Shadow ----------------------
;			ES:DI - address of screen buffer
Shadow	PROC	NEAR
	PUSH	AX
	INC	DI
	MOV	AL,ES:[DI]
	MOV	AH,ShadFl
	CALL	Color
	CALL	ShadCol
	STOSB
	POP	AX
	RET
Shadow	ENDP

ShadCol	PROC	NEAR
	CMP	AH,1
	JB	Shadw1
	PUSHF
	AND	AL,8Fh
	XOR	AL,8
	POPF
	JE	Shadw1
	MOV	AL,7
Shadw1:	RET
ShadCol	ENDP

;-------------------- Moving strings -------------------
;			CS:SI(B) -> ES:DI(W)
MvsBwc	PROC	NEAR
	LODS	CS:Byt
	STOSW
	LOOP	MvsBwc
	RET
MvsBwc	ENDP

;			(Color) -> ES:DI(W)
MvsColr	PROC	NEAR
	INC	DI
	STOSB
	LOOP	MvsColr
	RET
MvsColr	ENDP
;			CS:SI(B) -> ES:DI(W)
MovDz	PROC	NEAR
	CALL	BasAdr
MovDz0:	LODSB
	CMP	AL,0
	JE	MovDz1
	STOSB
	INC	DI
	JMP	MovDz0
MovDz1:	RET
MovDz	ENDP

;--------------------- Upper letter --------------------
;			   AL - letter
;	Return :	   AL - letter
UpCase	PROC	NEAR
	CMP	AL,'a'
	JB	UpCas1
	CMP	AL,'z'
	JA	UpCas1
	SUB	AL,20h
UpCas1:	CALL	ES:Country.CaseMap
	RET
UpCase	ENDP

;--------------------- Lower letter --------------------
;			   AL - letter
;	Return :	   AL - letter
LowCase	PROC	NEAR
	CMP	AL,'A'
	JB	LoCas2
	CMP	AL,'Z'
	JA	LoCas1
	ADD	AL,20h
	RET
LoCas1:	CMP	AL,80h
	JB	LoCas2
	PUSH	BX
	MOV	BL,AL
	MOV	BH,0
	MOV	AL,ES:LowTab[BX-128]
	POP	BX
LoCas2:	RET
LowCase	ENDP

;------------------------ Cut path ---------------------
;			DS:SI - path
CutPath	PROC	NEAR
	PUSH	BX
	PUSH	AX
CutPt1:	MOV	BX,SI
CutPt2:	LODSB
	CMP	AL,':'
	JE	CutPt1
	CMP	AL,'\'
	JE	CutPt1
	CMP	AL,0
	JNE	CutPt2
	MOV	SI,BX
	POP	AX
	POP	BX
	RET
CutPath	ENDP

;--------------------- Normal string -------------------
;			ES:SI - address of string
NormStr	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	BX
	PUSH	AX
	MOV	DI,SI
NrmSt1:	LODS	ES:Byt
	CMP	AL,' '
	JE	NrmSt1
	DEC	SI
NrmSt2:	MOV	BX,DI
NrmSt3:	LODS	ES:Byt
	STOSB
	CMP	AL,' '
	JE	NrmSt3
	CMP	AL,0
	JNE	NrmSt2
	MOV	ES:[BX],AL
	POP	AX
	POP	BX
	POP	DI
	POP	SI
	RET
NormStr	ENDP

;---------------------- Higher letter ------------------
;			ES:SI - address of string
PathLin	PROC	NEAR
	PUSH	DI
	PUSH	AX
	MOV	DI,SI
PthLn1:	MOV	AL,ES:[DI]
	CALL	UpCase
	STOSB
	CMP	AL,0
	JNE	PthLn1
	POP	AX
	POP	DI
	RET
PathLin	ENDP

;----------------- Higher & lower letters --------------
;			ES:SI - address of string
LetLin	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	DI
	PUSH	AX
	CALL	PathLin
	PUSH	ES
	POP	DS
	CALL	CutPath
	MOV	DI,SI
LetLn1:	LODSB
	CALL	LowCase
	STOSB
	CMP	AL,0
	JNE	LetLn1
	POP	AX
	POP	DI
	POP	SI
	POP	DS
	RET
LetLin	ENDP

;-------------------- Shirter string -------------------
;			ES:SI - address of string
;			   AX - limit
ShrtLin	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	DI
	PUSH	CX
	PUSH	AX
	PUSH	AX
	MOV	DI,SI
	MOV	CX,256
	MOV	AL,0
	REPNE	SCASB
	DEC	DI
	SUB	DI,SI
	POP	CX
	SUB	DI,CX
	JBE	ShtLn3
	PUSH	ES
	POP	DS
	XCHG	SI,DI
	CMP	BYTE PTR DS:[DI+1],':'
	JNE	ShtLn1
	INC	DI
	INC	DI
	DEC	CX
	DEC	CX
ShtLn1:	CMP	BYTE PTR DS:[DI],'\'
	JNE	ShtLn2
	INC	DI
	DEC	CX
ShtLn2:	MOV	AL,'.'
	STOSB
	STOSB
	STOSB
	DEC	CX
	DEC	CX
	ADD	SI,DI
	REP	MOVSB
ShtLn3:	POP	AX
	POP	CX
	POP	DI
	POP	SI
	POP	DS
	RET
ShrtLin	ENDP

;-------------------- Text hex number ------------------
;			   AL - ASCII byte
;	Return :	   AL - hex number, CY=0
;				     error, CY=1
TstHex	PROC	NEAR
	CMP	AL,'0'
	JB	TstHx2
	CMP	AL,'9'
	JA	TstHx1
	SUB	AL,'0'
	RET
TstHx1:	CALL	UpCase
	CMP	AL,'A'
	JB	TstHx2
	CMP	AL,'F'
	JA	TstHx2
	SUB	AL,'A'-10
	RET
TstHx2:	STC
	RET
TstHex	ENDP

;----------------- Convert hex to dec ------------------
HexDec	PROC	NEAR
	PUSH	CX
	MOV	AH,0
	MOV	CL,10
	DIV	CL
	ADD	AX,'00'
	POP	CX
	RET
HexDec	ENDP

;--------------------- Number input --------------------
;			ES:SI - string
;	Return :	   AX - number
;			 CY=1 - error
TxtNum	PROC	NEAR
	PUSH	DX
	PUSH	BX
	CLD
	PUSH	SI
	XOR	BX,BX
TxtNm1:	LODS	ES:Byt
	SUB	AL,'0'
	JB	TxtNm3
	CMP	AL,9
	JA	TxtNm3
	PUSH	AX
	MOV	AX,10
	MUL	BX
	MOV	BX,AX
	POP	AX
	OR	DX,DX
	JNE	TxtNm2
	CBW
	ADD	BX,AX
	JNC	TxtNm1
TxtNm2:	MOV	BX,0FFFFh
	JMP	TxtNm1
TxtNm3:	DEC	SI
	POP	AX
	CMP	AX,SI
	CMC
	MOV	AX,BX
	POP	BX
	POP	DX
	RET
TxtNum	ENDP

;----------------------- Find text ---------------------
;			DS:SI - string
FndLet	PROC	NEAR
	LODSB
	CMP	AL,' '
	JE	FndLet
	CMP	AL,9
	JE	FndLet
	DEC	SI
	RET
FndLet	ENDP
