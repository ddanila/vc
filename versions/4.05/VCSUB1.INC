;--------------------- Subroutines ---------------------
;		Version :	09.09.1999
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

;---------------------- Break test ---------------------
;	Return :	CY=1 - break
TstBrk	PROC	NEAR
	PUSH	BX
	PUSH	AX
	MOV	BL,1
TstBr1:	MOV	AH,1
	INT	16h
	JZ	TstBr3
	OR	AX,AX
	JE	TstBr2
	CMP	AX,2E03h
	JE	TstBr2
	CMP	AX,011Bh
	JE	TstBr2
	CMP	AX,4400h
	JNE	TstBr3
TstBr2:	MOV	AH,0
	INT	16h
	MOV	BL,0
	JMP	TstBr1
TstBr3:	CMP	BL,1
	POP	AX
	POP	BX
	RET
TstBrk	ENDP

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

;------------------ Check video mode -------------------
;			   ZR - ok
ChkMode	PROC	NEAR
	MOV	AH,0Fh
	INT	10h
	CMP	AH,80
	JNE	ChkMd1
	AND	AL,7Fh
	CMP	AL,2
	JE	ChkMd1
	CMP	AL,3
	JE	ChkMd1
	CMP	AL,7
ChkMd1:	RET
ChkMode	ENDP

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

;------------------ Set cursor on prompt ---------------
Cursor1	PROC	NEAR
	PUSH	AX
	MOV	AL,ES:DosCol
	SUB	AL,ES:OutCol
	ADD	AL,ES:BegCol
	MOV	AH,ES:DosLin
	CALL	Cursor
	POP	AX
	RET
Cursor1	ENDP

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

;----------------------- Beep sound --------------------
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

;---------------------- Init mouse ---------------------
IniMous	PROC	NEAR
	MOV	AX,21h
	INT	33h
	CMP	AX,21h
	JNE	IniMo1
	XOR	AX,AX
	INT	33h
IniMo1:	CMP	AX,0FFFFh
	CMC
	MOV	AL,0
	ADC	AL,AL
	MOV	ES:Mouse,AL
	OR	AL,AL
	JNE	IniMo2
	MOV	AX,0FFFFh
	MOV	ES:MouseX,AX
	MOV	ES:MouseY,AX
	RET
IniMo2:	MOV	AL,0
	CALL	TypMous
	MOV	DL,ES:Lines
	CMP	DL,25
	JE	IniMo3
	MOV	DH,0
	MOV	CL,3
	SHL	DX,CL
	DEC	DX
	XOR	CX,CX
	MOV	AX,8
	INT	33h
IniMo3:	MOV	CX,ES:MouseX
	MOV	DX,ES:MouseY
	MOV	AX,CX
	AND	AX,DX
	INC	AX
	JE	IniMo4
	MOV	AX,4
	INT	33h
IniMo4:	MOV	AL,1
	CALL	CurMous
	RET
IniMous	ENDP

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

;------------------ Save mouse cursor ------------------
SavMous	PROC	NEAR
	CMP	ES:Mouse,0
	JE	SavMo1
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	MOV	AX,3
	INT	33h
	MOV	ES:MouseX,CX
	MOV	ES:MouseY,DX
	POP	AX
	POP	BX
	POP	CX
	POP	DX
SavMo1:	RET
SavMous	ENDP

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

;-------------------- Screen Offset --------------------
ScrOffs	PROC	NEAR
	PUSH	DS
	XOR	DI,DI
	MOV	AH,0Fh
	INT	10h
	OR	BH,BH
	JE	ScrOf1
	MOV	DS,DI
	MOV	DI,DS:[44Eh]
ScrOf1:	POP	DS
	MOV	DS:Screen.Offs,DI
	RET
ScrOffs	ENDP

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

;----------------- Restore user screen -----------------
;			BH,BL - begin
;			DH,DL - size
ResUser	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	CLD
	PUSH	ES
	POP	DS
	CALL	BasAdr
	LEA	SI,[DI-ScrBuf+UserScr]
	MOV	CL,DH
	MOV	CH,0
	MOV	DH,0
ResUs1:	PUSH	SI
	PUSH	DI
	PUSH	CX
	MOV	CX,DX
	REP	MOVSW
	POP	CX
	POP	DI
	POP	SI
	ADD	SI,2*80
	ADD	DI,2*80
	LOOP	ResUs1
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	DS
	RET
ResUser	ENDP

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

;-------------------------- Box ------------------------
;			ES:DI - screen buffer
;			   AH - color
;		    ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
;		    º                 º   ¿
;		    º                 º   Ã SI
;		    º                 º   Ù
;		    ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼
;		 ÀÂÙ ÀÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÙ ÀÂÙ
;		  CX         DX         CX
BoxDubl	PROC	NEAR
	PUSH	SI
	PUSH	BX
	PUSH	AX
	MOV	AL,'Í'
	MOV	BX,'É»'
	CALL	BoxLine
	MOV	AL,' '
	MOV	BX,'ºº'
BoxDb1:	CALL	BoxLine
	DEC	SI
	JNE	BoxDb1
	MOV	AL,'Í'
	MOV	BX,'È¼'
	CALL	BoxLine
	POP	AX
	POP	BX
	POP	SI
	RET
BoxDubl	ENDP

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

StdLine	PROC	NEAR
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	CALL	BasAdr
	SUB	DL,10
	MOV	DH,0
	MOV	CX,3
	MOV	BX,SI
	CALL	BoxLine
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	INC	BH
	RET
StdLine	ENDP

;------------------- Zoom box only once ----------------
OncZoom	PROC	NEAR
	PUSH	AX
	MOV	AL,ES:ZoomWin
	CMP	ES:ZoomOne,0
	JZ	OncZo1
	MOV	ES:ZoomWin,0
OncZo1:	CALL	DialBox
	CALL	Window
	MOV	ES:ZoomWin,AL
	MOV	ES:ZoomOne,1
	POP	AX
	RET
OncZoom	ENDP

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
	MOV	AL,CS:[BP].bMenu
	TEST	CS:[BP].bFlags,10h
	JNZ	Dial01
	MOV	BX,CS:[BP].BoxBase
	CMP	AL,1
	JNE	Dial01
	MOV	CX,DS
	CMP	CX,ES:WCB1seg
	JE	Dial01
	MOV	AL,5
	ADD	BL,40
Dial01:	CMP	AL,0FFh
	JE	Dial02
	CALL	PutMnu
Dial02:	MOV	AL,CS:[BP].bKeyBar
	CALL	PutKey
	TEST	CS:[BP].bFlags,4
	JZ	Dial03
	ADD	BH,ES:SftErr
Dial03:	TEST	CS:[BP].bFlags,10h
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
	MOV	DI,OFFSET DosBuf+LenCmd+10
	TEST	CH,1
	JZ	Dial14
	ADD	DI,100
Dial14:	MOV	CH,0
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
	MOV	AL,CS:[DI].bKeyBar
	MOV	CX,CS:[SI].mCtrl
	MOV	SI,OFFSET DosBuf+LenCmd+10+3
	TEST	CH,1
	JZ	Dial32
	ADD	SI,100
Dial32:	MOV	CH,0
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
	MOV	SI,OFFSET DosBuf+LenCmd+10+3
	TEST	CH,1
	JZ	Dial74
	ADD	SI,100
Dial74:	TEST	CH,2
	JZ	Dial75
	CALL	NormStr
Dial75:	MOV	CH,0
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
	MOV	AH,1
Dial79:	MOV	AL,1
	JMP	Dial71
Dial80:	TEST	CS:[BP].bFlags,8	; Alt-F10
	MOV	AH,0
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
	ADD	DI,ColTab
	POP	AX
	RET
ColAddr	ENDP

;------------------------- Shadow ----------------------
;			ES:DI - address of screen buffer
Shadow	PROC	NEAR
	PUSH	AX
	INC	DI
	MOV	AH,ShadFl
	CALL	Color
	MOV	AL,ES:[DI]
	CMP	AH,1
	JB	Shadw1
	PUSHF
	AND	AL,8Fh
	XOR	AL,8
	POPF
	JE	Shadw1
	MOV	AL,7
Shadw1:	STOSB
	POP	AX
	RET
Shadow	ENDP

;-------------------- Moving strings -------------------
;			CS:SI(B) -> ES:DI(W)
MvsBwc	PROC	NEAR
	LODS	CS:Byt
	STOSW
	LOOP	MvsBwc
	RET
MvsBwc	ENDP
;			ES:SI(B) -> ES:DI(W)
MvsBwe	PROC	NEAR
	LODS	ES:Byt
	STOSW
	LOOP	MvsBwe
	RET
MvsBwe	ENDP
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
;			Add file name
AddFile	PROC	NEAR
	MOV	AL,'\'
	CMP	AL,ES:[DI-1]
	JE	AddFl1
	STOSB
AddFl1:	RET
AddFile	ENDP
;			DS:SI(B) -> ES:DI(B)
MvzLin0	PROC	NEAR
	MOV	CX,LenPath
MvzLin0	ENDP

MvzLine	PROC	NEAR
	PUSH	DI
	PUSH	CX
	REP	MOVSB
	POP	CX
	POP	DI
	MOV	AL,0
	REPNE	SCASB
	DEC	DI
	RET
MvzLine	ENDP
;			Clear rest of line DS:SI, legth CX
FilZero	PROC	NEAR
	PUSH	ES
	PUSH	DI
	PUSH	DS
	POP	ES
	MOV	DI,SI
	MOV	AL,0
	REPNE	SCASB
	JCXZ	FilZr1
	REP	STOSB
FilZr1:	POP	DI
	POP	ES
	RET
FilZero	ENDP

;--------------------- Check CrLf ----------------------
;			   AX - word
;	Return :	   ZR - yes
IsCrLf0	PROC	NEAR
	DEC	SI
	DEC	SI
IsCrLf0	ENDP

IsCrLfS	PROC	NEAR
	LODSW
IsCrLfS	ENDP

IsCrLf	PROC	NEAR
	CMP	AX,0D0Ah
	JE	IsCrL1
	CMP	AX,0A0Dh
IsCrL1:	RET
IsCrLf	ENDP

;-------------------- String length --------------------
;			ES:DI - string
;	Return :	   CX - length
StrLen	PROC	NEAR
	PUSH	DI
	MOV	CX,0FFFFh
	MOV	AL,0
	REPNE	SCASB
	NOT	CX
	POP	DI
	RET
StrLen	ENDP

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

;----------------------- Main file ---------------------
;			NcPath + CS:SI -> ES:DI
MainFil	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	ES
	POP	DS
	MOV	SI,OFFSET NcPath
	MOV	CX,LenPath
	CALL	MvzLine
	POP	SI
	PUSH	CS
	POP	DS
	MOV	CX,13
	REP	MOVSB
	POP	DS
	RET
MainFil	ENDP

;-------------- Output number into buffer --------------
;			ES:DI - address of buffer
;			BX CX - number
;			   AL - mode: b0-coma, b1-spaces
Number	PROC	NEAR
	PUSH	BP
	PUSH	SI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	MOV	DH,AL
	CMP	ES:Country.Sep1000,0
	JNE	Numb01
	TEST	AL,1
	JZ	Numb01
	AND	DH,0FEh
	JZ	Numb01
	MOV	AL,' '
	STOSB
	STOSB
	STOSB
Numb01:	MOV	DL,0
	MOV	BP,0CA00h	; 1.000.000.000
	MOV	SI,3B9Ah
	CALL	Numb10
	CALL	Numb20
	MOV	BP,0E100h	;   100.000.000
	MOV	SI,5F5h
	CALL	Numb10
	MOV	BP,9680h	;    10.000.000
	MOV	SI,98h
	CALL	Numb10
	MOV	BP,4240h	;     1.000.000
	MOV	SI,0Fh
	CALL	Numb10
	CALL	Numb20
	MOV	BP,86A0h	;       100.000
	MOV	SI,1
	CALL	Numb10
	MOV	BP,10000	;        10.000
	XOR	SI,SI
	CALL	Numb10
	MOV	BP,1000		;         1.000
	CALL	Numb10
	CALL	Numb20
	MOV	BP,100		;           100
	CALL	Numb10
	MOV	BP,10		;            10
	CALL	Numb10
	MOV	AL,'0'		;             1
	ADD	AL,CL
	MOV	AH,0
	STOSW
	DEC	DI
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	SI
	POP	BP
	RET
Number	ENDP

Numb10	PROC	NEAR
	MOV	AL,'0'-1
Numb11:	INC	AX
	SUB	CX,BP
	SBB	BX,SI
	JNC	Numb11
	ADD	CX,BP
	ADC	BX,SI
	CMP	AL,'0'
	JE	Numb21
	MOV	DL,1
	STOSB
	RET
Numb10	ENDP

Numb20	PROC	NEAR
	TEST	DH,1
	JZ	Numb23
	MOV	AL,ES:Country.Sep1000
Numb21:	OR	DL,DL
	JNZ	Numb22
	TEST	DH,2
	JZ	Numb23
	MOV	AL,' '
Numb22:	STOSB
Numb23:	RET
Numb20	ENDP

;-------------------- Output hex byte ------------------
;			   AL - code
HexByt	PROC	NEAR
	PUSH	CX
	PUSH	AX
	MOV	CL,4
	SHR	AL,CL
	CALL	HexCod
	POP	AX
	CALL	HexCod
	POP	CX
	RET
HexByt	ENDP

HexCod	PROC	NEAR
	AND	AL,0Fh
	ADD	AL,'0'
	CMP	AL,'9'
	JBE	HexCd1
	ADD	AL,7
HexCd1:	STOSB
	INC	DI
	RET
HexCod	ENDP

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

;---------------------- Number input -------------------
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

;--------------- Convert name & extension --------------
;			DS:SI - address of file
;			ES:DI - convert buffer
Convert	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	CX
	PUSH	AX
        CLD
	MOV	CX,8
	MOV	AH,'.'
	CALL	Conv10
Conv01:	LODSB
	CMP	AL,'.'
	JE	Conv02
	CMP	AL,0
	JNE	Conv01
	DEC	SI
Conv02:	MOV	CX,3
	MOV	AH,0
	CALL	Conv10
	POP	AX
	POP	CX
	POP	DI
	POP	SI
	RET
Convert	ENDP

Conv10	PROC	NEAR
	LODSB
	CMP	AL,0
	JE	Conv11
	CMP	AL,AH
	JE	Conv11
	CMP	AL,'*'
	JE	Conv12
	CALL	UpCase
	STOSB
	LOOP	Conv10
	RET
Conv11:	MOV	AL,' '
	JMP	Conv13
Conv12:	MOV	AL,'?'
Conv13:	REP	STOSB
	DEC	SI
	RET
Conv10	ENDP

;-------------- Unconvert name & extension -------------
;			DS:SI - convert buffer
;			ES:DI - address of file
UnConv	PROC	NEAR
	PUSH	CX
	PUSH	BX
	PUSH	AX
	MOV	BX,DI
	MOV	CX,8
	CALL	UnCnv1
	MOV	AL,'.'
	STOSB
	MOV	CX,3
	CALL	UnCnv1
	MOV	BYTE PTR ES:[DI],0
	POP	AX
	POP	BX
	POP	CX
	RET
UnConv	ENDP

UnCnv1	PROC	NEAR
	LODSB
	STOSB
	CMP	AL,' '
	JE	UnCnv2
	MOV	BX,DI
UnCnv2:	LOOP	UnCnv1
	MOV	DI,BX
	RET
UnCnv1	ENDP

;--------------------- Test mask file ------------------
;			DS:SI - address of file
;			ES:DI - converted mask
;	Return :	 CY=1 - yes
;			 CY=0 - no
TstMas	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	CX
	PUSH	AX
	PUSH	DI
	MOV	DI,OFFSET TmpBuf+20
	PUSH	DI
	CALL	Convert
	POP	DI
	POP	SI
	MOV	CX,11
TstMs1:	LODS	ES:Byt
	CMP	AL,'?'
	JNE	TstMs2
	INC	DI
	JMP	TstMs3
TstMs2:	SCASB
	CLC
	JNE	TstMs4
TstMs3:	LOOP	TstMs1
	STC
TstMs4:	POP	AX
	POP	CX
	POP	DI
	POP	SI
	RET
TstMas	ENDP

;------------------------ Set DTA ----------------------
SetDTA	PROC	NEAR
	PUSH	DS
	PUSH	DX
	PUSH	AX
	PUSH	ES
	POP	DS
	MOV	DX,OFFSET DTA
	MOV	AH,1Ah
	INT	21h
	POP	AX
	POP	DX
	POP	DS
	RET
SetDTA	ENDP

;------------------- Test drive letter -----------------
;			   AL - drive letter
;	Return :	   ZR - yes
;			   NZ - no
TstDrv	PROC	NEAR
	PUSH	DX
	PUSH	AX
	SUB	AL,'A'
	MOV	DL,AL
	MOV	AH,19h
	INT	21h
	PUSH	AX
	MOV	AH,0Eh
	INT	21h
	MOV	AH,19h
	INT	21h
	CMP	AL,DL
	POP	DX
	PUSHF
	MOV	AH,0Eh
	INT	21h
	POPF
	POP	AX
	POP	DX
	RET
TstDrv	ENDP

;--------------- Int 21h with fatal error --------------
Intr21	PROC	NEAR
	PUSH	DS
	MOV	DS,CS:SecDat
	MOV	BYTE PTR DS:[ErrDat],0
	POP	DS
	INT	21h
	PUSHF
	PUSH	DS
	MOV	DS,CS:SecDat
	CMP	BYTE PTR DS:[ErrDat],0
	POP	DS
	JNE	I21_02
	POPF
	JNC	I21_01
	PUSH	DS
	PUSH	ES
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	XOR	BX,BX
	MOV	AH,59h
	INT	21h
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	POP	ES
	POP	DS
	STC
I21_01:	RET
I21_02:	POPF
	MOV	AX,53h
	STC
	RET
Intr21	ENDP

;------------------- Invert variable -------------------
;			ES:BX - address of variable
InvVar	PROC	NEAR
	CMP	BYTE PTR ES:[BX],1
	MOV	AL,0
	ADC	AL,0
	MOV	ES:[BX],AL
	RET
InvVar	ENDP

;----------------------- Check sum ---------------------
;			DS:SI - address
;			   CX - length
;	Return :	   AX - check sum
ChkSum	PROC	NEAR
	PUSH	SI
	PUSH	CX
	PUSH	BX
	XOR	BX,BX
	MOV	AH,0
ChkSm1:	LODSB
	ADD	BX,AX
	LOOP	ChkSm1
	LODSW
	CMP	AX,BX
	MOV	AX,BX
	POP	BX
	POP	CX
	POP	SI
	RET
ChkSum	ENDP

;----------------- CALL to PSP segment -----------------
;			   AX - offset in PSP
CallPSP	PROC	NEAR
	POP	BP
	PUSH	CS
	PUSH	BP
	PUSH	SS
	PUSH	AX
	RETF
CallPSP	ENDP

;----------------------- Page Up -----------------------
PgUp	PROC	NEAR
	OR	DI,DI
	JNE	PgUp1
	XOR	SI,SI
	JMP	PgUp3
PgUp1:	DEC	AX
	SUB	DI,AX
	JAE	PgUp2
	XOR	DI,DI
PgUp2:	ADD	AX,DI
	CMP	SI,AX
	JBE	PgUp3
	MOV	SI,AX
PgUp3:	RET
PgUp	ENDP

;---------------------- Page Down ----------------------
PgDn	PROC	NEAR
	PUSH	SI
	MOV	SI,BP
	SUB	BP,AX
	JAE	PgDn1
	XOR	BP,BP
PgDn1:	DEC	AX
	CMP	DI,BP
	JB	PgDn2
	POP	AX
	OR	SI,SI
	JE	PgDn5
	DEC	SI
	JMP	PgDn5
PgDn2:	POP	SI
	ADD	DI,AX
	JC	PgDn3
	CMP	DI,BP
	JBE	PgDn4
PgDn3:	MOV	DI,BP
PgDn4:	CMP	SI,DI
	JAE	PgDn5
	MOV	SI,DI
PgDn5:	RET
PgDn	ENDP
