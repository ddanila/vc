;-------------------------------------------------------
;		Version :	10.01.2000
;-------------------------------------------------------

LenFnf	EQU	LenStr+1

FndData	STRUC
	DB	TmpBuf + 100 DUP(?)
FndCur	DW	?			; Current line
FndFrst	DW	?			; First output line
FndNumb	DW	?			; Total number of lines
FndSize	DW	?			; Max. number of files
FndBg	DW	?			; Base of the window
FndSz	DW	?			; Size of the window
FndFls	DW	?			; Number of found files
FndMode	DB	?			; 0-auto scroll; 1-allow move cursor
FndDir	DB	?			; Not first file in subdirectory
FndMenu	DB	?			; Menu item
FndDrv	DB	?			; Drive letter
FndData	ENDS

	.CONST
Find0_S	DB	' Find File ',0
Find1_S	DB	'File name:',0
Find2_S	DB	'Containing:',0
Find3_S	DB	'Scanning:',0
Find5_S	DB	'^r[ Stop ]^r',0FFh,3,'[ Go to ]',0FFh,3,'[ Quit ]',0
Find6_S	DB	'[ New search ]',0FFh,3,'[ Go to ]',0FFh,3,'[ Quit ]',0
FnFl6_S	DB	' files',0
FnFl7_S	DB	' found.',0

	.DATA
Find_P	DB	Conf_C, CnfRe_C, CnfBo_C, CnfUn_C
FnFl_P	DB	Conf_C, CnfRe_C, CnfRe_C
Find_B	DW	Find_P, 0, 0
	DB	1, 0FFh, 32, 10h
	DB	1
	DB	1, 1, 0, 0
	DW	305h, FindStr, LenStr
	DB	1
	DB	0, 0, 1, 1
	DW	505h, SrchStr, 100h+LenStr
	DB	0
Find_T	DW	4800h, FnMv12
	DW	5000h, FnMv13
	DW	4700h, FnMv10
	DW	4F00h, FnMv11
	DW	4900h, FnMv14
	DW	5100h, FnMv15
	DW	000Dh, FnMv31
	DW	001Bh, FnMv32
	DW	4400h, FnMv32
	DW	0
Find5_D	DB	 8,  0
	DB	 9, 11
	DB	 8, 23
	DB	 0
Find6_D	DB	14,  0
	DB	 9, 17
	DB	 8, 29
	DB	 0

	.CODE
F67_00:	CALL	HidCur			; Alt F7 - file find
	CALL	PutMnu3
	MOV	BP,OFFSET Find_P
	MOV	BX,203h
	MOV	DH,ES:Lines
	SUB	DH,3
	MOV	DL,76
	MOV	SI,WinBuf
	CALL	SavWin
	MOV	AH,CS:[BP]
	CALL	Color
	CALL	MinBox
	PUSH	SI
	PUSH	CS
	POP	DS
	PUSH	BX
	INC	BH
	MOV	BL,40
	MOV	SI,OFFSET Find0_S
	CALL	CntLine
	POP	BX
F67_01:	PUSH	CS			; Clear window
	POP	DS
	PUSH	BX
	ADD	BH,2
	MOV	CL,DH
	SUB	CL,5
	MOV	CH,0
	MOV	SI,'ºº'
	MOV	AL,' '
	MOV	BP,OFFSET Find_P
	MOV	AH,CS:[BP]
	CALL	Color
F67_02:	CALL	StdLine
	LOOP	F67_02
	SUB	BH,5
	MOV	SI,'Ç¶'
	MOV	AL,'Ä'
	CALL	StdLine
	MOV	AL,25
	ADD	BL,5
	CALL	BasAdr
	MOV	SI,OFFSET Find1_S
	CALL	NgLine
	ADD	BH,2
	CALL	BasAdr
	MOV	SI,OFFSET Find2_S
	CALL	NgLine
	PUSH	ES
	POP	DS
	POP	BX
	CALL	Window
	PUSH	DX			; Input parameters
	PUSH	BX
	ADD	BH,DH
	MOV	DH,9
	SUB	BH,DH
	MOV	BP,OFFSET Find_B
	CALL	Dialog
	POP	BX
	POP	DX
	JE	F67_03
	JMP	F67_25
F67_03:	MOV	BP,OFFSET Find_P	; Clear window
	MOV	AH,CS:[BP]
	CALL	Color
	PUSH	BX
	ADD	BH,DH
	SUB	BH,8
	MOV	SI,'ºº'
	MOV	AL,' '
	CALL	StdLine
	CALL	StdLine
	INC	BH
	CALL	StdLine
	CALL	StdLine
	SUB	BH,3
	MOV	SI,'Ç¶'
	MOV	AL,'Ä'
	CALL	StdLine
	PUSH	CS
	POP	DS
	MOV	AL,25
	ADD	BL,5
	CALL	BasAdr
	MOV	SI,OFFSET Find3_S
	CALL	NgLine
	INC	BH
	MOV	BL,40
	MOV	SI,OFFSET Find5_S
	CALL	CntLine
	POP	BX
	CALL	Window
	PUSH	ES			; Find files
	POP	DS
	MOV	DS:FndBg,BX
	MOV	DS:FndSz,DX
	XOR	AX,AX
	MOV	DS:FndCur,AX
	MOV	DS:FndFrst,AX
	MOV	DS:FndNumb,AX
	MOV	DS:FndFls,AX
	MOV	DS:FndMode,AL
	MOV	DS:FndMenu,AL
	MOV	DS:WinMous,AL
	MOV	DS:MousEnt,AL
	MOV	DS:MousTim.Hi,0FFFFh
	MOV	DS:ViTxHex,AL
	MOV	AX,ES
	ADD	AX,DS:FreMem2
	SUB	AX,DS:ViewSeg
	MOV	DL,AH
	MOV	DH,0
	MOV	CL,4
	SHL	AX,CL
	SHR	DX,CL
	MOV	CX,LenFnf
	DIV	CX
	DEC	AX
	MOV	DS:FndSize,AX
	MOV	DS:FndDrv,'A'
F67_08:	MOV	DX,OFFSET TmpErr
	MOV	SI,OFFSET FindStr
	MOV	DI,DX
	LODSW
	STOSW
	CMP	AX,':*'
	JNE	F67_09
	MOV	AX,SI
	CALL	CutPath
	CMP	AX,SI
	MOV	SI,AX
	JNE	F67_09
	MOV	AL,'\'
	STOSB
F67_09:	MOV	CX,LenPath+13-2
	REP	MOVSB
	MOV	SI,DX
	CALL	NormStr
	CMP	BYTE PTR DS:[SI],0
	JE	F67_14
	CALL	CutPath
	MOV	WORD PTR DS:[SI],'A'
	PUSH	BX
	MOV	SI,DX
	MOV	BP,DS:[SI]
	CMP	BP,':*'
	JNE	F67_15
F67_10:	MOV	CL,DS:FndDrv
	MOV	DS:[SI],CL
	INC	DS:FndDrv
	SUB	CL,'A'
	MOV	CH,0
	XOR	BX,BX
	MOV	AX,150Bh
	INT	2Fh
	CMP	BX,0ADADh
	JNE	F67_11
	OR	AX,AX
	JNE	F67_13
F67_11:	MOV	BX,CX
	INC	BX
	MOV	AX,4409h
	INT	21h
	JC	F67_12
	TEST	DH,10h
	JNZ	F67_15
F67_12:	MOV	AX,4408h
	INT	21h
	JC	F67_13
	CMP	AL,1
	JE	F67_15
F67_13:	CMP	DS:FndDrv,'Z'
	JBE	F67_10
	POP	BX
	JMP	F67_18
F67_14:	MOV	DX,DS:FndSz
	JMP	F67_25
F67_15:	POP	BX
	MOV	DX,SI
	CALL	ChgDisk
	JC	F67_14
	MOV	DI,OFFSET TmpBuf
	LODSW
	CMP	AH,':'
	JE	F67_16
	DEC	SI
	DEC	SI
	MOV	AX,WORD PTR DS:DosPth
F67_16:	CALL	UpCase
	STOSW
	SUB	AL,'A'-1
	MOV	DL,AL
	MOV	CX,LenPath+13
	MOV	AL,'\'
	CMP	AL,DS:[SI]
	JE	F67_17
	STOSB
	XCHG	SI,DI
	MOV	AH,47h
	CALL	Intr21
	XCHG	SI,DI
	JC	F67_19
	MOV	AL,0
	REPNE	SCASB
	DEC	DI
	CALL	AddFile
F67_17:	REP	MOVSB
	MOV	AL,0
	STOSB
	MOV	SI,OFFSET TmpBuf
	CALL	CutPath
	MOV	BYTE PTR DS:[SI],0
	MOV	DS:FncAddr,OFFSET FindFil
	MOV	AL,10h
	CALL	FindFil
	OR	AH,AH
	JNZ	F67_18
	CMP	BP,':*'
	JNE	F67_18
	CMP	DS:FndDrv,'Z'
	JA	F67_18
	JMP	F67_08
F67_18:	TEST	DS:FndMode,2
	JZ	F67_19
	MOV	AL,DS:FndMenu
	CMP	AL,0
	JE	F67_19
	MOV	DX,DS:FndSz
	JMP	F67_23
F67_19:	MOV	DI,OFFSET TmpBuf	; Output NNN files found
	PUSH	DI
	MOV	CX,DS:FndFls
	XOR	BX,BX
	MOV	AL,1
	CALL	Number
	PUSH	CS
	POP	DS
	MOV	SI,OFFSET FnFl6_S
	MOV	CX,15
	CALL	MvzLine
	CMP	ES:FndFls,1
	JNE	F67_20
	DEC	DI
F67_20:	MOV	SI,OFFSET FnFl7_S
	MOV	CX,15
	REP	MOVSB
	PUSH	ES
	POP	DS
	POP	SI
	MOV	BX,DS:FndBg
	MOV	DX,DS:FndSz
	PUSH	DX
	PUSH	BX
	ADD	BH,DH
	SUB	BH,5
	ADD	BL,5
	CALL	BasAdr
	SUB	DL,12
	MOV	DH,0
	MOV	CX,DX
	MOV	AH,DS:[DI+1]
	MOV	AL,0
	PUSH	DI
	REP	STOSW
	POP	DI
	MOV	BP,OFFSET Find_P
	MOV	AX,DX
	CALL	NgLine
	MOV	DH,1
	CALL	Window
	POP	BX
	POP	DX
	PUSH	BX
	PUSH	CS
	POP	DS
	MOV	SI,OFFSET Find6_S
	ADD	BH,DH
	SUB	BH,4
	MOV	BL,40
	CALL	CntLine
	PUSH	ES
	POP	DS
	POP	BX
	MOV	DS:FndMenu,1
	MOV	BP,DS:FndCur
	MOV	DI,DS:FndFrst
	OR	BP,BP
	JE	F67_21
	MOV	AH,CS:Find_P[1]
	CALL	FndCurs
F67_21:	CALL	GetMous			; Dialog
	MOV	AH,0FCh
	JNZ	F67_22
	MOV	DS:WinMous,0
	MOV	DS:MousEnt,0
	PUSH	DX
	PUSH	BX
	ADD	BH,DH
	SUB	BH,4
	ADD	BL,18
	MOV	DX,125h
	MOV	BP,OFFSET FnFl_P
	MOV	SI,OFFSET Find6_D
	MOV	CL,DS:FndMenu
	MOV	AL,CS:Find_B.bKeyBar
	CALL	MinMnu
	MOV	DS:FndMenu,CL
	POP	BX
	POP	DX
F67_22:	MOV	CL,1
	CALL	FndMove
	OR	AH,AH
	JE	F67_21
F67_23:	CMP	AL,1
	JE	F67_30
	JA	F67_25
	JMP	F67_01
F67_25:	POP	SI
	CALL	ResWin
	CALL	Window
F67_26:	JMP	Func04
F67_30:	POP	SI			; Go to ...
	CALL	ResWin
	CALL	Window
	CMP	DS:FndNumb,0
	JE	F67_26
	MOV	BP,DS:FndCur
F67_31:	MOV	AX,BP
	CALL	FndAdr
	DEC	BP
	CMP	BYTE PTR DS:[SI],' '
	JE	F67_31
	MOV	DX,SI
	CALL	ChgDisk
	JC	F67_35
	MOV	AH,3Bh
	CALL	Intr21
	JC	F67_35
	MOV	DL,DS:[SI]
	SUB	DL,'A'
	MOV	AH,0Eh
	INT	21h
	PUSH	ES			; Set cursor
	PUSH	DS
	MOV	DS,ES:WCBcrn
	CMP	DS:WinTyp,0
	JE	F67_32
	CALL	InvWCB
F67_32:	PUSH	DS
	POP	ES
	POP	DS
	MOV	DI,OFFSET WinPath
	MOV	CX,LenPath+1
	REP	MOVSB
	PUSH	ES
	POP	DS
	POP	ES
	MOV	DI,OFFSET DskBuf
	LEA	BX,[DI+SIZE FilEnt]
	MOV	DS:CurAdr,DI
	MOV	DS:First,BX
	MOV	DS:EndBuf,BX
	MOV	DS:[DI].NameF,0
	MOV	DS:[BX].NameF,0
	PUSH	ES
	PUSH	DS
	MOV	AX,ES:FndCur
	CALL	FndAdr
	CMP	BYTE PTR DS:[SI],' '
	JNE	F67_33
	POP	ES
	PUSH	DI
	ADD	SI,3
	MOV	CX,12
	REP	MOVSB
	MOV	AL,0
	STOSB
	POP	SI
	CALL	NormStr
	PUSH	ES
F67_33:	POP	DS
	POP	ES
	MOV	AH,19h
	INT	21h
	ADD	AL,'A'
	CMP	AL,ES:DosPth
	JNE	F67_34
	JMP	F24_02
F67_34:	JMP	FncEx1
F67_35:	JMP	Func04

FindFil	PROC	NEAR			; Find files
	PUSH	DS
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	TEST	AL,10h
	JZ	FnFl10
	PUSH	ES			; Output 'Sanning: PATH'
	POP	DS
	CALL	FndStat
	MOV	DI,DX			; Find files
	MOV	CX,LenPath+13
	MOV	AL,0
	REPNE	SCASB
	DEC	DI
	MOV	SI,DI
	SUB	SI,DX
	CMP	SI,LenFnf
	JB	FnFl11
FnFl10:	XOR	AX,AX
	JMP	FnFl51
FnFl11:	MOV	AL,DS:[DI-1]
	CMP	AL,':'
	JE	FnFl12
	CMP	AL,'\'
	JE	FnFl12
	MOV	AL,'\'
	STOSB
FnFl12:	MOV	SI,OFFSET FindStr
	CALL	CutPath
	PUSH	DI
	MOV	DI,OFFSET TmpErr+20
	PUSH	DI
	MOV	BX,OFFSET Mask_S+1
FnFl13:	LODSB
	CMP	AL,0
	JE	FnFl15
	CMP	AL,'.'
	JNE	FnFl14
	MOV	BX,OFFSET Mask_S+4
FnFl14:	STOSB
	JMP	FnFl13
FnFl15:	PUSH	CS
	POP	DS
	MOV	SI,BX
	MOV	CX,4
	REP	MOVSB
	PUSH	ES
	POP	DS
	POP	SI
	MOV	DI,OFFSET TmpErr
	CALL	Convert
	MOV	SI,DI
	POP	DI
	CALL	UnConv
	MOV	DS:FndDir,0
	JMP	FnFl40
FnFl20:	TEST	DS:FndDir,1		; First file in directory
	JNZ	FnFl30
	OR	DS:FndDir,1
	MOV	AX,DS:FndNumb
	OR	AX,AX
	JE	FnFl22
	INC	AX
	INC	AX
	CMP	AX,DS:FndSize
	JB	FnFl21
	JMP	FnFl43			; Out of memory ???
FnFl21:	MOV	DI,OFFSET TmpErr
	MOV	CX,LenFnf
	MOV	AL,0
	REP	STOSB
	CALL	FndStuf
FnFl22:	MOV	SI,OFFSET TmpBuf
	MOV	DI,OFFSET TmpErr
	MOV	CX,LenPath+13
	PUSH	DI
	REP	MOVSB
	POP	DI
	MOV	SI,DI
	CALL	CutPath
	MOV	AX,SI
	SUB	AX,DI
	CMP	AX,3
	JBE	FnFl23
	DEC	SI
FnFl23:	MOV	BYTE PTR DS:[SI],0
	MOV	SI,DI
	CALL	PathLin
	MOV	CX,LenFnf
	CALL	FilZero
	CALL	FndStuf
FnFl30:	MOV	DI,OFFSET TmpErr	; Find next file
	MOV	CX,LenFnf
	MOV	AL,' '
	PUSH	DI
	REP	STOSB
	POP	DI
	ADD	DI,3
	PUSH	DI
	MOV	SI,OFFSET DTA.FilName
	MOV	AH,DS:DTA.FilAttr
FnFl31:	LODSB
	CMP	AL,0
	JE	FnFl33
	TEST	AH,10h
	JNZ	FnFl32
	CALL	LowCase
FnFl32:	STOSB
	JMP	FnFl31
FnFl33:	POP	DI
	ADD	DI,14
	TEST	AH,10h
	JZ	FnFl34
	PUSH	CS
	POP	DS
	ADD	DI,6
	MOV	SI,OFFSET SubDir+1
	MOV	CX,7
	REP	MOVSB
	PUSH	ES
	POP	DS
	JMP	FnFl35
FnFl34:	MOV	CX,DS:DTA.FilSize.Lo
	MOV	BX,DS:DTA.FilSize.Hi
	MOV	AL,3
	CALL	Number
	MOV	BYTE PTR DS:[DI],' '
FnFl35:	ADD	DI,3
	SHL	AH,1
	SHL	AH,1
	MOV	AL,'A'
	CALL	FnFl60
	SHL	AH,1
	SHL	AH,1
	MOV	AL,'S'
	CALL	FnFl60
	MOV	AL,'H'
	CALL	FnFl60
	MOV	AL,'R'
	CALL	FnFl60
	ADD	DI,3
	MOV	AX,DS:DTA.FilDate
	CALL	PutDate
	CALL	BufDate
	ADD	DI,3
	MOV	AX,DS:DTA.FilTime
	CALL	PutTime
	CALL	BufTime
	CALL	FndStuf
	JC	FnFl43			; Out of memory ???
	INC	DS:FndFls
FnFl40:	CALL	FndBack
	JNC	FnFl43
	MOV	DX,OFFSET TmpBuf
	CALL	ChgDisk
	JC	FnFl43
	MOV	AH,4Fh
	TEST	DS:FndDir,2
	JNZ	FnFl41
	OR	DS:FndDir,2
	MOV	CX,37h
	MOV	AH,4Eh
FnFl41:	CALL	SetDTA
	CALL	Intr21
	JNC	FnFl44
	CMP	AX,53h
	JE	FnFl43
	JMP	FnFl50
FnFl42:	CMP	AX,53h
	JNE	FnFl40
FnFl43:	MOV	AX,100h
	JMP	FnFl51
FnFl44:	CMP	DS:DTA.FilName[0],'.'
	JE	FnFl40
	CMP	DS:SrchStr[0],0		; Find by contents
	JE	FnFl47
	CALL	V81_00
	MOV	DX,OFFSET TmpBuf
	MOV	SI,DX
	CALL	CutPath
	MOV	DI,SI
	MOV	SI,OFFSET DTA.FilName
	MOV	CX,13
	REP	MOVSB
	CALL	FndStat
	MOV	AL,40h
	CALL	OpenFil
	JC	FnFl42
	MOV	BX,AX
	MOV	AX,ES
	ADD	AX,DS:FreMem2
	SUB	AX,DS:ViewSeg
	MOV	BP,8000h+80
	TEST	AH,0F0h
	JNZ	FnFl45
	MOV	CL,4
	SHL	AX,CL
	CMP	AX,BP
	JAE	FnFl45
	MOV	BP,AX
FnFl45:	CMP	BP,4000
	JB	FnFl48			; Out of memory ???
	MOV	AX,DS:FndNumb
	INC	AX
	CALL	FndAdr
FnFl46:	CALL	FndBack
	JNC	FnFl48
	MOV	CX,BP
	XOR	DX,DX
	MOV	AH,3Fh
	CALL	Intr21
	JC	FnFl48
	MOV	CX,AX
	XOR	SI,SI
	CALL	V82_00
	JNC	FnFl49
	CMP	CX,BP
	JB	FnFl49
	MOV	AX,4201h
	MOV	CX,-1
	MOV	DX,-80
	CALL	Intr21
	JMP	FnFl46
FnFl47:	JMP	FnFl20
FnFl48:	PUSH	ES
	POP	DS
	MOV	AH,3Eh
	CALL	Intr21
	JMP	FnFl43
FnFl49:	PUSHF
	PUSH	ES
	POP	DS
	MOV	AH,3Eh
	CALL	Intr21
	POPF
	JNC	FnFl47
	JMP	FnFl40
FnFl50:	MOV	SI,OFFSET TmpBuf	; Exit
	CALL	CutPath
	MOV	BYTE PTR DS:[SI],0
	CALL	DirFunc
FnFl51:	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	POP	DS
	RET
FindFil	ENDP

FnFl60	PROC	NEAR
	SHL	AH,1
	JC	FnFl61
	MOV	AL,'.'
FnFl61:	STOSB
	RET
FnFl60	ENDP

FndBack	PROC	NEAR			; Background movement
	PUSH	DS
	PUSH	BP
	PUSH	BX
	PUSH	ES
	POP	DS
FndBk1:	CALL	GetMous
	MOV	AH,0FCh
	JNZ	FndBk2
	MOV	AH,1
	INT	16h
	STC
	JZ	FndBk5
	MOV	AH,0
	INT	16h
	OR	AX,AX
	JE	FndBk5
	CMP	AX,2E03h
	JE	FndBk5
	CMP	AX,011Bh
	JE	FndBk5
	CMP	AX,3B00h
	JE	FndBk4
FndBk2:	MOV	BX,DS:FndBg
	MOV	DX,DS:FndSz
	PUSH	DX
	PUSH	BX
	CMP	DS:WinMous,0
	JNZ	FndBk3
	ADD	BH,DH
	SUB	BH,4
	ADD	BL,21
	MOV	DX,11Fh
	MOV	BP,OFFSET FnFl_P
	MOV	DI,OFFSET Find5_D
	MOV	CH,0
	MOV	CL,DS:FndMenu
	CALL	MnMn10
	MOV	DS:FndMenu,CL
	JNC	FndBk3
	MOV	AH,CS:[BP+0]
	MOV	AL,CS:[BP+1]
	CALL	MnMn40
FndBk3:	POP	BX
	POP	DX
	JC	FndBk1
	MOV	CL,1
	CALL	FndMove
	OR	AH,AH
	JZ	FndBk1
	OR	DS:FndMode,2
	MOV	DS:FndMenu,AL
	JMP	FndBk5
FndBk4:	CALL	Help
	JMP	FndBk1
FndBk5:	MOV	DS:WinMous,0
	MOV	DS:MousEnt,0
	POP	BX
	POP	BP
	POP	DS
	RET
FndBack	ENDP

FndStuf	PROC	NEAR			; Add line
	PUSH	DX
	PUSH	BX
	PUSH	ES
	POP	DS
	MOV	AX,DS:FndNumb
	CMP	DS:FndSize,AX
	JB	FndSt4
	INC	DS:FndNumb
	MOV	DX,LenFnf
	MUL	DX
	MOV	DI,AX
	AND	DI,0Fh
	MOV	CL,4
	SHR	AX,CL
	SHL	DL,CL
	OR	AH,DL
	ADD	AX,DS:ViewSeg
	MOV	ES,AX
	MOV	SI,OFFSET TmpErr
	MOV	CX,LenFnf
	REP	MOVSB
	PUSH	DS
	POP	ES
	MOV	BX,DS:FndBg
	MOV	DX,DS:FndSz
	TEST	DS:FndMode,1
	JNZ	FndSt1
	MOV	CL,0
	MOV	AX,4F00h
	CALL	FndMove
	JMP	FndSt3
FndSt1:	MOV	BP,DS:FndNumb
	DEC	BP
	MOV	DI,DS:FndFrst
	MOV	AX,BP
	SUB	AX,DI
	ADD	AX,8
	OR	AH,AH
	JNE	FndSt3
	CMP	AL,DH
	JAE	FndSt3
	MOV	AH,CS:Find_P[0]
	OR	BP,BP
	JNE	FndSt2
	MOV	AH,CS:Find_P[1]
FndSt2:	CALL	FndCurs
FndSt3:	CLC
FndSt4:	POP	BX
	POP	DX
	RET
FndStuf	ENDP

FndMove	PROC	NEAR			; Move the cursor
	MOV	BP,DS:FndCur
	MOV	DI,DS:FndFrst
	CMP	AH,0FCh
	JE	FnMv00
	MOV	SI,OFFSET Find_T
	CALL	Case
	JMP	FnMv30
FnMv00:	PUSH	CX
	CALL	GetMous			; Mouse support
FnMv01:	CALL	ClrKbd
	MOV	SI,CX
	TEST	AL,4
	JZ	FnMv03
	MOV	AL,'x'
	CALL	TypMous
FnMv02:	CALL	GetMous
	CALL	ClrKbd
	TEST	AL,4
	JNZ	FnMv02
	PUSH	AX
	MOV	AL,0
	CALL	TypMous
	POP	AX
	TEST	AL,3
	JNZ	FnMv03
	POP	CX
	JMP	FnMv32
FnMv03:	PUSH	DX
	PUSH	BX
	ADD	BX,204h
	SUB	DX,80Ah
	CALL	InWind
	POP	BX
	POP	DX
	JNC	FnMv08
	CMP	DS:WinMous,0
	JZ	FnMv05
	MOV	BP,DS:FndCur
	MOV	DI,DS:FndFrst
	MOV	AL,BH
	ADD	AL,2
	CMP	CH,AL
	JAE	FnMv04
	POP	CX
	JMP	FnMv12
FnMv04:	MOV	AL,BH
	ADD	AL,DH
	SUB	AL,6
	CMP	CH,AL
	JB	FnMv06
	POP	CX
	JMP	FnMv13
FnMv05:	TEST	AL,1
	JZ	FnMv06
	CALL	GetMous
	JNZ	FnMv01
	MOV	CX,SI
	PUSH	DX
	SUB	DX,102h
	CALL	InWind
	POP	DX
	JNC	FnMv06
	POP	CX
	JMP	FnMv32
FnMv06:	POP	CX
FnMv07:	JMP	FnMv30
FnMv08:	OR	DS:WinMous,1
	MOV	BP,DS:FndNumb
	OR	BP,BP
	JE	FnMv06
	PUSH	CX
	SUB	CH,BH
	SUB	CH,2
	MOV	CL,CH
	MOV	CH,0
	MOV	DI,DS:FndFrst
	ADD	CX,DI
	DEC	BP
	POP	SI
	XCHG	BP,CX
	CMP	BP,CX
	JBE	FnMv09
	MOV	BP,CX
	POP	CX
	JMP	FnMv20
FnMv09:	PUSH	DX
	PUSH	BX
	MOV	AX,SI
	MOV	SI,BP
	MOV	BH,AH
	ADD	BL,2
	SUB	DL,10
	MOV	DH,1
	CALL	DoublMo
	POP	BX
	POP	DX
	POP	CX
	JC	FnMv20
	JE	FnMv07
	MOV	AL,0
	CALL	TypMous
	MOV	AX,101h
	RET
FnMv10:	XOR	BP,BP			; Home
	JMP	FnMv20
FnMv11:	MOV	BP,DS:FndNumb		; End
FnMv12:	OR	BP,BP			; Up
	JE	FnMv20
	DEC	BP
	JMP	FnMv20
FnMv13:	INC	BP			; Down
	CMP	BP,DS:FndNumb
	JB	FnMv20
	DEC	BP
	JMP	FnMv20
FnMv14:	MOV	AL,DH			; PgUp
	SUB	AL,8
	CBW
	MOV	SI,BP
	CALL	PgUp
	MOV	BP,SI
	JMP	FnMv20
FnMv15:	MOV	SI,BP			; PgDn
	MOV	BP,DS:FndNumb
	MOV	AL,DH
	SUB	AL,8
	CBW
	CALL	PgDn
	MOV	BP,SI
FnMv20:	MOV	AL,DS:FndMode		; Test parameters
	PUSH	AX
	OR	DS:FndMode,CL
	CMP	BP,DI
	JAE	FnMv21
	MOV	DI,BP
FnMv21:	MOV	AL,DH
	SUB	AL,8
	CBW
	ADD	AX,DI
	CMP	BP,AX
	JB	FnMv22
	SUB	AX,DI
	MOV	DI,BP
	SUB	DI,AX
	INC	DI
FnMv22:	POP	AX
	MOV	CX,DI
	XCHG	BP,DS:FndCur
	XCHG	DI,DS:FndFrst
	SUB	CX,DI
	JNE	FnMv24
	TEST	DS:FndMode,1
	JZ	FnMv23
	CMP	BP,DS:FndCur
	JNE	FnMv24
	TEST	AL,1
	JZ	FnMv23
	JMP	FnMv30
FnMv23:	JMP	FnMv28
FnMv24:	MOV	AH,CS:Find_P		; Reset cursor
	CALL	FndCurs
	JCXZ	FnMv23
	PUSH	CX			; Output lines
	PUSH	BX
	MOV	CL,DH
	SUB	CL,8
	MOV	CH,0
	ADD	BX,205h
	MOV	AX,DS:FndFrst
FnMv25:	CALL	FndLine
	INC	AX
	INC	BH
	LOOP	FnMv25
	POP	BX
	POP	CX
	PUSH	DX			; Output screen
	PUSH	BX
	ADD	BX,205h
	SUB	DH,8
	MOV	AL,DH
	DEC	AX
	MOV	AH,6
	CMP	CX,1
	JE	FnMv26
	CMP	CX,-1
	JNE	FnMv27
	MOV	AX,700h
FnMv26:	MOV	CX,BX
	MOV	DL,63
	DEC	DH
	ADD	DX,CX
	PUSH	AX
	MOV	AH,CS:Find_P
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
FnMv27:	MOV	DL,64
	CALL	Window
	POP	BX
	POP	DX
FnMv28:	CMP	DS:FndNumb,0		; Set cursor
	JE	FnMv30
	MOV	BP,DS:FndCur
	MOV	DI,DS:FndFrst
	MOV	AH,CS:Find_P[0]
	TEST	DS:FndMode,1
	JZ	FnMv29
	MOV	AH,CS:Find_P[1]
FnMv29:	CALL	FndCurs
FnMv30:	XOR	AX,AX			; Exit
	RET
FnMv31:	MOV	AL,DS:FndMenu		; Enter
	MOV	AH,1
	RET
FnMv32:	MOV	AX,1FFh			; Esc
	RET
FndMove	ENDP

FndCurs	PROC	NEAR			; Set/Reset cursor
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	MOV	CX,BP
	SUB	CX,DI
	ADD	BH,CL
	ADD	BX,205h
	PUSH	AX
	MOV	AX,BP
	CALL	FndLine
	POP	AX
	SUB	DL,12
	MOV	CL,DL
	MOV	CH,0
	CALL	BasAdr
	CALL	Color
	MOV	AL,AH
	CALL	MvsColr
	MOV	DH,1
	CALL	Window
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	RET
FndCurs	ENDP

FndLine	PROC	NEAR			; Output item
	PUSH	DS
	PUSH	DI
	PUSH	CX
	PUSH	AX
	CALL	FndAdr
	CALL	BasAdr
	MOV	AH,CS:Find_P
	CALL	Color
	MOV	CX,LenFnf-1
FndLn1:	LODSB
	STOSW
	LOOP	FndLn1
	POP	AX
	POP	CX
	POP	DI
	POP	DS
	RET
FndLine	ENDP

FndAdr	PROC	NEAR
	PUSH	DX
	PUSH	CX
	MOV	DX,LenFnf
	MUL	DX
	MOV	SI,AX
	AND	SI,0Fh
	MOV	CL,4
	SHR	AX,CL
	SHL	DL,CL
	OR	AH,DL
	ADD	AX,ES:ViewSeg
	MOV	DS,AX
	POP	CX
	POP	DX
	RET
FndAdr	ENDP

FndStat	PROC	NEAR
	MOV	SI,OFFSET TmpBuf
	MOV	DI,OFFSET TmpErr
	MOV	CX,LenPath+13
	PUSH	SI
	PUSH	DI
	REP	MOVSB
	POP	SI
	MOV	AX,LenFnf-11
	MOV	DL,AL
	MOV	DH,1
	CALL	ShrtLin
	CALL	PathLin
	MOV	BX,DS:FndBg
	ADD	BH,DS:FndSz.Y
	SUB	BH,5
	ADD	BL,5+10
	CALL	BasAdr
	MOV	CL,DL
	MOV	CH,0
FnFl01:	LODSB
	CMP	AL,0
	JNE	FnFl02
	DEC	SI
	MOV	AL,' '
FnFl02:	STOSB
	INC	DI
	LOOP	FnFl01
	CALL	Window
	POP	DX
	RET
FndStat	ENDP
