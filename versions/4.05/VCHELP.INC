;-------------------------------------------------------
;		Version :	09.05.95
;-------------------------------------------------------

HlpData	STRUC
	DB	HelpBuf DUP(?)
HlpHndl	DW	?
HlpPrms	LABEL	BYTE
HlpMode	DW	?
HlpSize	DW	?
HlpLins	DW	?
	DW	?
HlpBkLn	DW	?
HlpOffs	DD	?
HlpFrst	DW	?
HlpCur	DW	?
	DB	10 DUP(?)
HlpBkFr	DW	?
HlpInfo	LABEL	BYTE
HlpData	ENDS

	.CONST
Help_F	DB	'VC.HLP',0
HlpHead	DB	HelpVer ,0
Help_S	DB	' Help ',0
HlEr1_S	DB	'There was error reading data',0
HlEr2_S	DB	'from the help file.',0
HlMn1_S	DB	0FFh,21,'[ Help ]',0FFh,3,'[ Cancel ]',0
HlMn2_S	DB	0FFh, 7,'[ Next ]',0FFh,3,'[ Previous ]',0FFh,3,'[ Index ]',0FFh,3,'[ Cancel ]',0

	.DATA
HlpMn_P	DB	Help_C, HlpRe_C, HlpRe_C
Help_P	DB	Help_C, HlpRe_C, HlpBo_C, HlpUn_C
HlMn1_D	DB	 8, 14
	DB	10, 25
	DB	 0
HlMn2_D	DB	 8,  0
	DB	12, 11
	DB	 9, 26
	DB	10, 38
	DB	 0
Help_T	DW	4800h, Help50
	DW	5000h, Help53
	DW	4700h, Help55
	DW	4F00h, Help56
	DW	4900h, Help57
	DW	5100h, Help58
	DW	000Dh, Help60
	DW	001Bh, Help66
	DW	4400h, Help66
	DW	0
HlpEr_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 0
	DW	HlpInfo
	DW	Help_S
	DW	HlEr1_S
	DW	HlEr2_S
	DW	Ok_S
HlpEr_B	DW	HlpEr_P, 928h, 0
	DB	2, 0FFh, 0FFh, 2
	DB	4
	DB	0, 0, 0, 0
	DW	426h, TmpErr, OkMn_D
	DB	0

	.CODE
Help	PROC	NEAR
	PUSH	DS
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	PUSH	ES
	POP	DS
	MOV	AH,0Fh
	INT	10h
	MOV	AH,3
	INT	10h
	PUSH	DX
	MOV	AH,1
	XCHG	AH,DS:BlocEr
	MOV	AL,DS:HlpPage
	PUSH	AX
	CMP	AL,0FFh
	JE	Help02
	CALL	HidCur
	MOV	DX,OFFSET HelpBuf	; Reading
	MOV	SI,OFFSET Help_F
	MOV	DI,DX
	CALL	MainFil
	CALL	ChgDisk
	JC	Help02
	MOV	AL,40h
	CALL	OpenFil
	JC	Help02
	MOV	DS:HlpHndl,AX
	MOV	BX,AX
	XOR	CX,CX
	MOV	DX,8
	CALL	SeekDsk
	JC	Help01
	MOV	CX,13
	MOV	DX,OFFSET HelpBuf[16]
	CALL	ReadDsk
	JC	Help01
	PUSH	CS
	POP	DS
	MOV	DI,DX
	MOV	SI,OFFSET HlpHead
	REPE	CMPSB
	PUSH	ES
	POP	DS
	JNE	Help01
	XOR	CX,CX
	MOV	DX,194h
	CALL	SeekDsk
	JC	Help01
	MOV	CX,4
	MOV	DX,OFFSET HlpOffs
	CALL	ReadDsk
	JC	Help01
	CALL	DeCode
	MOV	DS:HlpBkLn,0
	CMP	DS:HlpPage,0
	JE	Help04
	CALL	RdEntry
	JNC	Help03
Help01:	MOV	BP,OFFSET HlpEr_B
	CALL	Dialog
	JMP	Help67
Help02:	JMP	Help68
Help03:	CMP	DS:HlpMode,0
	JNE	Help05
	MOV	AL,DS:HlpPage
	CBW
	DEC	AX
	CALL	Help70
Help04:	CALL	RdEntry
	JC	Help01
Help05:	MOV	BX,204h			; Output window
	MOV	DH,DS:Lines
	SUB	DH,3
	MOV	DL,74
	MOV	SI,OFFSET ErrBuf1
	CALL	SavWin
	PUSH	SI
	MOV	AH,Help_C
	CALL	Color
	CALL	MinBox
	PUSH	DX
	PUSH	BX
	CALL	BasAdr
	PUSH	DI
	ADD	DI,2*(80+33)
	MOV	SI,OFFSET Help_S
	MOV	CX,6
	CALL	MvsBwc
	ADD	BH,DH
	SUB	BH,5
	CALL	BasAdr
	MOV	CX,3
	MOV	DX,64
	MOV	AL,'Ä'
	MOV	BX,'Ç¶'
	CALL	BoxLine
	POP	DI
	ADD	DI,2*3*80
	CALL	BoxLine
	POP	BX
	POP	DX
	CALL	Window
	XOR	CX,CX
	MOV	DS:HlpBkFr,CX
	MOV	DS:HlpPage,0FFh
	MOV	DS:WinMous,CL
	MOV	DS:MousEnt,CL
	MOV	DS:MousTim.Hi,0FFFFh

Help06:	PUSH	BX			; Output first line & menu line
	PUSH	DX
	MOV	SI,OFFSET HlpInfo
	CMP	DS:HlpMode,0
	JNE	Help07
	PUSH	CS
	POP	DS
	MOV	SI,OFFSET Title_S
Help07:	ADD	BX,205h
	CALL	Help77
	MOV	DX,13Eh
	CALL	Window
	PUSH	CS
	POP	DS
	POP	DX
	MOV	SI,OFFSET HlMn2_S
	MOV	AX,ES:HlpOffs.Lo
	AND	AX,ES:HlpOffs.Hi
	INC	AX
	JNE	Help08
	MOV	CL,0
	MOV	SI,OFFSET HlMn1_S
Help08:	ADD	BH,DH
	SUB	BH,6
	CALL	Help77
	PUSH	ES
	POP	DS
	CALL	Help71
	MOV	DS:HlpFrst,0FFF0h
	POP	BX

Help10:	CMP	SI,DI			; Test parameters
	JAE	Help11
	MOV	DI,SI
Help11:	MOV	AL,DH
	SUB	AL,9
	CBW
	ADD	AX,DI
	CMP	SI,AX
	JB	Help12
	SUB	AX,DI
	MOV	DI,SI
	SUB	DI,AX
	INC	DI
Help12:	MOV	AX,DI
	XCHG	SI,DS:HlpCur
	XCHG	DI,DS:HlpFrst
	SUB	AX,DI
	JNE	Help14
	CMP	DS:HlpMode,0
	JNE	Help13
	CMP	SI,DS:HlpCur
	JNE	Help14
Help13:	JMP	Help25
Help14:	PUSH	CX			; Reset cursor
	PUSH	AX
	CMP	DI,0FFF0h
	JE	Help15
	PUSH	DX
	PUSH	BX
	MOV	AX,SI
	SUB	AX,DI
	ADD	BH,AL
	MOV	CX,SI
	CALL	Help72
	ADD	BX,405h
	CALL	Help77
	MOV	DX,13Eh
	CALL	Window
	POP	BX
	POP	DX
Help15:	POP	CX
	JCXZ	Help20
	PUSH	CX			; Output lines
	PUSH	BX
	MOV	CX,DS:HlpFrst
	CALL	Help72
	MOV	CL,DH
	SUB	CL,9
	MOV	CH,0
	ADD	BX,405h
Help16:	LEA	DI,[SI-HlpInfo]
	CMP	DI,DS:HlpSize
	JB	Help17
	DEC	SI
Help17:	CALL	Help77
	INC	BH
	LOOP	Help16
	POP	BX
	POP	CX
	PUSH	DX			; Output screen
	PUSH	BX
	CALL	Help71
	ADD	BX,405h
	MOV	DH,AL
	DEC	AX
	MOV	AH,6
	CMP	CX,1
	JE	Help18
	CMP	CX,-1
	JNE	Help19
	MOV	AX,700h
Help18:	MOV	CX,BX
	MOV	DL,61
	DEC	DH
	ADD	DX,CX
	PUSH	AX
	MOV	AH,Help_C
	CALL	Color
	MOV	BH,AH
	POP	AX
	PUSH	AX
	MOV	AL,1
	CALL	Int10m
	POP	AX
	MOV	BX,CX
	ADD	BH,AL
	MOV	DH,1
Help19:	MOV	DL,62
	CALL	Window
	POP	BX
	POP	DX
Help20:	CMP	DS:HlpMode,0		; Set cursor
	JNE	Help21
	PUSH	DX
	PUSH	BX
	MOV	AX,DS:HlpCur
	SUB	AX,DS:HlpFrst
	ADD	BH,AL
	ADD	BX,405h
	CALL	BasAdr
	MOV	AH,HlpRe_C
	CALL	Color
	MOV	AL,AH
	MOV	CX,62
	CALL	MvsColr
	MOV	DX,13Eh
	CALL	Window
	POP	BX
	POP	DX
Help21:	POP	CX

Help25:	PUSH	CX			; Input
	CALL	GetMous
	POP	CX
	JNZ	Help30
	MOV	DS:WinMous,0
	MOV	DS:MousEnt,0
	PUSH	DX
	PUSH	BX
	ADD	BH,DH
	SUB	BH,4
	ADD	BL,12
	MOV	DX,130h
	MOV	BP,OFFSET HlpMn_P
	MOV	SI,OFFSET HlMn1_D
	MOV	AX,DS:HlpOffs.Lo
	AND	AX,DS:HlpOffs.Hi
	INC	AX
	JE	Help26
	MOV	SI,OFFSET HlMn2_D
Help26:	MOV	AL,2
	CALL	MinMnu
	POP	BX
	POP	DX
	CMP	AH,0FCh
	JE	Help30
	MOV	SI,OFFSET Help_T
	CALL	Case
	JMP	Help25

Help30:	PUSH	CX			; Mouse support
	CALL	GetMous
Help31:	CALL	ClrKbd
	MOV	SI,CX
	TEST	AL,4
	JZ	Help33
	MOV	AL,'x'
	CALL	TypMous
Help32:	CALL	GetMous
	CALL	ClrKbd
	TEST	AL,4
	JNZ	Help32
	PUSH	AX
	MOV	AL,0
	CALL	TypMous
	POP	AX
	TEST	AL,3
	JZ	Help40
Help33:	PUSH	DX
	PUSH	BX
	ADD	BX,404h
	SUB	DX,90Ah
	CALL	InWind
	POP	BX
	POP	DX
	JNC	Help41
	CMP	DS:WinMous,0
	JZ	Help39
	MOV	AL,0
	CMP	DS:HlpMode,0
	JZ	Help35
Help34:	MOV	AL,DH
	SUB	AL,9
	CBW
	PUSH	CX
	MOV	CL,3
	DIV	CL
	POP	CX
Help35:	MOV	AH,BH
	ADD	AH,4
	ADD	AH,AL
	CMP	CH,AH
	JB	Help37
	MOV	AH,BH
	ADD	AH,DH
	SUB	AH,5
	SUB	AH,AL
	CMP	CH,AH
	JAE	Help38
Help36:	POP	CX
	JMP	Help25
Help37:	POP	CX
	JMP	Help50
Help38:	POP	CX
	JMP	Help53
Help39:	TEST	AL,1
	JZ	Help36
	CALL	GetMous
	JNZ	Help31
	MOV	CX,SI
	PUSH	DX
	SUB	DX,102h
	CALL	InWind
	POP	DX
	JNC	Help36
Help40:	POP	CX
	JMP	Help66
Help41:	OR	DS:WinMous,1
	CMP	DS:HlpMode,0
	JNZ	Help34
	PUSH	CX
	SUB	CH,BH
	SUB	CH,4
	MOV	CL,CH
	MOV	CH,0
	MOV	DI,DS:HlpFrst
	ADD	CX,DI
	MOV	BP,DS:HlpLins
	DEC	BP
	POP	SI
	XCHG	BP,CX
	CMP	BP,CX
	JBE	Help43
	MOV	SI,CX
Help42:	POP	CX
	JMP	Help10
Help43:	PUSH	DX
	PUSH	BX
	MOV	AX,SI
	MOV	SI,BP
	MOV	BH,AH
	ADD	BL,4
	SUB	DL,10
	MOV	DH,1
	CALL	DoublMo
	POP	BX
	POP	DX
	JC	Help42
	JE	Help36
	MOV	DS:MousEnt,0
	MOV	DS:MousTim.Hi,0FFFFh
	MOV	AL,0
	CALL	TypMous
	POP	CX
	JMP	Help60

Help50:	CALL	Help71			; Up
	MOV	AX,SI
	CMP	DS:HlpMode,0
	JE	Help51
	MOV	AX,DI
Help51:	OR	AX,AX
	JE	Help52
	DEC	AX
	MOV	SI,AX
Help52:	JMP	Help10

Help53:	CALL	Help71			; Down
	ADD	AX,DI
	CMP	DS:HlpMode,0
	JNE	Help54
	MOV	AX,SI
	INC	AX
Help54:	CMP	AX,DS:HlpLins
	JAE	Help52
	MOV	SI,AX
	JMP	Help10

Help55:	XOR	SI,SI			; Home
	JMP	Help10

Help56:	XOR	DI,DI			; End
	MOV	SI,DS:HlpLins
	OR	SI,SI
	JE	Help52
	DEC	SI
	JMP	Help10

Help57:	CALL	Help71			; PgUp
	CALL	PgUp
	JMP	Help10

Help58:	CALL	Help71			; PgDn
	MOV	BP,DS:HlpLins
	CALL	PgDn
	JMP	Help10

Help60:	CALL	Help71			; Enter
	MOV	AX,DS:HlpOffs.Lo
	AND	AX,DS:HlpOffs.Hi
	INC	AX
	JNE	Help61
	OR	CL,CL
	JNE	Help66
	CMP	DS:HlpMode,0
	JNE	Help52
	MOV	AX,DS:HlpCur
	JMP	Help63
Help61:	MOV	BYTE PTR DS:HlpMode,1
	CMP	CL,2
	JE	Help64
	JA	Help66
	CALL	RdEntry
	JC	Help65
	MOV	AX,DS:HlpCur
	OR	CL,CL
	JNE	Help62
	INC	AX
	CMP	AX,DS:HlpLins
	JB	Help63
	XOR	AX,AX
	JMP	Help63
Help62:	SUB	AX,1
	JAE	Help63
	MOV	AX,DS:HlpLins
	DEC	AX
Help63:	CALL	Help70
Help64:	CALL	RdEntry
	JC	Help65
	JMP	Help06

Help65:	MOV	BP,OFFSET HlpEr_B	; Error
	CALL	Dialog

Help66:	POP	SI			; Esc
	CALL	ResWin
	CALL	Window

Help67:	MOV	BX,DS:HlpHndl		; Exit
	MOV	AH,3Eh
	CALL	Intr21
Help68:	POP	AX
	MOV	DS:HlpPage,AL
	MOV	DS:BlocEr,AH
	POP	AX
	CALL	Cursor
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	POP	DS
	RET
Help	ENDP

Help70	PROC	NEAR
	ADD	AX,AX
	MOV	SI,AX
	ADD	SI,AX
	ADD	SI,AX
	MOV	AX,DS:[SI].HlpInfo[2].Lo
	MOV	DS:HlpOffs.Lo,AX
	MOV	AX,DS:[SI].HlpInfo[2].Hi
	MOV	DS:HlpOffs.Hi,AX
	RET
Help70	ENDP

Help71	PROC	NEAR
	MOV	SI,DS:HlpCur
	MOV	DI,DS:HlpFrst
	MOV	AL,DS:Lines
	SUB	AL,12
	CBW
	MOV	BP,AX
	RET
Help71	ENDP

Help72	PROC	NEAR
	MOV	SI,OFFSET HlpInfo
	INC	CX
	CMP	DS:HlpMode,0
	JNE	Help73
	DEC	CX
	MOV	AX,DS:HlpLins
	ADD	AX,AX
	ADD	SI,AX
	ADD	SI,AX
	ADD	SI,AX
Help73:	MOV	DI,SI
	MOV	AL,0
	JCXZ	Help75
Help74:	PUSH	CX
	MOV	CX,0FFFFh
	REPNE	SCASB
	POP	CX
	LOOP	Help74
Help75:	MOV	SI,DI
	RET
Help72	ENDP

Help77	PROC	NEAR
	PUSH	BP
	PUSH	DX
	PUSH	AX
	CALL	BasAdr
	MOV	BP,OFFSET Help_P
	SUB	DL,12
	PUSH	DI
	PUSH	CX
	MOV	CL,DL
	MOV	CH,0
	MOV	AH,CS:[BP]
	CALL	Color
	MOV	AL,' '
	REP	STOSW
	POP	CX
	POP	DI
	MOV	AL,DL
	MOV	BYTE PTR ES:[TmpErr+1],0
	MOV	BYTE PTR ES:[TmpErr+101],0
	MOV	DX,OFFSET Empty_S
	CALL	NgLine
	POP	AX
	POP	DX
	POP	BP
	RET
Help77	ENDP

RdEntry	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	MOV	SI,DS:HlpBkLn
	MOV	DI,DS:HlpBkFr
	CMP	DS:HlpMode,0
	JNE	RdEnt1
	MOV	AX,DS:HlpFrst
	MOV	DS:HlpBkFr,AX
	XOR	SI,SI
	XOR	DI,DI
RdEnt1:	MOV	BX,DS:HlpHndl
	MOV	DX,DS:HlpOffs.Lo
	MOV	CX,DS:HlpOffs.Hi
	CALL	SeekDsk
	JC	RdEnt2
	MOV	CX,1Ah
	MOV	DX,OFFSET HlpPrms
	CALL	ReadDsk
	JC	RdEnt2
	CALL	DeCode
	MOV	CX,DS:HlpSize
	CMP	CX,LenHelp-20h
	CMC
	JC	RdEnt2
	MOV	DX,OFFSET HlpInfo
	CALL	ReadDsk
	JC	RdEnt2
	CALL	DeCode
	MOV	AX,DS:HlpMode
	SUB	DS:HlpLins,AX
	MOV	DS:HlpFrst,DI
	MOV	DS:HlpCur,SI
RdEnt2:	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	RET
RdEntry	ENDP

SeekDsk	PROC	NEAR
	PUSH	SI
	MOV	SI,DX
	MOV	AX,4200h
	CALL	Intr21
	JC	SkDsk1
	XOR	SI,AX
	XOR	DX,CX
	OR	SI,DX
	JE	SkDsk1
	STC
SkDsk1:	POP	SI
	RET
SeekDsk	ENDP

ReadDsk	PROC	NEAR
	MOV	AH,3Fh
	CALL	Intr21
	JC	RdDsk1
	CMP	AX,CX
	JE	RdDsk1
	STC
RdDsk1:	RET
ReadDsk	ENDP

DeCode	PROC	NEAR
	PUSH	SI
	PUSH	CX
	MOV	SI,DX
DeCod1:	XOR	BYTE PTR DS:[SI],1Ah
	INC	SI
	LOOP	DeCod1
	POP	CX
	POP	SI
	RET
DeCode	ENDP
