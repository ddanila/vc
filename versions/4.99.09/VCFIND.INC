;---------------------------------------------------------------
;		Find file
;
;		Version:	08.01.2001
;---------------------------------------------------------------

FndFiles	EQU	WORD PTR TmpBuf[0]	; Number of found files
FndDirs		EQU	WORD PTR TmpBuf[2]	; Number of found dirs
FndSrcPoint	EQU	WORD PTR TmpBuf[4]	; Point of find mask line
FndMaskPoint	EQU	WORD PTR TmpBuf[6]	; Point of masks
FndBase		EQU	WORD PTR TmpBuf[8]	; Base of window
FndWindow	EQU	WORD PTR TmpBuf[10]	; Size of window
FndCursor	EQU	WORD PTR TmpBuf[12]	; Number of line (cache)
FndHandle	EQU	WORD PTR TmpBuf[14]	; File handle
FndPoint	EQU    DWORD PTR TmpBuf[16]	; Pointer to end
FndPoint2	EQU    DWORD PTR TmpBuf[20]	; Pointer (Cache)
FndFoundSize	EQU    QWORD PTR TmpBuf[24]	; Size of found files
FndSrchOffs	EQU    DWORD PTR TmpBuf[32]	; Offset of found string
FndSrchTxHx	EQU	BYTE PTR TmpBuf[36]	; Found Text/Hex
FndDrive	EQU	BYTE PTR TmpBuf[37]	; Drive letter
FndFlag		EQU	BYTE PTR TmpBuf[38]	; New directory flag
FndTblTitle	EQU	BYTE PTR TmpBuf[39]	; Title of table
FndDtSz		EQU	39+256

FindDt		STRUC
  FnTbl		TblData	?               ; Translation table
  FnSearch	SrData	?		; Search data
  FnStore	DB	FndDtSz DUP(?)	; Buffer for data
  FnBegin	LABEL	BYTE		; Begin of entries
FindDt		ENDS

FindEnt		STRUC
  FndAttr	DB	?
  FndTime	DW	?
  FndDate	DW	?
  FndSize	DD	?
  FndOffset	DD	?
  FndTxtHex	DB	?
  FndLenShort	DW	?
  FndLenLong	DW	?
  FndName	LABEL	BYTE
FindEnt		ENDS

		.CONST
FindFile_S	DB	' Find File ',0
FindName_S	DB	'File name:',0
FindCont_S	DB	'Containing:',0
FindTable_S	DB	'Character table: ',0
FindScan_S	DB	'Scanning:',0
FindLeft_S	DB	'^3E64<',0
FindFound_S	DB	' found',0
		DB	' (',0
		DB	'bytes',0
		DB	')',0
FindError1_S	DB	'There is no more space',0
FindError2_S	DB	'to continue searching.',0
FindMenu1_S	DB	'[ Find ]',0FFh,3,'[ Table ]',0FFh,3,'[ Cancel ]',0
FindMenu2_S	DB	'^r[ Stop ]^r',0FFh,3,'[ Go to ]',0FFh,3,'[ View ]',0FFh,3,'[ Quit ]',0
FindMenu3_S	DB	'[ New search ]',0FFh,3,'[ Go to ]',0FFh,3,'[ View ]',0FFh,3,'[ Quit ]',0
FindSubDir_S	DB	'      SUB-DIR'

		.DATA
FindFile1_P	DB	Conf_C, CnfRe_C, CnfBo_C, 0
		DW	FindFile_S
		DW	0FFFEh
		DW	0FFFFh
		DW	Inc0l_S
		DW	Empty_S
		DW	Inc1l_S
		DW	Empty_S
		DW	0FFFFh
		DW	Inc2l_S
		DW	IncSubdirs_S
		DW	FindLeft_S
		DW	0FFFFh
		DW	FindMenu1_S
		DW	0
FindFile2_P	DB	Conf_C, CnfRe_C, CnfBo_C, 0
		DW	FindFile_S
		DW	0FFFEh
		DW	0FFFFh
		DW	Inc0l_S
		DW	FindMenu2_S
		DW	0
FindFile3_P	DB	Conf_C, CnfRe_C, CnfBo_C, 0
		DW	FindFile_S
		DW	0FFFEh
		DW	0FFFFh
		DW	FindLeft_S
		DW	FindMenu3_S
		DW	0
FindFile1_B	BOXHD	< FindFile1_P, 8500h, FindSub1, FindFile1_K, Def_M, FileFind_H, 10h >
		BOXMN	< 1, 4,1,0,0, 0F405h, FindStr, LenStrFull+LenStr >
		BOXMN	< 1, 0,2,1,1, 0F605h, SrchStr, LenStrFull+LenStr >
		BOXMN	< 3, 1,3,2,2, 0F806h, SrchCase, 0 >
		BOXMN	< 3, 2,4,3,3, 0F906h, IncDirs, 0 >
		BOXMN	< 4, 3,0,4,4, 0FCEFh, FindMenu1_D, 0 >
		DB	0
FindFile2_B	BOXHD	< FindFile2_P, 8500h, FindSub2, FindFile2_K, Def_M, FileFind_H, 10h >
		BOXMN	< 4, 0,0,0,0, 0FCEBh, FindMenu2_D, 0 >
		DB	0
FindFile3_B	BOXHD	< FindFile3_P, 8500h, FindSub2, FindFile2_K, Def_M, FileFind_H, 10h >
		BOXMN	< 4, 0,0,0,0, 0FCE8h, FindMenu3_D, 0FFh >
		DB	0
FindError_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	FindFile_S
		DW	FindError1_S
		DW	FindError2_S
		DW	Ok_S
		DW	0
FindError_B	BOXHD	< FindError_P, 900h, 0, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
FindFile1_T	DW	FindName_S, FindCont_S, SearchCase_S, LineBuf4
FindFile2_T	DW	FindScan_S
FindMenu1_D	DB	 8,  0
		DB	 9, 11
		DB	10, 23
		DB	 0
FindMenu2_D	DB	 8,  0
		DB	 9, 11
		DB	 8, 23
		DB	 8, 34
		DB	 0
FindMenu3_D	DB	14,  0
		DB	 9, 17
		DB	 8, 29
		DB	 8, 40
		DB	 0

		.CODE
F67_00:		CALL	HidCur			; Alt F7 - file find
		MOV	AL,Empty_K
		CALL	PutKey
		MOV	AL,Cmd_M
		CALL	PutMnu
		MOV	DS,ES:WorkBuffer
		MOV	AL,ES:TransTblNumb
		MOV	DS:FnTbl.TblNumb,AL
		MOV	SI,OFFSET FnTbl
		MOV	AL,1
		CALL	TableSelect
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET TblList
		MOV	DI,OFFSET FndTblTitle
		CALL	strcpy
F67_02:		MOV	AL,LenStr		; First dialog
		MOV	AH,DS:Lines
		SUB	AH,20
		MOV	DS:ListWindow,AX
		MOV	DS:ListBase,205h
		CALL	FindTable
		MOV	BX,8500h
		MOV	DX,OFFSET FindFile1_T
		MOV	BP,OFFSET FindFile1_P
		CALL	OncZoom
		MOV	BP,OFFSET FindFile1_B
		CALL	Dialog
		JNE	F67_07
		MOV	SI,OFFSET FindStr
		CALL	FndLet
		CMP	AL,0
		JE	F67_07
		SUB	DX,80Ch			; Second dialog
		MOV	DS:ListWindow,DX
		MOV	BX,8500h
		MOV	DX,OFFSET FindFile2_T
		MOV	BP,OFFSET FindFile2_P
		CALL	OncZoom
		XOR	AX,AX			; Init variables
		MOV	DS:ListCursor,AX
		MOV	DS:ListFirst,AX
		MOV	DS:ListTotal,AX
		MOV	DS:ListLine,OFFSET FindLine
		MOV	DS:ListHotKey,AX
		MOV	DS:FndBase,BX
		MOV	DS:FndWindow,DX
		MOV	DS:FndHandle,AX
		MOV	DS:FndFiles,AX
		MOV	DS:FndDirs,AX
		MOV	DS:FndFoundSize[0].Lo,AX
		MOV	DS:FndFoundSize[0].Hi,AX
		MOV	DS:FndFoundSize[4].Lo,AX
		MOV	DS:FndFoundSize[4].Hi,AX
		MOV	DS:FndSrchTxHx,AL
		DEC	AX
		MOV	DS:FndSrchOffs.Lo,AX
		MOV	DS:FndSrchOffs.Hi,AX
		MOV	DS:FndCursor,AX
		MOV	AX,DS:WorkBuffer
		MOV	DS:FndPoint.Offs,OFFSET FnBegin
		MOV	DS:FndPoint.Segm,AX
		MOV	SI,OFFSET FnTbl
		MOV	DS,AX
		MOV	AL,1
		CALL	PrepareSearch
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	SI,OFFSET Search
		MOV	DI,OFFSET FnSearch
		MOV	CX,SIZE SrData
		REP	MOVSB
		PUSH	DS
		POP	ES
		MOV	BP,SP			; Init task
		SUB	SP,512
		MOV	AX,OFFSET F67_20
		PUSH	CS
		PUSH	AX
		PUSH	AX
		PUSH	BX
		PUSH	CX
		PUSH	DX
		PUSH	SI
		PUSH	DI
		PUSH	DS
		PUSH	ES
		PUSH	BP
		PUSHF
		MOV	DS:TaskStack,SP
		MOV	SP,BP
		MOV	DS:MinMnuDefault,1
		MOV	BX,DS:FndBase
		MOV	DX,DS:FndWindow
		MOV	BP,OFFSET FindFile2_B
		CALL	Dialog
		MOV	DS:TaskStack,0
		CALL	CloseFindFile
		MOV	BX,DS:FndHandle
		OR	BX,BX
		JE	F67_03
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	AX
F67_03:		CMP	AL,0
		JNE	F67_07
		MOV	DI,OFFSET LineBuf4	; Result
		MOV	DX,DS:FndFiles
		MOV	BX,DS:FndDirs
		MOV	AL,1
		CALL	FilDir
		MOV	DS,CS:SubsSeg
		MOV	SI,OFFSET FindFound_S
		CALL	strcpy
		OR	DX,DX
		JE	F67_06
		CALL	strcpy
		PUSH	DI
		MOV	DI,OFFSET FndFoundSize
		CALL	ReduceNumb
		MOV	CX,ES:[DI].Lo
		MOV	BX,ES:[DI].Hi
		POP	DI
		PUSH	AX
		MOV	AL,1
		CALL	Number
		MOV	AL,' '
		STOSB
		POP	AX
		CMP	AL,0
		JE	F67_04
		STOSB
F67_04:		CALL	strcpy
		DEC	CX
		OR	CX,BX
		JNE	F67_05
		DEC	DI
F67_05:		CALL	strcpy
F67_06:		PUSH	ES
		POP	DS
		MOV	BX,8500h
		MOV	DX,OFFSET FindFile1_T
		MOV	BP,OFFSET FindFile3_P
		CALL	OncZoom
		MOV	BP,OFFSET FindFile3_B
		CALL	Dialog
		JZ	F67_02
F67_07:		CALL	ResWin
		CALL	Window
		CMP	AL,1
		JNE	Func04
		CMP	DS:ListTotal,0		; Go to file
		JE	Func04
		CALL	FindEntry
		MOV	ES:LineBuf1.FndName,0
		TEST	DS:[SI].FndAttr,80h
		JNZ	F67_10
		PUSH	DI
		MOV	DI,OFFSET LineBuf1
		MOV	CX,SIZE FindEnt
		ADD	CX,DS:[SI].FndLenShort
		REP	MOVSB
		POP	SI
		MOV	DS,BP
F67_10:		LEA	BX,[SI]+SIZE FindEnt
		ADD	BX,DS:[SI].FndLenShort
		ADD	BX,DS:[SI].FndLenLong
		TEST	DS:[BX].FndAttr,80h
		JZ	F67_11
		MOV	SI,BX
F67_11:		LEA	DX,[SI]+SIZE FindEnt
		CALL	ChgDisk
		JC	Func04
		MOV	AH,3Bh
		CALL	Intr21
		JC	Func04
		MOV	DL,DS:[SI]+SIZE FindEnt
		SUB	DL,'A'
		MOV	AH,0Eh
		CALL	Intr21
		MOV	DS,ES:WCBcrn
		CALL	DelAllTempArcs
		CALL	GetPath
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	SI,OFFSET DosShortPath
		MOV	DI,OFFSET WinShortPath
		CALL	strcpy
		XOR	BX,BX
		XOR	DX,DX
		MOV	DI,OFFSET ArcFiles
		MOV	ES:DiskBuf,DI
		MOV	SI,OFFSET LineBuf1
		CMP	DS:[SI].FndName,0
		JE	F67_12
		MOV	AL,DS:[SI].FndAttr
		MOV	ES:[DI].AttrF,AL
		MOV	CX,DS:[SI].FndLenShort
		MOV	ES:[DI].LenShortNamF,CX
		MOV	ES:[DI].LenLongNamF,0
		ADD	SI,OFFSET FndName
		ADD	DI,OFFSET NameF
		REP	MOVSB
		INC	DX
F67_12:		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	DS:FileCur,BX
		MOV	DS:FileFirst,DX
		MOV	DS:EndBuf,DI
		MOV	DS:ArcPnt,0
		JMP	F24_11

;---------------------------------------------------------------
;	Background FindFile process
;---------------------------------------------------------------
F67_20		PROC	FAR
		MOV	DS:FndSrcPoint,OFFSET FindStr
F67_21:		MOV	SI,DS:FndSrcPoint	; Parse one mask
		MOV	DI,OFFSET LineBuf1
		PUSH	DI
		MOV	AH,1
F67_22:		MOV	CX,SI
		MOV	BX,DI
		CALL	FndLet
F67_23:		MOV	DX,DI
F67_24:		LODSB
		CMP	AL,';'
		JE	F67_25
		CMP	AL,','
		JE	F67_25
		CMP	AL,0
		JE	F67_25
		STOSB
		CMP	AL,' '
		JNE	F67_23
		JMP	F67_24
F67_25:		MOV	DI,DX
		CMP	DI,BX
		MOV	AL,';'
		JE	F67_26
		STOSB
F67_26:		MOV	BYTE PTR DS:[DI],0
		OR	AH,AH
		JNE	F67_27
		PUSH	SI
		MOV	SI,BX
		CALL	CutPath
		CMP	SI,BX
		POP	SI
		JE	F67_27
		MOV	SI,CX
		MOV	BYTE PTR DS:[BX],0
		JMP	F67_28
F67_27:		MOV	AH,0
		CMP	BYTE PTR DS:[SI-1],0
		JNE	F67_22
		DEC	SI
		CMP	CX,OFFSET OFFSET FindStr
		JNE	F67_28
		MOV	BYTE PTR DS:[DI-1],'*'
F67_28:		MOV	DS:FndSrcPoint,SI
		POP	SI
		CMP	BYTE PTR DS:[SI],0
		JE	F67_35
		MOV	DI,OFFSET LineBuf2	; Split into path and mask
		MOV	DS:FndDrive,0
		PUSH	DI
		CMP	WORD PTR DS:[SI],':*'
		JNE	F67_29
		MOV	DS:FndDrive,'A'
		MOVSW
		LODSB
		DEC	SI
		CMP	AL,'\'
		JE	F67_29
		CMP	AL,'/'
		JE	F67_29
		MOV	AL,'\'
		STOSB
F67_29:		CALL	strcpy
		POP	SI
		CALL	CutPath
		MOV	DS:FndMaskPoint,SI
		CMP	DS:FndDrive,0
		JE	F67_31
F67_30:		MOV	AL,DS:FndDrive		; Find fixed drive
		CMP	AL,'Z'
		JA	F67_21
		MOV	DS:LineBuf2[0],AL
		INC	DS:FndDrive
		CALL	TstDrv
		JC	F67_30
		CMP	AL,CDROM
		JE	F67_30
		CMP	AL,Floppy
		JE	F67_30
		CMP	AL,Logical
		JE	F67_30
F67_31:		MOV	SI,OFFSET LineBuf2	; Canonize path
		CALL	ChgDisk
		JC	F67_35
		MOV	BX,DS:FndMaskPoint
		MOV	CL,0
		XCHG	CL,DS:[BX]
		MOV	DI,OFFSET LineBuf1
		CALL	NormPath
		MOV	DS:[BX],CL
		JNC	F67_32
		CMP	AX,53h
		JNE	F67_30
		JMP	F67_35
F67_32:		MOV	SI,DI
		CALL	RealPath
		JC	F67_35
		CALL	StrLen
		CMP	CX,3+1
		JBE	F67_33
		ADD	DI,CX
		MOV	BYTE PTR DS:[DI-2],0
F67_33:		MOV	DS:FncAddr,OFFSET FindProc
		MOV	CL,DS:IncDirs
		CMP	DS:FndDrive,0
		JE	F67_34
		MOV	DS:IncDirs,1
F67_34:		MOV	AL,10h
		CALL	FindProc
		MOV	DS:IncDirs,CL
		OR	AH,AH
		JNE	F67_35
		CMP	DS:FndDrive,0
		JNE	F67_30
		JMP	F67_21
F67_35:		MOV	DS:KeyStroke,4400h
F67_36:		CALL	TaskSwitch
		JMP	F67_36
F67_20		ENDP

;---------------------------------------------------------------
;	Find file
;---------------------------------------------------------------
FindProc	PROC	NEAR
		USES	BX,CX,DX,SI,DI,BP
		TEST	AL,10h
		JNZ	@@FindDir
		XOR	AX,AX
		JMP	@@Exit
  @@FindDir:	MOV	DX,OFFSET LineBuf1	; Output directory
		CALL	FindSearching
		CALL	ChgDisk
		JC	@@StopFF
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		PUSH	DI
		CALL	AddPathSep
		MOV	DS,CS:SubsSeg
		MOV	SI,OFFSET Mask_S+1
		CALL	strcpy
		PUSH	ES
		POP	DS
		MOV	DS:FndFlag,0
		MOV	CX,37h
		MOV	AH,4Eh
  @@FindNext:	CALL	TaskSwitch
		CALL	Intr21			; Find entry
		JC	@@CloseFF
		MOV	SI,OFFSET DTA.FilName
		CALL	ChkUpDir
		JE	@@Next
		MOV	DI,DS:FndMaskPoint
		CALL	TestMask
		JE	@@SearchTxt
		MOV	SI,OFFSET FindData.WinLongName
		CALL	TestMask
		JNE	@@Next
  @@SearchTxt:	CMP	DS:SrchStr[0],0		; Search string
		JE	@@StoreEntry
		TEST	DS:DTA.FilAttr,10h
		JNZ	@@Next
		MOV	DX,OFFSET LineBuf1
		MOV	SI,DX
		CALL	CutPath
		MOV	DI,SI
		MOV	SI,OFFSET FindData.WinLongName
		CMP	BYTE PTR DS:[SI],0
		JNE	@@FullName
		MOV	SI,OFFSET DTA.FilName
  @@FullName:	CALL	strcpy
		CALL	FindSearching
		CALL	FindString
		JNC	@@StoreEntry
		CMP	AX,53h
		JE	@@CloseFF
		JMP	@@Next

  @@StoreEntry:	CMP	DS:FndFlag,0		; Empty line
		JNE	@@SetEntry
		XOR	BX,BX
		CMP	DS:ListTotal,0
		JE	@@StorePath
		MOV	DX,1
		CALL	FindChkOver
		JC	@@CloseFF
		LES	DI,DS:FndPoint
		MOV	ES:[DI].FndAttr,80h
		MOV	ES:[DI].FndLenShort,DX
		MOV	ES:[DI].FndLenLong,BX
		ADD	DI,OFFSET FndName
		MOV	AL,0
		STOSB
		CALL	FindNextPoint
  @@StorePath:	MOV	SI,OFFSET LineBuf1	; Path
		PUSH	SI
		CALL	CutPath
		MOV	CX,SI
		POP	SI
		SUB	CX,SI
		CMP	CX,3
		JA	@@ChkOver
		INC	CX
  @@ChkOver:	MOV	DX,CX
		CALL	FindChkOver
		JC	@@CloseFF
		LES	DI,DS:FndPoint
		MOV	ES:[DI].FndAttr,80h
		MOV	ES:[DI].FndLenShort,DX
		MOV	ES:[DI].FndLenLong,BX
		ADD	DI,OFFSET FndName
		MOV	CX,DX
		DEC	CX
		REP	MOVSB
		MOV	AL,0
		STOSB
		CALL	FindNextPoint
  @@SetEntry:	MOV	DI,OFFSET DTA.FilName	; Store entry
		CALL	StrLen
		MOV	BX,CX
		XOR	CX,CX
		MOV	DI,OFFSET FindData.WinLongName
		CMP	BYTE PTR DS:[DI],0
		JE	@@EntrySize
		CALL	StrLen
  @@EntrySize:	MOV	DX,CX
		CALL	FindChkOver
		JC	@@CloseFF
		MOV	AX,1
		CMP	DS:FndFlag,AH
		JNE	@@SetParms
		MOV	DS:FndFlag,AL
		INC	AX
		CMP	DS:ListTotal,0
		JE	@@SetParms
		INC	AX
  @@SetParms:	ADD	DS:ListTotal,AX
		LES	DI,DS:FndPoint
		MOV	SI,OFFSET DTA.FilAttr
		MOVSB
		MOVSW
		MOVSW
		LODSW
		STOSW
		ADD	DS:FndFoundSize[0].Lo,AX
		LODSW
		STOSW
		ADC	DS:FndFoundSize[0].Hi,AX
		ADC	DS:FndFoundSize[4].Lo,0
		ADC	DS:FndFoundSize[4].Hi,0
		MOV	AX,DS:FndSrchOffs.Lo
		STOSW
		MOV	AX,DS:FndSrchOffs.Hi
		STOSW
		MOV	AL,DS:FndSrchTxHx
		STOSB
		MOV	AX,BX
		STOSW
		MOV	AX,DX
		STOSW
		MOV	CX,BX
		REP	MOVSB
		MOV	CX,DX
		JCXZ	@@SetPoiner
		MOV	SI,OFFSET FindData.WinLongName
		REP	MOVSB
  @@SetPoiner:	CALL	FindNextPoint
		TEST	DS:DTA.FilAttr,10h
		JNZ	@@AddDir
		INC	DS:FndFiles
		JMP	@@Screen
  @@AddDir:	INC	DS:FndDirs
  @@Screen:	MOV	BP,OFFSET FindFile2_P	; Scroll window
		MOV	BX,DS:FndBase
		MOV	DX,DS:FndWindow
		CMP	DS:ListMoveFlag,0
		JNE	@@NoMove
		MOV	AX,0FFFCh
		CMP	DS:ListScrollBar,0
		JNE	@@CallFunc
		MOV	AX,DS:ListTotal
		DEC	AX
		MOV	DS:ListCursor,AX
		JMP	@@Update
  @@NoMove:	MOV	AX,0FFFDh
		CMP	DS:ListScrollBar,0
		JNE	@@CallFunc
  @@Update:	MOV	AX,0FFFFh
  @@CallFunc:	MOV	CL,0
		XCHG	CL,DS:MouseWin
		MOV	CH,DS:MouseEnter
		PUSH	DS:MouseTime.Hi
		PUSH	DS:MouseCmd
		PUSH	CX
		CALL	FindDialList
		POP	CX
		POP	DS:MouseCmd
		POP	DS:MouseTime.Hi
		MOV	DS:MouseWin,CL
		MOV	DS:MouseEnter,CH
  @@Next:	MOV	AH,4Fh
		JMP	@@FindNext
  @@CloseFF:	CALL	CloseFindFile		; Exit
		POP	SI
		MOV	BYTE PTR DS:[SI],0
		CMP	AX,53h
		JNE	@@Recurse
  @@StopFF:	MOV	AX,100h
		JMP	@@Exit
  @@Recurse:	XOR	AX,AX
		CMP	DS:IncDirs,0
		JE	@@Exit
		CALL	DirFunc
		CMP	AH,1
		JBE	@@Exit
		MOV	AH,0
  @@Exit:	RET
FindProc	ENDP

;---------------------------------------------------------------
;	Find string in file
;		DS:DX - file
;	Return:	   NC - found
;		   CY - not found, AX = error code
;---------------------------------------------------------------
		.CODE
FindString	PROC	NEAR
		MOV	DS:BlocEr,2
		MOV	AL,40h
		CALL	OpenFil
		MOV	DS:BlocEr,0
		JNC	@@BufSize
		CMP	DS:ErrDat,2
		JNE	@@Error
		MOV	AL,0
		JMP	@@Error
  @@BufSize:	MOV	DS:FndHandle,AX
		MOV	BX,AX
		MOV	AX,ES			; Size of buffer
		ADD	AX,ES:SizeDataSeg
		SUB	AX,MinView/16/2
		LDS	DX,ES:FndPoint
		MOV	CX,DS
		SUB	AX,CX
		JBE	@@Overflow
		MOV	CX,0FFFh
		CMP	AX,CX
		JBE	@@BufSize1
		MOV	AX,CX
  @@BufSize1:	MOV	CL,4
		SHL	AX,CL
		SUB	AX,DX
		JBE	@@Overflow
		MOV	CX,8200h
		CMP	CX,AX
		JBE	@@BufSize2
		MOV	CX,AX
  @@BufSize2:	MOV	ES:FndSrchOffs.Lo,0
		MOV	ES:FndSrchOffs.Hi,0
		MOV	AX,200h
		MOV	BP,CX
		AND	BP,0FE00h
		CMP	BP,400h
		JAE	@@MoveEnd
  @@Overflow:	XOR	CX,CX			; Not enough memory
		XCHG	CX,ES:TaskStack
		MOV	BP,OFFSET FindError_B
		CALL	Dialog
		MOV	ES:TaskStack,CX
		MOV	AX,53h
		JMP	@@CloseErr
  @@MoveEnd:	PUSH	ES			; Move last 512 bytes to the beginning
		PUSH	DS
		POP	ES
		MOV	SI,DX
		MOV	DI,DX
		ADD	SI,BP
		MOV	CX,200h
		SUB	SI,CX
		REP	MOVSB
		POP	ES
		PUSH	DX			; Read buffer
		MOV	SI,AX
		MOV	DX,DI
		LEA	CX,[BP-200h]
		MOV	AH,3Fh
		CALL	TaskSwitch
		CALL	Intr21
		POP	DX
		JC	@@CloseErr
		PUSH	DS			; Search in buffer
		PUSH	SI
		PUSH	CX
		MOV	DS,ES:WorkBuffer
		MOV	SI,OFFSET FnSearch
		MOV	DI,OFFSET Search
		MOV	CX,SIZE SrData
		REP	MOVSB
		POP	DI
		POP	SI
		POP	DS
		SUB	DI,AX
		MOV	CX,AX
		SUB	CX,SI
		PUSH	CX
		ADD	CX,200h
		ADD	SI,DX
		CALL	SearchNext
		POP	CX
		JC	@@Next
		MOV	ES:FndSrchTxHx,AL
		ADD	ES:FndSrchOffs.Lo,SI
		ADC	ES:FndSrchOffs.Hi,0
		JMP	@@CloseFile
  @@Next:	XOR	AX,AX
		ADD	ES:FndSrchOffs.Lo,CX
		ADC	ES:FndSrchOffs.Hi,AX
		OR	DI,DI
		JE	@@MoveEnd
  @@CloseErr:	STC
  @@CloseFile:	PUSH	ES			; Close file
		POP	DS
		PUSHF
		PUSH	AX
		MOV	DS:FndHandle,0
		MOV	AH,3Eh
		CALL	Intr21
		POP	AX
		POPF
		JNC	@@Exit
  @@Error:	STC
  @@Exit:	RET
FindString	ENDP

;---------------------------------------------------------------
;	Searching process status
;		DS:DX - line
;---------------------------------------------------------------
FindSearching	PROC	NEAR
		MOV	SI,DX
		MOV	DI,OFFSET LineBuf4
		PUSH	DI
		CALL	strcpy
		POP	SI
		MOV	ES,CS:SubsSeg
		MOV	DI,OFFSET FindScan_S
		CALL	StrLen
		PUSH	DS
		POP	ES
		MOV	AX,LenStr
		SUB	AX,CX
		CALL	ShrtLin
		MOV	BX,DS:FndBase
		ADD	BH,DS:FndWindow.Y
		SUB	BH,5
		ADD	BL,5
		ADD	BL,CL
		CALL	BasAdr
		MOV	CX,AX
		PUSH	CX
  @@PutStatus:	LODSB
		CMP	AL,0
		JNE	@@PutChar
		DEC	SI
		MOV	AL,' '
  @@PutChar:	STOSB
		INC	DI
		LOOP	@@PutStatus
		PUSH	DS
		POP	ES
		MOV	DI,DX
		POP	DX
		MOV	DH,1
		CALL	Window
		MOV	DX,DI
		RET
FindSearching	ENDP

;---------------------------------------------------------------
;	Check overflow
;---------------------------------------------------------------
FindChkOver	PROC	NEAR
		MOV	AX,SIZE FindEnt
		ADD	AX,BX
		ADD	AX,DX
		ADD	AX,DS:FndPoint.Offs
		DEC	AX
		MOV	CL,4
		SHR	AX,CL
		INC	AX
		ADD	AX,DS:FndPoint.Segm
		MOV	CX,ES
		ADD	CX,DS:SizeDataSeg
		SUB	CX,MinView/16/2
		CMP	CX,AX
		JNC	@@Exit
		XOR	CX,CX
		XCHG	CX,DS:TaskStack
		MOV	BP,OFFSET FindError_B
		CALL	Dialog
		MOV	DS:TaskStack,CX
		MOV	AX,53h
		STC
  @@Exit:	RET
FindChkOver	ENDP

;---------------------------------------------------------------
;	Set pointer
;---------------------------------------------------------------
FindNextPoint	PROC	NEAR
		MOV	AX,DI
		AND	AX,0Fh
		MOV	DS:FndPoint.Offs,AX
		MOV	CL,4
		SHR	DI,CL
		ADD	DS:FndPoint.Segm,DI
		PUSH	DS
		POP	ES
		RET
FindNextPoint	ENDP

;---------------------------------------------------------------
;	Call DialogList
;---------------------------------------------------------------
		.CODE	Subs
FindDialList	PROC	FAR
		CALL	DialogList
		RET
FindDialList	ENDP

;---------------------------------------------------------------
;	Select table subfunction
;---------------------------------------------------------------
FindSub1	PROC	NEAR
		CMP	AX,4200h
		JE	@@Table
		CMP	AH,0FCh
		JAE	@@Exit
		CMP	AL,0Dh
		JE	@@ChkMenu
		CMP	AL,0Ah
		JNE	@@Exit
  @@ChkMenu:	CMP	CL,1
		JNE	@@Exit
  @@Table:	PUSH	DS:ListWindow
		PUSH	DS:ListBase
		PUSH	DS
		PUSH	BP
		PUSH	BX
		MOV	DS,DS:WorkBuffer
		MOV	SI,OFFSET FnTbl
		MOV	AL,0
		CALL	TableSelect
		POP	CX
		POP	BP
		POP	DS
		POP	DS:ListBase
		POP	DS:ListWindow
		JC	@@ExitTbl
		MOV	DS:TransTblNumb,AL
		MOV	SI,OFFSET TblList
		MOV	DI,OFFSET FndTblTitle
		CALL	strcpy
		CALL	FindTable
		MOV	DX,OFFSET FindFile1_T
		MOV	SI,OFFSET FindLeft_S
		MOV	BX,CX
		ADD	BX,DS:ListBase
		ADD	BH,DS:ListWindow.Y
		ADD	BH,8
		MOV	AX,LenStr
		PUSH	CS
		POP	DS
		CALL	NgLine
		MOV	DX,100h+LenStr
		CALL	Window
  @@ExitTbl:	XOR	AX,AX
  @@Exit:	RET
FindSub1	ENDP

FindTable	PROC	FAR
		PUSH	DS
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET FindTable_S
		MOV	DI,OFFSET LineBuf4
		CALL	strcpy
		POP	DS
		MOV	SI,OFFSET FndTblTitle
		CALL	strcpy
		MOV	DS:LineBuf4+LenStr,AL
		RET
FindTable	ENDP

;---------------------------------------------------------------
;	Searching/Result subfinction
;---------------------------------------------------------------
		.DATA
FindSub2_T	DW     0FFFFh, FindSubInit
		DW	001Bh, FindSubStop
		DW	4400h, FindSubStop
		DW	000Dh, FindSubEnter
		DW	000Ah, FindSubEnter
		DW	3D00h, FindSubView
		DW	6A00h, FindSubView
		DW	3F00h, FindSubGoto
		DW	0

		.CODE	Subs
FindSub2	PROC	NEAR
		PUSH	SI
		CMP	AH,0FCh
		JE	@@DialList
		MOV	SI,OFFSET FindSub2_T
		CALL	Case
		OR	AX,AX
		JNE	@@DialList
  FindSubStop:	POP	SI
		PUSH	SI
		CMP	SI,OFFSET FindFile2_B
		JNE	@@DialList
		MOV	BP,DI
		POP	DI
		ADD	DI,SIZE BoxHd
		CALL	DialogBuf
		MOV	AL,0
		XCHG	AL,SS:[SI]
		CMP	AL,0
		JNE	@@MinMnuDef
		INC	AX
  @@MinMnuDef:	MOV	ES:MinMnuDefault,AL
		JMP	@@Enter
  FindSubGoto:	MOV	BP,DI
		POP	DI
		ADD	DI,SIZE BoxHd
		CALL	DialogBuf
		MOV	BYTE PTR SS:[SI],1
  @@Enter:	MOV	AX,1C0Dh
		JMP	@@Exit
  FindSubEnter:	CMP	CL,2
		JNE	@@DialList
  FindSubView:	POP	SI
		CMP	ES:ListTotal,0		; View file
		JE	@@ViewExit
		CALL	FindEntry
		TEST	DS:[SI].FndAttr,90h
		JNZ	@@ViewExit
		PUSH	DS
		PUSH	SI
		MOV	DS,BP
		MOV	SI,DI
		ADD	SI,OFFSET FndName
		MOV	DI,OFFSET LineBuf1
		CALL	strcpy
		CALL	AddPathSep
		POP	SI
		POP	DS
		MOV	DX,SI
		MOV	BP,DS
		CMP	DS:[SI].FndLenLong,0
		JE	@@MoveName
		ADD	SI,DS:[SI].FndLenShort
  @@MoveName:	ADD	SI,OFFSET FndName
		CALL	strcpy
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET LineBuf1
		MOV	DI,OFFSET SourceFile
		MOV	CX,LenStrFull+2
		CALL	SaveHistory
		MOV	ES,DS:WorkBuffer	; Save data
		MOV	SI,OFFSET TmpBuf
		MOV	DI,OFFSET FnStore
		MOV	CX,FndDtSz
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	CX
		REP	MOVSB
		PUSH	DS
		POP	ES
		PUSH	DX			; Save window
		XOR	BX,BX
		MOV	DL,DS:Columns
		MOV	DH,DS:Lines
		CALL	SavWin
		POP	DI
		XOR	AX,AX
		XCHG	AX,DS:TaskStack
		PUSH	AX
		MOV	BX,DS:FndPoint.Offs	; Set new WorkBuffer
		ADD	BX,200h-1
		MOV	CL,4
		SHR	BX,CL
		INC	BX
		ADD	BX,DS:FndPoint.Segm
		XCHG	BX,DS:WorkBuffer
		PUSH	BX
		MOV	DS:ArcPathName,0
		MOV	DS,BP
		MOV	AL,DS:[DI].FndTxtHex
		PUSH	DS:[DI].FndOffset.Hi
		PUSH	DS:[DI].FndOffset.Lo
		MOV	DS,BX
		MOV	BX,OFFSET FnTbl
		CALL	ViewLoad
		POP	SI
		POP	DI
		JC	@@Restore
		CALL	ViewFile
  @@Restore:	POP	ES:WorkBuffer
		POP	ES:TaskStack
		CALL	ResWin
		CALL	Window
		POP	CX
		POP	DI
		POP	SI
		POP	DS
		REP	MOVSB
  @@ViewExit:	XOR	AX,AX
		JMP	@@Exit
  FindSubInit:	MOV	ES:MinMnuFlag,1
		MOV	ES:ListLoop,1
		POP	SI
		PUSH	SI
		CMP	SI,OFFSET FindFile2_B
		JNE	@@DialList
		MOV	ES:ListCurFlag,1
  @@DialList:	POP	SI
		CALL	DialogList
  @@Exit:	RET
FindSub2	ENDP

;---------------------------------------------------------------
;	Display line
;		   SI - current line number
;		   DI - first line number
;		BL,BH - base of window
;	Return:	BL,BH - base of line
;		DL,DH - size of line winwod
;---------------------------------------------------------------
FndNameWidth	EQU	18

FindLine	PROC	NEAR
		USES	AX,CX,SI,DI,ES
		MOV	AX,SI
		SUB	AX,DI
		ADD	BX,DS:ListBase
		ADD	BH,AL
		CALL	FindNum2Adr
		MOV	DI,OFFSET LineBuf4
		PUSH	DI
		MOV	CX,LenStr
		TEST	DS:[SI].FndAttr,80h	; Path
		JZ	@@Entry
		ADD	SI,OFFSET FndName
		CALL	strcpy
		POP	SI
		MOV	AX,CX
		CALL	ShrtLin
		MOV	DI,SI
		PUSH	CX
		CALL	StrLen
		POP	AX
		DEC	CX
		ADD	DI,CX
		SUB	AX,CX
		MOV	CX,AX
		MOV	AL,' '
		REP	STOSB
		JMP	@@PutLine
  @@Entry:	MOV	AL,' '			; Clear line
		REP	STOSB
		POP	DI
		PUSH	DI
		INC	DI			; Name
		INC	DI
		MOV	CX,DS:[SI].FndLenShort
		MOV	AX,DS:[SI].FndLenLong
		PUSH	SI
		OR	AX,AX
		JE	@@NameLen
		ADD	SI,CX
		MOV	CX,AX
  @@NameLen:	ADD	SI,OFFSET FndName
		DEC	CX
		CMP	CX,FndNameWidth
		JBE	@@PutName
		MOV	CX,FndNameWidth-1
  @@PutName:	REP	MOVSB
		JBE	@@PutSize
		MOV	AL,ES:Scrolls[1]
		STOSB
  @@PutSize:	POP	SI			; Size
		MOV	DI,OFFSET LineBuf4+2+FndNameWidth+1
		TEST	DS:[SI].FndAttr,10h
		JNE	@@PutDir
		PUSH	BX
		MOV	CX,DS:[SI].FndSize.Lo
		MOV	BX,DS:[SI].FndSize.Hi
		MOV	AL,3
		CALL	Number
		POP	BX
		JMP	@@PutAttr
  @@PutDir:	PUSH	DS
		PUSH	SI
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET FindSubDir_S
		MOV	CX,13
		REP	MOVSB
		POP	SI
		POP	DS
  @@PutAttr:	MOV	AL,' '
		STOSB
		STOSB
		MOV	AL,DS:[SI].FndAttr	; Attributes
		CALL	PutAttr
		INC	DI
		INC	DI
		MOV	AL,1			; Date and time
		CALL	PutDate
		MOV	AL,' '
		STOSB
		STOSB
		CALL	PutTime
		MOV	AL,' '
		STOSB
		POP	SI
  @@PutLine:	PUSH	ES			; Output line
		POP	DS
		MOV	CL,DS:ListWindow.X
		MOV	CH,0
		MOV	DL,CL
		MOV	DH,1
		MOV	AH,CS:[BP]
		CALL	Color
		CALL	BasAdr
  @@PutChar:	LODSB
		STOSW
		LOOP	@@PutChar
		CLC
		RET
FindLine	ENDP

;---------------------------------------------------------------
;	Convert line number to entry address
;		   SI - line number
;	Return:	DS:SI - entry address
;---------------------------------------------------------------
FindNum2Adr	PROC	NEAR
		USES	AX,BX,CX,DX
		MOV	BX,SI
		LDS	SI,ES:FndPoint2
		MOV	AX,BX
		XCHG	AX,ES:FndCursor
		SUB	BX,AX
		JE	@@Exit
		JA	@@Prepare
		MOV	DS,ES:WorkBuffer
		MOV	SI,OFFSET FnBegin
		ADD	BX,AX
		JE	@@Exit
  @@Prepare:	MOV	CL,4
		MOV	DX,DS
  @@Next:	MOV	AX,DS:[SI].FndLenShort
		ADD	AX,DS:[SI].FndLenLong
		ADD	AX,SIZE FindEnt
		ADD	AX,SI
		MOV	SI,AX
		AND	SI,0Fh
		SHR	AX,CL
		ADD	DX,AX
		MOV	DS,DX
		DEC	BX
		JNE	@@Next
  @@Exit:	MOV	ES:FndPoint2.Offs,SI
		MOV	ES:FndPoint2.Segm,DS
		RET
FindNum2Adr	ENDP

;---------------------------------------------------------------
;	Find file and directory entries
;	Return:	DS:SI - file entry
;		BP:DI - directory entry
;---------------------------------------------------------------
FindEntry	PROC	FAR
		USES	AX,BX,CX,DX
		MOV	BX,ES:ListCursor
		MOV	DS,ES:WorkBuffer
		MOV	SI,OFFSET FnBegin
		MOV	BP,DS
		MOV	DI,SI
		OR	BX,BX
		JE	@@Exit
		MOV	CL,4
		MOV	DX,DS
  @@Loop:	MOV	AX,DS:[SI].FndLenShort
		ADD	AX,DS:[SI].FndLenLong
		ADD	AX,SIZE FindEnt
		ADD	AX,SI
		MOV	SI,AX
		AND	SI,0Fh
		SHR	AX,CL
		ADD	DX,AX
		MOV	DS,DX
		TEST	DS:[SI].FndAttr,80h
		JZ	@@Next
		MOV	BP,DS
		MOV	DI,SI
  @@Next:	DEC	BX
		JNE	@@Loop
  @@Exit:	RET
FindEntry	ENDP

;---------------------------------------------------------------
;	Prepare search string
;		   AL - text / hex search
;		DS:SI - translation table
;---------------------------------------------------------------
Search		EQU	BYTE PTR HelpBuf	; Search data

		.CODE
PrepareSearch	PROC	NEAR
		USES	AX,BX,CX,SI,DI,DS
		MOV	ES:Search.SrTblDecode.Offs,SI
		MOV	ES:Search.SrTblDecode.Segm,DS
		MOV	BX,DS:[SI].Translate
		PUSH	ES
		POP	DS
		PUSH	AX
		MOV	SI,OFFSET SrchStr
		MOV	DI,OFFSET Search.SrText
		PUSH	DI
		CALL	strcpy
		POP	SI
		CMP	DS:SrchCase,0
		JNZ	@@LenText
		CALL	PathLin
  @@LenText:	SUB	DI,SI
		MOV	DS:Search.SrLenText,DI
		MOV	CX,OFFSET TransNone
		CMP	BX,CX
		JE	@@SetProc
		MOV	CX,OFFSET SrchChar1
		CMP	BX,OFFSET TransDBCS
		JNE	@@SetProc
		MOV	CX,OFFSET SrchChar2
		ADD	DI,DI
  @@SetProc:	MOV	DS:Search.SrLenTextFull,DI
		MOV	DS:Search.SrTransProc,CX
		MOV	DS:Search.SrLenHex,0
		POP	AX
		CMP	AL,0
		JZ	@@Exit
		MOV	SI,OFFSET SrchStr
		MOV	DI,OFFSET Search.SrHex
  @@GetHex:	LODSB
		CMP	AL,' '
		JE	@@GetHex
		CMP	AL,0
		JE	@@Exit
		CALL	TstHex
		JC	@@NoHex
		MOV	DS:[DI],AL
		INC	DS:Search.SrLenHex
		LODSB
		CMP	AL,0
		JE	@@Exit
		CALL	TstHex
		JC	@@NextHex
		MOV	AH,DS:[DI]
		MOV	CL,4
		SHL	AH,CL
		ADD	AL,AH
		MOV	DS:[DI],AL
		INC	SI
  @@NextHex:	DEC	SI
		INC	DI
  @@FindNext:	LODSB
		CMP	AL,' '
		JE	@@FindNext
		CMP	AL,','
		JE	@@GetHex
		DEC	SI
		JMP	@@GetHex
  @@NoHex:	MOV	DS:Search.SrLenHex,0
  @@Exit:	RET
PrepareSearch	ENDP

SrchChar2	PROC	NEAR
		MOV	AH,DS:[SI]
		INC	SI
SrchChar2	ENDP

SrchChar1	PROC	NEAR
		USES	DS
		MOV	DS,ES:Search.SrTblDecode.Segm
		CALL	DS:[BX].Translate
		RET
SrchChar1	ENDP

;---------------------------------------------------------------
;	Search next
;		   CX - bytes in buffer
;		DS:SI - buffer
;	Return:	   SI - offset
;		   AL - text / hex substring
;		   CY - not found
;---------------------------------------------------------------
SearchNext	PROC	NEAR
		USES	BX,CX,DX,DI,BP
		PUSH	SI
		PUSH	SI
		CALL	Search1
		JC	@@Exit
		JE	@@NextHex
  @@GetChar:	LODSB
		CMP	ES:Search.SrLenHex,0
		JE	@@ConvChar
		CMP	AL,ES:Search.SrHex[0]
		JNE	@@ConvChar
		PUSH	CX
		MOV	CX,ES:Search.SrLenHex
		DEC	CX
		JE	@@ExitHex
		PUSH	DI
		PUSH	SI
		MOV	DI,OFFSET Search.SrHex+1
		REPE	CMPSB
		POP	SI
		POP	DI
  @@ExitHex:	POP	CX
		JNE	@@ConvChar
		MOV	AL,1
		JMP	@@Found
  @@ConvChar:	JCXZ	@@NextHex
		PUSH	SI
		CALL	BP
		CMP	ES:SrchCase,0
		JNZ	@@CmpChar
		CALL	UpCase
  @@CmpChar:	CMP	AL,ES:[DI]
		JNE	@@Next
		PUSH	CX
		MOV	CX,ES:Search.SrLenText
		DEC	CX
		JE	@@ExitText
		PUSH	DI
		INC	DI
  @@GetText:	LODSB
		CALL	BP
		CMP	ES:SrchCase,0
		JNZ	@@CmpText
		CALL	UpCase
  @@CmpText:	SCASB
		LOOPE	@@GetText
		POP	DI
  @@ExitText:	POP	CX
		JNE	@@Next
		POP	SI
		MOV	AL,0
  @@Found:	DEC	SI
		POP	CX
		PUSH	SI
		JMP	@@Exit
  @@Next:	POP	SI
		LOOP	@@GetChar
		POP	AX
		PUSH	SI
  @@NextHex:	SUB	DX,1
		JAE	@@GetChar
  @@Exit:	POP	SI
		POP	CX
		PUSHF
		SUB	SI,CX
		POPF
		RET
SearchNext	ENDP

;---------------------------------------------------------------
;	Search previous
;		   CX - bytes in buffer
;		DS:SI - buffer
;	Return:	   SI - offset
;		   AL - text / hex substring
;		   CY - not found
;---------------------------------------------------------------
SearchPrev	PROC	NEAR
		USES	BX,CX,DX,DI,BP
		CALL	Search1
		PUSH	CX
		PUSH	SI
		JC	@@Error
		SUB	SI,ES:Search.SrLenHex
  @@GetChar:	STD
		LODSB
		CLD
		CMP	ES:Search.SrLenHex,0
		JE	@@ChkHex
		CMP	AL,ES:Search.SrHex[0]
		JNE	@@ChkHex
		PUSH	CX
		MOV	CX,ES:Search.SrLenHex
		DEC	CX
		JE	@@ExitHex
		PUSH	DI
		PUSH	SI
		INC	SI
		INC	SI
		MOV	DI,OFFSET Search.SrHex+1
		REPE	CMPSB
		POP	SI
		POP	DI
  @@ExitHex:	POP	CX
		JNE	@@ChkHex
		MOV	AL,1
		JMP	@@Found
  @@ChkHex:	OR	DX,DX
		JE	@@ConvChar
		DEC	DX
		JMP	@@GetChar
  @@ConvChar:	JCXZ	@@Error
		PUSH	SI
		INC	SI
		INC	SI
		CALL	BP
		CMP	ES:SrchCase,0
		JNZ	@@CmpChar
		CALL	UpCase
  @@CmpChar:	CMP	AL,ES:[DI]
		JNE	@@Next
		PUSH	CX
		MOV	CX,ES:Search.SrLenText
		DEC	CX
		JE	@@ExitText
		PUSH	DI
		INC	DI
  @@GetText:	LODSB
		CALL	BP
		CMP	ES:SrchCase,0
		JNZ	@@CmpText
		CALL	UpCase
  @@CmpText:	SCASB
		LOOPE	@@GetText
		POP	DI
  @@ExitText:	POP	CX
		JNE	@@Next
		POP	SI
		MOV	AL,0
  @@Found:	MOV	DI,SI
		INC	DI
		POP	SI
		POP	CX
		SUB	SI,DI
		JMP	@@Exit
  @@Next:	POP	SI
		LOOP	@@GetChar
  @@Error:	POP	SI
		POP	SI
		STC
  @@Exit:	RET
SearchPrev	ENDP

Search1		PROC	NEAR
		INC	CX
		MOV	AX,ES:Search.SrLenTextFull
		MOV	DX,AX
		SUB	DX,ES:Search.SrLenHex
		SUB	CX,AX
		JAE	@@Exit
		ADD	DX,CX
		MOV	CX,0
		CMC
		JC	@@Exit
		OR	CX,CX
  @@Exit:	MOV	DI,OFFSET Search.SrText
		MOV	BX,ES:Search.SrTblDecode.Offs
		MOV	BP,ES:Search.SrTransProc
		RET
Search1		ENDP

;---------------------------------------------------------------
;	Select table
;		DS:SI - buffer for table
;		   AL - choose/load table
;	Return:	   AL - number of table
;		   BX - translation procedure
;		   CY - error
;---------------------------------------------------------------
TblPath		EQU	BYTE PTR HelpBuf[0]
TblPoints	EQU	WORD PTR HelpBuf[2]
TblList		EQU	BYTE PTR HelpBuf[100]
TblMask		EQU	BYTE PTR TmpBuf
TblMax		EQU	35

		.CONST
Table_S		DB	" Table ",0
TableMask_S	DB	"*.tbl",0
TableDataErr_S	DB	"Incorrect data in the file",0

		.DATA
Table_P		DB	Box_C, BoxCu_C, 0, 0
		DW	Table_S
		DW	0FFFEh
		DW	0
Table_B		BOXHD	< Table_P, 8500h, DialogList, Table_K, Def_M, Default_H, 0 >
		DB	0
TblError_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Table_S
		DW	Inc0_S
		DW	Line1_S
		DW	Ok_S
		DW	0
TblError_B	BOXHD	< TblError_P, 900h, 0, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
TableReadErr_T	DW	CopyReadEr_S, TblMask
TableDataErr_T	DW	TableDataErr_S, TblMask

		.CODE
TableSelect	PROC	FAR
		MOV	CH,AL
		MOV	CL,DS:[SI].TblNumb
		PUSH	DS
		PUSH	SI
		MOV	BX,OFFSET TblPoints	; Read tables
		MOV	DI,OFFSET TblList
		MOV	ES:[BX],DI
		INC	BX
		INC	BX
		MOV	AX,1
		STOSB
		STOSW
		MOV	DS,CS:SubsSeg
		MOV	SI,OFFSET PlainText_S
		CALL	strcpy
		PUSH	ES
		POP	DS
		CMP	CX,100h
		JE	@@None
		INC	DI
		MOV	AL,0
		MOV	SI,OFFSET VcSet
		CALL	TableRead
		JC	@@ExitErr
		PUSH	DI
		MOV	SI,OFFSET VcDir
		MOV	DI,OFFSET VcSet
		CALL	CompNormLines
		POP	DI
		JZ	@@Sort0
		MOV	AL,1
		CALL	TableRead
		JC	@@ExitErr
  @@Sort0:	PUSH	CX			; Sort tables
  @@Sort1:	MOV	SI,OFFSET TblPoints+2
		MOV	DL,0
  @@Sort2:	INC	SI
		INC	SI
		CMP	SI,BX
		JAE	@@Sort3
		PUSH	SI
		MOV	DI,DS:[SI]
		MOV	SI,DS:[SI-2]
		INC	SI
		INC	DI
		CALL	StrLen
		REPE	CMPSB
		POP	SI
		JBE	@@Sort2
		MOV	DI,DS:[SI]
		XCHG	DI,DS:[SI-2]
		MOV	DS:[SI],DI
		MOV	DL,1
		JMP	@@Sort2
  @@Sort3:	OR	DL,DL
		JNE	@@Sort1
		MOV	SI,OFFSET TblPoints	; Dialog
		MOV	DX,16
  @@BoxSize1:	CMP	SI,BX
		JAE	@@BoxSize2
		INC	DH
		LODSW
		MOV	DI,AX
		INC	DI
		CALL	StrLen
		ADD	DI,CX
		CALL	StrLen
		DEC	CX
		CMP	CL,DL
		JBE	@@BoxSize1
		MOV	DL,CL
		JMP	@@BoxSize1
  @@BoxSize2:	ADD	DL,4
		MOV	AL,DH
		MOV	AH,0
		MOV	DH,DS:Lines
		SUB	DH,5+4
		CMP	DH,AL
		JBE	@@ChkCur
		MOV	DH,AL
  @@ChkCur:	POP	CX
		MOV	BL,CH
		MOV	CH,0
		CMP	CX,AX
		JB	@@InitVars
		XOR	CX,CX
  @@InitVars:	MOV	DS:ListWindow,DX
		MOV	DS:ListBase,205h
		MOV	DS:ListTotal,AX
		MOV	DS:ListCursor,CX
		MOV	DS:ListFirst,0
		MOV	DS:ListLine,OFFSET TableLine
		MOV	DS:ListHotKey,OFFSET TableHotKey
		OR	BL,BL
		JNE	@@GetCur
		MOV	BP,OFFSET Table_B
		CALL	Dialog
		JNZ	@@ExitErr
  @@GetCur:	MOV	BX,DS:ListCursor
		MOV	SI,BX
		ADD	SI,SI
		JE	@@None
		MOV	DI,DS:TblPoints[SI]	; Load table
		MOV	SI,OFFSET VcSet
		CMP	BYTE PTR DS:[DI],0
		JZ	@@Read
		MOV	SI,OFFSET VcDir
  @@Read:	PUSH	DI
		MOV	DX,OFFSET TblMask
		MOV	DI,DX
		CALL	strcpy
		POP	SI
		INC	SI
		CALL	strcpy
		CALL	ChgDisk
		JC	@@ExitErr
		MOV	BP,OFFSET TableReadErr_T
		MOV	AL,40h
		CALL	OpenFil
		JC	@@ChkErr
		PUSH	BX
		MOV	BX,AX
		MOV	DI,OFFSET TblList
		CALL	strcpy
		LEA	DX,[DI+1]
		MOV	CX,TblWidth+2+100h+1
		MOV	AH,3Fh
		CALL	Intr21
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	SI
		POP	DI
		POP	BX
		JC	@@ChkErr
		PUSH	DI
		PUSH	SI
		POP	AX
		POPF
		JC	@@ChkErr
		MOV	SI,DX
		MOV	DI,AX
		ADD	DI,DX
		MOV	BYTE PTR DS:[DI],13
  @@SkipTitle:	LODSW
		CMP	AX,0A0Dh
		JE	@@ChkSize
		CMP	AX,0D0Ah
		JE	@@ChkSize
		DEC	SI
		CMP	AL,0Dh
		JE	@@ChkSize
		CMP	AL,0Ah
		JNE	@@SkipTitle
  @@ChkSize:	MOV	BP,OFFSET TableDataErr_T
		SUB	DI,SI
		JB	@@Error
		CMP	DI,101h
		JNE	@@Error
		LODSB
		CMP	AL,0
		JNE	@@PrepDBCS
		POP	DI			; SBCS
		POP	ES
		PUSH	DI
		ADD	DI,OFFSET Decode
		CALL	TableFill
		REP	MOVSB
		POP	DI
		PUSH	DI
		CALL	TableFill
  @@GetSBCS:	LODSB
		CMP	AL,0
		JNE	@@NextSBCS
		MOV	AL,'.'
  @@NextSBCS:	STOSB
		LOOP	@@GetSBCS
		MOV	AX,BX
		MOV	BX,OFFSET TransSBCS
		JMP	@@ExitTbl
  @@PrepDBCS:	MOV	DL,BL			; DBCS
		MOV	DI,OFFSET TmpErr
		MOV	CX,100h
		MOV	AL,0
		PUSH	DI
		REP	STOSB
		POP	DI
		PUSH	SI
		MOV	BH,0
		MOV	CL,80h
  @@Frequency:	LODSW
		MOV	BL,AH
		INC	BYTE PTR DS:[DI+BX]
		LOOP	@@Frequency
		MOV	BX,DI
		MOV	AH,0
		MOV	SI,DI
		MOV	CH,1
  @@MaxHi1:	LODSB
		CMP	AL,AH
		JBE	@@MaxHi2
		MOV	AH,AL
		MOV	BX,SI
		DEC	BX
  @@MaxHi2:	LOOP	@@MaxHi1
		SUB	BX,DI
		MOV	DH,BL
		POP	SI
		POP	DI
		POP	ES
		MOV	ES:[DI].HiDBCS,DH
		PUSH	DI
		PUSH	SI
		ADD	DI,OFFSET Encode
		MOV	CL,80h
		REP	MOVSW
		POP	SI
		POP	DI
		PUSH	DI
		MOV	CH,1
		OR	DH,DH
		JNE	@@FillDBCS
		CALL	TableFill
  @@FillDBCS:	MOV	AL,'.'
		REP	STOSB
		POP	DI
		PUSH	DX
		MOV	BH,0
		MOV	DL,80h
		MOV	CL,80h
  @@ConvDBCS1:	LODSW
		CMP	AH,DH
		JNE	@@ConvDBCS2
		CMP	AX,80h
		JB	@@ConvDBCS2
		MOV	BL,AL
		MOV	AL,ES:[DI+BX]
		CMP	AL,'.'
		JNE	@@ConvDBCS2
		MOV	ES:[DI+BX],DL
  @@ConvDBCS2:	INC	DX
		LOOP	@@ConvDBCS1
		POP	AX
		MOV	BX,OFFSET TransDBCS
		PUSH	DI
  @@ExitTbl:	PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		POP	SI
		CLC
		JMP	@@SetParam
  @@ChkErr:	CMP	AX,53h
		JE	@@ExitErr
  @@Error:	MOV	DX,BP
		MOV	BP,OFFSET TblError_B
		CALL	Dialog
  @@ExitErr:	STC
  @@None:	PUSHF
		MOV	DI,OFFSET TblList
		LEA	SI,[DI+3]
		CALL	strcpy
		XOR	AX,AX
		POPF
		POP	SI
		POP	DS
		MOV	BX,OFFSET TransNone
		JC	@@Exit
  @@SetParam:	MOV	DS:[SI].TblNumb,AL
		MOV	DS:[SI].Translate,BX
  @@Exit:	RET
TableSelect	ENDP

TableRead	PROC	NEAR			; Read tables
		USES	CX
		MOV	DS:TblPath,AL
		PUSH	DI
		MOV	DX,OFFSET TblMask
		MOV	DI,DX
		CALL	strcpy
		PUSH	DS
		MOV	DS,CS:SubsSeg
		MOV	SI,OFFSET TableMask_S
		CALL	strcpy
		POP	DS
		POP	DI
		CALL	ChgDisk
		JC	@@Error
		MOV	CX,27h
		MOV	AH,4Eh
		JMP	@@FindFile
  @@ChkSize:	CMP	BX,OFFSET TblPoints+2*TblMax
		JAE	@@Exit
		MOV	DS:[BX],DI
		CMP	DS:DTA.FilSize.Hi,0
		JNE	@@Skip
		MOV	AL,DS:TblPath
		STOSB
		MOV	SI,OFFSET DTA.FilName
		CALL	strcpy
		INC	DI
		PUSH	DI
		MOV	SI,DX
		CALL	CutPath
		MOV	DI,SI
		MOV	SI,OFFSET DTA.FilName
		CALL	strcpy
		POP	DI
		MOV	AL,40h
		CALL	OpenFil
		JC	@@ChkErr
		PUSH	DX
		PUSH	BX
		MOV	BX,AX
		MOV	CX,TblWidth+2
		MOV	DX,DI
		MOV	AH,3Fh
		CALL	Intr21
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	BP
		POP	SI
		POP	BX
		POP	DX
		JC	@@ChkErr
		MOV	AX,BP
		PUSH	SI
		POPF
		JC	@@ChkErr
		CMP	AX,CX
		JB	@@Skip
		MOV	SI,DI
		MOV	BYTE PTR DS:[SI+TblWidth+1],13
  @@ChkTitle1:	MOV	CX,SI
  @@ChkTitle2:	LODSW
		CMP	AX,0A0Dh
		JE	@@ChkTitle3
		CMP	AX,0D0Ah
		JE	@@ChkTitle3
		DEC	SI
		CMP	AL,13
		JE	@@ChkTitle3
		CMP	AL,10
		JE	@@ChkTitle3
		CMP	AL,' '
		JE	@@ChkTitle2
		CMP	AL,9
		JE	@@ChkTitle2
		JMP	@@ChkTitle1
  @@ChkTitle3:	SUB	SI,DI
		ADD	SI,101h
		CMP	SI,DS:DTA.FilSize.Lo
		JNE	@@Skip
		MOV	SI,DI
		SUB	CX,DI
		JNE	@@SetName2
		MOV	SI,OFFSET FindData.WinLongName
		CMP	BYTE PTR DS:[SI],0
		JNE	@@SetName1
		MOV	SI,OFFSET DTA.FilName
  @@SetName1:	PUSH	SI
		CALL	FindFileExt
		DEC	SI
		MOV	CX,SI
		POP	SI
		SUB	CX,SI
  @@SetName2:	CMP	CX,TblWidth
		JA	@@Skip
		PUSH	DI
		PUSH	CX
		REP	MOVSB
		MOV	AL,0
		STOSB
		POP	CX
		POP	SI
		INC	CX
		MOV	BP,OFFSET TblPoints
  @@ChkDupes:	CMP	BP,BX
		JAE	@@Next
		PUSH	DI
		MOV	DI,DS:[BP]
		INC	BP
		INC	BP
		INC	DI
		PUSH	CX
		CALL	StrLen
		ADD	DI,CX
		POP	CX
		PUSH	SI
		PUSH	CX
		REPE	CMPSB
		POP	CX
		POP	SI
		POP	DI
		JNE	@@ChkDupes
		JMP	@@Skip
  @@Next:	INC	BX
		INC	BX
		JMP	@@FindNext
  @@ChkErr:	CMP	AX,53h
		JE	@@Error
  @@Skip:	MOV	DI,DS:[BX]
  @@FindNext:	MOV	AH,4Fh
  @@FindFile:	CALL	Intr21
		JNC	@@ChkSize
		XOR	AX,53h
		JNE	@@Exit
  @@Error:	STC
  @@Exit:	CALL	CloseFindFile
		RET
TableRead	ENDP

TableFill	PROC	NEAR			; Fill symbols 0-127
		MOV	CX,80h
		MOV	AL,0
  @@Next:	STOSB
		INC	AX
		LOOP	@@Next
		MOV	CL,80h
		RET
TableFill	ENDP

		.CODE	Subs
TableLine	PROC	NEAR
		USES	AX,CX,SI,DI,ES
		MOV	AX,SI
		SUB	AX,DI
		ADD	BX,DS:ListBase
		ADD	BH,AL
		MOV	AX,SI
		ADD	AX,'1'
		CMP	AX,'9'
		JBE	@@HotKey
		ADD	AX,'A'-'9'-1
		CMP	AX,'Z'
		JBE	@@HotKey
		MOV	AL,' '
  @@HotKey:	PUSH	AX
		ADD	SI,SI
		MOV	DI,DS:TblPoints[SI]
		INC	DI
		CALL	StrLen
		ADD	DI,CX
		MOV	SI,DI
		CALL	BasAdr
		POP	AX
		MOV	ES:[DI],AL
		ADD	DI,6
		MOV	AH,CS:[BP]
		CALL	Color
		MOV	ES:[DI-1],AH
		MOV	CL,DS:ListWindow.X
		MOV	CH,0
		SUB	CL,3
  @@Char:	LODSB
		CMP	AL,9
		JE	@@Tab
		CMP	AL,0
		JNE	@@Write
		DEC	SI
  @@Tab:	MOV	AL,' '
  @@Write:	STOSW
		LOOP	@@Char
		MOV	DL,DS:ListWindow.X
		MOV	DH,1
		CLC
		RET
TableLine	ENDP

TableHotKey	PROC	NEAR
		CMP	AX,4200h
		JNE	@@HotKeys
		MOV	AX,DS:ListCursor
		INC	AX
		CMP	AX,DS:ListTotal
		JB	@@Cursor
		XOR	AX,AX
  @@Cursor:	MOV	DS:ListCursor,AX
		MOV	AX,1C0Dh
		STC
		JMP	@@Exit
  @@HotKeys:	CALL	ModeHotKey
  @@Exit:	RET
TableHotKey	ENDP

		.CODE
TransDBCS	PROC	NEAR
		CMP	AH,DS:[BX].HiDBCS
		JE	TransSBCS
		CMP	AX,80h
		JB	@@Exit
		PUSH	ES
		PUSH	DS
		POP	ES
		PUSH	DI
		PUSH	CX
		LEA	DI,[BX].Encode
		MOV	CX,80h
		REPNE	SCASW
		MOV	AX,DI
		POP	CX
		POP	DI
		POP	ES
		JNE	@@Unknown
		SUB	AX,BX
		SHR	AX,1
		DEC	AX
		RET
  @@Unknown:	MOV	AL,'.'
  @@Exit:	RET
TransDBCS	ENDP

TransSBCS	PROC	NEAR
		XLATB
TransSBCS	ENDP

TransNone	PROC	NEAR
		RET
TransNone	ENDP
