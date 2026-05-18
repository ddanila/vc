;---------------------------------------------------------------
;		Copy, Rename, Move
;
;		Version:	16.07.2000
;---------------------------------------------------------------

CopyTarget	EQU	LineBuf2	; Input buffer
CopyOverSrc	EQU	TmpErr
CopyOverDest	EQU	TmpErr+128

CopyDat		STRUC
		DB	TmpBuf+100 DUP(?)
CopyMove	DB	?		; Copy / Move / Rename
CopyTargetType	DB	?		; Target is path/file/mask
CopyBegName	DW	?		; Start address of source filename
CopySourceDev	DB	?		; Source is a device
CopyTargetDev	DB	?		; Target is a device
CopyHandle1	DW	?		; Source handle
CopyHandle2	DW	?		; Target handle
CopyRead	DD	?		; Was read
CopyWrite	DD	?		; Was written
CopySize	DD	?		; File size
CopyAttrib	DW	?		; File attributes
CopyTime	DW	?		; File time
CopyDate	DW	?		; File date
CopyDestAttrib	DW	?		; Attributes of dest file
CopyDestTime	DW	?		; Time of dest file
CopyDestDate	DW	?		; Date of dest file
CopyAppend	DB	?		; Append flag
CopyAppOffset	DD	?		; Size of target file before append
CopySplit	DB	?		; Split flag
CopySplitSeg	DW	?		; Start segment to continue copying
CopyResult	DW	?		; Result code
CopyBufEAs	DB	12 DUP(?)	; Buffer for copying EAs
CopyFlagEAs	DB	?		; Save EAs flag
CopyShortNamBuf	DB	13 DUP(?)	; Buffer for short name
CopyPathFlag	DB	?		; Create path when copy
CopyMkPath1	DW	?		; End of first new dir entry
CopyDescrNamLen	DW	?		; Length of source name in description
CopyDelEofMenu	DB	?		; Delete EOF menu
CopySplitMenu	DB	?		; Split menu
CopyWarnMenu	DB	?		; Not enough space menu
CopyDat		ENDS

		.CONST
Copy_S		DB	' Copy ',0
Move_S		DB	' Move ',0
Rename_S	DB	' Rename ',0
RenMove_S	DB	' Rename or move ',0
CopyMoveFile_S	DB	'^4C32<',0
CopyFile_S	DB	'Copying the file',0
RenMoveFile_S	DB	'Renaming or moving the file',0
CopyMoveDir_S	DB	'^5C32<',0
CopyDir_S	DB	'Copying the directory',0
RenMoveDir_S	DB	'Renaming or moving the directory',0
ExtractArc_S	DB	'Extracting files from archive',0FFh,3,0
CompressArc_S	DB	'Compressing files from',0FFh,10,0
CopyContinue_S	DB	'Continue copying to',0
MoveContinue_S	DB	'Continue moving to',0
CopyDiagram_S	DB	0FFh,32,0
To0_S		DB	'to',0FFh,30,0
To1_S		DB	'^1E61'
To2_S		DB	' to',0
CopyTo_S	DB	'to',0FFh,LenStr-2,0
CopyOver1_S	DB	'The following file exists',0
CopyOver2_S	DB	'Source:',0FFh,7,'^6E',0
CopyOver3_S	DB	'Destination:  ^7E',0
CopyReadOver_S	DB	'Do you still wish to overwrite it?',0
CopyReadApp_S	DB	'Do you still wish to append it?',0
CopyReadOnly_S	DB	'The destination file is marked read-only.',0
CopySourceRO_S	DB	'The source file is marked read-only.',0
CopyDelEof1_S	DB	'The last symbol of the following file is EOF',0
CopyDelEof2_S	DB	'Do you wish to remove EOF?',0
CopyOverMenu_S	DB	' Overwrite   All   aPpend   Skip   sKip all   Cancel ',0
CopyReadMenu_S	DB	' Yes   All   Skip   sKip all   Cancel ',0
CopyMenu_S	DB	'[^3C]   [ F10-Tree ]   [ Cancel ]',0
RenError_S	DB	"Can't rename the file",0
CopyOpenEr_S	DB	"Can't open the file",0
CopyReadEr_S	DB	"Can't read the file",0
CopyWriteEr_S	DB	"Can't write the file",0
CopyItself_S	DB	"You can't copy a file to itself",0
CopyWarning_S	DB	"^uWarning:^u "
CopySplit_S	DB	"There isn't enough room to copy",0
CopySplitMenu_S	DB	" Another drive   Split   Skip   Cancel ",0
CopyWarnMenu_S	DB	" Continue   Another drive   Skip   Cancel ",0
CopyArcErr_S	DB	"Target path does not exist",0

		.DATA
CopySel_P	DB	Box_C, BoxCu_C, Box_C, 0
		DW	Inc0_S
		DW	Line1_S
		DW	Empty_S
		DW	0FFFFh
		DW	CopyMenu_S
		DW	0
CopySel_B	BOXHD	< CopySel_P, 500h, F35_TreeSel, CopyMove_K, Fls_M, Default_H, 8 >
		BOXMN	< 1, 1,1,0,0, 305h, CopyTarget, LenStrFull+80h+LenStr >
		BOXMN	< 4, 0,0,1,1, 5EEh, Copy_D, 0 >
		DB	0
CopyMask_P	DB	Box_C, BoxCu_C, Box_C, 0
		DW	Inc0_S
		DW	Line1_S
		DW	Empty_S
		DW	CopyTo_S
		DW	Empty_S
		DW	0FFFFh
		DW	IncSubdirs_S
		DW	0FFFFh
		DW	CopyMenu_S
		DW	0
CopyMask_B	BOXHD	< CopyMask_P, 500h, F35_TreeMask, CopyMove_K, Fls_M, Default_H, 8 >
		BOXMN	< 1, 3,1,0,0, 305h, SourceFile, LenStrFull+LenStr >
		BOXMN	< 1, 0,2,1,1, 505h, CopyTarget, LenStrFull+80h+LenStr >
		BOXMN	< 3, 1,3,2,2, 706h, IncDirs, 0 >
		BOXMN	< 4, 2,0,3,3, 9EEh, Copy_D, 0 >
		DB	0
CopyContinue_P	DB	Box_C, BoxCu_C, Box_C, 0
		DW	Inc0_S
		DW	Line2_S
		DW	Empty_S
		DW	0
CopyContinue_B	BOXHD	< CopyContinue_P, 500h, 0, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 1, 0,0,0,0, 305h, TargetFile, LenStrFull+LenStr >
		DB	0
CopyMoveFile_P	DB	Box_C, 0, 0, 0
		DW	Inc0_S
		DW	CopyMoveFile_S
		DW	Line1sl_S
		DW	To0_S
		DW	Line2sl_S
		DW	CopyDiagram_S
		DW	0
CopyMoveDir_P	DB	Box_C, 0, 0, 0
		DW	Inc0_S
		DW	CopyMoveDir_S
		DW	Line1sl_S
		DW	To0_S
		DW	Line2sl_S
		DW	CopyDiagram_S
		DW	0
ExtractArc_P	DB	Box_C, 0, 0, 0
		DW	Inc0_S
		DW	ExtractArc_S
		DW	Line1sl_S
		DW	To0_S
		DW	Line2sl_S
		DW	0
CompressArc_P	DB	Box_C, 0, 0, 0
		DW	Inc0_S
		DW	CompressArc_S
		DW	Line1sl_S
		DW	To0_S
		DW	Line2sl_S
		DW	0
CopyOver_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	CopyOver1_S
		DW	Line2_S
		DW	CopyOver2_S
		DW	CopyOver3_S
		DW	CopyOverMenu_S
		DW	0
CopyOver_B	BOXHD	< CopyOver_P, 900h, 0, Dialog_K, Def_M, Default_H, 5 >
		BOXMN	< 4, 0,0,0,0, 6E5h, CopyOverMenu_D, 0FFh >
		DB	0
CopyWarning_P	DB	Err_C, ErInv_C, ErInv_C, ErBr_C
		DW	Inc0_S
		DW	CopyWarning_S
		DW	To1_S
		DW	Line2_S
		DW	CopyWarnMenu_S
		DW	0
CopyWarning_B	BOXHD	< CopyWarning_P, 900h, 0, Dialog_K, Def_M, Default_H, 5 >
		BOXMN	< 4, 0,0,0,0, 5EBh, CopyWarning_D, 0FFh >
		DB	0
CopySplit_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	CopySplit_S
		DW	To1_S
		DW	Line2_S
		DW	CopySplitMenu_S
		DW	0
CopySplit_B	BOXHD	< CopySplit_P, 900h, 0, Dialog_K, Def_M, Default_H, 5 >
		BOXMN	< 4, 0,0,0,0, 5ECh, CopySplit_D, 0FFh >
		DB	0
CopySourceEr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	CopyOpenEr_S
		DW	Line1_S
		DW	OkCan_S
		DW	0
CopySourceEr_B	BOXHD	< CopySourceEr_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
CopyTargetEr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	CopyOpenEr_S
		DW	Line2_S
		DW	OkCan_S
		DW	0
CopyTargetEr_B	BOXHD	< CopyTargetEr_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
CopyReadEr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	CopyReadEr_S
		DW	Line1_S
		DW	OkCan_S
		DW	0
CopyReadEr_B	BOXHD	< CopyReadEr_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
CopyWriteEr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	CopyWriteEr_S
		DW	Line2_S
		DW	OkCan_S
		DW	0
CopyWriteEr_B	BOXHD	< CopyWriteEr_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
CopyItself_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	CopyItself_S
		DW	Line1_S
		DW	OkCan_S
		DW	0
CopyItself_B	BOXHD	< CopyItself_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
RenError_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Rename_S
		DW	RenError_S
		DW	Line1_S
		DW	OkCan_S
		DW	0
RenError_B	BOXHD	< RenError_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
CopyDelFileEr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	DelFileErr_S
		DW	Line2_S
		DW	OkCan_S
		DW	0
CopyDelFileEr_B	BOXHD	< CopyDelFileEr_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
CopyReadOver_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	CopyReadOnly_S
		DW	Line2_S
		DW	CopyReadOver_S
		DW	CopyReadMenu_S
		DW	0
CopyReadOver_B	BOXHD	< CopyReadOver_P, 900h, 0, Dialog_K, Def_M, Default_H, 5 >
		BOXMN	< 4, 0,0,0,0, 5EDh, CopyReadMenu_D, 0FFh >
		DB	0
CopyReadApp_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	CopyReadOnly_S
		DW	Line2_S
		DW	CopyReadApp_S
		DW	CopyReadMenu_S
		DW	0
CopyReadApp_B	BOXHD	< CopyReadApp_P, 900h, 0, Dialog_K, Def_M, Default_H, 5 >
		BOXMN	< 4, 0,0,0,0, 5EDh, CopyReadMenu_D, 0FFh >
		DB	0
CopySourceRO_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	CopySourceRO_S
		DW	Line1_S
		DW	DelStill_S
		DW	DelAllSkip_S
		DW	0
CopySourceRO_B	BOXHD	< CopySourceRO_P, 900h, 0, Dialog_K, Def_M, Default_H, 5 >
		BOXMN	< 4, 0,0,0,0, 5EBh, DelAllSkip_D, 0FFh >
		DB	0
CopyDelEof_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	CopyDelEof1_S
		DW	Line2_S
		DW	CopyDelEof2_S
		DW	YesNo_S
		DW	0
CopyDelEof_B	BOXHD	< CopyDelEof_P, 900h, 0, Dialog_K, Def_M, Default_H, 5 >
		BOXMN	< 4, 0,0,0,0, 5FBh, YesNo_D, 0FFh >
		DB	0
CopyMkDirErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	MkDirError_S
		DW	Line2_S
		DW	OkCan_S
		DW	0
CopyMkDirErr_B	BOXHD	< CopyMkDirErr_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
MoveDelDirErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	DelDirErr_S
		DW	Line1_S
		DW	OkCan_S
		DW	0
MoveDelDirErr_B	BOXHD	< MoveDelDirErr_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
CopyArcErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	CopyArcErr_S
		DW	Line1_S
		DW	Ok_S
		DW	0
CopyArcErr_B	BOXHD	< CopyArcErr_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0

Copy_T		DW	Copy_S, LineBuf1, LineBuf2, Copy_S, CopyFile_S, CopyDir_S, CopyOverSrc, CopyOverDest
RenMove_T	DW	RenMove_S, LineBuf1, LineBuf2, Move_S, RenMoveFile_S, RenMoveDir_S, CopyOverSrc, CopyOverDest
CopyMask_T	DW	Copy_S, SourceFile
RenMoveMask_T	DW	RenMove_S, SourceFile
CopyArc_T	DW	Copy_S, LineBuf1, ExecArcProgram
MoveArc_T	DW	Move_S, LineBuf1, ExecArcProgram

Copy_D		DB	 8, 0
		DB	12,11
		DB	10,26
		DB	 0
CopyOverMenu_D	DB	11, 0
		DB	 5,12
		DB	 8,18
		DB	 6,27
		DB	10,34
		DB	 8,45
		DB	 0
CopyReadMenu_D	DB	 5, 0
		DB	 5, 6
		DB	 6,12
		DB	10,19
		DB	 8,30
		DB	 0
CopySplit_D	DB	15, 0
		DB	 7,16
		DB	 6,24
		DB	 8,31
		DB	 0
CopyWarning_D	DB	10, 0
		DB	15,11
		DB	 6,27
		DB	 8,34
		DB	 0

		.CODE
F36_00:		MOV	AL,1			; Rename / Move
		JMP	F35_01
F35_00:		MOV	AL,0			; Copy
F35_01:		MOV	ES:CopyMove,AL
		MOV	AH,AL
		ADD	AH,Copy_H
		MOV	ES:HlpPage,AH
		CMP	DS:Visible,0
		JZ	F45_01
		CMP	DS:WinTyp,p_Tree
		JA	F45_01			; Info or QuickView panel
		JNE	F35_02                  ; File panel
		CMP	DS:CrnPath[3],0		; Tree panel
		JE	Func04
		MOV	SI,OFFSET CrnPath
		PUSH	SI
		JMP	F35_06
F35_02:		CMP	DS:ArcPnt,0		; Copy / Move from archive
		JE	F35_04
		MOV	DI,OFFSET LineBuf1
		PUSH	DS
		PUSH	DI
		MOV	AX,'.A'
		STOSW
		MOV	SI,OFFSET ArcExt
		CALL	strcpy
		PUSH	ES
		POP	DS
		POP	SI
		MOV	DL,1
		CALL	ChkArcExt
		POP	DS
		JC	F35_03
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,CX
		MOV	AL,2
		CALL	FindArcCmd
		POP	DS
		JC	F35_03
		CMP	ES:CopyMove,0
		JE	F35_04
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,CX
		MOV	AL,4
		CALL	FindArcCmd
		POP	DS
		JNC	F35_04
F35_03:		MOV	DI,OFFSET LineBuf1
		MOV	SI,OFFSET ArcExt
		CALL	strcpy
		MOV	AL,':'
		STOSB
		CALL	PutArcPathName
		JMP	F35_11
F35_04:		CMP	ES:OneFile,0		; Selected files
		JNZ	F35_05
		CALL	TotSel
		MOV	AX,BX
		OR	AX,DX
		JE	F35_05
		XOR	SI,SI
		PUSH	CX
		CALL	F35_Target
		POP	CX
		MOV	AL,0
		CALL	FilDir
		XOR	BX,BX
		JMP	F35_09
F35_05:		MOV	SI,DS:FileCur		; One file
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	Func04
		MOV	BX,SI
		CALL	LongNameOffset
		CALL	ChkUpDir
		JE	Func04
		PUSH	SI
		CMP	DS:ArcPnt,0
		JE	F35_06
		XOR	SI,SI
F35_06:		PUSH	CX
		CALL	F35_Target
		POP	BP
		POP	SI
		PUSH	DI
		MOV	DI,OFFSET TmpErr
		PUSH	DI
		CMP	SI,OFFSET CrnPath
		JNE	F35_07
		CALL	LongTreePath
		JMP	F35_08
F35_07:		CALL	strcpy
F35_08:		PUSH	ES
		POP	DS
		POP	SI
		POP	DI
		MOV	AX,CX
		CALL	ShrtLin
		MOV	AL,'"'
		STOSB
		CALL	strcpy
		MOV	AL,'"'
		STOSB
		MOV	CX,BP
F35_09:		MOV	DS,CS:SubsSeg
		MOV	SI,OFFSET To2_S
		CALL	strcpy
		PUSH	CX
		MOV	CX,OFFSET LineBuf1+LenStr
		SUB	CX,DI
		MOV	AL,' '
		REP	STOSB
		MOV	AL,0
		STOSB
		POP	CX
		PUSH	ES			; Dialog
		POP	DS
		MOV	BP,OFFSET CopySel_B
		CALL	F35_Table
		CALL	Dialog
		JNE	Panels00
		PUSH	CX
		MOV	SI,OFFSET TargetFile
		MOV	DI,OFFSET LineBuf3
		PUSH	SI
		CALL	MvzHist
		POP	DI
		MOV	SI,OFFSET CopyTarget
		MOV	CX,LenStrFull+2
		CALL	SaveHistory
		POP	CX
		CMP	DS:TargetFile[0],0
		JE	Panels00
		CALL	F35_Init
		MOV	DS:CopyBegName,OFFSET LineBuf1
		MOV	DS,DS:WCBcrn
		CMP	DS:WinTyp,p_File
		JNE	F35_20
		CMP	DS:ArcPnt,0
		JNE	F35_30
		MOV	SI,OFFSET TargetFile
		CALL	ParseArcName
		JNC	F35_10
		CMP	AX,53h
		JE	Func04
		JMP	FuncSelect
F35_10:		PUSH	DS			; Copy / Move to archive
		PUSH	SI
		PUSH	ES
		POP	DS
		MOV	AH,0
		XCHG	AH,DS:[SI]
		MOV	SI,CX
		MOV	AL,3
		CALL	FindArcCmd
		POP	SI
		POP	DS
		JNC	F35_12
		PUSH	AX
		PUSH	ES
		POP	DS
		MOV	BX,SI
		MOV	SI,OFFSET TargetFile
		MOV	DI,OFFSET LineBuf1
		CALL	strcpy
		POP	AX
		MOV	DS:[BX],AH
F35_11:		MOV	BP,OFFSET NotImpl_B
		CALL	F35_Table
		CALL	Dialog
		JMP	Func04
F35_12:		PUSH	SI
		PUSH	AX
		OR	AH,AH
		JE	F35_13
		INC	SI
F35_13:		PUSH	DX
		PUSH	SI
		CALL	WinPath
		MOV	DI,OFFSET LineBuf1
		CALL	strcpy
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET TargetFile
		MOV	DI,OFFSET ExecArcProgram
		CALL	strcpy
		POP	DS
		POP	SI
		MOV	DX,103h
		OR	BX,BX
		JE	F35_15
		MOV	BP,SI
		MOV	DI,OFFSET LineBuf2
		PUSH	DI
		PUSH	DS
		PUSH	ES
		POP	DS
		CMP	BYTE PTR DS:[SI],0
		JE	F35_14
		CALL	strcpy
		CALL	AddFile
F35_14:		POP	DS
		MOV	SI,BX
		CALL	LongNameOffset
		CALL	strcpy
		POP	SI
		TEST	DS:[BX].AttrF,10h
		PUSH	ES
		POP	DS
		JE	F35_16
		OR	DH,10h
		JMP	F35_16
F35_15:		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	BP,OFFSET ArcPath
		MOV	DI,BP
		CALL	strcpy
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		XOR	SI,SI
F35_16:		POP	DI
		MOV	AX,DX
		MOV	DX,OFFSET CopyArc_T
		CMP	ES:CopyMove,0
		JE	F35_17
		MOV	DX,OFFSET MoveArc_T
F35_17:		PUSH	BP
		PUSH	DX
		MOV	BP,OFFSET CompressArc_P
		MOV	BX,500h
		CALL	OncZoom
		POP	DX
		POP	BP
		XOR	BX,BX
		CALL	ExecArcCmd
		POP	AX
		POP	SI
		MOV	ES:[SI],AH
		MOV	ES:GroupChg,1
		JMP	FuncExit0
F35_20:		MOV	SI,OFFSET CrnPath	; Copy / Move from tree panel
		MOV	DI,OFFSET LineBuf1
		CALL	LongTreePath
		PUSH	ES
		POP	DS
		MOV	SI,DI
		CALL	CutPath
		MOV	DS:CopyBegName,SI
		MOV	SI,OFFSET CopyTarget
		MOV	AX,DS:[SI]
		CMP	AL,'\'
		JE	F35_21
		CMP	AL,'/'
		JE	F35_21
		CMP	AH,':'
		JE	F35_21
		MOV	DI,OFFSET LineBuf4
		PUSH	DI
		PUSH	SI
		MOV	DS,DS:WCBcrn
		MOV	SI,OFFSET CrnPath
		CALL	LongTreePath
		PUSH	ES
		POP	DS
		MOV	SI,DI
		CALL	CutPath
		MOV	DI,SI
		POP	SI
		CALL	strcpy
		MOV	SI,OFFSET LineBuf3
		MOV	DI,OFFSET TargetFile
		PUSH	DI
		CALL	MvzHist
		POP	DI
		POP	SI
		MOV	CX,LenStrFull+2
		CALL	SaveHistory
F35_21:		CALL	FuncTwoNames
		JC	Func04
		MOV	AL,ES:DescrProcess
		CMP	AL,1
		JNE	F35_22
		MOV	AL,0
F35_22:		MOV	ES:DescrUpdate,AL
		MOV	CL,1
		XCHG	CL,ES:IncDirs
		XOR	BX,BX
		MOV	AL,10h
		CALL	C00_00
		MOV	ES:IncDirs,CL
		CMP	AL,0
		JZ	FuncExit
		CMP	DS:CopyMove,0
		JE	FuncExit
		MOV	SI,OFFSET LineBuf2
		MOV	DI,OFFSET LineBuf4
		CALL	ShortName
		JNC	F35_23
		CMP	AX,7100h
		JNE	F35_24
		CALL	NormPath
		JC	F35_24
F35_23:		MOV	SI,DI
		CALL	CutPath
		MOV	BX,SI
		MOV	CL,0
		XCHG	DS:[BX],CL
		MOV	DS,DS:WCBcrn
		MOV	SI,OFFSET CrnPath
		PUSH	SI
		CALL	CutPath
		MOV	BYTE PTR DS:[SI],0
		POP	SI
		CALL	CompNormLines
		JNE	F35_24
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		XCHG	SI,DI
		MOV	DS:[BX],CL
		CALL	strcpy
		PUSH	DS
		POP	ES
		JMP	FuncExit
F35_24:		MOV	DS,DS:WCBcrn
		MOV	SI,OFFSET CrnPath
		CALL	CutPath
		CMP	SI,OFFSET CrnPath+3
		JBE	F35_25
		DEC	SI
F35_25:		MOV	BYTE PTR DS:[SI],0
		JMP	FuncExit
F35_30:		PUSH	DS			; Copy / Move from archive
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET LineBuf1
		MOV	SI,OFFSET TargetFile
		MOV	DI,OFFSET ExecArcProgram
		PUSH	SI
		CALL	strcpy
		POP	SI
		MOV	DI,DX
		CALL	strcpy
		MOV	SI,DX
		CALL	CutPath
		CMP	BYTE PTR DS:[SI],0
		JNE	F35_31
		MOV	AX,'.'
		STOSW
		DEC	DI
		DEC	DI
F35_31:		CALL	ChgDisk
		POP	DS
		JC	Func04
		MOV	SI,CX
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	AX,4300h
		CALL	Intr21
		POP	DS
		JC	F35_32
		TEST	CL,10h
		JZ	F35_33
		JMP	F35_34
F35_32:		CMP	AX,53h
		JE	Func04
F35_33:		PUSH	ES
		POP	DS
		MOV	BYTE PTR DS:[DI],0
		MOV	BP,OFFSET CopyArcErr_B
		CALL	F35_Table
		CALL	Dialog
		JMP	Func04
F35_34:		MOV	CX,SI
		MOV	DI,DX
		PUSH	DI
		CALL	PutArcPathName
		MOV	DX,102h
		XOR	SI,SI
		MOV	BP,OFFSET ArcPath
		OR	BX,BX
		JE	F35_36
		MOV	BP,OFFSET ExecArcArcPath
		MOV	DI,BP
		MOV	SI,OFFSET ArcPath
		PUSH	SI
		CALL	strcpy
		POP	SI
		MOV	DI,OFFSET LineBuf2
		PUSH	DI
		CMP	BYTE PTR DS:[SI],0
		JE	F35_35
		CALL	strcpy
		CALL	AddFile
F35_35:		MOV	SI,BX
		CALL	LongNameOffset
		CALL	strcpy
		TEST	DS:[BX].AttrF,10h
		PUSH	ES
		POP	DS
		POP	SI
		JZ	F35_36
		OR	DH,10h
F35_36:		POP	DI
		MOV	AX,DX
		MOV	DX,OFFSET CopyArc_T
		CMP	ES:CopyMove,0
		JE	F35_37
		MOV	DX,OFFSET MoveArc_T
F35_37:		PUSH	BP
		PUSH	DX
		MOV	BP,OFFSET ExtractArc_P
		MOV	BX,500h
		CALL	OncZoom
		POP	DX
		POP	BP
		MOV	BL,ES:CopyMove
		PUSH	BX
		PUSH	AX
		MOV	BX,OFFSET TargetFile
		MOV	ES:GroupChg,1
		CALL	ExecArcCmd
		POP	BP
		POP	BX
		JC	FuncExit0
		OR	AX,AX
		JNE	FuncExit0
		OR	BL,BL
		JE	FuncExit0
		MOV	AX,BP
		PUSH	SI
		OR	SI,SI
		JNE	F35_40
		MOV	SI,DS:DiskBuf
F35_38:		CMP	SI,DS:EndBuf
		JAE	F35_40
		TEST	DS:[SI].AttrF,80h
		JZ	F35_39
		OR	DS:[SI].AttrF,40h
F35_39:		CALL	NextFilEnt
		JMP	F35_38
F35_40:		POP	SI
		XOR	BX,BX
		XOR	BP,BP
		MOV	AL,4
		CALL	ExecArcCmd
		JMP	FuncExit0

F46_00:		MOV	AL,1			; Rename / Move...
		JMP	F45_01
F45_00:		MOV	AL,0			; Copy...
F45_01:		MOV	ES:CopyMove,AL
		ADD	AL,Copy_H
		MOV	ES:HlpPage,AL
		XOR	SI,SI
		CALL	F35_Target
		MOV	CX,OFFSET LineBuf1+LenStr
		SUB	CX,DI
		MOV	AL,' '
		REP	STOSB
		MOV	AL,0
		STOSB
		PUSH	ES
		POP	DS
		CALL	F35_Table
		MOV	BP,OFFSET CopyMask_B
		CALL	Dialog
		CMP	AL,0
		JNE	Panels00
		MOV	SI,OFFSET CopyTarget
		MOV	DI,OFFSET TargetFile
		MOV	CX,LenStrFull+2
		CALL	SaveHistory
		CMP	DS:TargetFile,0
		JE	Panels00
		CALL	F35_Init
		MOV	DX,OFFSET CopyMask_T
		CMP	DS:CopyMove,0
		JE	F45_02
		MOV	DX,OFFSET RenMoveMask_T
F45_02:		MOV	BP,OFFSET F45_10
		JMP	FuncMask

F45_10		PROC	NEAR
		MOV	SI,OFFSET LineBuf1
		CALL	CutPath
		MOV	DI,SI
		CMP	AL,0
		MOV	AX,0
		JNE	F45_11
		CMP	AL,DS:[DI]
		JE	F45_11
		MOV	CH,0FFh
		REPNE	SCASB
F45_11:		MOV	DS:CopyBegName,DI
		RET
F45_10		ENDP

F55_00:		MOV	AL,0			; ^F5 - copy file
		JMP	F56_01

F56_00:		MOV	AL,1			; ^F6 - rename or move file
F56_01:		MOV	ES:OneFile,1
		JMP	F35_01

;---------------------------------------------------------------
;	Set table
;	Return:	   DX - address of table
;---------------------------------------------------------------
F35_Table	PROC	NEAR
		MOV	DX,OFFSET Copy_T
		CMP	ES:CopyMove,0
		JE	@@Exit
		MOV	DX,OFFSET RenMove_T
  @@Exit:	RET
F35_Table	ENDP

;---------------------------------------------------------------
;	Set default target path and name
;		   DS - current WCB
;		DS:SI - pointer to filename
;	Return:	ES:DI - end of dialog line
;		   CX - max length of name
;---------------------------------------------------------------
F35_Target	PROC	NEAR
		USES	DS
		MOV	BP,SI
		CALL	InvWCB
		MOV	DI,OFFSET LineBuf4
		PUSH	DI
		CMP	DS:Visible,0
		JZ	@@Default
		CMP	DS:WinTyp,p_Tree
		JA	@@Default		; Info or QuickView panel
		JE	@@Tree
		CALL	WinPath			; File panel
		CMP	DS:ArcPnt,0
		JE	@@Path
		MOV	SI,OFFSET ArcExt	; Archive descriptor
		CALL	strcpy
		MOV	AL,':'
		STOSB
		CALL	PutArcPathName
		CALL	AddFile
		MOV	SI,OFFSET ArcPath
		JMP	@@Move
  @@Tree:	MOV	SI,OFFSET CrnPath
		CALL	LongTreePath
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		JMP	@@AddSlash
  @@Default:	OR	BP,BP			; Default
		JNE	@@FileName
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET TargetFile
		JMP	@@Move
  @@FileName:	MOV	SI,BP			; Add filename
		CALL	InvWCB
		CMP	SI,OFFSET CrnPath
		JNE	@@Move
		PUSH	DI
		MOV	DI,OFFSET LineBuf1
		CALL	LongTreePath
		MOV	SI,DI
		PUSH	ES
		POP	DS
		CALL	CutPath
		POP	DI
  @@Move:	CALL	strcpy
		JMP	@@SaveTarget
  @@Path:	CALL	strcpy			; Add path
  @@AddSlash:	CALL	AddFile
		MOV	BYTE PTR ES:[DI],0
  @@SaveTarget:	PUSH	ES			; Save target
		POP	DS
		MOV	CX,DI
		MOV	SI,OFFSET TargetFile
		MOV	DI,OFFSET CopyTarget
		PUSH	DI
		CALL	MvzHist
		POP	DI
		POP	SI
		CMP	CX,OFFSET LineBuf4+LenStrFull
		JBE	@@MoveTarget
		MOV	BYTE PTR DS:[SI],0
  @@MoveTarget:	MOV	CX,LenStrFull+2
		CALL	SaveHistory

		MOV	SI,OFFSET Copy_S+1	; Dialog line
		CMP	ES:CopyMove,0
		JE	@@DialogLine
		MOV	SI,OFFSET RenMove_S+1
  @@DialogLine:	MOV	DS,CS:SubsSeg
		MOV	DI,OFFSET LineBuf1
		CALL	strcpy
		PUSH	DI
		MOV	SI,OFFSET To2_S
		CALL	strcpy
		MOV	CX,OFFSET LineBuf1+LenStr-2
		SUB	CX,DI
		POP	DI
		RET
F35_Target	ENDP

;---------------------------------------------------------------
;	Tree
;---------------------------------------------------------------
		.CODE	Subs
F35_TreeMask	PROC	NEAR
		ADD	BH,2
		MOV	SI,OFFSET CopyMask_B+SIZE BoxHd+SIZE BoxMn
		JMP	F35_Tree
F35_TreeMask	ENDP

F35_TreeSel	PROC	NEAR
		MOV	SI,OFFSET CopySel_B+SIZE BoxHd
F35_TreeSel	ENDP

F35_Tree	PROC	NEAR
		CMP	AX,4400h
		JE	@@ChooseDir
		CMP	AX,7100h
		JE	@@ChooseDir
		CMP	AH,0FCh
		JAE	@@Exit
		CMP	AL,0Dh
		JE	@@ChkMenu
		CMP	AL,0Ah
		JNE	@@Exit
  @@ChkMenu:	CMP	CL,1
		JNE	@@Exit
  @@ChooseDir:	PUSH	BX			; Get path
		MOV	BP,DI
		MOV	DI,SI
		CALL	DialogBuf
		ADD	SI,6
		PUSH	SI
		PUSH	AX
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		PUSH	DS
		PUSH	SS
		POP	DS
		CALL	strcpy
		POP	DS
		MOV	SI,OFFSET DosShortPath
		MOV	DI,OFFSET LineBuf3
		CALL	strcpy
		MOV	SI,OFFSET DosPath
		MOV	DI,OFFSET LineBuf4
		CALL	strcpy
		POP	SI
		CALL	NormStr
		POP	AX
		MOV	CL,DS:PathEr
		CMP	AX,7100h
		JNE	@@CurPath
		MOV	BYTE PTR DS:[SI],0
		JMP	@@Dialog
  @@CurPath:	MOV	AX,DS:[SI]
		CMP	AH,':'
		JNE	@@Dialog
		CALL	UpCase
		CMP	AL,'Z'
		JA	@@Dialog
		SUB	AL,'A'
		JB	@@Dialog
		MOV	DL,AL
		MOV	AH,0Eh
		INT	21h
		CALL	GetPath
  @@Dialog:	MOV	DL,DS:Columns
		MOV	DH,DS:Lines
		MOV	BL,DL
		MOV	BH,2
		SHR	DL,1
		ADD	DL,10
		SUB	DH,3
		SUB	BL,DL
		SUB	BL,2
		CALL	ChooseDir
		MOV	DS:PathEr,CL
		POP	DI
		POP	BX
		JC	@@Restore
		MOV	SI,OFFSET TmpBuf	; Set path
		PUSH	DI
		PUSH	SS
		POP	ES
		CALL	strcpy
		CALL	AddFile
		MOV	AL,0
		STOSB
		PUSH	DS
		POP	ES
		POP	SI
		DEC	DI
		SUB	DI,SI
		MOV	SS:[SI-5],DI
		SUB	DI,LenStr-1
		JAE	@@Offset
		XOR	DI,DI
  @@Offset:	MOV	SS:[SI-3],DI
		MOV	BYTE PTR SS:[SI-1],1
		PUSH	SS
		POP	DS
		ADD	BX,305h
		MOV	BP,DI
		MOV	CL,LenStr
		MOV	AH,CS:CopySel_P[1]
		CALL	InpStrOut
		MOV	DL,CL
		MOV	DH,1
		CALL	Window
		PUSH	ES
		POP	DS
  @@Restore:	MOV	SI,OFFSET LineBuf3
		MOV	DI,OFFSET DosShortPath
		CALL	strcpy
		MOV	SI,OFFSET LineBuf4
		MOV	DI,OFFSET DosPath
		CALL	strcpy
		MOV	DL,DS:DosShortPath[0]
		SUB	DL,'A'
		MOV	AH,0Eh
		INT	21h
		XOR	AX,AX
  @@Exit:	RET
F35_Tree	ENDP

;---------------------------------------------------------------
;	Init variables
;---------------------------------------------------------------
		.CODE
F35_Init	PROC	NEAR
		MOV	AL,0
		MOV	ES:CopyTargetType,AL
		MOV	ES:CopyTargetDev,AL
		MOV	ES:FncAddr,OFFSET C00_00
		RET
F35_Init	ENDP

;---------------------------------------------------------------
;	Copy/move/rename file or directory
;---------------------------------------------------------------
C00_00		PROC	NEAR
		USES	BX,CX,DX,SI,DI,BP,DS
		PUSH	ES
		POP	DS
		MOV	DS:GroupAttr,AL
		MOV	DS:GroupFileNumb,BX
		MOV	DS:SftErr,2
		MOV	DS:CopySplit,0
		MOV	DS:LineBuf3+514-1,0
C00_01:		MOV	DS:LineBuf4+514-1,0
		MOV	DS:CopyAppend,0
		MOV	DS:CopyMkPath1,0
		MOV	DX,OFFSET LineBuf1	; Get real name
		CALL	ChgDisk
		JC	C00_72
		MOV	SI,DX
		MOV	DI,OFFSET LineBuf3
		CALL	F35_TrueName
		JNC	C00_02
		CMP	AX,53h
		JE	C00_72
		JMP	C00_06
C00_02:		PUSH	DS			; Check recursion
		MOV	DS,DS:GroupFiles
		CALL	ChkInBuffer
		POP	DS
		JC	C00_70
C00_06:		MOV	SI,OFFSET TargetFile	; Set target name
		MOV	DX,OFFSET LineBuf2
		MOV	DI,DX
		CALL	strcpy
		MOV	SI,DX
		CALL	NormStr
		MOV	AL,DS:CopyTargetType	; Target: 0-unknown; 1-mask;
		CMP	AL,0			;         2-dir; 3-file
		JNE	C00_13
		CALL	CutPath			; Target is path
		CMP	BYTE PTR DS:[SI],0
		JE	C00_12
		CALL	ChkUpDir
		JE	C00_12
C00_07:		LODSB				; Check mask
		CMP	AL,'?'
		JE	C00_10
		CMP	AL,'*'
		JE	C00_10
		CMP	AL,0
		JNE	C00_07
		CALL	ChgDisk			; Check device
		JC	C00_72
		MOV	DS:BlocEr,1
		MOV	AL,40h
		CALL	OpenFil
		MOV	DS:BlocEr,0
		JC	C00_09
		PUSH	DX
		MOV	BX,AX
		MOV	AX,4400h
		CALL	Intr21
		MOV	AH,3Eh
		CALL	Intr21
		MOV	AX,DX
		POP	DX
		TEST	AL,80h
		JZ	C00_09
		MOV	DS:CopyTargetDev,1
C00_08:		MOV	AL,3
		JMP	C00_13
C00_09:		MOV	AX,4300h		; Get attributes
		CALL	Intr21
		JNC	C00_11
		CMP	AX,53h
		JE	C00_72
C00_10:		MOV	AL,1
		JMP	C00_13
C00_11:		TEST	CL,10h
		JZ	C00_08
		TEST	DS:GroupAttr,10h
		JZ	C00_12
		MOV	AL,0
		CALL	F35_Compare
		JNC	C00_30
		CMP	AX,53h
		JE	C00_72
C00_12:		MOV	AL,2
C00_13:		MOV	DS:CopyTargetType,AL
		CMP	AL,2
		JA	C00_30
		JNE	C00_14
		CALL	AddFile			; Add path
		MOV	SI,DS:CopyBegName
		JMP	C00_22
C00_14:		MOV	SI,DX			; Mask
		CALL	CutPath
		MOV	DI,OFFSET TmpErr
		PUSH	DI
		PUSH	SI
		CALL	strcpy
		POP	DI
		POP	SI
		MOV	BX,DS:CopyBegName
C00_15:		LODSW
		DEC	SI
		MOV	CX,SI
		CMP	AL,0
		JE	C00_21
		CMP	AL,'.'
		JE	C00_16
		CMP	AL,'?'
		JE	C00_18
		CMP	AL,'*'
		JE	C00_19
		STOSB
		MOV	AL,DS:[BX]
		CMP	AL,0
		JE	C00_15
		CMP	AL,'.'
		JE	C00_15
		CMP	AL,'\'
		JE	C00_15
		CMP	AL,'/'
		JE	C00_15
		INC	BX
		JMP	C00_15
C00_16:		OR	AH,AH
		JE	C00_21
		STOSB
C00_17:		MOV	AL,DS:[BX]
		CMP	AL,0
		JE	C00_15
		CMP	AL,'\'
		JE	C00_15
		CMP	AL,'/'
		JE	C00_15
		INC	BX
		CMP	AL,'.'
		JNE	C00_17
		JMP	C00_15
C00_18:		MOV	AL,DS:[BX]
		CMP	AL,0
		JE	C00_15
		CMP	AL,'.'
		JE	C00_15
		CMP	AL,'\'
		JE	C00_15
		CMP	AL,'/'
		JE	C00_15
		INC	BX
		STOSB
		JMP	C00_15
C00_19:		LODSB
		MOV	AH,AL
		CMP	AL,0
		JE	C00_20
		CMP	AL,'.'
		JNE	C00_19
C00_20:		MOV	SI,CX
		MOV	AL,DS:[BX]
		CMP	AL,0
		JE	C00_15
		CMP	AL,'\'
		JE	C00_15
		CMP	AL,'/'
		JE	C00_15
		CMP	AL,AH
		JE	C00_15
		INC	BX
		STOSB
		JMP	C00_20
C00_21:		MOV	SI,BX
		MOV	AL,DS:[BX]
		CMP	AL,0
		JE	C00_22
		CMP	AL,'\'
		JE	C00_22
		CMP	AL,'/'
		JE	C00_22
		INC	BX
		JMP	C00_21
C00_22:		MOV	CX,OFFSET LineBuf2+512-1 ; Add name
		SUB	CX,DI
		REP	MOVSB
		MOV	AL,0
		STOSB

C00_30:		MOV	DI,OFFSET LineBuf2	; Rename or move
		MOV	CX,512
		MOV	AL,0
		REPNE	SCASB
		CMP	DS:CopySplit,0
		JNZ	C20_00
		CMP	DS:CopyTargetDev,0
		JNZ	C00_31
		CMP	DS:CopyMove,0
		JE	C00_31
		MOV	AX,WORD PTR DS:LineBuf1
		CALL	C00_40
		JE	C00_31
		MOV	BL,AL
		CALL	TstDrv
		JC	C00_31
		CMP	AL,Network
		JNE	C00_32
		MOV	AX,WORD PTR DS:LineBuf2
		CALL	C00_40
		JE	C00_31
		CMP	AL,BL
		JE	C00_32
C00_31:		TEST	DS:GroupAttr,10h
		JZ	C20_00			; Copy/Move file
		MOV	AL,1
		JMP	C30_00			; Copy/Move dir
C00_32:		TEST	DS:GroupAttr,10h
		JZ	C10_00			; Rename file
		MOV	AL,0
		JMP	C30_00			; Rename dir

C00_40		PROC	NEAR
		CMP	AX,'\\'
		JE	C00_42
		CALL	UpCase
		CMP	AH,':'
		JE	C00_41
		MOV	AL,DS:DosPath[0]
C00_41:		OR	AL,AL
C00_42:		RET
C00_40		ENDP

C10_00:		MOV	BX,500h			; Renaming file
		MOV	DX,OFFSET RenMove_T
		MOV	BP,OFFSET CopyMoveFile_P
		CALL	OncZoom
		MOV	AL,0
		OR	CX,CX
		JE	C10_09
		MOV	AL,1
		CALL	F35_Compare
		JNC	C00_71
		OR	AX,AX
		JNE	C10_09
		TEST	DS:GroupAttr,8
		JZ	C10_01
		MOV	SI,OFFSET LineBuf2
		CALL	CutPath
		MOV	DI,OFFSET FileSymb1_S
		CALL	ChkForSymb
		JNC	C10_20
		MOV	BP,OFFSET CopyTargetEr_B
		JMP	C00_76
C10_01:		MOV	DX,OFFSET LineBuf2
		CALL	ChgDisk
		JC	C00_72
		MOV	DI,DX
		MOV	DX,OFFSET LineBuf1
		MOV	AH,56h
		CALL	F35_FuncShort
		JNC	C10_12
		CMP	AX,53h
		JE	C00_72
		CMP	AX,3
		JNE	C10_03
		MOV	DX,OFFSET LineBuf2
		CALL	ChgDisk
		JC	C00_72
		CALL	F35_MkPath
		JC	C10_09
		CALL	F35_UpdPath
		JNC	C10_02
		CMP	AX,53h
		JE	C00_72
C10_02:		MOV	DI,DX
		MOV	DX,OFFSET LineBuf1
		MOV	AH,56h
		CALL	F35_FuncShort
		JNC	C10_12
		JMP	C10_09
C10_03:		CMP	AX,5
		JNE	C20_01
		XCHG	DX,DI
		MOV	AX,4300h
		CALL	Intr21
		JC	C10_09
		MOV	AL,0
		TEST	CL,10h
		JNZ	C10_09
		MOV	AL,DS:GroupAll
		CMP	AL,1
		JE	C10_06
		CMP	AL,4
		JE	C00_70
		MOV	DS:MinMnuDefault,AL
		MOV	BP,CX
		PUSH	DX
		MOV	DX,DI
		MOV	AX,4300h
		CALL	Intr21
		POP	DX
		JNC	C10_04
		CMP	AX,53h
		JE	C00_72
		XOR	CX,CX
C10_04:		MOV	DS:CopyAttrib,CX
		PUSH	DI
		PUSH	DX
		MOV	DX,DI
		CALL	F35_GetAttribs
		MOV	DS:CopySize.Lo,CX
		MOV	DS:CopySize.Hi,BX
		MOV	CX,DI
		POP	DX
		POP	DI
		JNC	C10_05
		CMP	AX,53h
		JE	C00_72
		XOR	SI,SI
		XOR	CX,CX
		OR	DS:CopyAttrib,8000h
C10_05:		MOV	DS:CopyTime,SI
		MOV	DS:CopyDate,CX
		MOV	CX,BP
		PUSH	DX
		MOV	DX,OFFSET RenMove_T
		CALL	F35_Overwrite
		POP	DX
		JC	C00_72
		MOV	DS:GroupAll,AL
		CMP	AL,5
		JAE	C00_72
		CMP	AL,2
		JE	C10_11
		JA	C00_70
C10_06:		MOV	AH,41h
		CALL	Intr21
		JNC	C10_08
		CMP	AX,53h
		JE	C00_72
		TEST	CL,1
		JZ	C10_10
		MOV	AL,DS:GroupAllDstRO
		CMP	AL,1
		JE	C10_07
		CMP	AL,3
		JE	C00_70
		MOV	DS:MinMnuDefault,AL
		PUSH	DX
		MOV	DX,OFFSET RenMove_T
		MOV	BP,OFFSET CopyReadOver_B
		CALL	Dialog
		POP	DX
		MOV	DS:GroupAllDstRO,AL
		CMP	AL,4
		JAE	C00_72
		CMP	AL,2
		JAE	C00_70
C10_07:		AND	CL,0FEh
		MOV	AX,4301h
		CALL	Intr21
		JC	C10_10
		MOV	DS:GroupChg,1
		MOV	AH,41h
		CALL	Intr21
		JNC	C10_08
		CMP	AX,53h
		JE	C00_72
		OR	CL,1
		MOV	AX,4301h
		CALL	Intr21
		JC	C10_10
		MOV	AL,0
		JMP	C10_10
C10_08:		XCHG	DX,DI
		MOV	AH,56h
		CALL	F35_FuncShort
		JNC	C10_12
C10_09:		MOV	BP,OFFSET RenError_B
		JMP	C00_75
C10_10:		MOV	BP,OFFSET CopyDelFileEr_B
		JMP	C00_75
C10_11:		MOV	DS:CopyAppend,1
		MOV	CL,1
		JMP	C20_01
C10_12:		CALL	F35_SaveTarget
		JC	C00_72

C10_20:		PUSH	ES			; Set new name in panel
		POP	DS
		MOV	DI,OFFSET LineBuf1
		MOV	SI,DI
		CALL	CutPath
		CMP	SI,DI
		JNE	C40_00
		MOV	DX,OFFSET LineBuf2
		MOV	SI,DX
		CALL	CutPath
		CMP	SI,DX
		JNE	C40_00
		MOV	AX,DS:GroupFileNumb
		MOV	DS,DS:WCBcrn
		CALL	FindFile
		JC	C40_00
		MOV	BL,DS:[SI].AttrF
		PUSH	DS
		PUSH	SI
		PUSH	DI
		PUSH	ES
		POP	DS
		TEST	BL,10h
		JNZ	C10_21
		TEST	BL,8
		JNZ	C10_22
C10_21:		MOV	AL,DS:DosPath[0]
		CALL	GetFS
		OR	AX,AX
		JNE	C10_23
		XOR	CX,CX
		MOV	SI,DX
		MOV	DI,OFFSET HelpBuf
		MOV	DX,DI
		CALL	NormName
		TEST	BL,10h
		JNE	C10_23
C10_22:		CMP	DS:ConvCase,0
		JE	C10_23
		MOV	SI,DX
		CALL	LetLin
C10_23:		POP	DI
		POP	SI
		POP	DS
		XCHG	DX,DI
		CALL	StrLen
		MOV	AX,CX
		PUSH	CX
		MOV	CX,DS:[SI].LenShortNamF
		ADD	CX,DS:[SI].LenLongNamF
		SUB	AX,CX
		POP	CX
		JE	C10_28
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	CX
		JA	C10_24
		PUSH	DS
		POP	ES
		CALL	NextFilEnt
		MOV	CX,DS:EndBuf
		ADD	DS:EndBuf,AX
		SUB	CX,SI
		JBE	C10_27
		MOV	DI,SI
		ADD	DI,AX
		REP	MOVSB
		JMP	C10_27
C10_24:		PUSH	SI
		MOV	BX,DS
		INC	BX
		MOV	SI,OFFSET AllSegs_L
		CALL	FindNextSeg
		DEC	BX
		SUB	DX,BX
		MOV	CL,4
		SHL	DX,CL
		POP	BX
		MOV	SI,DS:EndBuf
		MOV	DI,SI
		ADD	DI,AX
		JC	C10_25
		CMP	DI,0FFF0h
		JA	C10_25
		CMP	DI,DX
		JBE	C10_26
		PUSH	SI
		MOV	SI,OFFSET AllSegs_L
		MOV	AX,ExpandSize
		CALL	ExpandBlock
		POP	SI
		CMP	DI,AX
		JBE	C10_26
C10_25:		POP	CX
		POP	SI
		POP	DI
		POP	ES
		JMP	C40_00
C10_26:		MOV	DS:EndBuf,DI
		MOV	CX,DS:[BX].LenShortNamF
		ADD	BX,DS:[BX].LenLongNamF
		ADD	BX,SIZE FilEnt
		ADD	BX,CX
		MOV	CX,SI
		SUB	CX,BX
		JBE	C10_27
		PUSH	DS
		POP	ES
		DEC	SI
		DEC	DI
		STD
		REP	MOVSB
		CLD
C10_27:		POP	CX
		POP	SI
		POP	DI
		POP	ES
C10_28:		MOV	DS:[SI].LenShortNamF,CX
		MOV	DS:[SI].LenLongNamF,0
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		XCHG	DI,SI
		ADD	DI,OFFSET NameF
		REP	MOVSB
		PUSH	DS
		POP	ES
		JMP	C40_00

C20_00:		MOV	BX,500h			; Copy / Move file
		MOV	BP,OFFSET CopyMoveFile_P
		CALL	F35_Table
		CALL	OncZoom
C20_01:		MOV	BL,DS:Columns
		SHR	BL,1
		SUB	BL,16
		MOV	BH,11
		CALL	SftEGA
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		MOV	AX,256*(80h+32)+0FFh
		STOSW
		MOV	AL,DS:ProgressBars[0]
		MOV	AH,0
		STOSW
		POP	SI
		MOV	BP,OFFSET CopyMoveFile_P
		MOV	AL,80h
		CALL	NgLine
		CMP	DS:CopySplit,0
		JZ	C20_02
		CALL	F35_PerCent
C20_02:		MOV	DX,120h
		CALL	Window
		MOV	BP,OFFSET CopySourceEr_B
		OR	CX,CX
		JE	C00_76
		MOV	AL,0
		CALL	F35_Compare
		MOV	BP,OFFSET CopyItself_B
		JNC	C00_76
		MOV	BP,OFFSET CopySourceEr_B
		OR	AX,AX
		JNE	C00_75
		TEST	DS:GroupAttr,8
		JZ	C20_03
		MOV	SI,OFFSET LineBuf2
		CALL	CutPath
		MOV	DI,OFFSET FileSymb1_S
		CALL	ChkForSymb
		JNC	C40_00
		MOV	BP,OFFSET CopyTargetEr_B
		JMP	C00_76
C20_03:		CMP	DS:CopySplit,0
		JNZ	C20_15
		MOV	DX,OFFSET LineBuf1
		CALL	ChgDisk
		JC	C00_72
		MOV	DS:BlocEr,2
		MOV	AL,40h
		CALL	OpenFil
		MOV	DS:BlocEr,0
		JNC	C20_04
		CMP	DS:ErrDat,2
		JE	C00_70
		JMP	C00_75
C20_04:		MOV	DS:CopyHandle1,AX
		MOV	BX,AX
		PUSH	DX
		MOV	AX,4400h
		CALL	Intr21
		MOV	AX,DX
		POP	DX
		AND	AL,81h
		CMP	AL,81h
		JE	C20_80
		AND	AL,80h
		MOV	DS:CopySourceDev,AL
		MOV	AX,0FFFFh
		MOV	DS:CopySize.Lo,AX
		MOV	DS:CopySize.Hi,AX
		MOV	DS:CopyAttrib,20h
		JNZ	C20_05
		CALL	F35_GetAttr
		JC	C20_80
		AND	CX,27h
		MOV	DS:CopyAttrib,CX
		XOR	CX,CX
		XOR	DX,DX
		MOV	AX,4202h
		CALL	Intr21
		MOV	DS:CopySize.Lo,AX
		MOV	DS:CopySize.Hi,DX
		XOR	CX,CX
		XOR	DX,DX
		MOV	AX,4200h
		CALL	Intr21
C20_05:		MOV	AX,5700h
		CALL	Intr21
		MOV	DS:CopyDate,DX
		MOV	DS:CopyTime,CX
		XOR	AX,AX
		MOV	DS:CopyRead.Lo,AX
		MOV	DS:CopyRead.Hi,AX
		MOV	DS:CopyWrite.Lo,AX
		MOV	DS:CopyWrite.Hi,AX
		MOV	DS:CopyHandle2,AX
		MOV	DS:CopySplitSeg,AX
		MOV	DS:CopyFlagEAs,AL
		MOV	DS,DS:WorkBuffer
		CMP	ES:CopySourceDev,0
		JNE	C20_08
		CMP	ES:OS2box,0
		JE	C20_08
		CMP	ES:CopyEAs,0
		JE	C20_08
		MOV	DX,ES
		ADD	DX,ES:SizeDataSeg
		MOV	AX,DS
		SUB	DX,AX
		MOV	CX,0FFFFh
		CMP	DX,1000h
		JAE	C20_06
		MOV	CL,4
		SHL	DX,CL
		MOV	CX,DX
C20_06:		MOV	DS:[0].Lo,CX
		MOV	DS:[0].Hi,0
		MOV	DI,OFFSET CopyBufEAs
		MOV	ES:[DI+0].Offs,0
		MOV	ES:[DI+0].Segm,0
		MOV	ES:[DI+4].Offs,0
		MOV	ES:[DI+4].Segm,AX
		MOV	DX,4
		MOV	CX,12
		MOV	AX,5702h
		CALL	Intr21
		JC	C20_07
		MOV	AX,DS:[0].Lo
		CMP	AX,4
		JBE	C20_08
		ADD	ES:CopySize.Lo,AX
		ADC	ES:CopySize.Hi,0
		MOV	ES:CopyRead.Lo,AX
		DEC	AX
		MOV	CL,4
		SHR	AX,CL
		INC	AX
		MOV	CX,DS
		ADD	AX,CX
		MOV	DS,AX
		MOV	ES:CopySplitSeg,AX
		MOV	ES:CopyFlagEAs,1
		CALL	F35_PerCent
		JMP	C20_08
C20_07:		CMP	AX,53h
		JE	C20_82
C20_08:		CMP	ES:ChkTarget,0
		JE	C20_10
		CMP	ES:CopyTargetDev,0
		JE	C20_20

C20_10:		MOV	AX,ES			; Reading
		ADD	AX,ES:SizeDataSeg
		MOV	SI,DS
		SUB	AX,SI
		AND	AL,0E0h
		MOV	SI,AX
		MOV	AL,AH
		MOV	AH,0
		MOV	CL,4
		SHL	SI,CL
		SHR	AX,CL
		MOV	DI,AX
		MOV	BX,ES:CopyHandle1
		MOV	DX,OFFSET LineBuf1
		CALL	ChgDisk
		JC	C20_82
C20_11:		CALL	TstBrk
		JC	C20_82
		XOR	DX,DX
		MOV	CX,8000h
		OR	DI,DI
		JNE	C20_12
		CMP	CX,SI
		JBE	C20_12
		MOV	CX,SI
C20_12:		XOR	AX,AX
		JCXZ	C20_13
		MOV	AH,3Fh
		CALL	Intr21
		MOV	BP,OFFSET CopyReadEr_B
		JC	C20_80
C20_13:		ADD	ES:CopyRead.Lo,AX
		ADC	ES:CopyRead.Hi,0
		CALL	F35_PerCent
		OR	AX,AX
		JE	C20_14
		ADD	DX,AX
		SUB	CX,AX
		JNE	C20_12
		MOV	DX,DS
		ADD	DX,800h
		MOV	DS,DX
		SUB	SI,AX
		SBB	DI,0
		MOV	AX,SI
		OR	AX,DI
		JNE	C20_11
		JMP	C20_20
C20_14:		MOV	AH,3Eh
		CALL	Intr21
		MOV	ES:CopyHandle1,0
		CMP	ES:CopySourceDev,0
		JZ	C20_20
		MOV	AX,ES:CopyRead.Lo
		MOV	BX,ES:CopyRead.Hi
		MOV	ES:CopySize.Lo,AX
		MOV	ES:CopySize.Hi,BX
		OR	AX,BX
		JE	C20_82
C20_15:		CALL	F35_PerCent

C20_20:		PUSH	ES			; Open target file
		POP	DS
		MOV	DX,OFFSET LineBuf2
		CALL	ChgDisk
		JC	C20_82
		CMP	DS:CopyHandle2,0
		JNE	C20_40
		MOV	DS:CopyPathFlag,0
		CMP	DS:CopyTargetDev,0
		JZ	C20_21
		MOV	AL,11h
		CALL	OpenFil
		JC	C20_29
		MOV	BX,AX
		JMP	C20_28
C20_21:		MOV	AX,4300h
		CALL	Intr21
		JNC	C20_22
		MOV	CX,20h
		CMP	AX,3
		JA	C20_29
		PUSH	AX
		MOV	AL,0
		CALL	F35_ChkTarget
		POP	BX
		CMP	AL,3
		JAE	C20_82
		CMP	AL,1
		JA	C20_81
		JE	C20_30
		CMP	BX,3
		JNE	C20_25
		CALL	F35_MkPath
		JC	C20_29
		MOV	DS:CopyPathFlag,1
		JMP	C20_25
C20_22:		TEST	CL,10h
		JNZ	C20_29
		CMP	DS:CopyAppend,0
		JNZ	C20_31
		MOV	AL,DS:GroupAll
		CMP	AL,1
		JE	C20_23
		CMP	AL,4
		JE	C20_81
		MOV	DS:MinMnuDefault,AL
		PUSH	DX
		CALL	F35_Table
		CALL	F35_Overwrite
		POP	DX
		JC	C20_82
		MOV	DS:GroupAll,AL
		CMP	AL,5
		JAE	C20_82
		CMP	AL,2
		JE	C20_31
		JA	C20_81
C20_23:		MOV	AL,1
		CALL	F35_ChkTarget
		CMP	AL,3
		JAE	C20_82
		CMP	AL,1
		JA	C20_81
		JE	C20_30
		TEST	CL,1
		JZ	C20_25
		MOV	AL,DS:GroupAllDstRO
		CMP	AL,1
		JE	C20_24
		CMP	AL,3
		JE	C20_81
		MOV	DS:MinMnuDefault,AL
		PUSH	DX
		CALL	F35_Table
		MOV	BP,OFFSET CopyReadOver_B
		CALL	Dialog
		POP	DX
		MOV	DS:GroupAllDstRO,AL
		CMP	AL,4
		JAE	C20_82
		CMP	AL,2
		JAE	C20_81
C20_24:		AND	CX,26h
		MOV	AX,4301h
		CALL	Intr21
		JC	C20_29
		MOV	DS:GroupChg,1
C20_25:		MOV	DI,DX
		MOV	DS:BlocEr,2
		MOV	AH,3Ch
		CALL	F35_FuncShort
		MOV	DS:BlocEr,0
		JNC	C20_26
		CMP	DS:ErrDat,2
		JE	C20_81
		JMP	C20_29
C20_26:		MOV	BX,AX
		MOV	AL,0
		XCHG	AL,DS:CopyFlagEAs
		CMP	AL,0
		JE	C20_28
		MOV	DI,OFFSET CopyBufEAs
		MOV	DX,2
		MOV	CX,12
		MOV	AX,5703h
		CALL	Intr21
		JC	C20_27
		MOV	DS,DS:WorkBuffer
		MOV	AX,DS:[0].Lo
		PUSH	ES
		POP	DS
		MOV	DS:CopyWrite.Lo,AX
		CALL	F35_PerCent
		JMP	C20_28
C20_27:		CMP	AX,53h
		JE	C20_82
C20_28:		MOV	DS:CopyHandle2,BX
		MOV	DS:GroupChg,1
		MOV	AX,4400h
		CALL	Intr21
		JC	C20_29
		TEST	DL,80h
		JZ	C20_40
		MOV	DH,0
		OR	DL,20h
		MOV	AX,4401h
		CALL	Intr21
		JC	C20_29
		TEST	DL,2
		JZ	C20_40
		MOV	AL,0
C20_29:		MOV	BP,OFFSET CopyTargetEr_B
		JMP	C20_80
C20_30:		MOV	CL,0
		MOV	BX,DS:CopyHandle1
		OR	BX,BX
		JE	C20_59
		MOV	AH,3Eh
		CALL	Intr21
		JMP	C20_59
C20_31:		MOV	AL,0
		CALL	F35_ChkTarget
		CMP	AL,3
		JAE	C20_82
		CMP	AL,1
		JA	C20_81
		JE	C20_30
		MOV	DS:CopyAppend,1
		AND	CX,27h
		MOV	DS:CopyAttrib,CX
		TEST	CL,1
		JZ	C20_33
		MOV	AL,DS:GroupAllDstRO
		CMP	AL,1
		JE	C20_32
		CMP	AL,3
		JE	C20_81
		MOV	DS:MinMnuDefault,AL
		PUSH	DX
		CALL	F35_Table
		MOV	BP,OFFSET CopyReadApp_B
		CALL	Dialog
		POP	DX
		MOV	DS:GroupAllDstRO,AL
		CMP	AL,4
		JAE	C20_82
		CMP	AL,2
		JAE	C20_81
C20_32:		AND	CL,0FEh
		MOV	AX,4301h
		CALL	Intr21
		JC	C20_29
		MOV	DS:GroupChg,1
C20_33:		MOV	DS:BlocEr,2
		MOV	AL,12h
		CALL	OpenFil
		MOV	DS:BlocEr,0
		JNC	C20_34
		CMP	DS:ErrDat,2
		JNE	C20_29
		JMP	C20_81
C20_34:		MOV	DS:GroupChg,1
		MOV	DS:CopyHandle2,AX
		MOV	BX,AX
		MOV	AX,5700h
		CALL	Intr21
		MOV	DS:CopyDate,DX
		MOV	DS:CopyTime,CX
		XOR	CX,CX
		XOR	DX,DX
		MOV	AX,4202h
		CALL	Intr21
		MOV	DS:CopyAppOffset.Lo,AX
		MOV	DS:CopyAppOffset.Hi,DX
		MOV	SI,AX
		MOV	DI,DX
		OR	AX,DX
		JE	C20_40
		SUB	SI,1
		SBB	DI,0
		MOV	DX,SI
		MOV	CX,DI
		MOV	AX,4200h
		CALL	Intr21
		MOV	DX,OFFSET TmpBuf
		MOV	CX,1
		MOV	AH,3Fh
		CALL	Intr21
		JC	C20_29
		CMP	DS:TmpBuf[0],1Ah
		JNE	C20_40
		MOV	AL,DS:CopyDelEofMenu
		MOV	DS:MinMnuDefault,AL
		MOV	BP,OFFSET CopyDelEof_B
		CALL	F35_Table
		CALL	Dialog
		MOV	DS:CopyDelEofMenu,AL
		JNZ	C20_40
		MOV	DS:CopyAppOffset.Lo,SI
		MOV	DS:CopyAppOffset.Hi,DI
		MOV	DX,SI
		MOV	CX,DI
		MOV	AX,4200h
		CALL	Intr21

C20_40:		MOV	SI,DS:CopyRead.Lo		; Writing
		MOV	DI,DS:CopyRead.Hi
		SUB	SI,DS:CopyWrite.Lo
		SBB	DI,DS:CopyWrite.Hi
		MOV	BX,DS:CopyHandle2
		XOR	AX,AX
		XCHG	AX,DS:CopySplitSeg
		MOV	DS,DS:WorkBuffer
		OR	AX,AX
		JE	C20_41
		MOV	DS,AX
C20_41:		CALL	TstBrk
		JC	C20_82
		MOV	CX,8000h
		OR	DI,DI
		JNE	C20_42
		CMP	CX,SI
		JBE	C20_42
		MOV	CX,SI
C20_42:		XOR	AX,AX
		JCXZ	C20_43
		XOR	DX,DX
		MOV	AH,40h
		CALL	Intr21
		JC	C20_44
C20_43:		ADD	ES:CopyWrite.Lo,AX
		ADC	ES:CopyWrite.Hi,0
		CALL	F35_PerCent
		CMP	AX,CX
		JE	C20_45
		CMP	ES:CopyTargetDev,0
		JE	C20_50
C20_44:		MOV	BP,OFFSET CopyWriteEr_B
		JMP	C20_80
C20_45:		SUB	SI,AX
		SBB	DI,0
		MOV	CL,4
		SHR	AX,CL
		MOV	DX,DS
		ADD	DX,AX
		MOV	DS,DX
		MOV	AX,SI
		OR	AX,DI
		JNE	C20_41
		CMP	ES:CopyHandle1,0
		JE	C20_70
		MOV	DS,ES:WorkBuffer
		JMP	C20_10

C20_50:		MOV	AX,WORD PTR ES:LineBuf2	; Not enough room
		CMP	AH,':'
		JE	C20_51
		MOV	AL,ES:DosPath[0]
C20_51:		CALL	UpCase
		MOV	DI,OFFSET TmpBuf
		CALL	GetDiskInfo
		JNC	C20_52
		CMP	AX,53h
		JNE	C20_53
		JMP	C20_82
C20_52:		MOV	AX,ES:[DI+0].Lo
		MUL	ES:[DI+8].Lo
		OR	DX,ES:[DI+0].Hi
		OR	DX,ES:[DI+8].Hi
		JNE	C20_53
		CMP	AX,CX
		JAE	C20_53
		MOV	CX,AX
		MOV	AH,40h
		CALL	Intr21
		JC	C20_44
		ADD	ES:CopyWrite.Lo,AX
		ADC	ES:CopyWrite.Hi,0
C20_53:		MOV	SI,DS
		PUSH	ES
		POP	DS
		CALL	F35_PerCent
		MOV	CL,4
		SHR	AX,CL
		ADD	AX,SI
		MOV	DS:CopySplitSeg,AX
		XOR	BX,BX
		XCHG	BX,DS:CopyHandle2
		CALL	F35_SetDate
		XOR	CX,CX
		XOR	DX,DX
		MOV	AX,4202h
		CALL	Intr21
		MOV	SI,AX
		OR	SI,DX
		MOV	AH,3Eh
		CALL	Intr21
		JC	C20_44
		MOV	DX,OFFSET LineBuf2
		CALL	F35_SetName
		JNC	C20_54
		CMP	AX,53h
		JE	C20_82
C20_54:		MOV	CX,DS:CopyAttrib
		OR	CL,20h
		MOV	AX,4301h
		CMP	DS:CopyAppend,0
		JNZ	C20_55
		OR	SI,SI
		JNE	C20_55
		MOV	AH,41h
C20_55:		CALL	Intr21
		JNC	C20_56
		CMP	AX,53h
		JE	C20_82
C20_56:		MOV	AL,DS:CopySplitMenu
		MOV	DS:MinMnuDefault,AL
		MOV	BP,OFFSET CopySplit_B
		CALL	F35_Table
		CALL	Dialog
		MOV	DS:CopySplitMenu,AL
		MOV	CL,AL
		CMP	AL,1
		JNE	C20_57
		MOV	AX,DS:CopyWrite.Lo
		OR	AX,DS:CopyWrite.Hi
		JNE	C20_59
		MOV	CL,2
		JMP	C20_59
C20_57:		PUSH	CX
		MOV	DX,OFFSET LineBuf2
		CALL	F35_DelCut
		POP	CX
		JNC	C20_58
		CMP	AX,53h
		JE	C20_82
C20_58:		CMP	CL,2
		JA	C20_82
		JE	C20_81
C20_59:		MOV	DS:CopySplit,CL
		PUSH	DS
		MOV	SI,OFFSET CopyContinue_S
		CMP	DS:CopyMove,0
		JZ	C20_60
		MOV	SI,OFFSET MoveContinue_S
C20_60:		MOV	DI,OFFSET LineBuf2
		MOV	DS,CS:SubsSeg
		CALL	strcpy
		MOV	CX,OFFSET LineBuf2+LenStr
		SUB	CX,DI
		MOV	AL,' '
		REP	STOSB
		MOV	AL,0
		STOSB
		POP	DS
		CALL	ResWin
		CALL	Window
		MOV	DS:ZoomOne,0
		MOV	BP,OFFSET CopyContinue_B
		CALL	F35_Table
		CALL	Dialog
		JNZ	C20_82
		CALL	F35_Init
		JMP	C00_01

C20_70:		PUSH	ES			; Close target file
		POP	DS
		CALL	F35_SetDate
		MOV	AH,3Eh
		CALL	Intr21
		JC	C20_44
		CALL	F35_SaveTarget
		JC	C00_72
		MOV	DX,OFFSET LineBuf2
		CMP	DS:CopyPathFlag,0
		JE	C20_71
		CALL	F35_UpdPath
		JNC	C20_71
		CMP	AX,53h
		JE	C00_72
C20_71:		CMP	DS:CopyTargetDev,0
		JNE	C20_73
		CALL	F35_SetName
		JNC	C20_72
		CMP	AX,53h
		JE	C00_72
C20_72:		MOV	CX,DS:CopyAttrib
		OR	CL,20h
		MOV	AX,4301h
		CALL	Intr21
		JNC	C20_73
		CMP	AX,53h
		JE	C00_72
C20_73:		CMP	DS:CopyMove,0
		JE	C40_00
		MOV	DX,OFFSET LineBuf1
		CALL	ChgDisk
		JC	C00_72
		MOV	AH,41h
		CALL	Intr21
		JNC	C40_00
		CMP	AX,53h
		JE	C00_72
		MOV	AX,4300h
		CALL	Intr21
		JC	C20_75
		TEST	CL,1
		JZ	C20_75
		MOV	AL,DS:GroupAllSrcRO
		CMP	AL,1
		JE	C20_74
		CMP	AL,3
		JE	C40_00
		MOV	DS:MinMnuDefault,AL
		PUSH	DX
		MOV	BP,OFFSET CopySourceRO_B
		CALL	F35_Table
		CALL	Dialog
		POP	DX
		MOV	DS:GroupAllSrcRO,AL
		CMP	AL,4
		JAE	C00_72
		CMP	AL,2
		JAE	C40_00
C20_74:		AND	CX,36h
		MOV	AX,4301h
		CALL	Intr21
		JC	C20_75
		MOV	AH,41h
		CALL	Intr21
		JNC	C40_00
		CMP	AX,53h
		JE	C00_72
		OR	CL,1
		MOV	AX,4301h
		CALL	Intr21
		JC	C20_75
		MOV	AL,0
C20_75:		MOV	BP,OFFSET DelFileErr_B
		JMP	C00_75

C20_80:		CMP	AX,53h			; Error
		JE	C20_82
		CALL	F35_Table
		CALL	Dialog
		JNZ	C20_82
C20_81:		XOR	AX,AX
		JMP	C20_83
C20_82:		MOV	AX,100h			; Terminate
C20_83:		PUSH	AX
		PUSH	ES
		POP	DS
		MOV	DS:BlocEr,1
		MOV	BX,DS:CopyHandle2
		OR	BX,BX
		JE	C20_84
		MOV	DX,OFFSET LineBuf2
		CALL	ChgDisk
		JC	C20_84
		MOV	AH,3Eh
		CALL	Intr21
		CALL	F35_DelCut
C20_84:		MOV	BX,DS:CopyHandle1
		OR	BX,BX
		JE	C20_85
		MOV	AH,3Eh
		CALL	Intr21
C20_85:		MOV	DS:BlocEr,0
		POP	AX
		JMP	C00_81

C30_00:		CMP	DS:CopyTargetDev,0	; Copy or move directory
		JNZ	C30_41
		CMP	DS:GroupMask,0
		JNZ	C30_41
		MOV	BP,OFFSET CopyMoveDir_P
		MOV	BX,500h
		CALL	F35_Table
		CALL	OncZoom
		MOV	BL,AL
		MOV	BP,OFFSET CopyMkDirErr_B
		OR	CX,CX
		JZ	C00_76
		CMP	DS:CopyMove,0
		JE	C30_20
		OR	BL,BL
		JNE	C30_20
		MOV	AL,1			; Rename
		CALL	F35_Compare
		JNC	C00_71
		OR	AX,AX
		JNE	C00_75
		MOV	SI,OFFSET LineBuf1
		MOV	DI,OFFSET LineBuf3
		CALL	F35_ShortPath
		MOV	BP,OFFSET MoveDelDirErr_B
		JC	C00_76
		MOV	DX,OFFSET LineBuf2
		CALL	ChgDisk
		JC	C00_72
		PUSH	SI
		MOV	SI,OFFSET DosShortPath
		CALL	StrLen
		DEC	CX
		REPE	CMPSB
		LODSB
		POP	SI
		MOV	CL,0
		JNE	C30_01
		INC	CX
		CMP	AL,0
		JE	C30_01
		CMP	AL,'\'
		JE	C30_01
		DEC	CX
C30_01:		MOV	DI,DX
		MOV	DX,SI
		CALL	F35_MoveDir
		JNC	C30_02
		CMP	AX,53h
		JE	C00_72
		CMP	AX,3
		JNE	C30_20
		XCHG	DX,DI
		CALL	ChgDisk
		JC	C00_72
		CALL	F35_MkPath
		JC	C00_75
		XCHG	DX,DI
		CALL	F35_MoveDir
		JC	C30_20
C30_02:		MOV	DS:GroupChg,1
		OR	CL,CL
		JE	C30_07
		MOV	SI,DI
		MOV	DI,OFFSET LineBuf4
		CALL	F35_ShortPath
		JNC	C30_03
		CMP	AX,53h
		JE	C00_72
		MOV	BYTE PTR DS:[DI],0
C30_03:		MOV	SI,DI
		CALL	CutPath
		XOR	AX,AX
		XCHG	AL,DS:[SI]
		PUSH	SI
		MOV	BX,OFFSET LineBuf3
		MOV	SI,BX
		CALL	CutPath
		XCHG	SI,BX
		XCHG	AH,DS:[BX]
		MOV	CX,BX
		SUB	CX,SI
		PUSH	DI
		PUSH	SI
		PUSH	CX
		INC	CX
		REPE	CMPSB
		POP	CX
		POP	SI
		POP	DI
		MOV	DS:[BX],AH
		POP	BX
		MOV	DS:[BX],AL
		MOV	DX,OFFSET DosShortPath
		JNE	C30_04
		PUSH	DI
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		XCHG	SI,DI
		CALL	StrLen
		DEC	CX
		MOV	DI,SI
		MOV	SI,DX
		PUSH	SI
		ADD	SI,CX
		CALL	strcpy
		POP	DI
		POP	SI
		CALL	strcpy
		JMP	C30_06
C30_04:		CMP	CX,3
		JBE	C30_05
		DEC	CX
C30_05:		MOV	DI,DX
		REP	MOVSB
		MOV	AL,0
		STOSB
C30_06:		MOV	AH,3Bh
		CALL	Intr21
		JNC	C30_07
		CMP	AX,53h
		JE	C00_72
C30_07:		CALL	F35_SaveTarget
		JC	C00_72
		MOV	BX,DS:GroupTree2	; Update TREEINFO
		CMP	BX,1
		JE	C30_17
		MOV	DX,OFFSET LineBuf3
		MOV	DS,BX
		OR	BX,BX
		JNZ	C30_10
		MOV	AX,ExpandSize
		CALL	GetTreeInfo
		MOV	ES:GroupTree2,1
		JC	C30_16
		MOV	ES:GroupTree2,DS
		CALL	AllocateBlock
C30_10:		MOV	SI,DX
		MOV	DI,OFFSET HelpBuf
		PUSH	DI
		PUSH	DS
		PUSH	ES
		POP	DS
		CALL	strcpy
		POP	DS
		MOV	DX,OFFSET LineBuf2
		CALL	F35_AddTree
		POP	DX
		JC	C30_16
		CALL	FindTreeEntry
		JC	C30_17
		MOV	DI,SI
		MOV	SI,DS:TreeLen
		CALL	TreeNumb2Adr1
		XCHG	SI,DI
		PUSH	SI
		MOV	AL,DS:[SI].DirLevel
C30_11:		CALL	NextDirEnt
		CMP	SI,DI
		JAE	C30_12
		CMP	AL,DS:[SI].DirLevel
		JB	C30_11
C30_12:		MOV	AX,SI
		POP	SI
		PUSH	SI
		CALL	NextDirEnt
		XOR	BX,BX
		MOV	DX,SI
		SUB	AX,SI
		POP	SI
		JBE	C30_14
		MOV	DI,ES
		ADD	DI,ES:SizeDataSeg
		SUB	DI,ES:WorkBuffer
		CMP	DI,1000h
		JAE	C30_13
		MOV	CL,4
		SHL	DI,CL
		CMP	AX,DI
		JA	C30_14
C30_13:		PUSH	ES
		PUSH	SI
		MOV	CX,AX
		MOV	SI,DX
		MOV	ES,ES:WorkBuffer
		XOR	DI,DI
		REP	MOVSB
		MOV	BX,AX
		POP	SI
		POP	ES
C30_14:		MOV	ES:GroupTreeChg2,1
		CALL	DelTreeEntry
		OR	BX,BX
		JE	C30_17
		MOV	DX,OFFSET LineBuf4
		CALL	FindTreeEntry
		JC	C30_17
		MOV	DL,DS:[SI].DirLevel
		CALL	NextDirEnt
		MOV	AX,SI
		MOV	SI,DS:TreeLen
		CALL	TreeNumb2Adr1
		MOV	CX,SI
		SUB	CX,AX
		DEC	SI
		MOV	DI,SI
		ADD	DI,BX
		PUSH	ES
		PUSH	ES:WorkBuffer
		PUSH	DS
		POP	ES
		STD
		REP	MOVSB
		CLD
		POP	DS
		MOV	DI,AX
		MOV	CX,BX
		XOR	SI,SI
		REP	MOVSB
		PUSH	ES
		POP	DS
		POP	ES
		MOV	SI,AX
		INC	DX
		SUB	DL,DS:[SI].DirLevel
C30_15:		CMP	SI,DI
		JAE	C30_17
		ADD	DS:[SI].DirLevel,DL
		INC	DS:TreeLen
		CALL	NextDirEnt
		JMP	C30_15
C30_16:		CMP	AX,53h
		JE	C00_72
C30_17:		JMP	C10_20

C30_20:		MOV	AL,0			; Make target directory
 		CALL	F35_Compare
		JNC	C00_71
		OR	AX,AX
		JNE	C00_75
		MOV	DX,OFFSET LineBuf1
		CALL	ChgDisk
		JC	C00_72
		CALL	F35_GetAttr
		JC	C00_75
		MOV	DS:CopyAttrib,CX
		CALL	GetFileTime
		JNC	C30_21
		CMP	AX,53h
		JE	C00_72
		MOV	CX,0FFFFh
		MOV	DI,CX
C30_21:		MOV	DS:CopyDate,DI
		MOV	DS:CopyTime,CX
		CMP	DS:OS2box,0
		JE	C30_23
		CMP	DS:CopyEAs,0
		JE	C30_23
		MOV	BX,DS
		ADD	BX,DS:SizeDataSeg
		MOV	AX,DS:WorkBuffer
		SUB	BX,AX
		MOV	DS,AX
		MOV	SI,0FFFFh
		CMP	BX,1000h
		JAE	C30_22
		MOV	SI,BX
		MOV	CL,4
		SHL	SI,CL
C30_22:		MOV	DS:[0].Lo,SI
		MOV	DS:[0].Hi,0
		PUSH	ES
		POP	DS
		MOV	DI,OFFSET CopyBufEAs
		MOV	DS:[DI+0].Offs,0
		MOV	DS:[DI+0].Segm,0
		MOV	DS:[DI+4].Offs,0
		MOV	DS:[DI+4].Segm,AX
		MOV	SI,DX
		MOV	DX,4
		MOV	CX,12
		MOV	BX,0FFFFh
		MOV	AX,5702h
		CALL	Intr21
		JNC	C30_24
C30_23:		MOV	DS,DS:WorkBuffer
		MOV	DS:[0].Lo,4
		PUSH	ES
		POP	DS
C30_24:		MOV	DX,OFFSET LineBuf2
		CALL	ChgDisk
		JC	C00_72
		MOV	AH,39h
		CALL	Intr21
		JNC	C30_26
		CMP	AX,53h
		JE	C00_72
		CMP	AX,3
		JE	C30_25
		MOV	AX,4300h
		CALL	Intr21
		JC	C00_75
		TEST	CL,10h
		JZ	C00_76
		CALL	F35_SaveTarget
		JC	C00_72
		JMP	C30_40
C30_25:		CALL	F35_MkPath
		JC	C00_75
		MOV	AH,39h
		CALL	Intr21
		JC	C00_75
C30_26:		MOV	DS:GroupChg,1
		MOV	AX,DS:CopyDate
		MOV	CX,DS:CopyTime
		MOV	DI,AX
		AND	AX,CX
		INC	AX
		JZ	C30_27
		CALL	SetFileTime
		JNC	C30_27
		CMP	AX,53h
		JE	C00_72
C30_27:		MOV	CX,DS:CopyAttrib
		AND	CX,27h
		MOV	AX,4301h
		CALL	Intr21
		JNC	C30_28
		CMP	AX,53h
		JE	C00_72
C30_28:		MOV	DS,DS:WorkBuffer
		CMP	DS:[0].Lo,4
		PUSH	ES
		POP	DS
		JBE	C30_30
		MOV	DS:BlocEr,1
		MOV	DI,OFFSET CopyBufEAs
		MOV	SI,DX
		MOV	DX,2
		MOV	CX,12
		MOV	BX,0FFFFh
		MOV	AX,5703h
		CALL	Intr21
		MOV	DS:BlocEr,0
C30_30:		CALL	F35_SaveTarget
		JC	C00_72
		MOV	BX,DS:GroupTree2	; Update target TREEINFO
		CMP	BX,1
		JE	C30_40
		MOV	DX,OFFSET LineBuf2
		MOV	DS,BX
		OR	BX,BX
		JNZ	C30_31
		MOV	AX,ExpandSize
		CALL	GetTreeInfo
		MOV	ES:GroupTree2,1
		JC	C30_32
		MOV	ES:GroupTree2,DS
		CALL	AllocateBlock
C30_31:		CALL	F35_AddTree
		JNC	C30_40
C30_32:		CMP	AX,53h
		JE	C00_72

C30_40:		PUSH	ES			; Delete source directory
		POP	DS
		CMP	DS:CopyMove,0
		JE	C30_41
		MOV	DX,OFFSET LineBuf1
		CALL	ChgDisk
		JC	C00_72
		MOV	DS:LineBuf3[514-1],0
		MOV	DS:LineBuf4[514-1],0
		MOV	BYTE PTR DS:[DI+514-1],0
		CALL	F38_65
		JNC	C30_42
		CMP	AX,53h
		JE	C00_72
		CMP	AX,5
		JE	C30_41
		CMP	AX,10h
		JE	C30_41
		CMP	AX,91h
		JNE	C00_70
C30_41:		MOV	DI,OFFSET LineBuf2	; Copy / move files in directory
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		PUSH	DI
		CALL	DirFunc
		POP	DI
		MOV	BYTE PTR ES:[DI],0
		OR	DS:GroupChg,AL
		CMP	AH,1
		JA	C00_70
		JZ	C00_81
		MOV	DS:CopyResult,AX
		CMP	DS:CopyMove,0
		JE	C40_01
		CMP	DS:GroupMask[0],0
		JNZ	C40_01
		MOV	BP,OFFSET CopyMoveDir_P	; Delete source directory
		MOV	BX,500h
		CALL	F35_Table
		CALL	OncZoom
		MOV	DX,OFFSET LineBuf1
		CALL	ChgDisk
		JC	C00_72
		MOV	DS:LineBuf3[514-1],0
		MOV	DS:LineBuf4[514-1],0
		CALL	F38_65
		JNC	C30_42
		CMP	AX,53h
		JE	C00_72
		MOV	DS:FncAddr,OFFSET F38_Empty ; Empty ?
		MOV	CL,1
		XCHG	CL,DS:IncDirs
		PUSH	CX
		CALL	DirFunc
		POP	CX
		MOV	DS:IncDirs,CL
		MOV	DS:FncAddr,OFFSET C00_00
		CMP	AH,1
		JA	C00_70
		JZ	C00_81
		CMP	AL,0
		JE	C00_70
		MOV	BP,OFFSET MoveDelDirErr_B
		JMP	C00_76
C30_42:		MOV	BX,DS:GroupTree1	; Update source TREEINFO
		OR	BX,BX
		JNE	C30_45
		MOV	AX,WORD PTR ES:LineBuf1
		CMP	AH,':'
		JE	C30_43
		MOV	AL,ES:DosPath[0]
C30_43:		CALL	UpCase
		MOV	CL,AL
		MOV	AX,WORD PTR ES:LineBuf2
		CMP	AH,':'
		JE	C30_44
		MOV	AL,ES:DosPath[0]
C30_44:		CALL	UpCase
		CMP	AL,CL
		JNE	C30_45
		MOV	BX,ES:GroupTree2
		MOV	ES:GroupTree1,BX
C30_45:		CMP	BX,1
		JE	C40_00
		MOV	DX,OFFSET LineBuf3
		MOV	DS,BX
		OR	BX,BX
		JNZ	C30_46
		XOR	AX,AX
		CALL	GetTreeInfo
		MOV	ES:GroupTree1,1
		JC	C30_47
		MOV	ES:GroupTree1,DS
		CALL	AllocateBlock
C30_46:		CALL	FindTreeEntry
		JC	C40_00
		MOV	ES:GroupTreeChg1,1
		CALL	DelTreeEntry
		JMP	C40_00
C30_47:		CMP	AX,53h
		JE	C00_72

C40_00:		MOV	ES:CopyResult,1		; Update description
C40_01:		TEST	ES:GroupAttr,10h
		JNZ	C40_02
		TEST	ES:GroupAttr,8
		JNZ	C40_03
C40_02:		CMP	ES:DescrUpdate,0
		JE	C40_53
C40_03:		MOV	BX,0FFFFh		; Load source description
		CMP	ES:CopySourceDev,0
		JNE	C40_07
		MOV	AX,ES:GroupDescr1
		CMP	AX,1
		JE	C40_07
		MOV	DX,OFFSET LineBuf1
		MOV	DS,AX
		OR	AX,AX
		JNZ	C40_06
		XOR	CX,CX
		CALL	GetDescription
		JNC	C40_05
		MOV	ES:GroupDescr1,1
		CMP	AX,53h
		JNE	C40_07
		JMP	C00_72
C40_05:		MOV	ES:GroupDescr1,DS
		CALL	AllocateBlock
C40_06:		MOV	AX,ES:GroupFileNumb
		CALL	FindDescription
		JC	C40_07
		MOV	ES:CopyDescrNamLen,AX
		MOV	BX,SI
C40_07:		CMP	ES:CopyTargetDev,0	; Load target description
		JNE	C40_51
		MOV	AX,ES:GroupDescr2
		CMP	AX,1
		JE	C40_51
		MOV	SI,DX
		MOV	DX,OFFSET LineBuf2
		MOV	DS,AX
		OR	AX,AX
		JNZ	C40_20
		PUSH	ES
		POP	DS
		CMP	DS:GroupDescr1,1
		JBE	C40_08
		MOV	DI,OFFSET LineBuf3
		CALL	F35_NormPath
		JC	C40_50
		MOV	SI,DI
		CALL	CutPath
		MOV	BYTE PTR DS:[SI-1],0
		PUSH	DI
		MOV	SI,DX
		MOV	DI,OFFSET LineBuf4
		CALL	F35_NormPath
		POP	SI
		JC	C40_50
		PUSH	SI
		MOV	SI,DI
		CALL	CutPath
		MOV	BYTE PTR DS:[SI-1],0
		POP	SI
		MOV	AX,DS:GroupDescr1
		MOV	DS:GroupDescr2,AX
		CALL	CompNormLines
		MOV	DS,AX
		JZ	C40_20
C40_08:		XOR	AX,AX
		MOV	DS,AX
		PUSH	CX
		MOV	CX,ExpandSize
		CALL	GetDescription
		POP	CX
		JNC	C40_10
		MOV	ES:GroupDescr2,1
		CMP	AX,53h
		JE	C00_72
		CMP	ES:DescrCreate,0	; Create description
		JE	C40_51
		MOV	DS,ES:WorkBuffer
		MOV	BP,ES
		ADD	BP,ES:SizeDataSeg
		MOV	AX,DS
		SUB	BP,AX
		SUB	BP,MinView/16
		JBE	C40_51
		MOV	AX,ExpandSize/16
		CMP	BP,AX
		JBE	C40_09
		MOV	BP,AX
C40_09:		PUSH	CX
		MOV	CL,4
		SHL	BP,CL
		POP	CX
		PUSH	ES
		MOV	ES,ES:GroupDescr1
		MOV	DS:DscrSegSize,BP
		MOV	DS:DscrSize,OFFSET DscrBegin
		MOV	AX,ES:DscrName
		MOV	DS:DscrName,AX
		MOV	AL,ES:DscrAttr
		AND	AL,0FEh
		OR	AL,80h
		MOV	DS:DscrAttr,AL
		POP	ES
C40_10:		MOV	ES:GroupDescr2,DS
		CALL	AllocateBlock

C40_20:		MOV	BP,CX			; Delete old target description
		MOV	AL,0
		CALL	F35_Compare
		JNC	C40_23
		OR	AX,AX
		JNE	C40_50
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,DX
		MOV	DI,OFFSET HelpBuf
		CALL	ShortName
		POP	DS
		JNC	C40_21
		CMP	AX,53h
		JE	C00_72
		MOV	BYTE PTR ES:[DI],0
C40_21:		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,DI
		CALL	CutPath
		MOV	DI,OFFSET DTA.FilName
		CALL	strcpy
		POP	DS
		CALL	FindDescription
		JC	C40_23
		CMP	BX,0FFFFh
		JNE	C40_22
		CMP	ES:DescrEmpty,0
		JE	C40_53
C40_22:		CALL	DelDescription
		MOV	ES:GroupDescrChg2,1
		CMP	BX,0FFFFh
		JE	C40_23
		MOV	AX,DS
		CMP	AX,ES:GroupDescr1
		JNE	C40_23
		CMP	SI,BX
		JAE	C40_23
		SUB	BX,CX
C40_23:		CMP	BX,0FFFFh
		JE	C40_53
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET LineBuf2
		MOV	DI,OFFSET LineBuf4
		MOV	CX,256
		TEST	DS:GroupAttr,10h
		JNE	C40_24
		TEST	DS:GroupAttr,8
		JNE	C40_26
C40_24:		CALL	ShortName
		JNC	C40_25
		CMP	AX,53h
		JE	C00_72
		CMP	AX,7100h
		JNE	C40_25
		XOR	CX,CX
		JMP	C40_26
C40_25:		MOV	SI,DI
C40_26:		PUSH	DI
		CALL	CutPath
		CALL	NormName
		POP	SI
		CALL	PathLin
		MOV	DI,OFFSET FileSymb2_S
		CALL	ChkForSymb
		MOV	DI,SI
		JNC	C40_27
		PUSH	DI
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		MOV	AX,'"'
		STOSW
		MOV	SI,DI
		DEC	SI
		MOV	CX,DI
		SUB	CX,OFFSET LineBuf4
		STD
		REP	MOVSB
		STOSB
		CLD
		POP	DI
C40_27:		PUSH	DS
		MOV	DX,DS:CopyDescrNamLen
		MOV	DS,DS:GroupDescr1
		MOV	SI,BX
		ADD	SI,DX
		DEC	SI
		LODSW
		CMP	AL,'"'
		JNE	C40_28
		CMP	AL,ES:[DI]
		JE	C40_28
		CMP	AH,13
		JE	C40_28
		CMP	AH,10
		JE	C40_28
		CMP	AH,1Ah
		JE	C40_28
		CMP	AH,' '
		JE	C40_28
		CMP	AH,9
		JE	C40_28
		PUSH	DI
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		MOV	AX,' '
		STOSW
		POP	DI
C40_28:		POP	DS
		CALL	StrLen
		DEC	CX
		XCHG	CX,BP
		MOV	SI,BX
		MOV	AX,DS:GroupDescr2	; Rename description
		CMP	AX,DS:GroupDescr1
		JNE	C40_40
		CMP	DS:CopyMove,0
		JE	C40_40
		MOV	DS,AX
		MOV	CX,DX
		SUB	CX,BP
		JE	C40_31
		JB	C40_29
		ADD	DX,SI
		MOV	AX,DS:DscrSize
		SUB	DS:DscrSize,CX
		SUB	AX,DX
		JBE	C40_31
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	DS
		POP	ES
		MOV	CX,AX
		MOV	DI,SI
		ADD	DI,BP
		MOV	SI,DX
		REP	MOVSB
		POP	SI
		POP	DI
		POP	ES
		JMP	C40_31
C40_29:		NEG	CX
		MOV	BX,DS:DscrSize
		ADD	BX,CX
		JC	C40_53
		CMP	BX,DS:DscrSegSize
		JBE	C40_30
		PUSH	SI
		MOV	SI,OFFSET AllSegs_L
		MOV	AX,ExpandSize
		CALL	ExpandBlock
		POP	SI
		CMP	BX,AX
		JA	C40_53
C40_30:		ADD	DX,SI
		XCHG	BX,DS:DscrSize
		SUB	BX,DX
		JBE	C40_31
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	DS
		POP	ES
		MOV	DI,DS:DscrSize
		DEC	DI
		MOV	SI,DI
		SUB	SI,CX
		MOV	CX,BX
		STD
		REP	MOVSB
		CLD
		POP	SI
		POP	DI
		POP	ES
C40_31:		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		XCHG	SI,DI
		MOV	CX,BP
		REP	MOVSB
		PUSH	DS
		POP	ES
		MOV	DS:GroupDescrChg1,1
		JMP	C40_53

C40_40:		MOV	DS,AX			; Copy description
		MOV	AX,CX
		SUB	AX,DX
		ADD	BP,AX
		JC	C40_52
		MOV	BX,DS:DscrSize
		CMP	BX,OFFSET DscrBegin
		JBE	C40_42
		CMP	BYTE PTR DS:[BX-1],1Ah
		JNE	C40_41
		DEC	BX
		CMP	BX,OFFSET DscrBegin
		JBE	C40_42
C40_41:		MOV	AL,DS:[BX-1]
		CMP	AL,13
		JE	C40_42
		CMP	AL,10
		JE	C40_42
		ADD	BX,2
		JC	C40_52
C40_42:		ADD	BP,BX
		JC	C40_52
		XCHG	BX,BP
		MOV	AX,BX
		SUB	AX,DS:DscrSegSize
		JBE	C40_44
		CMP	AX,ExpandSize
		JAE	C40_43
		MOV	AX,ExpandSize
C40_43:		PUSH	SI
		MOV	SI,OFFSET AllSegs_L
		CALL	ExpandBlock
		POP	SI
		MOV	DS:DscrSegSize,AX
		CMP	BX,AX
		JA	C40_52
C40_44:		PUSH	ES
		PUSH	SI
		MOV	SI,DI
		MOV	DI,BP
		CMP	DI,DS:DscrSize
		JBE	C40_45
		MOV	WORD PTR DS:[DI-2],0A0Dh
C40_45:		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		CALL	strcpy
		POP	SI
		PUSH	SI
		PUSH	CX
		MOV	DS,DS:GroupDescr1
		ADD	SI,DX
		SUB	CX,DX
		JBE	C40_46
		REP	MOVSB
C40_46:		MOV	ES:DscrSize,DI
		POP	CX
		POP	SI
		POP	ES
		MOV	ES:GroupDescrChg2,1
		JMP	C40_52

C40_50:		CMP	AX,53h			; Error reading target description
		JE	C00_72
C40_51:		CMP	BX,0FFFFh
		JE	C40_53
		MOV	SI,BX
C40_52:		CMP	ES:CopyMove,0
		JE	C40_53
		MOV	DS,ES:GroupDescr1
		CALL	DelDescription
		MOV	ES:GroupDescrChg1,1
C40_53:		MOV	AX,ES:CopyResult
		JMP	C00_81

C00_75:		CMP	AX,53h			; Exit
		JE	C00_72
C00_76:		CALL	F35_Table
		CALL	Dialog
		JNZ	C00_72
C00_70:		XOR	AX,AX
		JMP	C00_81
C00_71:		MOV	AX,1
		JMP	C00_81
C00_72:		MOV	AX,100h
C00_81:		MOV	ES:SftErr,0
		MOV	ES:LineBuf3+514-1,0
		MOV	ES:LineBuf4+514-1,0
		CMP	AH,1
		CMC
		RET
C00_00		ENDP
