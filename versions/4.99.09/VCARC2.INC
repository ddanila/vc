;---------------------------------------------------------------
;		Archive subroutines
;
;		Version:	14.03.1999
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Exec archive command
;		ES:CX - first command pointer
;		DS:SI - filename or SI=0 DS=WCB
;		ES:DI -	archive filename
;		ES:BX - path to extract or 0
;		DS:BP - path in archive (for extract and add)
;		   DX - pointers table (Title, Archive, ExecArcProgram)
;		   AL - number of command
;		   AH - flags:
;			b0 - out name
;			b1 - out progress bar
;			b4 - 0=file, 1=subdirectory
;	Return:	   NC - Ok, AX - errorlevel
;		   CY - error, AX - error code
;---------------------------------------------------------------
ExecArcProgram	EQU	TmpBuf
ExecArcArchive	EQU	LineBuf3
ExecArcCmdLine	EQU	BYTE PTR LineBuf4
ExecArcEPB	EQU	BYTE PTR LineBuf4[128]
ExecArcListNum	EQU	WORD PTR LineBuf4[142]
ExecArcPoint	EQU	WORD PTR LineBuf4[146]
ExecArcPntEnd	EQU	WORD PTR LineBuf4[148]
ExecArcNumbF	EQU	WORD PTR LineBuf4[150]
ExecArcBufSeg	EQU	WORD PTR LineBuf4[152]
ExecArcPathPnt	EQU	WORD PTR LineBuf4[154]
ExecArcPrmPnt	EQU	WORD PTR LineBuf4[156]
ExecArcPrmFlg	EQU	BYTE PTR LineBuf4[158]
ExecArcCommand	EQU	BYTE PTR LineBuf4[159]
ExecArcFlags	EQU	BYTE PTR LineBuf4[160]
ExecArcKbdBusy	EQU	BYTE PTR LineBuf4[161]
ExecArcErrLevel	EQU	BYTE PTR LineBuf4[162]
ExecArcOverflow	EQU	BYTE PTR LineBuf4[163]
ExecArcDirBuf	EQU	WORD PTR LineBuf4[164]	; Buffer for directory list
ExecArcDirEnd	EQU	WORD PTR LineBuf4[166]	; End of ExecArcDirBuf
ExecArcDirProc	EQU	WORD PTR LineBuf4[168]	; Procedure to process entry
ExecArcCompSize	EQU    DWORD PTR LineBuf4[170]	; Compressed size
ExecArcVwSeg	EQU	WORD PTR LineBuf4[174]	; Seg of panel with archive

ExecArcFileLst	EQU	HelpBuf
ExecArcScrBuf	EQU	HelpBuf			; Buffer for captured text
ExecArcBufEnd	EQU	HelpBuf+LenHelp		; End of ExecArcScrBuf

ExecArcEntry	EQU	TmpErr
ExecArcNameBuf	EQU	TmpErr+40
ExecArcNameEnd	EQU	TmpErr+256
ExecArcStatus	EQU	BYTE PTR TmpErr[27]
ExecArcScript	EQU	WORD PTR TmpErr[28]
ExecArcNameBeg	EQU	WORD PTR TmpErr[30]
ExecArcNameLen	EQU	WORD PTR TmpErr[32]
ExecArcPathBeg	EQU	WORD PTR TmpErr[34]
ExecArcPathLen	EQU	WORD PTR TmpErr[36]
ExecArcPathEnd	EQU	WORD PTR TmpErr[38]

		.CONST
NotImpl_S	DB	"Function is not implemented for this archive",0
ExecArcErr_S	DB	"Can't execute the program",0
ExecArcLstEr_S	DB	"Error writting list of the files for",0
ExecArcLong_S	DB	"Command line too long",0
ExecArcLevel_S	DB	' ^0E errorlevel ^1I ',0
ExecArcInLine_S	DB	' ^0E ',0
ArcList_F	DB	'vcarch',0
Null_F		DB	'NUL',0
FileChars_S	DB	'"[]:|<>+=;,'

		.DATA
NotImpl_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	NotImpl_S
		DW	Line1_S
		DW	Ok_S
		DW	0
NotImpl_B	BOXHD	< NotImpl_P, 900h, 0, Dialog_K, Def_M, Default_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
ExecArcErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	ExecArcErr_S
		DW	Line2_S
		DW	Ok_S
		DW	0
ExecArcErr_B	BOXHD	< ExecArcErr_P, 900h, 0, Error_K, Def_M, None_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
ExecArcLstEr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	ExecArcLstEr_S
		DW	Line2_S
		DW	Ok_S
		DW	0
ExecArcLstEr_B	BOXHD	< ExecArcLstEr_P, 900h, 0, Error_K, Def_M, None_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
ExecArcLong_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	ExecArcLong_S
		DW	Line2_S
		DW	Ok_S
		DW	0
ExecArcLong_B	BOXHD	< ExecArcLong_P, 900h, 0, Error_K, Def_M, None_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
ExecArcMem_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	RunEr_S
		DW	Line2_S
		DW	Ok_S
		DW	0
ExecArcMem_B	BOXHD	< ExecArcMem_P, 900h, 0, Error_K, Def_M, None_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
ExecArcLevel_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	ExecArcLevel_S
		DW	0FFFEh
		DW	Ok_S
		DW	0
ExecArcLevel_B	BOXHD	< ExecArcLevel_P, 700h, ExecArcDialCall, Error_K, Def_M, None_H, 2 >
		BOXMN	< 4, 0,0,0,0, 2FEh, OkMn_D, 0 >
		DB	0
ExecArcInLine_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	ExecArcInLine_S
		DW	0FFFEh
		DW	0
ExecArcInLine_B	BOXHD	< ExecArcInLine_P, 700h, ExecArcDialCall, Error_K, Def_M, None_H, 0 >
		BOXMN	< 1, 0,0,0,0, 105h, TmpErr+50, 0 >
		DB	0
ExecArcInChar_B	BOXHD	< ExecArcInLine_P, 700h, ExecArcInChCall, Error_K, Def_M, None_H, 0 >
		DB	0
ExecArc_T	DW	ArchiveExt, TmpErr
ExecArcIntFlag	DB	0
ExecArcInt10	DD	0
ExecArcInt16	DD	0
ExecArcInt21	DD	0

		.CODE	Subs
ExecArcCmd	PROC	FAR
		USES	BX,CX,DX,SI,DI,BP,DS
		MOV	ES:ExecArcCommand,AL
		MOV	ES:ExecArcFlags,AH
		MOV	ES:ExecArcPoint,OFFSET ExecArcScrBuf
		MOV	ES:ExecArcPntEnd,OFFSET ExecArcScrBuf
		MOV	ES:ExecArcPathEnd,OFFSET ExecArcNameBuf
		MOV	ES:ExecArcPathPnt,BP
		MOV	ES:ExecArcBufSeg,DS
		MOV	ES:ExecArcErrLevel,0
		PUSH	DS
		PUSH	SI
		PUSH	BX
		PUSH	DI
		PUSH	ES
		POP	DS
		MOV	SI,CX			; Find line
		CALL	FindArcCmd
		JNC	@@CmdFound
		MOV	BP,OFFSET NotImpl_B
		CALL	Dialog
		POP	DI
		POP	BX
		XOR	AX,AX
		JMP	@@ExecError4

  @@CmdFound:	MOV	DI,OFFSET LineBuf4	; Utility to exec
		PUSH	DI
		LEA	BP,[DI+512-1]
  @@MovPrgName:	LODSB
		CMP	AL,13
		JE	@@EndPrgName
		CMP	AL,10
		JE	@@EndPrgName
		CMP	AL,' '
		JE	@@EndPrgName
		CMP	AL,9
		JE	@@EndPrgName
		CMP	AL,1Ah
		JE	@@EndPrgName
		CMP	DI,BP
		JAE	@@MovPrgName
		STOSB
		JMP	@@MovPrgName
  @@EndPrgName:	MOV	AL,0
		STOSB
		DEC	SI
		CALL	FndLet
		MOV	BP,SI
		POP	SI
		MOV	DI,OFFSET ExecArcProgram
		PUSH	DI
		PUSH	SI
		CALL	strcpy
		POP	SI
		POP	DI
		CALL	FindPath		; Find file in PATH
		JC	@@ExitPrgNam

;##
		CALL	ShortName

		JNC	@@ExitPrgNam
		CMP	AX,53h
		STC
		JE	@@ExitPrgNam
		CLC
  @@ExitPrgNam:	POP	SI
		JC	@@ExecError1

		MOV	DI,OFFSET ExecArcArchive ; Canonize archive name
		CMP	ES:ExecArcCommand,1
		JNE	@@NormArc
		CALL	strcpy
		JMP	@@ChkCmdLn0
  @@NormArc:	CALL	NormPath
		JC	@@ExecError1

  @@ChkCmdLn0:	MOV	SI,BP			; Check command line properties
		MOV	AH,0
		XOR	BX,BX
  @@ChkCmdLn1:	XOR	DI,DI
  @@ChkCmdLn2:	LODSB
		CMP	AL,13
		JE	@@CountFls0
		CMP	AL,10
		JE	@@CountFls0
		CMP	AL,1Ah
		JE	@@CountFls0
		CMP	AL,'!'
		JNE	@@ChkCmdLn8
		LODSB
		CALL	UpCase
		CMP	AL,'!'
		JE	@@ChkCmdLn8
		CMP	AL,'A'
		JNE	@@ChkCmdLn3
		PUSH	DI
		PUSH	CX
		MOV	DI,OFFSET ExecArcArchive
		CALL	StrLen
		DEC	CX
		ADD	BX,CX
		POP	CX
		POP	DI
		JMP	@@ChkCmdLn1
  @@ChkCmdLn3:	CMP	AL,'F'
		JNE	@@ChkCmdLn4
		OR	AH,1
		JMP	@@ChkCmdLn1
  @@ChkCmdLn4:	CMP	AL,'M'
		JNE	@@ChkCmdLn5
		OR	AH,3
		JMP	@@ChkCmdLn1
  @@ChkCmdLn5:	CMP	AL,'@'
		JNE	@@ChkCmdLn6
		TEST	AH,4
		JNZ	@@ChkCmdLn1
		SUB	BX,DI
		PUSH	DI
		MOV	DI,OFFSET TempDir
		CALL	StrLen
		DEC	CX
		POP	DI
		ADD	DI,CX
		PUSH	ES
		PUSH	DI
		PUSH	CS
		POP	ES
		MOV	DI,OFFSET ArcList_F
		CALL	StrLen
		DEC	CX
		POP	DI
		POP	ES
		ADD	CX,DI
		ADD	CX,4
		OR	AH,4
		JMP	@@ChkCmdLn1
  @@ChkCmdLn6:	CMP	AL,'T'
		JNE	@@ChkCmdLn7
		PUSH	CX
		MOV	DI,OFFSET TempDir
		CALL	StrLen
		DEC	CX
		ADD	BX,CX
		POP	CX
		JMP	@@ChkCmdLn1
  @@ChkCmdLn7:	DEC	SI
		JMP	@@ChkCmdLn2
  @@ChkCmdLn8:	INC	BX
		INC	DI
		CMP	AL,' '
		JE	@@ChkCmdLn1
		CMP	AL,9
		JE	@@ChkCmdLn1
		JMP	@@ChkCmdLn2

  @@CountFls0:	POP	SI			; Count files
		POP	DI
		POP	ES
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	DX
		PUSH	CX
		XOR	DX,DX
		XOR	SI,SI
		CMP	DS:ExecArcCommand,1
		JE	@@CountFls8
		OR	DI,DI
		JE	@@CountFls2
		CALL	ExecArcChkName2
		JNZ	@@CountFls1
		OR	AH,8
  @@CountFls1:	CALL	StrLen
		DEC	CX
		MOV	SI,CX
		MOV	DX,1
		TEST	DS:ExecArcFlags,10h
		JZ	@@CountFls8
		ADD	SI,4
		JMP	@@CountFls8
  @@CountFls2:	MOV	DI,OFFSET ArcPath
		CALL	ExecArcChkName2
		JNZ	@@CountFls3
		OR	AH,8
  @@CountFls3:	CALL	StrLen
		XOR	DX,DX
		XOR	SI,SI
		MOV	DI,ES:DiskBuf
  @@CountFls4:	CMP	DI,ES:EndBuf
		JAE	@@CountFls8
		PUSH	DI
		AND	ES:[DI].AttrF,7Fh
		TEST	ES:[DI].AttrF,40h
		JZ	@@CountFls7
		INC	DX

;##
		ADD	SI,ES:[DI].LenShortNamF
		ADD	DI,OFFSET NameF

		CMP	CX,1
		JNE	@@CountFls5
		CALL	ExecArcChkName2
		JMP	@@CountFls6
  @@CountFls5:	ADD	SI,CX
		CALL	ExecArcChkName1
  @@CountFls6:	JNZ	@@CountFls7
		OR	AH,8
  @@CountFls7:	POP	DI
		PUSH	AX
		MOV	AX,ES:[DI].LenShortNamF
		ADD	DI,ES:[DI].LenLongNamF
		ADD	DI,SIZE FilEnt
		ADD	DI,AX
		POP	AX
		JMP	@@CountFls4
  @@CountFls8:	PUSH	DS
		POP	ES
		POP	CX
		MOV	DI,BX
		ADD	DI,SI
		TEST	AH,8
		JNZ	@@CountFls9
		OR	AH,8
		CMP	DI,126
		JA	@@CountFls9
		AND	AH,7
		TEST	AH,2
		JNZ	@@LenCmdLn1
		CMP	DX,1
		JA	@@CountFls9
		TEST	AH,1
		JNZ	@@LenCmdLn1
  @@CountFls9:	TEST	AH,4
		JNZ	@@LenCmdLn0
		TEST	AH,8
		JZ	@@LenCmdLn1
		CMP	DX,1
		JE	@@LenCmdLn1
		XOR	DI,DI
		JMP	@@LenCmdLn1

  @@LenCmdLn0:	MOV	DI,BX			; Check length of command line
		ADD	DI,CX
		OR	AH,80h
  @@LenCmdLn1:	POP	DX
		POP	SI
		MOV	BL,AH
		CMP	DI,126
		JBE	@@FileList00
		MOV	BP,OFFSET ExecArcLong_B
		CALL	Dialog
		XOR	AX,AX
		JMP	@@ExecError4

  @@FileList00:	TEST	BL,80h			; Create filelist
		JZ	@@ChgDir0
		POP	AX
		POP	DS
		PUSH	DS
		PUSH	AX
		PUSH	SI
		PUSH	DX
		PUSH	BX
		PUSH	AX
		MOV	DX,OFFSET ExecArcFileLst
		MOV	DI,DX
		CALL	MoveTempPath
		MOV	SI,OFFSET ArcList_F
		CALL	OpenTempFile
		POP	DX
		JC	@@FileList12
		MOV	ES:ExecArcListNum,AX
		OR	DX,DX
		JE	@@FileList02
		PUSH	ES
		PUSH	DS
		POP	ES
		MOV	DI,DX
		CALL	StrLen
		DEC	CX
		POP	ES
		MOV	AH,40h
		CALL	Intr21
		JC	@@FileList09
		CMP	AX,CX
		JNE	@@FileList08
		PUSH	CS
		POP	DS
		TEST	ES:ExecArcFlags,10h
		JZ	@@FileList01
		MOV	DX,OFFSET Mask_S
		MOV	CX,4
		MOV	AH,40h
		CALL	Intr21
		JC	@@FileList09
		CMP	AX,CX
		JNE	@@FileList08
  @@FileList01:	MOV	DX,OFFSET CrLf_S
		MOV	CX,2
		MOV	AH,40h
		CALL	Intr21
		JC	@@FileList09
		CMP	AX,CX
		JNE	@@FileList08
		JMP	@@FileList07
  @@FileList02:	PUSH	ES
		PUSH	DS
		POP	ES
		MOV	DI,OFFSET ArcPath
		CALL	StrLen
		DEC	CX
		MOV	DI,CX
		POP	ES
		MOV	SI,DS:DiskBuf
  @@FileList03:	CMP	SI,DS:EndBuf
		JAE	@@FileList07
		TEST	DS:[SI].AttrF,40h
		JZ	@@FileList06
		OR	DI,DI
		JE	@@FileList04
		MOV	DX,OFFSET ArcPath
		MOV	CX,DI
		MOV	AH,40h
		CALL	Intr21
		JC	@@FileList09
		CMP	AX,CX
		JNE	@@FileList08
		PUSH	DS
		PUSH	CS
		POP	DS
		MOV	DX,OFFSET Mask_S
		MOV	CX,1
		MOV	AH,40h
		CALL	Intr21
		POP	DS
		JC	@@FileList09
		CMP	AX,CX
		JNE	@@FileList08
  @@FileList04:

;##
		LEA	DX,[SI].NameF
		MOV	CX,DS:[SI].LenShortNamF

		DEC	CX
		MOV	AH,40h
		CALL	Intr21
		JC	@@FileList09
		CMP	AX,CX
		JNE	@@FileList08
		TEST	DS:[SI].AttrF,10h
		JZ	@@FileList05
		PUSH	DS
		PUSH	CS
		POP	DS
		MOV	DX,OFFSET Mask_S
		MOV	CX,4
		MOV	AH,40h
		CALL	Intr21
		POP	DS
		JC	@@FileList09
		CMP	AX,CX
		JNE	@@FileList08
  @@FileList05:	PUSH	DS
		PUSH	CS
		POP	DS
		MOV	DX,OFFSET CrLf_S
		MOV	CX,2
		MOV	AH,40h
		CALL	Intr21
		POP	DS
		JC	@@FileList09
		CMP	AX,CX
		JNE	@@FileList08
  @@FileList06:	CALL	NextFilEnt
		JMP	@@FileList03
  @@FileList07:	MOV	AH,3Eh
		CALL	Intr21
		JC	@@FileList12
		XOR	AX,AX
		JMP	@@FileList12
  @@FileList08:	MOV	AX,1
  @@FileList09:	MOV	CX,AX
		MOV	AH,3Eh
		CALL	Intr21
		JNC	@@FileList10
		CMP	AX,53h
		JE	@@FileList12
  @@FileList10:	PUSH	ES
		POP	DS
		MOV	DX,OFFSET ExecArcFileLst
		MOV	AH,41h
		CALL	Intr21
		JNC	@@FileList11
		CMP	AX,53h
		JE	@@FileList12
  @@FileList11:	MOV	AX,CX
  @@FileList12:	PUSH	ES
		POP	DS
		POP	BX
		POP	DX
		POP	SI
		OR	AX,AX
		JE	@@ChgDir0
		CMP	AX,53h
		JE	@@ExecError4
		PUSH	AX
		MOV	BP,OFFSET ExecArcLstEr_B
		CALL	Dialog
		POP	AX
		JMP	@@ExecError4

  @@ChgDir0:	OR	SI,SI			; Change directory
		JE	@@CmdLine00
		PUSH	DX
		MOV	AX,DS:[SI]
		CMP	AH,':'
		JNE	@@ChgDir1
		CALL	UpCase
		CMP	AL,DS:DosPath[0]
		JE	@@ChgDir1
		MOV	DH,AL
		CALL	Phantom
		MOV	AX,53h
		JC	@@ExecError2
		PUSH	SI
		MOV	SI,OFFSET DosPath+3
		MOV	DL,DH
		SUB	DL,'A'-1
		MOV	AH,47h
		CALL	Intr21
		POP	SI
		JC	@@ChgDir3
		MOV	DS:DosPath[0],DH
		DEC	DX
		MOV	AH,0Eh
		CALL	Intr21
  @@ChgDir1:	MOV	DX,OFFSET ExecArcCmdLine
		MOV	DI,DX
		CALL	strcpy
		MOV	SI,DX
		CALL	CutPath
		CMP	BYTE PTR DS:[SI],0
		JNE	@@ChgDir2
		MOV	AX,'.'
		STOSW
  @@ChgDir2:	CALL	ChgDisk
		MOV	AX,53h
		JC	@@ExecError2
		MOV	AH,3Bh
		CALL	Intr21
  @@ChgDir3:	POP	DX
		JNC	@@CmdLine00

		PUSH	SI
  @@ExecError1:	POP	SI			; Error
		CMP	AX,53h
		JE	@@ExecError3
		PUSH	AX
		MOV	BP,OFFSET ExecArcErr_B
		CALL	Dialog
		POP	AX
		PUSH	DX
  @@ExecError2:	POP	DX
  @@ExecError3:	PUSH	AX
		MOV	DL,DS:DosPath[0]
		SUB	DL,'A'
		MOV	AH,0Eh
		CALL	Intr21
		POP	AX
  @@ExecError4:	POP	SI
		POP	DS
		STC
		JMP	@@Exit

  @@CmdLine00:	MOV	SI,BP			; Move command line
		MOV	DS:ExecArcPrmPnt,BP
		MOV	DS:ExecArcPrmFlg,BL
		MOV	DI,OFFSET ExecArcCmdLine+1
		LEA	BP,[DI+126]
  @@CmdLine01:	MOV	CX,DI
  @@CmdLine02:	LODSB
		CMP	AL,13
		JE	@@CmdLine25
		CMP	AL,10
		JE	@@CmdLine25
		CMP	AL,1Ah
		JE	@@CmdLine25
		CMP	AL,'!'
		JNE	@@CmdLine20
		LODSB
		CALL	UpCase
		CMP	AL,'!'
		JE	@@CmdLine20
		CMP	AL,'A'
		JNE	@@CmdLine05
		PUSH	SI
		MOV	SI,OFFSET ExecArcArchive
  @@CmdLine03:	CALL	ExecArcMoveCmd
  @@CmdLine04:	PUSH	ES
		POP	DS
		POP	SI
		JMP	@@CmdLine01
  @@CmdLine05:	CMP	AL,'@'
		JNE	@@CmdLine07
		TEST	BL,40h
		JNZ	@@CmdLine01
		TEST	BL,80h
		JNZ	@@CmdLine06
		MOV	DI,CX
		JMP	@@CmdLine01
  @@CmdLine06:	OR	BL,40h
		PUSH	SI
		MOV	SI,OFFSET ExecArcFileLst
		JMP	@@CmdLine03
  @@CmdLine07:	CMP	AL,'F'
		JE	@@CmdLine08
		CMP	AL,'M'
		JNE	@@CmdLine17
  @@CmdLine08:	TEST	BL,20h
		JNZ	@@CmdLine01
		TEST	BL,80h
		JNZ	@@CmdLine01
		OR	BL,20h
		MOV	AX,SI
		POP	SI
		POP	DS
		PUSH	DS
		PUSH	SI
		PUSH	AX
		OR	SI,SI
		JNE	@@CmdLine16
		MOV	SI,DS:DiskBuf
  @@CmdLine09:	CMP	SI,DS:EndBuf
		JAE	@@CmdLine04
		TEST	DS:[SI].AttrF,40h
		JZ	@@CmdLine15
		TEST	BL,10h
		JZ	@@CmdLine12
		PUSH	ES
		PUSH	DI
		PUSH	DS
		POP	ES
		MOV	DI,OFFSET ArcPath
		CALL	StrLen
		POP	DI
		POP	ES
		CMP	CX,1
		JNE	@@CmdLine10
		DEC	CX
  @@CmdLine10:

;##
		ADD	CX,DS:[SI].LenShortNamF

		TEST	DS:[SI].AttrF,10h
		JZ	@@CmdLine11
		ADD	CX,4
  @@CmdLine11:	ADD	CX,DI
		CMP	CX,BP
		JA	@@CmdLine04
		MOV	AL,' '
		STOSB
  @@CmdLine12:	OR	BL,10h
		PUSH	SI
		MOV	SI,OFFSET ArcPath
		CMP	BYTE PTR DS:[SI],0
		JE	@@CmdLine13
		CALL	ExecArcMoveCmd
		MOV	AL,'\'
		CMP	DI,BP
		JAE	@@CmdLine13
		STOSB
  @@CmdLine13:	POP	SI
		PUSH	SI

;##
		ADD	SI,OFFSET NameF

		CALL	ExecArcMoveCmd
		POP	SI
		TEST	DS:[SI].AttrF,10h
		JZ	@@CmdLine14
		PUSH	DS
		PUSH	SI
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET Mask_S
		CALL	ExecArcMoveCmd
		POP	SI
		POP	DS
  @@CmdLine14:	AND	DS:[SI].AttrF,3Fh
		OR	DS:[SI].AttrF,80h
		TEST	BL,2
		JZ	@@CmdLine04
  @@CmdLine15:	CALL	NextFilEnt
		JMP	@@CmdLine09
  @@CmdLine16:	CALL	ExecArcMoveCmd
		TEST	ES:ExecArcFlags,10h
		JZ	@@CmdLine04
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET Mask_S
		JMP	@@CmdLine03
  @@CmdLine17:	CMP	AL,'T'
		JNE	@@CmdLine18
		PUSH	SI
		MOV	SI,OFFSET TempDir
		JMP	@@CmdLine03
  @@CmdLine18:	DEC	SI
		JMP	@@CmdLine02
  @@CmdLine20:	CMP	DI,BP
		JAE	@@CmdLine21
		STOSB
  @@CmdLine21:	CMP	AL,' '
		JE	@@CmdLine01
		CMP	AL,9
		JE	@@CmdLine01
		JMP	@@CmdLine02
  @@CmdLine25:	MOV	AL,13
		STOSB
		MOV	AX,DI
		SUB	AX,OFFSET ExecArcCmdLine+2
		MOV	DS:ExecArcCmdLine[0],AL
		PUSH	DX

		PUSH	CS			; Redirect StdOut
		POP	DS
		XOR	SI,SI
		XOR	DI,DI
		MOV	DX,OFFSET Null_F
		MOV	AL,41h
		CALL	OpenFil
		JC	@@HookInt
		MOV	CX,AX
		MOV	BX,1
		MOV	AH,45h
		CALL	Intr21
		XCHG	BX,CX
		JC	@@CloseNul
		MOV	SI,AX
		MOV	AH,46h
		CALL	Intr21
  @@CloseNul:	MOV	AH,3Eh
		CALL	Intr21
		MOV	BX,2
		MOV	AH,45h
		CALL	Intr21
		JC	@@HookInt
		MOV	DI,AX
		XCHG	BX,CX
		MOV	AH,46h
		CALL	Intr21

  @@HookInt:	PUSH	ES			; Hook INT 21h
		PUSH	CS
		POP	DS
		MOV	AX,3521h
		INT	21h
		MOV	DS:ExecArcInt21.Offs,BX
		MOV	DS:ExecArcInt21.Segm,ES
		MOV	DX,OFFSET ExecArcHook21
		MOV	AH,25h
		INT	21h
		MOV	AX,3510h		; Hook INT 10h
		INT	21h
		MOV	DS:ExecArcInt10.Offs,BX
		MOV	DS:ExecArcInt10.Segm,ES
		MOV	DX,OFFSET ExecArcHook10
		MOV	AH,25h
		INT	21h
		MOV	AX,3516h		; Hook INT 16h
		INT	21h
		MOV	DS:ExecArcInt16.Offs,BX
		MOV	DS:ExecArcInt16.Segm,ES
		MOV	DX,OFFSET ExecArcHook16
		MOV	AH,25h
		INT	21h
		MOV	DS:ExecArcIntFlag,0
		POP	ES

		PUSH	ES			; Release memory
		POP	DS
		MOV	BX,DS:WorkBuffer
		MOV	AX,ES
		SUB	BX,AX
		ADD	BX,(MinView SHR 4)+1
		MOV	AH,4Ah
		INT	21h
		MOV	DS:ExecArcKbdBusy,0

		MOV	BX,OFFSET ExecArcEPB	; Exec command
		MOV	DS:[BX].EnvSeg,0
		MOV	DS:[BX].CmdLine.Offs,OFFSET ExecArcCmdLine
		MOV	DS:[BX].CmdLine.Segm,ES
		MOV	DS:[BX].FCB1.Offs,5Ch
		MOV	DS:[BX].FCB1.Segm,ES
		MOV	DS:[BX].FCB2.Offs,6Ch
		MOV	DS:[BX].FCB2.Segm,ES
		MOV	DX,OFFSET ExecArcProgram
		MOV	AX,4B00h
		CALL	Intr21
		PUSHF				; Last LineFeed
		PUSH	AX
		MOV	AL,0Ah
		CALL	ExecArcChar
		MOV	BX,0FFFFh		; Allocate memory
		MOV	AH,4Ah
		INT	21h
		MOV	AH,4Ah
		INT	21h

		PUSH	DS			; Restore INT 16h
		LDS	DX,CS:ExecArcInt16
		MOV	AX,2516h
		INT	21h
		LDS	DX,CS:ExecArcInt10	; Restore INT 10h
		MOV	AX,2510h
		INT	21h
		LDS	DX,CS:ExecArcInt21	; Restore INT 21h
		MOV	AX,2521h
		INT	21h
		POP	DS

		OR	SI,SI			; Restore reditection
		JE	@@RedirBack
		MOV	BX,SI
		MOV	CX,1
		MOV	AH,46h
		CALL	Intr21
		MOV	AH,3Eh
		CALL	Intr21
  @@RedirBack:	OR	DI,DI
		JE	@@ChkError
		MOV	BX,DI
		MOV	CX,2
		MOV	AH,46h
		CALL	Intr21
		MOV	AH,3Eh
		CALL	Intr21

  @@ChkError:	POP	AX			; Execute error
		POPF
		POP	DX
		POP	SI
		POP	DS
		JNC	@@ErrorLevel
		PUSH	ES
		POP	DS
		MOV	BP,OFFSET ExecArcMem_B
		CMP	AX,7
		JE	@@ExecError
		CMP	AX,8
		JE	@@ExecError
		MOV	BP,OFFSET ExecArcErr_B
  @@ExecError:	PUSH	AX
		CALL	Dialog
		POP	AX
		JMP	@@DelList

  @@ErrorLevel:	MOV	AH,4Dh			; Error Level
		INT	21h
		CMP	AL,0
		JE	@@ChkDone
		PUSH	DS
		PUSH	DX
		PUSH	ES
		POP	DS
		MOV	AH,0
		MOV	WORD PTR DS:TmpErr,AX
		MOV	DS:ExecArcErrLevel,AL
		MOV	AL,1
		CALL	ExecArcDialog
		MOV	DS:ListWindow,BX
		MOV	DX,OFFSET ExecArc_T
		MOV	BP,OFFSET ExecArcLevel_B
		ADD	CS:[BP+SIZE BoxHd].mBase.Y,BH
		CALL	Dialog
		SUB	CS:[BP+SIZE BoxHd].mBase.Y,BH
		POP	DX
		POP	DS

  @@ChkDone:	XOR	AX,AX			; Check all files done
		CMP	ES:ExecArcCommand,1
		JE	@@DelList
		OR	SI,SI
		JNE	@@DelList
		MOV	SI,DS:DiskBuf
		MOV	BL,ES:ExecArcPrmFlg
		TEST	BL,80h
		JNZ	@@UnSelect
  @@ChkSelFls:	CMP	SI,DS:EndBuf
		JAE	@@CurDir
		TEST	DS:[SI].AttrF,40h
		JNZ	@@DoAgain
		CALL	NextFilEnt
		JMP	@@ChkSelFls
  @@DoAgain:	XOR	SI,SI
		PUSH	DS
		PUSH	SI
		PUSH	ES
		POP	DS
		MOV	BP,DS:ExecArcPrmPnt
		JMP	@@CmdLine00

  @@UnSelect:	CMP	SI,DS:EndBuf		; Unselect all files
		JAE	@@DelList
		TEST	DS:[SI].AttrF,40h
		JZ	@@UnSelNext
		AND	DS:[SI].AttrF,3Fh
		OR	DS:[SI].AttrF,80h
  @@UnSelNext:	CALL	NextFilEnt
		JMP	@@UnSelect

  @@DelList:	PUSH	ES			; Delete filelist
		POP	DS
		TEST	DS:ExecArcPrmFlg,80h
		JZ	@@CurDir
		PUSH	AX
		MOV	DX,OFFSET ExecArcFileLst
		MOV	DI,DX
		MOV	SI,OFFSET ArcList_F
		MOV	AX,DS:ExecArcListNum
		CALL	MakePathName
		MOV	AH,41h
		CALL	Intr21
		POP	AX

  @@CurDir:	PUSH	AX			; Restore current dir
		PUSH	ES
		POP	DS
		MOV	DL,DS:DosShortPath[0]
		SUB	DL,'A'
		MOV	AH,0Eh
		CALL	Intr21
		MOV	DX,OFFSET DosPath
		MOV	AH,3Bh
		CALL	Intr21
		CALL	GetPath
		POP	AX
		CMP	AX,1
		CMC
		JC	@@Exit
		MOV	AL,DS:ExecArcErrLevel
		MOV	AH,0
  @@Exit:	RET
ExecArcCmd	ENDP

ExecArcChkName1	PROC	NEAR			; Check spaces in name
		USES	SI
		MOV	SI,DI
  @@GetChar:	LODS	BYTE PTR ES:[SI]
		CMP	AL,' '
		JE	@@Exit
		CMP	AL,0
		JNE	@@GetChar
		CMP	AL,1
  @@Exit:	RET
ExecArcChkName1	ENDP

ExecArcChkName2	PROC	NEAR			; Check spaces and first char
		MOV	AL,ES:[DI]
		CMP	AL,'-'
		JE	@@Exit
		CMP	AL,'@'
		JE	@@Exit
		CMP	AL,'!'
		JE	@@Exit
		CALL	ExecArcChkName1
  @@Exit:	RET
ExecArcChkName2	ENDP

ExecArcMoveCmd	PROC	NEAR			; Move CmdLine, BP-limit
  @@Char:	LODSB
		CMP	AL,0
		JE	@@Exit
		CMP	DI,BP
		JAE	@@Char
		STOSB
		JMP	@@Char
  @@Exit:	RET
ExecArcMoveCmd	ENDP

ExecArcHook16	PROC	FAR
		TEST	CS:ExecArcIntFlag,3
		JNZ	@@OldInt
		CMP	AH,1
		JA	@@OldInt
		PUSH	ES
		MOV	ES,CS:DataSegment
		JE	@@Func01
		CALL	ExecArcInChar
		MOV	AH,0
		JMP	@@Exit
  @@Func01:	XOR	AX,AX
		DEC	ES:ExecArcKbdBusy
		JNE	@@SetFlag
		MOV	AH,0F0h
  @@SetFlag:	OR	AX,AX
  @@Exit:	POP	ES
		RET	2
  @@OldInt:	JMP	CS:ExecArcInt16
ExecArcHook16	ENDP

ExecArcHook10	PROC	FAR
		TEST	CS:ExecArcIntFlag,1
		JNZ	@@OldInt
		CMP	AH,3			; Get cursor size and position
		JE	@@OldInt
		CMP	AH,4			; Get light pen
		JE	@@OldInt
		CMP	AH,8			; Get char/attributes
		JE	@@OldInt
		CMP	AH,0Fh			; Get video mode
		JE	@@OldInt
		CMP	AH,0Eh			; Terminal output
		JNE	@@Exit
		TEST	CS:ExecArcIntFlag,2
		JNZ	@@Exit
		PUSH	ES
		MOV	ES,CS:DataSegment
		CALL	ExecArcChar
		POP	ES
  @@Exit:	IRET
  @@OldInt:	JMP	CS:ExecArcInt10
ExecArcHook10	ENDP

ExecArcHook21	PROC	FAR
		PUSHF
		PUSH	ES
		PUSH	SI
		PUSH	CX
		PUSH	AX
		CLD
		CMP	AH,0
		JE	@@OldInt1
		CMP	AH,31h
		JE	@@OldInt1
		CMP	AH,4Bh
		JE	@@OldInt1
		CMP	AH,4Ch
		JE	@@OldInt1
		OR	CS:ExecArcIntFlag,2
		MOV	ES,CS:DataSegment
  @@Func01:	CMP	AH,1			; Kbd input
		JNE	@@Func02
		CALL	ExecArcInChar
		CALL	ExecArcChar
  @@InpChar1:	POP	CX
		MOV	AH,CH
		PUSH	AX
		JMP	@@Exit

  @@Func02:	CMP	AH,2			; Display char
		JNE	@@Func06
  @@DispChar:	MOV	AL,DL
		CALL	ExecArcChar
  @@OldInt1:	JMP	@@OldInt

  @@Func06:	CMP	AH,6			; Console I/O
		JNE	@@Func07
		CMP	DL,0FFh
		JNE	@@DispChar
		POP	AX
		MOV	AL,1
		PUSH	AX
		JMP	@@Exit

  @@Func07:	CMP	AH,7			; Unfiltred Console input
		JNE	@@Func08
  @@InpChar:	CALL	ExecArcInChar
		JMP	@@InpChar1

  @@Func08:	CMP	AH,8			; Console input
		JNE	@@Func09
		JMP	@@InpChar

  @@Func09:	CMP	AH,9			; Display string
		JNE	@@Func0A
		MOV	SI,DX
  @@Func09a:	LODSB
		CMP	AL,'$'
		JE	@@OldInt
		CALL	ExecArcChar
		JMP	@@Func09a

  @@Func0A:	CMP	AH,0Ah			; Buffered input
		JNE	@@Func0B
		MOV	SI,DX
		MOV	CL,DS:[SI]
		DEC	CX
		INC	DX
		INC	DX
		CALL	ExecArcInLine
		DEC	AX
		DEC	DX
		MOV	SI,DX
		MOV	DS:[SI],AL
		DEC	DX
		JMP	@@Exit

  @@Func0B:	CMP	AH,0Bh			; Input status
		JNE	@@Func0C
		POP	AX
		MOV	AL,0
		DEC	ES:ExecArcKbdBusy
		JNE	@@KbdReady
		MOV	AL,0FFh
  @@KbdReady:	PUSH	AX
		JMP	@@Exit

  @@Func0C:	CMP	AH,0Ch			; Clear & Input
		JNE	@@Func3F
		CMP	AL,AH
		JE	@@OldInt
		CALL	ClrKbd
		MOV	AH,AL
		JMP	@@Func01

  @@Func3F:	CMP	AH,3Fh			; Read from file
		JNE	@@Func40
		OR	BX,BX
		JNE	@@OldInt
		OR	CH,CH
		JE	@@InLine
		MOV	CL,0FFh
  @@InLine:	DEC	CX
		DEC	CX
		CALL	ExecArcInLine
		INC	AX
		POP	CX
		PUSH	AX
		MOV	AL,0Ah
		MOV	DS:[SI],AL
		CALL	ExecArcChar
		JMP	@@Exit

  @@Func40:	CMP	AH,40h			; Write to file
		JNE	@@Redirect
		CMP	BX,1
		JE	@@Func40a
		CMP	BX,2
		JNE	@@OldInt
  @@Func40a:	JCXZ	@@OldInt
		MOV	SI,DX
  @@Func40b:	LODSB
		CALL	ExecArcChar
		LOOP	@@Func40b
		JMP	@@OldInt

  @@Redirect:	CMP	ES:ExecArcCommand,2	; Redirect filenames
		JB	@@OldInt
		CMP	ES:ExecArcCommand,6
		JA	@@OldInt
		CMP	AH,39h
		JE	@@Redir1
		CMP	AH,3Ah
		JE	@@Redir1
		CMP	AH,3Bh
		JE	@@Redir1
		CMP	AH,3Ch
		JE	@@Redir1
		CMP	AH,3Dh
		JE	@@Redir1
		CMP	AH,41h
		JE	@@Redir1
		CMP	AH,43h
		JE	@@Redir1
		CMP	AH,4Eh
		JE	@@Redir1
		CMP	AH,56h
		JE	@@Redir1
		CMP	AH,5Ah
		JE	@@Redir1
		CMP	AH,5Bh
		JE	@@Redir1
		CMP	AH,6Ch
;		JE	@@Redir1
;		CMP	AX,7139h
;		JE	@@Redir1
;		CMP	AX,713Ah
;		JE	@@Redir1
;		CMP	AX,713Bh
;		JE	@@Redir1
;		CMP	AX,7141h
;		JE	@@Redir1
;		CMP	AX,7143h
;		JE	@@Redir1
;		CMP	AX,714Eh
;		JE	@@Redir1
;		CMP	AX,7156h
;		JE	@@Redir1
;		CMP	AX,716Ch
		JNE	@@OldInt
  @@Redir1:	POP	AX
		POP	CX
		POP	SI
		POP	ES
		POPF
		PUSH	ES
		PUSH	DS
		PUSH	DI
		PUSH	SI
		PUSH	DX
		PUSHF
		CMP	AH,6Ch
		JE	@@Swap1
		CMP	AX,716Ch
		JNE	@@Arg1
  @@Swap1:	XCHG	DX,SI
  @@Arg1:	PUSH	AX
		MOV	AX,OFFSET LineBuf3
		CALL	ExecArcRedir
		POP	AX
		CMP	AH,6Ch
		JE	@@Swap2
		CMP	AX,716Ch
		JNE	@@Rename
  @@Swap2:	XCHG	DX,SI
  @@Rename:	CMP	AH,56h
		JE	@@Arg2
		CMP	AX,7156h
		JNE	@@Redir2
  @@Arg2:	PUSH	AX
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		XCHG	DX,DI
		MOV	AX,OFFSET LineBuf4
		CALL	ExecArcRedir
		XCHG	DX,DI
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		POP	AX
  @@Redir2:	POPF
		PUSHF
		CALL	CS:ExecArcInt21
		POP	DX
		POP	SI
		POP	DI
		POP	DS
		POP	ES
		JMP	@@OldIntEx

  @@OldInt:	POP	AX			; Old Interrupt
		POP	CX
		POP	SI
		POP	ES
		POPF
		PUSHF
		CALL	CS:ExecArcInt21
  @@OldIntEx:	PUSHF
		AND	CS:ExecArcIntFlag,0FDh
		POPF
		RET	2

  @@Exit:	AND	CS:ExecArcIntFlag,0FDh	; Exit
		POP	AX
		POP	CX
		POP	SI
		POP	ES
		POPF
		CLC
		RET	2
ExecArcHook21	ENDP

ExecArcRedir	PROC	NEAR
		USES	BX,CX,SI,DI,ES
		MOV	ES,CS:DataSegment
		PUSH	ES
		PUSH	AX
		MOV	DI,ES:ExecArcPathPnt
		MOV	ES,ES:ExecArcBufSeg
		MOV	SI,DX
		CMP	BYTE PTR ES:[DI],0
		JE	@@OldPath

		CMP	BYTE PTR DS:[SI+1],':'
		JNE	@@TargetPath
		MOV	AH,19h
		PUSHF
		CALL	CS:ExecArcInt21
		ADD	AL,'A'
		MOV	AH,AL
		LODSB
		CALL	UpCase
		CMP	AL,AH
		JNE	@@OldPath
		INC	SI

  @@TargetPath:	MOV	AL,DS:[SI]
		CMP	AL,'\'
		JE	@@GetCurDir
		CMP	AL,'/'
		JNE	@@CompPath
  @@GetCurDir:	INC	SI
		MOV	DI,SI
		PUSH	DS
		POP	ES
		POP	SI
		POP	DS
		PUSH	DS
		PUSH	SI
		PUSH	DX
		MOV	DL,0
		MOV	AH,47h
		STC
		PUSHF
		CALL	CS:ExecArcInt21
		POP	DX
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		JC	@@OldPath
		XCHG	SI,DI
  @@CompTarget:	CMP	BYTE PTR ES:[DI],0
		JE	@@Slash
		LODSB
		CMP	AL,'/'
		JNE	@@UpCase1
		MOV	AL,'\'
  @@UpCase1:	CALL	UpCase
		MOV	AH,AL
		MOV	AL,ES:[DI]
		INC	DI
		CALL	UpCase
		CMP	AL,AH
		JE	@@CompTarget
  @@Slash:	MOV	DI,ES:ExecArcPathPnt
		MOV	ES,ES:ExecArcBufSeg
		LODSB
		CMP	AL,'\'
		JE	@@CompPath
		CMP	AL,'/'
		JNE	@@OldPath

  @@CompPath:	CMP	BYTE PTR ES:[DI],0
		JE	@@ChkPath1
		LODSB
		CMP	AL,'/'
		JNE	@@UpCase2
		MOV	AL,'\'
  @@UpCase2:	CALL	UpCase
		MOV	AH,AL
		MOV	AL,ES:[DI]
		INC	DI
		CALL	UpCase
		CMP	AL,AH
		JE	@@CompPath
  @@OldPath:	POP	AX
		POP	ES
		JMP	@@Exit

  @@ChkPath1:	MOV	BX,DX
		POP	DX
		POP	ES
		MOV	DI,DX
		MOV	AX,'.'
		STOSW
		LODSW
		CMP	AL,0
		JE	@@Exit1
		XCHG	BX,DX
		CMP	AL,'\'
		JE	@@ChkPath2
		CMP	AL,'/'
		JNE	@@Exit
  @@ChkPath2:	XCHG	BX,DX
		DEC	DI
		STOSW
		OR	AH,AH
		JE	@@Exit1
		MOV	DI,DX
		DEC	SI
		MOV	CX,127
		REP	MOVSB
		MOV	AL,0
		STOSB
  @@Exit1:	PUSH	ES
		POP	DS
  @@Exit:	RET
ExecArcRedir	ENDP

ExecArcInChar	PROC	NEAR
		USES	BX,CX,DX,SI,DI,BP,DS
		PUSH	ES
		POP	DS
		MOV	DS:ExecArcKbdBusy,0
		MOV	AL,0Ah
		CALL	ExecArcChar
		MOV	AL,0
		CALL	ExecArcDialog
		CMP	AX,65-1
		JBE	@@InChar2
		MOV	AL,0Ah
		CALL	ExecArcChar
		MOV	AL,0
		CALL	ExecArcDialog
  @@InChar2:	MOV	DS:TmpErr[50],AL
		MOV	DS:TmpErr[51],0
		INC	AX
		CMP	BL,AL
		JAE	@@InChar4
		MOV	BL,AL
  @@InChar4:	MOV	DS:ListWindow,BX
		MOV	DX,OFFSET ExecArc_T
		MOV	BP,OFFSET ExecArcInChar_B
		OR	CS:ExecArcIntFlag,1
		XOR	AX,AX
		XCHG	AL,DS:Clock
		XCHG	AH,DS:BlDelay
		MOV	SI,AX
		MOV	CL,DS:HlpPage
		MOV	DS:AutoTim.Lo,0FFFFh
		CALL	Dialog
		AND	CS:ExecArcIntFlag,0FEh
		MOV	AX,SI
		MOV	DS:Clock,AL
		MOV	DS:BlDelay,AH
		MOV	DS:HlpPage,CL
		DEC	DS:ExecArcPoint
		DEC	DS:ExecArcPntEnd
		MOV	AL,DS:TmpErr[51]
		RET
ExecArcInChar	ENDP

ExecArcInLine	PROC	NEAR
		USES	BX,DI,BP,DS
		PUSH	DS
		PUSH	DX
		PUSH	ES
		POP	DS
		MOV	DS:ExecArcKbdBusy,0
		MOV	AL,0Ah
		CALL	ExecArcChar
		MOV	AL,0
		CALL	ExecArcDialog
		OR	AH,AH
		JNE	@@InLine1
		MOV	CH,65
		SUB	CH,AL
		JBE	@@InLine1
		CMP	CH,12
		JAE	@@InLine2
  @@InLine1:	MOV	AL,0Ah
		CALL	ExecArcChar
		MOV	AL,0
		CALL	ExecArcDialog
		MOV	CH,65
		SUB	CH,AL
  @@InLine2:	CMP	CL,CH
		JBE	@@InLine3
		MOV	CL,CH
  @@InLine3:	MOV	CH,CL
		ADD	CH,AL
		CMP	BL,CH
		JAE	@@InLine4
		MOV	BL,CH
  @@InLine4:	MOV	DS:ListWindow,BX
		MOV	BL,AL
		MOV	WORD PTR DS:TmpErr+50,0
		MOV	DX,OFFSET ExecArc_T
		MOV	BP,OFFSET ExecArcInLine_B
		MOV	BYTE PTR CS:[BP+SIZE BoxHd].mCtrl,CL
		ADD	CS:[BP+SIZE BoxHd].mBase,BX
		OR	CS:ExecArcIntFlag,1
		XOR	AX,AX
		XCHG	AL,DS:Clock
		XCHG	AH,DS:BlDelay
		MOV	SI,AX
		MOV	CL,DS:HlpPage
		MOV	DS:AutoTim.Lo,0FFFFh
		CALL	Dialog
		AND	CS:ExecArcIntFlag,0FEh
		SUB	CS:[BP+SIZE BoxHd].mBase,BX
		MOV	AX,SI
		MOV	DS:Clock,AL
		MOV	DS:BlDelay,AH
		MOV	DS:HlpPage,CL
		POP	DX
		POP	ES
		DEC	DS:ExecArcPoint
		DEC	DS:ExecArcPntEnd
		MOV	SI,OFFSET TmpErr+50
		MOV	DI,DX
  @@InLine5:	LODSB
		CMP	AL,0
		JE	@@InLine6
		STOSB
		PUSH	ES
		PUSH	DS
		POP	ES
		CALL	ExecArcChar
		POP	ES
		JMP	@@InLine5
  @@InLine6:	MOV	AL,0Dh
		STOSB
		PUSH	DS
		POP	ES
		CALL	ExecArcChar
		MOV	SI,DI
		MOV	AX,DI
		SUB	AX,DX
		RET
ExecArcInLine	ENDP

MaxDialLines	EQU	5

ExecArcDialog	PROC	NEAR
		USES	CX,SI,DI,DS,ES
		PUSH	ES
		POP	DS
		MOV	BL,AL
		MOV	SI,OFFSET ExecArcScrBuf
		MOV	DI,OFFSET TmpErr+2
  @@CheckEnd:	CMP	DI,OFFSET TmpErr+2*127
		JB	@@SetPointer
		PUSH	SI
		MOV	SI,OFFSET TmpErr+2+2
		MOV	DI,OFFSET TmpErr+2
		MOV	CX,125
		REP	MOVSW
		POP	SI
  @@SetPointer:	MOV	AX,SI
		STOSW
		CMP	SI,DS:ExecArcPntEnd
		JAE	@@Analise
  @@NextLine:	LODSB
		CMP	AL,0Ah
		JNE	@@NextLine
		JMP	@@CheckEnd
  @@Analise:	XOR	AX,AX
		STOSW
		STD				; Cut empty lines at end
		LEA	SI,[DI-4]
  @@EmptyEnd:	LODSW
		CMP	SI,OFFSET TmpErr+2
		JBE	@@CutLines
		SUB	AX,DS:[SI]
		DEC	AX
		JNE	@@CutLines
		OR	BL,BL
		JE	@@CutLines
		MOV	DS:[SI+2],AX
		JMP	@@EmptyEnd
  @@CutLines:	CLD				; Leave only N lines at end
		MOV	DI,OFFSET TmpErr+2
		SUB	SI,2*(MaxDialLines-1)
		CMP	SI,DI
		JBE	@@BoxSize
  @@EmptyBeg:	LODSW				; Cut empty lines at beginning
		INC	AX
		CMP	AX,DS:[SI]
		JE	@@EmptyBeg
		LODSW
		SUB	SI,4
		OR	AX,AX
		JNE	@@MovePoint
		DEC	SI
		DEC	SI
  @@MovePoint:	MOV	CX,MaxDialLines+2
		REP	MOVSW
  @@BoxSize:	MOV	SI,OFFSET TmpErr+2+2	; Calculate size of box
		MOV	BX,30
		XOR	CX,CX
  @@BoxSiz1:	LODSW
		OR	AX,AX
		JE	@@Exit
		INC	CX
		SUB	AX,DS:[SI-4]
		DEC	AX
		MOV	DI,AX
		CMP	AX,BX
		JBE	@@BoxSiz1
		MOV	BX,AX
		CMP	BX,65
		JBE	@@BoxSiz1
		MOV	BX,65
		JMP	@@BoxSiz1
  @@Exit:	MOV	BH,CL
		MOV	AX,DI
		RET
ExecArcDialog	ENDP

ExecArcDialCall	PROC	NEAR
		CMP	AX,0FFFEh
		JNE	@@Exit
		ADD	BX,205h
		MOV	SI,OFFSET TmpErr+2
		LODSW
  @@OutLine:	MOV	DX,AX
		LODSW
		OR	AX,AX
		JE	@@Exit
		PUSH	SI
		PUSH	AX
		MOV	SI,DX
		MOV	DX,AX
		SUB	AX,SI
		DEC	AX
		JE	@@NextLine
		MOV	CL,DS:ListWindow.X
		MOV	CH,0
		CMP	CX,AX
		JBE	@@Write
		MOV	CX,AX
  @@Write:	CALL	BasAdr
  @@Move:	MOVSB
		INC	DI
		LOOP	@@Move
  @@NextLine:	POP	AX
		POP	SI
		INC	BH
		JMP	@@OutLine
  @@Exit:	RET
ExecArcDialCall	ENDP

ExecArcInChCall	PROC	NEAR
		PUSH	BX
		PUSH	AX
		CALL	ExecArcDialCall
		POP	AX
		POP	BX
		ADD	BX,105h
		ADD	BH,DS:ListWindow.Y
		ADD	BL,DS:TmpErr[50]
		CMP	AX,0FFFEh
		JA	@@SetCursor
		JB	@@CheckKey
		MOV	AH,CS:[BP+1]
		CALL	Color
		CALL	BasAdr
		MOV	ES:[DI+1],AH
		JMP	@@Exit
  @@SetCursor:	MOV	AX,BX
		CALL	Cursor
		JMP	@@Exit
  @@CheckKey:	CMP	AH,0FCh
		JAE	@@Exit
		MOV	AH,0
		CMP	AL,0
		JE	@@Exit
		MOV	DS:TmpErr[51],AL
		MOV	AX,1C0Dh
  @@Exit:	RET
ExecArcInChCall	ENDP

ExecArcChar	PROC	NEAR
		USES	AX,BX,CX,DX,SI,DI,BP,DS
		PUSH	ES
		POP	DS
		CMP	AL,7
		JNE	@@Cr0
		CMP	DS:ErrSnd,0
		JE	@@Exit
		MOV	AH,0Eh
		PUSH	BP
		PUSHF
		CALL	CS:ExecArcInt10
		POP	BP
		JMP	@@Exit
  @@Cr0:	MOV	DI,DS:ExecArcPoint
		CMP	AL,0Dh
		JNE	@@BackSp0
  @@Cr1:	CMP	DI,OFFSET ExecArcScrBuf
		JBE	@@Cr2
		DEC	DI
		CMP	BYTE PTR DS:[DI],0Ah
		JNE	@@Cr1
		INC	DI
  @@Cr2:	MOV	DS:ExecArcPoint,DI
		JMP	@@Exit
  @@BackSp0:	CMP	AL,8
		JNE	@@Lf
		CMP	DI,OFFSET ExecArcScrBuf
		JBE	@@Exit
		CMP	BYTE PTR DS:[DI-1],0Ah
		JE	@@Exit
		DEC	DI
		JMP	@@Cr2
  @@Lf:		CMP	AL,0Ah
		JNE	@@InsChar
		MOV	DI,DS:ExecArcPntEnd
  @@InsChar:	CMP	DI,OFFSET ExecArcBufEnd
		JB	@@SaveChar
		MOV	DI,OFFSET ExecArcScrBuf	; Delete oldest line if overflow
		MOV	DS:ExecArcPntEnd,DI
		PUSH	DI
		LEA	SI,[DI+1]
		MOV	CX,LenHelp
		MOV	AL,0Ah
		REPNE	SCASB
		JNE	@@Reduce
		MOV	SI,DI
  @@Reduce:	POP	DI
		MOV	CX,OFFSET ExecArcBufEnd
		SUB	CX,SI
		JBE	@@SaveChar
		REP	MOVSB
  @@SaveChar:	STOSB				; Save char
		MOV	DS:ExecArcPoint,DI
		CMP	DI,DS:ExecArcPntEnd
		JBE	@@ChkCmd
		MOV	DS:ExecArcPntEnd,DI
  @@ChkCmd:	CMP	DS:ExecArcCommand,1
		JNE	@@Exit
		CMP	AL,0Ah
		JNE	@@Exit

		MOV	SI,OFFSET ExecArcScrBuf	; Check lines
  @@CheckLine0:	MOV	BX,OFFSET LineBuf1
  @@CheckLine1:	XOR	DX,DX
		MOV	DS:ExecArcScript,BX
		MOV	DS:ExecArcNameLen,DX
		MOV	DS:ExecArcPathLen,DX
  @@CheckLine2:	PUSH	SI
		MOV	DI,OFFSET ExecArcEntry	; Clear entry
		MOV	CX,SIZE FilEnt
		XOR	AX,AX
		REP	STOSB
		MOV	DS:ExecArcCompSize.Lo,0FFFFh
		MOV	DS:ExecArcCompSize.Hi,0FFFFh
		MOV	DS:ExecArcNameBeg,AX
		MOV	DS:ExecArcPathBeg,AX
		MOV	DS:ExecArcStatus,AL
		MOV	BX,DS:ExecArcScript
  @@CheckItem:	CALL	FndLet
		MOV	AL,DS:[BX]
		INC	BX
		CMP	AL,2			; Check end of line
		JB	@@GetItem
		JNE	@@CheckName
		LODSB
		CMP	AL,0Ah
		JNE	@@NextCheck
		CMP	SI,DS:ExecArcPntEnd
		JB	@@CheckItem
		CMP	BYTE PTR DS:[BX],1
		JBE	@@GetItem
		POP	SI
		JMP	@@Exit

  @@CheckName:	CMP	AL,4			; Check file name or path
		JA	@@CheckEof
		MOV	CX,0FEh
		CMP	BYTE PTR DS:[BX],2
		JBE	@@CheckNam0
		MOV	CL,DL
		CMP	AL,3
		JE	@@CheckNam0
		MOV	CL,DH
  @@CheckNam0:	INC	CX
		MOV	AH,AL
		LODSB
		CMP	AL,'\'
		JE	@@CheckNam1
		CMP	AL,'/'
		JE	@@CheckNam1
		DEC	SI
  @@CheckNam1:	MOV	BP,SI
  @@CheckNam2:	LODSB
		CMP	AL,' '
		JE	@@CheckNam4
  @@CheckNam3:	CMP	AL,9
		JE	@@CheckNam6
		CMP	AL,0Ah
		JE	@@CheckNam6
		PUSH	ES
		PUSH	CX
		PUSH	CS
		POP	ES
		MOV	DI,OFFSET FileChars_S
		MOV	CX,11
		REPNE	SCASB
		POP	CX
		POP	ES
		JNE	@@CheckNam2
		CMP	BYTE PTR DS:[BX],14
		JE	@@CheckNam6
		JMP	@@NextLine
  @@CheckNam4:	DEC	CX
		JE	@@CheckNam6
  @@CheckNam5:	LODSB
		CMP	AL,' '
		JE	@@CheckNam5
		JMP	@@CheckNam3
  @@CheckNam6:	DEC	SI
		MOV	CX,SI
		SUB	CX,BP
		CMP	AH,3
		JNE	@@CheckNam8
		CMP	CX,DS:ExecArcNameLen
		JE	@@CheckNam7
		MOV	DS:ExecArcNameLen,CX
		MOV	DS:ExecArcNameBeg,BP
		MOV	DS:ExecArcStatus,1
		MOV	AL,DS:[BX]

;##
		MOV	DS:ExecArcEntry.NameF[0],AL

		JMP	@@CheckItem
  @@CheckNam7:	CMP	DS:ExecArcStatus,0
		JE	@@NextLine
		MOV	DL,0
		MOV	DS:ExecArcNameLen,0
		JMP	@@NextCheck
  @@CheckNam8:	JCXZ	@@CheckNam9
		CMP	CX,DS:ExecArcPathLen
		JNE	@@CheckNam9
		CMP	DS:ExecArcStatus,0
		JE	@@NextLine
		MOV	DH,0
		MOV	DS:ExecArcPathLen,0
		JMP	@@NextLine
  @@CheckNam9:	MOV	DS:ExecArcPathLen,CX
		MOV	DS:ExecArcPathBeg,BP
		MOV	DS:ExecArcStatus,2
		JMP	@@CheckItem

  @@CheckEof:	CMP	BYTE PTR DS:[SI],0Ah
		JNE	@@CheckSize
		INC	SI
		CMP	SI,DS:ExecArcPntEnd
		JBE	@@NextLine
		POP	SI
		JMP	@@Exit

  @@CheckSize:	CMP	AL,6			; Check size, compressed size
		JA	@@CheckAttr
		PUSH	SI
		PUSH	DX
		PUSH	BX
		PUSH	AX
		XOR	BX,BX
		XOR	DI,DI
  @@CheckSiz1:	LODSB
		CMP	AL,','
		JNE	@@CheckSiz2
		LODSB
		DEC	SI
		CMP	AL,'0'
		JB	@@CheckSiz4
		CMP	AL,'9'
		JA	@@CheckSiz4
		INC	SI
  @@CheckSiz2:	SUB	AL,'0'
		JB	@@CheckSiz4
		CMP	AL,9
		JA	@@CheckSiz4
		MOV	AH,0
		MOV	CX,BX
		MOV	BP,DI
		ADD	BX,BX
		ADC	DI,DI
		JC	@@CheckSiz3
		ADD	BX,BX
		ADC	DI,DI
		JC	@@CheckSiz3
		ADD	BX,CX
		ADC	DI,BP
		JC	@@CheckSiz3
		ADD	BX,BX
		ADC	DI,DI
		JC	@@CheckSiz3
		ADD	BX,AX
		ADC	DI,0
		JC	@@CheckSiz3
		JMP	@@CheckSiz1
  @@CheckSiz3:	XOR	SI,SI
  @@CheckSiz4:	POP	AX
		CMP	AL,5
		JNE	@@CheckSiz5
		MOV	DS:ExecArcEntry.SizeF.Lo,BX
		MOV	DS:ExecArcEntry.SizeF.Hi,DI
		JMP	@@CheckSiz6
  @@CheckSiz5:	MOV	DS:ExecArcCompSize.Lo,BX
		MOV	DS:ExecArcCompSize.Hi,DI
  @@CheckSiz6:	POP	BX
		POP	DX
		POP	AX
		DEC	SI
		CMP	SI,AX
		JBE	@@NextCheck
  @@CheckSiz7:	LODSB
		CMP	AL,' '
		JE	@@CheckItem
		CMP	AL,9
		JE	@@CheckItem
		CMP	AL,0Ah
		JE	@@CheckSiz8
		CMP	BYTE PTR DS:[BX],14
		JNE	@@NextCheck
  @@CheckSiz8:	DEC	SI
		JMP	@@CheckItem

  @@CheckAttr:	CMP	AL,7			; Check attributes
		JNE	@@CheckDate
  @@CheckAtr1:	LODSB
		CMP	AL,' '
		JE	@@CheckItem
		CMP	AL,9
		JE	@@CheckItem
		CMP	AL,0Ah
		JE	@@CheckSiz8
		CALL	UpCase
		MOV	AH,1
		CMP	AL,'R'
		JE	@@CheckAtr2
		MOV	AH,2
		CMP	AL,'H'
		JE	@@CheckAtr2
		MOV	AH,4
		CMP	AL,'S'
		JE	@@CheckAtr2
		MOV	AH,8
		CMP	AL,'V'
		JE	@@CheckAtr2
		MOV	AH,10h
		CMP	AL,'D'
		JE	@@CheckAtr2
		MOV	AH,20h
		CMP	AL,'A'
		JNE	@@CheckAtr1
  @@CheckAtr2:	OR	DS:ExecArcEntry.AttrF,AH
		JMP	@@CheckAtr1

  @@CheckDate:	CMP	AL,8			; Check date
		JNE	@@CheckDay
		PUSH	SI
		CALL	ExecArcYear
		JC	@@CheckDat01
		JNZ	@@CheckDat01
		MOV	DI,AX
		CALL	ExecArcDateSep
		JC	@@CheckDat01
		CALL	ExecArcMonth
		JC	@@CheckDat01
		OR	DI,AX
		CALL	ExecArcDateSep
		JC	@@CheckDat01
		CALL	ExecArcDay
		JC	@@CheckDat01
		OR	AX,DI
		POP	CX
		JMP	@@CheckDat11
  @@CheckDat01:	POP	SI
		PUSH	SI
		CALL	ExecArcMonth
		JC	@@CheckDat02
		JZ	@@CheckDat02
		MOV	DI,AX
		CALL	ExecArcDateSep
		JC	@@CheckDat02
		CALL	ExecArcDay
		JC	@@CheckDat02
		OR	DI,AX
		CALL	ExecArcDateSep
		JC	@@CheckDat02
		CALL	ExecArcYear
		JC	@@CheckDat02
		OR	AX,DI
		POP	CX
		JMP	@@CheckDat11
  @@CheckDat02:	POP	SI
		PUSH	SI
		CALL	ExecArcDay
		JC	@@CheckDat03
		MOV	DI,AX
		CALL	ExecArcDateSep
		JC	@@CheckDat03
		CALL	ExecArcMonth
		JC	@@CheckDat03
		JZ	@@CheckDat03
		OR	DI,AX
		CALL	ExecArcDateSep
		JC	@@CheckDat03
		CALL	ExecArcYear
		JC	@@CheckDat03
		JNZ	@@CheckDat03
		OR	AX,DI
		POP	CX
		JMP	@@CheckDat11
  @@CheckDat03:	POP	SI
		CMP	DS:Country.DateFmt,1
		JE	@@CheckDat04
		JA	@@CheckDat05
		CALL	ExecArcMonth
		JMP	@@CheckDat06
  @@CheckDat04:	CALL	ExecArcDay
		JMP	@@CheckDat06
  @@CheckDat05:	CALL	ExecArcYear
  @@CheckDat06:	JC	@@NextCheck
		MOV	DI,AX
		CALL	ExecArcDateSep
		JC	@@NextCheck
		CMP	DS:Country.DateFmt,1
		JAE	@@CheckDat07
		CALL	ExecArcDay
		JMP	@@CheckDat08
  @@CheckDat07:	CALL	ExecArcMonth
  @@CheckDat08:	JC	@@NextCheck
		OR	DI,AX
		CALL	ExecArcDateSep
		JC	@@NextCheck
		CMP	DS:Country.DateFmt,1
		JA	@@CheckDat09
		CALL	ExecArcYear
		JMP	@@CheckDat10
  @@CheckDat09:	CALL	ExecArcDay
  @@CheckDat10:	JC	@@NextCheck
		OR	AX,DI
  @@CheckDat11:	MOV	DS:ExecArcEntry.DateF,AX
		JMP	@@CheckSiz7

  @@CheckDay:	CMP	AL,9			; Check day
		JNE	@@CheckMonth
		CALL	ExecArcDay
		JC	@@NextCheck
		AND	DS:ExecArcEntry.DateF,0FFE0h
		OR	DS:ExecArcEntry.DateF,AX
		JMP	@@CheckSiz7

  @@CheckMonth:	CMP	AL,10			; Check month
		JNE	@@CheckYear
		CALL	ExecArcMonth
		JC	@@NextCheck
		AND	DS:ExecArcEntry.DateF,0FE1Fh
		OR	DS:ExecArcEntry.DateF,AX
		JMP	@@CheckSiz7

  @@CheckYear:	CMP	AL,11			; Check year
		JNE	@@CheckTime
		CALL	ExecArcYear
		JC	@@NextCheck
		AND	DS:ExecArcEntry.DateF,01FFh
		OR	DS:ExecArcEntry.DateF,AX
		JMP	@@CheckSiz7

  @@CheckTime:	CMP	AL,12			; Check time
		JNE	@@CheckDir
  		XOR	DI,DI
		MOV	BP,SI
		CALL	TxtNum
		JC	@@NextCheck
		MOV	CH,AL
		MOV	AX,SI
		SUB	AX,BP
		CMP	AX,2
		JA	@@NextCheck
		LODSB
		CALL	UpCase
		CMP	AL,':'
		JE	@@CheckTim01
		CMP	AL,DS:Country.TimeSep
		JNE	@@NextCheck
  @@CheckTim01:	MOV	BP,SI
		CALL	TxtNum
		JC	@@NextCheck
		MOV	CL,AL
		MOV	AX,SI
		SUB	AX,BP
		CMP	AX,2
		JA	@@NextCheck
		LODSB
		CALL	UpCase
		CMP	AL,':'
		JE	@@CheckTim02
		CMP	AL,DS:Country.TimeSep
		JNE	@@CheckTim03
  @@CheckTim02:	MOV	BP,SI
		CALL	TxtNum
		JC	@@NextCheck
		MOV	DI,AX
		MOV	AX,SI
		SUB	AX,BP
		CMP	AX,2
		JA	@@NextCheck
		LODSB
  @@CheckTim03:	CALL	UpCase
		CMP	AL,' '
		JE	@@CheckTim07
		MOV	AH,0
		CMP	AL,'A'
		JE	@@CheckTim04
		CMP	AL,'P'
		JNE	@@CheckTim06
		MOV	AH,12
  @@CheckTim04:	CMP	CH,12
		JNE	@@CheckTim05
		MOV	CH,0
  @@CheckTim05:	ADD	CH,AH
		LODSB
		CALL	UpCase
		CMP	AL,'M'
		JE	@@CheckTim11
  @@CheckTim06:	DEC	SI
		JMP	@@CheckTim11
  @@CheckTim07:	LODSW
		CALL	UpCase
		XCHG	AL,AH
		CALL	UpCase
		CMP	AX,'PM'
		JE	@@CheckTim08
		SUB	AX,'AM'
		JE	@@CheckTim09
		SUB	SI,3
		JMP	@@CheckTim11
  @@CheckTim08:	MOV	AL,12
  @@CheckTim09:	CMP	CH,12
		JNE	@@CheckTim10
		MOV	CH,0
  @@CheckTim10:	ADD	CH,AL
  @@CheckTim11:	CMP	CH,32
		JAE	@@NextCheck
		CMP	CL,64
		JAE	@@NextCheck
		CMP	DI,64
		JAE	@@NextCheck
		MOV	AX,CX
		MOV	CL,2
		SHL	AL,CL
		MOV	CL,3
		SHL	AX,CL
		SHR	DI,1
		OR	AX,DI
		MOV	DS:ExecArcEntry.TimeF,AX
		JMP	@@CheckSiz7

  @@CheckDir:	CMP	AL,13			; Set directory flag
		JNE	@@CheckConst
		MOV	DS:ExecArcEntry.AttrF,10h
		JMP	@@CheckItem

  @@CheckConst:	CMP	AL,14			; Check text constant
		JNE	@@CheckWord
		MOV	CL,DS:[BX]
		MOV	CH,0
		INC	BX
  @@CheckCons1:	LODSB
		CMP	AL,DS:[BX]
		JNE	@@CheckCons2
		INC	BX
		LOOP	@@CheckCons1
		JMP	@@CheckItem
  @@CheckCons2:	ADD	BX,CX
		JMP	@@NextCheck

  @@CheckWord:	LODSB				; Check any word
		CMP	AL,' '
		JE	@@CheckItem
		CMP	AL,9
		JE	@@CheckItem
		CMP	AL,0Ah
		JNE	@@CheckWord
		DEC	SI
		JMP	@@CheckItem

  @@NextCheck:	POP	SI			; Check line again, more spaces
		CMP	DS:ExecArcStatus,1
		JB	@@NextChk3
		JA	@@NextChk1
		INC	DL
		JMP	@@NextChk2
  @@NextChk1:	INC	DH
  @@NextChk2:	JNE	@@CheckLine2
  @@NextChk3:	PUSH	SI

  @@NextLine:	MOV	AL,DS:[BX]		; Check next part
		INC	BX
		CMP	AL,14
		JNE	@@NextLin1
		MOV	AL,DS:[BX]
		MOV	AH,0
		INC	BX
		ADD	BX,AX
		JMP	@@NextLine
  @@NextLin1:	CMP	AL,1
		JA	@@NextLine
		POP	SI
		JE	@@CheckLine1
		MOV	DI,SI			; Check next line
		MOV	CH,0FFh
		MOV	AL,0Ah
		REPNE	SCASB
		CMP	DI,DS:ExecArcPntEnd
		JAE	@@Exit
		MOV	SI,DI
		JMP	@@CheckLine0

  @@GetItem:	PUSH	ES			; Analize path and name
		MOV	DI,DS:ExecArcPathEnd
		MOV	SI,DS:ExecArcPathBeg
		OR	SI,SI
		JE	@@MakeName3
		MOV	DI,OFFSET ExecArcNameBuf
		MOV	CX,DS:ExecArcPathLen
		JCXZ	@@MakeName1
		MOV	DS:ExecArcPathEnd,0
		CALL	ExecArcMoveName
		JNC	@@SkipEntry
		CALL	AddPathSep
		CMP	DI,OFFSET ExecArcNameEnd
		JAE	@@SkipEntry
  @@MakeName1:	MOV	AX,DI
		CMP	DS:ExecArcNameBeg,0
		JE	@@MakeName2
		MOV	AX,OFFSET ExecArcNameBuf
  @@MakeName2:	MOV	DS:ExecArcPathEnd,AX
  @@MakeName3:	MOV	BP,DI
		OR	DI,DI
		JE	@@SkipEntry
		MOV	SI,DS:ExecArcNameBeg
		OR	SI,SI
		JE	@@MakeName4
		MOV	CX,DS:ExecArcNameLen
		CALL	ExecArcMoveName
		JNC	@@SkipEntry
  @@MakeName4:	MOV	AL,0
		STOSB

;##
		CMP	DS:ExecArcEntry.NameF[0],2 ; Check name 'FILENAME EXT'
		JBE	@@CheckDir0
		MOV	SI,BP
  @@GetName1:	LODSB	
		CMP	AL,'.'
		JE	@@CheckDir0
		CMP	AL,'\'
		JE	@@CheckDir0
		MOV	AH,AL
		CALL	UpCase
		CMP	AL,AH
		JNE	@@CheckDir0
  @@GetName2:	CMP	AL,0
		JNE	@@GetName1
		SUB	SI,BP
		DEC	SI
		CMP	SI,8
		JBE	@@CheckDir0
		CMP	SI,12
		JA	@@CheckDir0
		LEA	DI,[BP+8]
		CMP	BYTE PTR DS:[DI],' '
		JNE	@@CheckDir0
  @@GetName3:	CMP	DI,BP
		JBE	@@GetName4
		DEC	DI
		CMP	BYTE PTR DS:[DI],' '
		JE	@@GetName3
		INC	DI
  @@GetName4:	MOV	AL,'.'
		STOSB
		LEA	SI,[BP+9]
		CALL	strcpy

  @@CheckDir0:	MOV	ES,DS:ExecArcBufSeg	; Save entry
		CALL	ExecArcSetEntry
  @@SkipEntry:	POP	ES
		POP	SI
		MOV	DS:ExecArcPoint,OFFSET ExecArcScrBuf
		MOV	DS:ExecArcPntEnd,OFFSET ExecArcScrBuf
  @@Exit:	RET
ExecArcChar	ENDP

ExecArcSetEntry	PROC	NEAR
		TEST	DS:ExecArcEntry.AttrF,8
		JNZ	@@Exit
		TEST	DS:ExecArcEntry.AttrF,10h
		JZ	@@NormStr
		OR	DS:ExecArcEntry.AttrF,80h
  @@NormStr:	PUSH	ES			; Remove spaces
		PUSH	DS
		POP	ES
		MOV	SI,OFFSET ExecArcNameBuf
		CALL	NormStr
		PUSH	SI			; Remove D:
		MOV	DI,SI
		LODSW
		CMP	AH,':'
		JE	@@ChkPath1
		DEC	SI
		DEC	SI
  @@ChkPath1:	LODSB				; Remove \
		CMP	AL,'\'
		JE	@@ChkPath1
		CMP	AL,'/'
		JE	@@ChkPath1
		DEC	SI
  @@ChkPath2:	MOV	CX,SI			; Remove . and ..
		CMP	BYTE PTR DS:[SI],'.'
		JNE	@@ChkPath4
		INC	SI
		CMP	BYTE PTR DS:[SI],'.'
		JNE	@@ChkPath3
		INC	SI
  @@ChkPath3:	LODSB
		CMP	AL,'\'
		JE	@@ChkPath1
		CMP	AL,'/'
		JE	@@ChkPath1
		CMP	AL,0
		JE	@@Slash1
  @@ChkPath4:	MOV	SI,CX
  @@Slash1:	LODSB				; Convert / to \
		CMP	AL,'/'
		JNE	@@Slash2
		MOV	AL,'\'
  @@Slash2:	STOSB
		CMP	AL,0
		JNE	@@Slash1
		POP	SI
		POP	ES
		CALL	DS:ExecArcDirProc
  @@Exit:	RET
ExecArcSetEntry	ENDP

ExecArcSaveEnt	PROC	NEAR
		MOV	DI,OFFSET ArcPath	; Compare path
		CALL	StrLen
		DEC	CX
		JE	@@CheckDir2
		CALL	CompNormSubStr
		JNE	@@Exit
		LODSW
		DEC	SI
		CMP	AL,0
		JE	@@CheckDir1
		CMP	AL,'\'
		JNE	@@Exit
		CMP	AH,0
		JNE	@@CheckDir2
  @@CheckDir1:	MOV	DI,ES:DiskBuf		; Entry = current path
		MOV	AX,ES:[DI].DateF
		OR	AX,ES:[DI].TimeF
		JNE	@@Exit
		MOV	AX,DS:ExecArcEntry.DateF
		MOV	ES:[DI].DateF,AX
		MOV	AX,DS:ExecArcEntry.TimeF
		MOV	ES:[DI].TimeF,AX
		JMP	@@Exit
  @@CheckDir2:	MOV	DI,SI			; Entry is a directory
  @@CheckDir3:	LODSB
		CMP	AL,0
		JE	@@CheckDir4
		CMP	AL,'\'
		JNE	@@CheckDir3
		MOV	AL,DS:ExecArcEntry.AttrF
		OR	DS:ExecArcEntry.AttrF,90h
		CMP	BYTE PTR DS:[SI],0
		JE	@@CheckDir4
		MOV	DS:ExecArcEntry.DateF,0
		MOV	DS:ExecArcEntry.TimeF,0
		MOV	DS:ExecArcEntry.AttrF,10h
		TEST	AL,10h
		JZ	@@CheckDir4
		OR	DS:ExecArcEntry.AttrF,40h
  @@CheckDir4:	MOV	CX,SI
		SUB	CX,DI
		DEC	CX
		JE	@@Exit
		MOV	SI,DI
		TEST	DS:ExecArcEntry.AttrF,10h
		JZ	@@SaveEntry
		OR	DS:ExecArcEntry.AttrF,8
		CALL	ExecArcChkDupe
		JC	@@SaveEntry
		MOV	AX,DS:ExecArcEntry.SizeF.Lo
		ADD	ES:[DI].SizeF.Lo,AX
		MOV	AX,DS:ExecArcEntry.SizeF.Hi
		ADC	ES:[DI].SizeF.Hi,AX
		TEST	DS:ExecArcEntry.AttrF,80h
		JZ	@@SaveDir0
		MOV	AX,ES:[DI].DateF
		OR	AX,ES:[DI].TimeF
		JNE	@@SaveDir0
		TEST	ES:[DI].AttrF,10h
		JE	@@SaveDir0
		MOV	AL,DS:ExecArcEntry.AttrF
		AND	AL,3Fh
		MOV	ES:[DI].AttrF,AL
		MOV	AX,DS:ExecArcEntry.TimeF
		MOV	ES:[DI].TimeF,AX
		MOV	AX,DS:ExecArcEntry.DateF
		MOV	ES:[DI].DateF,AX
		JMP	@@SaveDir0
  @@Overflow:	MOV	AL,1			; Overflow
		XCHG	AL,DS:ExecArcOverflow
		CMP	AL,0
		JNE	@@Exit
		CALL	ExecArcBeep
		JMP	@@Exit
  @@SaveEntry:	MOV	DI,DS:ExecArcDirBuf	; Save file entry
		INC	CX
		MOV	AX,CX
		ADD	AX,SIZE FilEnt
		ADD	AX,DI
		JC	@@Overflow
		CMP	AX,DS:ExecArcDirEnd
		JA	@@Overflow
		MOV	DS:ExecArcEntry.LenShortNamF,CX
		MOV	DS:ExecArcEntry.LenLongNamF,0
		DEC	CX
		PUSH	SI
		PUSH	CX
		MOV	SI,OFFSET ExecArcEntry
		MOV	AX,DS:ExecArcNumbF
		MOV	DS:[SI].NumbF,AX
		INC	DS:ExecArcNumbF
		MOV	AH,DS:[SI].AttrF
		AND	DS:[SI].AttrF,3Fh
		MOV	CX,SIZE FilEnt
		REP	MOVSB
		POP	CX
		POP	SI
		PUSH	SI
		PUSH	CX
		AND	AH,0FEh
		TEST	AH,10h
		JNZ	@@MoveName
		CMP	DS:ConvCase,0
		JZ	@@MoveName
		MOV	AL,1
		CALL	ChkShortNam
		JC	@@MoveName
		OR	AH,1
  @@MoveName:	LODSB
		TEST	AH,1
		JZ	@@SaveChar
		CALL	LowCase
  @@SaveChar:	STOSB
		LOOP	@@MoveName
		MOV	AL,0
		STOSB
		MOV	DS:ExecArcDirBuf,DI
		POP	CX
		POP	SI
		TEST	AH,10h
		JZ	@@Exit
  @@SaveDir0:

;##
	JMP	@@Exit

		MOV	DS:ExecArcEntry.DateF,0	; Included directories
		MOV	DS:ExecArcEntry.TimeF,0
		MOV	DS:ExecArcEntry.AttrF,10h
		MOV	DS:ExecArcEntry.SizeF.Lo,0
		MOV	DS:ExecArcEntry.SizeF.Hi,0
  @@SaveDir1:	MOV	DI,SI
		ADD	SI,CX
		LODSB
		CMP	AL,0
		JE	@@Exit
  @@SaveDir2:	LODSB
		CMP	AL,'\'
		JE	@@SaveDir3
		CMP	AL,0
		JNE	@@SaveDir2
		TEST	AH,40h
		JZ	@@Exit
  @@SaveDir3:	DEC	SI
		SUB	SI,DI
		INC	CX
		CMP	CX,SI
		JE	@@Exit
		MOV	CX,SI
		MOV	SI,DI
		CALL	ExecArcChkDupe
		JNC	@@SaveDir1
		INC	CX
		MOV	DI,CX
		ADD	DI,SIZE FilEnt
		ADD	DI,DS:ExecArcDirBuf
		JC	@@Overflow
		CMP	DI,DS:ExecArcDirEnd
		JE	@@Overflow
		MOV	DI,DS:ExecArcDirBuf
		MOV	DS:ExecArcEntry.LenShortNamF,CX
		MOV	DS:ExecArcEntry.LenLongNamF,0
		DEC	CX
		PUSH	SI
		PUSH	CX
		MOV	SI,OFFSET ExecArcEntry
		MOV	CX,SIZE FilEnt
		REP	MOVSB
		POP	CX
		POP	SI
		PUSH	SI
		PUSH	CX
		REP	MOVSB
		MOV	AL,0
		STOSB
		POP	CX
		POP	SI
		MOV	DS:ExecArcDirBuf,DI
		JMP	@@SaveDir1
  @@Exit:	RET
ExecArcSaveEnt	ENDP

ExecArcBeep	PROC	NEAR
		CMP	DS:ErrSnd,0
		JZ	@@Exit
		MOV	DL,7
		MOV	AH,6
		INT	21h
  @@Exit:	RET
ExecArcBeep	ENDP

ExecArcChkDupe	PROC	NEAR			; Check dupe dirs
		MOV	DI,ES:DiskBuf
  @@CompDir:	CMP	DI,DS:ExecArcDirBuf
		JAE	@@NotFound
		PUSH	DI
		PUSH	SI
		PUSH	CX
		ADD	DI,OFFSET NameF
		CALL	CompNormSubStr
		JNE	@@ExitCmp
		CMP	BYTE PTR ES:[DI],0
  @@ExitCmp:	POP	CX
		POP	SI
		POP	DI
		JE	@@Exit
		PUSH	AX
		MOV	AX,ES:[DI].LenShortNamF
		ADD	DI,ES:[DI].LenLongNamF
		ADD	DI,SIZE FilEnt
		ADD	DI,AX
		POP	AX
		JMP	@@CompDir
  @@NotFound:	STC
  @@Exit:	RET
ExecArcChkDupe	ENDP

ExecArcMoveName	PROC	NEAR
		MOV	AX,DI
		ADD	AX,CX
		CMP	AX,OFFSET ExecArcNameEnd-1
		JAE	@@Exit
		REP	MOVSB
  @@Exit:	RET
ExecArcMoveName	ENDP

ExecArcDay	PROC	NEAR
		MOV	CX,SI
		CALL	TxtNum
		JC	@@Error
		SUB	CX,SI
		NEG	CX
		CMP	CX,2
		JA	@@Error
		CMP	AX,32
		JAE	@@Error
		CLC
		JMP	@@Exit
  @@Error:	STC
  @@Exit:	RET
ExecArcDay	ENDP

		.CONST
Monthes_S	DB	'JAN'
		DB	'FEB'
		DB	'MAR'
		DB	'APR'
		DB	'MAY'
		DB	'JUN'
		DB	'JUL'
		DB	'AUG'
		DB	'SEP'
		DB	'OCT'
		DB	'NOV'
		DB	'DEC'

		.CODE	Subs
ExecArcMonth	PROC	NEAR
		USES	DI
		MOV	CX,SI
		CALL	TxtNum
		JC	@@Month1
		SUB	CX,SI
		NEG	CX
		CMP	CX,2
		JA	@@Error
		CMP	AX,16
		JAE	@@Error
		XOR	DI,DI
		JMP	@@Month4
  @@Month1:	CMP	SI,CX
		JNE	@@Error
		MOV	DI,OFFSET Monthes_S-3
		MOV	CX,12
  @@Month2:	ADD	DI,3
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	CX
		PUSH	CS
		POP	ES
		MOV	CX,3
  @@Month3:	LODSB
		CALL	UpCase
		SCASB
		LOOPE	@@Month3
		POP	CX
		POP	SI
		POP	DI
		POP	ES
		LOOPNE	@@Month2
		JNE	@@Error
		ADD	SI,3
		MOV	AX,12
		SUB	AX,CX
  @@Month4:	MOV	CL,5
		SHL	AX,CL
		OR	DI,DI
		JMP	@@Exit
  @@Error:	STC
  @@Exit:	RET
ExecArcMonth	ENDP

ExecArcYear	PROC	NEAR
		USES	BP
		MOV	BP,SI
		CALL	TxtNum
		JC	@@Error
		CMP	AX,80
		JAE	@@Year1
		ADD	AX,20
		JMP	@@Year3
  @@Year1:	CMP	AX,100
		JAE	@@Year2
		SUB	AX,80
		JMP	@@Year3
  @@Year2:	SUB	AX,1980
		JB	@@Error
		CMP	AX,128
		JAE	@@Error
  @@Year3:	MOV	CL,9
		SHL	AX,CL
		MOV	CX,SI
		SUB	CX,BP
		CMP	CX,4
		JA	@@Error
		CLC
		JMP	@@Exit
  @@Error:	STC
  @@Exit:	RET
ExecArcYear	ENDP

ExecArcDateSep	PROC	NEAR
		LODSB
		CMP	AL,DS:Country.DateSep
		JE	@@Exit
		CMP	AL,'-'
		JE	@@Exit
		CMP	AL,'/'
		JE	@@Exit
		CMP	AL,'.'
		JE	@@Exit
		DEC	SI
		STC
  @@Exit:	RET
ExecArcDateSep	ENDP
