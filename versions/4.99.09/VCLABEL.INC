;---------------------------------------------------------------
;		Ghange volume label
;
;		Version:	14.03.1998
;---------------------------------------------------------------

LabelSerNum	EQU	DWORD PTR TmpBuf[0]	; Serial number
LabelTreeBuf	EQU	 BYTE PTR TmpBuf[4]	; TREEINFO header

		.CONST
Label_S		DB	' Volume Label ',0
LabelChg_S	DB	'Change volume label^1E to',0
LabelDel_S	DB	'Do you wish to delete volume label',0
LabelName_S	DB	'^0E ?',0
LabelFix1_S	DB	'Current volume label^1E',0
LabelFix2_S	DB	'can not be changed',0
LabelErr_S	DB	'Could not ^0C the volume label',0
LabelDelErr_S	DB	'delete',0
LabelSetErr_S	DB	'set',0

		.DATA
Label_P		DB	Box_C, BoxCu_C, 0, 0
		DW	Label_S
		DW	LabelChg_S
		DW	Empty_S
		DW	0
Label_B		BOXHD	< Label_P, 500h, 0, Dialog_K, Cmd_M, Label_H, 0 >
		BOXMN	< 1, 0,0,0,0, 305h, LineBuf1, 80h+11 >
		DB	0
LabelFix_P	DB	Box_C, BoxCu_C, 0, 0
		DW	Label_S
		DW	LabelFix1_S
		DW	LabelFix2_S
		DW	Ok_S
		DW	0
LabelFix_B	BOXHD	< LabelFix_P, 500h, 0, Dialog_K, Cmd_M, Label_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
LabelDel_T	DW	Volume
LabelDel_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Label_S
		DW	LabelDel_S
		DW	LabelName_S
		DW	DelCancel_S
		DW	0
LabelSetErr_T	DW	LabelSetErr_S, LineBuf1
LabelDelErr_T	DW	LabelDelErr_S, Volume
LabelDel_B	BOXHD	< LabelDel_P, 700h, 0, Dialog_K, Def_M, Default_H, 5 >
		BOXMN	< 4, 0,0,0,0, 4F7h, DelCancel_D, 0 >
		DB	0
LabelErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Label_S
		DW	LabelErr_S
		DW	Line1_S
		DW	Ok_S
		DW	0
LabelErr_B	BOXHD	< LabelErr_P, 700h, 0, Dialog_K, Def_M, Default_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0

		.CODE
F54_00:		CALL	HidCur			; Change label
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET Volume	; Get label
		CALL	GetVol
		JNC	F54_01
		CMP	AX,53h
		JE	Func04
		MOV	BL,1
F54_01:		MOV	DS:LabelSerNum.Lo,BX
		MOV	DS:LabelSerNum.Hi,CX
		PUSH	SI
		MOV	DI,OFFSET LineBuf1
		CALL	MvzLine
		STOSB
		STOSB
		POP	SI
		MOV	DI,OFFSET LineBuf2
		MOV	BYTE PTR DS:[DI],0
		CMP	BYTE PTR DS:[SI],0
		JE	F54_02
		MOV	AX,'" '
		STOSW
		CALL	MvzLine
		MOV	AX,'"'
		STOSW
F54_02:		MOV	DX,OFFSET LineBufs_T	; Check drive
		MOV	AH,19h
		INT	21h
		ADD	AL,'A'
		CALL	TstDrv
		CMP	AL,Network
		JE	F54_08
		CMP	AL,CDROM
		JE	F54_08
		MOV	BP,OFFSET Label_B
		CALL	Dialog
		JNE	Func04
		MOV	SI,OFFSET Volume	; Compare old and new labels
		MOV	DI,OFFSET LineBuf1
		CALL	StrLen
		REPE	CMPSB
		JE	Func04
		MOV	SI,OFFSET LineBuf1	; Check new label
F54_03:		LODSB
		CMP	AL,'.'
		JE	F54_06
		CMP	AL,0
		JNE	F54_03
		CMP	DS:LineBuf1[0],0	; Delete warning
		JNE	F54_04
		MOV	DX,OFFSET LabelDel_T
		MOV	BP,OFFSET LabelDel_B
		CALL	Dialog
		JNE	Func04
F54_04:		MOV	SI,OFFSET Volume
		CMP	BYTE PTR DS:[SI],0
		JE	F54_05
		MOV	DX,OFFSET LineBuf2
		MOV	DI,DX
		MOV	AL,0FFh
		STOSB
		ADD	DI,5
		MOV	AX,8
		STOSW
		CALL	MvzLine
		MOV	CX,12
		MOV	AL,' '
		REP	STOSB
		MOV	AH,13h
		CALL	Intr21
		MOV	DX,OFFSET LabelDelErr_T
		JC	F54_07
F54_05:		MOV	SI,OFFSET LineBuf1
		CMP	BYTE PTR DS:[SI],0
		JE	F54_10
		MOV	DX,OFFSET LineBuf2
		MOV	DI,DX
		MOV	AL,'\'
		STOSB
		MOV	CX,8
		REP	MOVSB
		MOV	AL,'.'
		STOSB
		MOV	CL,4
		REP	MOVSB
		MOV	CX,8
		MOV	AH,3Ch
		CALL	Intr21
		JNC	F54_10
F54_06:		MOV	DX,OFFSET LabelSetErr_T
F54_07:		MOV	BP,OFFSET LabelErr_B
		CMP	AX,53h
		JE	F54_18
		CALL	Dialog
		JMP	F54_18
F54_08:		MOV	BP,OFFSET LabelFix_B
		CALL	Dialog
		JMP	Func04
F54_10:		MOV	AX,DS:LabelSerNum.Lo	; Update TREEINFO in home dir
		OR	AX,DS:LabelSerNum.Hi
		JNE	F54_18
		MOV	AL,DS:VcSet[0]
		CALL	Phantom
		JC	F54_18
		MOV	DI,OFFSET Volume
		CALL	StrLen
		DEC	CX
		ADD	DI,CX
		SUB	CX,13
		NEG	CX
		MOV	AL,' '
		REP	STOSB
		MOV	DI,OFFSET LineBuf1
		CALL	StrLen
		DEC	CX
		ADD	DI,CX
		SUB	CX,13
		NEG	CX
		MOV	AL,' '
		REP	STOSB
		MOV	SI,OFFSET VcSet
		MOV	DI,OFFSET LineBuf2
		CALL	MvzLine
		PUSH	DS
		MOV	DS,CS:SubsSeg
		MOV	SI,OFFSET TreeInfo_F+1
		CALL	MvzLine
		POP	DS
		MOV	AL,'.'
		STOSB
		MOV	AX,'*'
		STOSW
		MOV	CX,27h
		MOV	AH,4Eh
F54_11:		MOV	DX,OFFSET LineBuf2
		CALL	Intr21
		JC	F54_17
		MOV	SI,OFFSET DTA.FilName
F54_12:		LODSB
		CMP	AL,0
		JE	F54_15
		CMP	AL,'.'
		JNE	F54_12
		MOV	DI,SI
		CALL	StrLen
		CMP	CX,4
		JNE	F54_15
		CALL	TxtNum
		JC	F54_15
		MOV	SI,DX
		CALL	CutPath
		MOV	DI,SI
		MOV	SI,OFFSET DTA.FilName
		CALL	MvzLine
		MOV	AL,12h
		CALL	OpenFil
		JNC	F54_13
		CMP	AX,53h
		JNE	F54_15
		JMP	F54_17
F54_13:		MOV	DX,OFFSET LabelTreeBuf
		MOV	CX,OFFSET TreeList
		MOV	BX,AX
		MOV	AH,3Fh
		CALL	Intr21
		JC	F54_16
		CMP	AX,CX
		JNE	F54_14
		MOV	AX,DS:LabelTreeBuf.TreeSerNum.Lo
		OR	AX,DS:LabelTreeBuf.TreeSerNum.Hi
		JNE	F54_14
		MOV	SI,OFFSET Volume
		MOV	DI,OFFSET LabelTreeBuf.TreeLabel
		MOV	CX,11
		REPE	CMPSB
		JNE	F54_14
		XOR	CX,CX
		MOV	DX,OFFSET TreeLabel
		MOV	SI,DX
		MOV	AX,4200h
		CALL	Intr21
		JC	F54_16
		CMP	AX,SI
		JNE	F54_14
		CMP	DX,CX
		JNE	F54_14
		MOV	DX,OFFSET LineBuf1
		MOV	CX,11
		MOV	AH,40h
		CALL	Intr21
		JC	F54_16
F54_14:		MOV	AH,3Eh
		CALL	Intr21
		JNC	F54_15
		CMP	AX,53h
		JE	F54_17
F54_15:		MOV	AH,4Fh
		JMP	F54_11
F54_16:		CMP	AX,53h
		JNE	F54_14
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	AX
F54_17:		CALL	CloseFindFile
F54_18:		JMP	FuncExit2

;---------------------------------------------------------------
;	Get volume label and serial number
;		DS:SI - buffer for label
;	Return:	BX CX - serial number
;		   NC - no errors
;		   CY - error, AX - error code
;---------------------------------------------------------------
		.CODE	Subs
GetVol		PROC	FAR
		LOCAL	LabelBuf:BYTE:256
		USES	DX,SI,DI,DS,ES
		MOV	AH,19h
		INT	21h
		MOV	BL,AL
		INC	BX
		ADD	AL,'A'
		CALL	Phantom
		MOV	AX,53h
		JC	@@Exit
		PUSH	DS			; Get serial number
		PUSH	SS
		POP	DS
		LEA	DX,LabelBuf
		MOV	WORD PTR LabelBuf,0
		MOV	CX,0866h
		MOV	AX,440Dh
		CALL	Intr21
		POP	DS
		MOV	DL,BL
		MOV	BX,LabelBuf[2].Lo
		MOV	CX,LabelBuf[2].Hi
		JNC	@@SaveSerNum
		XOR	BX,BX
		XOR	CX,CX
		CMP	AX,53h
		STC
		JE	@@Exit
  @@SaveSerNum:	PUSH	CX
		PUSH	BX
		MOV	AL,DL			; Check type of drive
		ADD	AL,'A'-1
		CALL	TstDrv
		CMP	AL,Network
		JNE	@@DiskLabel
		PUSH	ES			; Check network drive
		PUSH	SI
		PUSH	SS
		POP	ES
		MOV	BYTE PTR DS:[SI+0],'\'
		MOV	WORD PTR DS:[SI+1],'A'
		LEA	DI,LabelBuf
		MOV	AH,60h
		CALL	Intr21
		MOV	BYTE PTR DS:[SI],0
		POP	SI
		POP	ES
		JC	@@Error
		MOV	AX,'\\'
		CMP	AX,SS:[DI]
		JNE	@@DiskLabel
		PUSH	SS			; Network label
		POP	ES
		INC	DI
		INC	DI
		MOV	CH,0FFh
		REPNE	SCASB
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		XCHG	SI,DI
		MOV	CX,12
  @@NetLabel:	LODSB
		CMP	AL,'\'
		JE	@@EndLine
		STOSB
		LOOP	@@NetLabel
  @@NoError:	CLC
		JMP	@@EndLine
  @@DiskLabel:	MOV	DI,SI			; Disk label
		PUSH	DS
		PUSH	ES
		PUSH	CS
		POP	DS
		MOV	DX,OFFSET Mask_S
		MOV	CX,8
		MOV	AH,4Eh
		CALL	Intr21
		POP	DS
		POP	ES
		JNC	@@Found
		CMP	AX,53h
		JNE	@@NoError
		STC
		JMP	@@EndLine
  @@Found:	CALL	CloseFindFile
		MOV	SI,OFFSET DTA.FilName
		MOV	CX,8
  @@Move1:	LODSB
		CMP	AL,0
		JE	@@EndLine
		CMP	AL,'.'
		JE	@@Move2
		STOSB
		LOOP	@@Move1
		LODSB
		CMP	AL,'.'
		JE	@@Move2
		DEC	SI
  @@Move2:	MOV	AL,' '
		REP	STOSB
		MOV	CX,3
  @@Move3:	LODSB
		CMP	AL,0
		JE	@@EndLine
		STOSB
		LOOP	@@Move3
  @@EndLine:	MOV	BYTE PTR ES:[DI],0
  @@Error:	POP	BX
		POP	CX
  @@Exit:	RET
GetVol		ENDP
