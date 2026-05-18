;---------------------------------------------------------------
;		Change attributes of files and subdirectories
;
;		Version:	22.06.2000
;---------------------------------------------------------------

AttrSet		EQU	BYTE PTR TmpBuf[0]
AttrClr		EQU	BYTE PTR TmpBuf[10]
AttrDate	EQU	BYTE PTR TmpBuf[15]
AttrTime	EQU	BYTE PTR TmpBuf[30]
AttribMask	EQU	WORD PTR TmpBuf[50]
AttribDate	EQU	WORD PTR TmpBuf[52]
AttribTime	EQU	WORD PTR TmpBuf[54]
AttribEA	EQU	BYTE PTR TmpBuf[56]
AttribOld	EQU	BYTE PTR TmpBuf[57]
AttrBufEA	EQU	BYTE PTR TmpBuf[58]

		.CONST
Attrib_S	DB	" Attributes ",0
AttrChg_S	DB	" Change file attributes ^0C",0
AttrFor_S	DB	"for ",0
AttrName_S	DB	'"^4E26"',0
AttrCol_S	DB	"^uSet  Clear^u",0FFh,21,0
Attrib1_S	DB	" [ ]  [ ] Read only",0FFh,2,"^1C",0
Attrib2_S	DB	" [ ]  [ ] Archive"  ,0FFh,4,"^2C",0
Attrib3_S	DB	" [ ]  [ ] Hidden"   ,0FFh,5,"^3C",0
Attrib4_S	DB	" [ ]  [ ] System"   ,0FFh,5,"^2C",0
Attrib5_S	DB	"Size of EAs is ^5L byte^5Q/s/",0
Attrib6_S	DB	0FFh,4
Attrib7_S	DB	"[ ] Clear EAs^2C",0
AttrDate_S	DB	"  Date      ",0
AttrTime_S	DB	"  Time      ",0
AttrSpc_S	DB	"            ",0
AttrSet_S	DB	"Setting attributes:",0FFh,13,0
AttrErr_S	DB	"Can't set attributes",0
AttrErrEAs_S	DB	"Can't delete extended attributes",0
AttrErrDir_S	DB	"Can't find the directory",0
AttrBadTime1_S	DB	"Can't set the attributes.",0
AttrBadTime2_S	DB	"Illegal date or time.",0
SetCancel_S	DB	"[ Set ]   [ Cancel ]",0

		.DATA
AttrFile_T	DW	AttrFor_S, AttrDate_S, AttrSpc_S, AttrTime_S, LineBuf1, AttrBufEA[22]
AttrDir_T	DW	AttrFor_S, Empty_S   , Empty_S  , Empty_S   , LineBuf1, AttrBufEA[22]
AttrName_P	DB	Box_C, BoxCu_C, Box_C, BoxBr_C
		DW	Attrib_S
		DW	AttrChg_S
		DW	AttrName_S
		DW	Attrib1_S + 5
		DW	Attrib2_S + 5
		DW	Attrib3_S + 5
		DW	Attrib4_S + 5
		DW	0FFFFh
		DW	SetCancel_S
		DW	0
AttrFile_B	BOXHD	< AttrName_P, 400h, F87_60, Dialog_K, Fls_M, Attrib_H, 0 >
		BOXMN	< 3, 6,1,0,4, 407h, AttrSet+0, 0 >	; Read only
		BOXMN	< 3, 0,2,1,4, 507h, AttrSet+1, 0 >	; Archive
		BOXMN	< 3, 1,3,2,5, 607h, AttrSet+2, 0 >	; Hidden
		BOXMN	< 3, 2,6,3,5, 707h, AttrSet+3, 0 >	; System
		BOXMN	< 1, 3,5,1,4, 517h, AttrDate ,10 >	; Date
		BOXMN	< 1, 4,6,3,5, 717h, AttrTime ,10 >	; Time
		BOXMN	< 4, 5,0,6,6, 9F6h, SetCancel_D, 0 >	; Set / Cancel
		DB	0
AttrDir_B	BOXHD	< AttrName_P, 400h, 0, Dialog_K, Fls_M, Attrib_H, 0 >
		BOXMN	< 3, 4,1,0,0, 40Dh, AttrSet+0, 0 >	; Read only
		BOXMN	< 3, 0,2,1,1, 50Dh, AttrSet+1, 0 >	; Archive
		BOXMN	< 3, 1,3,2,2, 60Dh, AttrSet+2, 0 >	; Hidden
		BOXMN	< 3, 2,4,3,3, 70Dh, AttrSet+3, 0 >	; System
		BOXMN	< 4, 3,0,4,4, 9F6h, SetCancel_D, 0 >	; Set / Cancel
		DB	0
AttrNameEA_P	DB	Box_C, BoxCu_C, Box_C, BoxBr_C
		DW	Attrib_S
		DW	AttrChg_S
		DW	AttrName_S
		DW	Attrib1_S + 5
		DW	Attrib2_S + 5
		DW	Attrib3_S + 5
		DW	Attrib4_S + 5
		DW	0FFFFh
		DW	Attrib5_S
		DW	Attrib7_S
		DW	0FFFFh
		DW	SetCancel_S
		DW	0
AttrFileEA_B	BOXHD	< AttrNameEA_P, 400h, F87_60, Dialog_K, Fls_M, Attrib_H, 0 >
		BOXMN	< 3, 7,1,0,4, 407h, AttrSet+0, 0 >	; Read only
		BOXMN	< 3, 0,2,1,4, 507h, AttrSet+1, 0 >	; Archive
		BOXMN	< 3, 1,3,2,5, 607h, AttrSet+2, 0 >	; Hidden
		BOXMN	< 3, 2,6,3,5, 707h, AttrSet+3, 0 >	; System
		BOXMN	< 1, 3,5,1,4, 517h, AttrDate ,10 >	; Date
		BOXMN	< 1, 4,6,3,5, 717h, AttrTime ,10 >	; Time
		BOXMN	< 3, 5,7,6,6,0A07h, AttribEA , 0 >	; EAs
		BOXMN	< 4, 6,0,7,7,0CF6h, SetCancel_D, 0 >	; Set / Cancel
		DB	0
AttrDirEA_B	BOXHD	< AttrNameEA_P, 400h, 0, Dialog_K, Fls_M, Attrib_H, 0 >
		BOXMN	< 3, 5,1,0,0, 40Dh, AttrSet+0, 0 >	; Read only
		BOXMN	< 3, 0,2,1,1, 50Dh, AttrSet+1, 0 >	; Archive
		BOXMN	< 3, 1,3,2,2, 60Dh, AttrSet+2, 0 >	; Hidden
		BOXMN	< 3, 2,4,3,3, 70Dh, AttrSet+3, 0 >	; System
		BOXMN	< 3, 3,5,4,4,0A0Dh, AttribEA , 0 >	; EAs
		BOXMN	< 4, 4,0,5,5,0CF6h, SetCancel_D, 0 >	; Set / Cancel
		DB	0
AttrFiles_T	DW	Empty_S, AttrDate_S, AttrSpc_S, AttrTime_S
AttrFiles_P	DB	Box_C, BoxCu_C, Box_C, BoxBr_C
		DW	Attrib_S
		DW	AttrChg_S
		DW	AttrCol_S
		DW	Attrib1_S
		DW	Attrib2_S
		DW	Attrib3_S
		DW	Attrib4_S
		DW	0FFFFh
		DW	SetCancel_S
		DW	0
AttrFiles_B	BOXHD	< AttrFiles_P, 400h, F87_60, Dialog_K, Fls_M, Attrib_H, 0 >
		BOXMN	< 3,10, 1, 0, 4, 407h, AttrSet+0, 0 >	; Read only
		BOXMN	< 3, 0, 2, 1, 5, 507h, AttrSet+1, 0 >	; Archive
		BOXMN	< 3, 1, 3, 2, 6, 607h, AttrSet+2, 0 >	; Hidden
		BOXMN	< 3, 2,10, 3, 7, 707h, AttrSet+3, 0 >	; System
		BOXMN	< 3, 3, 5, 0, 8, 40Ch, AttrClr+0, 0 >	; Read only
		BOXMN	< 3, 4, 6, 1, 8, 50Ch, AttrClr+1, 0 >	; Archive
		BOXMN	< 3, 5, 7, 2, 9, 60Ch, AttrClr+2, 0 >	; System
		BOXMN	< 3, 6,10, 3, 9, 70Ch, AttrClr+3, 0 >	; Hidden
		BOXMN	< 1, 7, 9, 5, 8, 51Ch, AttrDate ,10 >	; Date
		BOXMN	< 1, 8,10, 7, 9, 71Ch, AttrTime ,10 >	; Time
		BOXMN	< 4, 9, 0,10,10, 9F6h, SetCancel_D, 0 >	; Set / Cancel
		DB	0
AttrFilesEA_P	DB	Box_C, BoxCu_C, Box_C, BoxBr_C
		DW	Attrib_S
		DW	AttrChg_S
		DW	AttrCol_S
		DW	Attrib1_S
		DW	Attrib2_S
		DW	Attrib3_S
		DW	Attrib4_S
		DW	0FFFFh
		DW	Attrib6_S
		DW	0FFFFh
		DW	SetCancel_S
		DW	0
AttrFilesEA_B	BOXHD	< AttrFilesEA_P, 400h, F87_60, Dialog_K, Fls_M, Attrib_H, 0 >
		BOXMN	< 3,11, 1, 0, 4, 407h, AttrSet+0, 0 >	; Read only
		BOXMN	< 3, 0, 2, 1, 5, 507h, AttrSet+1, 0 >	; Archive
		BOXMN	< 3, 1, 3, 2, 6, 607h, AttrSet+2, 0 >	; Hidden
		BOXMN	< 3, 2,10, 3, 7, 707h, AttrSet+3, 0 >	; System
		BOXMN	< 3, 3, 5, 0, 8, 40Ch, AttrClr+0, 0 >	; Read only
		BOXMN	< 3, 4, 6, 1, 8, 50Ch, AttrClr+1, 0 >	; Archive
		BOXMN	< 3, 5, 7, 2, 9, 60Ch, AttrClr+2, 0 >	; System
		BOXMN	< 3, 6,10, 3, 9, 70Ch, AttrClr+3, 0 >	; Hidden
		BOXMN	< 1, 7, 9, 5, 8, 51Ch, AttrDate ,10 >	; Date
		BOXMN	< 1, 8,10, 7, 9, 71Ch, AttrTime ,10 >	; Time
		BOXMN	< 3, 9,11,10,10, 90Ch, AttribEA , 0 >	; EAs
		BOXMN	< 4,10, 0,11,11,0BF6h, SetCancel_D, 0 >	; Set / Cancel
		DB	0
AttrSet_P	DB	Box_C, 0, 0, 0
		DW	Attrib_S
		DW	AttrSet_S
		DW	Line0sl_S
		DW	0
AttrBadTime_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Attrib_S
		DW	AttrBadTime1_S
		DW	AttrBadTime2_S
		DW	Ok_S
		DW	0
AttrBadTime_B	BOXHD	< AttrBadTime_P, 0C00h, 0, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
AttrErrFnd_T	DW	NoFnd_S, LineBuf1
AttrErr_T	DW	AttrErr_S, LineBuf1
AttrErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Attrib_S
		DW	Inc0_S
		DW	Line1_S
		DW	OkCan_S
		DW	0
AttrErr_B	BOXHD	< AttrErr_P, 900h, 0, Dialog_K, Def_M, Default_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FAh, OkCan_D, 0 >
		DB	0
AttrErrDir_T	DW	AttrErrDir_S, LineBuf1
AttrErrEAs_T	DW	AttrErrEAs_S, LineBuf1
AttrErrDir_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Attrib_S
		DW	Inc0_S
		DW	Line1_S
		DW	Ok_S
		DW	0
AttrErrDir_B	BOXHD	< AttrErrDir_P, 900h, 0, Dialog_K, Def_M, Default_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0

SetCancel_D	DB	 7,  0
		DB	10, 10
		DB	 0

		.CODE
F87_00:		CMP	DS:Visible,0		; Attributes...
		JZ	Func04
		CMP	DS:WinTyp,p_Tree
		JA	Func04			; Info or QuickView panel
		JB	F87_20                  ; File panel
		MOV	ES:AttribEA,0
		MOV	SI,OFFSET CrnPath	; Directory
		CMP	BYTE PTR DS:[SI+3],0
		JE	Func04
		MOV	DX,OFFSET LineBuf1
		MOV	DI,DX
		CALL	LongTreePath
		PUSH	ES
		POP	DS
		CALL	ChgDisk
		JC	Func04
		MOV	AX,4300h
		CALL	Intr21
		JC	F87_15
		CALL	F87_ParseAttr
		PUSH	CX
		MOV	BX,OFFSET AttrDir_T
		MOV	SI,OFFSET AttrDir_B
		MOV	BP,OFFSET AttrDirEA_B
		CALL	GetFileTime
		JC	F87_01
		MOV	SI,OFFSET LineBuf2
		MOV	DS:[SI].TimeF,CX
		MOV	DS:[SI].DateF,DI
		MOV	DI,OFFSET AttrDate
		PUSH	DI
		MOV	AL,1
		CALL	PutDate
		MOV	DI,OFFSET AttrTime
		PUSH	DI
		CALL	PutTime
		POP	SI
		CALL	NormStr
		POP	SI
		CALL	NormStr
		MOV	BX,OFFSET AttrFile_T
		MOV	SI,OFFSET AttrFile_B
		MOV	BP,OFFSET AttrFileEA_B
F87_01:		POP	CX
		MOV	DS:BlocEr,1
		CALL	F87_ChkEAs
		MOV	DS:BlocEr,0
		JNC	F87_02
		MOV	BP,SI
F87_02:		MOV	DX,BX
		MOV	DS:AttribDate,0FFFFh
		MOV	DS:AttribTime,0FFFFh
		CALL	Dialog
		JNE	Func04
		MOV	AX,DS:AttribDate
		CMP	AX,DS:LineBuf2.DateF
		JNE	F87_03
		MOV	DS:AttribDate,0FFFFh
F87_03:		MOV	AX,DS:AttribTime
		CMP	AX,DS:LineBuf2.TimeF
		JNE	F87_04
		MOV	DS:AttribTime,0FFFFh
F87_04:		MOV	SI,OFFSET AttrSet
		CALL	F87_CombAttr
		MOV	AH,CL
		AND	AH,27h
		MOV	DS:AttribOld,AH
		CMP	AL,AH
		JNE	F87_05
		CMP	DS:AttribEA,0
		JNE	F87_05
		MOV	BX,DS:AttribDate
		AND	BX,DS:AttribTime
		INC	BX
		JE	Func04
F87_05:		AND	CL,0C0h
		OR	CL,AL
		MOV	BX,500h
		MOV	DX,OFFSET LineBufs_T
		MOV	BP,OFFSET AttrSet_P
		CALL	OncZoom
		MOV	DX,OFFSET LineBuf1
		CALL	ChgDisk
		JC	FuncExit
		MOV	AX,4301h
		CALL	Intr21
		JC	F87_15
		MOV	DS:GroupChg,1
		MOV	AX,DS:AttribDate
		AND	AX,DS:AttribTime
		INC	AX
		JE	F87_08
		PUSH	CX
		MOV	CX,DS:AttribTime
		CMP	CX,0FFFFh
		JNE	F87_06
		MOV	CX,DS:LineBuf2.TimeF
F87_06:		MOV	DI,DS:AttribDate
		CMP	DI,0FFFFh
		JNE	F87_07
		MOV	DI,DS:LineBuf2.DateF
F87_07:		CALL	SetFileTime
		POP	CX
		JC	F87_15
F87_08:		CALL	F87_DelEAs
		JNC	F87_10
		CMP	AX,53h
		JE	FuncExit
		MOV	DX,OFFSET AttrErrEAs_T
		JMP	F87_16
F87_10:		XOR	AX,AX			; Update TREEINFO
		CALL	GetTreeInfo
		JC	FuncExit
		CALL	FindTreeEntry
		JC	FuncExit
		AND	CL,27h
		MOV	BH,CL
		MOV	BL,CL
		XCHG	BL,DS:[SI].DirAttr
		XOR	BL,CL
		CMP	ES:AttribEA,0
		JZ	F87_11
		CMP	DS:[SI].DirOS2Len,0
		JE	F87_11
		MOV	BL,1
		MOV	CX,SI
		MOV	SI,DS:TreeLen
		CALL	TreeNumb2Adr1
		XCHG	CX,SI
		MOV	DI,SI
		CALL	NextDirEnt
		XOR	AX,AX
		XCHG	AX,DS:[DI].DirOS2Len
		SUB	CX,SI
		JBE	F87_11
		PUSH	ES
		PUSH	DS
		POP	ES
		MOV	DI,SI
		SUB	DI,AX
		REP	MOVSB
		POP	ES
F87_11:		OR	BL,BL
		JE	F87_12
		PUSH	BX
		CALL	SaveTreeInfo
		POP	BX
F87_12:		CMP	ES:HidDirs,0
		JNZ	FuncExit
		TEST	BH,6
		JZ	FuncExit
		MOV	DS,ES:WCBcrn
		MOV	SI,OFFSET CrnPath
		CALL	CutPath
		CMP	SI,OFFSET CrnPath+3
		JBE	F87_13
		DEC	SI
F87_13:		MOV	BYTE PTR DS:[SI],0
		JMP	FuncExit
F87_15:		CMP	AX,53h			; Error
		JE	FuncExit
		MOV	DX,OFFSET AttrErr_T
		CMP	AX,3
		JA	F87_16
		MOV	DX,OFFSET AttrErrDir_T
F87_16:		MOV	BP,OFFSET AttrErrDir_B
		CALL	Dialog
		JMP	FuncExit
F87_20:		CMP	DS:ArcPnt,0		; From file panel
		JNE	Func04
		CALL	TotSel
		OR	BX,DX
		JNE	F87_24
		MOV	SI,DS:FileCur		; One file
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	Func04
		PUSH	SI
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		POP	SI
		JE	Func04
		MOV	CL,DS:[SI].AttrF
		TEST	CL,10h
		JNZ	F87_24
		TEST	CL,8
		JNZ	Func04
		CALL	F87_ParseAttr
		MOV	DI,OFFSET AttrDate
		PUSH	DI
		MOV	AL,1
		CALL	PutDate
		MOV	DI,OFFSET AttrTime
		PUSH	DI
		CALL	PutTime
		CALL	LongNameOffset
		MOV	DX,SI
		MOV	DI,OFFSET LineBuf1
		CALL	strcpy
		POP	SI
		CALL	NormStr
		POP	SI
		CALL	NormStr
		CALL	F87_ChkEAs
		MOV	BP,OFFSET AttrFileEA_B
		JNC	F87_21
		CMP	AX,53h
		JE	FuncExit
		MOV	BP,OFFSET AttrFile_B
F87_21:		MOV	DI,DS:[BX].DateF
		MOV	BX,DS:[BX].TimeF
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET AttrFile_T
		CALL	Dialog
		JNE	Func04
		MOV	AX,DS:AttribTime
		CMP	AX,BX
		JNE	F87_22
		MOV	DS:AttribTime,0FFFFh
F87_22:		MOV	BX,DS:AttribDate
		CMP	BX,DI
		JNE	F87_23
		MOV	DS:AttribDate,0FFFFh
F87_23:		MOV	SI,OFFSET AttrSet
		CALL	F87_CombAttr
		MOV	AH,AL
		CMP	AL,CL
		JNE	F87_26
		CMP	DS:AttribEA,0
		JNE	F87_26
		MOV	BX,DS:AttribTime
		AND	BX,DS:AttribDate
		INC	BX
		JNE	F87_26
		JMP	Func04
F87_24:		PUSH	ES			; Selected files
		POP	DS
		MOV	BP,OFFSET AttrFiles_B
		CMP	DS:OS2box,0
		JE	F87_25
		MOV	BP,OFFSET AttrFilesEA_B
F87_25:		MOV	DX,OFFSET AttrFiles_T
		CALL	Dialog
		JNE	Func04
		MOV	SI,OFFSET AttrSet
		CALL	F87_CombAttr
		MOV	AH,AL
		MOV	SI,OFFSET AttrClr
		CALL	F87_CombAttr
		NOT	AL
		CMP	AX,0FFh
		JNE	F87_26
		MOV	CX,DS:AttribDate
		AND	CX,DS:AttribTime
		INC	CX
		JNE	F87_26
		CMP	DS:AttribEA,0
		JE	Func04
F87_26:		MOV	DS:AttribMask,AX
		MOV	DS:FncAddr,OFFSET F87_50
		JMP	FuncSelect

;---------------------------------------------------------------
;	Setting attributes
;---------------------------------------------------------------
F87_50		PROC	NEAR
		USES	BX,CX,DX,SI,DI,BP,DS
		PUSH	ES
		POP	DS
		TEST	AL,10h
		JZ	F87_51
		CALL	DirFunc
		CMP	AH,1
		JBE	F87_59
		MOV	AH,0
		JMP	F87_59
F87_51:		MOV	BX,500h
		MOV	DX,OFFSET LineBufs_T
		MOV	BP,OFFSET AttrSet_P
		CALL	OncZoom
		TEST	AL,8
		JNZ	F87_57
		MOV	DX,OFFSET LineBuf1
		CALL	ChgDisk
		JC	F87_55
		MOV	AX,4300h
		CALL	Intr21
		JC	F87_57
		MOV	DI,CX
		MOV	AX,DS:AttribDate
		AND	AX,DS:AttribTime
		INC	AX
		JE	F87_54
		AND	CL,0FEh
		MOV	AX,4301h
		CALL	Intr21
		JC	F87_57
		MOV	DS:GroupChg,1
		MOV	AL,41h
		CALL	OpenFil
		JC	F87_57
		MOV	BX,AX
		PUSH	DX
		MOV	AX,5700h
		CALL	Intr21
		MOV	AX,DS:AttribDate
		CMP	AX,0FFFFh
		JE	F87_52
		MOV	DX,AX
F87_52:		MOV	AX,DS:AttribTime
		CMP	AX,0FFFFh
		JE	F87_53
		MOV	CX,AX
F87_53:		MOV	AX,5701h
		CALL	Intr21
		POP	DX
		MOV	AH,3Eh
		CALL	Intr21
		JC	F87_57
F87_54:		MOV	CX,DI
		MOV	AX,DS:AttribMask
		AND	CL,AL
		OR	CL,AH
		MOV	AX,4301h
		CALL	Intr21
		JC	F87_57
		CALL	F87_DelEAs
		JC	F87_56
		MOV	AX,1
		OR	DS:GroupChg,AL
		JMP	F87_59
F87_55:		MOV	AX,100h
		JMP	F87_59
F87_56:		CMP	AX,53h
		JE	F87_55
		MOV	DX,OFFSET AttrErrEAs_T
		JMP	F87_58
F87_57:		CMP	AX,53h
		JE	F87_55
		MOV	DX,OFFSET AttrErrFnd_T
		CMP	AX,3
		JBE	F87_58
		MOV	DX,OFFSET AttrErr_T
F87_58:		MOV	BP,OFFSET AttrErr_B
		CALL	Dialog
		JNE	F87_55
		XOR	AX,AX
F87_59:		RET
F87_50		ENDP

;---------------------------------------------------------------
;	Check date and time
;---------------------------------------------------------------
		.CODE	Subs
F87_60		PROC	NEAR			; Test date & time
		CMP	AH,0FCh
		JAE	F87_75
		OR	CL,CL
		JNE	F87_75
		CMP	AL,0Dh
		JE	F87_61
		CMP	AL,0Ah
		JNE	F87_75
F87_61:		MOV	ES:AttribDate,0FFFFh
		MOV	ES:AttribTime,0FFFFh
		ADD	SI,SIZE BoxHd
F87_62:		CMP	CS:[SI].mType,1
		JE	F87_63
		DEC	DI
		ADD	SI,SIZE BoxMn
		JMP	F87_62
F87_63:		PUSH	SS
		POP	DS
		SUB	DI,11
		MOV	SI,DI
		CALL	FndLet
		CMP	AL,0
		JE	F87_68
		CALL	TxtNum
		JC	F87_74
		MOV	CX,AX
		CALL	FndLet
		CMP	AL,ES:Country.DateSep
		JNE	F87_74
		INC	SI
		CALL	FndLet
		CALL	TxtNum
		JC	F87_74
		MOV	DX,AX
		CALL	FndLet
		CMP	AL,ES:Country.DateSep
		JNE	F87_74
		INC	SI
		CALL	FndLet
		CALL	TxtNum
		JC	F87_74
		MOV	BX,AX
		CALL	FndLet
		CMP	AL,0
		JNE	F87_74
		CMP	ES:Country.DateFmt,1
		JB	F87_64
		XCHG	CX,DX
		JE	F87_64
		XCHG	BX,DX
F87_64:		SUB	BX,1980
		JAE	F87_65
		ADD	BX,1980
		CMP	BX,100
		JAE	F87_74
		SUB	BL,80
		JAE	F87_65
		ADD	BL,100
F87_65:		CMP	BX,128
		JAE	F87_74
		CMP	CX,12
		JA	F87_74
		CMP	CL,1
		JB	F87_74
		CMP	DX,31
		JA	F87_74
		CMP	DL,1
		JB	F87_74
		MOV	AL,CL
		CMP	AL,7
		JBE	F87_66
		INC	AL
F87_66:		AND	AL,1
		ADD	AL,30
		CMP	CL,2
		JNE	F87_67
		MOV	AL,28
		TEST	BL,3
		JNZ	F87_67
		MOV	AL,29
F87_67:		CMP	DL,AL
		JA	F87_74
		MOV	AL,CL
		MOV	AH,0
		MOV	CL,5
		SHL	AX,CL
		SHL	BL,1
		ADD	AL,DL
		ADD	AH,BL
		MOV	ES:AttribDate,AX
F87_68:		LEA	SI,[DI-17]
		CALL	FndLet
		CMP	AL,0
		JE	F87_73
		XOR	CX,CX
		XOR	BX,BX
		CALL	TxtNum
		JC	F87_74
		OR	AH,AH
		JNE	F87_74
		MOV	CL,AL
		CALL	FndLet
		CMP	AL,ES:Country.TimeSep
		JNE	F87_69
		INC	SI
		CALL	FndLet
		CALL	TxtNum
		JC	F87_74
		OR	AH,AH
		JNE	F87_74
		MOV	CH,AL
		CALL	FndLet
		CMP	AL,ES:Country.TimeSep
		JNE	F87_69
		INC	SI
		CALL	FndLet
		CALL	TxtNum
		JC	F87_74
		OR	AH,AH
		JNE	F87_74
		MOV	BL,AL
		CALL	FndLet
F87_69:		CMP	AL,0
		JE	F87_70
		MOV	BH,AL
		INC	SI
		CALL	FndLet
		CMP	AL,0
		JNE	F87_74
F87_70:		CMP	CH,60
		JAE	F87_74
		CMP	BL,60
		JAE	F87_74
		MOV	AX,24
		CMP	BH,0
		JE	F87_71
		MOV	AL,12
		CMP	BH,'A'
		JE	F87_71
		CMP	BH,'a'
		JE	F87_71
		MOV	AH,12
		CMP	BH,'P'
		JE	F87_71
		CMP	BH,'p'
		JNE	F87_74
F87_71:		CMP	CL,AL
		JA	F87_74
		JNE	F87_72
		MOV	CL,0
F87_72:		ADD	AH,CL
		MOV	CL,3
		SHL	AH,CL
		MOV	AL,BL
		SHR	AL,1
		MOV	BH,CH
		MOV	BL,0
		SHR	BX,CL
		ADD	AX,BX
		MOV	ES:AttribTime,AX
F87_73:		MOV	AX,1C0Dh
		JMP	F87_75
F87_74:		MOV	BP,OFFSET AttrBadTime_B
		CALL	Dialog
		XOR	AX,AX
F87_75:		RET
F87_60		ENDP

;---------------------------------------------------------------
;	Parse attributes
;		   CX - attributes
;---------------------------------------------------------------
		.CODE
F87_ParseAttr	PROC	NEAR
		MOV	DI,OFFSET AttrSet
		MOV	AL,1
		CALL	F87_Parse1
		MOV	AL,20h
		CALL	F87_Parse1
		MOV	AL,2
		CALL	F87_Parse1
		MOV	AL,4
		CALL	F87_Parse1
		RET
F87_ParseAttr	ENDP

F87_Parse1	PROC	NEAR
		TEST	AL,CL
		MOV	AL,0
		JZ	@@Exit
		MOV	AL,1
  @@Exit:	STOSB
		RET
F87_Parse1	ENDP

;---------------------------------------------------------------
;	Combine attributes
;		DS:SI - pointer to attributes
;	Return:	   AL - attributes
;---------------------------------------------------------------
F87_CombAttr	PROC	NEAR
		USES	BX
		MOV	AL,0
		MOV	BL,1
		CALL	F87_Comb1
		MOV	BL,20h
		CALL	F87_Comb1
		MOV	BL,2
		CALL	F87_Comb1
		MOV	BL,4
		CALL	F87_Comb1
		RET
F87_CombAttr	ENDP

F87_Comb1	PROC	NEAR
		CMP	BYTE PTR DS:[SI],0
		JE	@@Exit
		OR	AL,BL
  @@Exit:	INC	SI
		RET
F87_Comb1	ENDP

;---------------------------------------------------------------
;	Check EAs
;		DS:DX - file
;	Return:    CY - no EAs or error
;---------------------------------------------------------------
F87_ChkEAs	PROC	NEAR
		USES	BX,CX,DX,SI,DI
		CMP	ES:OS2box,0
		JE	@@Error
		CALL	ChgDisk
		MOV	AX,53h
		JC	@@Error
		MOV	SI,DX
		MOV	DI,OFFSET AttrBufEA
		MOV	DX,2
		MOV	CX,26
		MOV	BX,0FFFFh
		MOV	AX,5702h
		CALL	Intr21
		JC	@@Exit
		CMP	ES:AttrBufEA[22].Hi,0
		JNE	@@Exit
		CMP	ES:AttrBufEA[22].Lo,5
		JAE	@@Exit
  @@Error:	MOV	AL,0
		STC
  @@Exit:	RET
F87_ChkEAs	ENDP

;---------------------------------------------------------------
;	Delete EAs
;	Return:    CY - error
;---------------------------------------------------------------
F87_DelEAs	PROC	NEAR
		USES	BX,CX,DX,SI,DI,DS
		CMP	DS:AttribEA,0
		JE	@@Exit
		MOV	AX,DS:WorkBuffer
		MOV	BX,ES
		ADD	BX,DS:SizeDataSeg
		SUB	BX,AX
		MOV	CX,0FFFFh
		CMP	BX,1000h
		JAE	@@GetEAs
		MOV	CL,4
		SHL	BX,CL
		MOV	CX,BX
  @@GetEAs:	MOV	DS,AX
		MOV	DS:[0].Lo,CX
		MOV	DS:[0].Hi,0
		PUSH	ES
		POP	DS
		MOV	DS:AttrBufEA[0].Offs,0
		MOV	DS:AttrBufEA[0].Segm,0
		MOV	DS:AttrBufEA[4].Offs,0
		MOV	DS:AttrBufEA[4].Segm,AX
		MOV	SI,OFFSET LineBuf1
		MOV	DI,OFFSET AttrBufEA
		MOV	DX,4
		MOV	CX,12
		MOV	BX,0FFFFh
		MOV	AX,5702h
		CALL	Intr21
		JC	@@Exit
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	CX
		PUSH	BX
		MOV	DS,DS:WorkBuffer
		MOV	SI,4
		MOV	DI,SI
		PUSH	DS
		POP	ES
  @@Prepare:	CMP	SI,DS:[0]
		JAE	@@DelEA
		LODSW
		STOSW
		MOV	CL,AH
		MOV	CH,0
		LODSW
		MOV	DX,AX
		XOR	AX,AX
		STOSW
		INC	CX
		REP	MOVSB
		ADD	SI,DX
		JMP	@@Prepare
  @@DelEA:	MOV	DS:[0],DI
		POP	BX
		POP	CX
		POP	SI
		POP	DI
		POP	ES
		PUSH	ES
		POP	DS
		MOV	DX,2
		MOV	AX,5703h
		CALL	Intr21
  @@Exit:	RET
F87_DelEAs	ENDP
