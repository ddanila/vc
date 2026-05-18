;---------------------------------------------------------------
;		Delete files and subdirectories
;
;		Version:	22.06.2000
;---------------------------------------------------------------

DeleteFCB	EQU	TmpBuf+200

		.CONST
Delete_S	DB	" Delete ",0
DelQuest_S	DB	"Do you wish to delete^0C",0
TheDir_S	DB	" the directory",0
DelSel_S	DB	"You have selected",0
DelSel1_S	DB	"You are ^uDELETING^u",0
DelSel2_S	DB	"^0E from",0
DelMask_S	DB	"Delete the file:",0FFh,LenStr-16,0
DelArc_S	DB	"Deleting the files from archive ",0
DelSubdir_S	DB	"Deleting the subdirectory",0FFh,7,0
DelFile_S	DB	"Deleting the file^0C",0FFh,14,0
DelFiles_S	DB	"s",0
DelSpace_S	DB	" ",0
DelReadOnly_S	DB	"The following file is marked read-only.",0
DelNotEmpty_S	DB	"The following subdirectory is not empty.",0
DelStill_S	DB	"Do you still wish to delete it?",0
DelFileErr_S	DB	"Can't delete the file",0
DelDirErr_S	DB	"Can't delete the subdirectory",0
DelWarn1_S	DB	"^uWARNING!^u",0
DelWarn2_S	DB	"Do you want to delete all the files",0
DelWarn3_S	DB	"^1E63?",0
DelWarn4_S	DB	"in the current directory",0
IncSubdirs_S	DB	"[ ] Include subdirectories",0FFh,LenStr-26,0
DelCancel_S	DB	" Delete",0FFh,3,"Cancel ",0
DelAll_S	DB	" Delete",0FFh,3,"All",0FFh,3,"Cancel ",0
DelAllSkip_S	DB	" Delete",0FFh,3,"All",0FFh,3,"Skip",0FFh,3,"sKip all",0FFh,3,"Cancel ",0

		.DATA
DelFile_T	DW	Empty_S, LineBuf1
DelFile_P	DB	Box_C, BoxCu_C, BoxCu_C, 0
		DW	Delete_S
		DW	DelQuest_S
		DW	Line1_S
		DW	DelCancel_S
		DW	0
DelFile_B	BOXHD	< DelFile_P, 500h, 0, Dialog_K, Fls_M, Delete_H, 1 >
		BOXMN	< 4, 0,0,0,0, 4F7h, DelCancel_D, 0 >
		DB	0
DelDir_T	DW	TheDir_S, LineBuf1
DelDir_P	DB	Box_C, BoxCu_C, BoxCu_C, 0
		DW	Delete_S
		DW	DelQuest_S
		DW	Quote1_S
		DW	DelCancel_S
		DW	0
DelDir_B	BOXHD	< DelDir_P, 500h, 0, Dialog_K, Fls_M, Delete_H, 1 >
		BOXMN	< 4, 0,0,0,0, 4F7h, DelCancel_D, 0 >
		DB	0
DelSel_P	DB	Box_C, BoxCu_C, BoxCu_C, 0
		DW	Delete_S
		DW	DelSel_S
		DW	Line0_S
		DW	DelCancel_S
		DW	0
DelSel_B	BOXHD	< DelSel_P, 500h, F38_30, Dialog_K, Fls_M, Delete_H, 1 >
		BOXMN	< 4, 0,0,0,0, 4F7h, DelCancel_D, 0 >
		DB	0
DelConfirm_P	DB	Err_C, ErInv_C, ErInv_C, ErBr_C
		DW	Delete_S
		DW	DelSel1_S
		DW	DelSel2_S
		DW	Line1_S
		DW	DelAll_S
		DW	0
DelConfirm_B	BOXHD	< DelConfirm_P, 900h, 0, Dialog_K, Def_M, Default_H, 1 >
		BOXMN	< 4, 0,0,0,0, 5F4h, DelAll_D, 0 >
		DB	0
DelMask_P	DB	Box_C, BoxCu_C, 0, 0
		DW	Delete_S
		DW	DelMask_S
		DW	Empty_S
		DW	0FFFFh
		DW	IncSubdirs_S
		DW	0
DelMask_B	BOXHD	< DelMask_P, 500h, 0, Dialog_K, Fls_M, Delete_H, 0 >
		BOXMN	< 1, 1,1,0,0, 305h, SourceFile, LenStrFull+LenStr >
		BOXMN	< 3, 0,0,1,1, 506h, IncDirs, 0 >
		DB	0
DelWarn_P	DB	Err_C, ErInv_C, ErInv_C, ErBr_C
		DW	Delete_S
		DW	DelWarn1_S
		DW	DelWarn2_S
		DW	DelWarn3_S
		DW	DelCancel_S
		DW	0
DelWarn_B	BOXHD	< DelWarn_P, 500h, 0, Dialog_K, Def_M, Default_H, 1 >
		BOXMN	< 4, 0,0,0,0, 5F7h, DelCancel_D, 0 >
		DB	0
DeletingFile_T	DW	DelSpace_S, LineBuf1
DeletingFiles_T	DW	DelFiles_S, LineBuf1
DeletingFile_P	DB	Box_C, 0, 0, 0
		DW	Delete_S
		DW	DelFile_S
		DW	Line1sl_S
		DW	0
DeletingDir_P	DB	Box_C, 0, 0, 0
		DW	Delete_S
		DW	DelSubdir_S
		DW	Line0sl_S
		DW	0
DeletingArc_P	DB	Box_C, 0, 0, 0
		DW	Delete_S
		DW	DelArc_S
		DW	Line1sl_S
		DW	0
DelReadOnly_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Delete_S
		DW	DelReadOnly_S
		DW	Line0_S
		DW	DelStill_S
		DW	DelAllSkip_S
		DW	0
DelReadOnly_B	BOXHD	< DelReadOnly_P, 900h, 0, Dialog_K, Def_M, Default_H, 5 >
		BOXMN	< 4, 0,0,0,0, 5EBh, DelAllSkip_D, 0FFh >
		DB	0
DelNotEmpty_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Delete_S
		DW	DelNotEmpty_S
		DW	Line0_S
		DW	DelStill_S
		DW	DelAllSkip_S
		DW	0
DelNotEmpty_B	BOXHD	< DelNotEmpty_P, 900h, 0, Dialog_K, Def_M, Default_H, 1 >
		BOXMN	< 4, 0,0,0,0, 5EBh, DelAllSkip_D, 0FFh >
		DB	0
DelFileErr_T	DW	Delete_S, LineBuf1
DelFileErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	DelFileErr_S
		DW	Line1_S
		DW	OkCan_S
		DW	0
DelFileErr_B	BOXHD	< DelFileErr_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
DelDirErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Delete_S
		DW	DelDirErr_S
		DW	Line0_S
		DW	OkCan_S
		DW	0
DelDirErr_B	BOXHD	< DelDirErr_P, 900h, 0, Dialog_K, Def_M, Default_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0

DelMask_T	DW	Delete_S, SourceFile
DelArc_T	DW	Delete_S, LineBuf1, ExecArcProgram

DelCancel_D	DB	8, 0
		DB	8, 9
		DB	0
DelAll_D	DB	8, 0
		DB	5, 9
		DB	8,15
		DB	0
DelAllSkip_D	DB	8, 0
		DB	5, 9
		DB	6,15
		DB     10,22
		DB	8,33
		DB	0

		.CODE
F38_00:		MOV	ES:FncAddr,OFFSET F38_40 ; F8 - delete
		CMP	DS:Visible,0
		JZ	F48_00
		CMP	DS:WinTyp,p_Tree
		JB	F38_03			; File panel
		JNE	F48_00                  ; Info or QuickView panel
		MOV	SI,OFFSET CrnPath	; Delete from tree panel
		CMP	BYTE PTR DS:[SI+3],0
		JE	Func04
		MOV	DI,OFFSET LineBuf1
		CALL	LongTreePath
		MOV	DX,OFFSET DelDir_T
		MOV	BP,OFFSET DelDir_B
		CALL	Dialog
		JNE	Func04
		CALL	FuncTwoNames
		JC	Func04
		MOV	AL,ES:DescrProcess
		CMP	AL,1
		JNE	F38_01
		MOV	AL,0
F38_01:		MOV	ES:DescrUpdate,AL
		MOV	CL,1
		XCHG	CL,ES:IncDirs
		XOR	BX,BX
		MOV	AL,10h
		CALL	F38_40
		MOV	ES:IncDirs,CL
		CMP	AL,0
		JE	FuncExit
		MOV	SI,OFFSET CrnPath
		CALL	CutPath
		CMP	SI,OFFSET CrnPath+3
		JBE	F38_02
		DEC	SI
F38_02:		MOV	BYTE PTR DS:[SI],0
		JMP	FuncExit
F38_03:		CMP	DS:ArcPnt,0		; Delete from archive
		JE	F38_10
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
		JC	F38_04
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,CX
		MOV	AL,4
		CALL	FindArcCmd
		POP	DS
		JNC	F38_10
F38_04:		MOV	DI,OFFSET LineBuf1
		MOV	SI,OFFSET ArcExt
		CALL	strcpy
		MOV	AL,':'
		STOSB
		CALL	PutArcPathName
		MOV	DX,OFFSET DelArc_T
		MOV	BP,OFFSET NotImpl_B
		CALL	Dialog
		JMP	Func04
F38_10:		CALL	TotSel			; Delete from file panel
		MOV	DI,OFFSET LineBuf1
		MOV	AX,BX
		OR	AX,DX
		JE	F38_12
		CMP	ES:OneFile,0
		JNZ	F38_12
		MOV	AL,0			; Delete selected files
		CALL	FilDir
		MOV	DI,OFFSET LineBuf2
		CALL	WinPath
		CMP	DS:ArcPnt,0
		JE	F38_11
		MOV	SI,OFFSET ArcExt
		CALL	strcpy
		MOV	AL,':'
		STOSB
		MOV	SI,DS:ArcPnt
		LODSW
		CALL	CutPath
		CALL	strcpy
		CALL	AddPathSep
		MOV	SI,OFFSET ArcPath
F38_11:		CALL	strcpy
		MOV	DX,OFFSET LineBufs_T
		MOV	BP,OFFSET DelSel_B
		XOR	BX,BX
		JMP	F38_14
F38_12:		MOV	SI,DS:FileCur		; Delete file / directory
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	Func04
		MOV	BX,SI
		CALL	LongNameOffset
		CALL	ChkUpDir
		JE	Func04
		MOV	DX,OFFSET DelFile_T
		MOV	BP,OFFSET DelFile_B
		TEST	DS:[BX].AttrF,10h
		JZ	F38_13
		MOV	DX,OFFSET DelDir_T
		MOV	BP,OFFSET DelDir_B
F38_13:		CALL	strcpy
F38_14:		CALL	Dialog
		JNE	Func04
		CMP	DS:ArcPnt,0
		JE	FuncSelect
		MOV	DI,OFFSET LineBuf1	; Delete from archive
		PUSH	DI
		CALL	PutArcPathName
		MOV	DX,104h
		XOR	SI,SI
		OR	BX,BX
		JE	F38_16
		MOV	DI,OFFSET LineBuf2
		PUSH	DI
		MOV	SI,OFFSET ArcPath
		CMP	BYTE PTR DS:[SI],0
		JE	F38_15
		CALL	strcpy
		CALL	AddPathSep
F38_15:		MOV	SI,BX
		CALL	LongNameOffset
		CALL	strcpy
		TEST	DS:[BX].AttrF,10h
		PUSH	ES
		POP	DS
		POP	SI
		JZ	F38_16
		OR	DH,10h
F38_16:		POP	DI
		MOV	AX,DX
		MOV	BX,500h
		MOV	DX,OFFSET DelArc_T
		MOV	BP,OFFSET DeletingArc_P
		PUSH	DX
		CALL	OncZoom
		POP	DX
		XOR	BX,BX
		XOR	BP,BP
		CALL	ExecArcCmd
		MOV	ES:GroupChg,1
		JMP	FuncExit0

F48_00:		PUSH	ES			; Shift F8 - delete
		POP	DS
		MOV	BP,OFFSET DelMask_B
		CALL	Dialog
		JNE	Func04
		MOV	DS:FncAddr,OFFSET F38_40
		MOV	DX,OFFSET DelMask_T
		MOV	BP,OFFSET F48_10
		JMP	FuncMask

F48_10		PROC	NEAR
		CMP	AL,0
		MOV	AX,0
		JNE	F48_13
		CMP	DS:GroupMask[0],AL
		JNE	F48_13
		PUSH	DS
		MOV	DS,CS:SubsSeg
		MOV	SI,OFFSET DelWarn4_S
		MOV	DI,OFFSET LineBuf2
		PUSH	DI
		CALL	strcpy
		POP	DI
		MOV	SI,OFFSET LineBuf1
		CMP	BYTE PTR ES:[SI],0
		JE	F48_12
		PUSH	DS
		PUSH	SI
		PUSH	ES
		POP	DS
		CALL	strcpy
		POP	SI
		CALL	CutPath
		CALL	FndLet
		POP	DS
		MOV	SI,OFFSET Mask_S
		CMP	AL,0
		JNE	F48_11
		INC	SI
F48_11:		CALL	strcpy
		MOV	AX,' '
		STOSW
F48_12:		POP	DS
		MOV	DX,OFFSET LineBufs_T
		MOV	BP,OFFSET DelWarn_B
		CALL	Dialog
		MOV	AX,100h
		JNZ	F48_13
		CALL	F38_80
		OR	DS:GroupFound,AL
F48_13:		RET
F48_10		ENDP

F58_00:		MOV	ES:OneFile,1		; ^F8 - delete one file
		JMP	F38_00

;---------------------------------------------------------------
;	Confirm group delete
;---------------------------------------------------------------
		.CODE	Subs
F38_30		PROC	NEAR
		CMP	AH,0FCh
		JAE	@@Exit
		CMP	AL,0Dh
		JE	@@Warning
		CMP	AL,0Ah
		JNE	@@Exit
  @@Warning:	OR	CL,CL
		JNE	@@Escape
		MOV	DX,OFFSET LineBufs_T
		MOV	BP,OFFSET DelConfirm_B
		CALL	Dialog
		MOV	ES:GroupAll,AL
		CMP	AL,1
		MOV	AX,1C0Dh
		JBE	@@Exit
  @@Escape:	MOV	AX,011Bh
  @@Exit:	RET
F38_30		ENDP

;---------------------------------------------------------------
;	Deleting file of directory
;---------------------------------------------------------------
		.CODE
F38_40		PROC	NEAR
		USES	BX,CX,DX,SI,DI,BP,DS
		PUSH	ES
		POP	DS
		MOV	DS:GroupAttr,AL
		MOV	DS:GroupFileNumb,BX
		TEST	AL,10h
		JNZ	F38_46
		MOV	BX,500h
		MOV	DX,OFFSET DeletingFile_T
		MOV	BP,OFFSET DeletingFile_P
		CALL	OncZoom
		TEST	AL,8
		JNZ	F38_55
		MOV	DX,OFFSET LineBuf1
		CALL	ChgDisk
		JC	F38_50
		MOV	AH,41h
		CALL	Intr21
		JNC	F38_54
		CMP	AX,53h
		JE	F38_50
		MOV	AX,4300h
		CALL	Intr21
		JNC	F38_41
		CMP	AX,53h
		JNE	F38_43
		JMP	F38_50
F38_41:		MOV	AL,0
		TEST	CL,10h
		JNZ	F38_43
		TEST	CL,1
		JZ	F38_43
		MOV	AL,DS:GroupAllSrcRO
		CMP	AL,1
		JE	F38_42
		CMP	AL,3
		JE	F38_45
		MOV	DS:MinMnuDefault,AL
		PUSH	DX
		MOV	DX,OFFSET LineBufs_T
		MOV	BP,OFFSET DelReadOnly_B
		CALL	Dialog
		POP	DX
		MOV	DS:GroupAllSrcRO,AL
		CMP	AL,4
		JAE	F38_50
		CMP	AL,2
		JAE	F38_45
F38_42:		AND	CL,0FEh
		MOV	AX,4301h
		CALL	Intr21
		JC	F38_43
		MOV	DS:GroupChg,1
		MOV	AH,41h
		CALL	Intr21
		JNC	F38_54
		CMP	AX,53h
		JE	F38_50
		OR	CL,1
		MOV	AX,4301h
		CALL	Intr21
		JC	F38_43
		MOV	AL,0
F38_43:		MOV	DX,OFFSET DelFileErr_T
		MOV	BP,OFFSET DelFileErr_B
F38_44:		CMP	AX,53h
		JE	F38_50
		CALL	Dialog
		JNZ	F38_50
F38_45:		XOR	AX,AX
		JMP	F38_60

F38_46:		CMP	DS:GroupMask,0		; Deleting subdirectory
		JNE	F38_48
		MOV	BX,500h
		MOV	DX,OFFSET LineBufs_T
		MOV	BP,OFFSET DeletingDir_P
		CALL	OncZoom
		MOV	DX,OFFSET LineBuf1
		CALL	ChgDisk
		JC	F38_50
		CALL	F38_65			; Try to delete subdirectory
		JNC	F38_51
		CMP	AX,53h
		JE	F38_50
		MOV	DS:FncAddr,OFFSET F38_Empty ; Empty ?
		MOV	CL,1
		XCHG	CL,DS:IncDirs
		PUSH	CX
		CALL	DirFunc
		POP	CX
		MOV	DS:IncDirs,CL
		MOV	DS:FncAddr,OFFSET F38_40
		CMP	AH,1
		JA	F38_45
		JZ	F38_60
		CMP	AL,0
		JNE	F38_49
		MOV	AL,DS:GroupAll
		CMP	AL,1
		JE	F38_47
		CMP	AL,3
		JE	F38_45
		MOV	DS:MinMnuDefault,AL
		MOV	DX,OFFSET LineBufs_T
		MOV	BP,OFFSET DelNotEmpty_B
		CALL	Dialog
		MOV	DS:GroupAll,AL
		CMP	AL,4
		JAE	F38_50
		CMP	AL,2
		JAE	F38_45
F38_47:		CALL	F38_80			; Delete *.* in subdirectory
		OR	DS:GroupFound,AL
		OR	AH,AH
		JNZ	F38_60
F38_48:		CALL	DirFunc			; Delete files in subdirectory
		CMP	AH,1
		JA	F38_45
		JZ	F38_60
		CMP	DS:GroupMask,0
		JNE	F38_60
		CMP	AL,0
		JE	F38_45
		MOV	BX,500h			; Delete subdirectory
		MOV	DX,OFFSET LineBufs_T
		MOV	BP,OFFSET DeletingDir_P
		CALL	OncZoom
		MOV	DX,OFFSET LineBuf1
		CALL	F38_65
		JNC	F38_51
		CMP	AX,53h
		JE	F38_50
F38_49:		MOV	DX,OFFSET LineBufs_T
		MOV	BP,OFFSET DelDirErr_B
		JMP	F38_44
F38_50:		MOV	AX,100h
		JMP	F38_60
F38_51:		MOV	DS:GroupChg,1
		MOV	BX,DS:GroupTree1	; Update TREEINFO
		CMP	BX,1
		JE	F38_54
		MOV	DX,OFFSET LineBuf3
		MOV	DS,BX
		OR	BX,BX
		JNZ	F38_52
		XOR	AX,AX
		CALL	GetTreeInfo
		MOV	ES:GroupTree1,1
		JC	F38_53
		MOV	ES:GroupTree1,DS
		CALL	AllocateBlock
F38_52:		CALL	FindTreeEntry
		JC	F38_54
		MOV	ES:GroupTreeChg1,1
		CALL	DelTreeEntry
		JMP	F38_54
F38_53:		CMP	AX,53h
		JE	F38_50
F38_54:		MOV	ES:GroupChg,1
		CMP	ES:DescrUpdate,0	; Update description
		JE	F38_58
F38_55:		MOV	AX,ES:GroupDescr1
		CMP	AX,1
		JE	F38_58
		MOV	DX,OFFSET LineBuf1
		MOV	DS,AX
		OR	AX,AX
		JNZ	F38_56
		XOR	CX,CX
		CALL	GetDescription
		MOV	ES:GroupDescr1,1
		JC	F38_57
		MOV	ES:GroupDescr1,DS
		CALL	AllocateBlock
F38_56:		MOV	AX,ES:GroupFileNumb
		CALL	FindDescription
		JC	F38_58
		MOV	ES:GroupDescrChg1,1
		CALL	DelDescription
		JMP	F38_59
F38_57:		CMP	AX,53h
		JE	F38_50
F38_58:		TEST	ES:GroupAttr,10h
		JNZ	F38_59
		TEST	ES:GroupAttr,8
		JNZ	F38_43
F38_59:		MOV	AX,1
F38_60:		OR	ES:GroupChg,AL
		RET
F38_40		ENDP

F38_Empty	PROC	NEAR
		XOR	AX,AX
		RET
F38_Empty	ENDP

;---------------------------------------------------------------
;	Deleting empty subdirectory
;---------------------------------------------------------------
F38_65		PROC	NEAR
		MOV	AX,4300h
		CALL	Intr21
		JC	F38_76
		MOV	SI,DX
		MOV	DI,OFFSET LineBuf3
		CALL	ShortName
		JNC	F38_66
		CMP	AX,7100h
		STC
		JNE	F38_76
		CALL	NormPath
		JC	F38_76
F38_66:		AND	CL,0EFh
		TEST	CL,1
		JZ	F38_67
		PUSH	CX
		AND	CL,0FEh
		MOV	AX,4301h
		CALL	Intr21
		POP	CX
		JNC	F38_67
		CMP	AX,53h
		STC
		JE	F38_76
F38_67:		PUSH	CX
		MOV	SI,OFFSET DosShortPath
		CALL	StrLen
		DEC	CX
		REPE	CMPSB
		POP	CX
		JNE	F38_69
		LODSB
		CMP	AL,0
		JE	F38_68
		CMP	AL,'\'
		JNE	F38_69
F38_68:		MOV	AX,10h
		STC
		PUSHF
		PUSH	AX
		JMP	F38_70
F38_69:		MOV	AH,3Ah
		CALL	Intr21
		JNC	F38_76
		PUSHF
		PUSH	AX
		CMP	AX,10h
		JNE	F38_72
		MOV	AX,WORD PTR DS:LineBuf1
		CMP	AH,':'
		JNE	F38_70
		CALL	UpCase
		CMP	AL,DS:DosPath[0]
		JNE	F38_72
F38_70:		PUSH	DX
		MOV	DX,OFFSET LineBuf4
		MOV	DI,DX
		MOV	AX,'.'
		STOSB
		STOSW
		MOV	AH,3Bh
		CALL	Intr21
		POP	DX
		JC	F38_71
		PUSH	DX
		MOV	SI,DX
		MOV	DI,OFFSET LineBuf4
		MOV	DX,DI
		CALL	NormPath
		MOV	AH,3Ah
		CALL	Intr21
		POP	DX
		POP	SI
		POP	DI
		JNC	F38_76
		PUSH	DI
		PUSH	SI
		PUSH	DX
		MOV	DX,OFFSET DosPath
		MOV	AH,3Bh
		CALL	Intr21
		POP	DX
		JNC	F38_72
		JMP	F38_73
F38_71:		CMP	AX,53h
		JE	F38_74
F38_72:		TEST	CL,1
		JZ	F38_75
		MOV	AX,4301h
		CALL	Intr21
		JNC	F38_75
F38_73:		CMP	AX,53h
		JNE	F38_75
F38_74:		POP	DI
		PUSH	AX
F38_75:		POP	AX
		POPF
F38_76:		RET
F38_65		ENDP

;---------------------------------------------------------------
;	Deleting files by mask *.*
;---------------------------------------------------------------
F38_80		PROC	NEAR
		USES	BX,CX,DX,SI,DI,BP,DS
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET LineBuf1	; Put box
		MOV	DI,SI
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		CALL	CutPath
		CALL	FndLet
		PUSH	DS
		MOV	DS,CS:SubsSeg
		MOV	SI,OFFSET Mask_S
		CMP	AL,0
		JNE	F38_81
		INC	SI
F38_81:		PUSH	DI
		CALL	strcpy
		POP	DI
		POP	DS
		MOV	BX,500h
		MOV	DX,OFFSET DeletingFiles_T
		MOV	BP,OFFSET DeletingFile_P
		CALL	OncZoom
		MOV	BYTE PTR DS:[DI],0
		MOV	DI,OFFSET DeleteFCB	; Prepare FCB
		MOV	AL,0FFh
		STOSB
		ADD	DI,5
		MOV	AL,26h
		STOSB
		MOV	AL,0
		STOSB
		MOV	AL,'?'
		MOV	CX,8+3
		REP	STOSB
		MOV	DX,OFFSET LineBuf1	; Save current path
		CALL	ChgDisk
		JC	F38_84
		MOV	DI,OFFSET LineBuf2
		MOV	BYTE PTR DS:[DI],0
		MOV	SI,DX
		LODSW
		CMP	AL,0
		JE	F38_85
		CMP	AH,':'
		JNE	F38_82
		STOSW
		CALL	UpCase
		SUB	AL,'A'-1
		MOV	DS:DeleteFCB[7],AL
		LODSB
		STOSB
		CMP	AL,0
		JE	F38_85
		DEC	DI
F38_82:		MOV	AL,'\'
		STOSB
		PUSH	DX
		MOV	SI,DI
		MOV	DL,DS:DeleteFCB[7]
		MOV	AH,47h
		CALL	Intr21
		POP	DX
		JC	F38_83
		MOV	AH,3Bh			; Set current dir
		CALL	Intr21
		JNC	F38_85
F38_83:		CMP	AX,53h
		JE	F38_84
		XOR	AX,AX
		JMP	F38_87
F38_84:		MOV	AX,100h
		JMP	F38_87
F38_85:		MOV	DS:GroupChg,1		; Delete files
		MOV	DX,OFFSET DeleteFCB
		MOV	AH,13h
		CALL	Intr21
		MOV	BX,1
		JNC	F38_86
		CMP	AX,53h
		JE	F38_84
		MOV	BL,0
F38_86:		MOV	AX,BX
		CMP	DS:LineBuf2[0],0	; Restore current dir
		JE	F38_87
		MOV	DX,OFFSET LineBuf2
		MOV	AH,3Bh
		CALL	Intr21
		JC	F38_84
		MOV	AX,BX
F38_87:		RET
F38_80		ENDP
