;--------------------- Subroutines ---------------------
;		Version :	16.10.1999
;-------------------------------------------------------

;------------------ Begin of main windows --------------
;			   DS - WCBseg
;	Return :	BH,BL - Begin
BegBX	PROC	NEAR
	MOV	BX,DS
	CMP	BX,ES:WCB1seg
	MOV	BX,0
	JE	BegBX1
	MOV	BL,40
BegBX1:	CMP	BH,ES:MenuVs
	JZ	BegBX2
	INC	BH
BegBX2:	RET
BegBX	ENDP

;----------------- Size of main windows ----------------
;	Return :	DH,DL - Size
SizDX	PROC	NEAR
	MOV	DH,ES:Lines
	SHR	DH,1
	ADD	DH,3
	CMP	ES:Lines,25
	JBE	SizDX1
	INC	DH
SizDX1:	CMP	ES:FulScr,0
	JZ	SizDX2
	MOV	DH,ES:Lines
	SUB	DH,2
	CMP	ES:MenuVs,0
	JZ	SizDX2
	DEC	DH
SizDX2:	MOV	DL,40
	RET
SizDX	ENDP

;------------------ Files in main windows --------------
LnsAX	PROC	NEAR
	PUSH	DX
	CALL	SizDX
	MOV	AL,DH
	CBW
	SUB	AL,3
	CMP	AH,ES:MiniSta
	JZ	LnsAX1
	SUB	AL,2
LnsAX1:	POP	DX
	RET
LnsAX	ENDP

;------------------- Clear screen line -----------------
;			   BH - line
ClsLine	PROC	NEAR
	PUSH	CX
	MOV	BL,0
	CALL	BasAdr
	MOV	CX,80
	MOV	AH,DS:CrntCol
	MOV	AL,' '
	REP	STOSW
	POP	CX
	RET
ClsLine	ENDP

;-------------------- Output key bar -------------------
;			   AL - Case
	.DATA
KeyB_T	DB	 1, 2, 3, 4		;  0
	DB	 7, 0, 0, 0		;  1
	DB	 8, 0, 0, 0		;  2
	DB	 9,12, 0, 0		;  3
	DB	10,12, 0, 0		;  4
	DB	11,12, 0, 0		;  5
	DB	13,16, 0, 0		;  6
	DB	14,16, 0, 0		;  7
	DB	 6, 0, 0,17		;  8
	DB	 1, 2, 3, 5		;  9
	DB	15,16, 0, 0		; 10
	DB	 1, 1, 1, 1		; 11
	DB	 2, 2, 2, 2		; 12
	DB	 4, 4, 4, 4		; 13
	DB	18, 0, 0, 0		; 14
LenKeyB	=	$ - KeyB_T

KeyC_T	DB	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0	;  0
	DB	 1,  2,  3,  4,  5,  6,  7,  8,  9, 10	;  1
	DB	 0, 31,  3,  4,  5,  6,  7,  8, 11, 12	;  2
	DB	13, 14,  0, 32,  5,  6,  0,  8,  0,  0	;  3
	DB	13, 14, 15, 16, 30, 17, 18, 19, 20, 21	;  4
	DB	13, 14, 15, 16, 30, 17, 18, 19,  0, 21	;  5
	DB	 1,  0,  0,  0,  0,  0,  0,  0,  0, 21	;  6
	DB	 1,  0,  0,  0,  0,  0,  0,  0,  0, 10	;  7
	DB	 0,  0,  0,  0,  0,  0,  0,  0,  0, 10	;  8
	DB	 1,  0, 10, 22,  0,  0, 26,  0,  0, 10	;  9
	DB	 1, 24, 10, 23,  0,  0, 26,  0,  0, 10	; 10
	DB	 1, 25, 10, 23,  0,  0, 26,  0,  0, 10	; 11
	DB	 0,  0,  0,  0,  0,  0, 26,  0,  0,  0	; 12
	DB	 1, 27,  0, 22,  0,  0, 26,  0,  0, 10	; 13
	DB	 1, 27,  0, 23,  0,  0, 26,  0,  0, 10	; 14
	DB	 1, 27,  0,  0,  0,  0, 26,  0,  0, 10	; 15
	DB	 0, 28,  0,  0,  0,  0, 26,  0,  0, 29	; 16
	DB	 0,  0,  0,  0,  0,  0,  0,  0,  0, 21	; 17
	DB	 1, 33,  0,  0,  0,  0,  0,  0,  0, 10	; 18

	.CONST
Key_S	DB	'      '		;  0
	DB	'Help  '		;  1
	DB	'Menu  '		;  2
	DB	'View  '		;  3
	DB	'Edit  '		;  4
	DB	'Copy  '		;  5
	DB	'RenMov'		;  6
	DB	'Mkdir '		;  7
	DB	'Delete'		;  8
	DB	'PullDn'		;  9
	DB	'Quit  '		; 10
	DB	'SavSet'		; 11
	DB	'PullDn'		; 12
	DB	'Left  '		; 13
	DB	'Right '		; 14
	DB	'View..'		; 15
	DB	'Edit..'		; 16
	DB	'DirSiz'		; 17
	DB	'Find  '		; 18
	DB	'Histry'		; 19
	DB	'EGA Ln'		; 20
	DB	'Tree  '		; 21
	DB	'ASCII '		; 22
	DB	'Hex   '		; 23
	DB	'Wrap  '		; 24
	DB	'Unwrap'		; 25
	DB	'Search'		; 26
	DB	'Save  '		; 27
	DB	'Save..'		; 28
	DB	'Q&Save'		; 29
	DB	'Memory'		; 30
	DB	'Main  '		; 31
	DB	'Label '		; 32
	DB	'Rescan'		; 33

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
	INC	DX
	CMP	DL,'9'+1
	JA	PutKy6
	XCHG	AH,DH
	MOV	AL,' '
	STOSW
	JB	PutKy4
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

;-------------------- Output menu bar ------------------
;			   AL - Case
	.CONST
PtMnu_S	DB	0FFh,4,'Left',0FFh,4,'Files',0FFh,4,'Commands'
	DB	0FFh,4,'Options',0FFh,4,'Right',0FFh,31,0

	.DATA
PtMnu_B	DB	Menu_C

	.CODE
PutMnu3	PROC	NEAR
	MOV	AL,3
PutMnu3	ENDP

PutMnu	PROC	NEAR
	CMP	ES:MenuVs,0
	JNZ	PtMnu1
	RET
PtMnu1:	PUSH	DS
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	PUSH	CS
	POP	DS
	CBW
	MOV	CX,AX
	XOR	BX,BX
	CALL	BasAdr
	MOV	SI,OFFSET PtMnu_S
	PUSH	SI
	MOV	BP,OFFSET PtMnu_B
	MOV	AL,80
	CALL	NgLine
	POP	SI
	CMP	CL,5
	JA	PtMnu4
	JCXZ	PtMnu4
	MOV	BL,-2
PtMnu2:	LODSB
	INC	BX
	CMP	AL,0FFh
	JNE	PtMnu2
	LODSB
	DEC	BX
	ADD	BL,AL
	LOOP	PtMnu2
	MOV	BH,0
	MOV	CX,4-1
PtMnu3:	LODSB
	INC	CX
	CMP	AL,0FFh
	JNE	PtMnu3
	CALL	BasAdr
	MOV	AH,MnCur_C
	CALL	Color
	MOV	AL,AH
	CALL	MvsColr
PtMnu4:	XOR	BX,BX
	MOV	DX,150h
	CALL	Window
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	POP	DS
	RET
PutMnu	ENDP

;--------------------- Output prompt -------------------
PutPrm	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	BX
	PUSH	AX
	CALL	BegBX
	CALL	SizDX
	ADD	DH,BH
	PUSH	ES
	POP	DS
	MOV	BH,DS:DosLin
	CMP	BH,DH
	JAE	PutPr1
	PUSH	DS
	MOV	DS,DS:WCBcrn
	CMP	DS:Visible,0
	POP	DS
	JZ	PutPr1
	MOV	BH,DH
	MOV	DS:DosLin,BH
PutPr1:	MOV	BL,0
	CALL	BasAdr
	MOV	AH,DS:CrntCol
	MOV	SI,OFFSET DosPth
	LODSB
	STOSW
	INC	BX
	CMP	DS:Prompt,0
	JZ	PutPr3
PutPr2:	LODSB
	CMP	AL,0
	JE	PutPr3
	STOSW
	INC	BX
	JMP	PutPr2
PutPr3:	CMP	DS:PathEr,0
	JZ	PutPr4
	MOV	AL,'?'
	STOSW
	INC	BX
PutPr4:	MOV	AL,'>'
	STOSW
	INC	BX
	MOV	DS:BegCol,BL
	CALL	DosCom
	POP	AX
	POP	BX
	POP	DX
	POP	DI
	POP	SI
	POP	DS
	RET
PutPrm	ENDP

;------------------ Output command line ----------------
DosCom	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	PUSH	ES
	POP	DS
	MOV	AL,DS:DosCol
	MOV	BH,AL
	MOV	BL,0
	SUB	AL,5
	JB	DosCm1
	MOV	BL,DS:OutCol
	CMP	AL,BL
	JAE	DosCm1
	MOV	BL,AL
DosCm1:	MOV	CX,79
	SUB	CL,DS:BegCol
	SUB	BH,CL
	JB	DosCm2
	CMP	BH,BL
	JBE	DosCm2
	MOV	BL,BH
DosCm2:	MOV	DS:OutCol,BL
	MOV	BH,0
	LEA	SI,[BX].DosBuf
	INC	CX
	MOV	BH,DS:DosLin
	MOV	BL,DS:BegCol
	CALL	BasAdr
	MOV	AH,DS:CrntCol
DosCm3:	LODSB
	CMP	AL,0
	JNE	DosCm4
	DEC	SI
	MOV	AL,' '
DosCm4:	STOSW
	LOOP	DosCm3
	MOV	BL,0
	MOV	DX,150h
	CALL	Window
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	DS
	RET
DosCom	ENDP

;------------------------ Panel ------------------------
;			   DS - WCBseg
	.CONST
Brief_S	DB	'^rName',0FFh,9,'Name',0FFh,9,'Name',0
Full_S	DB	'^rName',0FFh,8,'Size',0FFh,5,'Date',0FFh,4,'Time',0

	.DATA
Panel_P	DB	Back_C, Title_C
Brief_T	DB	13, 26, 0
Full_T	DB	13, 23, 32, 0

	.CODE
Panel	PROC	NEAR
	PUSH	DS
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	MOV	BP,OFFSET Panel_P
	CALL	BegBX
	CALL	BasAdr
	CALL	SizDX
	MOV	DL,DH
	MOV	DH,0
	MOV	SI,DX
	DEC	SI
	DEC	SI
	XOR	CX,CX
	MOV	DX,40-2
	MOV	AH,CS:[BP]
	CALL	Color
	CALL	BoxDubl
	MOV	AL,'Ä'
	SUB	DI,2*3*80
	CMP	DS:WinTyp,1
	JE	Panel1
	CMP	DS:WinTyp,3
	JE	Panel1
	CMP	DS:WinTyp,2
	JNE	Panel2
	PUSH	CS
	POP	DS
	ADD	BX,114h
	MOV	SI,OFFSET Title_S+2
	CALL	CntLine
	CALL	BasAdr
	ADD	DI,2*(80-20)
	MOV	BX,'Ç¶'
	CALL	BoxLine
	ADD	DI,2*7*80
Panel1:	MOV	BX,'Ç¶'
	CALL	BoxLine
Panel2:	CMP	DS:WinTyp,0
	JNE	Panel6
	CALL	BasAdr
	PUSH	DI
	ADD	DI,2*85
	MOV	SI,OFFSET Brief_S
	MOV	BX,OFFSET Brief_T
	CMP	DS:BrfFul,0
	JZ	Panel3
	MOV	SI,OFFSET Full_S
	MOV	BX,OFFSET Full_T
Panel3:	PUSH	CS
	POP	DS
	MOV	AL,33
	CALL	NgLine
	POP	DI
	PUSH	AX
	CALL	LnsAX
	INC	AX
	MOV	CX,AX
	POP	AX
	MOV	SI,BX
	MOV	AL,'Ñ'
	CALL	Panel8
	MOV	AL,'³'
Panel4:	CALL	Panel8
	LOOP	Panel4
	MOV	AL,'Ï'
	CMP	ES:MiniSta,0
	JZ	Panel5
	PUSH	DI
	MOV	BX,'Ç¶'
	MOV	AL,'Ä'
	XOR	CX,CX
	CALL	BoxLine
	POP	DI
	MOV	AL,'Á'
Panel5:	CALL	Panel8
Panel6:	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	POP	DS
	RET
Panel	ENDP

Panel8	PROC	NEAR
	PUSH	SI
	MOV	BH,0
Panel9:	MOV	BL,DS:[SI]
	ADD	BL,BL
	MOV	ES:[DI+BX],AX
	INC	SI
	CMP	BH,DS:[SI]
	JNE	Panel9
	ADD	DI,2*80
	POP	SI
	RET
Panel8	ENDP

;------------------- Set directory mask ----------------
;			   DS - WCBseg
SetMask	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	CX
	PUSH	BX
	PUSH	AX
	MOV	DI,OFFSET TmpBuf
	MOV	AL,'?'
	MOV	CX,11
	PUSH	DI
	REP	STOSB
	POP	DI
	MOV	SI,OFFSET DskMas
	CMP	BYTE PTR DS:[SI],0
	JE	SetMs1
	CALL	Convert
SetMs1:	MOV	SI,OFFSET DskBuf
	MOV	BX,DS:EndFul
	CMP	SI,BX
	JAE	SetMs9
SetMs2:	SUB	BX,SIZE FilEnt
SetMs3:	CMP	BYTE PTR DS:Hidden,0
	JNZ	SetMs4
	TEST	BYTE PTR DS:[SI+13],6
	JNZ	SetMs6
SetMs4:	TEST	BYTE PTR DS:[SI+13],10h
	JNZ	SetMs8
	CMP	BYTE PTR DS:MasTyp,0
	JZ	SetMs5
	CALL	TstExe
	JNC	SetMs6
SetMs5:	CALL	TstMas
	JC	SetMs8
SetMs6:	CMP	SI,BX
	JE	SetMs9
	PUSH	SI
	PUSH	BX
	MOV	CX,SIZE FilEnt
SetMs7:	MOV	AL,DS:[SI]
	XCHG	AL,DS:[BX]
	MOV	DS:[SI],AL
	INC	SI
	INC	BX
	LOOP	SetMs7
	POP	BX
	POP	SI
	AND	BYTE PTR DS:[BX+13],3Fh
	JMP	SetMs2
SetMs8:	ADD	SI,SIZE FilEnt
	CMP	SI,BX
	JBE	SetMs3
SetMs9:	MOV	DS:EndBuf,SI
	POP	AX
	POP	BX
	POP	CX
	POP	DI
	POP	SI
	RET
SetMask	ENDP

;------------------ Test executable file ---------------
;			DS:SI - address of file
;	Return :	CY = 1 - executable file
;			CY = 0 - no
	.CONST
Ext_S	DB	'com','exe','bat'

	.CODE
TstExe	PROC	NEAR
	PUSH	ES
	PUSH	SI
	PUSH	DI
	PUSH	CX
	PUSH	AX
TstEx1:	LODSB
	CMP	AL,0
	JE	TstEx3
	CMP	AL,'.'
	JNE	TstEx1
	PUSH	CS
	POP	ES
	MOV	DI,OFFSET Ext_S
	MOV	CX,3
TstEx2:	PUSH	SI
	PUSH	DI
	PUSH	CX
	MOV	CX,3
	REPE	CMPSB
	POP	CX
	POP	DI
	POP	SI
	STC
	JE	TstEx3
	ADD	DI,3
	LOOP	TstEx2
	CLC
TstEx3:	POP	AX
	POP	CX
	POP	DI
	POP	SI
	POP	ES
	RET
TstExe	ENDP

;----------------------- File sort ---------------------
;			   DS - WCBseg
Sort	PROC	NEAR
	PUSH	ES
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	PUSH	DS			; Convert all
	POP	ES
	MOV	SI,OFFSET DskBuf
Sort01:	MOV	AL,0
	MOV	CX,13
	MOV	DI,SI
	REPNE	SCASB
	JCXZ	Sort02
	DEC	DI
	PUSH	DI
	MOV	AL,' '
	REP	STOSB
	POP	CX
	SUB	CX,SI
	MOV	AL,'.'
	CMP	AL,DS:[SI]
	JE	Sort02
	MOV	DI,SI
	REPNE	SCASB
	JNE	Sort02
	MOV	AX,DS:[DI]
	MOV	BL,DS:[DI+2]
	MOV	DS:[SI+9],AX
	MOV	DS:[SI+11],BL
	DEC	DI
	LEA	CX,[SI+8]
	SUB	CX,DI
	JCXZ	Sort02
	MOV	AL,' '
	REP	STOSB
Sort02:	ADD	SI,SIZE FilEnt
	CMP	SI,DS:EndBuf
	JB	Sort01
	MOV	BX,DS:EndBuf		; Shell sort
	SUB	BX,OFFSET DskBuf
Sort10:	MOV	CX,SIZE FilEnt
	MOV	AX,BX
	XOR	DX,DX
	DIV	CX
	SHR	AX,1
	MUL	CX
	MOV	BX,AX
	OR	BX,BX
	JE	Sort15
	MOV	CX,DS:EndBuf
Sort11:	XOR	DX,DX
	MOV	SI,OFFSET DskBuf
Sort12:	LEA	DI,[SI+BX]
	CMP	DI,CX
	JAE	Sort14
	PUSH	CX
	CALL	CmpPar
	JNC	Sort13
	MOV	DX,DI
	MOV	DI,DS:EndFul
	MOV	CX,SIZE FilEnt
	PUSH	SI
	PUSH	CX
	REP	MOVSB
	POP	CX
	POP	DI
	PUSH	DI
	MOV	SI,DX
	PUSH	CX
	REP	MOVSB
	POP	CX
	MOV	SI,DS:EndFul
	MOV	DI,DX
	REP	MOVSB
	POP	SI
Sort13:	POP	CX
	ADD	SI,SIZE FilEnt
	JMP	Sort12
Sort14:	MOV	CX,DX
	OR	DX,DX
	JNE	Sort11
	JMP	Sort10
Sort15:	MOV	DI,OFFSET DskBuf	; Unconvert all
Sort20:	MOV	SI,DI
	MOV	CX,8
	MOV	BX,SI
Sort21:	LODSB
	CMP	AL,' '
	JE	Sort22
	MOV	BX,SI
Sort22:	LOOP	Sort21
	MOV	AL,'.'
	MOV	AH,DS:[DI+9]
	MOV	DX,DS:[DI+10]
	MOV	DS:[BX],AX
	MOV	DS:[BX+2],DX
	MOV	SI,BX
	INC	SI
	MOV	CX,3
Sort23:	LODSB
	CMP	AL,' '
	JE	Sort24
	MOV	BX,SI
Sort24:	LOOP	Sort23
	MOV	BYTE PTR DS:[BX],0
	ADD	DI,SIZE FilEnt
	CMP	DI,DS:EndBuf
	JB	Sort20
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	ES
	RET
Sort	ENDP

CmpPar	PROC	NEAR
	MOV	AL,DS:SortTyp
	CMP	AL,4
	JB	CmpPr0
	MOV	AX,DS:[DI+22]
	CMP	AX,DS:[SI+22]
	RET
CmpPr0:	MOV	CL,DS:[SI+13]
	MOV	CH,DS:[DI+13]
	TEST	CL,10h
	JNZ	CmpPr1
	TEST	CH,10h
	JZ	CmpPr2
	STC
	RET
CmpPr1:	TEST	CH,10h
	JZ	CmpPr9
	CMP	BYTE PTR DS:[SI],'.'
	JE	CmpPr9
	CMP	BYTE PTR DS:[DI],'.'
	STC
	JE	CmpPr9
	JMP	CmpPr4
CmpPr2:	TEST	CL,4
	JNZ	CmpPr3
	TEST	CH,4
	JZ	CmpPr4
	STC
	RET
CmpPr3:	TEST	CH,4
	JZ	CmpPr9
CmpPr4:	CMP	AL,0
        JNE	CmpPr6
CmpPr5:	PUSH	SI
	PUSH	DI
	XCHG	SI,DI
	MOV	CX,8
	REPE	CMPSB
	POP	DI
	POP	SI
	JNE	CmpPr9
	CALL	CmpExt
	RET
CmpPr6:	CMP	AL,1
	JNE	CmpPr7
	CALL	CmpExt
	JE	CmpPr5
	RET
CmpPr7:	CMP	AL,2
	JNE	CmpPr8
	MOV	AX,DS:[SI+16]
	CMP	AX,DS:[DI+16]
	JNE	CmpPr9
	MOV	AX,DS:[SI+14]
	CMP	AX,DS:[DI+14]
	JE	CmpPr5
	RET
CmpPr8:	MOV	AX,DS:[SI+20]
	CMP	AX,DS:[DI+20]
	JNE	CmpPr9
	MOV	AX,DS:[SI+18]
	CMP	AX,DS:[DI+18]
	JE	CmpPr5
CmpPr9:	RET
CmpPar	ENDP

CmpExt	PROC	NEAR
	PUSH	SI
	PUSH	DI
	XCHG	SI,DI
	ADD	SI,9
	ADD	DI,9
	MOV	CX,3
	REPE	CMPSB
	POP	DI
	POP	SI
	RET
CmpExt	ENDP

;----------------------- Find file ---------------------
;			ES:DI - File
;			   DS - WCBseg
;	Return :	   SI - Address of file , CF=1
;				Not found ,       CF=0
FnFile	PROC	NEAR
	MOV	SI,OFFSET DskBuf
FnFil1:	CMP	SI,DS:EndBuf
	JAE	FnFil3
	PUSH	SI
	PUSH	DI
FnFil2:	CMPSB
	JNE	FnFil4
	CMP	BYTE PTR DS:[SI-1],0
	JNE	FnFil2
	POP	DI
	POP	SI
	STC
FnFil3:	RET
FnFil4:	POP	DI
	POP	SI
	ADD	SI,SIZE FilEnt
	JMP	FnFil1
FnFile	ENDP

;--------------- Output path in the middle -------------
;			   DS - WCBseg
	.CONST
Tree_S	DB	' Tree ',0
Info_S	DB	' Info ',0
Progs_S	DB	' Progs',0

	.DATA
PutPt_P	DB	Back_C, Cursr_C

	.CODE
PutPath	PROC	NEAR
	PUSH	DS
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	CALL	BegBX
	ADD	BL,20
	MOV	BP,OFFSET PutPt_P
	MOV	AX,DS
	CMP	AX,ES:WCBcrn
	JNE	PutPt1
	INC	BP
PutPt1:	CMP	DS:WinTyp,0
	JE	PutPt2
	MOV	SI,OFFSET Tree_S
	CMP	DS:WinTyp,2
	JA	PutPt7
	JNE	PutPt6
	MOV	SI,OFFSET Info_S
	JMP	PutPt6
PutPt2:	CALL	PtPt10
	MOV	AL,38
	CMP	BH,0
	JNE	PutPt5
	CMP	ES:Clock,0
	JZ	PutPt5
	MOV	CX,DS
	CMP	CX,ES:WCB2seg
	JNE	PutPt5
	MOV	AX,1C20h
	CMP	ES:Country.TimeFmt,0
	JE	PutPt3
	MOV	AX,1E21h
PutPt3:	MOV	CX,DI
	SUB	CX,SI
	CMP	CL,AH
	JB	PutPt5
	CMP	CL,AL
	JBE	PutPt4
	MOV	CL,AL
PutPt4:	SUB	CL,AH
	SHR	CL,1
	INC	CX
	SUB	BL,CL
PutPt5:	CBW
	DEC	AX
	INC	SI
	CALL	ShrtLin
	MOV	SI,OFFSET Var_S
PutPt6:	PUSH	CS
	POP	DS
	CALL	CntLine
PutPt7:	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	POP	DS
	RET
PutPath	ENDP

PtPt10	PROC	NEAR
	MOV	DI,OFFSET TmpErr+1
	PUSH	DI
	MOV	SI,OFFSET WinPath
	MOV	AL,' '
	STOSB
	MOV	CX,LenPath
	CALL	MvzLine
	MOV	SI,OFFSET DskMas
	CMP	BYTE PTR DS:[SI],0
	JE	PtPt11
	MOV	CX,13
	CALL	AddFile
	CALL	MvzLine
	POP	SI
	CALL	LetLin
	PUSH	SI
PtPt11:	CMP	BYTE PTR DS:MasTyp,0
	JZ	PtPt12
	PUSH	DS
	PUSH	CS
	POP	DS
	MOV	SI,OFFSET Progs_S
	MOV	CX,25
	CALL	MvzLine
	POP	DS
PtPt12:	MOV	AX,' '
	STOSW
	DEC	DI
	POP	SI
	RET
PtPt10	ENDP

;-------------------- Output file name -----------------
;			DS:SI - address of disk buffer
;			ES:DI - address of screen buffer
;			   AH - color
PutName	PROC	NEAR
	PUSH	SI
	PUSH	CX
	PUSH	BX
	MOV	CX,12
	CMP	BYTE PTR DS:[SI],'.'
	JE	PutNm3
	MOV	BL,DS:[SI+13]
	MOV	BH,'.'
	MOV	CX,8
	CALL	PutNm5
	MOV	AL,' '
	TEST	BL,6
	JZ	PutNm1
	MOV	AL,'°'
PutNm1:	CMP	ES:ColTyp,2
	JNE	PutNm2
	TEST	BL,40H
	JZ	PutNm2
	MOV	AL,'Þ'
	TEST	BL,6
	JZ	PutNm2
	MOV	AL,'Û'
PutNm2:	STOSW
	MOV	CX,3
	LODSB
	CMP	AL,0
	JNE	PutNm3
	DEC	SI
PutNm3:	MOV	BH,0
	CALL	PutNm5
	POP	BX
	POP	CX
	POP	SI
	RET
PutName	ENDP

PutNm5	PROC	NEAR
	LODSB
	CMP	AL,0
	JE	PutNm6
	CMP	AL,BH
	JNE	PutNm7
PutNm6:	DEC	SI
	MOV	AL,' '
PutNm7:	STOSW
	LOOP	PutNm5
	RET
PutNm5	ENDP

;--------------- Output size, date & time --------------
;			DS:SI - address of disk buffer
;			ES:DI - address of screen buffer
;			   AH - color
SzDtTm	PROC	NEAR
	INC	DI
	MOV	AL,AH
	STOSB
	CALL	PutSize
	INC	DI
	MOV	AL,AH
	STOSB
	CALL	PutDat0
	INC	DI
	MOV	AL,AH
	STOSB
	CALL	PutTim0
	RET
SzDtTm	ENDP

;----------------------- Output size -------------------
;			DS:SI - address of disk buffer
;			ES:DI - address of screen buffer
;			   AH - color
	.CONST
UpDir	DB	'UP--DIR'
SubDir	DB	'SUB-DIR'

	.CODE
PutSize	PROC	NEAR
	PUSH	SI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	TEST	BYTE PTR DS:[SI+13],10h
	JZ	PutSz2
	MOV	DX,OFFSET UpDir
	CMP	BYTE PTR DS:[SI],'.'
	JE	PutSz1
	CMP	BYTE PTR DS:SizDir,0
	JNZ	PutSz2
	MOV	DX,OFFSET SubDir
PutSz1:	MOV	SI,DX
	MOV	CX,9
	CALL	MvsBwc
	JMP	PutSz4
PutSz2:	PUSH	DI
	MOV	CX,DS:[SI+18]
	MOV	BX,DS:[SI+20]
	MOV	SI,OFFSET TmpBuf
	MOV	DI,SI
	MOV	AL,2
	CALL	Number
	MOV	DI,SI
	CMP	BYTE PTR ES:[SI],' '
	JE	PutSz3
	MOV	CX,10
	MOV	AL,'9'
	REP	STOSB
PutSz3:	POP	DI
	INC	SI
	MOV	CX,9
	CALL	MvsBwe
PutSz4:	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	SI
	RET
PutSize	ENDP

;---------------------- Output date --------------------
;			DS:SI - address of disk buffer
;			ES:DI - address of screen buffer
;			   AH - color
PutDat0	PROC	NEAR
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	MOV	AX,DS:[SI+16]
	CALL	PutDate
	POP	AX
	PUSH	AX
	MOV	AL,BL
	STOSW
	MOV	AL,BH
	STOSW
	MOV	AL,ES:Country.DateSep
	STOSW
	PUSH	AX
	MOV	AL,DL
	STOSW
	MOV	AL,DH
	STOSW
	POP	AX
	STOSW
	MOV	AL,CL
	STOSW
	MOV	AL,CH
	STOSW
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	RET
PutDat0	ENDP

PutDate	PROC	NEAR
	PUSH	AX
	AND	AX,1Fh
	CALL	HexDec
	MOV	DX,AX
	POP	AX
	MOV	CL,5
	SHR	AX,CL
	PUSH	AX
	AND	AX,0Fh
	CALL	HexDec
	MOV	BX,AX
	POP	AX
	MOV	CL,4
	SHR	AX,CL
	AND	AX,7Fh
	ADD	AL,80
	MOV	CL,100
	DIV	CL
	MOV	CL,AH
	ADD	AL,19
	CALL	HexDec
	XCHG	CX,AX
	CALL	HexDec
	CMP	ES:Country.DateFmt,1
	JE	PutDt1
	JB	PutDt2
	XCHG	DX,AX
PutDt1:	XCHG	DX,BX
PutDt2:	XCHG	CX,AX
	JA	PutDt3
	CMP	BL,'0'
	JNE	PutDt3
	MOV	BL,' '
PutDt3:	RET
PutDate	ENDP

BufDate	PROC	NEAR
	CMP	ES:Country.DateFmt,1
	JBE	BufDt1
	STOSW
BufDt1:	XCHG	AX,BX
	STOSW
	MOV	AL,ES:Country.DateSep
	STOSB
	PUSH	AX
	MOV	AX,DX
	STOSW
	POP	AX
	STOSB
	JA	BufDt2
	MOV	AX,BX
	STOSW
BufDt2:	MOV	AX,CX
	STOSW
	RET
BufDate	ENDP

;---------------------- Output time --------------------
;			DS:SI - address of disk buffer
;			ES:DI - address of screen buffer
;			   AH - color
PutTim0	PROC	NEAR
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	MOV	AX,DS:[SI+14]
	CALL	PutTime
	POP	AX
	PUSH	AX
	CMP	CH,' '
	JNE	PutTm1
	MOV	AL,CH
	STOSW
PutTm1:	MOV	AL,BL
	STOSW
	MOV	AL,BH
	STOSW
	MOV	AL,ES:Country.TimeSep
	STOSW
	MOV	AL,DL
	STOSW
	MOV	AL,DH
	STOSW
	CMP	CH,' '
	JE	PutTm2
	MOV	AL,CH
	STOSW
PutTm2:	POP	AX
	POP	BX
	POP	CX
	POP	DX
	RET
PutTim0	ENDP

PutTime	PROC	NEAR
	PUSH	AX
	AND	AX,1Fh
	ADD	AX,AX
	CALL	HexDec
	MOV	BX,AX
	POP	AX
	MOV	CL,5
	SHR	AX,CL
	PUSH	AX
	AND	AX,3Fh
	CALL	HexDec
	MOV	DX,AX
	POP	AX
	MOV	CL,6
	SHR	AX,CL
	AND	AX,1Fh
	MOV	CH,' '
	CMP	ES:Country.TimeFmt,0
	JNE	PutTm6
	MOV	CL,12
	DIV	CL
	OR	AH,AH
	JNE	PutTm5
	MOV	AH,12
PutTm5:	CMP	AL,0
	MOV	AL,AH
	MOV	CH,'a'
	JE	PutTm6
	MOV	CH,'p'
PutTm6:	CALL	HexDec
	CMP	AL,'0'
	JNE	PutTm7
	MOV	AL,' '
PutTm7:	XCHG	BX,AX
	RET
PutTime	ENDP

BufTime	PROC	NEAR
	XCHG	AX,BX
	STOSW
	MOV	AL,ES:Country.TimeSep
	STOSB
	PUSH	AX
	MOV	AX,DX
	STOSW
	POP	AX
	STOSB
	MOV	AX,BX
	STOSW
	MOV	AL,CH
	STOSB
	RET
BufTime	ENDP

HexDec	PROC	NEAR
	PUSH	CX
	MOV	AH,0
	MOV	CL,10
	DIV	CL
	ADD	AX,'00'
	POP	CX
	RET
HexDec	ENDP

;--------------- Correct first parameter ---------------
;			   DS - WCBseg
CorPar	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	CMP	DS:WinTyp,1
	JA	CorPr5
	CALL	AdrNum
	CMP	SI,DI
	JAE	CorPr1
	MOV	DI,SI
CorPr1:	MOV	CL,DS:WinTyp
	OR	CL,DS:BrfFul
	JNZ	CorPr2
	MOV	CX,AX
	ADD	AX,CX
	ADD	AX,CX
CorPr2:	DEC	AX
	MOV	CX,SI
	SUB	CX,DI
	SUB	CX,AX
	JBE	CorPr3
	ADD	DI,CX
CorPr3:	SUB	DX,DI
	SUB	AX,DX
	JBE	CorPr4
	SUB	DI,AX
	JAE	CorPr4
	XOR	DI,DI
CorPr4:	XCHG	SI,DI
	CALL	NumAdr
	XCHG	SI,DI
	CALL	NumAdr
	CALL	ChgAdr
CorPr5:	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	RET
CorPar	ENDP

;--------------- Set cursor in main window -------------
;			   DS - WCBseg
;			 AL=0 - reset cursor
;			 AL=1 - set cursor
SetCur	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	MOV	AH,Back_C
	CMP	AL,0
	JZ	SetCr0
	MOV	AH,Cursr_C
SetCr0:	PUSH	AX
	CALL	AdrNum
	POP	AX
	MOV	CX,SI
	SUB	CX,DI
	CMP	DS:WinTyp,1
	JA	SetCr3
	JE	SetCr4
	MOV	SI,DS:CurAdr
	CMP	SI,DS:EndBuf
	JAE	SetCr3
	PUSH	AX
	MOV	AX,BX
	CALL	BegBX
	ADD	BX,201h
	MOV	DL,38
	CMP	BYTE PTR DS:BrfFul,0
	JNZ	SetCr1
	XCHG	AX,CX
	DIV	CL
	MOV	CL,AH
	MOV	AH,13
	MUL	AH
	ADD	BL,AL
	MOV	DL,12
SetCr1:	ADD	BH,CL
	POP	AX
	CALL	BasAdr
	MOV	CL,DS:[SI+13]
	LEA	SI,[DI+16]
	MOV	BYTE PTR ES:[SI],' '
	TEST	CL,6
	JZ	SetCr2
	MOV	BYTE PTR ES:[SI],'°'
SetCr2:	TEST	CL,40h
	JZ	SetCr8
	CMP	ES:ColTyp,2
	JNE	SetCr7
	MOV	BYTE PTR ES:[SI],'Þ'
	TEST	CL,6
	JZ	SetCr7
	MOV	BYTE PTR ES:[SI],'Û'
	JMP	SetCr7
SetCr3:	JMP	SetCr9
SetCr4:	CMP	SI,ES:TreeBuf.TreeLen
	JAE	SetCr3
	CALL	BegBX
	ADD	BX,118h
	ADD	BH,CL
	MOV	SI,ES:TreAdr
	MOV	CL,ES:[SI].DirLevel
	CMP	CL,11
	JA	SetCr5
	SUB	BL,17h
	ADD	BL,CL
	ADD	BL,CL
	CMP	ES:TreeTyp,0
	JZ	SetCr5
	ADD	BL,CL
SetCr5:	CALL	BasAdr
	MOV	BYTE PTR ES:[DI],' '
	MOV	DL,3
	CMP	SI,OFFSET TreeBuf.TreList
	JBE	SetCr6
	MOV	DL,14
	CMP	AL,0
	JNZ	SetCr6
	PUSH	AX
	MOV	AH,Back_C
	CALL	Color
	MOV	AL,'Ä'
	STOSW
	POP	AX
	DEC	DX
SetCr6:	CMP	SI,ES:TreCrnt
	JNE	SetCr8
SetCr7:	MOV	AH,Slct_C
	CMP	AL,0
	JZ	SetCr8
	MOV	AH,CurSl_C
SetCr8:	CALL	Color
	MOV	DH,1
	MOV	CL,DL
	MOV	CH,0
	MOV	AL,AH
	CALL	MvsColr
	INC	DX
	CALL	Window
SetCr9:	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	RET
SetCur	ENDP

;----------------------- Test info ---------------------
TstInfo	PROC	NEAR
	PUSH	DS
	PUSH	AX
	MOV	AL,DS:WinTyp
	CALL	InvWCB
	CMP	AL,DS:WinTyp
	JAE	TstIn1
	MOV	BYTE PTR DS:ChgFlg,1
TstIn1:	POP	AX
	POP	DS
	RET
TstInfo	ENDP

;-------------- BX files and DX directories ------------
;			   BX - number of files
;			   DX - number of directories
;			ES:DI - buffer
	.CONST
FilDi_S	DB	' files and  directoriesy'

	.CODE
FilDir	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	CX
	PUSH	AX
	PUSH	CS
	POP	DS
	OR	DX,DX
	JE	FilDi2
	PUSH	BX
	XOR	BX,BX
	MOV	CX,DX
	MOV	AL,1
	CALL	Number
	POP	BX
	MOV	SI,OFFSET FilDi_S
	MOV	CX,6
	REP	MOVSB
	CMP	DX,1
	JNE	FilDi1
	DEC	DI
FilDi1:	OR	BX,BX
	JE	FilDi3
	MOV	CX,5
	REP	MOVSB
FilDi2:	PUSH	BX
	MOV	CX,BX
	XOR	BX,BX
	MOV	AL,1
	CALL	Number
	POP	BX
	MOV	SI,OFFSET FilDi_S+11
	MOV	CX,12
	REP	MOVSB
	CMP	BX,1
	JNE	FilDi3
	SUB	DI,3
	MOVSB
FilDi3:	MOV	AL,0
	STOSB
	POP	AX
	POP	CX
	POP	SI
	POP	DS
	RET
FilDir	ENDP

;------------ Selected files and directories -----------
;	Return :	   DX - files
;			   BX - directories
TotSel	PROC	NEAR
	PUSH	SI
	MOV	SI,OFFSET DskBuf
	XOR	BX,BX
	XOR	DX,DX
TotSe1:	CMP	SI,DS:EndBuf
	JAE	TotSe3
	TEST	BYTE PTR DS:[SI+13],40h
	JZ	TotSe2
	INC	DX
	TEST	BYTE PTR DS:[SI+13],10h
	JZ	TotSe2
	DEC	DX
	INC	BX
TotSe2:	ADD	SI,SIZE FilEnt
	JMP	TotSe1
TotSe3:	POP	SI
	RET
TotSel	ENDP

;---------------------- Another WCB --------------------
InvWCB	PROC	NEAR
	PUSH	AX
	MOV	AX,DS
	MOV	DS,ES:WCB1seg
	CMP	AX,ES:WCB1seg
	JNE	In1WCB
	MOV	DS,ES:WCB2seg
In1WCB:	POP	AX
	RET
InvWCB	ENDP

;------------ Find iten in extension file ------------
;			DS:SI - file buffer
;	Return :	ES:SI - address of item
FndItem	PROC	NEAR
	MOV	AX,DS:[SI]
	CMP	AL," "
	JE	FndIt1
	CMP	AL,9
	JE	FndIt1
	CMP	AL,"'"
	JE	FndIt1
	CALL	IsCrLf
	JE	FndIt1
	CMP	AL,1Ah
	JE	FndIt2
	CLC
	RET
FndIt1:	CALL	IsCrLfS
	JE	FndItem
	DEC	SI
	CMP	AL,1Ah
	JNE	FndIt1
FndIt2:	STC
	RET
FndItem	ENDP

;------------ Find string in extension file ------------
;			DS:SI - file
;	Return :	ES:SI - string, 0 - if not found
FindExt	PROC	NEAR
	MOV	DI,OFFSET TmpErr
	MOV	CX,3
FnEx01:	LODSB
	CMP	AL,'.'
	JE	FnEx02
	CMP	AL,0
	JNE	FnEx01
	DEC	SI
FnEx02:	LODSB
	CMP	AL,0
	JNE	FnEx03
	DEC	SI
	MOV	AL,' '
FnEx03:	STOSB
	LOOP	FnEx02
	PUSH	DS
	PUSH	ES
	POP	DS
	MOV	SI,OFFSET MenuBuf
FnEx04:	CALL	FndItem
	JC	FnEx13
FnEx05:	MOV	DI,OFFSET TmpErr
	MOV	CX,3
FnEx06:	LODSB
	CMP	AL,':'
	JE	FnEx10
	CMP	AL,9
	JE	FnEx11
	CMP	AL,'*'
	JE	FnEx09
	CMP	AL,'?'
	JE	FnEx08
	CALL	LowCase
	CMP	AL,DS:[DI]
	JNE	FnEx12
FnEx08:	INC	DI
	LOOP	FnEx06
FnEx09:	CALL	IsCrLfS
	JE	FnEx04
	DEC	SI
	CMP	AL,1Ah
	JE	FnEx13
	CMP	AL,':'
	JNE	FnEx09
	POP	DS
	RET
FnEx10:	DEC	SI
FnEx11:	MOV	AL,' '
	REPE	SCASB
	JE	FnEx09
FnEx12:	CALL	FndIt1
	JNC	FnEx05
FnEx13:	XOR	SI,SI
	POP	DS
	STC
	RET
FindExt ENDP

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
	CMP	DS:WinLeav,0
	JNZ	Leave1
	MOV	AX,1680h
	INT	2Fh
	CMP	AL,80h
	JNZ	Leave1
	MOV	DS:WinLeav,1
Leave1:	CMP	DS:DESQvie,0
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

;-------------------- Search name ----------------------
;			   SI - current address
;			   BX - begin
;			   BP - end
;			   DX - add
;			   AX - key code
;	Return :	Z = 0 - not found
;			Z = 1 - found
;			   SI - new address
Search	PROC	NEAR
	PUSH	SI
	CMP	AL,0Ah
	JNE	Srch01
	ADD	SI,DX
	CMP	SI,BP
	JB	Srch01
	MOV	SI,BX
Srch01:	PUSH	SI
	PUSH	DS
	PUSH	ES
	POP	DS
	MOV	SI,OFFSET MenuBuf+228
	MOV	DI,OFFSET MenuBuf+208
	MOV	CX,15
	REP	MOVSB
	POP	DS
	CMP	AL,0Ah
	JE	Srch02
	PUSH	DX
	MOV	CX,12
	MOV	SI,OFFSET MenuBuf+210
	MOV	DX,WORD PTR ES:[MenuBuf+208]
	CALL	EdLin
	MOV	WORD PTR ES:[MenuBuf+208],DX
	POP	DX
Srch02:	PUSH	DS
	PUSH	ES
	POP	DS
	MOV	SI,OFFSET MenuBuf+210
	MOV	DI,OFFSET MenuBuf
	MOV	CX,13
	PUSH	DI
	CALL	MvzLine
	MOV	CX,DI
	POP	DI
	SUB	CX,DI
	POP	DS
	MOV	SI,DI
	CALL	PathLin
	POP	SI
	POP	AX
	JCXZ	Srch06
	CMP	BP,BX
	JBE	Srch07
Srch03:	PUSH	SI
	PUSH	DI
	PUSH	CX
	PUSH	AX
Srch04:	LODSB
	CALL	UpCase
	SCASB
	LOOPE	Srch04
	POP	AX
	POP	CX
	POP	DI
	POP	SI
	JE	Srch06
	ADD	SI,DX
	CMP	SI,BP
	JB	Srch05
	MOV	SI,BX
Srch05:	CMP	SI,AX
	JNE	Srch03
	JMP	Srch07
Srch06:	PUSH	DS
	PUSH	ES
	POP	DS
	PUSH	SI
	MOV	SI,OFFSET MenuBuf+208
	MOV	DI,OFFSET MenuBuf+228
	MOV	CX,15
	REP	MOVSB
	POP	SI
	POP	DS
Srch07:	CMP	SI,AX
	RET
Search	ENDP

;----------------- Check file panel --------------------
FilePan	PROC	NEAR
	CMP     DS:Visible,0
	JZ      FilPn1
	CMP     DS:WinTyp,1
FilPn1:	RET
FilePan	ENDP
