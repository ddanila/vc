;---------------------------------------------------------------
;		Subroutines
;
;		Version:	12.07.2000
;---------------------------------------------------------------

MaxVect		EQU	8
SegDOS		EQU	70h

BlkList		EQU	HelpBuf
BlkEnd		EQU	HelpBuf+LenHelp
MemVectors	EQU	TmpErr

MemLimit	EQU	 WORD PTR TmpBuf[0]	; Size of memory list
MemRelease	EQU	DWORD PTR TmpBuf[0]	; Address of Release routine

MemEnt		STRUC
  MemFlags	DB	?		; Flags: b0 - releasable, b1 - selected
  MemSegment	DW	?		; Segment
  MemBlocks	DW	?		; Number of blocks
  MemSize	DW	?		; Size in paragraphs
  MemIntNumb	DB	?		; Number of vectors
  MemInts	DB	MaxVect DUP(?)	; Vectors
  MemName	DB	12 DUP(?)	; Block name
MemEnt		ENDS

		.CONST
MemInfo_S	DB	' Memory Info ',0
MemTitle_S	DB	'Address Blocks',0FFh,3,'Size',0FFh,4,'Program',0FFh,8,'Hooked vectors',0FFh,LenStr-54,0
MemMenu_S	DB	'[ Release ]',0FFh,3,'[ Cancel ]',0
MemDos_S	DB	'DOS ',0
MemFree_S	DB	'free memory',0
MemSystem_S	DB	'system',0
MemUnknown_S	DB	'unknown',0
MemCmdProc_S	DB	'COMMAND.COM',0
MemError0_S	DB	'This memory block',0
MemError1_S	DB	'^0C can not be released',0
MemError2_S	DB	'is free',0
MemError3_S	DB	'because its vectors are hooked by another program',0

		.DATA
MemInfo_P	DB	Conf_C, CnfRe_C, CnfBo_C, CnfUn_C
		DW	MemInfo_S
		DW	MemTitle_S
		DW	0FFFFh
		DW	0FFFEh
		DW	0FFFFh
		DW	MemMenu_S
		DW	0
MemInfo_B	BOXHD	< MemInfo_P, 8500h, MemProc, Dialog_K, Cmd_M, Memory_H, 0 >
		BOXMN	< 4, 0,0,0,0, 0FCF4h, MemMenu_D, 0 >
		DB	0
MemError1_T	DW	MemError0_S, LineBuf1
MemError1_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	MemInfo_S
		DW	MemError1_S
		DW	Quote1_S
		DW	Ok_S
		DW	0
MemError1_B	BOXHD	< MemError1_P, 900h, 0, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
MemError2_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	MemInfo_S
		DW	MemError0_S
		DW	MemError2_S
		DW	Ok_S
		DW	0
MemError2_B	BOXHD	< MemError2_P, 900h, 0, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
MemError3_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	MemInfo_S
		DW	MemError1_S
		DW	MemError3_S
		DW	Quote1_S
		DW	Ok_S
		DW	0
MemError3_B	BOXHD	< MemError3_P, 900h, 0, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 5FEh, OkMn_D, 0 >
		DB	0
MemMenu_D	DB	11,  0
		DB	10, 14
		DB	 0

		.CODE
F65_00:		PUSH	ES			; Alt F5 - memory info
		POP	DS
		MOV	AX,5802h		; MCB analisis
		INT	21h
		PUSH	AX
		PUSHF
		JC	F65_01
		MOV	BX,1
		MOV	AX,5803h
		INT	21h
F65_01:		MOV	SI,OFFSET BlkList
		MOV	DI,SI
		MOV	AX,SegDOS
		STOSW
		STOSW
		PUSH	AX
		MOV	AH,52h
		INT	21h
		MOV	AX,ES:[BX-2]
		PUSH	DS
		POP	ES
		POP	BX
		MOV	DS,AX
		INC	AX
		JMP	F65_05
F65_02:		INC	AX
		MOV	BX,DS:[1]
		MOV	CX,ES
		CMP	AX,CX
		JNE	F65_03
		XOR	BX,BX
		JMP	F65_05
F65_03:		MOV	CX,CS
		CMP	AX,CX
		JNE	F65_05
		CMP	ES:DataLocation,2
		JAE	F65_04
		CMP	ES:LoadFl,0
		JE	F65_05
F65_04:		XOR	BX,BX
F65_05:		STOSW
		MOV	AX,BX
		STOSW
		MOV	AX,DS
		ADD	AX,DS:[3]
		INC	AX
		CMP	DI,OFFSET BlkEnd-8
		JA	F65_06
		CMP	BYTE PTR DS:[0],'M'
		MOV	DS,AX
		JE	F65_02
F65_06:		INC	AX
		STOSW
		XOR	AX,AX
		STOSW
		PUSH	ES
		POP	DS
		STOSW
		POPF
		POP	BX
		JC	F65_10
		MOV	BH,0
		MOV	AX,5803h
		INT	21h

F65_10:		MOV	DS:ListTotal,0		; Program analisis
		MOV	AX,ES
		ADD	AX,DS:SizeDataSeg
		MOV	BX,DS:WorkBuffer
		MOV	ES,BX
		SUB	AX,BX
		MOV	BX,0FFFh
		CMP	AX,BX
		JBE	F65_11
		MOV	AX,BX
F65_11:		MOV	CL,4
		SHL	AX,CL
		SUB	AX,SIZE MemEnt
		MOV	DS:MemLimit,AX
		MOV	DX,SI
		XOR	DI,DI
F65_12:		CMP	DI,DS:MemLimit		; Loop
		JAE	F65_39
		LODSW
		MOV	BX,AX
		LODSW
		OR	AX,AX
		JE	F65_14
		CMP	AX,BX
		JE	F65_16
		MOV	BP,DX
F65_13:		CMP	AX,DS:[BP]		; Check owner
		JE	F65_38
		ADD	BP,4
		CMP	WORD PTR DS:[BP+4],0
		JNE	F65_13
		MOV	BP,1
		MOV	CX,DS:[SI]
		SUB	CX,BX
		DEC	CX
		PUSH	SI
		MOV	SI,DX
		JMP	F65_17
F65_14:		INC	AX			; Free block
		CMP	WORD PTR DS:[SI+2],0
		JNE	F65_15
		CMP	WORD PTR DS:[SI+4],0
		JE	F65_15
		ADD	SI,4
		JMP	F65_14
F65_15:		MOV	BP,AX
		MOV	CX,DS:[SI]
		SUB	CX,BX
		DEC	CX
		JMP	F65_19
F65_16:		PUSH	SI			; Program block
		MOV	SI,DX
		XOR	BP,BP
		XOR	CX,CX
F65_17:		LODSW
		CMP	BX,DS:[SI]
		JNE	F65_18
		INC	BP
		ADD	CX,DS:[SI+2]
		SUB	CX,AX
		DEC	CX
F65_18:		LODSW
		CMP	WORD PTR DS:[SI+4],0
		JNE	F65_17
		POP	SI
F65_19:		MOV	ES:[DI].MemFlags,0
		MOV	ES:[DI].MemSegment,BX
		MOV	ES:[DI].MemBlocks,BP
		MOV	ES:[DI].MemSize,CX
		MOV	ES:[DI].MemIntNumb,0

		PUSH	SI			; Searching for name
		PUSH	DI
		PUSH	DS
		POP	ES
		CMP	WORD PTR DS:[SI-2],0	; Free memory
		JNE	F65_20
		MOV	DS,DS:WorkBuffer
		OR	DS:[DI].MemFlags,4
		MOV	SI,OFFSET MemFree_S
		JMP	F65_34
F65_20:		CMP	BX,SegDOS		; DOS area
		JNE	F65_22
		MOV	SI,OFFSET MemDos_S
		MOV	DI,OFFSET LineBuf1
		PUSH	DI
		PUSH	DS
		MOV	DS,CS:SubsSeg
		CALL	MvzLine
		POP	DS
		MOV	AX,DS:VerDOS
		MOV	BH,AH
		CALL	HexDec
		CMP	AL,'0'
		JE	F65_21
		STOSB
F65_21:		MOV	AL,AH
		STOSB
		MOV	AL,'.'
		STOSB
		MOV	AL,BH
		CALL	HexDec
		STOSW
		MOV	AL,0
		STOSB
		POP	SI
		JMP	F65_35
F65_22:		PUSH	SI			; Releasable program
		CMP	DS:ChkTSR,0
		JZ	F65_26
		CMP	DS:DataLocation,3
		JNE	F65_23
		PUSH	DX
		PUSH	BX
		MOV	DX,DS:DatHandle
		XOR	BX,BX
		MOV	AX,4400h
		CALL	CallEMS
		POP	BX
		POP	DX
		JNE	F65_26
F65_23:		MOV	DS,CS:SubsSeg
		MOV	DS,DS:ResidentPoint.Segm
		MOV	SI,ES:ReleaseData
		MOV	CX,DS:[SI+2]
		ADD	SI,4
		JCXZ	F65_26
F65_24:		PUSH	DS
		LODSW
		MOV	DS,AX
		CMP	BX,DS:[0]
		POP	DS
		LOOPNE	F65_24
		JNE	F65_26
		PUSH	ES
		MOV	ES,ES:WorkBuffer
		OR	ES:[DI].MemFlags,1
		MOV	DS,AX			; Vectors
		ADD	DI,OFFSET MemIntNumb
		MOV	SI,14
		MOVSB
		MOV	CX,MaxVect
F65_25:		MOVSB
		LODSW
		LODSW
		LOOP	F65_25
		MOV	SI,2
		POP	ES
		POP	AX
		JMP	F65_35
F65_26:		POP	SI
		MOV	AX,BX			; System area
		XCHG	AX,ES:[SI-2]
		CMP	AX,8
		JA	F65_27
		MOV	SI,OFFSET MemSystem_S
		JMP	F65_34
F65_27:		CMP	AX,BX			; Other programs
		JNE	F65_33
		MOV	DS,BX			; Check environment
		MOV	CX,DS:[2Ch]
		JCXZ	F65_31
		CMP	BX,CX
		JE	F65_31
		MOV	SI,DX
F65_28:		CMP	WORD PTR ES:[SI+4],0
		JE	F65_31
		LODS	ES:Wrd
		CMP	AX,CX
		LODS	ES:Wrd
		JNE	F65_28
		DEC	CX
		MOV	DS,CX
		MOV	SI,11h
		CMP	BX,DS:[1]
		JNE	F65_31
F65_29:		DEC	SI			; Get name from environment
		LODSW
		OR	AX,AX
		JNE	F65_29
		INC	SI
		INC	SI
		CMP	WORD PTR DS:[SI-2],1
		JE	F65_30
		CMP	BYTE PTR ES:VerDOS,4
		JAE	F65_31
		MOV	DS,BX
		CMP	BX,DS:[16h]
		JNE	F65_31
		MOV	SI,OFFSET MemCmdProc_S
		JMP	F65_34
F65_30:		CALL	CutPath
		CMP	BYTE PTR DS:[SI],0
		JNE	F65_35
F65_31:		CMP	BYTE PTR ES:VerDOS,4	; Get name from MCB
		JAE	F65_32
		PUSH	DX
		MOV	AX,4452h
		INT	21h
		POP	DX
		JC	F65_33
		CMP	AL,65h
		JB	F65_33
F65_32:		DEC	BX
		MOV	DS,BX
		MOV	SI,8
		MOV	DI,OFFSET LineBuf1
		PUSH	DI
		MOV	CX,8
		REP	MOVSB
		MOV	AL,0
		STOSB
		PUSH	ES
		POP	DS
		POP	SI
		JMP	F65_35
F65_33:		MOV	SI,OFFSET MemUnknown_S	; Unknown program
F65_34:		MOV	DS,CS:SubsSeg
F65_35:		POP	DI			; Save name
		ADD	DI,OFFSET MemName
		PUSH	ES
		MOV	ES,ES:WorkBuffer
		MOV	CX,12
F65_36:		LODSB
		CMP	AL,0
		JNE	F65_37
		DEC	SI
		MOV	AL,' '
F65_37:		STOSB
		LOOP	F65_36
		POP	DS
		POP	SI
		INC	DS:ListTotal
F65_38:		CMP	WORD PTR DS:[SI+4],0
		JNE	F65_12
F65_39:		PUSH	DS
		POP	ES

		XOR	SI,SI			; Vectors
		MOV	CX,4
F65_40:		XOR	AX,AX
		MOV	DS,AX
		LODSW
		SHR	AX,CL
		MOV	DI,AX
		LODSW
		ADD	AX,DI			; AX = vector address
		PUSH	ES
		POP	DS
		MOV	BX,DX
		CMP	AX,DS:[BX]
		JB	F65_44
F65_41:		CMP	WORD PTR DS:[BX+4],0
		JE	F65_44
		ADD	BX,4
		CMP	AX,DS:[BX]
		JAE	F65_41
		MOV	DI,DS:[BX-2]		; DI = owner
		MOV	BP,DS:ListTotal
		INC	BP
		MOV	DS,DS:WorkBuffer
		XOR	BX,BX
F65_42:		DEC	BP
		JE	F65_44
		MOV	AX,DS:[BX].MemSegment
		CMP	AX,DI
		JE	F65_43
		ADD	BX,SIZE MemEnt
		JMP	F65_42
F65_43:		TEST	DS:[BX].MemFlags,1
		JNZ	F65_44
		MOV	AL,DS:[BX].MemIntNumb
		INC	AX
		MOV	DS:[BX].MemIntNumb,AL
		CMP	AL,MaxVect
		JA	F65_44
		MOV	AH,0
		ADD	BX,AX
		MOV	DS:[BX-1].MemInts,CH
F65_44:		INC	CH
		JNE	F65_40

		PUSH	ES			; Dialog
		POP	DS
		MOV	AL,LenStr
		MOV	AH,DS:Lines
		SUB	AH,12
		MOV	DS:ListWindow,AX
		MOV	DS:ListBase,405h
		MOV	DS:ListFirst,0
		MOV	DS:ListCursor,0
		MOV	DS:ListLine,OFFSET MemItem
		MOV	DS:ListHotKey,0
		MOV	BP,OFFSET MemInfo_B
		CALL	Dialog
		JNE	Func04

		MOV	DS,CS:SubsSeg		; Prepare for release
		MOV	DS,DS:ResidentPoint.Segm
		MOV	SI,ES:ReleaseData
		MOV	AX,DS:[SI]
		MOV	ES:MemRelease.Offs,AX
		MOV	ES:MemRelease.Segm,DS
		MOV	CX,DS:[SI+2]
		MOV	DI,CX
		ADD	DI,DI
		JE	Func04
		ADD	DI,SI
		INC	DI
		INC	DI
		CALL	SavMous			; Save mouse cursor
		MOV	AL,0
		CALL	CurMous
		PUSH	DS			; Restore vectors
		CALL	RestVct
		POP	DS
		MOV	DX,ES			; Save INT 23h
		MOV	AX,3523h
		INT	21h
		PUSH	ES
		PUSH	BX
		MOV	ES,DX
F65_50:		PUSH	DS			; Release
		PUSH	CX
		MOV	DS,DS:[DI]
		MOV	AX,DS:[0]
		MOV	CX,ES:ListTotal
		XOR	SI,SI
		PUSH	DS
		MOV	DS,ES:WorkBuffer
F65_51:		CMP	AX,DS:[SI].MemSegment
		JE	F65_52
		ADD	SI,SIZE MemEnt
		LOOP	F65_51
		POP	DS
		JMP	F65_53
F65_52:		TEST	DS:[SI].MemFlags,2
		POP	DS
		JZ	F65_53
		CALL	ES:MemRelease
F65_53:		POP	CX
		POP	DS
		DEC	DI
		DEC	DI
		LOOP	F65_50
		POP	DX			; Set INT 23h
		POP	DS
		MOV	AX,2523h
		INT	21h
		PUSH	ES			; Set vectors
		POP	DS
		CALL	HookVct
		PUSH	DS
		POP	ES
		CALL	IniMous			; Restore mouse
		JMP	Func04

;---------------------------------------------------------------
;	Memory info subfinction
;---------------------------------------------------------------
		.CODE	Subs
MemProc		PROC	NEAR
		CMP	AX,0FFFFh
		JE	@@Init
		CMP	AH,0FCh
		JAE	@@DialList
		CMP	AL,0Dh
		JE	@@Enter
		CMP	AL,0Ah
		JE	@@Enter
		JMP	@@DialList
  @@Init:	MOV	DS:ListSelect,OFFSET MemSelect	; Init
		MOV	DS:ListLoop,1
		MOV	DS:MinMnuFlag,1
  @@DialList:	CALL	DialogList
		CMP	AL,0Dh
		JE	@@CheckAll
		CMP	AL,0Ah
		JE	@@CheckAll
		JMP	@@Exit
  @@Enter:	OR	CL,CL
		JNE	@@Exit
  @@CheckAll:	MOV	CX,DS:ListTotal		; Check programs
		MOV	DS,DS:WorkBuffer
		JCXZ	@@Done
		MOV	BX,0FFFFh
		XOR	SI,SI
  @@ChkSel:	TEST	DS:[SI].MemFlags,2
		JNZ	@@ChkRelease
		ADD	SI,SIZE MemEnt
		LOOP	@@ChkSel
		MOV	SI,ES:ListCursor
		CALL	MemAddr
		TEST	DS:[SI].MemFlags,1
		JZ	@@NoRelease
		OR	DS:[SI].MemFlags,2
		MOV	BX,SI
		JMP	@@ChkRelease
  @@NoRelease:	MOV	BP,OFFSET MemError2_B
		TEST	DS:[SI].MemFlags,4
		JNZ	@@ErrDial
		MOV	BP,OFFSET MemError1_B
  @@ErrTSR:	MOV	DX,OFFSET MemError1_T
		CALL	MemSetName
  @@ErrDial:	CALL	Dialog
		XOR	AX,AX
		CMP	BX,0FFFFh
		JE	@@Exit
		AND	DS:[SI].MemFlags,0FDh
		JMP	@@Exit
  @@ChkRelease:	MOV	DI,OFFSET MemVectors
		MOV	CX,256
		MOV	AL,0
		REP	STOSB
		MOV	DS,CS:ResidentPoint.Segm
		MOV	SI,ES:ReleaseData
		MOV	CX,DS:[SI+2]
		MOV	DI,CX
		ADD	DI,DI
		JE	@@Done
		ADD	DI,SI
		INC	DI
		INC	DI
  @@ChkTSR:	PUSH	DS			; Check TSRs for release
		PUSH	CX
		MOV	DS,DS:[DI]
		MOV	AX,DS:[0]
		MOV	CX,ES:ListTotal
		XOR	SI,SI
		PUSH	DS
		MOV	DS,ES:WorkBuffer
  @@FindEntry:	CMP	AX,DS:[SI].MemSegment
		JE	@@Selected
		ADD	SI,SIZE MemEnt
		LOOP	@@FindEntry
		POP	DS
		JMP	@@Next
  @@Selected:	TEST	DS:[SI].MemFlags,2
		POP	DS
		JZ	@@MarkVects
		MOV	CL,DS:[14]
		MOV	CH,0
		JCXZ	@@Next
		MOV	DI,15
  @@ChkVect:	MOV	AL,DS:[DI]
		MOV	AH,0
		MOV	BP,AX
		CMP	AH,ES:MemVectors[BP]
		JE	@@NextVect
		POP	CX
		POP	DS
		MOV	DS,ES:WorkBuffer
		MOV	BP,OFFSET MemError3_B
		JMP	@@ErrTSR
  @@NextVect:	ADD	DI,5
		LOOP	@@ChkVect
		JMP	@@Next
  @@MarkVects:	MOV	CL,DS:[14]
		MOV	CH,0
		JCXZ	@@Next
		MOV	SI,15
  @@MarkVect:	MOV	AL,DS:[SI]
		MOV	AH,0
		MOV	BP,AX
		MOV	ES:MemVectors[BP],1
		ADD	SI,5
		LOOP	@@MarkVect
  @@Next:	POP	CX
		POP	DS
		DEC	DI
		DEC	DI
		LOOP	@@ChkTSR
  @@Done:	MOV	AX,1C0Dh
  @@Exit:	RET
MemProc		ENDP

;---------------------------------------------------------------
;	Output line
;---------------------------------------------------------------
		.CODE	Subs
MemItem		PROC	NEAR
		USES	AX,CX,SI,DI,BP,ES
		MOV	AX,SI
		SUB	AX,DI
		ADD	BX,DS:ListBase
		ADD	BH,AL
		MOV	DI,OFFSET LineBuf1
		MOV	CL,DS:ListWindow.X
		MOV	CH,0
		MOV	AL,' '
		PUSH	BX
		PUSH	CX
		PUSH	DI
		REP	STOSB
		POP	DI
		CALL	MemAddr
		MOV	DS,DS:WorkBuffer
		PUSH	DI
		MOV	AL,' '
		TEST	DS:[SI].MemFlags,1
		JZ	@@Status
		MOV	AL,''
		TEST	DS:[SI].MemFlags,2
		JZ	@@Status
		INC	BP
		INC	BP
		MOV	AL,'û'
  @@Status:	STOSB
		ADD	DI,8
		MOV	AX,DS:[SI].MemSize
		MOV	CL,4
		MOV	BL,AH
		MOV	BH,0
		SHR	BX,CL
		SHL	AX,CL
		MOV	CX,AX
		MOV	AL,3
		CALL	Number
		SUB	DI,20
		XOR	BX,BX
		MOV	CX,DS:[SI].MemBlocks
		MOV	AL,2
		CALL	Number
		POP	DI
		PUSH	DI
		INC	DI
		INC	DI
		MOV	AX,DS:[SI].MemSegment
		PUSH	AX
		MOV	AL,AH
		CALL	HexByt
		STOSW
		POP	AX
		CALL	HexByt
		STOSW
		PUSH	SI
		ADD	DI,19
		ADD	SI,OFFSET MemName
		MOV	CX,12
		REP	MOVSB
		POP	SI
		MOV	CX,MaxVect
		MOV	DL,DS:[SI].MemIntNumb
		ADD	SI,OFFSET MemInts
		INC	DI
		INC	DI
  @@Vector:	CMP	DL,0
		JE	@@PutLine
		INC	DI
		LODSB
		CALL	HexByt
		STOSW
		DEC	DL
		LOOP	@@Vector
		JE	@@PutLine
		DEC	DI
		DEC	DI
		MOV	AL,'.'
		STOSB
		STOSB
		STOSB
  @@PutLine:	PUSH	ES
		POP	DS
		POP	SI
		POP	CX
		POP	BX
		MOV	DL,CL
		MOV	DH,1
		CALL	BasAdr
		MOV	AH,CS:[BP]
		CALL	Color
  @@Move:	LODSB
		STOSW
		LOOP	@@Move
		CLC
		RET
MemItem		ENDP

;---------------------------------------------------------------
;	Select memory info item
;---------------------------------------------------------------
		.CODE	Subs
MemSelect	PROC	NEAR
		USES	SI,DS
		CALL	MemAddr
		MOV	DS,DS:WorkBuffer
		TEST	DS:[SI].MemFlags,1
		JZ	@@Error
		CMP	AL,1
		JB	@@UnSelect
		JE	@@Select
		MOV	AL,0
		TEST	DS:[SI].MemFlags,2
		JZ	@@Exit
		INC	AX
		JMP	@@Exit
  @@UnSelect:	TEST	DS:[SI].MemFlags,2
		JZ	@@Error
		AND	DS:[SI].MemFlags,0FDh
		JMP	@@Exit
  @@Select:	TEST	DS:[SI].MemFlags,2
		JNZ	@@Error
		OR	DS:[SI].MemFlags,2
		JMP	@@Exit
  @@Error:	STC
  @@Exit:	RET
MemSelect	ENDP

;---------------------------------------------------------------
;	Set address of entry
;		   SI - number of entry
;	Return:	   SI - address of entry
;---------------------------------------------------------------
		.CODE	Subs2
MemAddr		PROC	FAR
		USES	AX,DX
		MOV	AX,SIZE MemEnt
		MUL	SI
		MOV	SI,AX
		RET
MemAddr		ENDP

;---------------------------------------------------------------
;	Copy memory block name
;		DS:SI - memory entry
;---------------------------------------------------------------
		.CODE	Subs2
MemSetName	PROC	FAR
		USES	AX,BX,CX,SI,DI
		ADD	SI,OFFSET MemName
		MOV	DI,OFFSET LineBuf1
		MOV	CX,12
		MOV	BX,DI
  @@MoveChar:	LODSB
		STOSB
		CMP	AL,' '
		JE	@@Next
		MOV	BX,DI
  @@Next:	LOOP	@@MoveChar
		MOV	BYTE PTR ES:[BX],0
		RET
MemSetName	ENDP
