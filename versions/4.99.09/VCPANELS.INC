;---------------------------------------------------------------
;		Subroutines
;
;		Version:	13.07.2000
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Read directory with save
;		    DS - WCBseg
;		AL = 0 - Without restore selection
;---------------------------------------------------------------
EAPathOffset	EQU	WORD PTR LineBuf1[0]

		.CONST
Mask_S		DB	'\*.*',0
Ext_F		DB	'vc.ext',0

		.DATA
WCBsegs_L	DW	WCB1seg, WCB2seg, WCBcrn, WCBsec, 0

		.CODE	Subs
ReadDir		PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,ES
		PUSH	BP
		MOV	DS:ChgFlg,1
		CMP	DS:WinTyp,0
		JNE	ReadTree
		MOV	BX,DS:EndBuf		; Compress old WCB
		DEC	BX
		MOV	CL,4
		SHR	BX,CL
		INC	BX
		MOV	CX,DS
		ADD	BX,CX
		MOV	SI,OFFSET GlobalSegs_L
		CALL	CompressBlock
		PUSH	DS			; Create new WCB
		PUSH	ES
		MOV	BP,DS
		MOV	ES,ES:WorkBuffer
		MOV	CX,DS:DiskBuf
		XOR	SI,SI
		XOR	DI,DI
		REP	MOVSB
		PUSH	ES
		POP	DS
		POP	ES
		MOV	SI,OFFSET WCBsegs_L
  @@CorAddr:	MOV	BX,CS:[SI]
		OR	BX,BX
		JE	@@SetLimit1
		INC	SI
		INC	SI
		CMP	ES:[BX],BP
		JNE	@@CorAddr
		MOV	ES:[BX],DS
		JMP	@@CorAddr
  @@SetLimit1:	MOV	BP,ES			; Set limit
		ADD	BP,ES:SizeDataSeg
		MOV	CX,DS
		SUB	BP,CX
		SUB	BP,MinView/16/2
		CMP	BP,0FFFh
		JBE	@@SetLimit2
		MOV	BP,0FFFh
  @@SetLimit2:	MOV	CL,4
		SHL	BP,CL

		MOV	AH,0
  @@SetPath:	PUSH	AX
		PUSH	ES			; Create .. entry
		CALL	SetParentDir
		POP	ES
		CMP	BP,DI
		JAE	@@ChkSerial
		MOV	BP,DI

  @@ChkSerial:	MOV	AL,DS:WinShortPath[0]	; Check serial number
		CALL	Phantom
		JC	@@SerialErr
		SUB	AL,'A'-1
		MOV	BL,AL
		MOV	DX,OFFSET WinLongPath
		MOV	WORD PTR DS:WinLongPath[0],0
		MOV	CX,866h
		MOV	AX,440Dh
		CALL	Intr21
		MOV	CX,DS:WinLongPath[2].Lo
		MOV	DX,DS:WinLongPath[2].Hi
		JNC	@@CmpSerNum
		CMP	AX,53h
		JE	@@SerialErr
		CMP	AX,15h
		JE	@@SerialErr
		XOR	CX,CX
		XOR	DX,DX
  @@CmpSerNum:	MOV	AX,CX
		MOV	BX,DX
		XCHG	AX,DS:SerNumb.Lo
		XCHG	BX,DS:SerNumb.Hi
		XOR	CX,AX
		XOR	DX,BX
		MOV	AL,0
		XCHG	AL,DS:SerNumErr
		CMP	AL,0
		JNE	@@ChkArc
		OR	CX,DX
		JE	@@ChkArc
		POP	AX
		MOV	AL,0
		PUSH	AX

  @@ChkArc:	CMP	DS:ArcPnt,0
		JE	@@AddMask
		MOV	WORD PTR DS:ArcExt,'?'	; Test archive
		CALL	DirTwoNames
		JC	@@ArcPathEr1
		MOV	DX,OFFSET LineBuf2	; Check main archive
		MOV	SI,OFFSET WinShortPath
		MOV	DI,DX
		CALL	MvzLine
		CALL	AddFile
		MOV	SI,OFFSET ArcFiles+2
		CALL	MvzLine
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	AX,4300h
		CALL	Intr21
		POP	DS
		JC	@@ArcPathEr1
		MOV	SI,OFFSET WinShortPath
		MOV	DI,OFFSET DosShortPath
		CALL	CompNormLines
		MOV	DI,OFFSET LineBuf2
		PUSH	DI
		MOV	AL,1
		JNE	@@ArcPathNam
		MOV	AL,0
  @@ArcPathNam:	CALL	PutArcPathName1
		POP	DI
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,DI
		CALL	TestArchive
		POP	DS
		JNC	@@ArcFound
		CMP	AX,53h
		JE	@@ArcPathErr
		OR	AX,AX
		JNE	@@NoArchive
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	DS:DTA.FilSize.Lo,1
		CALL	DetectArc
		POP	DS
		JNC	@@ArcFound
  @@ArcPathEr1:	CMP	AX,53h
		JNE	@@NoArchive
  @@ArcPathErr:	MOV	DI,DS:DiskBuf
		ADD	DI,SIZE FilEnt+3
		JMP	@@PathErr

  @@NoArchive:	CALL	DelAllTempArcs		; Error in archive
		JC	@@PathErr
		POP	AX
		MOV	AL,0
		XOR	DI,DI
		XCHG	DS:ArcPnt,DI
		MOV	DS:DiskBuf,DI
		MOV	DS:ArcPath,AL
		MOV	SI,OFFSET ArcFiles
		CMP	DI,SI
		JBE	@@SetPath
		PUSH	AX
  @@UpArc1:	MOV	BX,SI
		LODSW
  @@UpArc2:	LODSB
		CMP	AL,0
		JNE	@@UpArc2
		CMP	SI,DI
		JB	@@UpArc1
		MOV	DS:ArcPnt,BX
		POP	AX
		JMP	@@SetPath

  @@ArcFound:	PUSH	DS			; Set extension
		PUSH	ES
		POP	DS
		POP	ES
		MOV	SI,OFFSET ArchiveExt
		MOV	DI,OFFSET ArcExt
		CALL	MvzLine
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	DI,DS:DiskBuf		; Read archive directory
		CMP	DS:ArcPath,0
		JNE	@@ChkEmpty
		MOV	AX,ES:DTA.FilDate
		MOV	DS:[DI].DateF,AX
		MOV	AX,ES:DTA.FilTime
		MOV	DS:[DI].TimeF,AX
  @@ChkEmpty:	ADD	DI,SIZE FilEnt+3
		MOV	AX,ES:DTA.FilSize.Lo
		OR	AX,ES:DTA.FilSize.Hi
		JE	@@SearchOk
		MOV	AX,OFFSET ExecArcSaveEnt
		CALL	ReadArcDir
		JNC	@@SearchOk
		OR	AH,AH
		JNE	@@ErrMsg
		CMP	AL,0
		JE	@@NoArchive
		CMP	AL,53h
		JE	@@PathErr
		JMP	@@SearchOk
  @@ErrMsg:	PUSH	BP
		MOV	BP,AX
		CALL	Dialog
		POP	BP
		JMP	@@SearchOk

  @@AddMask:	MOV	SI,OFFSET WinShortPath	; Path to search
		MOV	DI,OFFSET DosShortPath
		CALL	CompNormLines
		MOV	DI,OFFSET TmpBuf
		MOV	DX,DI
		JE	@@Mask
		CALL	MvzLine
		CALL	AddFile
  @@Mask:	PUSH	DS
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET Mask_S+1
		MOV	CX,4
		REP	MOVSB
		POP	DS
		XOR	BX,BX
		MOV	DI,DS:DiskBuf
		CMP	DS:WinShortPath[3],BL
		JE	@@FindFirst
		ADD	DI,SIZE FilEnt+3
		INC	BX
  @@FindFirst:	CALL	ChgDisk			; Check phantom drive
		JC	@@PathErr
		CALL	DirTwoNames		; Find first file
		JC	@@FindError
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	CX,37h
		MOV	AH,4Eh
		CALL	Intr21
		JNC	@@PathLength
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		CMP	AX,2
		JNE	@@FindError
		MOV	AX,12h
  @@FindError:	CMP	AX,53h
		JE	@@PathErr
		CMP	AX,3
		JA	@@SearchOk

		MOV	AX,DS			; Set new path
		CMP	AX,ES:WCBcrn
		JE	@@PathErr
		MOV	SI,OFFSET WinShortPath
		CALL	CutPath
		MOV	DL,DS:WinShortPath[0]
		SUB	DL,'A'-1
		CMP	BYTE PTR DS:[SI],0
		JE	@@NextDrive
		CMP	SI,OFFSET WinShortPath+3
		JBE	@@CutPath
		DEC	SI
  @@CutPath:	MOV	BYTE PTR DS:[SI],0
		POP	AX
		MOV	AL,0
		JMP	@@SetPath
  @@NextDrive:	DEC	DL
		JE	@@PathErr
		CMP	DL,2
		JNE	@@ChkPath
		CMP	ES:DriveB,0
		JNE	@@ChkPath
		MOV	AL,DL
		ADD	AL,'A'-1
		CALL	TstDrv
		JC	@@ChkPath
		CMP	AL,Logical
		JNE	@@ChkPath
		DEC	DX
  @@ChkPath:	MOV	AL,DL
		ADD	AL,'A'-1
		CALL	Phantom
		JC	@@PathErr
		MOV	SI,OFFSET WinShortPath+3
		MOV	AH,47h
		CALL	Intr21
		JNC	@@NewDrive
		CMP	AX,0Fh
		JE	@@NextDrive
		CMP	AX,53h
		JE	@@NextDrive
		JMP	@@PathErr
  @@NewDrive:	ADD	DL,'A'-1
		MOV	DS:WinShortPath[0],DL
		POP	AX
		MOV	AL,0
		JMP	@@SetPath
  @@SerialErr:	MOV	DS:SerNumErr,1
  @@PathErr:	POP	AX
		MOV	AX,100h
		PUSH	AX
		JMP	@@SearchErr
  @@PathLength:	MOV	SI,OFFSET TmpBuf
		CALL	CutPath
		MOV	DS:EAPathOffset,SI

  @@SaveEntry:	MOV	DS:BlocEr,0		; Save file entry
		MOV	SI,OFFSET DTA.FilName
		TEST	DS:DTA.FilAttr,10h
		JZ	@@SaveName
		CALL	ChkUpDir
		JNE	@@SaveName
		CMP	ES:WinShortPath[3],0
		JE	@@FindNext
		PUSH	DI
		AND	DS:DTA.FilAttr,0F9h
		MOV	DI,ES:DiskBuf
		JMP	@@SaveAttrib
  @@Overflow1:	POP	AX
  @@Overflow2:	MOV	DI,CX
		CALL	Beep
		CLC
		JMP	@@ExitSearch
  @@SaveName:	MOV	CX,DI
		ADD	DI,OFFSET NameF
		JC	@@Overflow2
  @@GetChar:	LODSB
		CMP	DI,BP
		JAE	@@Overflow2
		STOSB
		CMP	AL,0
		JNE	@@GetChar
		MOV	AX,DI
		SUB	AX,CX
		SUB	AX,OFFSET NameF
		MOV	SI,CX
		MOV	ES:[SI].LenShortNamF,AX
		MOV	SI,OFFSET FindData.WinLongName
		CMP	BYTE PTR DS:[SI],0
		JNE	@@LongName
		XOR	SI,SI
		CMP	DS:OS2Box,0
		JE	@@SkipLong
		CMP	ES:LongNames,0
		JE	@@SkipLong
		PUSH	ES
		PUSH	DI
		PUSH	DS
		POP	ES
		MOV	SI,OFFSET DTA.FilName
		MOV	DI,DS:EAPathOffset
		CALL	MvzLine
		POP	DI
		POP	ES
		MOV	SI,OFFSET TmpBuf
		CALL	GetLongNameEA
		MOV	SI,0
		JC	@@SkipLong
		MOV	SI,AX
		ADD	DI,AX
		JMP	@@SkipLong
  @@LongName:	PUSH	SI
  @@SaveLong:	LODSB
		CMP	DI,BP
		JAE	@@Overflow1
		STOSB
		CMP	AL,0
		JNE	@@SaveLong
		POP	AX
		SUB	SI,AX
  @@SkipLong:	PUSH	DI
		MOV	DI,CX
		MOV	ES:[DI].LenLongNamF,SI
		MOV	ES:[DI].NumbF,BX
		INC	BX
  @@SaveAttrib:	MOV	ES:[DI].DescrF,0
		ADD	DI,OFFSET AttrF
		MOV	SI,OFFSET DTA.FilAttr
		AND	BYTE PTR DS:[SI],3Fh
		MOV	CX,9
		REP	MOVSB
		POP	DI
  @@FindNext:	MOV	AH,4Fh			; Find next file
		CALL	Intr21
		JNC	@@SaveEntry

  @@ExitSearch:	CALL	CloseFindFile		; Exit search
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		JNC	@@SearchOk
		CMP	AX,53h
		JE	@@SearchErr
  @@SearchOk:	MOV	DS:ChgFlg,0
  @@SearchErr:	MOV	DS:EndFul,DI
		MOV	DS:EndBuf,DI
		XOR	AX,AX
		MOV	DS:FileCur,AX
		MOV	DS:FileFirst,AX
		MOV	ES:BlocEr,AL

		MOV	DS:ExtFile,0		; Read local extension file
		CMP	DS:ArcPnt,0
		JNE	@@Sort
		CMP	DS:ChgFlg,0
		JNZ	@@Sort
		MOV	AX,DI
		INC	AX
		CMP	AX,BP
		JAE	@@DirSizes
		PUSH	DS
		PUSH	DI
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET TmpBuf
		MOV	SI,DX
		CALL	CutPath
		PUSH	CS
		POP	DS
		MOV	DI,SI
		MOV	SI,OFFSET Ext_F
		CALL	MvzLine
		PUSH	ES
		POP	DS
		POP	DI
		MOV	AL,40h
		CALL	OpenFil
		POP	DS
		JC	@@ErrExt
		MOV	BX,AX
		MOV	DX,DI
		MOV	CX,BP
		SUB	CX,DX
		MOV	AH,3Fh
		CALL	Intr21
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	BX
		POP	SI
		JC	@@ErrExt
		PUSH	SI
		POPF
		JC	@@ErrExt
		MOV	DS:ExtFile,DI
		ADD	DI,BX
		MOV	BYTE PTR DS:[DI],1Ah
		INC	DI
		JMP	@@ReadDescr
  @@ErrExt:	CMP	AL,53h
		JNE	@@ReadDescr
		MOV	DS:ChgFlg,1
		JMP	@@Sort

DescrEnd	EQU	SizeWCB

  @@ReadDescr:	MOV	AX,DI			; Read description
		INC	AX
		INC	AX
		CMP	AX,BP
		JAE	@@DirSizes
		MOV	DX,OFFSET TmpBuf
		PUSH	DI
		CALL	ReadDescription
		POP	DX
		JC	@@ErrDescr
		CMP	DX,DI
		JAE	@@ParseDescr
		CMP	BYTE PTR DS:[DI-1],1Ah
		JNE	@@ParseDescr
		DEC	DI
  @@ParseDescr:	MOV	DS:DescrEnd,DI
		MOV	BYTE PTR DS:[DI],4
		INC	DI
		PUSH	ES
		PUSH	DI
		PUSH	DS
		POP	ES
		MOV	SI,DX
  @@ChkBegDesc:	CMP	SI,DS:DescrEnd
		JAE	@@EndDescr
		MOV	DI,SI
		LODSB
		CMP	AL,13
		JE	@@ChkBegDesc
		CMP	AL,10
		JE	@@ChkBegDesc
		CMP	AL,' '
		JE	@@SkipLine
		CMP	AL,9
		JE	@@SkipLine
		DEC	SI
		MOV	AH,0
		CMP	BYTE PTR DS:[SI],'"'
		JNE	@@FnEndName
		MOV	AH,1
		INC	SI
  @@FnEndName:	LODSB
		CMP	AL,13
		JE	@@ChkNoQuote
		CMP	AL,10
		JE	@@ChkNoQuote
		CMP	SI,DS:DescrEnd
		JA	@@ChkNoQuote
		OR	AH,AH
		JNE	@@ChkQuote
		CMP	AL,' '
		JE	@@EndNamDesc
		CMP	AL,9
		JE	@@EndNamDesc
		JMP	@@FnEndName
  @@ChkNoQuote:	OR	AH,AH
		JE	@@EndNamDesc
		DEC	SI
		JMP	@@SkipLine
  @@ChkQuote:	CMP	AL,'"'
		JNE	@@FnEndName
		INC	DI
  @@EndNamDesc:	DEC	SI
		PUSH	SI
		PUSH	AX
		XOR	AX,AX
		MOV	DS:[SI],AL
		CALL	FindFile
		MOV	BX,SI
		POP	AX
		POP	SI
		MOV	DS:[SI],AL
		JC	@@Absent
		CMP	DS:[BX].DescrF,0
		JNE	@@Absent
		OR	AH,AH
		JE	@@SkipSpace
		INC	SI
		JMP	@@SkipSpace
  @@Absent:	MOV	BX,SI
		SUB	BX,DI
		CMP	BX,2
		JNE	@@EndPoint
		CMP	WORD PTR DS:[DI],'..'
		JE	@@SaveAbsent
  @@EndPoint:	CMP	BYTE PTR DS:[SI-1],'.'
		JE	@@SkipLine
		PUSH	DI
		PUSH	SI
		MOV	AL,0
		XCHG	AL,DS:[SI]
		MOV	SI,DI
		OR	AH,AH
		JNE	@@NameSymb1
		MOV	DI,OFFSET FileSymb2_S
		CALL	ChkForSymb
		JC	@@NameSymb2
  @@NameSymb1:	MOV	DI,OFFSET FileSymb1_S
		CALL	ChkForSymb
  @@NameSymb2:	POP	SI
		POP	DI
		MOV	DS:[SI],AL
		JC	@@SkipLine
  @@SaveAbsent:	ADD	BX,SIZE FilEnt+1
		MOV	AX,SI
		ADD	AX,BX
		JC	@@SkipLine
		CMP	AX,BP
		JAE	@@SkipLine
		POP	CX
		ADD	BX,CX
		JC	@@CutDescr
		CMP	BX,BP
		JBE	@@MoveDescr
  @@CutDescr:	MOV	AX,BX
		SUB	AX,BP
		SUB	CX,AX
		MOV	BX,CX
		MOV	BYTE PTR DS:[BX-1],4
		MOV	BX,BP
  @@MoveDescr:	PUSH	BX
		PUSH	SI
		PUSH	DI
		MOV	SI,CX
		MOV	DI,BX
		SUB	CX,DS:ExtFile
		CMP	DS:ExtFile,0
		JNE	@@InsEntry
		SUB	CX,DX
  @@InsEntry:	DEC	SI
		DEC	DI
		MOV	DS:DescrEnd,DI
		STD
		REP	MOVSB
		CLD
		MOV	CX,DI
		SUB	CX,SI
		MOV	SI,DS:DiskBuf
		CMP	DS:ExtFile,0
		JE	@@CorIdx
		ADD	DS:ExtFile,CX
  @@CorIdx:	CMP	SI,DS:EndBuf
		JAE	@@AddEntry
		CMP	DS:[SI].DescrF,0
		JE	@@NextIdx
		ADD	DS:[SI].DescrF,CX
  @@NextIdx:	CALL	NextFilEnt
		JMP	@@CorIdx
  @@AddEntry:	MOV	DI,SI
		POP	SI
		POP	BX
		ADD	SI,CX
		ADD	BX,CX
		ADD	DX,CX
		PUSH	BX
		PUSH	DI
		MOV	AL,0
		REP	STOSB
		MOV	DS:EndBuf,DI
		MOV	DS:EndFul,DI
		POP	DI
		MOV	DS:[DI].AttrF,28h
		MOV	CX,BX
		SUB	CX,SI
		INC	CX
		MOV	DS:[DI].LenShortNamF,CX
		MOV	DS:[DI].LenLongNamF,0
		DEC	CX
		CMP	DI,DS:DiskBuf
		JBE	@@SetNewName
		PUSH	SI
		MOV	SI,DI
		CALL	Addr2Numb
		DEC	SI
		CALL	Numb2Addr
		MOV	AX,DS:[SI].NumbF
		INC	AX
		MOV	DS:[DI].NumbF,AX
		POP	SI
  @@SetNewName:	PUSH	DS
		MOV	DS,CS:DataSegment
		MOV	AH,DS:ConvCase
		POP	DS
		PUSH	DI
		ADD	DI,OFFSET NameF
  @@NewName:	LODSB
		OR	AH,AH
		JE	@@SaveChar
		CALL	LowCase
  @@SaveChar:	STOSB
		LOOP	@@NewName
		POP	BX
		POP	SI
		LODSB
		CMP	AL,'"'
		JE	@@SkipSpace
		DEC	SI
  @@SkipSpace:	LODSB
		CMP	AL,' '
		JE	@@SkipSpace
		CMP	AL,9
		JE	@@SkipSpace
		DEC	SI
		MOV	DS:[BX].DescrF,SI
  @@SkipLine:	CMP	SI,DS:DescrEnd
		JAE	@@EndDescr
		LODSB
		CMP	AL,13
		JE	@@ChkBegDesc
		CMP	AL,10
		JNE	@@SkipLine
		JMP	@@ChkBegDesc
  @@EndDescr:	POP	DI
		POP	ES
		JMP	@@DirSizes
  @@ErrDescr:	CMP	AX,53h
		JNE	@@DirSizes
		MOV	DS:ChgFlg,1
		JMP	@@Sort

  @@DirSizes:	CMP	ES:AutoSiz,0		; Scan directory sizes
		JZ	@@Sort
		MOV	AL,0
		CALL	DirSize

  @@Sort:	MOV	AX,DI			; Sort
		DEC	AX
		OR	AX,0Fh
		INC	AX
		MOV	CL,4
		SHR	AX,CL
		MOV	CX,DS
		ADD	AX,CX
		MOV	ES:WorkBuffer,AX
		CALL	FileSort
		MOV	ES:WorkBuffer,DS
		MOV	AX,DS:EndFul
		ADD	AX,SIZE FilEnt+256
		JC	@@MaxWCB
		CMP	AX,DI
		JBE	@@RestSel
		MOV	DI,AX
		CMP	DI,BP
		JBE	@@RestSel
  @@MaxWCB:	MOV	DI,BP
  @@RestSel:	POP	AX
		POP	BP

		PUSH	ES			; Restore selection
		PUSH	DI
		MOV	ES,BP
		MOV	BX,ES:EndBuf
		MOV	DI,ES:DiskBuf
  @@ChkEndOld:	CMP	DI,BX
		JAE	@@RestCursor
		MOV	AH,ES:[DI].AttrF
		AND	AH,0C0h
		JZ	@@NextOld
		CMP	AL,0
		JNZ	@@FindNew
		AND	AH,80h
		JZ	@@NextOld
  @@FindNew:	CALL	FindFileEntry
		JC	@@NextOld
		OR	DS:[SI].AttrF,AH
  @@NextOld:	PUSH	AX
		MOV	AX,ES:[DI].LenShortNamF
		ADD	DI,ES:[DI].LenLongNamF
		ADD	DI,SIZE FilEnt
		ADD	DI,AX
		POP	AX
		JMP	@@ChkEndOld

  @@RestCursor:	CMP	AL,0			; Restore cursor, first file
		JZ	@@DelOldWCB
		MOV	SI,ES:FileCur
		CALL	FindFileNear
		JC	@@RestFirst
		MOV	DS:FileCur,SI
  @@RestFirst:	MOV	SI,ES:FileFirst
		CALL	FindFileNear
		JC	@@DelOldWCB
		MOV	DS:FileFirst,SI
  @@DelOldWCB:	POP	DI
		POP	ES

		DEC	DI			; Delete old WCB
		OR	DI,0Fh
		INC	DI
		MOV	DS:SizeWCB,DI
		MOV	CL,4
		SHR	DI,CL
		MOV	CX,DS
		ADD	CX,DI
		MOV	ES:WorkBuffer,CX
		MOV	BX,BP
		MOV	SI,OFFSET GlobalSegs_L
		CALL	CompressBlock
		MOV	AL,DS:LongNames
		MOV	DS:LongNamFlag,AL
ExitRdDir:	CMP	DS:ChgFlg,1		; Exit
		CMC
		POP	BP
		RET
ReadDir		ENDP

DirTwoNames	PROC	NEAR			; Set short and long path
		USES	CX,SI,DI,ES
		MOV	CL,ES:OS2box
		PUSH	DS
		POP	ES
		MOV	SI,OFFSET WinShortPath
		MOV	DI,OFFSET WinLongPath
		PUSH	DI
		PUSH	SI
		CALL	MvzLine
		POP	SI
		POP	DI
		OR	CL,CL
		JNZ	@@OS2
		XCHG	SI,DI
		CALL	ShortName
		JNC	@@LongName
		CMP	AX,7100h
		JE	@@Exit
		STC
		JMP	@@Exit
  @@LongName:	MOV	AL,0
		CALL	RealPath
		JMP	@@Exit
  @@OS2:	CMP	DS:LongNames,0
		JZ	@@Exit
		CALL	LongNameOS2
		JNC	@@Exit
		CMP	AX,53h
		STC	
		JE	@@Exit
		CLC
  @@Exit:	RET
DirTwoNames	ENDP

ReadTree:	CMP	DS:WinTyp,1
		JNE	ReadInfo
		CALL	ScanTree
		JC	RdTree1
		CALL	UpdateBranch
		MOV	DS:ChgFlg,0
RdTree1:	CALL	SizDX
		SUB	DH,4
		CALL	SetTreeParms
		JMP	ExitRdDir

		.CONST
DirInfo_S	DB	' Info ',0

		.DATA
DirInfo_T	DW	DirInfo_S, LineBuf2, ExecArcProgram

		.CODE	Subs
ReadInfo	PROC	FAR
		CMP	DS:WinTyp,2
		JNE	ReadQuickView
		XOR	BX,BX			; Remove DirInfo segment
		XCHG	BX,ES:DirinfoSeg
		MOV	SI,OFFSET GlobalSegs_L
		CALL	CompressBlock
		PUSH	DS
		PUSH	ES			; Init data
		POP	DS
		XOR	AX,AX
		MOV	DS:ClusterSize.Lo,1
		MOV	DS:ClusterSize.Hi,AX
		MOV	DS:TotalClusters.Lo,AX
		MOV	DS:TotalClusters.Hi,AX
		MOV	DS:FreeClusters.Lo,AX
		MOV	DS:FreeClusters.Hi,AX
		MOV	DS:Volume[0],AL
		MOV	SI,OFFSET DirInfoFile	; Delete old temp arc file
		DEC	AX
		XCHG	AX,DS:DirInfoNumb
		CALL	DelTempArcFile
		JNC	@@GetSize
		CMP	AX,53h
		JE	@@Exit
  @@GetSize:	MOV	AL,DS:DosPath[0]	; Get total and free space
		MOV	DI,OFFSET ClusterSize
		CALL	GetDiskInfo
		JNC	@@Label
		CMP	AX,53h
		JE	@@Exit
  @@Label:	MOV	SI,OFFSET Volume	; Get volume label
		CALL	GetVol
		JNC	@@DirInfo
		CMP	AX,53h
		JE	@@Exit
  @@DirInfo:	POP	DS			; Directory information
		PUSH	DS
		MOV	SI,OFFSET DirInfoFiles
		XOR	BP,BP
		CALL	InvWCB
		CMP	DS:Visible,0
		JZ	@@Parse
		CMP	DS:WinTyp,0
		JNE	@@Parse
		CMP	DS:ArcPnt,0
		JZ	@@Parse
		MOV	BP,DS
  @@Parse:	PUSH	ES			; Find file in current dir
		POP	DS
  @@NextFile:	CALL	FndLet
		LODSB
		CMP	AL,0
		JE	@@Done
		CMP	AL,';'
		JE	@@NextFile
		CMP	AL,','
		JE	@@NextFile
		DEC	SI
		MOV	DX,SI
  @@FindEnd:	LODSB
		CMP	AL,0
		JE	@@FindFile
		CMP	AL,';'
		JE	@@FindFile
		CMP	AL,','
		JNE	@@FindEnd
  @@FindFile:	DEC	SI
		MOV	DI,DS:[SI]
		MOV	BYTE PTR DS:[SI],0
		OR	BP,BP
		JNE	@@FindInArc
		MOV	CX,27h
		MOV	AH,4Eh
		CALL	Intr21
		CALL	CloseFindFile
		MOV	DS:[SI],DI
		JNC	@@SetFile
		CMP	AX,53h
		JE	@@Exit
		JMP	@@NextFile
  @@SetFile:	MOV	SI,OFFSET FindData.WinLongName
		CMP	BYTE PTR DS:[SI],0
		JNE	@@MoveName
		MOV	SI,OFFSET DTA.FilName
  @@MoveName:	MOV	DI,OFFSET DirInfoFile
		PUSH	DI
		CALL	MvzLine
		POP	SI
		JMP	@@InitView
  @@FindInArc:	PUSH	DI			; Find in archive
		PUSH	SI
		MOV	DS,BP
		MOV	DI,DX
		MOV	DX,0FFFFh
		MOV	SI,DS:DiskBuf
  @@ChkArcFile:	CMP	SI,DS:EndBuf
		JAE	@@ChkFound
		TEST	DS:[SI].AttrF,10h
		JNZ	@@Next
		PUSH	SI
		ADD	SI,OFFSET NameF
		CALL	TestMask
		POP	SI
		JNZ	@@Next
		CMP	DX,DS:[SI].NumbF
		JB	@@Next
		MOV	DX,DS:[SI].NumbF
		MOV	BX,SI
  @@Next:	CALL	NextFilEnt
		JMP	@@ChkArcFile
  @@Error:	PUSH	ES			; Error - delete temp arc file
		POP	DS
		MOV	SI,OFFSET DirInfoFile
		MOV	AX,0FFFFh
		XCHG	AX,DS:DirInfoNumb
		CALL	DelTempArcFile
		JMP	@@Exit
  @@ChkFound:	POP	SI
		POP	DI
		MOV	ES:[SI],DI
		INC	DX
		JE	@@Parse
		MOV	DI,OFFSET LineBuf2	; Extract archive
		PUSH	DI
		CALL	PutDescriptor
		CALL	AddFile
		LEA	SI,[BX].NameF
		CALL	MvzLine
		POP	SI
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET DirInfo_T
		CALL	ViewArcExtract
		JC	@@Exit
		CMP	AX,0FFFFh
		JE	@@Exit
		MOV	DS:DirInfoNumb,AX
		CALL	CutPath
		PUSH	SI
		MOV	DI,OFFSET DirInfoFile
		PUSH	AX
		CALL	MvzLine
		POP	AX
		MOV	DI,OFFSET TmpBuf
		MOV	SI,OFFSET VcTempArc_S
		CALL	MakePathName
		CALL	AddFile
		POP	SI
		CALL	MvzLine
		MOV	SI,OFFSET TmpBuf
  @@InitView:	MOV	AX,DS:WorkBuffer	; Init view file
		ADD	AX,MinView/16/2
		MOV	BP,ES
		ADD	BP,DS:SizeDataSeg
		SUB	BP,AX
		MOV	AX,QuickViewSize
		CMP	BP,AX
		JBE	@@ViewInit
		MOV	BP,AX
  @@ViewInit:	MOV	AL,1
		CALL	ViewInit
		JC	@@Error
		MOV	AH,3Eh
		CALL	Intr21
		JC	@@Error
		MOV	DS:VwHandle,0FFFEh
		MOV	DX,DS:VwLen.Lo
		MOV	CX,DS:VwLen.Hi
		MOV	AX,BP
		ADD	DX,OFFSET VwBuffer-1
		ADC	CX,0
		JNE	@@SetSeg
		MOV	CL,4
		SHR	DX,CL
		INC	DX
		CMP	AX,DX
		JBE	@@SetSeg
		MOV	AX,DX
  @@SetSeg:	ADD	ES:WorkBuffer,AX
		MOV	ES:DirinfoSeg,DS
  @@Done:	POP	DS
		MOV	DS:ChgFlg,0
		PUSH	DS
  @@Exit:	POP	DS
		JMP	ExitRdDir
ReadInfo	ENDP

QuickViewSize	EQU	400h			; Maximal size in paragraphs

QuickViewData	STRUC
  ViewAttr	DB	?
  ViewTempNumb	DW	?
  ViewArcPath	DW	?
  ViewName1	DB	LenStrFull+2 DUP(?)	; Short name / Arc name
  ViewName2	DB	LenStrFull+2 DUP(?)	; Long name / Temp name
QuickViewData	ENDS

; ViewAttr:
;	b0 - empty view panel
;	b1 - archive
;	b3 - absent file
;	b4 - directory
;	b7 - VwData was inited

		.CONST
Counting_S	DB	'Counting...',0

		.DATA
QuickView_T	DW	View_S, LineBuf2, ExecArcProgram

		.CODE	Subs
ReadQuickView	PROC	FAR
		MOV	DS:ChgFlg,0
		MOV	BX,ES:ViewSeg		; Compress old block
		OR	BX,BX
		JE	@@ChkPanel
		ADD	BX,(SIZE QuickViewData-1)/16+1 + (SIZE ViewData-1)/16+1
		MOV	SI,OFFSET GlobalSegs_L
		CALL	CompressBlock
  @@ChkPanel:	PUSH	DS			; Check another panel
		CALL	InvWCB
		XOR	BX,BX
		MOV	AL,DS:WinTyp
		CMP	AL,1
		JA	@@DelOld
		MOV	BX,ES:ViewSeg
		MOV	CX,ES:WorkBuffer
		CMP	DS:Visible,0
		JNZ	@@NewSeg
		OR	BX,BX
		JNE	@@CopyOld
		POP	DS
		JMP	@@Exit
  @@CopyOld:	PUSH	ES			; Copy old Quick View parameters
		MOV	DS,BX
		MOV	ES,CX
		MOV	CX,SIZE QuickViewData
		REP	MOVSB
		JMP	@@DelTemp
  @@NewSeg:	PUSH	ES			; Create new segment
		MOV	ES,CX
		MOV	ES:ViewAttr,10h
		MOV	ES:ViewTempNumb,0FFFFh
		MOV	DI,OFFSET ViewName2
		CMP	AL,0
		JNE	@@ViewTree
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JB	@@ViewFile
		MOV	ES:ViewAttr,1		; Empty (no files in panel)
		JMP	@@DelTemp
  @@ViewFile:	CMP	DS:ArcPnt,0		; File panel
		JNE	@@ViewArc
		PUSH	SI
		MOV	SI,OFFSET WinLongPath
		CALL	MvzLine
		MOV	DX,DI
		MOV	DI,OFFSET ViewName1
		MOV	SI,OFFSET WinShortPath
		CALL	MvzLine
		POP	SI
		PUSH	SI
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		JE	@@SetAttrib
		CALL	AddFile
		CALL	MvzLine
		POP	SI
		PUSH	SI
		CALL	LongNameOffset1
		MOV	DI,DX
		JMP	@@AddName
  @@ViewArc:	OR	ES:ViewAttr,2		; File panel - archive
		MOV	DI,OFFSET ViewName1
		PUSH	SI
		MOV	SI,OFFSET ArcExt
		CALL	MvzLine
		MOV	AL,':'
		STOSB
		CALL	PutArcPathName
		CALL	AddFile
		MOV	ES:ViewArcPath,DI
		MOV	SI,OFFSET ArcPath
		CALL	MvzLine
		POP	SI
		PUSH	SI
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		JE	@@SetAttrib
  @@AddName:	CALL	AddFile
		CALL	MvzLine
  @@SetAttrib:	POP	SI
		MOV	AL,DS:[SI].AttrF
		TEST	AL,10h
		JNZ	@@DelTemp
		AND	AL,8
		OR	ES:ViewAttr,AL
		AND	ES:ViewAttr,0EFh
		JMP	@@DelTemp
  @@ViewTree:	MOV	SI,OFFSET CrnPath	; Tree panel
		CALL	LongTreePath1
		MOV	DI,OFFSET ViewName1
		CALL	MvzLine
  @@DelTemp:	MOV	DS,CX			; Delete temp file if exists
		OR	BX,BX
		JZ	@@Init
		MOV	ES,BX
		MOV	DX,ES:ViewTempNumb
		CMP	DX,0FFFFh
		JE	@@Init
		MOV	SI,OFFSET ViewName1
		MOV	DI,SI
		CALL	StrLen
		PUSH	SI
		REPE	CMPSB
		POP	SI
		JNE	@@DelFile
		MOV	DS:ViewTempNumb,DX
		JMP	@@Init
  @@DelFile:	MOV	AX,ES
		POP	ES
		PUSH	ES
		PUSH	DS
		MOV	DS,AX
		MOV	AX,DX
		CALL	DelTempArcFile
		POP	DS
  @@Init:	POP	ES			; Init new Quick View
		ADD	ES:WorkBuffer,(SIZE QuickViewData-1)/16+1
		TEST	DS:ViewAttr,9
		JNZ	@@Empty
		TEST	DS:ViewAttr,10h
		JNZ	@@InitDir
		TEST	DS:ViewAttr,2
		JZ	@@InitFile
		MOV	AX,DS:ViewTempNumb	; Extract archive
		CMP	AX,0FFFFh
		JNE	@@InitArc
		MOV	SI,OFFSET ViewName1
		MOV	DI,OFFSET LineBuf2
		PUSH	DI
		CALL	MvzLine
		POP	SI
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET QuickView_T
		CALL	ViewArcExtract
		JC	@@FileError
		CMP	AX,0FFFFh
		JE	@@FileError
		POP	DS
		MOV	DS:ViewTempNumb,AX
  @@InitArc:	PUSH	ES
		PUSH	DS
		POP	ES
		MOV	DI,OFFSET ViewName2
		MOV	SI,OFFSET VcTempArc_S
		CALL	MakePathName
		CALL	AddFile
		MOV	SI,OFFSET ViewName1
		CALL	CutPath
		CALL	MvzLine
		POP	ES
  @@InitFile:	MOV	SI,OFFSET ViewName2	; Init view file
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		CALL	MvzLine
		POP	SI
		MOV	AX,DS
		ADD	AX,MinView/16/2
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	BP,ES
		CMP	DS:ViewSeg,0
		JE	@@SetLimit
		ADD	BP,(SIZE QuickViewData-1)/16+1 + (SIZE ViewData-1)/16+1
  @@SetLimit:	ADD	BP,DS:SizeDataSeg
		SUB	BP,AX
		MOV	AX,QuickViewSize
		CMP	BP,AX
		JBE	@@ViewInit
		MOV	BP,AX
  @@ViewInit:	MOV	AL,1
		CALL	ViewInit
		JC	@@FileError
		MOV	AH,3Eh
		CALL	Intr21
		JC	@@FileError
		MOV	DS:VwHandle,0FFFFh
		MOV	DX,DS:VwLen.Lo
		MOV	CX,DS:VwLen.Hi
		POP	DS
		OR	DS:ViewAttr,80h
		MOV	AX,BP
		ADD	DX,OFFSET VwBuffer-1
		ADC	CX,0
		JNE	@@SetSeg
		MOV	CL,4
		SHR	DX,CL
		INC	DX
		CMP	AX,DX
		JBE	@@SetSeg
		MOV	AX,DX
		JMP	@@SetSeg
  @@FileError:	POP	AX
		POP	DS
		MOV	DS:ChgFlg,1
		PUSH	DS
		MOV	DS,AX
		MOV	DS:ViewAttr,8
		JMP	@@Empty
  @@InitDir:	POP	AX			; Init view directory
		PUSH	AX
		PUSH	DS
		MOV	DS,AX
		CALL	BegBX
		CALL	SizDX
		ADD	BH,2
		MOV	DH,1
		CALL	SavWin
		PUSH	DX
		PUSH	BX
		PUSH	CS
		POP	DS
		SHR	DL,1
		ADD	BL,DL
		MOV	SI,OFFSET Counting_S
		MOV	BP,OFFSET PtIn_P
		CALL	CntLine
		POP	BX
		POP	DX
		CALL	Window
		POP	DS
		PUSH	DS
		TEST	DS:ViewAttr,2
		JNZ	@@ArcDir
		MOV	DI,OFFSET ClusterSize
		MOV	AL,ES:DosShortPath[0]
		CALL	GetDiskInfo
		JC	@@SkipSize
		MOV	DI,OFFSET TmpBuf
		MOV	CX,256
		XOR	AX,AX
		REP	STOSB
		MOV	DI,OFFSET QuickViewFiles
		MOV	CL,12
		REP	STOSW
		MOV	SI,OFFSET ViewName2
		MOV	DI,OFFSET LineBuf1
		CALL	MvzLine
		MOV	ES:FncAddr,OFFSET QuickViewDir
		MOV	CL,1
		XCHG	CL,ES:IncDirs
		PUSH	CX
		CALL	DirFunc
		POP	CX
		MOV	ES:IncDirs,CL
		CMP	AH,1
		JA	@@SkipSize
		CMC
		JMP	@@SkipSize
  @@ArcDir:	MOV	DI,OFFSET QuickViewFiles ; Init view dir of archive
		MOV	CX,12
		XOR	AX,AX
		REP	STOSW
		MOV	DI,OFFSET LineBuf2
		PUSH	DI
		MOV	AX,'.A'
		STOSW
		MOV	SI,OFFSET ViewName1
  @@MoveExt:	LODSB
		STOSB
		CMP	AL,':'
		JNE	@@MoveExt
		DEC	DI
		MOV	AL,0
		STOSB
		MOV	DI,SI
		POP	SI
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	DL,0
		CALL	ChkArcExt
		POP	DS
		JC	@@SkipSize
		PUSH	CX
		XCHG	SI,DI
		MOV	CX,DS:ViewArcPath
		SUB	CX,SI
		DEC	CX
		REP	MOVSB
		MOV	AL,0
		STOSB
		MOV	AX,ES:WorkBuffer
		MOV	DS,AX
		MOV	BP,ES
		ADD	BP,ES:SizeDataSeg
		SUB	BP,AX
		MOV	AX,0FFFh
		CMP	BP,AX
		JBE	@@CalcSize
		MOV	BP,AX
  @@CalcSize:	MOV	CL,4
		SHL	BP,CL
		POP	CX
		XOR	DI,DI
		MOV	ES:QuickViewComp,0
		MOV	AX,OFFSET ExecArcCalcEnt
		CALL	ReadArcDir
  @@SkipSize:	POP	DS
		JNC	@@RestMsg
		OR	DS:ViewAttr,1
  @@RestMsg:	CALL	ResWin
		CALL	Window
  @@Empty:	MOV	AX,(SIZE ViewData-1)/16+1
  @@SetSeg:	ADD	ES:WorkBuffer,AX
		MOV	BX,DS
		MOV	AX,ES:ViewSeg		; Copy old parameters
		OR	AX,AX
		JE	@@DelOld
		MOV	DS,AX
		TEST	DS:ViewAttr,80h
		JZ	@@DelOld
		PUSH	ES
		MOV	DS,BX
		OR	DS:ViewAttr,80h
		ADD	AX,(SIZE QuickViewData-1)/16+1
		MOV	DS,AX
		MOV	AX,BX
		ADD	AX,(SIZE QuickViewData-1)/16+1
		MOV	ES,AX
		MOV	AL,DS:VwTextHex
		MOV	ES:VwTextHex,AL
		MOV	AL,DS:VwWrap
		MOV	ES:VwWrap,AL
		MOV	AL,DS:VwShift
		MOV	ES:VwShift,AL
		MOV	SI,OFFSET VwTbl
		MOV	DI,SI
		MOV	CX,SIZE TblData
		REP	MOVSB
		POP	ES
  @@DelOld:	POP	DS			; Delete old segment
		XCHG	BX,ES:ViewSeg
		OR	BX,BX
		JE	@@Exit
		MOV	SI,OFFSET GlobalSegs_L
		CALL	CompressBlock
  @@Exit:	JMP	ExitRdDir
ReadQuickView	ENDP

		.CODE
QuickViewDir	PROC	NEAR
		USES	BX,CX,DX,SI,DI,DS
		XOR	CX,CX
		TEST	AL,10h
		JZ	@@AddSize
		ADD	ES:QuickViewDirs.Lo,1
		ADC	ES:QuickViewDirs.Hi,CX
		CALL	DirFunc
		CMP	AH,1
		JBE	@@Exit
		MOV	AH,0
		JMP	@@Exit
  @@AddSize:	PUSH	ES
		POP	DS
		ADD	DS:QuickViewFiles.Lo,1
		ADC	DS:QuickViewFiles.Hi,CX
		MOV	AX,DS:DTA.FilSize.Lo
		MOV	DX,DS:DTA.FilSize.Hi
		ADD	DS:QuickViewFlSize[0].Lo,AX
		ADC	DS:QuickViewFlSize[0].Hi,DX
		ADC	DS:QuickViewFlSize[4].Lo,CX
		ADC	DS:QuickViewFlSize[4].Hi,CX
		CALL	FileClustSize
		ADD	DS:QuickViewClSize[0].Lo,AX
		ADC	DS:QuickViewClSize[0].Hi,DX
		ADC	DS:QuickViewClSize[4].Lo,CX
		ADC	DS:QuickViewClSize[4].Hi,CX
		XOR	AX,AX
  @@Exit:	RET
QuickViewDir	ENDP

;---------------------------------------------------------------
;	Output contents of panel 
;		   DS - WCBseg
;---------------------------------------------------------------
		.CONST
BytIn_S		DB	'^2E byte^0Q/s/ in ^1I selected file^1P/s/',0

		.DATA
BytIn_D		DW	TmpErr, TmpErr+4, TmpErr+6

		.CODE	Subs
PutFls		PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,BP,DS
		CMP	DS:WinTyp,0
		JNE	PutTre
		CALL	CorPar
		CALL	LnsAX
		MOV	CX,AX
		CALL	BegBX
		ADD	BX,201h
		CALL	SizDX
		MOV	AL,DL
		MOV	BP,AX
		DEC	BP
		DEC	BP
		MOV	DL,BH
		ADD	DL,CL
		CMP	DS:BrfFul,0
		JNZ	@@SetAddr
		PUSH	DX
		PUSH	BX
		PUSH	AX
		CALL	NameWidthAL
		INC	AX
		MOV	DL,AL
		SUB	AL,3
		MOV	DH,AL
		POP	AX
		ADD	BL,AL
		ADD	AL,DH
		DIV	DL
		CMP	DS:LongNames,0
		JZ	@@CorrFill
		INC	DH
		SUB	DH,AL
  @@CorrFill:	CMP	AH,DH
		JA	@@Columns
		PUSH	ES
		PUSH	CX
		PUSH	BX
		PUSH	AX
		MOV	DL,AH
		INC	DX
		DEC	BX
		DEC	BX
		SUB	BL,DL
		MOV	AH,Back_C
		CALL	Color
		MOV	AL,' '
  @@FillBlock:	PUSH	CX
		CALL	BasAdr
		MOV	CL,DL
		REP	STOSW
		POP	CX
		INC	BH
		LOOP	@@FillBlock
		POP	AX
		POP	BX
		POP	CX
		POP	ES
  @@Columns:	PUSH	AX
		CBW
		MUL	CX
		MOV	CX,AX
		POP	AX
		POP	BX
		POP	DX

  @@SetAddr:	MOV	AH,DS:NameWidth		; Output files
		PUSH	AX
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		MOV	DI,SI
		MOV	SI,DS:FileFirst
		CALL	Numb2Addr
  @@NextFile:	CMP	SI,DS:EndBuf
		JAE	@@MiniSta
		MOV	AX,DS
		CMP	AX,ES:WCBcrn
		PUSHF
		MOV	AH,Back_C
		CMP	ES:ColorDirs,0
		JE	@@DirColor
		TEST	DS:[SI].AttrF,10h
		JE	@@DirColor
		MOV	AH,Dirs_C
  @@DirColor:	POPF
		MOV	AL,Slct_C
		JNE	@@ChkSelect
		CMP	SI,DI
		JNE	@@ChkSelect
		MOV	AH,Cursr_C
		MOV	AL,CurSl_C
  @@ChkSelect:	TEST	DS:[SI].AttrF,40h
		JZ	@@SetColor
		MOV	AH,AL
  @@SetColor:	PUSH	DI
		PUSH	SI
		PUSH	CX
		MOV	DI,OFFSET TmpErr
		MOV	AL,0
		CALL	PutName
		MOV	AL,ES:Borders[12]
		CMP	DS:BrfFul,1
		JB	@@Write
		JA	@@Descript
		CALL	SzDtTm
  @@Descript:	STOSB
		PUSH	DI
		SUB	DI,OFFSET TmpErr
		MOV	CX,BP
		SUB	CX,DI
		POP	DI
		JBE	@@Write
		CALL	PutDescr
  @@Write:	POP	CX
		PUSH	ES
		PUSH	DS
		PUSH	BP
		PUSH	AX
		CMP	DS:BrfFul,0
		JNE	@@SetLimit
		CALL	NameWidthAL
		MOV	AH,0
		CMP	BP,AX
		JBE	@@SetLimit
		MOV	BP,AX
  @@SetLimit:	POP	AX
		PUSH	ES
		POP	DS
		CALL	Color
		CALL	BasAdr
		MOV	SI,OFFSET TmpErr
  @@NextChar:	LODSB
		STOSW
		DEC	BP
		JNE	@@NextChar

		POP	BP
		POP	DS
		POP	ES
		POP	SI
		POP	DI
		INC	BH
		CMP	BH,DL
		JB	@@NextAdr
		CALL	LnsAX
		SUB	BH,AL
		CALL	NameWidthAL
		INC	AX
		ADD	BL,AL
		SUB	BP,AX
		CMP	DS:LongNames,0
		JZ	@@NextAdr
		DEC	AX
		SUB	AX,BP
		JBE	@@NextAdr
		MOV	DH,AL
		POP	AX
		PUSH	AX
		CMP	DH,AL
		JAE	@@NextAdr
		SUB	DS:NameWidth,DH
  @@NextAdr:	CALL	NextFilEnt
		LOOP	@@NextFile

  @@MiniSta:	POP	AX
		MOV	DS:NameWidth,AH
		CMP	ES:MiniSta,0		; Mini status
		JZ	ExitPutFls
		CALL	BegBX
		CALL	SizDX
		ADD	BH,DH
		SUB	BH,2
		INC	BX
		PUSH	ES
		CALL	BasAdr
		MOV	AH,Back_C
		CALL	Color
		MOV	AL,' '
		MOV	CL,DL
		MOV	CH,0
		DEC	CX
		DEC	CX
		REP	STOSW
		POP	ES
		XOR	DX,DX
		XOR	CX,CX
		XOR	DI,DI
		MOV	SI,DS:DiskBuf
		CMP	SI,DS:EndBuf
		JAE	ExitPutFls
  @@CalcSize:	TEST	DS:[SI].AttrF,40h
		JZ	@@Next
		TEST	DS:[SI].AttrF,10h
		JZ	@@AddSize
		TEST	DS:[SI].AttrF,8
		JZ	@@Next
  @@AddSize:	ADD	CX,DS:[SI].SizeF.Lo
		ADC	DI,DS:[SI].SizeF.Hi
		INC	DX
  @@Next:	CALL	NextFilEnt
		CMP	SI,DS:EndBuf
		JB	@@CalcSize
		OR	DX,DX
		JNE	@@Selected
		MOV	SI,DS:FileCur		; One file
		CALL	Numb2Addr
		CMP	ES:ColorDirs,0
		JE	@@DirStatus
		TEST	DS:[SI].AttrF,10h
		JE	@@DirStatus
		MOV	AH,Dirs_C
		CALL	Color
  @@DirStatus:	MOV	DI,OFFSET TmpErr
		PUSH	DI
		MOV	AL,1
		CALL	PutName
		MOV	AL,' '
		CALL	SzDtTm
		STOSB
		CALL	SizDX
		MOV	CL,DL
		MOV	CH,0
		DEC	CX
		DEC	CX
		PUSH	CX
		PUSH	DI
		SUB	DI,OFFSET TmpErr
		SUB	CX,DI
		POP	DI
		JBE	@@NoDescr
		CALL	PutDescr
  @@NoDescr:	POP	CX
		POP	SI
		PUSH	ES
		PUSH	ES
		POP	DS
		CALL	BasAdr
  @@StatFile:	LODSB
		STOSW
		LOOP	@@StatFile
		POP	ES
		JMP	ExitPutFls
  @@Selected:	MOV	ES:TmpErr[0].Lo,CX	; Selected files
		MOV	ES:TmpErr[0].Hi,DI
		MOV	ES:TmpErr[4].Lo,DX
		PUSH	BX
		MOV	BX,DI
		MOV	DI,OFFSET TmpErr+6
		MOV	AL,1
		CALL	Number
		POP	BX
		CALL	SizDX
		DEC	DX
		MOV	CL,DL
		DEC	CX
		SHR	DL,1
		ADD	BL,DL
		MOV	BP,OFFSET PtIn_P+1
		MOV	SI,OFFSET BytIn_S
		MOV	DX,OFFSET BytIn_D
		PUSH	SI
		PUSH	CS
		POP	DS
		MOV	AL,0
		CALL	NgLine
		POP	SI
		CMP	AL,CL
		JBE	@@WriteStat
		SUB	DI,4
		MOV	AX,'K'
		STOSW
		PUSH	SI
		MOV	AL,0
		CALL	NgLine
		POP	SI
		CMP	AL,CL
		JBE	@@WriteStat
		SUB	DI,6
		MOV	AX,'M'
		STOSW
  @@WriteStat:	CALL	CntLine
  ExitPutFls:	RET
PutFls		ENDP

		.DATA
PutTr_P		DB	Back_C, Cursr_C, Slct_C, CurSl_C

		.CODE	Subs
PutTre:		CMP	DS:WinTyp,1
		JNE	PutInf
		CALL	CorPar
		MOV	BP,OFFSET PutTr_P
		CALL	BegBX
		CALL	SizDX
		CALL	PutTree
		JMP	ExitPutFls

PutTree		PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,DS
		LOCAL	LineBuffer:BYTE:1024
		LOCAL	PathBuffer:BYTE:LenPath
		LOCAL	Branches  :BYTE:128
		LOCAL	WCBsegment:WORD
		LOCAL	StatusLine:WORD
		LOCAL	TreeAddr  :WORD
		LOCAL	TreeFrst  :WORD
		LOCAL	TreeCount :WORD
		LOCAL	TreeType  :BYTE
		MOV	WCBsegment,DS
		MOV	TreeCount,0
		MOV	AX,BX
		ADD	AH,DH
		SUB	AH,2
		INC	AX
		MOV	StatusLine,AX
		ADD	BX,101h
		XOR	AX,AX
		MOV	DS:TreeCrnt,AX
		MOV	SI,DS:TreeCur
		CALL	TreeNumb2AdrH
		MOV	TreeAddr,SI
		MOV	SI,DS:TreeFirst
		CALL	TreeNumb2AdrH
		MOV	TreeFrst,SI
		PUSH	ES			; Set TreeTyp
		PUSH	DS
		MOV	DS,ES:TreeSeg
		POP	ES
		MOV	CX,DS:TreeLenH
		JCXZ	@@SetType
		PUSH	BX
		PUSH	DX
		XOR	BX,BX
		MOV	SI,OFFSET TreeList
  @@MaxLevel:	MOV	AL,3
		MUL	DS:[SI].DirLevel
		MOV	DX,AX
		MOV	AL,ES:LongNames
		CALL	DirLen1
		ADD	AX,DX
		CMP	AX,BX
		JBE	@@NextLevel
		MOV	BX,AX
  @@NextLevel:	CALL	NextDirEntH
		LOOP	@@MaxLevel
		POP	DX
		MOV	AL,DL
		MOV	AH,0
		SUB	AX,4
		CMP	BX,AX
		POP	BX
		MOV	AL,0
		JBE	@@SetType
		INC	AX
  @@SetType:	MOV	ES:TreeTyp,AL
		MOV	TreeType,AL
		MOV	SI,TreeAddr		; Set offset
		MOV	AH,AL
		MOV	AL,3
		SUB	AL,AH
		MUL	DS:[SI].DirLevel
		PUSH	AX
		MOV	AL,ES:LongNames
		CALL	DirLen1
		POP	SI
		ADD	AX,SI
		PUSH	ES
		POP	DS
		POP	ES
		MOV	DS:TreeOffset,0
		MOV	CL,DL
		SUB	CL,4
		MOV	CH,0
		SUB	AX,CX
		JBE	@@InitVars
		CMP	AX,SI
		JBE	@@SetOffs
		MOV	AX,SI
  @@SetOffs:	MOV	DS:TreeOffset,AL
  @@InitVars:	MOV	AX,WORD PTR ES:DosPath	; Init variables
		PUSH	ES
		PUSH	SS
		POP	ES
		LEA	DI,PathBuffer
		STOSW
		MOV	AX,'\'
		STOSW
		LEA	DI,Branches
		MOV	CX,128
		MOV	AL,0
		REP	STOSB
		POP	ES
		MOV	SI,OFFSET TreeList	; Number of lines
		MOV	CX,DS:TreeFirst
		MOV	DS,ES:TreeSeg
		MOV	AX,DS:TreeLenH
		SUB	AX,CX
		MOV	CL,DH
		SUB	CL,4
		MOV	CH,0
		CMP	CX,AX
		JBE	@@WriteLine
		MOV	CX,AX
		OR	CX,CX
		JE	@@Exit

  @@WriteLine:	PUSH	SI			; Output lines
		PUSH	CX			; Clear line
		PUSH	BX
		MOV	DI,[BP]
		MOV	AH,CS:[DI]
		CALL	Color
		MOV	AL,' '
		MOV	CX,512
		LEA	DI,LineBuffer
		PUSH	ES
		PUSH	DI
		PUSH	SS
		POP	ES
		REP	STOSW
		POP	DI
		STOSW
		POP	ES

		LEA	BX,Branches		; Output vertical branches
		MOV	CL,DS:[SI].DirLevel
		JCXZ	@@ChkNew
		DEC	CX
		JCXZ	@@PutBranch
		MOV	AL,0B3h ; '│'
  @@Branches:	CMP	SS:[BX],CH
		JE	@@SkipBranch
		MOV	SS:[DI],AL
  @@SkipBranch:	ADD	DI,2*2
		CMP	TreeType,0
		JNZ	@@NextBranch
		INC	DI
		INC	DI
  @@NextBranch: INC	BX
		LOOP	@@Branches

  @@PutBranch:	PUSH	SI			; Output horisontal branch
		MOV	CX,DS:TreeLenH
		SUB	CX,TreeCount
		JBE	@@EndBranch
		MOV	AL,0C3h ; '├'
		MOV	AH,DS:[SI].DirLevel
  @@ChkBranch:	CALL	NextDirEntH
		CMP	AH,DS:[SI].DirLevel
		JA	@@EndBranch
		JE	@@PutBranch1
		LOOP	@@ChkBranch
  @@EndBranch:	MOV	AL,0C0h ; '└'
		MOV	BYTE PTR SS:[BX],0
  @@PutBranch1:	POP	SI
		INC	BX
		MOV	SS:[DI],AL
		INC	DI
		INC	DI
		MOV	AL,0C4h ; '─'
		MOV	SS:[DI],AL
		INC	DI
		INC	DI
		CMP	TreeType,0
		JNZ	@@ChkNew
		MOV	SS:[DI],AL
		INC	DI
		INC	DI

  @@ChkNew:	PUSH	SI			; Add new vertical branch
		MOV	AL,DS:[SI].DirLevel
		CALL	NextDirEntH
		CMP	AL,DS:[SI].DirLevel
		POP	SI
		JAE	@@FullPath
		MOV	BYTE PTR SS:[BX],1

  @@FullPath:	MOV	CL,DS:[SI].DirLevel	; Full path
		MOV	CH,0
		JCXZ	@@CmpPath
		PUSH	ES
		PUSH	SI
		PUSH	DI
		PUSH	SS
		POP	ES
		LEA	DI,PathBuffer
		MOV	AL,'\'
  @@FndSlash:	PUSH	CX
		MOV	CH,0FFh
		REPNE	SCASB
		POP	CX
		LOOP	@@FndSlash
		ADD	SI,OFFSET DirName
		CALL	MvzLine
		MOV	AX,'\'
		STOSW
		POP	DI
		POP	SI
		POP	ES

  @@CmpPath:	PUSH	DS			; Compare directory
		PUSH	SI
		PUSH	DI
		PUSH	SS
		POP	DS
		LEA	SI,PathBuffer
		MOV	DI,OFFSET DosShortPath
		CALL	StrLen
		DEC	CX
		REPE	CMPSB
		JNE	@@SetColor
		CMP	AL,DS:[SI]
		JE	@@SetColor
		CMP	AL,DS:[SI+1]
  @@SetColor:	MOV	SI,[BP]
		MOV	AX,CS:[SI]
		POP	DI
		POP	SI
		POP	DS
		JNE	@@ChkCursor

		PUSH	DS			; Current directory
		PUSH	SI
		MOV	DS,WCBsegment
		MOV	DS:TreeCrnt,SI
		MOV	BX,[BP]
		MOV	AX,CS:[BX+2]
		PUSH	AX
		MOV	AH,AL
		CALL	Color
		MOV	BL,DS:TreeOffset
		MOV	BH,0
		ADD	BL,DL
		SUB	BL,3
		ADD	BX,BX
		MOV	AL,''
		LEA	SI,LineBuffer
		ADD	BX,SI
		MOV	SS:[BX],AX
		POP	AX
		POP	SI
		POP	DS

  @@ChkCursor:	PUSH	AX			; Check cursor
		PUSH	DS
		MOV	DS,WCBsegment
		MOV	AL,DS:LongNames
		POP	DS
		CALL	DirLen1
		DEC	AX
		MOV	CX,AX
		MOV	AL,DL
		SUB	AL,5
		MOV	AH,0
		CMP	CX,AX
		JBE	@@CurColor
		MOV	CX,AX
  @@CurColor:	POP	AX
		XCHG	AH,AL
		CMP	SI,TreeAddr
		JE	@@SetCursor
		CALL	Color
		JMP	@@PutName
  @@SetCursor:	MOV	BX,WCBsegment
		CMP	BX,ES:WCBcrn
		MOV	BX,0DDDEh ; '▌▐'
		JNE	@@WriteCur
		MOV	BX,'  '
		MOV	AH,AL

  @@WriteCur:	PUSH	ES			; Output cursor
		PUSH	DS
		PUSH	DI
		PUSH	SI
		PUSH	CX
		PUSH	SS
		POP	DS
		CALL	Color
		MOV	AL,BH
		MOV	DS:[DI-2],AX
		ADD	DI,CX
		ADD	DI,CX
		MOV	AL,BL
		MOV	DS:[DI],AX
		LEA	SI,PathBuffer
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		CALL	MvzLine
		PUSH	ES
		POP	DS
		POP	DI
		MOV	SI,DI
		MOV	AL,0
		CMP	AL,DS:[DI+3]
		JE	@@PutStatus
		MOV	CH,0FFh
		REPNE	SCASB
		DEC	DI
		DEC	DI
		STOSB

  @@PutStatus:	PUSH	ES			; Output status line
		PUSH	SI
		MOV	ES,WCBsegment
		MOV	DI,OFFSET CrnPath
		MOV	CX,LenPath-1
		PUSH	DI
		REP	MOVSB
		STOSB
		POP	SI
		POP	DI
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		CALL	TreePath1
		MOV	SI,DI
		PUSH	ES
		POP	DS
		POP	ES
		MOV	CL,DL
		DEC	CX
		DEC	CX
		PUSH	AX
		MOV	AX,CX
		CALL	ShrtLin
		POP	AX
		MOV	BX,StatusLine
		CALL	BasAdr
  @@PutStat1:	LODSB
		CMP	AL,0
		JNE	@@PutStat2
		DEC	SI
		MOV	AL,' '
  @@PutStat2:	STOSB
		INC	DI
		LOOP	@@PutStat1
		POP	CX
		POP	SI
		POP	DI
		POP	DS
		POP	ES

  @@PutName:	PUSH	DS			; Output name
		MOV	DS,WCBsegment
		MOV	AL,DS:LongNames
		POP	DS
		CALL	DirOffset1
  @@GetChar:	LODSB
		MOV	SS:[DI],AX
		INC	DI
		INC	DI
		LOOP	@@GetChar
		CMP	BYTE PTR DS:[SI],0
		JE	@@ChkOut
		MOV	AL,ES:Scrolls[1]
		MOV	SS:[DI-2],AL
  @@ChkOut:	POP	BX
		POP	CX
		INC	CX
		CMP	SI,TreeFrst
		JB	@@NextLine
		PUSH	ES			; Write line
		PUSH	CX
		MOV	DS,WCBsegment
		MOV	CL,DS:TreeOffset
		PUSH	SS
		POP	DS
		LEA	SI,LineBuffer
		ADD	SI,CX
		ADD	SI,CX
		MOV	CL,DL
		DEC	CX
		DEC	CX
		CALL	BasAdr
		REP	MOVSW
		POP	CX
		POP	ES
		DEC	CX
		INC	BH
  @@NextLine:	POP	SI			; Next line
		MOV	DS,ES:TreeSeg
		CALL	NextDirEntH
		INC	TreeCount
		LOOP	@@WriteLine
  @@Exit:	RET
PutTree		ENDP

		.CONST
PtIn12_S	DB	"^r^0L^r Bytes ^1C",0
PtIn1_S		DB	"Memory",0
PtIn2_S		DB	"Free",0
PtIn3_S		DB	"Volume in drive ^r^0E:^r ^1Phas no label/is /^r^2E",0
PtIn45_S	DB	"^r^0L^r ^2Ebytes ^3C on drive ^r^1E:",0
PtIn4_S		DB	"total",0
PtIn5_S		DB	"free",0
PtIn6_S		DB	"^r^1I^r file^1P/s/ use^1Ps// ^r^0L^r ^2Ebyte^0Q/s/ in",0
PtIn7_S		DB	"^0E",0
PtIn8_S		DB	" ^0E ",0
PtIn9_S		DB	"Directory information file is absent",0

		.DATA
PtIn_P		DB	Back_C, Title_C
Line1_D		DW	TmpBuf, PtIn1_S
Line2_D		DW	TmpBuf, PtIn2_S
Line3_D		DW	TmpBuf+8, TmpBuf+6, Volume
Line4_D		DW	TmpBuf, TmpBuf+8, TmpBuf+10, PtIn4_S
Line5_D		DW	TmpBuf, TmpBuf+8, TmpBuf+10, PtIn5_S

		.CODE	Subs
PutInf		PROC	FAR
		CMP	DS:WinTyp,2
		JNE	PutView
		MOV	BP,OFFSET PtIn_P
		CALL	BegBX
		CALL	SizDX
		SHR	DL,1
		ADD	BL,DL
		ADD	BH,3
		PUSH	DS			; Total memory
		PUSH	ES
		PUSH	ES
		PUSH	BX
		MOV	AH,52h
		INT	21h
		MOV	AX,ES:[BX-2]
  @@NextBlk:	MOV	ES,AX
		INC	AX
		ADD	AX,ES:[3]
		CMP	BYTE PTR ES:[0],'M'
		JE	@@NextBlk
		POP	BX
		POP	DS
		POP	ES
		MOV	DX,OFFSET Line1_D
		CALL	PtIn20
		MOV	AX,DS:FreeMem		; Free memory
		CMP	DS:DataLocation,2
		JAE	@@FreeMem
		CMP	DS:LoadFl,0
		JNE	@@FreeMem
		MOV	AX,DS:SizeDataSeg
  @@FreeMem:	MOV	DX,OFFSET Line2_D
		CALL	PtIn20
		MOV	DI,OFFSET TmpBuf+6	; Volume label
		MOV	AH,0
		MOV	AL,DS:Volume[0]
		INC	AX
		STOSW
		MOV	AL,DS:DosPath[0]
		STOSW
		MOV	SI,OFFSET PtIn3_S
		MOV	DX,OFFSET Line3_D
		CALL	PtIn23
		MOV	DX,OFFSET Line4_D	; Size of disk
		MOV	SI,OFFSET TotalClusters
		CALL	PtIn21
		MOV	DX,OFFSET Line5_D	; Disk size free
		MOV	SI,OFFSET FreeClusters
		CALL	PtIn21
		POP	DS			; Size of files in directory
		PUSH	DS
		CALL	InvWCB
		CMP	DS:Visible,0
		JZ	@@Dirinfo
		CMP	DS:WinTyp,0
		JNE	@@Dirinfo
		XOR	CX,CX
		MOV	WORD PTR ES:TmpBuf[0],CX
		MOV	WORD PTR ES:TmpBuf[2],CX
		MOV	WORD PTR ES:TmpBuf[4],CX
		MOV	WORD PTR ES:TmpBuf[6],CX
		MOV	SI,DS:DiskBuf
  @@NextFile:	CMP	SI,DS:EndBuf
		JAE	@@OutOfFiles
		TEST	DS:[SI].AttrF,10h
		JNZ	@@SkipEntry
		INC	CX
		MOV	AX,DS:[SI].SizeF.Lo
		MOV	DX,DS:[SI].SizeF.Hi
		XOR	DI,DI
		CMP	DS:ArcPnt,0
		JNE	@@AddSize
		CALL	FileClustSize
  @@AddSize:	ADD	WORD PTR ES:TmpBuf[0],AX
		ADC	WORD PTR ES:TmpBuf[2],DX
		ADC	WORD PTR ES:TmpBuf[4],DI
  @@SkipEntry:	CALL	NextFilEnt
		JMP	@@NextFile
  @@OutOfFiles:	MOV	WORD PTR ES:TmpBuf[8],CX
		MOV	SI,OFFSET PtIn6_S
		MOV	DX,OFFSET Line4_D
		CALL	PtIn22
		MOV	DI,OFFSET TmpBuf	; Path of panel
		PUSH	DI
		CALL	WinPath1
		CMP	DS:ArcPnt,0
		JE	@@MovePath
		MOV	SI,OFFSET ArcExt
		CALL	MvzLine
		MOV	AL,':'
		STOSB
		MOV	SI,DS:ArcPnt
		LODSW
		CALL	CutPath
		CALL	MvzLine
		MOV	AL,'\'
		STOSB
		MOV	SI,OFFSET ArcPath
  @@MovePath:	CALL	MvzLine
		POP	SI
		CALL	SizDX
		MOV	AL,DL
		MOV	AH,0
		SUB	AL,4
		CALL	ShrtLin
		MOV	SI,OFFSET PtIn7_S
		MOV	DX,OFFSET Line1_D
		CALL	PtIn23
		SUB	BH,2
  @@Dirinfo:	PUSH	ES			; DirInfo file name
		POP	DS
		CMP	DS:DirinfoSeg,0
		JNE	@@Title
		ADD	BH,4
		MOV	SI,OFFSET PtIn9_S
		CALL	PtIn23
		JMP	@@Exit
  @@Title:	MOV	SI,OFFSET DirInfoFile
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		CALL	MvzLine
		POP	DI
		CALL	StrLen
		ADD	CX,6-1
		CALL	SizDX
		MOV	DH,0
		CMP	CX,DX
		JBE	@@DirInfoNam
		ADD	DI,DX
		SUB	DI,6+3
		MOV	AX,'.'
		STOSB
		STOSB
		STOSW
  @@DirInfoNam:	ADD	BH,2
		INC	BP
		MOV	SI,OFFSET PtIn8_S
		MOV	DX,OFFSET Line1_D
		CALL	PtIn23
		POP	DS			; DirInfo contents
		PUSH	DS
		CALL	BegBX
		CALL	SizDX
		MOV	DS,ES:DirinfoSeg
		MOV	DS:VwStatus,0
		MOV	DS:VwSize.Y,0
		SUB	DH,12
		JBE	@@Exit
		DEC	DX
		DEC	DX
		ADD	BX,0B01h
		MOV	DS:VwBase,BX
		MOV	DS:VwSize,DX
		CALL	ViewStatus
		CALL	ViewScreen0
  @@Exit:	POP	DS
		JMP	ExitPutFls
PutInf		ENDP

PtIn20		PROC	NEAR
		PUSH	DX
		MOV	DX,10h
		MUL	DX
		MOV	DS:TmpBuf.Lo,AX
		MOV	DS:TmpBuf.Hi,DX
		POP	DX
		MOV	SI,OFFSET PtIn12_S
		JMP	PtIn23
PtIn21:		PUSH	BX
		MOV	BX,OFFSET ClusterSize
		MOV	DI,OFFSET TmpBuf
		CALL	LongMul
		POP	BX
		MOV	SI,OFFSET PtIn45_S
PtIn22:		MOV	DI,OFFSET TmpBuf
		CALL	ReduceNumb
		MOV	WORD PTR ES:TmpBuf[10],AX
PtIn23:		PUSH	DX
		PUSH	BX
		CALL	BegBX
		CALL	SizDX
		MOV	AL,BH
		ADD	AL,DH
		POP	BX
		POP	DX
		DEC	AX
		CMP	BH,AL
		JAE	@@Exit
		PUSH	DS
		PUSH	CS
		POP	DS
		CALL	CntLine
		POP	DS
		INC	BH
  @@Exit:	RET
PtIn20		ENDP

		.CONST
ViewAbsent_S	DB	'File is absent or error reading file',0
Directory_S	DB	'Directory',0
Archive_S	DB	'Archive',0
ByteShort_S	DB	'b',0
ViewDir1_S	DB	'^0C "^r^1E^r"',0
ViewDir2_S	DB	'Contains:',0
ViewDir3_S	DB	'Subdirectories',0FFh,4,'^r^2L',0
ViewDir4_S	DB	'Files',0FFh,13,'^r^3L',0
ViewDir5_S	DB	'Files size',0FFh,8,'^r^1L^r ^4E',0
ViewDir6_S	DB	'Real files size',0FFh,3,'^r^1L^r ^4E',0
ViewDir7_S	DB	'Cluster size',0FFh,6,'^r^5L',0
ViewDir8_S	DB	'Compressed size',0FFh,3,'^r^1L^r ^4E',0

		.DATA
ViewDir_T	DW	Directory_S, TmpBuf, QuickViewDirs, QuickViewFiles, TmpBuf+10, ClusterSize
ViewDir1_T	DW	Archive_S, TmpBuf

		.CODE	Subs
PutView		PROC	FAR
		CALL	BegBX
		CALL	SizDX
		ADD	BX,101h
		SUB	DX,402h
		CALL	InvWCB
		MOV	CL,DS:LongNames
		MOV	AX,ES:ViewSeg
		OR	AX,AX
		JE	@@Exit
		MOV	DS,AX
		TEST	DS:ViewAttr,1
		JNZ	@@Exit
		TEST	DS:ViewAttr,10h
		JNZ	@@PutDir
		TEST	DS:ViewAttr,8
		JNZ	@@Absent
		ADD	AX,(SIZE QuickViewData-1)/16+1 ; File
		MOV	DS,AX
		MOV	DS:VwBase,BX
		MOV	DS:VwSize,DX
		PUSH	ES
		PUSH	DS
		MOV	DS,ES:ViewSeg
		MOV	SI,OFFSET ViewName1
		OR	CL,CL
		JE	@@PutName1
		MOV	SI,OFFSET ViewName2
  @@PutName1:	CALL	CutPath
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		CALL	MvzLine
		POP	SI
		PUSH	ES
		POP	DS
		MOV	AL,DL
		MOV	AH,0
		MOV	CX,AX
		DEC	CX
		SUB	AL,5
		CALL	ShrtLin
		ADD	BH,DH
		INC	BH
		CALL	BasAdr
  @@PutName2:	LODSB
		CMP	AL,0
		JNE	@@PutName3
		DEC	SI
		MOV	AL,' '
  @@PutName3:	STOSB
		INC	DI
		LOOP	@@PutName2
		MOV	AL,'%'
		STOSB
		POP	DS
		POP	ES
		MOV	DS:VwStatus,OFFSET ViewStat1a
		CALL	ViewStatus
		CALL	ViewScreen0
		MOV	DS:VwStatus,OFFSET ViewStat1b
		JMP	@@Exit
  @@Absent:	PUSH	DS			; Absent file
		PUSH	BX
		PUSH	CS
		POP	DS
		MOV	BP,OFFSET PtIn_P
		MOV	SI,OFFSET ViewAbsent_S
		MOV	AL,DL
		SHR	AL,1
		ADD	BL,AL
		INC	BH
		CALL	CntLine
		POP	BX
		POP	DS
		JMP	@@Status
  @@PutDir:	PUSH	DX			; Directory
		PUSH	CX
		PUSH	BX
		MOV	BP,OFFSET ViewDir_T
		PUSH	BP
		PUSH	ES
		PUSH	CX
		PUSH	CS
		POP	ES
		MOV	DI,OFFSET Directory_S
		TEST	DS:ViewAttr,2
		JZ	@@CorLen
		MOV	SI,OFFSET ViewName1
		CALL	CutPath
		CMP	BYTE PTR DS:[SI],0
		JNE	@@CorLen
		MOV	DI,OFFSET Archive_S
		MOV	BP,OFFSET ViewDir1_T
  @@CorLen:	CALL	StrLen
		SUB	DL,CL
		SUB	DL,4
		POP	CX
		POP	ES
		CALL	PutVwName
		INC	BH
		MOV	DX,BP
		MOV	BP,OFFSET PtIn_P
		MOV	SI,OFFSET ViewDir1_S
		CALL	PutVwLine
		POP	DX
		INC	BH
		MOV	SI,OFFSET ViewDir2_S
		CALL	PutVwLine
		INC	BH
		MOV	SI,OFFSET ViewDir3_S
		CALL	PutVwLine
		MOV	SI,OFFSET ViewDir4_S
		CALL	PutVwLine
		MOV	SI,OFFSET QuickViewFlSize
		MOV	DI,OFFSET ViewDir5_S
		CALL	PutVwLin1
		MOV	SI,OFFSET QuickViewClSize
		TEST	DS:ViewAttr,2
		JNZ	@@ArcDir
		MOV	DI,OFFSET ViewDir6_S
		CALL	PutVwLin1
		MOV	SI,OFFSET ViewDir7_S
		CALL	PutVwLine
		JMP	@@ExitVwDir
  @@ArcDir:	CMP	ES:QuickViewComp,0
		JE	@@ExitVwDir
		MOV	DI,OFFSET ViewDir8_S
		CALL	PutVwLin1
  @@ExitVwDir:	POP	BX
		POP	CX
		POP	DX
  @@Status:	CALL	PutVwName		; Status line
		MOV	CX,AX
		ADD	BH,DH
		INC	BH
		PUSH	ES
		PUSH	ES
		POP	DS
		CALL	BasAdr
  @@GetChar:	LODSB
		CMP	AL,0
		JNE	@@PutChar
		DEC	SI
		MOV	AL,' '
  @@PutChar:	STOSB
		INC	DI
		LOOP	@@GetChar
		POP	ES
  @@Exit:	JMP	ExitPutFls
PutView		ENDP

PutVwLin1	PROC	NEAR
		PUSH	DS
		PUSH	DI
		PUSH	ES
		POP	DS
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		MOVSW
		MOVSW
		MOVSW
		MOVSW
		POP	DI
		CALL	ReduceNumb
		MOV	WORD PTR DS:TmpBuf[10],AX
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET ByteShort_S
		MOV	DI,OFFSET TmpBuf+11
		CALL	MvzLine
		POP	SI
		POP	DS
PutVwLin1	ENDP

PutVwLine	PROC	NEAR
		PUSH	DX
		PUSH	BX
		CALL	BegBX
		CALL	SizDX
		MOV	AL,BH
		ADD	AL,DH
		POP	BX
		POP	DX
		SUB	AL,3
		CMP	BH,AL
		JAE	@@Exit
		PUSH	DS
		PUSH	CS
		POP	DS
		INC	BX
		MOV	AX,36
		CALL	NgLine
		DEC	BX
		POP	DS
		INC	BH
  @@Exit:	RET
PutVwLine	ENDP

PutVwName	PROC	NEAR
		MOV	SI,OFFSET ViewName1
		TEST	DS:ViewAttr,2
		JNZ	@@CutPath
		OR	CL,CL
		JE	@@CutPath
		MOV	SI,OFFSET ViewName2
  @@CutPath:	MOV	DI,OFFSET TmpBuf
		PUSH	DI
		MOV	AX,SI
		CALL	CutPath
		CMP	BYTE PTR DS:[SI],0
		JNE	@@ShortName
		DEC	SI
		TEST	DS:ViewAttr,2
		JZ	@@ShortName
		MOV	BYTE PTR DS:[SI],0
		PUSH	SI
		MOV	SI,AX
		CALL	CutPath
		CALL	MvzLine
		POP	SI
		MOV	BYTE PTR DS:[SI],'\'
		INC	SI
  @@ShortName:	CALL	MvzLine
		POP	SI
		MOV	AL,DL
		MOV	AH,0
		CALL	ShrtLin
		RET
PutVwName	ENDP

;---------------------------------------------------------------
;	Get current path
;	Return:	   CY - if error
;---------------------------------------------------------------
GetPath		PROC	FAR
		USES	AX,CX,DX,SI,DI,DS
		PUSH	ES
		POP	DS
		MOV	AH,19h
		INT	21h
		MOV	DL,AL
		MOV	DI,OFFSET DosPath
		ADD	AL,'A'
		MOV	AH,':'
		STOSW
		PUSH	AX
		MOV	AX,'\'
		STOSW
		DEC	DI
		POP	AX
		INC	DX
		MOV	SI,DI
		MOV	DI,OFFSET DosShortPath
		CALL	Phantom
		JC	@@Error
		MOV	AH,47h
		CALL	Intr21
		JC	@@Error
		SUB	SI,3
		CMP	BYTE PTR DS:[SI+3],0
		JE	@@ShortPath
		MOV	DX,SI
		MOV	CX,37h
		MOV	AH,4Eh
		CALL	Intr21
		CALL	CloseFindFile
		JNC	@@ShortPath
  @@CutSubDir:	PUSH	SI
		CALL	CutPath
		MOV	AX,SI
		SUB	AX,DX
		CMP	AX,3
		JBE	@@SetNewDir
		DEC	SI
  @@SetNewDir:	MOV	BYTE PTR DS:[SI],0
		POP	SI
		MOV	AH,3Bh
		CALL	Intr21
		JNC	@@ShortPath
		CMP	AX,53h
		JE	@@Error
		CMP	BYTE PTR DS:[SI+3],0
		JNE	@@CutSubDir
		JMP	@@Error
  @@ShortPath:	CALL	ShortName
		JNC	@@LongPath
		CMP	AX,7100h
		JE	@@Exit
		JMP	@@Error
  @@LongPath:	MOV	AL,1
		CALL	RealPath
		MOV	AL,0
		JNC	@@Exit
  @@Error:	MOV	SI,OFFSET DosPath
		CALL	MvzLine
		MOV	AL,1
  @@Exit:	MOV	DS:PathEr,AL
		RET
GetPath		ENDP

;---------------------------------------------------------------
;	Save setup
;		ES:DX - ini file
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
SaveSet		PROC	FAR
		USES	BX,CX,DX,SI,DI,DS
		CALL	MakeIni
		PUSH	ES
		POP	DS
		MOV	BX,CX
		CALL	ChgDisk
		JC	@@Err1
		MOV	AX,4300h
		CALL	Intr21
		JNC	@@Open
		CMP	AX,2
		JNE	@@Error
		XOR	CX,CX
  @@Open:	OR	CL,20h
		MOV	AH,3Ch
		CALL	Intr21
		JC	@@Exit
		MOV	CX,BX
		MOV	BX,AX
		MOV	DX,OFFSET IniBuffer
		MOV	AH,40h
		CALL	Intr21
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	BX
		POP	DX
		JC	@@Exit
		MOV	AX,BX
		PUSH	DX
		POPF
		JC	@@Exit
		CMP	AX,CX
		JE	@@Exit
  @@Err1:	MOV	AL,0
  @@Error:	STC
  @@Exit:	RET
SaveSet		ENDP

MakeIni		PROC	FAR
		USES	AX,DI,DS
		MOV	DI,OFFSET IniBuffer
		PUSH	DI
		MOV	AL,'V'
		STOSB
		STOSB
		STOSB
		MOV	DS,ES:WCB1seg
		CALL	MkIni1
		MOV	DS,ES:WCB2seg
		CALL	MkIni1
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET StarTab
		MOV	CX,5
		CALL	FilZero
		MOV	SI,OFFSET Switches
		MOV	CX,LenSwtch+2
		CALL	FilHist
		MOV	SI,OFFSET DescrFiles
		MOV	CX,LenStr+2
		CALL	FilHist
		XOR	SI,SI
		MOV	CX,CmnIni
		REP	MOVSB
		POP	SI
		MOV	CX,DI
		SUB	CX,SI
		CALL	ChkSum
		STOSW
		INC	CX
		INC	CX
		RET
MakeIni		ENDP

MkIni1		PROC	NEAR
		MOV	SI,OFFSET DskMas
		MOV	CX,LenFilt+2
		CALL	FilHist
		MOV	SI,OFFSET WinShortPath
		MOV	CX,LenPath
		CALL	FilZero
		XOR	SI,SI
		MOV	CX,WCBini
		REP	MOVSB
		RET
MkIni1		ENDP

;---------------------------------------------------------------
;	Directory sizes
;		   DS - WCB
;		   AL - 0=all, 1=selected
;---------------------------------------------------------------
SumSize		EQU	DWORD PTR TmpBuf[100]

		.CODE
DirSize		PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,DS
		MOV	CL,AL
		MOV	ES:FncAddr,OFFSET DirSiz1
		MOV	SI,OFFSET WinLongPath
		MOV	DI,OFFSET LineBuf1
		CALL	MvzLine
		CALL	AddFile
		MOV	AL,0
		STOSB
		MOV	SI,DS:DiskBuf
		OR	CL,CL
		JZ	@@ChkEnd
		CALL	TotSel
		OR	BX,BX
		JNE	@@ChkEnd
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		JMP	@@GetSum
  @@ChkEnd:	CMP	SI,DS:EndFul
		JAE	@@Exit
		OR	CL,CL
		JZ	@@GetSum
		TEST	DS:[SI].AttrF,40h
		JZ	@@Next
  @@GetSum:	TEST	DS:[SI].AttrF,10h
		JZ	@@Next
		PUSH	SI
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		POP	SI
		JE	@@Next
		MOV	ES:SumSize.Lo,0
		MOV	ES:SumSize.Hi,0
		PUSH	SI
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET LineBuf1
		CALL	CutPath
		MOV	DI,SI
		POP	DS
		POP	SI
		PUSH	SI
		CALL	LongNameOffset
		CALL	MvzLine
		POP	SI
		MOV	CH,1
		XCHG	CH,ES:IncDirs
		MOV	AL,10h
		CALL	DirSiz1
		MOV	ES:IncDirs,CH
		OR	AH,AH
		JNZ	@@Exit
		MOV	AX,ES:SumSize.Lo
		MOV	BX,ES:SumSize.Hi
		MOV	DS:[SI].SizeF.Lo,AX
		MOV	DS:[SI].SizeF.Hi,BX
		OR	DS:[SI].AttrF,8
  @@Next:	CALL	NextFilEnt
		JMP	@@ChkEnd
  @@Exit:	RET
DirSize		ENDP

DirSiz1		PROC	NEAR
		USES	BX,CX,DX,SI,DI,DS
		TEST	AL,10h
		JZ	@@AddSize
		CALL	DirFunc
		CMP	AH,1
		JBE	@@Exit
		MOV	AH,0
		JMP	@@Exit
  @@AddSize:	MOV	AX,ES:DTA.FilSize.Lo
		MOV	BX,ES:DTA.FilSize.Hi
		ADD	AX,ES:SumSize.Lo
		ADC	BX,ES:SumSize.Hi
		JNC	@@SaveSize
		MOV	AX,0FFFFh
		MOV	BX,AX
  @@SaveSize:	MOV	ES:SumSize.Lo,AX
		MOV	ES:SumSize.Hi,BX
		XOR	AX,AX
  @@Exit:	RET
DirSiz1		ENDP

