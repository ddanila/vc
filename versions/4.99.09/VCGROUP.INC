;---------------------------------------------------------------
;		Group functions
;
;		Version:	17.08.1998
;---------------------------------------------------------------

GroupFound	EQU	BYTE PTR TmpBuf[78]
GroupAll	EQU	BYTE PTR TmpBuf[79]
GroupAllSrcRO	EQU	BYTE PTR TmpBuf[80]
GroupAllDstRO	EQU	BYTE PTR TmpBuf[81]
GroupChg	EQU	BYTE PTR TmpBuf[82]
GroupTree1	EQU	WORD PTR TmpBuf[83]
GroupTreeChg1	EQU	BYTE PTR TmpBuf[85]
GroupTree2	EQU	WORD PTR TmpBuf[86]
GroupTreeChg2	EQU	BYTE PTR TmpBuf[88]
GroupDescr1	EQU	WORD PTR TmpBuf[89]
GroupDescrChg1	EQU	BYTE PTR TmpBuf[91]
GroupDescr2	EQU	WORD PTR TmpBuf[92]
GroupDescrChg2	EQU	BYTE PTR TmpBuf[94]
GroupAttr	EQU	BYTE PTR TmpBuf[95]
GroupFileNumb	EQU	WORD PTR TmpBuf[96]
GroupFiles	EQU	WORD PTR TmpBuf[98]

;---------------------------------------------------------------
;	Find files and dirs and do function
;---------------------------------------------------------------
DirFuncBuf	EQU	TmpBuf+130

		.CONST
PathErr_S	DB	'Path too long', 0

		.DATA
PathErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Error_S
		DW	PathErr_S
		DW	Line0_S
		DW	OkCan_S
		DW	0
PathErr_B	BOXHD	< PathErr_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0

		.CODE
DirFunc		PROC	FAR			; Find files in directory
		PUSH	ES
		POP	DS
		CMP	SP,380h
		JB	@@Overflow
		MOV	DI,OFFSET LineBuf1
		MOV	DX,DI
		MOV	CX,LenPath
		XOR	AX,AX
		REPNE	SCASB
		JE	@@SaveDescr1
  @@Overflow:	XOR	CX,CX
		XCHG	CX,DS:TaskStack
		MOV	DX,OFFSET LineBufs_T
		MOV	BP,OFFSET PathErr_B
		CALL	Dialog
		MOV	DS:TaskStack,CX
		JNZ	@@Error
  @@SkipExit:	MOV	BX,200h
		JMP	@@Exit
  @@SaveDescr1:	DEC	DI
		PUSH	DI
		MOV	AL,DS:DescrUpdate	; Update descriptions
		MOV	AH,DS:GroupAttr
		PUSH	AX
		PUSH	DS:GroupFileNumb
		CMP	DS:GroupMask[0],0
		JNE	@@SaveDescr2
		MOV	DS:DescrUpdate,0
		JMP	@@AddMask1
  @@SaveDescr2:	CALL	SaveDescrs
		JC	@@FuncExit
  @@AddMask1:	MOV	DS,CS:SubsSeg		; Add *.*
		MOV	SI,OFFSET Mask_S+1
		CMP	DI,DX
		JE	@@AddMask2
		MOV	AL,ES:[DI-1]
		CMP	AL,':'
		JE	@@AddMask2
		CMP	AL,'\'
		JE	@@AddMask2
		CMP	AL,'/'
		JE	@@AddMask2
		DEC	SI
  @@AddMask2:	CALL	MvzLine
		PUSH	ES
		POP	DS
		MOV	BX,1
		MOV	CX,37h
		MOV	AH,4Eh
		JMP	@@Find
  @@ChkName:	MOV	SI,OFFSET DTA.FilName	; Check found name
		CALL	ChkUpDir
		JE	@@FindNext
		MOV	DI,OFFSET GroupMask
		TEST	DS:DTA.FilAttr,10h
		JZ	@@TestMask
		CMP	DS:IncDirs,0
		JZ	@@FindNext
		CMP	BYTE PTR DS:[DI],0
		JE	@@AddName
		JMP	@@DoFunc
  @@TestMask:	CMP	BYTE PTR DS:[DI],0
		JE	@@AddName
		CALL	TestMask
		JZ	@@AddName
		MOV	SI,OFFSET FindData.WinLongName
		CMP	BYTE PTR DS:[SI],0
		JE	@@FindNext
		CALL	TestMask
		JNZ	@@FindNext
  @@AddName:	MOV	DS:GroupFound,1		; Add name to path
		MOV	SI,OFFSET DTA.FilName
		CMP	DS:ProcLongNams,0
		JZ	@@NameOffset
		CMP	DS:FindData.WinLongName,0
		JE	@@NameOffset
		MOV	SI,OFFSET FindData.WinLongName
  @@NameOffset:	MOV	DI,SI
		MOV	SI,OFFSET LineBuf1
		CALL	CutPath
		XCHG	SI,DI
		CALL	MvzLine
  @@DoFunc:	CALL	TstBrk
		JC	@@FuncExit
		PUSH	BX
		XOR	BX,BX
		MOV	AL,DS:DTA.FilAttr
		CALL	DS:FncAddr
		POP	BX
		OR	AH,AH
		JNZ	@@FuncExit
		CMP	AL,0
		JNZ	@@FindNext
		MOV	BL,0
  @@FindNext:	MOV	AH,4Fh			; Find name
  @@Find:	CALL	ChgDisk
		JC	@@FuncExit
		CALL	Intr21
		JNC	@@ChkName
		CMP	AX,53h
		JNE	@@SaveDescr3
  @@FuncExit:	MOV	BX,100h
  @@SaveDescr3:	CALL	CloseFindFile
		POP	DS:GroupFileNumb	; Update descriptions
		POP	AX
		MOV	DS:DescrUpdate,AL
		MOV	DS:GroupAttr,AH
		CMP	DS:GroupMask[0],0
		JE	@@GoUpDir
		CALL	SaveDescrs
		JNC	@@GoUpDir
		MOV	BX,100h
  @@GoUpDir:	POP	DI			; Go up directory
		MOV	BYTE PTR DS:[DI],0
		OR	BH,BH
		JNE	@@Exit
		MOV	DX,OFFSET LineBuf1	; Cut up directory
		MOV	SI,DX
		CALL	CutPath
		CMP	SI,DI
		JE	@@Exit
		PUSH	SI
		MOV	DI,OFFSET DirFuncBuf
		CALL	MvzLine
		POP	DI
		MOV	DS,CS:SubsSeg
		MOV	SI,OFFSET Mask_S+1
		CALL	MvzLine
		PUSH	ES			; Find up directory
		POP	DS
		CALL	ChgDisk
		JC	@@GlobalExit
		MOV	CX,37h
		MOV	AH,4Eh
  @@UpDirFind:	CALL	Intr21
		JNC	@@UpDirComp
		CMP	AX,53h
		JNE	@@UpDirAdd
  @@GlobalExit:	CALL	CloseFindFile
  @@Error:	MOV	BX,100h
		JMP	@@Exit
  @@UpDirComp:	MOV	AH,4Fh
		TEST	DS:DTA.FilAttr,10h
		JZ	@@UpDirFind
		MOV	SI,OFFSET DTA.FilName
		CMP	DS:ProcLongNams,0
		JE	@@NameOffs
		CMP	DS:FindData.WinLongName,0
		JE	@@NameOffs
		MOV	SI,OFFSET FindData.WinLongName
  @@NameOffs:	MOV	DI,OFFSET DirFuncBuf
		CALL	CompNormLines
		JNE	@@UpDirFind
  @@UpDirAdd:	MOV	SI,OFFSET LineBuf1	; Add up directory
		CALL	CutPath
		MOV	DI,SI
		MOV	SI,OFFSET DirFuncBuf
		CALL	MvzLine
  @@Exit:	MOV	AX,BX
		RET
DirFunc		ENDP

;---------------------------------------------------------------
;	Group operation on selected files and dirs
;---------------------------------------------------------------
FuncSelect:	MOV	DS,ES:WCBcrn
		MOV	AL,ES:DescrProcess
		CMP	AL,1
		JNE	FuncSel03
		CALL	NameWidthAL
		CMP	DS:BrfFul,1
		JB	FuncSel02
		JA	FuncSel01
		ADD	AL,10+9+7
FuncSel01:	ADD	AL,3
		CALL	SizDX
		CMP	AL,DL
		JB	FuncSel03
FuncSel02:	MOV	AL,0
FuncSel03:	MOV	ES:DescrUpdate,AL
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	ES:OneFile,0
		JNZ	FuncSel07
		PUSH	SI
		MOV	SI,DS:DiskBuf
FuncSel04:	CMP	SI,DS:EndBuf
		JAE	FuncSel05
		AND	DS:[SI].AttrF,7Fh
		CALL	NextFilEnt
		JMP	FuncSel04
FuncSel05:	POP	SI
		CALL	TotSel
		OR	BX,DX
		JE	FuncSel07
		MOV	SI,DS:DiskBuf
FuncSel06:	CMP	SI,DS:EndBuf		; Loop
		JAE	FuncExit
		TEST	DS:[SI].AttrF,40h
		JZ	FuncSel13
FuncSel07:	PUSH	SI
		ADD	SI,OFFSET NameF
		MOV	DI,OFFSET DTA.FilName
		CALL	MvzLine
		POP	SI
		PUSH	SI
		CALL	FileNumb
		MOV	BX,AX
		CALL	LongNameOffset
		MOV	DI,OFFSET LineBuf1
		CALL	MvzLine
		POP	SI
		MOV	CL,1
		XCHG	CL,ES:IncDirs
		MOV	AL,DS:[SI].AttrF
		CALL	ES:FncAddr
		OR	ES:GroupChg,AL
		MOV	ES:IncDirs,CL
		MOV	DS,ES:WCBcrn
		CMP	ES:OneFile,0
		JNZ	FuncExit
		CMP	AL,0
		JZ	FuncSel12
		TEST	DS:[SI].AttrF,40h
		JZ	FuncSel12
		AND	DS:[SI].AttrF,3Fh	; Unselect
		OR	DS:[SI].AttrF,80h
		CMP	ES:UnSlct,0
		JZ	FuncSel12
		CMP	ES:ZoomOne,0
		JZ	FuncSel08
		PUSH	DS
		MOV	DS,ES:WindowsPoint
		MOV	BX,DS:[0]
		MOV	DX,DS:[2]
		SUB	DX,102h
		MOV	CX,DS
		POP	DS
		CALL	SavWin
		XCHG	CX,ES:WindowsPoint
		CALL	ResWin
FuncSel08:	CALL	PutFls
		CMP	ES:ZoomOne,0
		JZ	FuncSel09
		CALL	SavWin
		MOV	ES:WindowsPoint,CX
		CALL	ShadBox
		CALL	ResWin
FuncSel09:	PUSH	SI
		PUSH	AX
		CALL	LnsAX
		MOV	CX,AX
		CALL	Addr2Numb
		MOV	AX,SI
		SUB	AX,DS:FileFirst
		JB	FuncSel11
		XOR	DX,DX
		DIV	CX
		CMP	DS:BrfFul,0
		JE	FuncSel10
		OR	AX,AX
		JNE	FuncSel11
FuncSel10:	CALL	BegBX
		ADD	BX,201h
		ADD	BH,DL
		CALL	SizDX
		DEC	DX
		DEC	DX
		MOV	DH,1
		CALL	Window
FuncSel11:	POP	AX
		POP	SI
		CMP	ES:MiniSta,0
		JZ	FuncSel12
		CALL	BegBX
		CALL	SizDX
		ADD	BH,DH
		SUB	BH,2
		MOV	DH,1
		CALL	Window
FuncSel12:	CALL	TstBrk			; Break
		JC	FuncExit
		OR	AH,AH
		JNZ	FuncExit
FuncSel13:	CALL	NextFilEnt
		JMP	FuncSel06

;---------------------------------------------------------------
;	Group operation by mask
;---------------------------------------------------------------
		.CONST
FuncMaskErr_S	DB	"Can't find the files",0

		.DATA
FuncMaskErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	FuncMaskErr_S
		DW	Line1_S
		DW	Ok_S
		DW	0
FuncMaskErr_B	BOXHD	< FuncMaskErr_P, 900h, 0, Dialog_K, Def_M, Default_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0

		.CODE
FuncMask:	PUSH	ES			; Masked files
		POP	DS
		MOV	AL,DS:DescrProcess
		MOV	DS:DescrUpdate,AL
		MOV	SI,OFFSET SourceFile
		CALL	FndLet
		CMP	AL,0
		JE	Func04
FuncMsk01:	MOV	DS:GroupMask[0],0
FuncMsk02:	MOV	DI,OFFSET LineBuf1	; Move one part
		MOV	CX,SI
FuncMsk03:	CALL	FndLet
		LODSB
		CMP	AL,0
		JE	FuncMsk29
		CMP	AL,';'
		JE	FuncMsk03
		CMP	AL,','
		JE	FuncMsk03
		DEC	SI
FuncMsk04:	LODSB
		CMP	AL,0
		JE	FuncMsk05
		CMP	AL,';'
		JE	FuncMsk05
		CMP	AL,','
		JE	FuncMsk05
		STOSB
		JMP	FuncMsk04
FuncMsk05:	MOV	AL,0
		STOSB
		DEC	SI
		MOV	DI,SI			; Test directory or mask
		MOV	SI,OFFSET LineBuf1
		CALL	CutPath
		XCHG	SI,DI
		CMP	DI,OFFSET LineBuf1
		JE	FuncMsk09
		MOV	BX,DI
		XCHG	SI,DI
		CALL	FndLet
		MOV	SI,DI
		CMP	AL,0
		JNE	FuncMsk09
		DEC	BX
		CMP	BYTE PTR DS:[BX],':'
		JNE	FuncMsk07
FuncMsk06:	CMP	DS:GroupMask[0],0
		JE	FuncMsk02
		JMP	FuncMsk16
FuncMsk07:	MOV	DI,OFFSET LineBuf1
		MOV	AX,DS:[DI]
		CMP	AH,':'
		JNE	FuncMsk08
		INC	DI
		INC	DI
		XCHG	SI,DI
		CALL	FndLet
		XCHG	SI,DI
FuncMsk08:	CMP	BX,DI
		JE	FuncMsk06
		MOV	BYTE PTR DS:[BX],0
		JMP	FuncMsk26
FuncMsk09:	MOV	BX,DX
		MOV	DX,OFFSET LineBuf1
		CALL	ChgDisk
		JC	FuncExit
		PUSH	CX
		MOV	AX,4300h
		CALL	Intr21
		MOV	DX,BX
		JNC	FuncMsk10
		POP	CX
		CMP	AX,53h
		JNE	FuncMsk11
		JMP	FuncExit
FuncMsk10:	TEST	CL,10h
		POP	CX
		JNZ	FuncMsk26
FuncMsk11:	MOV	BX,SI			; File or mask
		MOV	SI,OFFSET LineBuf1
		CALL	CutPath
		XCHG	SI,BX
		CMP	BX,OFFSET LineBuf1
		JE	FuncMsk14
		CMP	DS:GroupMask[0],0
		JNE	FuncMsk16
		PUSH	SI
		MOV	SI,BX
		MOV	DI,OFFSET GroupMask
		CALL	MvzLine
		POP	SI
		CMP	BYTE PTR DS:[BX-1],':'
		JE	FuncMsk13
		DEC	BX
		MOV	DI,OFFSET LineBuf1
		MOV	AX,DS:[DI]
		CMP	AH,':'
		JNE	FuncMsk12
		INC	DI
		INC	DI
		XCHG	DI,SI
		CALL	FndLet
		XCHG	DI,SI
FuncMsk12:	CMP	BX,DI
		JNE	FuncMsk13
		INC	BX
FuncMsk13:	MOV	BYTE PTR DS:[BX],0
		JMP	FuncMsk17
FuncMsk14:	MOV	DI,OFFSET GroupMask	; Add one more mask
		CMP	BYTE PTR DS:[DI],0
		JE	FuncMsk15
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		MOV	AL,';'
		STOSB
FuncMsk15:	PUSH	SI
		MOV	SI,OFFSET LineBuf1
		CALL	MvzLine
		POP	SI
		JMP	FuncMsk02
FuncMsk16:	MOV	SI,CX			; File or mask
		MOV	DS:LineBuf1[0],0
FuncMsk17:	PUSH	SI			; Test *.*
		MOV	SI,OFFSET GroupMask
FuncMsk18:	MOV	CX,100h
		CALL	FndLet
FuncMsk19:	LODSB
		CMP	AL,'?'
		JE	FuncMsk19
		INC	CX
		CMP	AL,'*'
		JE	FuncMsk19
		DEC	CX
		CMP	AL,'.'
		JNE	FuncMsk21
		MOV	CH,0
FuncMsk20:	LODSB
		CMP	AL,'?'
		JE	FuncMsk20
		INC	CH
		CMP	AL,'*'
		JE	FuncMsk20
		DEC	CH
FuncMsk21:	DEC	SI
		CALL	FndLet
		CMP	AL,0
		JE	FuncMsk23
		CMP	AL,';'
		JE	FuncMsk23
		CMP	AL,','
		JE	FuncMsk23
FuncMsk22:	LODSB
		CMP	AL,';'
		JE	FuncMsk18
		CMP	AL,','
		JE	FuncMsk18
		CMP	AL,0
		JNE	FuncMsk22
		JMP	FuncMsk24
FuncMsk23:	OR	CL,CL
		JE	FuncMsk22
		OR	CH,CH
		JE	FuncMsk22
		MOV	DS:GroupMask[0],0
FuncMsk24:	POP	SI
		OR	BP,BP			; File or mask
		JE	FuncMsk25
		PUSH	BP
		PUSH	SI
		PUSH	DX
		MOV	AL,0
		CALL	BP
		POP	DX
		POP	SI
		POP	BP
		OR	AH,AH
		JNE	FuncExit
FuncMsk25:	PUSH	BP
		PUSH	SI
		PUSH	DX
		CALL	DirFunc
		POP	DX
		POP	SI
		POP	BP
		CMP	AH,1
		JBE	FuncMsk28
		MOV	AH,0
		JMP	FuncMsk28
FuncMsk26:	CMP	DS:GroupMask[0],0	; Directory
		JNE	FuncMsk16
		MOV	DI,SI
		MOV	SI,OFFSET LineBuf1
		CALL	CutPath
		CALL	FndLet
		CALL	NormStr
		CALL	ChkUpDir
		MOV	SI,DI
		JZ	FuncMsk01
		OR	BP,BP
		JE	FuncMsk27
		PUSH	BP
		PUSH	SI
		PUSH	DX
		MOV	AL,1
		CALL	BP
		POP	DX
		POP	SI
		POP	BP
		OR	AH,AH
		JNE	FuncExit
FuncMsk27:	MOV	DS:GroupFound,1
		CALL	FuncTwoNames
		JC	FuncExit
		MOV	CL,1
		XCHG	CL,DS:IncDirs
		XOR	BX,BX
		MOV	AL,10h
		CALL	DS:FncAddr
		MOV	DS:IncDirs,CL
FuncMsk28:	OR	AH,AH
		JNE	FuncExit
		JMP	FuncMsk01
FuncMsk29:	CMP	DS:GroupMask[0],0	; Exit
		JNE	FuncMsk16
		CMP	DS:GroupFound,0
		JNZ	FuncExit
		MOV	BP,OFFSET FuncMaskErr_B
		CALL	Dialog
		JMP	FuncExit

;---------------------------------------------------------------
;	Exit group function
;---------------------------------------------------------------
FuncExit:	CALL	SaveDescrs
		JC	FuncExit03
		MOV	AX,ES:GroupTree1
		SUB	AX,ES:GroupTree2
		JNE	FuncExit01
		MOV	ES:GroupTree2,AX
		XCHG	ES:GroupTreeChg2,AL
		OR	ES:GroupTreeChg1,AL
FuncExit01:	CMP	ES:GroupTreeChg1,0
		JZ	FuncExit02
		MOV	DS,ES:GroupTree1
		MOV	DX,OFFSET LineBuf1
		CALL	SaveTreeInfo
		JNC	FuncExit02
		CMP	AX,53h
		JE	FuncExit03
FuncExit02:	CMP	ES:GroupTreeChg2,0
		JZ	FuncExit03
		MOV	DS,ES:GroupTree2
		MOV	DX,OFFSET LineBuf2
		CALL	SaveTreeInfo
FuncExit03:	MOV	SI,OFFSET AllSegs_L
		XOR	BX,BX
		XCHG	BX,ES:GroupTree1
		CALL	CompressBlock
		XOR	BX,BX
		XCHG	BX,ES:GroupTree2
		CALL	CompressBlock
		XOR	BX,BX
		XCHG	BX,ES:GroupFiles
		CALL	CompressBlock
FuncExit0:	CMP	ES:ZoomOne,0
		JZ	FuncExit1
		CALL	ResWin
		CALL	Window
FuncExit1:	CMP	ES:GroupChg,0
		JZ	Func04
FuncExit2:	MOV	DS,ES:WCBcrn
		MOV	DS:ChgFlg,1
		CALL	InvWCB
		MOV	DS:ChgFlg,1
		JMP	Panels00

;---------------------------------------------------------------
;	Prepare long and short names
;	Return:	   CY - error
;---------------------------------------------------------------
FuncTwoNames	PROC	NEAR
		USES	AX,SI,DI,DS
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET LineBuf1
		MOV	DI,OFFSET LineBuf2
		CALL	TrueName
		JC	@@Error
		PUSH	DI
		PUSH	SI
		CALL	CutPath
		XCHG	SI,DI
		CALL	CutPath
		CALL	MvzLine
		POP	SI
		POP	DI
		CALL	ShortName
		JNC	@@ShortName
  @@Error:	CMP	AX,53h
		JE	@@Exit
		MOV	DS:DTA.FilName[0],0
		JMP	@@ExitOk
  @@ShortName:	MOV	SI,DI
		CALL	CutPath
		MOV	DI,OFFSET DTA.FilName
		CALL	MvzLine
  @@ExitOk:	STC
  @@Exit:	CMC
		RET
FuncTwoNames	ENDP

;---------------------------------------------------------------
;	Create File List
;		   DS - WCB segment
;		   BX - file handle
;		   AL - 0-long names, >0-short names
;---------------------------------------------------------------
FileListSeg	EQU	WORD PTR TmpBuf[0]
FileListHandle	EQU	WORD PTR TmpBuf[2]
FileListOffset	EQU	WORD PTR TmpBuf[4]
FileListLong	EQU	BYTE PTR TmpBuf[6]

FileList	PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,DS
		CMP	DS:WinTyp,0
		JNE	@@List10
		CMP	DS:Visible,0
		JE	@@List10
		CMP	DS:ArcPnt,0
		JNE	@@List10
		MOV	DL,AL
		MOV	DI,OFFSET TmpBuf
		MOV	CX,512
		MOV	AL,0
		REP	STOSB
		MOV	ES:FileListSeg,DS
		MOV	ES:FileListHandle,BX
		MOV	ES:FileListLong,DL
		MOV	ES:FncAddr,OFFSET FncList
		MOV	SI,DS:DiskBuf
  @@List01:	CMP	SI,DS:EndBuf
		JAE	@@List02
		AND	DS:[SI].AttrF,7Fh
		CALL	NextFilEnt
		JMP	@@List01
  @@List02:	MOV	DI,OFFSET LineBuf1
		MOV	AX,DS
		CMP	AX,ES:WCBcrn
		JE	@@List04
		MOV	SI,OFFSET WinLongPath
		CMP	ES:FileListLong,0
		JNE	@@List03
		MOV	SI,OFFSET WinShortPath
  @@List03:	CALL	MvzLine
		CALL	AddFile
  @@List04:	MOV	ES:FileListOffset,DI
		MOV	SI,DS:DiskBuf
		CALL	TotSel
		OR	BX,DX
		JNE	@@List05
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	@@List10
		PUSH	SI
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		POP	SI
		JNE	@@List06
		JMP	@@List10
  @@List05:	CMP	SI,DS:EndBuf
		JAE	@@List10
		TEST	DS:[SI].AttrF,40h
		JZ	@@List09
  @@List06:	MOV	DI,ES:FileListOffset
		PUSH	SI
		CMP	ES:FileListLong,0
		JE	@@List07
		CMP	DS:[SI].LenLongNamF,0
		JE	@@List07
		ADD	SI,DS:[SI].LenShortNamF
  @@List07:	ADD	SI,OFFSET NameF
		CALL	MvzLine
		POP	SI
		MOV	CL,1
		XCHG	CL,ES:IncDirs
		MOV	CH,ES:FileListLong
		XCHG	CH,ES:ProcLongNams
		MOV	AL,DS:[SI].AttrF
		CALL	ES:FncAddr
		MOV	ES:IncDirs,CL
		MOV	ES:ProcLongNams,CH
		MOV	DS,ES:FileListSeg
		CMP	AL,0
		JZ	@@List08
		TEST	DS:[SI].AttrF,40h
		JZ	@@List08
		AND	DS:[SI].AttrF,3Fh
		OR	DS:[SI].AttrF,80h
  @@List08:	CALL	TstBrk
		JC	@@List10
		OR	AH,AH
		JNZ	@@List10
  @@List09:	CALL	NextFilEnt
		JMP	@@List05
  @@List10:	MOV	BX,ES:FileListHandle
		MOV	AH,3Eh
		CALL	Intr21
		RET
FileList	ENDP

FncList		PROC	NEAR
		USES	BX,CX,DX,SI,DI,BP,DS
		PUSH	ES
		POP	DS
		TEST	AL,10h
		JE	@@FncLst1
		CALL	DirFunc
		CMP	AH,1
		JBE	@@FncLst5
		MOV	AH,0
		JMP	@@FncLst5
  @@FncLst1:	TEST	AL,8
		JNZ	@@FncLst3
		MOV	DX,OFFSET TempDir
		CALL	ChgDisk
		JC	@@FncLst4
		MOV	DI,OFFSET LineBuf1
		MOV	DX,DI
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		MOV	AX,0A0Dh
		STOSW
		MOV	CX,DI
		SUB	CX,DX
		MOV	BX,DS:FileListHandle
		MOV	AH,40h
		CALL	Intr21
		MOV	BYTE PTR DS:[DI-2],0
		JC	@@FncLst2
		CMP	AX,CX
		JNE	@@FncLst3
		MOV	AX,1
		JMP	@@FncLst5
  @@FncLst2:	CMP	AX,53h
		JE	@@FncLst4
  @@FncLst3:	XOR	AX,AX
		JMP	@@FncLst5
  @@FncLst4:	MOV	AX,100h
  @@FncLst5:	RET
FncList		ENDP
