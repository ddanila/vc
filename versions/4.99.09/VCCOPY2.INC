;---------------------------------------------------------------
;		Copy, Rename, Move subroutines
;
;		Version:	16.07.2000
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Compare source and destination names
;		   AL - 0=uppercase all, 1=uppercase path
;	Return:	   CY - error or not equal
;---------------------------------------------------------------
F35_Compare	PROC	NEAR
		USES	BX,CX,SI,DI,DS
		PUSH	ES
		POP	DS
		MOV	CH,AL
		MOV	SI,OFFSET LineBuf1
		MOV	DI,OFFSET LineBuf3
		MOV	BX,DI
		CALL	F35_TrueName
		JC	@@Exit
		MOV	SI,OFFSET LineBuf2
		MOV	DI,OFFSET LineBuf4
		CALL	F35_TrueName
		JNC	@@Compare
		CMP	AX,53h
		JE	@@Error
  @@NotEqu:	XOR	AX,AX
  @@Error:	STC
		JMP	@@Exit
  @@Compare:	MOV	SI,BX
		CALL	CompNormLines
		JNE	@@NotEqu
		OR	CH,CH
		JE	@@Exit
		MOV	AX,WORD PTR DS:LineBuf1[0]
		CMP	AH,':'
		JE	@@GetFS
		MOV	AL,DS:DosPath[0]
  @@GetFS:	CALL	GetFS
		OR	AX,AX
		JE	@@Exit
		MOV	SI,OFFSET LineBuf2
		CALL	CutPath
		MOV	DI,SI
		MOV	SI,OFFSET LineBuf1
		CALL	CutPath
		CALL	StrLen
		REPE	CMPSB
		JNE	@@NotEqu
  @@Exit:	RET
F35_Compare	ENDP

;---------------------------------------------------------------
;	Full pathname
;		DS:SI - pathname
;		ES:DI - buffer
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
F35_NormPath	PROC	NEAR
		CMP	BYTE PTR ES:[DI+514-1],1
		JE	@@Exit
		CALL	NormPath
		MOV	BYTE PTR ES:[DI+514-1],0
		JC	@@Exit
		MOV	BYTE PTR ES:[DI+514-1],1
  @@Exit:	RET
F35_NormPath	ENDP

;---------------------------------------------------------------
;	Short pathname
;		DS:SI - pathname
;		ES:DI - buffer
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
F35_ShortPath	PROC	NEAR
		MOV	BYTE PTR ES:[DI+514-1],0
		CALL	ShortName
		JNC	@@Exit
		CMP	AX,7100h
		STC
		JNE	@@Exit
		CALL	F35_NormPath
  @@Exit:	RET
F35_ShortPath	ENDP

;---------------------------------------------------------------
;	True pathname
;		DS:SI - pathname
;		ES:DI - buffer
;	Return:	   CY - if error
;		   AX - error code
;---------------------------------------------------------------
F35_TrueName	PROC	NEAR
		USES	BX,CX,DX,SI,DI
		CMP	BYTE PTR ES:[DI+514-1],2
		JE	@@Exit
		CALL	TrueName
		JNC	@@NoErr
		MOV	DL,0
		CMP	AX,2
		JE	@@TruePath
		CMP	AX,3
		JNE	@@Status
@@TruePath:	MOV	BX,SI
		CALL	CutPath
		XCHG	BX,SI
		MOV	CX,'.'
		XCHG	CX,DS:[BX]
		CALL	TrueName
		MOV	DS:[BX],CX
		JC	@@Status
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		CALL	AddFile
		MOV	SI,BX
		CALL	MvzLine
  @@NoErr:	MOV	DL,2
  @@Status:	MOV	ES:[DI+514-1],DL
		CMP	DL,1
  @@Exit:	RET
F35_TrueName	ENDP

;---------------------------------------------------------------
;	Check target free space
;		DS:DX - target file
;		   AL - 0=don't add target file size
;	Return:	   AL - action: Continue/Another drive/Skip/Cancel
;---------------------------------------------------------------
F35_ChkTarget	PROC	NEAR
		USES	CX,DX
		CMP	DS:ChkTarget,0
		JE	@@ExitOk
		XOR	SI,SI
		XOR	DI,DI
		CMP	AL,0
		JE	@@SetDrive
		MOV	DS:BlocEr,2
		MOV	AL,40h
		CALL	OpenFil
		MOV	DS:BlocEr,0
		JNC	@@GetSize
		CMP	AX,53h
		JNE	@@SetDrive
		CMP	DS:ErrDat,2
		JNE	@@Exit
		MOV	AL,2
		JMP	@@Exit
  @@GetSize:	PUSH	DX
		XOR	CX,CX
		XOR	DX,DX
		MOV	BX,AX
		MOV	AX,4202h
		CALL	Intr21
		MOV	DI,DX
		MOV	SI,AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	DX
  @@SetDrive:	MOV	BX,DX
		MOV	AX,DS:[BX]
		CMP	AH,':'
		JE	@@FreeSpace
		MOV	AL,DS:DosPath[0]
  @@FreeSpace:	CALL	UpCase
		MOV	DX,DI
		MOV	DI,OFFSET TmpBuf
		CALL	GetDiskInfo
		JNC	@@ChkSpace
		CMP	AX,53h
		JE	@@Exit
  @@ExitOk:	MOV	AL,0
		JMP	@@Exit
  @@ChkSpace:	MOV	CX,SI
		MOV	BX,DI
		LEA	SI,[DI+8]
		ADD	DI,12
		CALL	LongMul
		MOV	SI,DS:[DI+4].Lo
		OR	SI,DS:[DI+4].Hi
		JNE	@@ExitOk
		ADD	CX,DS:[DI].Lo
		ADC	DX,DS:[DI].Hi
		JC	@@ExitOk
		MOV	AX,DS:CopySize.Lo
		MOV	BX,DS:CopySize.Hi
		SUB	AX,DS:CopyWrite.Lo
		SBB	BX,DS:CopyWrite.Hi
		SUB	CX,AX
		SBB	DX,BX
		JNC	@@ExitOk
		MOV	AL,DS:CopyWarnMenu
		MOV	DS:MinMnuDefault,AL
		MOV	BP,OFFSET CopyWarning_B
		CALL	F35_Table
		CALL	Dialog
		MOV	DS:CopyWarnMenu,AL
  @@Exit:	RET
F35_ChkTarget	ENDP

;---------------------------------------------------------------
;	Create target path
;		DS:DX - filename
;	Return:	   CY - error
;---------------------------------------------------------------
F35_MkPath	PROC	NEAR
		USES	BX,CX,SI
		MOV	SI,DX
		LODSW
		CMP	AX,'\\'
		JE	@@FindNext
		CMP	AH,':'
		JE	@@ChkSlash
		DEC	SI
		DEC	SI
  @@ChkSlash:	LODSB
		CMP	AL,'\'
		JE	@@FindNext
		CMP	AL,'/'
		JE	@@FindNext
		DEC	SI
  @@FindNext:	LODSB
		CMP	AL,0
		JE	@@Exit
		CMP	AL,'\'
		JE	@@ChkDir
		CMP	AL,'/'
		JNE	@@FindNext
  @@ChkDir:	MOV	BL,0
		XCHG	DS:[SI-1],BL
		MOV	AX,4300h
		CALL	Intr21
		XCHG	DS:[SI-1],BL
		JNC	@@FindNext
		CMP	AX,53h
		STC
		JE	@@Exit
		XCHG	DS:[SI-1],BL
		MOV	AH,39h
		CALL	Intr21
		XCHG	DS:[SI-1],BL
		JC	@@Exit
		MOV	DS:GroupChg,1
		CMP	DS:CopyMkPath1,0
		JNE	@@FindNext
		MOV	DS:CopyMkPath1,SI
		JMP	@@FindNext
  @@Exit:	RET
F35_MkPath	ENDP

;---------------------------------------------------------------
;	Update TREEINFO
;		DS:DX - filename
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
F35_UpdPath	PROC	NEAR
		USES	BX,CX,DX,SI,DI,DS
		MOV	AL,0
		MOV	BX,DS:GroupTree2
		CMP	BX,1
		JE	@@Exit
		MOV	SI,DX
		MOV	DI,OFFSET HelpBuf
		MOV	DX,DI
		CALL	NormPath
		JC	@@Exit
		MOV	SI,DX
		CALL	CutPath
		CMP	SI,DX
		JBE	@@ChkTree
		DEC	SI
		MOV	BYTE PTR DS:[SI],0
  @@ChkTree:	MOV	DS,BX
		OR	BX,BX
		JNZ	@@AddEntry
		MOV	AX,ExpandSize
		CALL	GetTreeInfo
		MOV	ES:GroupTree2,1
		JC	@@Exit
		MOV	ES:GroupTree2,DS
		CALL	AllocateBlock
  @@AddEntry:	CALL	F35_AddTree
  @@Exit:	RET
F35_UpdPath	ENDP

;---------------------------------------------------------------
;	Delete or cut target file
;---------------------------------------------------------------
F35_DelCut	PROC	NEAR
		MOV	AX,4300h
		CALL	Intr21
		JC	@@Exit
		TEST	CL,1
		JZ	@@Delete
		AND	CX,3Eh
		MOV	AX,4301h
		CALL	Intr21
		JC	@@Exit
  @@Delete:	CMP	DS:CopyAppend,0
		JNZ	@@Trunc
		MOV	AH,41h
		CALL	Intr21
		JMP	@@Exit
  @@Trunc:	MOV	AL,12h
		CALL	OpenFil
		JC	@@Exit
		PUSH	DX
		MOV	BX,AX
		MOV	DX,DS:CopyAppOffset.Lo
		MOV	CX,DS:CopyAppOffset.Hi
		MOV	AX,4200h
		CALL	Intr21
		XOR	CX,CX
		MOV	AH,40h
		CALL	Intr21
		MOV	DX,DS:CopyDate
		MOV	CX,DS:CopyTime
		MOV	AX,5701h
		CALL	Intr21
		MOV	AH,3Eh
		CALL	Intr21
		POP	DX
		JC	@@Exit
		MOV	CX,DS:CopyAttrib
		MOV	AX,4301h
		CALL	Intr21
  @@Exit:	RET
F35_DelCut	ENDP

;---------------------------------------------------------------
;	Set date and time
;---------------------------------------------------------------
F35_SetDate	PROC	NEAR
		CMP	DS:CopySplit,1
		JZ	@@Exit
		MOV	DX,DS:CopyDate
		MOV	CX,DS:CopyTime
		MOV	AX,5701h
		CALL	Intr21
  @@Exit:	RET
F35_SetDate	ENDP

;---------------------------------------------------------------
;	Progress bar
;---------------------------------------------------------------
F35_PerCent	PROC	NEAR
		USES	AX,BX,CX,DX,SI,DI,DS,ES
		PUSH	BP
		PUSH	ES
		POP	DS
		MOV	AX,DS:CopyRead.Lo
		MOV	BX,DS:CopyRead.Hi
		XOR	DX,DX
		ADD	AX,DS:CopyWrite.Lo
		ADC	BX,DS:CopyWrite.Hi
		ADC	DX,0
		XOR	SI,SI
		XOR	DI,DI
		XOR	BP,BP
		MOV	CX,32
		PUSH	CX
  @@Mul32:	ADD	SI,AX
		ADC	DI,BX
		ADC	BP,DX
		LOOP	@@Mul32
		POP	CX
		MOV	AX,DS:CopySize.Lo
		MOV	BX,DS:CopySize.Hi
		MOV	DX,AX
		OR	DX,BX
		JE	@@Display
		PUSH	CX
		XOR	DX,DX
		ADD	AX,AX
		ADC	BX,BX
		ADC	DX,DX
		MOV	CX,0FFFFh
  @@Div32:	INC	CX
		SUB	SI,AX
		SBB	DI,BX
		SBB	BP,DX
		JAE	@@Div32
		POP	AX
		CMP	CX,AX
		JBE	@@Display
		MOV	CX,AX
  @@Display:	MOV	BL,DS:Columns
		SHR	BL,1
		SUB	BL,16
		MOV	BH,11
		CALL	SftEGA
		CALL	BasAdr
		MOV	AH,ES:[DI+1]
		MOV	AL,DS:ProgressBars[1]
		REP	STOSW
		MOV	DX,120h
		CALL	Window
		POP	BP
		RET
F35_PerCent	ENDP

;---------------------------------------------------------------
;	Add tree entry in TREEINFO
;		   DS - segment of treeinfo
;		ES:DX - target path
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
F35_AddTree	PROC	NEAR
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,DX
		MOV	DI,OFFSET LineBuf4
		MOV	BX,OFFSET LineBuf3
		MOV	BYTE PTR DS:[DI+514-1],0
		MOV	BYTE PTR DS:[BX+514-1],0
		CALL	PrepAddTree
		POP	DS
		JC	@@Exit
		MOV	DX,DI
		CALL	FindTreeEntry
		JNC	@@Exit
		PUSH	SI
		PUSH	DX
		PUSH	BX
		MOV	BX,DS
		INC	BX
		MOV	SI,OFFSET AllSegs_L
		CALL	FindNextSeg
		DEC	BX
		SUB	DX,BX
		MOV	CL,4
		SHL	DX,CL
		MOV	BP,DX
		PUSH	DI
		XOR	CX,CX
  @@Length1:	INC	CX
  @@Length2:	MOV	AL,ES:[DI]
		INC	DI
		CMP	AL,'\'
		JE	@@Length1
		CMP	AL,0
		JNE	@@Length2
		MOV	BX,DI
		POP	DI
		SUB	BX,DI
		MOV	AX,SIZE DirEnt
		MUL	CX
		ADD	AX,BX
		POP	BX
		PUSH	DI
		PUSH	AX
		MOV	DI,BX
		CALL	StrLen
		POP	AX
		POP	DI
		ADD	AX,CX
		MOV	SI,DS:TreeLen
		CALL	TreeNumb2Adr1
		INC	SI
		INC	SI
		ADD	AX,SI
		POP	DX
		POP	SI
		JC	@@Exit
		CMP	AX,BP
		JBE	@@AddEntry
		PUSH	SI
		MOV	SI,OFFSET AllSegs_L
		MOV	AX,ExpandSize
		CALL	ExpandBlock
		POP	SI
		MOV	BP,AX
  @@AddEntry:	CALL	AddTreeEntry
		JNC	@@Exit
		MOV	ES:GroupTreeChg2,1
		CLC
  @@Exit:	RET
F35_AddTree	ENDP

;---------------------------------------------------------------
;	Get attributes, clear RO from CD-ROM
;		DS:DX - file
;	Return:	   CX - attributes
;		   CY - error, AX - error code
;---------------------------------------------------------------
F35_GetAttr	PROC	NEAR
		MOV	AX,4300h
		CALL	Intr21
		JC	@@Exit
		CMP	DS:ClearRO,0
		JE	@@Exit
		PUSH	SI
		PUSH	CX
		PUSH	BX
		MOV	SI,DX
		LODSW
		CMP	AH,':'
		JE	@@ChkDrv
		MOV	AL,DS:DosPath[0]
  @@ChkDrv:	CALL	UpCase
		SUB	AL,'A'
		CBW
		MOV	CX,AX
		XOR	BX,BX
		MOV	AX,150Bh
		INT	2Fh
		XOR	BX,0ADADh
		POP	BX
		POP	CX
		POP	SI
		JNE	@@Exit
		OR	AX,AX
		JE	@@Exit
		AND	CL,0FEh
  @@Exit:	RET
F35_GetAttr	ENDP

;---------------------------------------------------------------
;	Set real name
;		DS:DX - file name
;	Return:	   CY - error, AX=error code
;---------------------------------------------------------------
F35_SetName	PROC	NEAR
		USES	SI,DI
		CMP	DS:CopyAppend,0
		JNE	@@Exit
		MOV	DI,DX
		MOV	AX,DS:[DI]
		CMP	AH,':'
		JE	@@GetFS
		MOV	AL,DS:DosPath[0]
  @@GetFS:	CALL	GetFS
		OR	AX,AX
		JE	@@Exit
		MOV	AH,56h
		CALL	F35_FuncShort
  @@Exit:	RET
F35_SetName	ENDP

;---------------------------------------------------------------
;	Save target file in buffer
;---------------------------------------------------------------
F35_SaveTarget	PROC	NEAR
		USES	AX,CX,DX,SI,DI,BP,DS,ES
		MOV	AX,ES:GroupFiles
		MOV	DS,AX
		OR	AX,AX
		JNE	@@ChkSize
		MOV	DS,ES:WorkBuffer
		MOV	ES:GroupFiles,DS
		CALL	F35_NewBlock
		OR	AX,AX
		JNE	@@Create
		INC	AX
  @@Create:	MOV	DS:[0],AX
		MOV	WORD PTR DS:[2],4
		MOV	BP,AX
		CALL	AllocateBlock
  @@ChkSize:	MOV	DI,DS:[2]
		MOV	AX,DS:[0]
		SUB	AX,DI
		CMP	AX,LenPath+1
		JA	@@SaveName
		CALL	F35_NewBlock
		PUSH	SI
		MOV	SI,OFFSET AllSegs_L
		CALL	ExpandBlock
		POP	SI
		MOV	DS:[0],AX
		XOR	BP,BP
		SUB	AX,DI
		CMP	AX,LenPath+1
		JBE	@@Restore
  @@SaveName:	MOV	DX,OFFSET LineBuf2
		CALL	ChgDisk
		JC	@@Exit
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	SI,DX
		MOV	BP,DS:CopyMkPath1
		OR	BP,BP
		JE	@@Convert
		MOV	DL,0
		XCHG	DL,DS:[BP-1]
  @@Convert:	CALL	TrueName
		JNC	@@Length
		XOR	AX,53h
		JNE	@@Restore
		STC
		JMP	@@Exit
  @@Length:	MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		MOV	ES:[2],DI
  @@Restore:	OR	BP,BP
		JE	@@Exit
		MOV	DS:[BP-1],DL
		PUSH	DS
		POP	ES
		MOV	DS:CopyMkPath1,0
		CALL	F35_SaveTarget
  @@Exit:	RET
F35_SaveTarget	ENDP

F35_NewBlock	PROC	NEAR
		USES	BX,CX
		MOV	AX,ExpandSize
		MOV	BX,ES
		ADD	BX,ES:SizeDataSeg
		SUB	BX,ES:WorkBuffer
		CMP	BX,1000h
		JAE	@@Exit
		MOV	CL,4
		SHL	BX,CL
		XOR	AX,AX
		SUB	BX,MinView/2
		JBE	@@Exit
		MOV	AX,ExpandSize
		CMP	AX,BX
		JBE	@@Exit
		MOV	AX,BX
  @@Exit:	RET
F35_NewBlock	ENDP

;---------------------------------------------------------------
;	Check file in buffer
;		ES:DI - file
;		   DS - buffer
;	Return:	   CY - found
;		   NC - not found
;---------------------------------------------------------------
ChkInBuffer	PROC	FAR
		USES	AX,CX,SI
		MOV	SI,DS
		OR	SI,SI
		JE	@@Exit
		CALL	StrLen
		DEC	CX
		MOV	SI,4
  @@Compare:	CMP	SI,DS:[2]
		JAE	@@Exit
		PUSH	DI
		PUSH	SI
		PUSH	CX
		REPE	CMPSB
		LODSB
		POP	CX
		POP	SI
		POP	DI
		JNE	@@Next
		CMP	AL,0
		JE	@@Found
		CMP	AL,'\'
		JE	@@Found
  @@Next:	LODSB
		CMP	AL,0
		JNE	@@Next
		JMP	@@Compare
  @@Found:	STC
  @@Exit:	RET
ChkInBuffer	ENDP

;---------------------------------------------------------------
;	Do function with uppercased short name
;---------------------------------------------------------------
F35_FuncShort	PROC	NEAR
		USES	SI
		PUSH	DI
		PUSH	AX
		MOV	SI,DI
		MOV	DI,OFFSET CopyShortNamBuf
		MOV	BYTE PTR DS:[DI],0
		CALL	CutPath
		CMP	DS:ConvCase,0
		JE	@@Func
		MOV	AL,0
		CALL	ChkShortNam
		JC	@@Func
		PUSH	SI
		CALL	MvzLine
		POP	SI
		CALL	PathLin
  @@Func:	POP	AX
		POP	DI
		CALL	Intr21
		PUSHF
		PUSH	DI
		MOV	DI,SI
		MOV	SI,OFFSET CopyShortNamBuf
		CMP	BYTE PTR DS:[SI],0
		JE	@@Exit
		PUSH	AX
		CALL	MvzLine
		POP	AX
  @@Exit:	POP	DI
		POPF
		RET
F35_FuncShort	ENDP

;---------------------------------------------------------------
;	Move directory
;		DS:DX - source directory
;		DS:DI - target directory
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
F35_MoveDir	PROC	NEAR
		USES	DX,DI
		OR	CL,CL
		JE	@@Move
		MOV	DX,OFFSET LineBuf3
		MOV	SI,DX
		CALL	CutPath
		CMP	SI,OFFSET LineBuf3+3
		JBE	@@SetDir
		DEC	SI
  @@SetDir:	MOV	CH,0
		XCHG	CH,DS:[SI]
		MOV	AH,3Bh
		CALL	Intr21
		MOV	DS:[SI],CH
		JC	@@Exit
		MOV	AX,DS:[DI]
		OR	AL,AL
		JE	@@Move
		CMP	AH,':'
		JNE	@@ChkSlash
		MOV	AL,DS:[DI+2]
  @@ChkSlash:	CMP	AL,'\'
		JE	@@Move
		CMP	AL,'/'
		JE	@@Move
		MOV	SI,OFFSET DosShortPath
		PUSH	DI
		MOV	DI,OFFSET LineBuf4
		CALL	MvzLine
		CALL	AddFile
		POP	SI
		PUSH	SI
		CALL	MvzLine
		POP	DI
		MOV	SI,OFFSET LineBuf4
		MOV	BYTE PTR DS:[SI+514-1],0
		PUSH	DI
		CALL	MvzLine
		POP	DI
  @@Move:	MOV	AH,56h
		CALL	Intr21
		JNC	@@Exit
		OR	CL,CL
		JE	@@Error
		PUSH	AX
		MOV	DS:BlocEr,1
		MOV	DX,OFFSET DosShortPath
		MOV	AH,3Bh
		CALL	Intr21
		MOV	DS:BlocEr,0
		POP	AX
  @@Error:	STC
  @@Exit:	RET
F35_MoveDir	ENDP

;---------------------------------------------------------------
;	Ask for overwrite
;		CS:DX - pointers table
;		   CX - destination attributes
;	Return:	   AL - dialog menu
;		   CY - fatal error
;---------------------------------------------------------------
		.CODE	Subs
F35_Overwrite	PROC	FAR
		USES	BX,CX,SI,DI,BP
		MOV	BP,CX
		MOV	DI,OFFSET CopyOverSrc
		MOV	AX,DS:CopyAttrib
		MOV	CX,DS:CopySize.Lo
		MOV	BX,DS:CopySize.Hi
		MOV	SI,OFFSET CopyTime
		CALL	F35_PutAttribs
		PUSH	DX
		MOV	DX,OFFSET LineBuf2
		CALL	F35_GetAttribs
		POP	DX
		JNC	@@DestParms
		CMP	AX,53h
		STC
		JE	@@Exit
		XOR	SI,SI
		XOR	DI,DI
		OR	BP,8000h
  @@DestParms:	MOV	AX,BP
		MOV	DS:CopyDestTime,SI
		MOV	DS:CopyDestDate,DI
		MOV	DI,OFFSET CopyOverDest
		MOV	SI,OFFSET CopyDestTime
		CALL	F35_PutAttribs
		MOV	BP,OFFSET CopyOver_B
		CALL	Dialog
  @@Exit:	RET
F35_Overwrite	ENDP

F35_PutAttribs	PROC	NEAR
		PUSH	AX
		MOV	AL,' '
		CMP	DS:Country.TimeFmt,0
		JE	@@ChkSize
		STOSB
  @@ChkSize:	TEST	AH,80h
		JZ	@@Size
		MOV	CX,12
		REP	STOSB
		MOV	AL,'?'
		STOSB
		JMP	@Attrib
  @@Size:	MOV	AL,3
		CALL	Number
  @Attrib:	MOV	AL,' '
		STOSB
		POP	AX
		CALL	PutAttr
		MOV	AL,' '
		STOSB
		SUB	SI,OFFSET TimeF
		CALL	PutDate
		CMP	DS:Country.TimeFmt,0
		JNE	@@Time
		STOSB
  @@Time:	CALL	PutTime
		MOV	AL,0
		STOSB
		RET
F35_PutAttribs	ENDP

;---------------------------------------------------------------
;	Get size, date and time of file
;		DS:DX - file
;	Return:	   CY - error, AX = error code
;		BX CX - size
;		   SI - time
;		   DI - date
;---------------------------------------------------------------
		.CODE	Subs
F35_GetAttribs	PROC	FAR
		USES	DX
		CALL	ChgDisk
		MOV	AX,53h
		JC	@@Exit
		MOV	AL,40h
		CALL	OpenFil
		JC	@@Exit
		MOV	BX,AX
		MOV	AX,5700h
		CALL	Intr21
		MOV	SI,CX
		MOV	DI,DX
		XOR	CX,CX
		XOR	DX,DX
		MOV	AX,4202h
		CALL	Intr21
		MOV	CX,AX
		MOV	AH,3Eh
		CALL	Intr21
		MOV	BX,DX
  @@Exit:	RET
F35_GetAttribs	ENDP
