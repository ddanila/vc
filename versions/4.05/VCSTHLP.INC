;-------------------------------------------------------
;		Version :	09.05.95
;-------------------------------------------------------

BegHlp	EQU	55			; First topic in help
NumHlp	EQU	7			; Number of entries

HlpData	STRUC
HlpMode	DW	?
HlpSize	DW	?
HlpLins	DW	?
	DW	?
HlpBkLn	DW	?
HlpOffs	DD	?
HlpFrst	DW	?
HlpCur	DW	?
	DB	8 DUP(?)
HlpInfo	LABEL	BYTE
HlpData	ENDS

	.CONST
Help_F	DB	'VC.HLP',0
HlpHead	DB	HelpVer ,0
HlpEr_S	DB	'Error reading data from the help file.',13,10,'$'
Help_S	DB	' Help ',0
HlEr1_S	DB	'Help contents is not available',0

	.DATA
HlpMn_P	DB	Help_C, HlpRe_C, HlpRe_C
Help_P	DB	Help_C, HlpRe_C, HlpBo_C, HlpUn_C
HlpMn_D	DB	6, 20
	DB	0
Help_T	DW	4800h, Help30
	DW	5000h, Help32
	DW	4700h, Help35
	DW	4F00h, Help33
	DW	4900h, Help36
	DW	5100h, Help37
	DW	000Dh, Help40
	DW	001Bh, Help40
	DW	4400h, Help40
	DW	0
HlpEr_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	3, 0
	DW	BoxBuf
	DW	Help_S
	DW	HlEr1_S
	DW	OkMn_S
HlpEr_B	BOXHD	< HlpEr_P, 928h, 0, 2, 0FFh, 0FFh, 2 >
	BOXMN	< 4, 0,0,0,0, 326h, TmpErr, OkMn_D >
	DB	0

	.CODE
RdHelp	PROC	NEAR
	MOV	SI,OFFSET Help_F	; Open file
	MOV	CX,13
	REP	MOVSB
	MOV	AX,3D40h
	INT	21h
	JC	RdHlp2
	MOV	BX,AX
	XOR	CX,CX			; Check header
	MOV	DX,8
	CALL	SeekDsk
	JC	RdHlp1
	MOV	CX,13
	MOV	DX,OFFSET HelpBuf[16]
	CALL	ReadDsk
	JC	RdHlp1
	MOV	DI,DX
	MOV	SI,OFFSET HlpHead
	REPE	CMPSB
	JNE	RdHlp1
	XOR	CX,CX			; Read pointer to index
	MOV	DX,194h
	CALL	SeekDsk
	JC	RdHlp1
	MOV	CX,4
	MOV	DX,OFFSET HelpBuf.HlpOffs
	CALL	ReadDsk
	JC	RdHlp1
	CALL	DeCode
	MOV	DX,DS:HelpBuf.HlpOffs.Lo ; Read index header
	MOV	CX,DS:HelpBuf.HlpOffs.Hi
	CALL	SeekDsk
	JC	RdHlp1
	MOV	CX,OFFSET HlpInfo
	MOV	DX,OFFSET HelpBuf
	CALL	ReadDsk
	JC	RdHlp1
	CALL	DeCode
	MOV	CX,DS:HelpBuf.HlpSize	; Read index
	MOV	AX,0FEE0h
	SUB	AX,OFFSET HelpBuf
	CMP	AX,CX
	JNC	RdHlp3
RdHlp1:	MOV	AH,3Eh
	INT	21h
RdHlp2:	MOV	DX,OFFSET HlpEr_S
	MOV	AH,9
	INT	21h
	JMP	RdHlp5
RdHlp3:	MOV	DX,OFFSET HelpBuf.HlpInfo
	CALL	ReadDsk
	JC	RdHlp1
	CALL	DeCode
	CMP	DS:HelpBuf.HlpMode,0
	JNE	RdHlp1
	MOV	DI,DS:HelpBuf.HlpLins	; Read pages information
	SUB	DI,BegHlp-1
	JBE	RdHlp1
	MOV	SI,6*(BegHlp-1)
	MOV	DX,DS:[SI].HelpBuf.HlpInfo[2].Lo
	MOV	CX,DS:[SI].HelpBuf.HlpInfo[2].Hi
	CALL	SeekDsk
	JC	RdHlp1
	MOV	DX,OFFSET HelpBuf+2
	MOV	CX,0FEE0h
	SUB	CX,DX
	CMP	DI,NumHlp
	JBE	RdHlp4
	MOV	SI,6*(BegHlp-1+NumHlp)
	MOV	AX,DS:[SI].HelpBuf.HlpInfo[2].Lo
	MOV	DI,DS:[SI].HelpBuf.HlpInfo[2].Hi
	MOV	SI,6*(BegHlp-1)
	SUB	AX,DS:[SI].HelpBuf.HlpInfo[2].Lo
	SBB	DI,DS:[SI].HelpBuf.HlpInfo[2].Hi
	JNE	RdHlp4
	CMP	CX,AX
	JBE	RdHlp4
	MOV	CX,AX
RdHlp4:	MOV	AH,3Fh
	INT	21h
	JC	RdHlp1
	MOV	WORD PTR DS:HelpBuf[0],AX
	CALL	DeCode
	MOV	AH,3Eh
	INT	21h
	JC	RdHlp2
RdHlp5:	RET
RdHelp	ENDP

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
	CALL	HidCur
	MOV	AL,DS:HlpPage		; Help page offset
	MOV	AH,0
	PUSH	AX
	CMP	AL,0FFh
	JE	Help03
	MOV	CX,AX
	MOV	SI,OFFSET HelpBuf
	LODSW
	ADD	AX,SI
	MOV	DX,AX
	MOV	AX,SI
Help01:	MOV	SI,AX
	ADD	AX,OFFSET HlpInfo
	JC	Help02
	CMP	AX,DX
	JA	Help02
	ADD	AX,DS:[SI].HlpSize
	JC	Help02
	CMP	AX,DX
	JA	Help02
	LOOP	Help01
	CMP	DS:[SI].HlpMode,0
	JNZ	Help04
Help02:	MOV	BP,OFFSET HlpEr_B
	CALL	Dialog
Help03:	JMP	Help41
Help04:	MOV	BP,SI			; Output window
	MOV	BX,204h
	MOV	DH,DS:Lines
	SUB	DH,3
	MOV	DL,74
	MOV	SI,OFFSET BoxBuf
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
	PUSH	DI
	ADD	DI,2*(80+33)
	MOV	SI,OFFSET Ok_S
	MOV	CX,6
	CALL	MvsBwc
	POP	DI
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
	PUSH	BX
	LEA	SI,[BP].HlpInfo
	ADD	BX,205h
	CALL	HlpLine
	POP	BX
	CALL	Window
	XOR	AX,AX			; Init vars
	MOV	DS:WinMous,AL
	MOV	DS:MousEnt,AL
	DEC	AX
	MOV	DS:HlpPage,AL
	MOV	DS:MousTim.Hi,AX
	MOV	DS:[BP].HlpFrst,0FFF0h
	XOR	DI,DI

Help10:	MOV	AX,DI			; Check first line
	XCHG	DI,DS:[BP].HlpFrst
	SUB	AX,DI
	JE	Help20
	PUSH	AX			; Output lines
	PUSH	BX
	MOV	CX,DS:[BP].HlpFrst
	INC	CX
	LEA	DI,[BP].HlpInfo
	MOV	AL,0
Help11:	PUSH	CX
	MOV	CH,0FFh
	REPNE	SCASB
	POP	CX
	LOOP	Help11
	MOV	SI,DI
	MOV	CL,DH
	SUB	CL,9
	ADD	BX,405h
Help12:	MOV	DI,SI
	SUB	DI,BP
	SUB	DI,OFFSET HlpInfo
	CMP	DI,DS:[BP].HlpSize
	JB	Help13
	DEC	SI
Help13:	CALL	HlpLine
	INC	BH
	LOOP	Help12
	POP	BX
	POP	CX
	PUSH	DX			; Output screen
	PUSH	BX
	MOV	AL,DS:Lines
	SUB	AL,12
	ADD	BX,405h
	MOV	DH,AL
	DEC	AX
	MOV	AH,6
	CMP	CX,1
	JE	Help14
	CMP	CX,-1
	JNE	Help15
	MOV	AX,700h
Help14:	MOV	CX,BX
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
Help15:	MOV	DL,62
	CALL	Window
	POP	BX
	POP	DX

Help20:	CALL	GetMous			; Input
	JNZ	Help22
	MOV	DS:WinMous,0
	MOV	DS:MousEnt,0
	PUSH	BP
	PUSH	DX
	PUSH	BX
	ADD	BH,DH
	SUB	BH,4
	ADD	BL,13
	MOV	DX,130h
	MOV	BP,OFFSET HlpMn_P
	MOV	SI,OFFSET HlpMn_D
	MOV	CL,0
	MOV	AL,2
	CALL	MinMnu
	POP	BX
	POP	DX
	POP	BP
	CMP	AH,0FCh
	JE	Help21
	MOV	DI,DS:[BP].HlpFrst
	MOV	SI,OFFSET Help_T
	CALL	Case
	JMP	Help20

Help21:	CALL	GetMous			; Mouse support
Help22:	CALL	ClrKbd
	MOV	SI,CX
	TEST	AL,4
	JZ	Help24
	MOV	AL,'x'
	CALL	TypMous
Help23:	CALL	GetMous
	CALL	ClrKbd
	TEST	AL,4
	JNZ	Help23
	PUSH	AX
	MOV	AL,0
	CALL	TypMous
	POP	AX
	TEST	AL,3
	JZ	Help25
Help24:	PUSH	DX
	PUSH	BX
	ADD	BX,404h
	SUB	DX,90Ah
	CALL	InWind
	POP	BX
	POP	DX
	JNC	Help26
	CMP	DS:WinMous,0
	JNZ	Help26
	TEST	AL,1
	JZ	Help27
	CALL	GetMous
	JNZ	Help22
	MOV	CX,SI
	PUSH	DX
	SUB	DX,102h
	CALL	InWind
	POP	DX
	JNC	Help27
Help25:	JMP	Help40
Help26:	OR	DS:WinMous,1
	MOV	DI,DS:[BP].HlpFrst
	MOV	AL,DH
	SUB	AL,9
	CBW
	PUSH	CX
	MOV	CL,3
	DIV	CL
	POP	CX
	MOV	AH,BH
	ADD	AH,4
	ADD	AH,AL
	CMP	CH,AH
	JB	Help30
	MOV	AH,BH
	ADD	AH,DH
	SUB	AH,5
	SUB	AH,AL
	CMP	CH,AH
	JAE	Help32
Help27:	JMP	Help20

Help30:	OR	DI,DI			; Up
	JE	Help31
	DEC	DI
Help31:	JMP	Help10

Help32:	MOV	AL,DS:Lines		; Down
	SUB	AL,11
	CBW
	ADD	AX,DI
	CMP	AX,DS:[BP].HlpLins
	JAE	Help31
	INC	DI
	JMP	Help10

Help33:	MOV	DI,DS:[BP].HlpLins	; End
	MOV	AL,DS:Lines
	SUB	AL,11
Help34:	CBW
	SUB	DI,AX
	JAE	Help31

Help35:	XOR	DI,DI			; Home
	JMP	Help10

Help36:	OR	DI,DI			; PgUp
	JE	Help31
	MOV	AL,DS:Lines
	SUB	AL,13
	JMP	Help34

Help37:	MOV	AL,DS:Lines		; PgDn
	SUB	AL,12
	CBW
	MOV	SI,DS:[BP].HlpLins
	DEC	SI
	SUB	SI,AX
	JB	Help31
	CMP	DI,SI
	JAE	Help31
	DEC	AX
	ADD	DI,AX
	CMP	DI,SI
	JBE	Help31
	MOV	DI,SI
	JMP	Help10

Help40:	POP	SI			; Esc
	CALL	ResWin
	CALL	Window

Help41:	POP	AX			; Exit
	MOV	DS:HlpPage,AL
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

HlpLine	PROC	NEAR
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
HlpLine	ENDP

SeekDsk	PROC	NEAR
	PUSH	SI
	MOV	SI,DX
	MOV	AX,4200h
	INT	21h
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
	INT	21h
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