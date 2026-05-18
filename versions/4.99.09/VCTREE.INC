;---------------------------------------------------------------
;		Tree functions
;
;		Version:	30.05.2000
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Convert to long tree path
;		DS:SI - short path
;		ES:DI - long path
;---------------------------------------------------------------
		.CODE	Subs
TreePath	PROC	FAR			; Short/Long path under Win95
		PUSH	DS			; Short path under OS/2
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JNZ	ShortTreePath
TreePath	ENDP

TreePath1	PROC	FAR			; Short/Long path
		CMP	DS:LongNames,0
		JZ	ShortTreePath
		JMP	LongTreePath1
TreePath1	ENDP

LongTreePath	PROC	FAR			; Long path under Win95
		PUSH	DS			; Short path under OS/2
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JZ	LongTreePath1
ShortTreePath:	PUSH	DI
		PUSH	SI
		PUSH	AX
		CALL	strcpy
		POP	AX
		POP	SI
		POP	DI
		RET
LongTreePath	ENDP

LongTreePath1	PROC	FAR			; Long path
		USES	AX,BX,CX,DX,SI,DI,DS,ES
		PUSH	BP
		LODSB
		STOSB
		CMP	AL,0
		JE	@@Exit
		CMP	WORD PTR DS:[SI],'\:'
		JNE	@@MovePath
		MOVSW
		MOV	BX,DI
		MOV	CX,ES
		MOV	DI,SI
		PUSH	DS
		POP	ES
		MOV	DS,CS:DataSegment
		MOV	DS,DS:TreeSeg
		MOV	SI,DS:TreeLen
		CALL	TreeNumb2Adr1
		MOV	BP,SI
		MOV	DL,1
		MOV	SI,OFFSET TreeList
		CALL	NextDirEnt
  @@FindLevel:	CMP	SI,BP
		JAE	@@NotFound
		CMP	DL,DS:[SI].DirLevel
		JB	@@NextEntry
		JA	@@NotFound
		PUSH	SI
		ADD	SI,OFFSET DirName
		CALL	CompDirEntry
		POP	SI
		JNE	@@NextEntry
		MOV	DI,AX
		PUSH	ES
		PUSH	DI
		PUSH	SI
		MOV	DH,ES:[DI]
		MOV	ES,CX
		MOV	DI,BX
		CALL	LongDirOffset1
		CALL	strcpy
		OR	DH,DH
		JE	@@SaveTarget
		MOV	AL,'\'
		STOSB
  @@SaveTarget:	MOV	BX,DI
		POP	SI
		POP	DI
		POP	ES
		JE	@@Exit
		INC	DI
		INC	DX
  @@NextEntry:	CALL	NextDirEnt
		JMP	@@FindLevel
  @@NotFound:	PUSH	ES
		POP	DS
		MOV	SI,DI
		MOV	ES,CX
		MOV	DI,BX
  @@MovePath:	CALL	strcpy
  @@Exit:	POP	BP
		RET
LongTreePath1	ENDP

;---------------------------------------------------------------
;	Remove tree seg, read treeinfo to WorkBuffer
;		ES:DX - path
;		   AX - additional memory
;	Return:	   DS - WorkBuffer
;		   BP - limit
;		 CY=1 - error, AX - error code
;---------------------------------------------------------------
		.CODE
GetTreeInfo	PROC	NEAR
		USES	BX,CX,DX,SI,DI
		PUSH	AX
		MOV	BX,DX
		MOV	AX,ES:[BX]
		CMP	AL,0
		JE	@@SetSegm
		CMP	AH,':'
		JNE	@@SetSegm
		CALL	UpCase
		SUB	AL,'A'
		MOV	DL,AL
		MOV	AH,0Eh
		INT	21h
  @@SetSegm:	XOR	BX,BX
		XCHG	BX,ES:TreeSeg
		MOV	SI,OFFSET AllSegs_L
		CALL	CompressBlock
		MOV	DS,ES:WorkBuffer
		MOV	BP,ES
		ADD	BP,ES:SizeDataSeg
		MOV	CX,DS
		SUB	BP,CX
		XOR	AX,AX
		SUB	BP,MinView/16+1
		JB	@@Error
		INC	BP
		CMP	BP,0FFFh
		JBE	@@RdTree
		MOV	BP,0FFFh
  @@RdTree:	MOV	CL,4
		SHL	BP,CL
		CALL	ReadTreeInfo
  @@Error:	POP	SI
		PUSHF
		PUSH	AX
		JC	@@RestoreDrv
		ADD	DI,SI
		JC	@@RestoreDrv
		DEC	DI
		OR	DI,0Fh
		ADD	DI,1
		JC	@@RestoreDrv
		CMP	DI,BP
		JAE	@@RestoreDrv
		MOV	BP,DI
  @@RestoreDrv:	MOV	DL,ES:DosPath[0]
		SUB	DL,'A'
		MOV	AH,0Eh
		INT	21h
		POP	AX
		POPF
		RET
GetTreeInfo	ENDP

;---------------------------------------------------------------
;	Find entry in treeinfo
;		   DS - segment of treeinfo
;		ES:DX - full path to find
;	Return:	ES:DI - rest of path
;		DS:SI - entry point
;		   NC - whole path found
;		   CY - path not found
;---------------------------------------------------------------
FindTreeEntry	PROC	FAR
		USES	AX,CX,DX,BP
		MOV	DI,DX
		ADD	DI,3
		MOV	SI,DS:TreeLen
		CALL	TreeNumb2Adr1
		MOV	BP,SI
		MOV	DL,1
		MOV	CX,OFFSET TreeList
		MOV	SI,CX
		CALL	NextDirEnt
  @@FindLevel:	CMP	SI,BP
		JAE	@@NotFound
		CMP	DL,DS:[SI].DirLevel
		JB	@@NextEntry
		JA	@@NotFound
		PUSH	SI
		LEA	AX,[SI].DirName
		CALL	LongDirOffset
		CMP	AX,SI
		JE	@@CmpShort
		PUSH	AX
		CALL	CompDirEntry
		POP	SI
		JE	@@ExitCmp
  @@CmpShort:	CALL	CompDirEntry
  @@ExitCmp:	POP	SI
		JNE	@@NextEntry
		MOV	CX,SI
		MOV	DI,AX
		CMP	BYTE PTR ES:[DI],0
		JE	@@Exit
		INC	DI
		INC	DX
  @@NextEntry:	CALL	NextDirEnt
		JMP	@@FindLevel
  @@NotFound:	MOV	SI,CX
		STC
  @@Exit:	RET
FindTreeEntry	ENDP

CompDirEntry	PROC	FAR
		USES	DI
  @@CmpEntry:	LODSB
		CMP	AL,0
		JE	@@EndCmp
		CALL	UpCase
		MOV	AH,AL
		MOV	AL,ES:[DI]
		INC	DI
		CALL	UpCase
		CMP	AL,AH
		JE	@@CmpEntry
		JMP	@@Exit
  @@EndCmp:	CMP	AL,ES:[DI]
		MOV	AX,DI
		JE	@@Exit
		CMP	BYTE PTR ES:[DI],'\'
  @@Exit:	RET
CompDirEntry	ENDP

;---------------------------------------------------------------
;	Prepare short and long path
;		DS:SI - source path
;		ES:DI - buffer for short path
;		ES:BX - buffer for long path
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
PrepAddTree	PROC	NEAR
		USES	CX,SI,DI
		CMP	DS:OS2box,0
		JZ	@@WinName
		CALL	NormPath
		JC	@@Exit
		MOV	SI,DI
		MOV	DI,BX
		CALL	LongNameOS2
		CLC
		JMP	@@Exit
  @@WinName:	XCHG	BX,DI
		MOV	CX,8002h
		MOV	AX,7160h
		CALL	Intr21
		XCHG	BX,DI
		JC	@@Error
		CALL	ShortName
		JNC	@@Exit
  @@Error:	CMP	AX,7100h
		STC
		JNE	@@Exit
		MOV	BYTE PTR DS:[BX],0
		CALL	NormPath
  @@Exit:	RET
PrepAddTree	ENDP

;---------------------------------------------------------------
;	Add entry to treeinfo
;		DS:SI - entry point
;		ES:DX - short path
;		ES:DI - rest of short path
;		ES:BX - long path or 0
;		   BP - limit
;	Return:	   NC - no changes
;		   CY - entry was added
;---------------------------------------------------------------
AddTreeEntry	PROC	FAR
		LOCAL	LongPathOffs:WORD
		LOCAL	LenShortEntry:WORD
		LOCAL	LenLongEntry:WORD
		LOCAL	FullShortPath:WORD
		USES	AX,BX,CX,DX
		MOV	FullShortPath,DX
		MOV	DH,0
		CMP	BYTE PTR ES:[DI],0
		JE	@@Exit
		PUSH	SI
		MOV	DL,DS:[SI].DirLevel
		MOV	LongPathOffs,0
		MOV	SI,BX
		PUSH	DX
		OR	SI,SI
		JE	@@NoLong
		INC	DX
  @@LongOffs:	LODS	BYTE PTR ES:[SI]
		CMP	AL,0
		JE	@@NoLong
		CMP	AL,'\'
		JNE	@@LongOffs
		DEC	DX
		JNE	@@LongOffs
		MOV	LongPathOffs,SI
  @@NoLong:	POP	DX
		MOV	SI,DS:TreeLen
		CALL	TreeNumb2Adr1
		MOV	BX,SI
		POP	SI
  @@FindEnd:	CALL	NextDirEnt
		CMP	SI,BX
		JAE	@@AddEntry
		CMP	DL,DS:[SI].DirLevel
		JB	@@FindEnd
  @@AddEntry:	PUSH	DI			; Insert entry
		PUSH	SI
		CALL	AddTreeLenEnt
		MOV	LenShortEntry,AX
		MOV	SI,DI
		MOV	DI,LongPathOffs
		OR	DI,DI
		JE	@@NotLong
		CALL	AddTreeLenEnt
		MOV	LenLongEntry,AX
		CMP	AX,LenShortEntry
		JNE	@@ChkSize
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	CX,AX
		DEC	CX
		REPE	CMPSB
		POP	DS
		JNE	@@ChkSize
  @@NotLong:	MOV	LenLongEntry,0
  @@ChkSize:	POP	SI
		POP	DI
		MOV	AX,SIZE DirEnt
		ADD	AX,LenShortEntry
		ADD	AX,LenLongEntry
		MOV	CX,AX
		INC	AX
		INC	AX
		ADD	AX,BX
		JC	@@Overflow
		CMP	AX,[BP]
		JA	@@Overflow
		PUSH	DI
		MOV	DI,SI
		MOV	SI,BX
		DEC	AX
		DEC	AX
		MOV	BX,AX
		MOV	AX,SI
		SUB	AX,DI
		JBE	@@SaveEntry
		PUSH	ES
		PUSH	DI
		PUSH	DS
		POP	ES
		DEC	SI
		MOV	DI,SI
		ADD	DI,CX
		MOV	CX,AX
		STD
		REP	MOVSB
		CLD
		POP	DI
		POP	ES
  @@SaveEntry:	INC	DX			; Save entry
		MOV	DS:[DI].DirLevel,DL
		POP	SI
		PUSH	DS
		PUSH	SI
		PUSH	DX
		PUSH	BX
		PUSH	ES
		POP	DS
		MOV	DX,FullShortPath
		ADD	SI,LenShortEntry
		DEC	SI
		MOV	BL,0
		XCHG	BL,ES:[SI]
		MOV	ES:BlocEr,1
		MOV	AX,4300h
		CALL	Intr21
		MOV	ES:BlocEr,0
		MOV	ES:[SI],BL
		JNC	@@Attrib
		MOV	CL,0
  @@Attrib:	POP	BX
		POP	DX
		POP	SI
		POP	DS
		AND	CL,27h
		MOV	DS:[DI].DirAttr,CL
		XOR	CX,CX
		MOV	AX,LenLongEntry
		CMP	ES:OS2box,0
		JZ	@@SetLens
		XCHG	AX,CX
  @@SetLens:	MOV	DS:[DI].DirWinLen,AX
		MOV	DS:[DI].DirOS2Len,CX
		MOV	CX,LenShortEntry
		MOV	DS:[DI].DirShortLen,CL
		ADD	DI,OFFSET DirName
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		REP	MOVSB
		DEC	DI
		MOV	AL,0
		STOSB
		MOV	CX,LenLongEntry
		JCXZ	@@SkipLong
		PUSH	SI
		MOV	SI,LongPathOffs
		REP	MOVSB
		DEC	DI
		STOSB
		POP	SI
  @@SkipLong:	PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		XCHG	SI,DI
		INC	DS:TreeLen
		MOV	DH,1
		CMP	BYTE PTR ES:[DI-1],0
		JE	@@Exit
		PUSH	DI
		MOV	DI,LongPathOffs
		OR	DI,DI
		JE	@@NewLongOfs
		CALL	AddTreeLenEnt
		ADD	DI,AX
		CMP	BYTE PTR ES:[DI-1],0
		JNE	@@NewLongOfs
		XOR	DI,DI
  @@NewLongOfs:	MOV	LongPathOffs,DI
		POP	DI
		JMP	@@AddEntry
  @@Overflow:	CALL	Beep
  @@Exit:	CMP	DH,1
		CMC
		RET
AddTreeEntry	ENDP

AddTreeLenEnt	PROC	NEAR			; Length of dir entry
		USES	SI
		MOV	SI,DI
  @@GetChar:	LODS	BYTE PTR ES:[SI]
		CMP	AL,0
		JE	@@Exit
		CMP	AL,'\'
		JNE	@@GetChar
  @@Exit:	MOV	AX,SI
		SUB	AX,DI
		RET
AddTreeLenEnt	ENDP

;---------------------------------------------------------------
;	Delete entry from treeinfo
;		DS:SI - entry point
;---------------------------------------------------------------
DelTreeEntry	PROC	FAR
		USES	AX,CX,SI,DI,ES
		PUSH	SI
		MOV	SI,DS:TreeLen
		CALL	TreeNumb2Adr1
		MOV	CX,SI
		POP	SI
		MOV	DI,SI
		MOV	AL,DS:[SI].DirLevel
  @@FindNext:	CALL	NextDirEnt
		DEC	DS:TreeLen
		CMP	SI,CX
		JAE	@@Delete
		CMP	AL,DS:[SI].DirLevel
		JB	@@FindNext
  @@Delete:	SUB	CX,SI
		JBE	@@Exit
		PUSH	DS
		POP	ES
		REP	MOVSB
  @@Exit:	RET
DelTreeEntry	ENDP

;---------------------------------------------------------------
;	Scan tree
;---------------------------------------------------------------
TreeLen		EQU	WORD PTR TreeHead[0]
TreeLenH	EQU	WORD PTR TreeHead[2]
TreeLFN		EQU	BYTE PTR TreeHead[4]
TreeSize	EQU	WORD PTR TreeHead[5]
TreeEnd		EQU	WORD PTR TreeHead[5]
TreeNumber	EQU	WORD PTR TreeHead[7]
TreeLev		EQU	BYTE PTR TreeHead[9]
TreeBox		EQU	BYTE PTR TreeHead[10]
TreeOldEnd	EQU	WORD PTR TreeHead[11]
TreeOldPoint	EQU	WORD PTR TreeHead[13]
TreeCounter	EQU	WORD PTR TreeHead[15]

		.CONST
TreeInfo_F	DB	'\TREEINFO',0
		DB	'.VCD',0
TreeSign	DB	'VC TREE',13,10
Scan0_S		DB	' Tree ',0
Scan1_S		DB	'Scanning the directory',0
Scan2_S		DB	' structure on drive ^0E:',0
Scan3_S		DB	'Saving the',0
Scan4_S		DB	'directory information...',0

		.DATA
Tree_P		DB	Box_C, 0, 0, 0
		DW	Scan0_S
		DW	Scan1_S
		DW	Scan2_S
		DW	Empty_S
		DW	0
TrSav_P		DB	Box_C, 0, 0, 0
		DW	Scan0_S
		DW	Scan3_S
		DW	Scan4_S
		DW	0
Tree_T		DW	TmpBuf

		.CODE	Subs
ScanTree	PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,BP
		XOR	BX,BX			; Delete old TreeSeg
		XCHG	BX,ES:TreeSeg
		MOV	SI,OFFSET GlobalSegs_L
		CALL	CompressBlock
		PUSH	DS
		MOV	DS,ES:WorkBuffer
		MOV	ES:TreeSeg,DS
		MOV	BP,ES			; Set limit
		ADD	BP,ES:SizeDataSeg
		MOV	CX,DS
		SUB	BP,CX
		SUB	BP,MinView/16
		CMP	BP,0FFFh
		JBE	@@SetLimit
		MOV	BP,0FFFh
  @@SetLimit:	MOV	CL,4
		SHL	BP,CL

		CALL	ReadTreeInfo		; Reading TREEINFO
		JC	@@EmptyTree
		MOV	AL,0
		CMP	AL,ES:RescanTree
		JZ	@@Exit
		LEA	DX,[DI-2]		; Compress old tree
		MOV	SI,OFFSET TreeList
		PUSH	SI
		CALL	TreeCheck
		POP	SI
		PUSH	ES
		PUSH	DS
		POP	ES
		MOV	DI,SI
  @@Compress:	CMP	SI,DX
		JAE	@@ScanTree
		MOV	BX,SI
		CALL	NextDirEnt
		TEST	DS:[BX].DirAttr,80h
		JNZ	@@Compress
		MOV	CX,SI
		SUB	CX,BX
		MOV	SI,BX
		REP	MOVSB
		JMP	@@Compress
  @@FsErr:	MOV	DS:TreeNumber,0FFFDh
		JMP	@@Exit
  @@EmptyTree:	CALL	TreeRoot		; Default tree
		MOV	DI,SIZE DirTree+SIZE DirEnt+2
		CMP	AX,53h
		JE	@@Exit
		MOV	DS:TreeLFN,0
		MOV	AL,ES:DosPath[0]
		CALL	Phantom
		JC	@@FsErr
		CALL	GetFS
		JC	@@FsErr
		OR	AL,AH
		MOV	DS:TreeLFN,AL
		DEC	DI
		DEC	DI
		PUSH	ES

  @@ScanTree:	POP	ES			; Scanning tree
		MOV	AX,OFFSET TreeList
		CMP	DI,OFFSET TreeList+SIZE DirEnt+2
		JA	@@SetRoot
		MOV	DI,AX
		XOR	AX,AX
  @@SetRoot:	MOV	DS:[DI].DirLevel,0
		MOV	DS:[DI].DirAttr,0
		MOV	DS:[DI].DirShortLen,2
		MOV	DS:[DI].DirWinLen,0
		MOV	DS:[DI].DirOS2Len,0
		MOV	WORD PTR DS:[DI].DirName,'\'
		MOV	DS:TreeOldPoint,AX
		MOV	DS:TreeOldEnd,DI
		ADD	DI,SIZE DirEnt+2
		MOV	DS:TreeEnd,DI
		MOV	DS:TreeLev,0
		MOV	DS:TreeLen,1
		MOV	DS:TreeLenH,1
		MOV	DS:TreeCounter,1
		MOV	ES:LineBuf1[0],0
		CALL	TreeFunc
		CMP	AL,0
		JE	@@MoveTree
		MOV	DS:TreeNumber,0FFFDh
  @@MoveTree:	MOV	SI,DS:TreeOldEnd
		MOV	DI,OFFSET TreeList
		MOV	CX,DS:TreeEnd
		SUB	CX,SI
		PUSH	ES
		PUSH	DS
		POP	ES
		REP	MOVSB
		POP	ES
		INC	DI
		INC	DI
		CMP	DS:TreeCounter,5
		JB	@@DelTree
		CALL	ResWin
		CALL	Window

  @@DelTree:	CMP	AL,0			; Save TREEINFO
		JNE	@@Exit
		CALL	TstBrk
		JC	@@Exit
		MOV	AL,ES:TreeInHome
		OR	AL,ES:TreeInRoot
		JE	@@SaveTree
		CMP	DS:TreeCounter,5
		JB	@@SaveTree
		MOV	AL,0
		XCHG	AL,ES:ZoomWin
		MOV	BX,700h
		MOV	BP,OFFSET TrSav_P
		CALL	DialBox
		CALL	Window
		MOV	ES:ZoomWin,AL
  @@SaveTree:	MOV	DX,OFFSET DosPath
		CALL	SaveTreeInfo
		MOV	AL,ES:TreeInHome
		OR	AL,ES:TreeInRoot
		JE	@@Exit
		MOV	AL,0
		CMP	DS:TreeCounter,5
		JB	@@Exit
		CALL	ResWin
		CALL	Window
  @@Exit:	MOV	ES:RescanTree,0		; Set size of TreeSeg
		MOV	SI,DS:TreeLen
		CALL	TreeNumb2Adr1
		MOV	DS:TreeSize,SI
		CALL	TreeAdr2NumbH
		MOV	DS:TreeLenH,SI
		PUSH	AX
		MOV	AX,DI
		DEC	AX
		MOV	CL,4
		SHR	AX,CL
		INC	AX
		MOV	CX,DS
		ADD	AX,CX
		INC	AX
		MOV	ES:WorkBuffer,AX
		POP	AX
		POP	DS
		CMP	AL,1
		CMC
		RET
ScanTree	ENDP

TreeCheck	PROC	NEAR
		MOV	BX,SI
		MOV	AL,DS:[BX].DirLevel
		MOV	AH,0
		CALL	NextDirEnt
  @@Next:	CMP	SI,DX
		JAE	@@ChkWin
		CMP	AL,DS:[SI].DirLevel
		JAE	@@ChkWin
		PUSH	BX
		PUSH	AX
		CALL	TreeCheck
		MOV	CL,AH
		POP	AX
		POP	BX
		OR	AH,CL
		JMP	@@Next
  @@ChkWin:	OR	AH,AH
		JNE	@@Exit
		CMP	DS:TreeLFN,0
		JNZ	@@ChkOS2
		CMP	DS:[BX].DirWinLen,0
		JE	@@ChkOS2
  @@SetFlag:	MOV	AH,1
		JMP	@@Exit
  @@ChkOS2:	CMP	ES:OS2box,0
		JNZ	@@Remove
		CMP	DS:[BX].DirOS2Len,0
		JNE	@@SetFlag
  @@Remove:	OR	DS:[BX].DirAttr,80h
  @@Exit:	RET
TreeCheck	ENDP

TreeFunc	PROC	NEAR
		INC	DS:TreeLev
		PUSH	DI
		PUSH	DI
		MOV	DX,OFFSET LineBuf1
		MOV	DI,DX
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		PUSH	DS
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET Mask_S
		CALL	strcpy
		POP	DS
		POP	DI
		MOV	AH,4Eh
		MOV	CX,37h

  @@NextEntry:	CALL	TstBrk			; Find entry
		MOV	AL,2
		JC	@@FindBranch
		PUSH	DS
		PUSH	ES
		POP	DS
		CALL	Intr21
		POP	DS
		JC	@@Error
		TEST	ES:DTA.FilAttr,10h
		JZ	@@FindNext
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET DTA.FilName
		CALL	ChkUpDir
		POP	DS
		JE	@@FindNext
		CMP	ES:OS2box,0
		JZ	@@PutBox
		PUSH	DS
		PUSH	BP
		PUSH	DI
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET LineBuf1
		PUSH	SI
		CALL	CutPath
		MOV	DI,SI
		MOV	SI,OFFSET DTA.FilName
		CALL	strcpy
		POP	SI
		MOV	DI,OFFSET FindData.WinLongName
		LEA	BP,[SI+260+14]
		CALL	GetLongNameEA
		POP	DI
		POP	BP
		POP	DS
		JNC	@@PutBox
		CMP	AX,53h
		JE	@@FindBranch

  @@PutBox:	INC	DS:TreeLen		; Put box
		INC	DS:TreeCounter
		MOV	CX,DS:TreeCounter
		CMP	CX,5
		JB	@@InsEntry
		PUSH	BP
		PUSH	DI
		JA	@@PutNumber
		MOV	AL,ES:DosPath[0]
		CBW
		MOV	WORD PTR ES:TmpBuf,AX
		MOV	BX,700h
		MOV	DX,OFFSET Tree_T
		MOV	BP,OFFSET Tree_P
		CALL	DialBox
		CALL	Window
		MOV	DS:TreeBox,BH
  @@PutNumber:	XOR	BX,BX			; Write number
		MOV	SI,OFFSET TmpBuf
		MOV	DI,SI
		MOV	AL,1
		CALL	Number
		MOV	BH,DS:TreeBox
		ADD	BH,4
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	BL,DS:Columns
		SHR	BL,1
		MOV	BP,OFFSET Tree_P
		CALL	CntLine
		POP	DS
		SUB	BL,4
		MOV	DX,108h
		CALL	Window
		POP	DI
		POP	BP

  @@InsEntry:	PUSH	DI			; Insert entry
		MOV	DI,OFFSET DTA.FilName
		CALL	StrLen
		XOR	BX,BX
		MOV	SI,DS:TreeOldPoint
		OR	SI,SI
		JE	@@LenLongNam
		MOV	AL,DS:[SI].DirLevel
		INC	AX
  @@FindOld:	CALL	NextDirEnt
		CMP	SI,DS:TreeOldEnd
		JAE	@@LenLongNam
		CMP	AL,DS:[SI].DirLevel
		JB	@@FindOld
		JA	@@LenLongNam
		TEST	DS:[SI].DirAttr,80h
		JNZ	@@FindOld
		PUSH	DI
		PUSH	SI
		PUSH	CX
		ADD	SI,OFFSET DirName
		REPE	CMPSB
		POP	CX
		POP	SI
		POP	DI
		JNE	@@FindOld
		OR	DS:[SI].DirAttr,80h
		XOR	AX,AX
		CMP	DS:TreeLFN,0
		JNZ	@@SkipWinLen
		ADD	AX,DS:[SI].DirWinLen
  @@SkipWinLen:	CMP	ES:OS2box,0
		JNZ	@@SkipOS2Len
		ADD	AX,DS:[SI].DirOS2Len
  @@SkipOS2Len:	OR	AX,AX
		JE	@@LenLongNam
		MOV	BX,SI
		ADD	CX,AX

  @@LenLongNam:	PUSH	CX
		MOV	DI,OFFSET FindData.WinLongName
		CALL	StrLen
		DEC	CX
		JE	@@SizeEntry
		INC	CX
  @@SizeEntry:	POP	AX
		POP	DI
		ADD	AX,CX
		ADD	AX,SIZE DirEnt+2
		MOV	SI,AX
		ADD	AX,DS:TreeEnd
		JC	@@Overflow
		CMP	AX,BP
		JA	@@Overflow
		PUSH	CX
		LEA	CX,[SI-2]
		DEC	AX
		DEC	AX
		XCHG	AX,DS:TreeEnd
		MOV	SI,AX
		SUB	AX,DI
		JBE	@@SaveEntry
		PUSH	ES
		PUSH	DI
		PUSH	DS
		POP	ES
		DEC	SI
		MOV	DI,SI
		ADD	DI,CX
		MOV	CX,AX
		STD
		REP	MOVSB
		CLD
		POP	DI
		POP	ES
  @@SaveEntry:	POP	CX			; Save entry
		MOV	AL,DS:TreeLev
		MOV	DS:[DI].DirLevel,AL
		MOV	AL,ES:DTA.FilAttr
		AND	AL,27h
		MOV	DS:[DI].DirAttr,AL
		PUSH	CX
		XOR	SI,SI
		CMP	ES:OS2box,0
		JZ	@@SetLength
		XCHG	CX,SI
  @@SetLength:	MOV	DS:[DI].DirWinLen,CX
		MOV	DS:[DI].DirOS2Len,SI
		POP	CX
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	SI,OFFSET DTA.FilName
		PUSH	DI
		ADD	DI,OFFSET DirName
		PUSH	DI
		CALL	strcpy
		POP	SI
		INC	DI
		MOV	AX,DI
		SUB	AX,SI
		POP	SI
		MOV	ES:[SI].DirShortLen,AL
		OR	BX,BX
		JE	@@SkipWinOld
		MOV	AL,ES:[BX].DirShortLen
		MOV	AH,0
		ADD	AX,OFFSET DirName
		ADD	AX,BX
		CMP	ES:TreeLFN,0
		JNZ	@@SkipWinOld
		PUSH	DS
		PUSH	SI
		PUSH	CX
		PUSH	ES
		POP	DS
		MOV	CX,DS:[BX].DirWinLen
		MOV	DS:[SI].DirWinLen,CX
		MOV	SI,AX
		REP	MOVSB
		POP	CX
		POP	SI
		POP	DS
  @@SkipWinOld:	JCXZ	@@SkipLong
		PUSH	SI
		MOV	SI,OFFSET FindData.WinLongName
		REP	MOVSB
		POP	SI
  @@SkipLong:	OR	BX,BX
		JE	@@SkipOS2Old
		CMP	DS:OS2Box,0
		JNZ	@@SkipOS2Old
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	CX,DS:[BX].DirOS2Len
		MOV	DS:[SI].DirOS2Len,CX
		ADD	AX,DS:[BX].DirWinLen
		MOV	SI,AX
		REP	MOVSB
		POP	DS
  @@SkipOS2Old:	PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
  @@FindNext:	MOV	AH,4Fh
		JMP	@@NextEntry
  @@Overflow:	CALL	Beep			; Buffer overflow
		MOV	AL,2
		JMP	@@FindBranch
  @@Error:	CMP	AX,53h			; Find error
		JE	@@FindBranch
		MOV	AL,0

  @@FindBranch:	CALL	CloseFindFile		; Find in branches
		POP	DI
		CMP	AL,0
		JNE	@@Exit
  @@NextBranch:	CMP	DI,DS:TreeEnd
		JAE	@@Exit
		MOV	AH,DS:[DI].DirLevel
		CMP	AH,DS:TreeLev
		JB	@@Exit
		PUSH	DS:TreeOldPoint
		XOR	SI,SI
		XCHG	SI,DS:TreeOldPoint
		OR	SI,SI
		JE	@@ScanIn
		MOV	AL,DS:[SI].DirLevel
		INC	AX
  @@FindOld1:	CALL	NextDirEnt
		CMP	SI,DS:TreeOldEnd
		JAE	@@ScanIn
		CMP	AL,DS:[SI].DirLevel
		JB	@@FindOld1
		JA	@@ScanIn
		TEST	DS:[SI].DirAttr,40h
		JNZ	@@FindOld1
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	DS
		POP	ES
		MOV	CL,DS:[DI].DirShortLen
		MOV	CH,0
		ADD	SI,OFFSET DirName
		ADD	DI,OFFSET DirName
		REPE	CMPSB
		POP	SI
		POP	DI
		POP	ES
		JNE	@@FindOld1
		OR	DS:[SI].DirAttr,40h
		MOV	DS:TreeOldPoint,SI
  @@ScanIn:	PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET LineBuf1
		CALL	CutPath
		POP	DS
		XCHG	SI,DI
		PUSH	SI
		CMP	DS:TreeLFN,0
		JE	@@Recurse
		CMP	DS:[SI].DirWinLen,0
		JE	@@Recurse
		MOV	AL,DS:[SI].DirShortLen
		MOV	AH,0
		ADD	SI,AX
  @@Recurse:	ADD	SI,OFFSET DirName
		CALL	strcpy
		POP	SI
		CALL	NextDirEnt
		MOV	DI,SI
		CALL	TreeFunc
		POP	DS:TreeOldPoint
		CMP	AL,0
		JE	@@NextBranch

  @@Exit:	PUSH	DS			; Cut last subdirectory
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET LineBuf1
		CALL	CutPath
		MOV	DS:[SI-1],AL
		POP	DS
		DEC	DS:TreeLev
		RET
TreeFunc	ENDP

;---------------------------------------------------------------
;	Tree root
;		   DS - tree segment
;---------------------------------------------------------------
TreeRoot	PROC	FAR
		USES	SI
		MOV	DS:TreeLen,1
		MOV	DS:TreeLenH,1
		MOV	SI,OFFSET TreeList
		MOV	DS:[SI].DirLevel,0
		MOV	DS:[SI].DirAttr,0
		MOV	DS:[SI].DirShortLen,2
		MOV	DS:[SI].DirWinLen,0
		MOV	DS:[SI].DirOS2Len,0
		MOV	WORD PTR DS:[SI].DirName,'\'
		RET
TreeRoot	ENDP

;---------------------------------------------------------------
;	Read TREEINFO
;		   DS - segment to load
;		   BP - limit
;	Return:	   CY - error, AX - error code
;		   DI - size of TREEINFO
;---------------------------------------------------------------
		.CODE	Subs
ReadTreeInfo	PROC	FAR
		LOCAL	TreeNameBuf:BYTE:512
		LOCAL	TreeLabelBuf:BYTE:14
		LOCAL	TreeSerNumBuf:DWORD
		LOCAL	SaveDTA:BYTE:SIZE DTAs
		LOCAL	SaveFindHandle:WORD
		PUSH	DS		; Get label and serial number
		PUSH	ES
		PUSH	SS
		POP	ES
		POP	DS
		MOV	CX,SIZE DTAs
		MOV	SI,OFFSET DTA
		LEA	DI,SaveDTA
		REP	MOVSB
		XCHG	CX,DS:FindHandle
		MOV	SaveFindHandle,CX
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		PUSH	SS
		POP	DS
		LEA	SI,TreeLabelBuf
		CALL	GetVol
		POP	DS
		JC	@@ErrFind
		MOV	TreeSerNumBuf.Lo,BX
		MOV	TreeSerNumBuf.Hi,CX
		MOV	AL,ES:VcSet[0]
		CALL	Phantom
		MOV	AX,53h
		JC	@@ErrFind
		LEA	DX,TreeNameBuf
		PUSH	DS
		PUSH	SS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	DI,SI
		CALL	StrLen
		DEC	CX
		ADD	DI,CX
		SUB	CX,14
		NEG	CX
		MOV	AL,' '
		REP	STOSB
		MOV	SI,OFFSET VcSet		; Find tree in home directory
		MOV	DI,DX
		CALL	strcpy
		PUSH	DS
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET TreeInfo_F+1
		CALL	strcpy
		MOV	AL,'.'
		STOSB
		MOV	AX,'*'
		STOSW
		POP	ES
		POP	DS
		MOV	CX,27h
		MOV	AH,4Eh
  @@FindFile:	PUSH	DS
		PUSH	SS
		POP	DS
		CALL	Intr21
		POP	DS
		JC	@@ErrFind
		MOV	SI,OFFSET DTA.FilName
  @@FindExt:	LODS	BYTE PTR ES:[SI]
		CMP	AL,0
		JE	@@Next
		CMP	AL,'.'
		JNE	@@FindExt
		MOV	DI,SI
		CALL	StrLen
		CMP	CX,4
		JNE	@@Next
		PUSH	DS
		PUSH	ES
		POP	DS
		CALL	TxtNum
		POP	DS
		JC	@@Next
		MOV	CX,AX
		PUSH	DS
		PUSH	ES
		PUSH	SS
		POP	DS
		MOV	SI,DX
		CALL	CutPath
		PUSH	DS
		POP	ES
		POP	DS
		MOV	DI,SI
		MOV	SI,OFFSET DTA.FilName
		CALL	strcpy
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	AL,40h
		CALL	OpenFil
		POP	DS
		JNC	@@ReadHead
		CMP	AX,53h
		JNE	@@Next
		JMP	@@ErrFind
  @@ReadHead:	MOV	SI,CX
		PUSH	DX
		XOR	DX,DX
		MOV	CX,OFFSET TreeList
		MOV	BX,AX
		MOV	AH,3Fh
		CALL	Intr21
		POP	DX
		JNC	@@ChkHead
		CMP	AX,53h
		JNE	@@Close
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	AX
		JMP	@@ErrFind
  @@ChkHead:	CMP	AX,CX
		JNE	@@Close
		MOV	CX,SI
		MOV	SI,TreeSerNumBuf.Lo
		MOV	DI,TreeSerNumBuf.Hi
		MOV	AX,SI
		OR	AX,DI
		JNE	@@ChkSerNum
		PUSH	ES
		PUSH	CX
		PUSH	SS
		POP	ES
		MOV	CX,11
		MOV	SI,OFFSET TreeLabel
		LEA	DI,OFFSET TreeLabelBuf
		REPE	CMPSB
		POP	CX
		POP	ES
		JNE	@@Close
		MOV	AX,DS:TreeSerNum.Lo
		OR	AX,DS:TreeSerNum.Hi
		JNE	@@Close
		JMP	@@ExitFind
  @@ChkSerNum:	CMP	SI,DS:TreeSerNum.Lo
		JNE	@@Close
		CMP	DI,DS:TreeSerNum.Hi
		JNE	@@Close
		JMP	@@ExitFind
  @@Close:	MOV	AH,3Eh
		CALL	Intr21
		JNC	@@Next
		CMP	AX,53h
		JE	@@ErrFind
  @@Next:	MOV	AH,4Fh
		JMP	@@FindFile
  @@ErrFind:	STC
  @@ExitFind:	CALL	CloseFindFile
		PUSH	DS
		PUSH	CX
		PUSH	SS
		POP	DS
		LEA	SI,SaveDTA
		MOV	DI,OFFSET DTA
		MOV	CX,SIZE DTAs
		REP	MOVSB
		MOV	CX,SaveFindHandle
		MOV	ES:FindHandle,CX
		POP	CX
		POP	DS
		JNC	@@ReadHome
		CMP	AX,53h
		JNE	@@ReadRoot
		JMP	@@Error

  @@ReadHome:	PUSH	CX			; Read in home
		XOR	CX,CX
		XOR	DX,DX
		MOV	AX,4200h
		CALL	Intr21
		POP	CX
		JC	@@Error
		OR	AX,DX
		MOV	AX,1
		JNE	@@Error
		PUSH	CX
		MOV	CX,[BP]
		CALL	ReadTreeFile
		POP	BX
		JMP	@@EndRead

  @@ReadRoot:	MOV	AH,19h			; Read in root
		INT	21h
		ADD	AL,'A'
		CALL	Phantom
		MOV	AX,53h
		JC	@@Error
		PUSH	DS
		PUSH	ES
		PUSH	SS
		PUSH	CS
		POP	DS
		POP	ES
		MOV	SI,OFFSET TreeInfo_F
		MOV	DI,DX
		CALL	strcpy
		CALL	strcpy
		PUSH	ES
		POP	DS
		POP	ES
		MOV	AL,40h
		CALL	OpenFil
		POP	DS
		JC	@@OpenErr
		MOV	BX,AX
		MOV	CX,[BP]
		CALL	ReadTreeFile
		MOV	BX,0FFFFh
  @@EndRead:	JNC	@@TreeOk
		OR	AX,AX
		JE	@@ExitErr
  @@Error:	MOV	BX,0FFFDh
		JMP	@@ExitErr
  @@OpenErr:	CMP	AX,53h
		JE	@@Error
		MOV	BX,0FFFEh
  @@ExitErr:	STC
		JMP	@@Exit
  @@TreeOk:	MOV	DS:TreeLFN,0
		MOV	AH,19h
		INT	21h
		ADD	AL,'A'
		MOV	CL,AL
		CALL	Phantom
		MOV	AX,53h
		JC	@@Error
		MOV	AL,CL
		CALL	GetFS
		JC	@@Error
		OR	AL,AH
		MOV	DS:TreeLFN,AL
  @@Exit:	MOV	DS:TreeNumber,BX
		RET
ReadTreeInfo	ENDP

ReadTreeFile	PROC	NEAR			; Read TREEINFO
		XOR	DX,DX
		MOV	AH,3Fh
		CALL	Intr21
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	BX
		POP	SI
		JC	@@Exit
		MOV	AX,BX
		PUSH	SI
		POPF
		JC	@@Exit
		CMP	AX,CX			; Buffer overflow
		JAE	@@Overflow
		CMP	AX,SIZE DirTree+1	; Check header
		JB	@@Error
		PUSH	ES
		PUSH	CS
		POP	ES
		MOV	SI,OFFSET TreeHead
		MOV	DI,OFFSET TreeSign
		MOV	CX,9
		REPE	CMPSB
		POP	ES
		JNE	@@Error
		MOV	DI,AX
		MOV	CX,AX			; Check dir lengths
		DEC	CX
		DEC	CX
		XOR	AX,AX
		MOV	SI,OFFSET TreeList
  @@ChkDir:	CMP	SI,CX
		JA	@@Error
		JE	@@TreeOk
		MOV	BX,SI
		ADD	BX,SIZE DirEnt
		JC	@@Error
		ADD	BL,DS:[SI].DirShortLen
		ADC	BH,0
		JC	@@Error
		CMP	BYTE PTR DS:[BX-1],0
		JNE	@@Error
		ADD	BX,DS:[SI].DirWinLen
		JC	@@Error
		CMP	BYTE PTR DS:[BX-1],0
		JNE	@@Error
		ADD	BX,DS:[SI].DirOS2Len
		JC	@@Error
		CMP	BYTE PTR DS:[BX-1],0
		JNE	@@Error
		MOV	SI,BX
		INC	AX
		JMP	@@ChkDir
  @@Overflow:	MOV	AX,0FFFFh
		STC
		JMP	@@Exit
  @@TreeOk:	MOV	DS:TreeLen,AX
		MOV	BX,CX			; Check sum
		MOV	SI,OFFSET TreeCrLf
		SUB	CX,SI
		CALL	ChkSum
		CMP	AX,DS:[BX]
		JE	@@Exit
  @@Error:	XOR	AX,AX
		STC
  @@Exit:	RET
ReadTreeFile	ENDP

;---------------------------------------------------------------
;	Save TREEINFO
;		ES:DX - path
;		   DS - tree segment
;---------------------------------------------------------------
SaveTreeInfo	PROC	FAR
		LOCAL	TreeNameBuf:BYTE:512
		MOV	BX,DX
		MOV	AX,ES:[BX]
		CMP	AL,0
		JE	@@DelTree
		CMP	AH,':'
		JNE	@@DelTree
		CALL	UpCase
		SUB	AL,'A'
		MOV	DL,AL
		MOV	AH,0Eh
		INT	21h
  @@DelTree:	MOV	AX,DS:TreeNumber
		CMP	AX,0FFFDh
		JE	@@Exit
		CMP	DS:TreeLen,5
		JAE	@@SaveTree
		LEA	DX,TreeNameBuf		; Delete TREEINFO
		CMP	AX,0FFFEh
		JE	@@Exit
		JA	@@DelRoot
		PUSH	DS
		PUSH	ES
		PUSH	SS
		POP	ES
		POP	DS
		MOV	DI,DX
		MOV	SI,OFFSET VcSet
		PUSH	AX
		CALL	strcpy
		POP	AX
		MOV	SI,OFFSET TreeInfo_F+1
		CALL	MakeFileName
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	AL,ES:VcSet[0]
		CALL	Phantom
		MOV	AX,53h
		JC	@@ExDelHome
		MOV	AH,41h
		CALL	Intr21
  @@ExDelHome:	POP	DS
		JNC	@@DelRoot
		CMP	AX,53h
		JE	@@Exit
  @@DelRoot:	PUSH	DS
		PUSH	ES
		PUSH	SS
		PUSH	CS
		POP	DS
		POP	ES
		MOV	DI,DX
		MOV	SI,OFFSET TreeInfo_F
		CALL	strcpy
		CALL	strcpy
		PUSH	ES
		POP	DS
		POP	ES
		MOV	AH,19h
		CALL	Intr21
		ADD	AL,'A'
		CALL	Phantom
		MOV	AX,53h
		JC	@@ExDelRoot
		MOV	AH,41h
		CALL	Intr21
  @@ExDelRoot:	POP	DS
		JMP	@@Exit
  @@SaveTree:	MOV	DX,AX			; Save TREEINFO
		MOV	SI,OFFSET TreeLabel
		CALL	GetVol
		JC	@@Exit
		MOV	DS:TreeSerNum.Lo,BX
		MOV	DS:TreeSerNum.Hi,CX
		MOV	AL,DS:TreeLFN
		PUSH	AX
		PUSH	DS:TreeLenH
		MOV	SI,DS:TreeLen
		PUSH	SI
		CALL	TreeNumb2Adr1
		PUSH	SI
		PUSH	ES
		PUSH	SI
		PUSH	DS
		PUSH	DS
		PUSH	CS
		POP	DS
		POP	ES
		MOV	SI,OFFSET TreeSign
		XOR	DI,DI
		MOV	CX,9
		REP	MOVSB
		CALL	StrLen
		DEC	CX
		ADD	DI,CX
		SUB	CX,11
		NEG	CX
		MOV	AL,' '
		REP	STOSB
		MOV	AX,0A0Dh
		STOSW
		POP	DS
		POP	DI
		MOV	CX,DI
		MOV	SI,OFFSET TreeCrLf
		SUB	CX,SI
		CALL	ChkSum
		STOSW
		POP	ES
		MOV	BX,DX			; Save in home directory
		LEA	DX,TreeNameBuf
		CMP	BX,0FFFEh
		JA	@@SaveRoot
		JE	@@Default
		PUSH	DS
		PUSH	DI
		PUSH	ES
		PUSH	SS
		POP	ES
		POP	DS
		MOV	SI,OFFSET VcSet
		MOV	DI,DX
		CALL	strcpy
		MOV	SI,OFFSET TreeInfo_F+1
		MOV	AX,BX
		CALL	MakeFileName
		PUSH	DS
		POP	ES
		POP	DI
		POP	DS
		JMP	@@SaveHome
  @@Default:	CMP	ES:TreeInRoot,0
		JNZ	@@SaveRoot
		CMP	ES:TreeInHome,0
		JNZ	@@SaveHomNew
		JMP	@@ExitSave
  @@SaveRoot:	PUSH	ES			; Save in root
		PUSH	DS
		PUSH	DI
		PUSH	CS
		PUSH	SS
		POP	ES
		POP	DS
		MOV	SI,OFFSET TreeInfo_F
		MOV	DI,DX
		CALL	strcpy
		CALL	strcpy
		POP	DI
		POP	DS
		POP	ES
		MOV	BX,2
		CALL	SaveTreeFile
		MOV	BX,0FFFFh
		JNC	@@ExitSave
		CMP	AX,53h
		JE	@@Error
		OR	AX,AX
		JE	@@Error
		CMP	ES:TreeInHome,0
		JZ	@@Error
  @@SaveHomNew:	MOV	AL,ES:VcSet[0]		; Save new tree in home directory
		CALL	Phantom
		MOV	AX,53h
		JC	@@Error
		PUSH	DS
		PUSH	DI
		PUSH	ES
		PUSH	SS
		POP	ES
		POP	DS
		MOV	SI,OFFSET VcSet
		MOV	DI,DX
		CALL	strcpy
		MOV	SI,OFFSET TreeInfo_F+1
		XOR	AX,AX
		CALL	MakeFileName
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		XOR	BX,BX
  @@ChkFile:	MOV	AX,4300h
		CALL	Intr21
		JC	@@ExNewTree
		INC	BX
		MOV	SI,DI
		MOV	CX,3
  @@NextFile:	DEC	SI
		INC	BYTE PTR DS:[SI]
		CMP	BYTE PTR DS:[SI],'9'
		JBE	@@ChkFile
		MOV	BYTE PTR DS:[SI],'0'
		LOOP	@@NextFile
		MOV	AX,3
  @@ExNewTree:	POP	DI
		POP	DS
		CMP	AX,2
		JNE	@@Error
  @@SaveHome:	PUSH	BX
		XOR	BX,BX
		CALL	SaveTreeFile
		POP	BX
		JNC	@@ExitSave
  @@Error:	MOV	BX,0FFFDh
		CMP	AX,53h
		JE	@@ExitSave
		CALL	Beep
  @@ExitSave:	POP	DS:TreeSize
		POP	DS:TreeLen
		POP	DS:TreeLenH
		POP	AX
		MOV	DS:TreeLFN,AL
		MOV	DS:TreeNumber,BX
  @@Exit:	MOV	DL,ES:DosPath[0]
		SUB	DL,'A'
		MOV	AH,0Eh
		INT	21h
		RET
SaveTreeInfo	ENDP

SaveTreeFile	PROC	NEAR
		PUSH	DS
		PUSH	SS
		POP	DS
		MOV	AX,4300h
		CALL	Intr21
		JNC	@@OpenFile
		CMP	AX,2
		JNE	@@Error
		MOV	CX,BX
  @@OpenFile:	OR	CL,20h
		MOV	ES:BlocEr,1
		MOV	AH,3Ch
		CALL	Intr21
		MOV	ES:BlocEr,0
		JNC	@@WriteFile
		CMP	AX,53h
		JNE	@@Error
		MOV	AX,5
		JMP	@@Error
  @@WriteFile:	POP	DS
		MOV	BX,AX
		MOV	CX,DI
		XOR	DX,DX
		MOV	AH,40h
		CALL	Intr21
		PUSH	DS
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	DX
		POP	BX
		JC	@@Error
		MOV	AX,DX
		PUSH	BX
		POPF
		JC	@@Error
		CMP	DX,CX
		JE	@@Exit
		XOR	AX,AX
  @@Error:	STC
  @@Exit:	POP	DS
		RET
SaveTreeFile	ENDP

;---------------------------------------------------------------
;	Set tree cursor and first entry
;		   DH - number of lines
;---------------------------------------------------------------
SetTreeParms	PROC	FAR
		MOV	DS:TreeFirst,0
		MOV	DS:TreeCur,0
		MOV	DS:TreeOffset,0
		CALL	FindTreeCursor
		JNC	@@Found
		CALL	SetTreeCursor
		JC	@@Exit
  @@Found:	MOV	AL,DH
		CALL	SetTreeFirst
  @@Exit:	RET
SetTreeParms	ENDP

SetTreeCursor	PROC	FAR			; Find cursor position
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	SI,OFFSET DosShortPath
		MOV	DI,OFFSET CrnPath
		CALL	strcpy
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
SetTreeCursor	ENDP

FindTreeCursor	PROC	FAR
		MOV	AL,ES:DosShortPath[0]
		CMP	AL,DS:CrnPath[0]
		JNE	@@NotFound
		MOV	DI,OFFSET TmpBuf
		MOV	AH,':'
		STOSW
		MOV	AX,'\'
		STOSW
		PUSH	DS
		MOV	DS,ES:TreeSeg
		MOV	SI,OFFSET TreeList
		MOV	CX,DS:TreeLen
		POP	DS
  @@GetDir:	PUSH	SI
		PUSH	CX
		PUSH	DS
		MOV	DS,ES:TreeSeg
		MOV	AL,DS:[SI].DirLevel
		MOV	AH,0
		MOV	DI,AX
		ADD	DI,AX
		ADD	DI,AX
		MOV	WORD PTR ES:TmpErr[DI+1],SI
		MOV	CL,DS:[SI].DirAttr
		MOV	BYTE PTR ES:TmpErr[DI+0],CL
		MOV	CX,AX
		JCXZ	@@CmpPath
		MOV	DI,OFFSET TmpBuf
		MOV	AL,'\'
  @@FndSlash:	PUSH	CX
		MOV	CH,0FFh
		REPNE	SCASB
		POP	CX
		LOOP	@@FndSlash
		ADD	SI,OFFSET DirName
		CALL	strcpy
		MOV	AX,'\'
		STOSW
  @@CmpPath:	POP	DS
		PUSH	ES
		PUSH	DS
		POP	ES
		MOV	SI,OFFSET CrnPath
		MOV	DI,SI
		CALL	StrLen
		DEC	CX
		POP	ES
		MOV	DI,OFFSET TmpBuf
		REPE	CMPSB
		POP	CX
		POP	SI
		JNE	@@Next
		SCASB
		JE	@@Found
		SCASB
		JE	@@Found
  @@Next:	PUSH	DS
		MOV	DS,ES:TreeSeg
		CALL	NextDirEnt
		POP	DS
		LOOP	@@GetDir
  @@NotFound:	STC
		JMP	@@Exit
  @@Found:	PUSH	DS
		MOV	DS,ES:TreeSeg
		MOV	CL,DS:[SI].DirLevel
		MOV	CH,0
		INC	CX
		POP	DS
		MOV	SI,OFFSET TmpErr
		MOV	AL,0
		CMP	ES:HidDirs,0
		JNZ	@@SkipHidden
		MOV	AL,6
  @@SkipHidden:	TEST	AL,ES:[SI+0]
		JNZ	@@SetCursor
		ADD	SI,3
		LOOP	@@SkipHidden
  @@SetCursor:	MOV	SI,ES:[SI+1-3]
		CALL	TreeAdr2NumbH
		MOV	DS:TreeCur,SI
  @@Exit:	RET
FindTreeCursor	ENDP

SetTreeFirst	PROC	NEAR			; Set TreeFirst
		MOV	DI,DS:TreeCur
		MOV	AH,0
		MOV	CX,AX
		SHR	AX,1
		SUB	DI,AX
		JAE	@@ChkEnd
		XOR	DI,DI
  @@ChkEnd:	PUSH	DS
		MOV	DS,ES:TreeSeg
		MOV	SI,DS:TreeLenH
		POP	DS
		MOV	AX,SI
		SUB	SI,DI
		CMP	SI,CX
		JAE	@@Exit
		MOV	DI,AX
		SUB	DI,CX
		JAE	@@Exit
		XOR	DI,DI
  @@Exit:	MOV	DS:TreeFirst,DI
		RET
SetTreeFirst	ENDP

;---------------------------------------------------------------
;	Add current directory branch to tree
;---------------------------------------------------------------
UpdateBranch	PROC	FAR
		USES	DS
		CMP	ES:DosShortPath[3],0
		JE	@@Exit
		MOV	DS,ES:TreeSeg
		MOV	DX,OFFSET DosShortPath
		CALL	FindTreeEntry
		JNC	@@Exit
		PUSH	SI
		MOV	SI,OFFSET GlobalSegs_L
		MOV	AX,0FFF0h
		CALL	ExpandBlock
		MOV	BP,AX
		XOR	BX,BX
		MOV	SI,DI
  @@EndEntry:	LODS	BYTE PTR ES:[SI]
		CMP	AL,0
		JE	@@NameOS2
		CMP	AL,'\'
		JNE	@@EndEntry
		MOV	BX,SI
		DEC	BX
		MOV	BYTE PTR ES:[BX],0
  @@NameOS2:	MOV	CX,BX
		PUSH	DS
		PUSH	DI
		PUSH	ES
		POP	DS
		MOV	BX,OFFSET DosPath
		CMP	DS:OS2box,0
		JE	@@AddEntry
		MOV	SI,DX
		MOV	DI,OFFSET LineBuf1
		PUSH	DI
		CALL	strcpy
		POP	SI
		MOV	DI,OFFSET LineBuf2
		PUSH	BP
		LEA	BP,[DI+514]
		CALL	GetLongNameEA
		POP	BP
		JC	@@AddEntry
		OR	AX,AX
		JE	@@AddEntry
		MOV	BX,SI
		CALL	CutPath
		XCHG	SI,DI
		CALL	strcpy
  @@AddEntry:	POP	DI
		POP	DS
		POP	SI
		MOV	AL,DS:[SI].DirLevel
		INC	AX
		MOV	DS:TreeLev,AL
		PUSH	CX
		CALL	AddTreeEntry
		POP	BX
		PUSHF
		PUSH	DS
		PUSH	SI
		PUSH	ES
		POP	DS
		MOV	SI,DX
		MOV	DI,OFFSET LineBuf1
		CALL	strcpy
		POP	SI
		POP	DS
		OR	BX,BX
		JE	@@ChkChg
		MOV	BYTE PTR ES:[BX],'\'
  @@ChkChg:	POPF
		JNC	@@Compress
		MOV	DI,SI
		MOV	SI,DS:TreeLen
		CALL	TreeNumb2Adr1
		MOV	DS:TreeEnd,SI
		MOV	DS:TreeOldPoint,0
		MOV	DS:TreeCounter,0
		CALL	TreeFunc
		CMP	DS:TreeCounter,5
		JB	@@SaveTree
		CALL	ResWin
		CALL	Window
  @@SaveTree:	CALL	SaveTreeInfo
		MOV	SI,DS:TreeSize
		CALL	TreeAdr2NumbH
		MOV	DS:TreeLenH,SI
  @@Compress:	MOV	BX,DS:TreeSize
		INC	BX
		MOV	CL,4
		SHR	BX,CL
		INC	BX
		INC	BX
		MOV	CX,DS
		ADD	BX,CX
		MOV	SI,OFFSET GlobalSegs_L
		CALL	CompressBlock
  @@Exit:	RET
UpdateBranch	ENDP

;---------------------------------------------------------------
;	Tree Window
;		ES:SI - path
;		   BX - base
;		   DX - size
;	Return:	   CY - skip
;		   NC - selected, TmpBuf - directory
;---------------------------------------------------------------
ChDirFlag	EQU	BYTE PTR GroupMask[2]
ChDirOldCur	EQU	BYTE PTR GroupMask[3]

		.CONST
ChooseDir1_S	DB	' Choose Directory ',0
ChooseDir2_S	DB	'Reading directory tree...',0
		DB	'Speed search: ^r',0FFh,LenSearch,0

		.DATA
ChooseDir_P	DB	Conf_C, CnfRe_C, CnfBo_C, CnfUn_C
		DW	ChooseDir1_S
		DW	0FFFEh
		DW	0FFFFh
		DW	Empty_S
		DW	Empty_S
		DW	0
ChooseDir_B	BOXHD	< ChooseDir_P, 8200h, ChDirProc, Tree_K, Def_M, Tree_H, 0 >
		DB	0
ChooseDir_T	DW	0FFFEh, ChDirInit1
		DW	0FFFFh, ChDirInit2
		DW	4800h, ChDirUp
		DW	5000h, ChDirDown
		DW	0E08h, ChDirList
		DW	0008h, ChDirHidden
		DW	000Eh, ChDirLongNames
		DW	3C00h, ChDirRescan
		DW	0012h, ChDirRescan
		DW	0
ChDirKeys_T	DW	4E2Bh, ChDirPlus
		DW	4A2Dh, ChDirMinus
		DW	8400h, ChDirCtrlPgUp
		DW	7600h, ChDirCtrlPgDn
		DW	0

		.CODE	Subs
ChooseDir	PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,BP,DS
		MOV	AL,DL
		SHR	AL,1
		ADD	AL,BL
		DEC	AX
		MOV	CS:ChooseDir_B.BoxBase.X,AL
		MOV	DS,ES:WCB1seg
		CMP	DS:WinTyp,p_Tree
		JNE	@@LeftPan
		MOV	DS:ChgFlg,1
  @@LeftPan:	MOV	DS,ES:WCB2seg
		CMP	DS:WinTyp,p_Tree
		JNE	@@RightPan
		MOV	DS:ChgFlg,1
  @@RightPan:	PUSH	SI
		PUSH	BX
		XOR	BX,BX
		XCHG	BX,ES:TreeSeg
		MOV	SI,OFFSET AllSegs_L
		CALL	CompressBlock
		POP	BX
		POP	SI
		MOV	DS,ES:WorkBuffer
		MOV	BP,OFFSET ArcFiles
		CALL	AllocateBlock
		MOV	DS:WinTyp,p_Tree
		MOV	DS:LongNames,1
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	DI,OFFSET CrnPath
		PUSH	DI
		PUSH	SI
		CALL	strcpy
		POP	DI
		POP	SI
		MOV	AX,DS:[DI]
		CMP	AH,':'
		JNE	@@NormPath
		CALL	UpCase
		CMP	AL,DS:DosShortPath[0]
		JNE	@@Dialog
  @@NormPath:	PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		CALL	NormPath
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		JC	@@Dialog
		XCHG	SI,DI
		CALL	ShortName
  @@Dialog:	MOV	AX,ES
		PUSH	DS
		POP	ES
		XCHG	AX,DS:WCBcrn
		PUSH	AX
		MOV	AX,DX
		SUB	AX,80Ch
		MOV	DS:ListWindow,AX
		MOV	DS:ListBase,205h
		MOV	DS:ListTotal,0
		MOV	DS:ListLine,OFFSET ChDirLine
		MOV	DS:ListHotKey,OFFSET ChDirHotKey
		MOV	DS:ChDirFlag,0
		MOV	BP,OFFSET ChooseDir_B
		CALL	Dialog
		POP	AX
		XCHG	AX,DS:WCBcrn
		MOV	DS:WorkBuffer,AX
		MOV	DS:TreeSeg,0
		STC
		JNZ	@@Exit
		MOV	DS,AX
		MOV	SI,OFFSET CrnPath
		MOV	DI,OFFSET TmpBuf
		CALL	strcpy
		CLC
  @@Exit:	RET
ChooseDir	ENDP

ChDirProc	PROC	NEAR
		CMP	AH,0FCh
		JE	@@Mouse
		MOV	SI,OFFSET ChooseDir_T
		CALL	Case
ChDirList:	CALL	DialogList
		JMP	@@Exit
  @@Mouse:	CALL	GetMous			; Mouse
		JNZ	@@Button1
		XOR	AX,AX
		JMP	ChDirList
  @@Button1:	MOV	AH,0FCh
		CMP	AL,1
		JNE	ChDirList
		PUSH	DX
		PUSH	BX
		ADD	BH,DH
		SUB	BH,4
		ADD	BL,19
		MOV	SI,BX
		MOV	DX,100h+LenSearch+1
		CALL	InWind
		POP	BX
		POP	DX
		JC	ChDirList
		PUSH	CX
		MOV	DI,OFFSET SearchBuf0
		CALL	StrLen
		DEC	CX
		POP	DI
		SUB	DI,SI
		CMP	CX,DI
		JBE	@@SetPos
		MOV	CX,DI
  @@SetPos:	MOV	AX,CX
		XCHG	CX,DS:SearchCur0
		CMP	AX,CX
		JE	@@LeaveTime
		ADD	AX,SI
		CALL	Cursor
		JMP	@@Mouse
  @@LeaveTime:	CALL	Yield
		JMP	@@Mouse
ChDirInit1:	CALL	ChDirStatInit		; Init
		JMP	@@Exit
ChDirUp:	CMP	DS:ListCursor,0		; Up
		JNE	ChDirList
		JMP	@@Clear
ChDirDown:	MOV	CX,DS:ListCursor	; Down
		INC	CX
		CMP	CX,DS:ListTotal
		JB	ChDirList
		JMP	@@Clear
ChDirLongNames:	PUSH	DS			; ^N - long names
		MOV	DS,DS:WCBcrn
		PUSH	BX
		MOV	BX,OFFSET LongNames
		CALL	InvPanelVar
		POP	BX
		JMP	@@RefreshWin
ChDirHidden:	PUSH	DS			; ^H - hidden dirs
		MOV	DS,DS:WCBcrn
		CALL	F86_10
		PUSH	ES
		PUSH	DX
		PUSH	BX
		CALL	ChDirClearWin
		POP	BX
		POP	DX
		POP	ES
		JMP	@@SetParms
ChDirRescan:	CALL	HidCur			; ^R, F2 - rescan
		PUSH	ES
		PUSH	DX
		PUSH	BX
		CALL	ChDirClearWin
		INC	BH
		CALL	BasAdr
		MOV	CL,DL
		MOV	AL,' '
		REP	STOSW
		POP	BX
		POP	DX
		POP	ES
		CALL	ChDirStatInit
		PUSH	DX
		PUSH	BX
		ADD	BX,205h
		SUB	DX,50Ah
		CALL	Window
		POP	BX
		POP	DX
		MOV	DS:RescanTree,1
ChDirInit2:	MOV	DS:SearchCur0,0		; Read tree
		MOV	DS:SearchBuf0,0
		PUSH	DS
		MOV	DS,DS:WCBcrn
		CALL	ScanTree
  @@SetParms:	PUSH	DX
		MOV	DH,ES:ListWindow.Y
		CALL	SetTreeParms
		POP	DX
  @@RefreshWin:	MOV	AX,8000h
		XCHG	AX,DS:TreeCur
		MOV	ES:ListCursor,AX
		MOV	AX,8000h
		XCHG	AX,DS:TreeFirst
		MOV	ES:ListFirst,AX
		MOV	DS,ES:TreeSeg
		MOV	AX,DS:TreeLenH
		POP	DS
		MOV	DS:ListTotal,AX
		MOV	AX,0FFFEh
		PUSH	BX
		CALL	DialogList
		POP	BX
		MOV	AX,0FFFFh
		CALL	DialogList
  @@Clear:	XOR	AX,AX
  @@Exit:	RET
ChDirProc	ENDP

ChDirStatInit	PROC	NEAR			; Init status lines
		USES	BX,DS
		PUSH	CS
		POP	DS
		MOV	BP,OFFSET ChooseDir_P
		MOV	SI,OFFSET ChooseDir2_S
		ADD	BH,DH
		SUB	BH,5
		ADD	BL,5
		MOV	AL,DL
		CALL	NgLine
		INC	BH
		CALL	NgLine
		RET
ChDirStatInit	ENDP

ChDirClearWin	PROC	NEAR			; Clear window
		ADD	BX,205h
		SUB	DX,80Bh
		MOV	CL,DH
		MOV	CH,0
		MOV	AH,CS:ChooseDir_P[0]
		CALL	Color
		MOV	DH,ES:Borders[7]
  @@ClearLine:	PUSH	CX
		CALL	BasAdr
		MOV	CL,DL
		MOV	AL,' '
		REP	STOSW
		INC	BH
		MOV	AL,DH
		STOSW
		POP	CX
		LOOP	@@ClearLine
		RET
ChDirClearWin	ENDP

ChDirSearch	PROC	NEAR			; Output search line
		USES	AX,BX,CX,DX,SI,BP
		ADD	BH,DH
		SUB	BH,4
		ADD	BL,19
		MOV	CL,LenSearch
		MOV	SI,OFFSET SearchBuf0
		MOV	AH,CS:[BP+1]
		XOR	BP,BP
		CALL	InpStrOut
		MOV	DX,100h+LenSearch
		CALL	Window
		MOV	AX,BX
		ADD	AX,DS:SearchCur0
		CALL	Cursor
		RET
ChDirSearch	ENDP

ChDirLine	PROC	NEAR			; Output tree line
		USES	CX,DS
		MOV	AX,DS:ListCursor
		MOV	CX,DS:ListFirst
		MOV	DS,DS:WCBcrn
		XCHG	AX,DS:TreeCur
		XCHG	CX,DS:TreeFirst
		PUSH	SI
		MOV	SI,AX
		SUB	SI,CX
		SUB	AX,DS:TreeCur
		JE	@@ChgPos
		PUSH	DS
		PUSH	ES
		POP	DS
		CMP	DS:ChDirFlag,0
		JNE	@@Leave
		MOV	DS:SearchCur0,0
		MOV	DS:SearchBuf0,0
		CALL	ChDirSearch
  @@Leave:	MOV	DS:ChDirFlag,0
		POP	DS
  @@ChgPos:	ADD	BX,ES:ListBase
		MOV	DX,ES:ListWindow
		SUB	CX,DS:TreeFirst
		OR	AX,CX
		MOV	AX,SI
		POP	SI
		JE	@@SetCursor
		MOV	ES:ChDirOldCur,AL
		PUSH	DX
		PUSH	BX
		SUB	BX,101h
		ADD	DX,402h
		MOV	AL,DS:TreeOffset
		CALL	PutTree
		ADD	BH,DH
		SUB	BH,2
		MOV	DH,1
		CALL	Window
		POP	BX
		POP	DX
		CMP	AL,DS:TreeOffset
		JE	@@SetShift
		MOV	CX,8000h
  @@SetShift:	MOV	ES:ListShift,CX
  @@ChkOut:	JCXZ	@@Line
		STC
		JMP	@@Exit
  @@SetCursor:	CMP	BP,OFFSET ChooseDir_P
		JE	@@Exit
		MOV	CX,ES:ListShift
		CMP	CX,1
		JE	@@Scroll
		CMP	CX,-1
		JNE	@@ChkOut
  @@Scroll:	PUSH	BX
		ADD	BH,ES:ChDirOldCur
		ADD	BH,CL
		MOV	DH,1
		CALL	Window
		POP	BX
  @@Line:	MOV	AX,SI
		SUB	AX,DI
		ADD	BH,AL
		MOV	DH,1
		CLC
  @@Exit:	RET
ChDirLine	ENDP

ChDirHotKey	PROC	NEAR			; Hot keys
		MOV	SI,OFFSET ChDirKeys_T
		CALL	Case
		PUSH	DS			; Letters
		PUSH	BP
		PUSH	DX
		PUSH	BX
		PUSH	AX
		MOV	SI,DS:ListCursor
		CALL	F77_30
		CALL	SpeedSearch
		POP	AX
		POP	BX
		POP	DX
		POP	BP
		POP	DS
		PUSHF
		CALL	ChDirSearch
		POPF
		JNZ	@@Found
		CMP	AX,1C0Ah
		JNE	@@NotFound
		XOR	AX,AX
		JMP	@@NotFound
  @@Found:	CALL	TreeAdr2NumbH
		MOV	DS:ChDirFlag,1
		JMP	@@SetCur
ChDirPlus:	PUSH	DX			; Grey +
		MOV	DX,DS:ListTotal
		MOV	SI,DS:ListCursor
		CALL	F10_30
		POP	DX
		JMP	@@SetCur
ChDirMinus:	MOV	AL,0			; Grey -
		JMP	@@MoveLevel
ChDirCtrlPgUp:	MOV	AL,0FFh			; ^PgUp
  @@MoveLevel:	MOV	SI,DS:ListCursor
		CALL	F10_35
  @@SetCur:	CLC
		JMP	@@Exit
ChDirCtrlPgDn:	MOV	AX,1C0Dh		; ^PgDn
  @@NotFound:	STC
  @@Exit:	RET
ChDirHotKey	ENDP
