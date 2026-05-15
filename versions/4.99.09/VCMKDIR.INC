;---------------------------------------------------------------
;		Make directory
;
;		Version:	14.03.1998
;---------------------------------------------------------------

		.CONST
MkDir0_S	DB	"Make directory",0
MkDir1_S	DB	"Create the directory",0FFh,LenStr-20,0
MkDirError_S	DB	"Can't create the directory",0

		.DATA
MkDir_T		DW	MkDir0_S, LineBuf1
MkDir_P		DB	Box_C, BoxCu_C, 0, 0
		DW	IncSp_S
		DW	MkDir1_S
		DW	Empty_S
		DW	0
MkDir_B		BOXHD	< MkDir_P, 500h, 0, Dialog_K, Fls_M, MkDir_H, 0 >
		BOXMN	< 1, 0,0,0,0, 305h, LineBuf1, LenStrFull+LenStr >
		DB	0
MkDirError_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	IncSp_S
		DW	MkDirError_S
		DW	Line1_S
		DW	Ok_S
		DW	0
MkDirError_B	BOXHD	< MkDirError_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0

		.CODE
F37_00:		CMP	DS:Visible,0		; F7 - make directory
		JZ	F47_00
		MOV	CL,1
		CMP	DS:WinTyp,p_Tree
		JA	F47_00			; Info or QuickView panel
		JE	F47_01
		CMP	DS:ArcPnt,0             ; File panel
		JNE	Func04

F47_00:		MOV	CL,0			; Shift F7 - make directory
F47_01:		PUSH	ES
		POP	DS
		MOV	SI,OFFSET LineBuf1
		MOV	WORD PTR DS:[SI],0
		MOV	BP,OFFSET MkDir_B
		MOV	DX,OFFSET MkDir_T
		CALL	Dialog
		JNE	Func04
		CMP	BYTE PTR DS:[SI],0
		JE	Func04
		MOV	DI,OFFSET LineBuf3	; Add full path
		PUSH	DI
		OR	CL,CL
		JZ	F47_02
		MOV	AX,DS:[SI]
		CMP	AL,'\'
		JE	F47_02
		CMP	AL,'/'
		JE	F47_02
		CMP	AH,':'
		JE	F47_02
		INC	CX
		PUSH	DS
		PUSH	SI
		MOV	DS,DS:WCBcrn
		MOV	SI,OFFSET CrnPath
		CALL	MvzLine
		POP	SI
		POP	DS
		CALL	AddFile
F47_02:		CALL	MvzLine			; Canonize names
		POP	SI
		MOV	DI,OFFSET LineBuf2
		MOV	DX,DI
		MOV	CH,0
		CALL	NormPath
		JC	F47_06
		CALL	ChgDisk			; Create directory
		JC	Func04
		XOR	BX,BX
		MOV	SI,DX
		CALL	CutPath
		CMP	BYTE PTR DS:[SI],0
		JNE	F47_04
		DEC	SI
		CMP	SI,DX
		JE	F47_04
		CMP	BYTE PTR DS:[SI],':'
		JE	F47_04
F47_03:		MOV	BYTE PTR DS:[SI],0
F47_04:		MOV	AH,39h
		CALL	Intr21
		JNC	F47_05
		CMP	AX,53h
		JE	Func04
		MOV	SI,DX
		CALL	CutPath
		DEC	SI
		CMP	SI,DX
		JBE	F47_07
		CMP	BYTE PTR DS:[SI],':'
		JE	F47_07
		INC	BX
		JMP	F47_03
F47_05:		MOV	CH,1
		OR	BX,BX
		JE	F47_10
		DEC	BX
		PUSH	CX
		MOV	DI,DX
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		MOV	BYTE PTR DS:[DI-1],'\'
		POP	CX
		MOV	AH,39h
		CALL	Intr21
		JNC	F47_05
		MOV	BYTE PTR DS:[DI-1],0
F47_06:		CMP	AX,53h
		JE	Func04
F47_07:		PUSH	DX			; Error
		MOV	DX,OFFSET MkDir_T
		MOV	BP,OFFSET MkDirError_B
		CALL	Dialog
		POP	DX
		OR	CH,CH
		JZ	Func04
F47_10:		MOV	SI,DX			; Set path in tree panel
		MOV	DI,OFFSET LineBuf4
		MOV	BX,OFFSET LineBuf3
		CALL	PrepAddTree
		JC	FuncExit2
		MOV	SI,DI
		CMP	CL,2
		JNE	F47_11
		MOV	ES,DS:WCBcrn
		MOV	DI,OFFSET CrnPath
		MOV	CX,LenPath-1
		REP	MOVSB
		MOV	AL,0
		STOSB
		JMP	F47_20
F47_11:		MOV	DI,OFFSET DosShortPath	; Set path in file panel
		CALL	StrLen
		DEC	CX
		MOV	AX,CX
		REPE	CMPSB
		JNE	F47_20
		CMP	AX,3
		JE	F47_12
		LODSB
		CMP	AL,'\'
		JNE	F47_20
F47_12:		MOV	AX,ES:WCBcrn
		MOV	BX,AX
		INC	BX
		PUSH	SI
		MOV	SI,OFFSET GlobalSegs_L
		CALL	FindNextSeg
		POP	SI
		SUB	DX,AX
		MOV	CL,4
		SHL	DX,CL
		PUSH	AX
		MOV	DI,SI
F47_13:		LODSB
		CMP	AL,0
		JE	F47_14
		CMP	AL,'\'
		JNE	F47_13
F47_14:		MOV	CX,SI
		SUB	CX,DI
		POP	ES
		SUB	DX,ES:EndBuf
		MOV	AX,CX
		ADD	AX,SIZE FilEnt
		CMP	AX,DX
		JA	F47_20
		MOV	SI,ES:EndBuf
		PUSH	DS
		PUSH	SI
		PUSH	ES
		POP	DS
		CALL	Addr2Numb
		MOV	DS:FileCur,SI
		POP	SI
		POP	DS
		XCHG	SI,DI
		MOV	ES:[DI].AttrF,10h
		MOV	ES:[DI].LenShortNamF,CX
		MOV	ES:[DI].LenLongNamF,0
		ADD	DI,OFFSET NameF
		DEC	CX
		REP	MOVSB
		MOV	AL,0
		STOSB
		MOV	ES:EndBuf,DI
F47_20:		PUSH	DS			; Update TREEINFO
		POP	ES
		MOV	DX,OFFSET LineBuf4
		MOV	AX,0FFFFh
		CALL	GetTreeInfo
		JC	F47_21
		CALL	FindTreeEntry
		MOV	BX,OFFSET LineBuf3
		CALL	AddTreeEntry
		JNC	F47_21
		CALL	SaveTreeInfo
F47_21:		JMP	FuncExit2
