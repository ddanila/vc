;---------------------------------------------------------------
;		Archive subroutines
;
;		Version:	22.06.2000
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Put archive pathname
;		   DS - WCB address
;		ES:DI - target buffer
;	Return:	ES:DI - end of name
;---------------------------------------------------------------
		.CODE	Subs
PutArcPathName	PROC	FAR
		USES	AX
		MOV	AL,1
		CALL	PutArcPathName1
		RET
PutArcPathName	ENDP

PutArcPathName1	PROC	FAR
		USES	AX,BX,DX,SI
		MOV	DL,AL
		MOV	BYTE PTR ES:[DI],0
		MOV	SI,DS:ArcPnt
		LODSW
		CMP	SI,OFFSET ArcFiles+2
		JB	@@Exit
		JA	@@TmpArc
		OR	DL,DL
		JE	@@MoveName
		PUSH	SI

;##
		MOV	SI,OFFSET WinShortPath
		CALL	strcpy
		JMP	@@AddPathSep
  @@TmpArc:	CALL	CutPath
		PUSH	SI
		MOV	SI,OFFSET VcTempArc_S
		CALL	MakePathName
  @@AddPathSep:	CALL	AddPathSep
		POP	SI
  @@MoveName:	CALL	strcpy
  @@Exit:	RET
PutArcPathName1	ENDP

;---------------------------------------------------------------
;	Put archive descriptor - ext:path\name\path
;		   DS - WCB segment
;		ES:DI - target buffer
;	Return:	ES:DI - end of buffer
;---------------------------------------------------------------
PutDescriptor	PROC	FAR
		USES	AX,SI
		MOV	SI,OFFSET ArcExt
		CALL	strcpy
		MOV	AL,':'
		STOSB
		CALL	PutArcPathName
		CALL	AddPathSep
		MOV	SI,OFFSET ArcPath
		CALL	strcpy
		RET
PutDescriptor	ENDP

;---------------------------------------------------------------
;	Test archive extension
;		DS:SI - archive pathname
;		   DL - 0=all extensions, 1=only main ext
;	Return:	   CX - numder of arc or pointer, ArchiveExt=Main ext
;		   CY - error
;---------------------------------------------------------------
		.CONST
InternExt	DB	'ZIP',0
		DB	0

		.CODE	Subs
ChkArcExt	PROC	FAR
		USES	SI,DI
		CALL	CutPath
		PUSH	DS			; Find in VCARCH.EXT
		PUSH	ES
		POP	DS
		POP	ES
		MOV	AL,DL
		INC	AX
		MOV	DI,SI
		MOV	SI,DS:ArchPoint
		CALL	FindExt
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		JC	@@SaveArcExt
		PUSH	DI			; Save main extension
		PUSH	SI
		MOV	SI,AX
		MOV	DI,OFFSET ArchiveExt
  @@ArcExt1:	MOV	BX,DI
  @@ArcExt2:	CMP	DI,OFFSET ArchiveExt+3
		JAE	@@ArcExt3
		LODS	BYTE PTR ES:[SI]
		CMP	AL,':'
		JE	@@ArcExt3
		CMP	AL,';'
		JE	@@ArcExt3
		CMP	AL,','
		JE	@@ArcExt3
		CMP	AL,1Ah
		JE	@@ArcExt3
		CMP	AL,9
		JE	@@ArcExt3
		CMP	AL,13
		JE	@@ArcExt3
		CMP	AL,10
		JE	@@ArcExt3
		STOSB
		CMP	AL,' '
		JE	@@ArcExt2
		JMP	@@ArcExt1
  @@ArcExt3:	MOV	BYTE PTR ES:[BX],0
		POP	SI
		POP	DI
		MOV	CX,SI			; Internal or external mode
		OR	DL,DL
		JNZ	@@ChkFile
		LODS	BYTE PTR ES:[SI]
		CMP	AL,'-'
		JNE	@@ChkFile
  @@IsDefault:	LODS	BYTE PTR ES:[SI]
		CMP	AL,' '
		JE	@@IsDefault
		CMP	AL,9
		JE	@@IsDefault
		CMP	AL,13
		JE	@@FindInList
		CMP	AL,10
		JE	@@FindInList
		CMP	AL,1Ah
		JE	@@FindInList
		JMP	@@ChkFile
  @@SaveArcExt:	PUSH	DI			; Save extension of archive
		MOV	SI,DI
		CALL	FindFileExt
		MOV	DI,OFFSET ArchiveExt
		MOV	CX,3
  @@MoveExt:	LODSB
		CALL	UpCase
		STOSB
		LOOP	@@MoveExt
		MOV	AL,0
		STOSB
		POP	DI
  @@FindInList:	MOV	BX,OFFSET InternExt	; Find extension in list
		XOR	CX,CX
  @@IntExt:	CMP	BYTE PTR CS:[BX],1
		JB	@@Exit
		MOV	SI,OFFSET ArchiveExt
  @@ExtChar:	LODS	BYTE PTR ES:[SI]
		CMP	AL,CS:[BX]
		JNE	@@NextExt
		CMP	AL,0
		JE	@@ChkFile
		INC	BX
		JMP	@@ExtChar
  @@NextExt:	MOV	AL,CS:[BX]
		INC	BX
		CMP	AL,0
		JNE	@@NextExt
		INC	CX
		JMP	@@IntExt
  @@ChkFile:	CLC
  @@Exit:	RET
ChkArcExt	ENDP

;---------------------------------------------------------------
;	Find archive command
;		DS:SI - numder of arc or pointer
;		   AL - number of command
;	Return:	   NC - Ok, DS:SI - pointer to command
;		   CY - error
;---------------------------------------------------------------
FindArcCmd	PROC	FAR
		USES	AX,CX
		CMP	SI,30
		JB	@@Error
		MOV	CL,AL
		MOV	CH,0
  @@NextCmd:	LODSB
		CMP	AL,1Ah
		JE	@@Error
		CMP	AL,13
		JE	@@EndLine
		CMP	AL,10
		JNE	@@NextCmd
  @@EndLine:	DEC	SI
		CALL	FindCommand
		JC	@@Error
		DEC	CX
		JNE	@@NextCmd
		MOV	CX,SI			; Check line for -
		LODSB
		CMP	AL,'-'
		JNE	@@CmdFound
  @@ChkNoCmd:	LODSB
		CMP	AL,13
		JE	@@Error
		CMP	AL,10
		JE	@@Error
		CMP	AL,1Ah
		JE	@@Error
		CMP	AL,' '
		JE	@@ChkNoCmd
		CMP	AL,9
		JE	@@ChkNoCmd
  @@CmdFound:	MOV	SI,CX
		OR	AL,AL
		JMP	@@Exit
  @@Error:	STC
  @@Exit:	RET
FindArcCmd	ENDP

;---------------------------------------------------------------
;	Test archive by extension and it is on a disk
;		DS:SI - archive pathname
;	Return:	   CX - numder of arc or pointer
;		  DTA - archive file parameters
;		   CY - error, AX - error code
;---------------------------------------------------------------
TestArchive	PROC	FAR
		USES	SI,DI
		MOV	DL,0
		CALL	ChkArcExt
		MOV	AX,0
		PUSH	CX
		JC	@@Exit
		MOV	AL,53h			; Find archive file
		CALL	ChgDisk
		JC	@@Exit
		MOV	DX,SI
		MOV	CX,27h
		MOV	AH,4Eh
		CALL	Intr21
		JC	@@Exit
		CALL	CloseFindFile
  @@Exit:	POP	CX
		RET
TestArchive	ENDP

;---------------------------------------------------------------
;	Parse archive name
;		ES:SI - archive descriptor
;	Return:	   CY - error, AX - error code
;		   NC - Ok
;		ES:DX - archive name
;		ES:SI - path in archive
;		   CX - number of arc or pointer
;		   AX - 0=found, 1=new archive
;---------------------------------------------------------------
ParseArcName	PROC	FAR
		USES	BX,DI,BP,DS
		PUSH	ES
		POP	DS
		MOV	DI,OFFSET TmpErr
		PUSH	DI
		MOV	AX,'.A'
		STOSW
		XOR	AX,AX
  @@MoveDscr:	LODSB
		CMP	AL,0
		JE	@@Error
		STOSB
		CMP	AL,':'
		JNE	@@MoveDscr
		DEC	DI
		MOV	AL,0
		STOSB
		MOV	AX,SI
		POP	SI
		MOV	BP,DS:[SI+2]
		PUSH	AX
		MOV	DL,1
		CALL	ChkArcExt
		MOV	AX,0
		POP	DX
		PUSH	CX
		JC	@@Error
		MOV	DI,DX
		MOV	CH,1
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		MOV	SI,DI
		CALL	ChgDisk
		MOV	AX,53h
		JC	@@Exit
  @@SplitPath:	MOV	BL,0
		XCHG	BL,DS:[SI]
		MOV	AX,4300h
		CALL	Intr21
		MOV	DS:[SI],BL
		JC	@@NotFound
		XOR	AX,AX
		TEST	CL,10h
		JZ	@@ChkOneLet
		OR	BL,BL
		JZ	@@Error
  @@NewArc:	MOV	SI,DI
		MOV	AX,1
  @@ChkOneLet:	XCHG	AX,BP
		OR	AH,AH
		JNE	@@ExitArc
		CALL	UpCase
		CMP	AL,'A'
		JB	@@ExitArc
		CMP	AL,'Z'
		JA	@@ExitArc
		MOV	BL,AL
		MOV	AX,ES:[SI-2]
		CMP	AL,'.'
		JNE	@@ExitFile
		MOV	AL,AH
		CALL	UpCase
		CMP	AL,BL
		JE	@@ExitArc
  @@ExitFile:	XOR	AX,AX
		JMP	@@Error
  @@ExitArc:	MOV	AX,BP
		JMP	@@Exit
  @@NotFound:	CMP	AX,53h
		JE	@@Error
		MOV	DI,SI
		MOV	SI,DX
		MOV	BL,0
		XCHG	BL,DS:[DI]
		CALL	CutPath
		MOV	DS:[DI],BL
		CMP	DX,SI
		JAE	@@NewArc
		DEC	SI
		CMP	DX,SI
		JAE	@@NewArc
		CMP	BYTE PTR DS:[SI],':'
		JE	@@NewArc
		CMP	BYTE PTR DS:[SI-1],':'
		JE	@@NewArc
		JMP	@@SplitPath
  @@Error:	STC
  @@Exit:	POP	CX
		RET
ParseArcName	ENDP

;---------------------------------------------------------------
;	Extract file for view/edit
;		DS:SI - descriptor
;		   DX - pointers table (Title, Archive, ExecArcProgram)
;	Return:	   NC - no error, AX=0FFFFh - not archive, AX=N - archive
;		   CY - error, AX - error box or 0
;---------------------------------------------------------------
VwArcLine1	EQU	BYTE PTR HelpBuf+0
VwArcFile	EQU	BYTE PTR HelpBuf+512
VwArcTmp	EQU	BYTE PTR HelpBuf+1024

		.CONST
VcTempArc_S	DB	'VC',0
ExtractError_S	DB	"Can't extract file from archive",0

		.DATA
ExtractError_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	ExtractError_S
		DW	Line1_S
		DW	Ok_S
		DW	0
ExtractError_B	BOXHD	< ExtractError_P, 900h, 0, Dialog_K, Def_M, Default_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0

		.CODE
ViewArcExtract	PROC	FAR
		USES	DX,SI,BP,DS
		MOV	DI,DX
		MOV	ES:ArcPathName,0
		CALL	ParseArcName
		JC	@@NoArc
		OR	AX,AX
		MOV	AX,OFFSET FndEr_B
		JNZ	@@Error
		MOV	AX,OFFSET NotImpl_B
		CMP	CX,30
		JB	@@Error
		PUSH	ES
		POP	DS
		MOV	DS:ArcPathName,DX
		MOV	DS:ArcPathIn,SI
		MOV	DS:ArcCmdPoint,CX
		MOV	BYTE PTR DS:[SI],0
		INC	SI
		PUSH	DX
		PUSH	SI
		PUSH	DI
		PUSH	CX
		MOV	BP,OFFSET ExecArcArcPath
		MOV	DI,BP
		MOV	CX,LenPath-1
		PUSH	DI
		REP	MOVSB
		MOV	AL,0
		STOSB
		POP	SI
		CALL	CutPath
		CMP	SI,BP
		JBE	@@ExecArc
		DEC	SI
  @@ExecArc:	MOV	BYTE PTR DS:[SI],0
		MOV	DX,OFFSET TempDir
		MOV	DI,DX
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		MOV	BX,DI
		MOV	SI,OFFSET VcTempArc_S
		XOR	AX,AX
		CALL	MakeFileName
		CALL	ChgDisk
		MOV	AX,53h
		JC	@@ErrDir
  @@Create:	MOV	AH,39h
		CALL	Intr21
		JNC	@@Extract
		CMP	AX,5
		JNE	@@ErrDir
		MOV	SI,DI
		MOV	CX,3
  @@Next:	DEC	SI
		INC	BYTE PTR DS:[SI]
		CMP	BYTE PTR DS:[SI],'9'
		JBE	@@Create
		MOV	BYTE PTR DS:[SI],'0'
		LOOP	@@Next
  @@ErrDir:	MOV	BYTE PTR DS:[BX],0
		POP	DX
		POP	CX
		POP	SI
		POP	DI
		CMP	AX,53h
		JE	@@Error1
  @@ExtractErr:	MOV	AX,OFFSET ExtractError_B
		JMP	@@Error
  @@Extract:	MOV	AX,'\'
		STOSW
		MOV	AX,BX
		MOV	BX,DX
		POP	CX
		POP	DX
		POP	SI
		POP	DI
		PUSH	AX
		MOV	AX,2
		CALL	ExecArcCmd
		MOV	BP,AX
		PUSHF
		PUSH	SI
		MOV	SI,OFFSET TempDir
		MOV	DI,OFFSET VwArcFile
		CALL	strcpy
		POP	SI
		DEC	SI
		MOV	BYTE PTR DS:[SI],'\'
		CALL	CutPath
		CALL	strcpy
		POPF
		POP	DI
		LEA	SI,[DI+3]
		MOV	CX,0
		MOV	BYTE PTR DS:[SI+3],CL
		JNC	@@RmDir
		CMP	BP,53h
		JE	@@Error1
		INC	CX
  @@RmDir:	PUSH	DI
		PUSH	SI
		PUSH	CX
		MOV	SI,OFFSET LineBuf1
		MOV	DI,OFFSET VwArcLine1
		MOV	CX,512
		PUSH	SI
		PUSH	DI
		PUSH	CX
		PUSH	SI
		REP	MOVSB
		POP	DI
		MOV	SI,BX
		CALL	strcpy
		MOV	DI,OFFSET TmpBuf
		MOV	CX,256
		REP	STOSB
		MOV	DS:GroupMask[0],AL
		MOV	DL,1
		XCHG	DL,DS:IncDirs
		MOV	DS:FncAddr,OFFSET ViewArcFunc
		MOV	AL,10h
		CALL	DS:FncAddr
		MOV	DS:IncDirs,DL
		POP	CX
		POP	SI
		POP	DI
		REP	MOVSB
		POP	CX
		POP	SI
		POP	DI
		MOV	BYTE PTR DS:[DI],0
		OR	AH,AH
		JNE	@@Error1
		MOV	CH,AL
		OR	CL,CL
		JNZ	@@Error1
		CALL	TxtNum
		OR	CH,CH
		JE	@@Exit
		OR	BP,BP
		JE	@@ExtractErr
		JMP	@@Error1
  @@NoArc:	OR	AX,AX
		JNE	@@Error1
		DEC	AX
		JMP	@@Exit
  @@Error1:	XOR	AX,AX
  @@Error:	STC
  @@Exit:	RET
ViewArcExtract	ENDP

ViewArcFunc	PROC	NEAR
		USES	BX,CX,DX,SI,DI,BP
		MOV	BL,AL
		MOV	DX,OFFSET LineBuf1
		CALL	ChgDisk
		JC	@@Error
		MOV	CX,20h
		MOV	AX,4301h
		CALL	Intr21
		JC	@@ChkErr
		TEST	BL,10h
		JNZ	@@Dir
		MOV	DI,OFFSET VwArcFile
		MOV	AH,56h
		CALL	Intr21
		JNC	@@Done
		CMP	AX,53h
		JE	@@Error
		MOV	SI,DI
		MOV	DI,OFFSET VwArcTmp
		MOV	AH,60h
		CALL	Intr21
		JNC	@@Compare
		CMP	AX,53h
		JE	@@Error
		JMP	@@Del
  @@Compare:	PUSH	SI
		CALL	CutPath
		XCHG	SI,DI
		CALL	CutPath
		CALL	strcpy
		POP	SI
		MOV	DI,DX
		CALL	CompNormLines
		JE	@@Done
  @@Del:	MOV	AH,41h
		CALL	Intr21
		JC	@@ChkErr
  @@Done:	MOV	AX,1
		JMP	@@Exit
  @@Dir:	CALL	DirFunc
		OR	AH,AH
		JNE	@@Error
		MOV	DX,OFFSET LineBuf1
		MOV	AH,3Ah
		CALL	Intr21
		JNC	@@Done
  @@ChkErr:	CMP	AX,53h
		JE	@@Error
		XOR	AX,AX
		JMP	@@Exit
  @@Error:	MOV	AX,100h
  @@Exit:	RET
ViewArcFunc	ENDP

;---------------------------------------------------------------
;	Delete temporary file and directory
;		DS:SI - file
;		   AX - number of directory
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
		.CODE	Subs
DelTempArcFile	PROC	FAR
		USES	BX,CX,DX,SI,DI,DS
		CMP	AX,0FFFFh
		JE	@@Exit
		PUSH	SI
		MOV	DX,OFFSET TmpBuf
		MOV	SI,OFFSET VcTempArc_S
		MOV	DI,DX
		CALL	MakePathName
		MOV	BX,DI
		CALL	AddPathSep
		POP	SI
		CALL	CutPath
		CALL	strcpy
		PUSH	ES
		POP	DS
		CALL	ChgDisk
		MOV	AX,53h
		JC	@@Exit
		MOV	AH,41h
		CALL	Intr21
		JC	@@Exit
		MOV	BYTE PTR DS:[BX],0
		MOV	AH,3Ah
		CALL	Intr21
  @@Exit:	RET
DelTempArcFile	ENDP

;---------------------------------------------------------------
;	Delete temporary file and directory of Quick View
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
		.CODE
DelTempQuickVw	PROC	NEAR
		MOV	AX,ES:ViewSeg
		OR	AX,AX
		JE	@@Exit
		MOV	DS,AX
		MOV	AX,DS:ViewTempNumb
		MOV	SI,OFFSET ViewName1
		CALL	DelTempArcFile
  @@Exit:	RET
DelTempQuickVw	ENDP

;---------------------------------------------------------------
;	Delete all temporary archives
;		   DS - WCB segment
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
DelAllTempArcs	PROC	FAR
		USES	SI
		CMP	DS:ArcPnt,0
		JE	@@Exit
		MOV	SI,OFFSET ArcFiles+2
  @@FindNext:	LODSB
		CMP	AL,0
		JNE	@@FindNext
		CMP	SI,DS:ArcPnt
		LODSW
		JA	@@Exit
		CALL	DelTempArcFile
		JNC	@@FindNext
		CMP	AX,53h
		JNE	@@FindNext
		STC
  @@Exit:	RET
DelAllTempArcs	ENDP

;---------------------------------------------------------------
;	Detect type of archive
;		DS:SI - archive name
;		   BP - limit in WorkBuffer
;	Return:	   NC - Ok, CX - numder of arc or pointer
;			    ArchiveExt filled with extension
;		   CY - error, AX - error code
;---------------------------------------------------------------
		.CODE	Subs
DetectArc	PROC	FAR
		USES	SI,DS
		MOV	DX,SI			; Open archive file
		MOV	AX,53h
		CALL	ChgDisk
		JC	@@Exit
		MOV	AL,40h
		CALL	OpenFil
		JC	@@Exit
		MOV	BX,AX
		MOV	AX,5700h
		CALL	Intr21
		JC	@@Error
		MOV	ES:DTA.FilDate,DX
		MOV	ES:DTA.FilTime,CX
		MOV	ES:DTA.FilSize.Lo,1
		MOV	DS,ES:WorkBuffer

		CALL	ReadZipHdr		; Check for ZIP
		MOV	CX,0
		JNC	@@SetExt
		OR	AX,AX
		JNE	@@Error

; Next check

		JMP	@@Error

  @@SetExt:	MOV	AH,3Eh
		CALL	Intr21
		JC	@@Exit
		MOV	DI,OFFSET InternExt
		JCXZ	@@MoveExt
		PUSH	ES 
		PUSH	CS
		POP	ES
		MOV	AL,0
  @@FndExt:	PUSH	CX
		MOV	CH,1
		REPNE	SCASB
		POP	CX
		LOOP	@@FndExt
		POP	ES
  @@MoveExt:	PUSH	DS
		PUSH	CS
		POP	DS
		MOV	SI,DI
		MOV	DI,OFFSET TmpErr
		PUSH	DI
		MOV	AX,'.A'
		STOSW
		CALL	strcpy
		PUSH	ES
		POP	DS
		POP	SI
		MOV	DL,0
		CALL	ChkArcExt
		POP	DS
		MOV	AX,0
		JMP	@@Exit
  @@Error:	MOV	CX,AX
		MOV	AH,3Eh
		CALL	Intr21
		JC	@@Exit
		MOV	AX,CX
		STC
  @@Exit:	RET
DetectArc	ENDP

;---------------------------------------------------------------
;	Read end of central dir rec
;		DS:DI - buffer
;		   BX - file handle
;		   BP - limit
;	Return:	   NC - Ok, DS:AX - end of central dir rec
;			    DX SI - offset in file
;		   CY - error, AX - error code, 0=error in archive
;---------------------------------------------------------------
ReadZipHdr	PROC	NEAR
		USES	ES
		MOV	AX,0FFFFh
		MOV	ES,AX
		XOR	CX,CX
		XOR	DX,DX
		MOV	AX,4202h
		CALL	Intr21
		MOV	SI,AX
  @@EndHdr1:	MOV	CX,BP			; Read
		SUB	CX,DI
		CMP	CX,1000h
		JBE	@@EndHdr2
		MOV	CX,1000h
  @@EndHdr2:	OR	DX,DX
		JNE	@@EndHdr3
		CMP	CX,SI
		JBE	@@EndHdr3
		MOV	CX,SI
  @@EndHdr3:	SUB	CX,SIZE ZipEndHd
		JB	@@Broken
		MOV	AX,ES
		CMP	CX,AX
		JBE	@@EndHdr4
		MOV	CX,AX
  @@EndHdr4:	ADD	CX,SIZE ZipEndHd
		SUB	SI,CX
		SBB	DX,0
		PUSH	DX
		PUSH	CX
		MOV	CX,DX
		MOV	DX,SI
		MOV	AX,4200h
		CALL	Intr21
		POP	CX
		MOV	DX,DI
		MOV	AH,3Fh
		CALL	Intr21
		POP	DX
		JC	@@Error
		CMP	AX,CX
		MOV	AL,1
		JNE	@@Error
		PUSH	ES			; Find signature
		PUSH	DS
		PUSH	DI
		PUSH	SI
		PUSH	CS
		PUSH	DS
		POP	ES
		POP	DS
		SUB	CX,SIZE ZipEndHd
		PUSH	CX
		ADD	DI,CX
		INC	CX
  @@Compare:	MOV	SI,OFFSET ZipEnd_S
		MOV	AL,DS:[SI]
		STD
		REPNE	SCASB
		CLD
		JNE	@@CompExit
		PUSH	DI
		PUSH	CX
		INC	DI
		MOV	AX,DI
		MOV	CX,4
		REPE	CMPSB
		POP	CX
		POP	DI
		JE	@@CompExit
		CMP	CX,1
		JAE	@@Compare
  @@CompExit:	POP	CX
		POP	SI
		POP	DI
		POP	DS
		POP	ES
		JNE	@@NotFound
		PUSH	AX
		SUB	AX,DI
		ADD	SI,AX
		ADC	DX,0
		POP	AX
		JMP	@@Exit
  @@NotFound:	MOV	AX,SI
		OR	AX,DX
		JE	@@Broken
		ADD	SI,SIZE ZipEndHd
		ADC	DX,0
		MOV	AX,ES
		SUB	AX,CX
		MOV	ES,AX
		JA	@@EndHdr1
  @@Broken:	XOR	AX,AX
  @@Error:	STC
  @@Exit:	RET
ReadZipHdr	ENDP

;---------------------------------------------------------------
;	List archive directory
;	     LineBuf2 - archive
;		   AX - procedure
;		   CX - pointer to parser line or numb of archive
;		DS:DI - offset of buffer (in WorkBuffer)
;		   BP - limit of buffer
;	Return:	   DI - offset of buffer
;		   NC - no errors
;		   CY - error, AX=0 - no archive
;		   CY - error, AX - code
;		   CY - error, AX - dialog
;---------------------------------------------------------------
		.CONST
ArcErr_S	DB	'^1C archive',0
ArcErr0_S	DB	"Illegal descriptor of",0
ArcErr1_S	DB	"Can't read the",0
ArcErr2_S	DB	"Error in",0

		.DATA
ArcErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Error_S
		DW	ArcErr_S
		DW	Line0_S
		DW	Ok_S
		DW	0
ArcErr_B	BOXHD	< ArcErr_P, 900h, 0, Error_K, Def_M, None_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
ArcErr0_T	DW	LineBuf2, ArcErr0_S
ArcErr1_T	DW	LineBuf2, ArcErr1_S
ArcErr2_T	DW	LineBuf2, ArcErr2_S
ArcList_T	DW	Error_S, LineBuf2, ExecArcProgram

		.CODE	Subs
ReadArcDir	PROC	NEAR
		PUSH	ES:WorkBuffer
		MOV	ES:ExecArcDirProc,AX
		MOV	ES:ExecArcDirBuf,DI
		MOV	ES:ExecArcDirEnd,BP
		MOV	ES:ExecArcOverflow,0
		MOV	ES:ExecArcNumbF,0
		MOV	BX,BP
		PUSH	CX
		MOV	CL,4
		SHR	BX,CL
		POP	CX
		ADD	ES:WorkBuffer,BX
		CMP	CX,30
		JB	@@Zip
		PUSH	DS			; External support
		PUSH	BP
		PUSH	CX
		PUSH	ES
		POP	DS
		MOV	SI,CX
		MOV	DI,OFFSET LineBuf1
  @@Script01:	XOR	DX,DX
		XOR	BP,BP
  @@Script02:	CALL	FndLet
		LODSB
		CMP	AL,13
		JE	@@Script08
		CMP	AL,10
		JE	@@Script08
		CMP	AL,1Ah
		JE	@@Script08
		CALL	UpCase
		MOV	AH,1
		CMP	AL,';'
		JE	@@Script09
		MOV	AH,2
		CMP	AL,'L'
		JE	@@Script09
		MOV	BX,0001h
		MOV	AH,3
		CMP	AL,'N'
		JE	@@Script10
		MOV	BX,0002h
		MOV	AH,4
		CMP	AL,'P'
		JE	@@Script10
		MOV	BX,0004h
		MOV	AH,5
		CMP	AL,'S'
		JE	@@Script10
		MOV	BX,0008h
		MOV	AH,6
		CMP	AL,'C'
		JE	@@Script10
		MOV	BX,0010h
		MOV	AH,7
		CMP	AL,'A'
		JE	@@Script10
		MOV	BX,0020h
		MOV	AH,8
		CMP	AL,'D'
		JE	@@Script07
		MOV	BX,0040h
		MOV	AH,10
		CMP	AL,'M'
		JE	@@Script10
		MOV	BX,0080h
		MOV	AH,11
		CMP	AL,'Y'
		JE	@@Script10
		MOV	BX,0100h
		MOV	AH,12
		CMP	AL,'T'
		JE	@@Script10
		MOV	BX,0200h
		MOV	AH,13
		CMP	AL,'\'
		JE	@@Script10
		CMP	AL,'"'
		JE	@@Script04
		CMP	AL,"'"
		JE	@@Script04
		MOV	AH,0FFh
		CMP	AL,'*'
		JE	@@Script09
		CMP	AL,'A'
		JB	@@Script03
		CMP	AL,'Z'
		JBE	@@Script09
  @@Script03:	CMP	DI,OFFSET LineBuf1+128-3
		JAE	@@Script12
		PUSH	AX
		MOV	AX,100h+14
		STOSW
		POP	AX
		STOSB
		JMP	@@Script02
  @@Script04:	CMP	DI,OFFSET LineBuf1+128-3
		JAE	@@Script12
		MOV	CL,AL
		MOV	AL,14
		STOSB
		MOV	BX,DI
		INC	DI
		CALL	FndLet
  @@Script05:	LODSB
		CMP	AL,13
		JE	@@Script12
		CMP	AL,10
		JE	@@Script12
		CMP	AL,1Ah
		JE	@@Script12
		CMP	AL,CL
		JE	@@Script06
		CMP	DI,OFFSET LineBuf1+128
		JAE	@@Script12
		STOSB
		JMP	@@Script05
  @@Script06:	MOV	AX,DI
		SUB	AX,BX
		DEC	AX
		JE	@@Script12
		MOV	DS:[BX],AL
		JMP	@@Script02
  @@Script07:	MOV	BP,DI
		JMP	@@Script10
  @@Script08:	MOV	AH,0
  @@Script09:	XOR	BX,BX
  @@Script10:	CMP	DI,OFFSET LineBuf1+128
		JAE	@@Script12
		MOV	AL,AH
		STOSB
		TEST	DX,BX
		JNZ	@@Script12
		OR	DX,BX
		CMP	AL,1
		JA	@@Script02
		TEST	DX,0C0h
		JZ	@@Script11
		OR	BP,BP
		JE	@@Script11
		MOV	BYTE PTR DS:[BP],9
  @@Script11:	CMP	AL,0
		JNE	@@Script01
		JMP	@@Script13
  @@Script12:	POP	CX
		POP	BP
		POP	DS
		MOV	DX,OFFSET ArcErr0_T
		JMP	@@ArcErr
  @@Script13:	POP	CX
		POP	BP
		POP	DS
		XOR	BX,BX
		XOR	SI,SI
		MOV	DI,OFFSET LineBuf2
		MOV	DX,OFFSET ArcList_T
		MOV	AX,1
		CALL	ExecArcCmd
		JMP	@@ExitOk

		.CONST
ZipFile_S	DB	'PK',1,2
ZipEnd_S	DB	'PK',5,6
ZipMulti_S	DB	'PK',7,8
ZipVC_S		DB	' VcZip ',0
ZipLast_S	DB	'Insert the ^uLAST^u disk of the backup set',0
ZipDisk_S	DB	'Insert disk ^u^1I^u of the backup set',0

		.DATA
ZipLast_P	DB	Err_C, ErInv_C, ErInv_C, ErBr_C
		DW	ZipVC_S
		DW	ZipLast_S
		DW	Line0_S
		DW	OkCan_S
		DW	0
ZipLast_B	BOXHD	< ZipLast_P, 900h, 0, Error_K, Def_M, None_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
ZipDisk_P	DB	Err_C, ErInv_C, ErInv_C, ErBr_C
		DW	ZipVC_S
		DW	ZipDisk_S
		DW	Line0_S
		DW	OkCan_S
		DW	0
ZipDisk_B	BOXHD	< ZipDisk_P, 900h, 0, Error_K, Def_M, None_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
ZipDisk_T	DW	LineBuf2, TmpBuf+200

		.CODE	Subs
  @@Zip:	CMP	CL,1			; ZIP
		JAE	@@GoArj
		XOR	SI,SI
  @@ZipRead:	PUSH	DS			; Open archive
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET LineBuf2
		MOV	AL,40h
		CALL	OpenFil
		POP	DS
		JC	@@ArcRdErr3
		MOV	BX,AX
		PUSH	SI			; Read end of central dir rec
		CALL	ReadZipHdr
		POP	CX
		JNC	@@ZipMoveHdr
		OR	AX,AX
		JNE	@@ArcRdErr2
		MOV	SI,CX
		OR	SI,SI
		JNE	@@ZipLastVol
		XOR	CX,CX			; Check multi volume
		XOR	DX,DX
		MOV	AX,4200h
		CALL	Intr21
		JC	@@ArcRdErr2
		MOV	DX,DI
		MOV	CX,4
		MOV	AH,3Fh
		CALL	Intr21
		JC	@@ArcRdErr2
		CMP	AX,CX
		JNE	@@Broken
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	CS
		POP	ES
		MOV	SI,DI
		MOV	DI,OFFSET ZipMulti_S
		REPE	CMPSB
		POP	SI
		POP	DI
		POP	ES
		JNE	@@Broken
		MOV	SI,1
  @@ZipLastVol:	MOV	AH,3Eh			; Insert last volume
		CALL	Intr21
		JC	@@ArcRdErr3
		PUSH	BP
		MOV	BP,OFFSET ZipLast_B
		MOV	DX,OFFSET ArcErr1_T
		CALL	Dialog
		POP	BP
		JE	@@ZipRead
		JMP	@@ExitOk
  @@ZipMoveHdr:	PUSH	ES			; Move end of central dir rec
		PUSH	DI
		PUSH	SI
		PUSH	DS
		POP	ES
		MOV	SI,AX
		MOV	CX,SIZE ZipEndHd
		SUB	BP,CX
		SUB	BP,8
		MOV	ES:ExecArcDirEnd,BP
		SUB	ES:ExecArcDirEnd,256
		MOV	DI,BP
		REP	MOVSB
		POP	SI
		MOV	AX,SI
		STOSW
		MOV	AX,DX
		STOSW
		POP	DI
		POP	ES

		MOV	AX,DS:[BP].ZipStartDsk	; Read files headers
		CMP	AX,DS:[BP].ZipLastDsk
		JNE	@@ZipDir00
		SUB	SI,DS:[BP].ZipDirSize.Lo
		SBB	DX,DS:[BP].ZipDirSize.Hi
		MOV	DS:[BP].ZipDirOffset.Lo,SI
		MOV	DS:[BP].ZipDirOffset.Hi,DX
		JMP	@@ZipDir01
  @@ZipDir00:	MOV	AX,DS:[BP].ZipDirSize.Lo
		MOV	CX,DS:[BP].ZipDirSize.Hi
		SUB	AX,DS:[BP+SIZE ZipEndHd].Lo
		SBB	CX,DS:[BP+SIZE ZipEndHd].Hi
		XCHG	AX,DS:[BP+SIZE ZipEndHd].Lo
		XCHG	CX,DS:[BP+SIZE ZipEndHd].Hi
		MOV	DS:[BP].ZipDirSize.Lo,AX
		MOV	DS:[BP].ZipDirSize.Hi,CX
		XOR	AX,AX
		XOR	CX,CX
		XCHG	AX,DS:[BP].ZipDirOffset.Lo
		XCHG	CX,DS:[BP].ZipDirOffset.Hi
		MOV	DS:[BP+SIZE ZipEndHd+4].Lo,AX
		MOV	DS:[BP+SIZE ZipEndHd+4].Hi,CX
		MOV	AX,DS:[BP].ZipFlsTot
		SUB	AX,DS:[BP].ZipFlsDsk
		INC	AX
		MOV	DS:[BP+2],AX
  @@ZipDir01:	MOV	DX,DS:[BP].ZipDirOffset.Lo
		MOV	CX,DS:[BP].ZipDirOffset.Hi
		MOV	AX,4200h
		CALL	Intr21
		LEA	DX,[DI+SIZE FilEnt]
		MOV	CX,BP
		SUB	CX,DX
		CMP	DS:[BP].ZipDirSize.Hi,0
		JNE	@@ZipDir02
		CMP	CX,DS:[BP].ZipDirSize.Lo
		JBE	@@ZipDir02
		MOV	CX,DS:[BP].ZipDirSize.Lo
  @@ZipDir02:	OR	CX,CX
		JE	@@ZipDir23
		MOV	AH,3Fh
		CALL	Intr21
		JC	@@ArcRdErr2
		OR	AX,AX
		JE	@@ZipDir22
		SUB	DS:[BP].ZipDirSize.Lo,AX
		SBB	DS:[BP].ZipDirSize.Hi,0
		ADD	DS:[BP].ZipDirOffset.Lo,AX
		ADC	DS:[BP].ZipDirOffset.Hi,0

		MOV	CX,AX			; Convert to entries
		MOV	SI,DX
		MOV	DL,0
  @@ZipDir10:	MOV	AX,CX
		SUB	AX,OFFSET ZipName
		JB	@@ZipDir20
		CMP	AX,DS:[SI].ZipNameLen
		JB	@@ZipDir20
		PUSH	ES			; Check header sign
		PUSH	DI
		PUSH	SI
		PUSH	CX
		PUSH	CS
		POP	ES
		MOV	DI,OFFSET ZipFile_S
		MOV	CX,4
		REPE	CMPSB
		POP	CX
		POP	SI
		POP	DI
		POP	ES
		JNE	@@Broken
		MOV	AX,DS:[SI].ZipAttr.Lo	; Fill attributes
		MOV	ES:ExecArcEntry.AttrF,AL
		MOV	AX,DS:[SI].ZipTime
		MOV	ES:ExecArcEntry.TimeF,AX
		MOV	AX,DS:[SI].ZipDate
		MOV	ES:ExecArcEntry.DateF,AX
		MOV	AX,DS:[SI].ZipSize.Lo
		MOV	ES:ExecArcEntry.SizeF.Lo,AX
		MOV	AX,DS:[SI].ZipSize.Hi
		MOV	ES:ExecArcEntry.SizeF.Hi,AX
		MOV	AX,DS:[SI].ZipComp.Lo
		MOV	ES:ExecArcCompSize.Lo,AX
		MOV	AX,DS:[SI].ZipComp.Hi
		MOV	ES:ExecArcCompSize.Hi,AX
		MOV	AX,DS:[BP+2]
		INC	WORD PTR DS:[BP+2]
		MOV	ES:ExecArcNumbF,AX
		MOV	ES:ExecArcEntry.DescrF,0
		MOV	AX,OFFSET ZipName	; Fill path and name
  		XOR	DX,DX
		ADD	AX,DS:[SI].ZipNameLen
		ADC	DX,0
		ADD	AX,DS:[SI].ZipExtraLen
		ADC	DX,0
		ADD	AX,DS:[SI].ZipCommLen
		ADC	DX,0
		PUSH	DX
		PUSH	AX
		MOV	AX,DS:[SI].ZipNameLen
		ADD	AX,OFFSET ExecArcNameBuf+1
		CMP	AX,OFFSET ExecArcNameEnd
		JA	@@ZipDir11
		PUSH	SI
		PUSH	CX
		MOV	CX,DS:[SI].ZipNameLen
		ADD	SI,OFFSET ZipName
		MOV	DI,OFFSET ExecArcNameBuf
		REP	MOVSB
		MOV	AL,0
		STOSB
		PUSH	DS			; Set entry
		PUSH	ES
		POP	DS
		POP	ES
		PUSH	BX
		CALL	ExecArcSetEntry
		POP	BX
		MOV	DI,DS:ExecArcDirBuf
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		POP	CX
		POP	SI
  @@ZipDir11:	POP	AX
		POP	DX
		ADD	SI,AX			; Next entry
		SUB	CX,AX
		MOV	AX,0
		SBB	AX,DX
		JNE	@@ZipDir21
		MOV	DL,1
		OR	CX,CX
		JNE	@@ZipDir10
		JMP	@@ZipDir01

  @@ZipDir20:	OR	DL,DL			; Continue read
		JZ	@@ArcRdErr1
		XOR	AX,AX
  @@ZipDir21:	ADD	DS:[BP].ZipDirSize.Lo,CX
		ADC	DS:[BP].ZipDirSize.Hi,AX
		SUB	DS:[BP].ZipDirOffset.Lo,CX
		SBB	DS:[BP].ZipDirOffset.Hi,AX
		JMP	@@ZipDir01
  @@ZipDir22:	CMP	DS:[BP].ZipEndSign[0],0
		JNE	@@ArcRdErr1
  @@ZipDir23:	MOV	AH,3Eh
		CALL	Intr21
		JC	@@ArcRdErr3
		MOV	AX,DS:[BP].ZipStartDsk
		CMP	AX,DS:[BP].ZipLastDsk
		JE	@@ExitOk
		XOR	CX,CX
		XOR	DX,DX
		CMP	DS:[BP].ZipEndSign[0],CL
		JE	@@ZipDir24
		MOV	WORD PTR DS:[BP+2],1
		MOV	DS:[BP].ZipEndSign[0],CL
		MOV	AX,DS:[BP+SIZE ZipEndHd].Lo
		MOV	DS:[BP].ZipDirSize.Lo,AX
		MOV	AX,DS:[BP+SIZE ZipEndHd].Hi
		MOV	DS:[BP].ZipDirSize.Hi,AX
		MOV	CX,DS:[BP+SIZE ZipEndHd+4].Lo
		MOV	DX,DS:[BP+SIZE ZipEndHd+4].Hi
		DEC	DS:[BP].ZipStartDsk
  @@ZipDir24:	MOV	DS:[BP].ZipDirOffset.Lo,CX
		MOV	DS:[BP].ZipDirOffset.Hi,DX
		INC	DS:[BP].ZipStartDsk
		MOV	AX,DS:[BP].ZipStartDsk
		CMP	AX,DS:[BP].ZipLastDsk
		JAE	@@ExitOk
		INC	AX			; Next volume
		MOV	WORD PTR ES:TmpBuf[200],AX
  @@ZipDir25:	PUSH	BP
		MOV	DX,OFFSET ZipDisk_T
		MOV	BP,OFFSET ZipDisk_B
		CALL	Dialog
		POP	BP
		JNE	@@ExitOk
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET LineBuf2
		MOV	AL,40h
		CALL	OpenFil
		POP	DS
		MOV	BX,AX
		JNC	@@ZipDir01
		CMP	AX,3
		JBE	@@ZipDir25
		JMP	@@ArcRdErr3

  @@GoArj:	XOR	AX,AX			; ARJ
		JMP	@@Error

  @@ArcRdErr1:	MOV	AX,1			; Error
  @@ArcRdErr2:	MOV	CX,AX
		MOV	AH,3Eh
		CALL	Intr21
		JC	@@ArcRdErr3
		MOV	AX,CX
  @@ArcRdErr3:	CMP	AX,53h
		JE	@@Error
		MOV	DX,OFFSET ArcErr1_T
		JMP	@@ArcErr
  @@Broken:	MOV	AH,3Eh
		CALL	Intr21
		MOV	DX,OFFSET ArcErr2_T
  @@ArcErr:	MOV	AX,OFFSET ArcErr_B
  @@Error:	STC
		JMP	@@Exit
  @@ExitOk:	CLC
  @@Exit:	POP	ES:WorkBuffer
		MOV	DI,ES:ExecArcDirBuf
		RET
ReadArcDir	ENDP

		.CODE	Subs
ExecArcCalcEnt	PROC	NEAR
		CMP	BYTE PTR DS:[SI],0
		JE	@@Exit
		PUSH	ES			; Compare path
		MOV	AX,ES
		SUB	AX,(SIZE QuickViewData-1)/16+1
		MOV	ES,AX
		MOV	DI,ES:ViewArcPath
		CALL	StrLen
		DEC	CX
		JE	@@AddSize
		CALL	CompNormSubStr
		POP	ES
		JNE	@@Exit
		LODSW
		CMP	AL,'\'
		JNE	@@Exit
		OR	AH,AH
		JE	@@Exit
		DEC	SI
		PUSH	ES
  @@AddSize:	POP	ES
		MOV	AX,DS:ExecArcEntry.SizeF.Lo ; Add size
		ADD	DS:QuickViewFlSize.Lo,AX
		MOV	AX,DS:ExecArcEntry.SizeF.Hi
		ADC	DS:QuickViewFlSize.Hi,AX
		ADC	DS:QuickViewFlSize[4].Lo,0
		ADC	DS:QuickViewFlSize[4].Hi,0
		MOV	AX,DS:ExecArcCompSize.Lo
		MOV	DX,DS:ExecArcCompSize.Hi
		MOV	CX,AX
		AND	CX,DX
		INC	CX
		JE	@@ChkDir
		MOV	DS:QuickViewComp,1
		ADD	DS:QuickViewClSize.Lo,AX
		ADC	DS:QuickViewClSize.Hi,DX
		ADC	DS:QuickViewClSize[4].Lo,0
		ADC	DS:QuickViewClSize[4].Hi,0
  @@ChkDir:	XOR	DX,DX			; Check file
		MOV	DI,SI
		CALL	CutPath
		XCHG	SI,DI
		CMP	BYTE PTR DS:[DI],0
		JE	@@CutEnd
		TEST	DS:ExecArcEntry.AttrF,10h
		JNZ	@@ChkDupes
		ADD	DS:QuickViewFiles.Lo,1
		ADC	DS:QuickViewFiles.Hi,0
		CMP	SI,DI
		JE	@@Exit
  @@CutEnd:	MOV	BYTE PTR DS:[DI-1],0	; Check dupe dirs
  @@ChkDupes:	CMP	DS:ExecArcOverflow,0
		JNE	@@Exit
		PUSH	ES
		PUSH	DS
		POP	ES
		MOV	DI,SI
		CALL	StrLen
		POP	ES
		MOV	SI,DI
		XOR	DI,DI
  @@ChkDupe:	CMP	DI,DS:ExecArcDirBuf
		JAE	@@AddDir
		PUSH	DI
		PUSH	SI
		PUSH	CX
		DEC	CX
		CALL	CompNormSubStr
		MOV	AL,ES:[DI]
		POP	CX
		POP	SI
		POP	DI
		JNE	@@NextDupe
		CMP	AL,0
		JE	@@ChkDirs
		CMP	AL,'\'
		JE	@@ChkDirs
  @@NextDupe:	PUSH	CX
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		POP	CX
		JMP	@@ChkDupe
  @@Overflow:	MOV	DS:ExecArcOverflow,1
		CALL	ExecArcBeep
		JMP	@@Exit
  @@AddDir:	OR	DX,DX
		JNE	@@NextDir
		MOV	AX,DS:ExecArcDirBuf
		MOV	DI,AX
		ADD	AX,CX
		JC	@@Overflow
		CMP	AX,DS:ExecArcDirEnd
		JA	@@Overflow
		MOV	BX,CX
		PUSH	SI
		REP	MOVSB
		POP	SI
  @@NextDir:	INC	DX
		MOV	DI,SI
		CALL	CutPath
		XCHG	SI,DI
		CMP	SI,DI
		JNE	@@CutEnd
  @@ChkDirs:	OR	DX,DX
		JE	@@Exit
		ADD	DS:QuickViewDirs.Lo,DX
		ADC	DS:QuickViewDirs.Hi,0
		ADD	DS:ExecArcDirBuf,BX
  @@Exit:	RET
ExecArcCalcEnt	ENDP
