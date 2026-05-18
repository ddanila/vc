;---------------------------------------------------------------
;		Functions
;
;		Version:	16.07.2000
;---------------------------------------------------------------

		.CODE
AdrNum		PROC	FAR
		CMP	DS:WinTyp,p_File
		JNE	@@Tree
		MOV	SI,DS:EndBuf
		CALL	Addr2Numb
		MOV	DX,SI
		MOV	SI,DS:FileCur
		MOV	DI,DS:FileFirst
		CALL	LnsAX
		JMP	@@ChkEnd
  @@Tree:	MOV	SI,DS:TreeCur
		MOV	DI,DS:TreeFirst
		CALL	SizDX
		MOV	AL,DH
		SUB	AL,4
		CBW
		PUSH	DS
		MOV	DS,ES:TreeSeg
		MOV	DX,DS:TreeLenH
		POP	DS
  @@ChkEnd:	OR	DX,DX
		JE	@@Exit
		DEC	DX
  @@Exit:	MOV	BX,AX
		RET
AdrNum		ENDP

ChgAdr		PROC	FAR
		CMP	DS:WinTyp,p_File
		JNE	@@ChgTree
		XCHG	SI,DS:FileCur
		XCHG	DI,DS:FileFirst
		RET
  @@ChgTree:	XCHG	SI,DS:TreeCur
		XCHG	DI,DS:TreeFirst
		RET
ChgAdr		ENDP

BriefNumFiles	PROC	FAR
		USES	CX,DX
		CALL	NameWidthAL
		CALL	SizDX
		XCHG	AL,DL
		CBW
		DEC	AX
		INC	DX
		MOV	CX,AX
		DIV	DL
		CMP	DS:LongNames,0
		JZ	@@SkipCorr
		PUSH	AX
		MOV	AX,CX
		SUB	AL,3
		ADD	AL,DL
		DIV	DL
		MOV	CL,AL
		POP	AX
		SUB	DL,AH
		CMP	DL,CL
		JAE	@@SkipCorr
		INC	AX
  @@SkipCorr:	CBW
		MUL	BX
		RET
BriefNumFiles	ENDP

F00_50		PROC	NEAR
		CALL	ChgAdr
		CALL	CorPar
		MOV	CX,DI
		MOV	BX,DS:FileFirst
		MOV	DX,DS:FileCur
		CMP	DS:WinTyp,p_File
		JE	F00_51
		MOV	BX,DS:TreeFirst
		MOV	DX,DS:TreeCur
F00_51:		CMP	SI,DX
		JE	F00_52
		MOV	ES:MoveCurFlag,1
F00_52:		SUB	CX,BX
		JE	F00_54
		MOV	BL,7
		CMP	CX,1
		JE	F00_55
		MOV	BL,6
		CMP	CX,-1
		JE	F00_55
		CALL	PutFls
F00_53:		CALL	BegBX
		CALL	SizDX
		JMP	F00_60
F00_54:		CMP	SI,DX
		JE	F00_61
F00_55:		CALL	ChgAdr
		MOV	AL,0
		CALL	SetCur
		CALL	ChgAdr
		MOV	AL,DS:TreeOffset
		CALL	PutFls
		CMP	AL,DS:TreeOffset
		JNE	F00_53
		JCXZ	F00_59
		MOV	AL,BL
		CALL	BegBX
		LEA	CX,[BX+201h]
		CALL	SizDX
		PUSH	DX
		PUSH	AX
		SUB	DH,4
		CMP	DS:WinTyp,p_File
		JE	F00_56
		DEC	CH
		DEC	DH
		JMP	F00_57
F00_56:		CMP	ES:MiniSta,0
		JZ	F00_57
		SUB	DH,2
F00_57:		SUB	DL,3
		ADD	DX,CX
		MOV	AH,Back_C
		CALL	Color
		MOV	BH,AH
		MOV	AH,AL
		MOV	AL,1
		CALL	Int10m
		POP	AX
		MOV	BX,CX
		CMP	AL,7
		JE	F00_58
		MOV	BH,DH
F00_58:		POP	DX
		DEC	DX
		DEC	DX
		MOV	DH,1
		CALL	Window
F00_59:		MOV	AL,1
		CALL	SetCur
		MOV	AL,DS:WinTyp
		OR	AL,ES:MiniSta
		JZ	F00_61
		CALL	BegBX
		CALL	SizDX
		ADD	BH,DH
		SUB	BH,2
		INC	BX
		DEC	DX
		DEC	DX
		MOV	DH,1
F00_60:		CALL	Window
F00_61:		RET
F00_50		ENDP

F00_70:		TEST	BP,2			; Scroll & select
		JZ	F00_73
		CMP	DS:WinTyp,p_File
		JNE	F00_75
		PUSH	SI
		CALL	Numb2Addr
		MOV	BX,SI
		POP	SI
		CMP	BX,DS:EndBuf
		JAE	F00_75
		PUSH	SI
		LEA	SI,[BX].NameF
		CALL	ChkUpDir
		POP	SI
		JE	F00_75
		TEST	ES:MouseWin,2
		JNZ	F00_71
		OR	ES:MouseWin,2
		TEST	DS:[BX].AttrF,40h
		JNZ	F00_71
		OR	ES:MouseWin,4
F00_71:		MOV	AL,DS:[BX].AttrF
		AND	AL,0BFh
		TEST	ES:MouseWin,4
		JZ	F00_72
		OR	AL,40h
F00_72:		CMP	AL,DS:[BX].AttrF
		JE	F00_75
		MOV	DS:[BX].AttrF,AL
		CALL	F08_10
		JMP	F00_75
F00_73:		AND	ES:MouseWin,1

F00_75:		CALL	F00_50
		JMP	Func10

F00_00:		CMP	DS:Visible,0		; Up
		JZ	F25_00
		CMP	DS:WinTyp,p_Tree
		JA	Func10			; Info or QuickView panel
		CALL	AdrNum
		XOR	BP,BP
F00_05:		OR	SI,SI
		JE	F00_70
		DEC	SI
		JMP	F00_70

F01_00:		CMP	DS:Visible,0		; Down
		JZ	F26_00
F01_01:		CMP	DS:WinTyp,p_Tree
		JA	Func10			; Info or QuickView panel
		CALL	AdrNum
		XOR	BP,BP
F01_05:		CMP	SI,DX
		JAE	F00_70
		INC	SI
		JMP	F00_70

F02_00:		MOV	AL,0			; Left
		CMP	AL,DS:Visible
		JZ	F73_00
		CMP	AL,DS:WinTyp
		JNE	F73_00
		CMP	AL,DS:BrfFul
		JNE	F73_00
		CALL	AdrNum
		MOV	AX,SI
		SUB	AX,DI
		CMP	AX,BX
		JB	F02_01
		SUB	SI,BX
		JMP	F00_75
F02_01:		SUB	SI,BX
		JAE	F02_02
		XOR	SI,SI
F02_02:		SUB	DI,BX
		JAE	F00_75
		JMP	F06_01

F03_00:		MOV	AL,0			; Right
		CMP	AL,DS:Visible
		JZ	F73_00
		CMP	AL,DS:WinTyp
		JNE	F73_00
		CMP	AL,DS:BrfFul
		JNZ	F73_00
		CALL	AdrNum
		MOV	AX,SI
		SUB	AX,DI
		CALL	F03_30
		PUSH	AX
		CALL	BriefNumFiles
		SUB	AX,BX
		MOV	BP,AX
		POP	AX
		CMP	AX,BP
		JB	F00_75
		XCHG	SI,DI
		CALL	F03_30
		XCHG	SI,DI
		ADD	BP,BX
		MOV	AX,DX
		SUB	AX,DI
		INC	AX
		CMP	AX,BP
		JAE	F00_75
		OR	DI,DI
		JE	F00_75
		JMP	F07_01

F03_30		PROC	NEAR
		ADD	SI,BX
		CMP	SI,DX
		JBE	@@Exit
		MOV	SI,DX
  @@Exit:	RET
F03_30		ENDP

F04_00:		CMP	DS:Visible,0		; PgUp
		JZ	Func10
		CMP	DS:WinTyp,p_Tree
		JA	Func10			; Info or QuickView panel
		CALL	F04_30
		OR	DI,DI
		JE	F06_01
		SUB	DI,BX
		JAE	F04_01
		XOR	DI,DI
F04_01:		ADD	BX,DI
		CMP	BX,SI
		JAE	F00_75
		MOV	SI,BX
		JMP	F00_75

F04_30		PROC	NEAR
		CALL	AdrNum
		MOV	AL,DS:WinTyp
		OR	AL,DS:BrfFul
		JNZ	@@Exit
		CALL	BriefNumFiles
		MOV	BX,AX
  @@Exit:	DEC	BX
		RET
F04_30		ENDP

F05_00:		CMP	DS:Visible,0		; PgDn
		JZ	Func10
		CMP	DS:WinTyp,p_Tree
		JA	Func10			; Info or QuickView panel
		CALL	F04_30
		MOV	CX,DI
		XCHG	SI,DI
		CALL	F03_30
		XCHG	SI,DI
		MOV	AX,DX
		SUB	AX,BX
		JAE	F05_01
		XOR	AX,AX
F05_01:		CMP	DI,AX
		JBE	F05_02
		MOV	DI,AX
F05_02:		CMP	DI,CX
		JBE	F07_01
		CMP	DI,SI
		JBE	F00_75
		MOV	SI,DI
		JMP	F00_75

F06_00:		CMP	DS:Visible,0		; Home
		JZ	F73_00
		CMP	DS:WinTyp,p_Tree
		JA	F73_00			; Info or QuickView Panel
		CALL	AdrNum
F06_01:		XOR	SI,SI
		XOR	DI,DI
		JMP	F00_75

F07_00:		CMP	DS:Visible,0		; End
		JZ	F73_00
		CMP	DS:WinTyp,p_Tree
		JA	F73_00			; Info or QuickView panel
		CALL	AdrNum
F07_01:		MOV	SI,DX
		MOV	DI,DX
		JMP	F00_75

F08_00:		CMP	DS:Visible,0		; Ins
		JZ	Func10
		CMP	DS:WinTyp,p_File
		JNE	Func10
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	Func10
		PUSH	SI
		CMP	ES:InsDwn,0
		JZ	F08_01
		CALL	NextFilEnt
		CMP	SI,DS:EndBuf
		JAE	F08_01
		PUSH	DS
		XOR	AX,AX
		MOV	DS,AX
		AND	BYTE PTR DS:[418h],7Fh
		POP	DS
F08_01:		POP	SI
		PUSH	SI
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		POP	SI
		JE	F08_02
		XOR	DS:[SI].AttrF,40h
		CMP	ES:InsDirSize,0
		JZ	F08_02
		CMP	DS:ArcPnt,0
		JNE	F08_02
		TEST	DS:[SI].AttrF,40h
		JZ	F08_02
		TEST	DS:[SI].AttrF,10h
		JZ	F08_02
		TEST	DS:[SI].AttrF,8
		JNZ	F08_02
		PUSH	SI
		MOV	ES:FncAddr,OFFSET DirSiz1
		MOV	ES:SumSize.Lo,0
		MOV	ES:SumSize.Hi,0
		MOV	DI,OFFSET LineBuf1
		CALL	LongNameOffset
		CALL	strcpy
		MOV	CL,1
		XCHG	CL,ES:IncDirs
		MOV	AL,10h
		CALL	DirSiz1
		MOV	ES:IncDirs,CL
		POP	SI
		OR	AH,AH
		JNZ	F08_02
		MOV	AX,ES:SumSize.Lo
		MOV	BX,ES:SumSize.Hi
		MOV	DS:[SI].SizeF.Lo,AX
		MOV	DS:[SI].SizeF.Hi,BX
		OR	DS:[SI].AttrF,8
F08_02:		CALL	F08_10
		CMP	ES:InsDwn,0
		JZ	Func10
		JMP	F01_01

F08_10		PROC	NEAR
		CALL	PutFls
		MOV	AL,1
		CALL	SetCur
		CMP	ES:MiniSta,0
		JZ	@@Exit
		CALL	BegBX
		CALL	SizDX
		ADD	BH,DH
		SUB	BH,2
		INC	BX
		DEC	DX
		DEC	DX
		MOV	DH,1
		CALL	Window
  @@Exit:	RET
F08_10		ENDP

		.CONST
Sel_S		DB	'Select',0
Unsl_S		DB	'Unselect',0
Inv_S		DB	'Invert',0
Sel1_S		DB	'^0C the files^1C', 0
Sel2_S		DB	' and directories',0
Sel3_S		DB	'  '
Sel4_S		DB	'              ',0
ErSel_S		DB	'Could not find a match',0

		.DATA
Sel_P		DB	Box_C, BoxCu_C, 0, 0
		DW	IncSp_S
		DW	Sel1_S
		DW	Empty_S
		DW	0
Sel_B		BOXHD	< Sel_P, 500h, 0, Dialog_K, Fls_M, Select_H, 0 >
		BOXMN	< 1 ,0,0,0,0, 305h, SelMas, LenMaskFull+80h+LenMask >
		DB	0
ErSel_P		DB	Err_C, ErInv_C, ErInv_C, 0
		DW	IncSp_S
		DW	ErSel_S
		DW	Quote2_S
		DW	Ok_S
		DW	0
ErSel_B		BOXHD	< ErSel_P, 500h, 0, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
Sel_T		DW	Sel_S , Sel3_S, SelMas
		DW	Sel_S , Sel2_S,  SelMas
Unsl_T		DW	Unsl_S, Sel4_S, SelMas
		DW	Unsl_S, Sel2_S,  SelMas
Inv_T		DW	Inv_S , Sel3_S, SelMas
		DW	Inv_S , Sel2_S,  SelMas

		.CODE
F76_00:		CMP	ES:DosBuf,0		; Grey *
		JNE	F73_00
F76_10:		MOV	CL,0
		JMP	F76_03
F76_01:		MOV	CL,1
		JMP	F76_03
F76_02:		MOV	CL,2
F76_03:		MOV	DX,OFFSET Inv_T
		JMP	F09_10

F09_00:		MOV	CL,0			; Grey +
		JMP	F09_03
F09_01:		MOV	CL,1
		JMP	F09_03
F09_02:		MOV	CL,2
F09_03:		MOV	DX,OFFSET Sel_T
F09_10:		TEST	CL,1
		JNZ	F10_04
		PUSH	AX
		CALL	KeybStat
		TEST	AL,3
		POP	AX
		JZ	F10_04
		OR	CL,1
		JMP	F10_04

F10_00:		MOV	CL,0			; Grey -
		JMP	F10_03
F10_02:		MOV	CL,2
F10_03:		MOV	DX,OFFSET Unsl_T
		OR	CL,1
F10_04:		CMP	DS:Visible,0
		JZ	F73_00
		CMP	DS:WinTyp,p_Tree
		JA	F73_00			; Info or QuickView panel
		JE	F10_20
		TEST	CL,2
		JNZ	F10_06
		PUSH	DX
		TEST	CL,1
		JZ	F10_05
		ADD	DX,3*2
F10_05:		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	BP,OFFSET Sel_B
		CALL	Dialog
		POP	DS
		POP	DX
		JNE	Func04
		MOV	DI,OFFSET SelMas
		CMP	BYTE PTR ES:[DI],0
		JE	Func04
F10_06:		MOV	CH,0
		MOV	SI,DS:DiskBuf
F10_07:		CMP	SI,DS:EndBuf
		JAE	F10_11
		PUSH	SI
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		POP	SI
		JE	F10_10
		TEST	CL,2
		JNZ	F10_08
		PUSH	SI
		CALL	NameOffset1
		CALL	TestMask
		POP	SI
		JNE	F10_10
F10_08:		TEST	CL,1
		JNZ	F10_09
		TEST	DS:[SI].AttrF,10h
		JNZ	F10_10
F10_09:		MOV	CH,1
		XOR	DS:[SI].AttrF,40h
		CMP	DX,OFFSET Inv_T
		JE	F10_10
		OR	DS:[SI].AttrF,40h
		CMP	DX,OFFSET Sel_T
		JE	F10_10
		AND	DS:[SI].AttrF,0BFh
F10_10:		CALL	NextFilEnt
		JMP	F10_07
F10_11:		OR	CH,CH
		JNZ	Func01
		TEST	CL,2
		JNZ	Func04
		PUSH	ES
		POP	DS
		MOV	BP,OFFSET ErSel_B
		CALL	Dialog
		JMP	Func04

F10_20:		TEST	CL,2
		JNZ	F73_00
		CMP	DX,OFFSET Inv_T
		JE	F73_00
		PUSH	DX
		CALL	AdrNum
		POP	AX
		CMP	AX,OFFSET Sel_T
		JNE	F10_21
		CALL	F10_30
		JMP	F00_75
F10_21:		MOV	AL,0
		CALL	F10_35
		JMP	F00_75

F10_30		PROC	FAR
		USES	BX,DS
		MOV	DS,ES:TreeSeg
		MOV	BX,SI
		MOV	CX,SI
		CALL	TreeNumb2AdrH1
		MOV	AL,DS:[SI].DirLevel
F10_31:		CMP	CX,DX
		JAE	F10_32
		INC	CX
		CALL	NextDirEntH
		CMP	AL,DS:[SI].DirLevel
		JNE	F10_31
		MOV	BX,CX
F10_32:		MOV	SI,BX
		RET
F10_30		ENDP

F10_35		PROC	FAR
		USES	BX,DX,DS
		MOV	DS,ES:TreeSeg
		MOV	BX,SI
		MOV	DX,SI
		XOR	CX,CX
		CALL	TreeNumb2AdrH1
		ADD	AL,DS:[SI].DirLevel
		MOV	SI,OFFSET TreeList
F10_36:		CMP	CX,DX
		JAE	F10_38
		CMP	AL,DS:[SI].DirLevel
		JNE	F10_37
		MOV	BX,CX
F10_37:		INC	CX
		CALL	NextDirEntH
		JMP	F10_36
F10_38:		MOV	SI,BX
		RET
F10_35		ENDP

F78_00:		CMP	DS:Visible,0		; ^M - restore marks
		JZ	Func04
		CMP	DS:WinTyp,p_File
		JNE	Func04
		MOV	SI,DS:DiskBuf
F78_01:		CMP	SI,DS:EndBuf
		JAE	Func01
		PUSH	SI
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		POP	SI
		JE	F78_02
		TEST	DS:[SI].AttrF,80h
		JZ	F78_02
		OR	DS:[SI].AttrF,40h
F78_02:		CALL	NextFilEnt
		JMP	F78_01

F11_00:		CMP	DS:Visible,0		; ^O
		PUSH	DS
		MOV	DS,ES:WCBsec
		JZ	F11_01
		MOV	AL,DS:Visible		; Panels Off
		MOV	ES:Ctrl_O,AL
		MOV	DS:Visible,0
		POP	DS
		MOV	DS:Visible,0
		CALL	BegBX
		CALL	SizDX
		MOV	BL,0
		MOV	DL,ES:Columns
		CALL	ResUser
		JMP	Func03
F11_01:		MOV	AL,ES:Ctrl_O		; Panels On
		MOV	DS:Visible,AL
		MOV	AH,DS:ChgFlg
		POP	DS
		MOV	DS:Visible,1
		CMP	DS:ChgFlg,0
		JNZ	F11_02
		CMP	AL,0
		JZ	Panels00
		OR	AH,AH
		JZ	Panels00
F11_02:		MOV	AL,Cmd_M
		CALL	PutMnu
		JMP	Panels00

F12_00:		CMP	DS:Visible,0		; ^P
		PUSH	DS
		MOV	DS,ES:WCBsec
		MOV	CL,0
		JNZ	F14_01
		POP	DS
		JMP	Func05

F13_00:		PUSH	DS			; ^F1
		MOV	DS,ES:WCB1seg
		MOV	CL,1
		JMP	F14_01

F14_00:		PUSH	DS			; ^F2
		MOV	DS,ES:WCB2seg
		MOV	CL,5
F14_01:		JMP	M07_00

F15_00:		CMP	DS:Visible,0		; ^U
		JNZ	F15_01
		JMP	Func04
F15_01:		MOV	AX,ES:WCB1seg
		XCHG	AX,ES:WCB2seg
		MOV	ES:WCB1seg,AX
		MOV	BX,OFFSET Active
		CALL	InvVar
		CALL	BegBX
		CALL	SizDX
		MOV	BL,0
		MOV	DL,ES:Columns
		CALL	ResUser
		JMP	Panels00

F16_00:		PUSH	ES			; ^B
		POP	DS
		MOV	BH,DS:Lines
		DEC	BH
		MOV	BL,0
		CMP	DS:KeyBar,BL
		JZ	F16_02
		MOV	DS:KeyBar,BL
		MOV	DS:BarPrev,0FFh
		MOV	DL,ES:Columns
		MOV	DH,1
		CALL	ResUser
		CALL	Window
F16_01:		JMP	Func04
F16_02:		MOV	DS:KeyBar,1
		CMP	BH,DS:DosLin
		JNE	F16_01
		CALL	BegBX
		CALL	SizDX
		MOV	BL,0
		MOV	DL,ES:Columns
		CALL	ResUser
		CALL	Window
		CALL	Cursor1
		CALL	SavMous
		MOV	AL,0
		CALL	CurMous
		JMP	Init70

F17_00:		PUSH	DS			; ^R
F17_01:		MOV	AL,DS:WinTyp
		CMP	AL,p_Tree
		JB	F17_04			; File panel
		JNE	F17_02			; Info or QuickView panel
		MOV	ES:RescanTree,1
F17_02:		MOV	CH,4
		CMP	AL,2
		JE	F17_03
		MOV	CH,5
		JB	F17_03
		MOV	CH,6
F17_03:		JMP	M04_00
F17_04:		MOV	DS:ChgFlg,1
		MOV	CL,DS:SortTyp
		MOV	CH,DS:BrfFul
		JMP	M01_01

F79_00:		MOV	AL,1			; ^Z - tree
		JMP	F18_01

F83_00:		MOV	AL,3			; ^Q - quick view
		JMP	F18_01

F18_00:		MOV	AL,2			; ^L - info
F18_01:		CMP	DS:Visible,0
		JZ	Func04
		PUSH	DS
		CMP	AL,DS:WinTyp
		JE	F18_02
		CALL	InvWCB
		CMP	DS:Visible,0
		JZ	F17_02
		CMP	AL,DS:WinTyp
		JNE	F17_02
F18_02:		MOV	AL,DS:Ctrl_L
		CMP	AL,0
		JE	F17_04
		JMP	F17_02

F20_00:		PUSH	DS			; Tab
		CALL	InvWCB
		MOV	AX,DS
		CMP	DS:Visible,0
		POP	DS
		JNZ	F20_01
		JMP	Func05
F20_01:		XCHG	AX,ES:WCBcrn
		CALL	PutPath
		CALL	PutFls
		XCHG	AX,ES:WCBcrn
		CALL	F20_10
		CALL	PutPath
		JMP	Func01

F20_10		PROC	NEAR
		XOR	BX,BX
		MOV	DH,ES:Lines
		MOV	DL,ES:Columns
		CMP	DS:WinTyp,p_File
		MOV	DS,ES:WCBsec
		JNE	F20_11
		CMP	DS:WinTyp,p_File
		JNE	F20_11
		CMP	ES:PathEr,0
		JNZ	F20_12
		PUSH	ES
		MOV	ES,ES:WCBcrn
		MOV	SI,OFFSET WinShortPath
		MOV	DI,SI
		CALL	StrLen
		REPE	CMPSB
		POP	ES
		JNE	F20_12
		JMP	F20_16
F20_11:		CMP	ES:PathEr,0
		JZ	F20_16
		CALL	HidCur
		CALL	Window
		JMP	F20_15
F20_12:		CALL	HidCur
		CALL	Window
		MOV	AL,DS:WinShortPath[0]
		CALL	Phantom
		JC	F20_13
		MOV	DX,OFFSET WinShortPath
		MOV	AH,3Bh
		CALL	Intr21
		JNC	F20_14
		CMP	AX,3
		JE	F20_14
F20_13:		MOV	SI,OFFSET WinShortPath
		MOV	DI,OFFSET DosPath
		MOV	CX,3
		REP	MOVSB
		MOV	AL,0
		STOSB
		MOV	ES:PathEr,1
		MOV	DS:ChgFlg,1
		MOV	DL,DS:WinShortPath[0]
		SUB	DL,'A'
		MOV	AH,0Eh
		INT	21h
		JMP	F20_16
F20_14:		MOV	DL,DS:WinShortPath[0]
		SUB	DL,'A'
		MOV	AH,0Eh
		INT	21h
F20_15:		CALL	GetPath
F20_16:		CALL	PutPrompt
		MOV	AX,ES:WCBcrn
		XCHG	AX,ES:WCBsec
		MOV	ES:WCBcrn,AX
		MOV	DS,AX
		MOV	BX,OFFSET Active
		CALL	InvVar
		RET
F20_10		ENDP

		.DATA
RunExt_T	DW	User_S, TmpBuf

		.CODE
F21_00:		CMP	ES:DosBuf,0		; Enter
		JNE	F21_43
		CMP	DS:Visible,0
		JZ	F21_43
F21_10:		CMP	DS:WinTyp,p_Tree
		JNE	F21_20
		CALL	HidCur
		MOV	AL,ES:DosPath[0]
		CALL	Phantom
		JC	F21_13
		XOR	BP,BP
F21_11:		MOV	SI,OFFSET CrnPath
		MOV	DI,OFFSET TmpBuf
		CALL	LongTreePath
		MOV	DX,DI
		CMP	BYTE PTR DS:[SI+3],0
		JE	F21_12
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	AX,4300h
		CALL	Intr21
		POP	DS
		JC	F21_14
		TEST	CL,10h
		JZ	F21_15
F21_12:		OR	BP,BP
		JNE	F24_10
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	AH,3Bh
		CALL	Intr21
		POP	DS
		JNC	F24_10
F21_13:		JMP	Func04
F21_14:		CMP	AX,53h
		JE	F21_13
F21_15:		PUSH	DS
		PUSH	ES
		PUSH	ES:TreeSeg
		PUSH	DS
		POP	ES
		POP	DS
		MOV	DX,OFFSET CrnPath
		CALL	FindTreeEntry
		POP	ES
		JC	F21_16
		PUSH	DS:TreeLenH
		CALL	DelTreeEntry
		CALL	SaveTreeInfo
		MOV	SI,DS:TreeSize
		CALL	TreeAdr2NumbH
		MOV	DS:TreeLenH,SI
		POP	DI
		CLC
F21_16:		PUSHF
		MOV	CX,DS:TreeLenH
		SUB	DI,CX
		POPF
		POP	DS
		JC	F24_10
		MOV	SI,OFFSET CrnPath
		CALL	CutPath
		CMP	SI,OFFSET CrnPath+3
		JBE	F21_17
		DEC	SI
F21_17:		MOV	BYTE PTR DS:[SI],0
		OR	BP,BP
		JNE	F21_18
		INC	BP
		MOV	AX,DS:TreeCur
		CMP	AX,CX
		JB	F21_11
		DEC	CX
		MOV	DS:TreeCur,CX
		JMP	F21_11
F21_18:		SUB	DS:TreeCur,DI
		JMP	F21_11

F21_20:		JA	F21_13			; Files
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	F21_13
		TEST	DS:[SI].AttrF,10h
		JNZ	F23_10
		MOV	DI,SI
		CALL	NameOffset
		XCHG	SI,DI
		CMP	DS:ArcPnt,0
		JE	F21_21
		MOV	SI,DI
		MOV	DL,0
		CALL	ChkArcExt
		JC	F21_13
		JMP	F23_02
F21_21:		MOV	SI,DS:ExtFile		; Test extension
		OR	SI,SI
		JE	F21_23
		PUSH	ES
		PUSH	DS
		POP	ES
		MOV	AL,0
		CALL	FindExt
		POP	ES
		JC	F21_23
		MOV	ES:TmpBuf[0],0
F21_22:		MOV	ES:MenuViewEdit,OFFSET RunExt_T
		JMP	F32_30
F21_23:		MOV	BX,DS
		PUSH	DI
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET VcSet
		MOV	DI,OFFSET TmpBuf
		CALL	strcpy
		POP	DI
		MOV	ES,BX
		MOV	SI,OFFSET MainExt
		MOV	AL,0
		CALL	FindExt
		PUSH	DS
		POP	ES
		JNC	F21_22
		MOV	DS,BX
		MOV	SI,DI			; Check archive
F21_30:		CALL	TestArchive
		MOV	BX,0FFFFh
		JNC	F23_06
		CALL	TstExe
		JNC	F21_13
		PUSH	SI			; Run file from panel
		XOR	DX,DX
		MOV	ES:DosBuf[0],DL
		MOV	BP,0Dh
		MOV	DI,OFFSET FileSymb2_S
		CALL	ChkForSymb
		MOV	AH,0
		JNC	F21_31
		MOV	AH,3
F21_31:		POP	SI
		MOV	BX,OFFSET TmpBuf
		MOV	DI,BX
		TEST	AH,1
		JE	F21_32
		MOV	AL,'"'
		STOSB
F21_32:		CALL	strcpy
		TEST	AH,2
		JE	F21_33
		MOV	AL,'"'
		STOSB
F21_33:		OR	BP,BP
		JNE	F21_34
		MOV	AL,' '
		STOSB
F21_34:		MOV	AL,0
		STOSB
		PUSH	ES
		POP	DS
F21_40:		MOV	SI,OFFSET DosBuf
		MOV	CX,LenCmdLine
F21_41:		MOV	AL,DS:[BX]
		CMP	AL,0
		JE	F21_42
		INC	BX
		MOV	AH,0
		CALL	EdLin
		JC	F21_41
F21_42:		MOV	DS:DosCol,DL
		CALL	DosCom
		CALL	Cursor1
		MOV	DS,DS:WCBcrn
		MOV	AX,BP
		CMP	AH,0FFh
		JE	Func01
		CMP	AL,0Dh
		JNE	Func04
		OR	DL,DL
		JE	Func04
F21_43:		PUSH	ES			; Run command from command line
		POP	DS
		MOV	SI,OFFSET DosBuf
		MOV	DI,DS:HistPoint
		MOV	CX,DS:LenHistory
		INC	DI
		DEC	CX
		CMP	BYTE PTR DS:[SI],0
		JE	F21_44
		CALL	SaveHistory
F21_44:		JMP	RunCmd00

F22_00:		CMP	DS:Visible,0		; ^PgUp
		JZ	F22_01
		CMP	DS:WinTyp,p_Tree
		JA	F22_01			; Info or QuickView panel
		JE	F22_10
		MOV	DI,OFFSET TmpBuf	; File panel
		PUSH	DI
		MOV	AX,'.'
		STOSB
		STOSW
		POP	DI
		MOV	CX,DS:EndFul
		XCHG	CX,DS:EndBuf
		XOR	AX,AX
		CALL	FindFile
		MOV	DS:EndBuf,CX
		JNC	F23_01
F22_01:		JMP	Func05
F22_10:		CALL	AdrNum
		MOV	AL,0FFh
		CALL	F10_35
		JMP	F00_75

		.CONST
ChDir_S		DB	' Change directory ',0

		.DATA
ChDir_T		DW	ChDir_S, LineBuf1, ExecArcProgram

		.CODE
F23_00:		CMP	DS:Visible,0		; ^PgDn
		JZ	F22_01
		CMP	DS:WinTyp,p_Tree
		JA	F22_01			; Info or QuickView panel
		JE	F21_10
		MOV	SI,DS:FileCur		; File panel
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	F22_01
F23_01:		TEST	DS:[SI].AttrF,10h
		JNZ	F23_10
		CALL	NameOffset
F23_02:		MOV	BX,0FFFFh
		MOV	DI,OFFSET LineBuf1
		CMP	DS:ArcPnt,0		; Extracting file
		JE	F23_04
		MOV	BP,SI
		PUSH	DI
		CALL	PutDescriptor
		CALL	AddFile
		CALL	strcpy
		POP	SI
		MOV	DX,OFFSET ChDir_T
		CALL	ViewArcExtract
		JNC	F23_03
		OR	AX,AX
		JE	Func04
		MOV	BP,AX
		CALL	Dialog
		JMP	Func04
F23_03:		CMP	AX,0FFFFh
		JE	Func04
		MOV	BX,AX
		MOV	DI,SI
		MOV	SI,OFFSET VcTempArc_S
		CALL	MakePathName
		CALL	AddFile
		MOV	SI,BP
F23_04:		CALL	strcpy
		PUSH	DS			; Checking archive
		PUSH	BX
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET LineBuf1
		CALL	TestArchive
		POP	BX
		POP	DS
		JNC	F23_06
		MOV	BP,ES
		ADD	BP,ES:SizeDataSeg
		SUB	BP,ES:WorkBuffer
		CMP	BP,0FFFh
		JBE	F23_05
		MOV	BP,0FFFh
F23_05:		MOV	CL,4
		SHL	BP,CL
		XOR	DI,DI
		PUSH	DS
		PUSH	BX
		PUSH	ES
		POP	DS
		CALL	DetectArc
		POP	BX
		POP	DS
		JNC	F23_06
		PUSH	ES			; Deleting file - not archive
		POP	DS
		MOV	AX,BX
		CALL	DelTempArcFile
		JMP	Func04
F23_06:		MOV	DI,OFFSET LineBuf1	; Enter to archive
		PUSH	DI
		CMP	DS:ArcPnt,0
		JE	F23_07
		MOV	SI,OFFSET ArcPath
		CMP	BYTE PTR DS:[SI],0
		JE	F23_07
		CALL	strcpy
		CALL	AddFile
F23_07:		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CALL	NameOffset
		CALL	strcpy
		POP	SI
		MOV	DI,DS:DiskBuf
		MOV	DS:ArcPnt,DI
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	AX,BX
		STOSW
		CALL	strcpy
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		INC	DI
		MOV	DS:DiskBuf,DI
		MOV	DS:ArcPath[0],0
F23_08:		MOV	DS:FileCur,0
		MOV	AX,DS:DiskBuf
		MOV	DS:EndBuf,AX
		JMP	F24_10
F23_10:		CMP	DS:ArcPnt,0		; Directory
		JE	F23_30
		CALL	NameOffset
		CALL	ChkUpDir
		JNE	F23_15
		MOV	SI,OFFSET ArcPath
		CMP	BYTE PTR DS:[SI],0
		JNE	F23_32
		MOV	SI,DS:ArcPnt
		CMP	SI,OFFSET ArcFiles
		JA	F23_11
		LODSW				; Exit from archive to dir
		XOR	BX,BX
		CMP	DS:WinShortPath[3],0
		JE	F23_14
		CALL	HidCur
		MOV	AL,DS:WinShortPath[0]
		CALL	Phantom
		JC	F22_01
		MOV	DX,OFFSET WinShortPath
		MOV	AX,4300h
		CALL	Intr21
		JC	F22_01
		TEST	CL,10h
		JZ	F22_01
		JMP	F23_14
F23_11:		MOV	DX,SI			; Exit from archive to archive
		LODSW
		CALL	DelTempArcFile
		MOV	BX,SI
		CALL	CutPath
		PUSH	ES
		PUSH	DS
		POP	ES
		MOV	DI,OFFSET ArcPath
		MOV	CX,SI
		SUB	CX,BX
		JBE	F23_12
		MOV	SI,BX
		REP	MOVSB
		DEC	DI
F23_12:		MOV	AL,0
		STOSB
		MOV	DI,OFFSET ArcFiles
F23_13:		MOV	BX,DI
		SCASW
		MOV	CH,0FFh
		REPNE	SCASB
		CMP	DI,DX
		JB	F23_13
		POP	ES
F23_14:		XCHG	BX,DS:ArcPnt
		MOV	DS:DiskBuf,BX
		PUSH	ES
		PUSH	DS
		POP	ES
		MOV	DI,SI
		CALL	StrLen
		LEA	DI,[SI].NameF
		PUSH	DI
		ADD	SI,CX
		ADD	DI,CX
		DEC	SI
		DEC	DI
		STD
		REP	MOVSB
		CLD
		POP	SI
		POP	ES
		MOV	AH,0
		JMP	F23_33
F23_15:		PUSH	ES			; Change Dir in archive
		PUSH	DS
		POP	ES
		MOV	DI,OFFSET ArcPath
		MOV	AL,0
		CMP	AL,DS:[DI]
		JE	F23_16
		MOV	CH,1
		REPNE	SCASB
		DEC	DI
		CMP	DI,OFFSET ArcPath+LenPath-3
		JA	F23_20
		MOV	BX,DI
		MOV	AL,'\'
		STOSB
F23_16:		LODSB
		STOSB
		CMP	AL,0
		JE	F23_20
		CMP	DI,OFFSET ArcPath+LenPath-1
		JBE	F23_16
		MOV	BYTE PTR DS:[BX],0
F23_20:		POP	ES
		JNE	F22_01
		JMP	F23_08
F23_30:		CALL	HidCur			; Change Directory
		MOV	AL,DS:WinShortPath[0]
		CALL	Phantom
		JC	F23_36
		CALL	LongNameOffset
		MOV	DX,SI
		MOV	AH,3Bh
		CALL	Intr21
		JC	F23_36
		CALL	ChkUpDir
		JNE	F24_10
F23_31:		MOV	SI,OFFSET WinShortPath[3]
F23_32:		MOV	AH,10h
F23_33:		MOV	DS:FileCur,0
		MOV	DS:FileFirst,1
		MOV	BX,DS:DiskBuf
		LEA	DI,[BX].NameF
		MOV	DX,SI
		CALL	CutPath
		PUSH	ES
		PUSH	DS
		POP	ES
		PUSH	SI
		CALL	strcpy
		INC	DI
		MOV	DS:EndBuf,DI
		SUB	DI,BX
		SUB	DI,OFFSET NameF
		MOV	DS:[BX].LenShortNamF,DI
		MOV	DS:[BX].LenLongNamF,0
		MOV	DS:[BX].AttrF,AH
		POP	DI
		CMP	DX,OFFSET ArcFiles
		JAE	F23_35
		CMP	DI,DX
		JBE	F23_34
		DEC	DI
F23_34:		MOV	AL,0
		STOSB
F23_35:		POP	ES
		CALL	GetPath
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	SI,OFFSET DosShortPath
		MOV	DI,OFFSET WinShortPath
		CALL	strcpy
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		JMP	F24_11
F23_36:		JMP	Func04

		.CONST
CDir_S		DB	'\',0

		.CODE
F24_00:		CALL	HidCur			; ^\
		MOV	AL,ES:DosPath[0]
		CALL	Phantom
		JC	F23_36
		MOV	DX,OFFSET CDir_S
		PUSH	DS
		PUSH	CS:SubsSeg
		MOV	BP,1
F24_01:		POP	DS
		MOV	AH,3Bh
		CALL	Intr21
		POP	DS
		JC	F23_36
		CALL	DelAllTempArcs
		MOV	AX,OFFSET ArcFiles
		MOV	DS:DiskBuf,AX
		MOV	DS:EndBuf,AX
		MOV	DS:ArcPnt,0
		CMP	DS:WinTyp,p_Tree
		JNE	F24_02
		CMP	ES:TreeSeg,0
		JE	F24_02
		MOV	DS:TreeCur,0
F24_02:		OR	BP,BP
		JE	F24_10
		MOV	SI,OFFSET WinShortPath[3]
		CMP	BYTE PTR DS:[SI],0
		JE	F24_10
F24_03:		LODSB
		CMP	AL,0
		JE	F23_31
		CMP	AL,'\'
		JNE	F24_03
		MOV	BYTE PTR DS:[SI-1],0
		JMP	F23_31
F24_10:		CALL	GetPath
F24_11:		MOV	AH,0
		MOV	AL,DS:WinTyp
		CMP	AL,p_Tree
		JE	F24_12
		MOV	DS:ChgFlg,1
		JMP	F24_13
F24_12:		CMP	DS:ChgFlg,0
		JNE	F24_13
		MOV	AH,1
F24_13:		CALL	InvWCB
		OR	AL,DS:WinTyp
		JZ	F24_16
		CMP	DS:WinTyp,p_Tree
		JE	F24_14
		MOV	DS:ChgFlg,1
		JMP	F24_15
F24_14:		CMP	DS:ChgFlg,0
		JNE	F24_15
		MOV	AH,1
F24_15:		CMP	AH,0
		JE	F24_16
		CALL	UpdateBranch
		CALL	SetTreeCursor
F24_16:		JMP	Panels00

F25_00:		MOV	DI,ES:HistPoint		; ^E
		ADD	DI,ES:HstAdr
		MOV	DX,DI
		MOV	CH,1
		MOV	AL,0
		REPNE	SCASB
		CMP	AL,ES:[DI]
		JNE	F26_02
		MOV	DI,DX
		JMP	F26_02

F26_00:		MOV	DI,ES:HistPoint		; ^X
		ADD	DI,ES:HstAdr
		DEC	DI
		CMP	DI,ES:HistPoint
		JB	F26_01
		JE	F26_02
		DEC	DI
		MOV	CH,1
		MOV	AL,0
		STD
		REPNE	SCASB
		CLD
		INC	DI
F26_01:		INC	DI
F26_02:		MOV	BX,DI
		JMP	F27_04

F27_00:		MOV	DL,ES:DosCol		; ^Enter
		MOV	DH,0
		XOR	BP,BP
		MOV	SI,DX
		ADD	SI,OFFSET DosBuf
		CMP	ES:DosBuf,0
		JE	F27_06
		MOV	DI,ES:HistPoint
		ADD	DI,ES:HstAdr
		OR	DL,DL
		JE	F27_03
		MOV	AL,ES:[SI-1]
		CALL	CmpSym
		JE	F27_06
		CMP	AL,'@'
		JE	F27_06
		JMP	F27_03
F27_01:		CMP	AL,ES:[DI]
		JE	F27_05
		MOV	BX,DI
		MOV	SI,OFFSET DosBuf
F27_02:		LODS	BYTE PTR ES:[SI]
		CMP	AL,0
		JE	F27_04
		CALL	UpCase
		MOV	AH,AL
		MOV	AL,ES:[DI]
		INC	DI
		CALL	UpCase
		CMP	AL,AH
		JE	F27_02
F27_03:		MOV	CH,1
		MOV	AL,0
		REPNE	SCASB
		JMP	F27_01
F27_04:		PUSH	ES
		POP	DS
		MOV	AX,BX
		SUB	AX,DS:HistPoint
		MOV	DS:HstAdr,AX
		XOR	DX,DX
		MOV	DS:DosBuf[0],DL
		JMP	F21_40
F27_05:		OR	DL,DL
		JNE	F27_12
F27_06:		CMP	DS:Visible,0
		JZ	F27_12
		CMP	DS:WinTyp,p_File
		JNE	F27_12
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	F27_12
		LEA	DI,[SI].NameF
		CALL	NameOffset
		CALL	KeybStat
		TEST	AL,3
		JZ	F27_07
		MOV	SI,DI
F27_07:		PUSH	SI
		MOV	DI,OFFSET FileSymb2_S
		CALL	ChkForSymb
		MOV	AH,0
		JNC	F27_08
		MOV	AH,3
F27_08:		MOV	CX,LenCmdLine
		MOV	SI,OFFSET DosBuf
		MOV	DI,DX
		ADD	DI,SI
		CMP	DL,1
		JB	F21_31
		JE	F27_09
		CMP	WORD PTR ES:[DI-2],'"\'
		JNE	F27_09
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	AX,0E08h
		CALL	EdLin
		POP	DS
		MOV	AH,2
		JMP	F21_31
F27_09:		OR	AH,AH
		JE	F21_31
		CMP	BYTE PTR ES:[DI-1],'\'
		JNE	F21_31
		PUSH	DX
F27_10:		DEC	DI
		DEC	DX
		JE	F27_11
		CMP	BYTE PTR ES:[DI-1],' '
		JNE	F27_10
F27_11:		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	AX,'"'
		CALL	EdLin
		POP	DS
		POP	DX
		MOV	AH,2
		CMP	DX,CX
		JAE	F21_31
		INC	DX
		JMP	F21_31
F27_12:		JMP	Func05

F80_00:		CMP	DS:Visible,0		; ^I - insert files
		JZ	F27_12
		CMP	DS:WinTyp,p_File
		JNE	F27_12
		CALL	KeybStat
		AND	AL,3
		CALL	TotSel
		OR	BX,DX
		OR	BH,BL
		MOV	AH,BH
		MOV	BP,AX
		JNE	F80_01
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	F27_12
		MOV	DX,SI
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		JE	F27_12
F80_01:		MOV	SI,DS:DiskBuf
		PUSH	SI
F80_02:		CMP	SI,DS:EndBuf
		JAE	F80_03
		AND	DS:[SI].AttrF,7Fh
		CALL	NextFilEnt
		JMP	F80_02
F80_03:		POP	SI
		MOV	DI,OFFSET DosBuf
		CALL	StrLen
		MOV	BX,CX
		DEC	BX
		MOV	DI,OFFSET TmpBuf
		TEST	BP,0FF00h
		JNE	F80_04
		MOV	SI,DX
		JMP	F80_05
F80_04:		CMP	SI,DS:EndBuf
		JAE	F80_14
		TEST	DS:[SI].AttrF,40h
		JZ	F80_13
F80_05:		PUSH	DI
		PUSH	SI
		MOV	DL,DS:[SI].AttrF
		LEA	DI,[SI].NameF
		MOV	CX,DS:[SI].LenShortNamF
		CMP	DS:LongNames,0
		JE	F80_06
		TEST	BP,3
		JNE	F80_06
		CMP	DS:[SI].LenLongNamF,0
		JE	F80_06
		CMP	ES:OS2box,0
		JNE	F80_06
		ADD	DI,CX
		MOV	CX,DS:[SI].LenLongNamF
F80_06:		MOV	SI,DI
		TEST	DL,10h
		JZ	F80_07
		ADD	CX,4
F80_07:		MOV	DI,OFFSET FileSymb2_S
		CALL	ChkForSymb
		JNC	F80_08
		ADD	CX,2
		OR	BP,80h
F80_08:		ADD	BX,CX
		MOV	CX,SI
		POP	SI
		POP	DI
		CMP	BX,LenCmdLine+1
		JA	F80_14
		PUSH	SI
		MOV	SI,CX
		TEST	BP,80h
		JE	F80_09
		MOV	AL,'"'
		STOSB
F80_09:		CALL	strcpy
		TEST	DL,10h
		JZ	F80_10
		PUSH	DS
		MOV	DS,CS:SubsSeg
		MOV	SI,OFFSET Mask_S
		CALL	strcpy
		POP	DS
F80_10:		TEST	BP,80h
		JE	F80_11
		MOV	AL,'"'
		STOSB
F80_11:		CMP	BX,LenCmdLine
		JA	F80_12
		MOV	AL,' '
		STOSB
F80_12:		POP	SI
		AND	BP,0FF03h
		TEST	BP,0FF00h
		JE	F80_14
		AND	DS:[SI].AttrF,0BFh
		OR	DS:[SI].AttrF, 80h
F80_13:		CALL	NextFilEnt
		JMP	F80_04
F80_14:		MOV	AL,0
		STOSB
		PUSH	ES
		POP	DS
		MOV	BP,0FF00h
		MOV	BX,OFFSET TmpBuf
		MOV	DL,DS:DosCol
		MOV	DH,0
		JMP	F21_40

		.CONST
Name_S		DB	PrgName,0
Quit1_S		DB	' The ^0C ',0
Quit2_S		DB	'Do you want to quit the ^0C?',0
YesNo_S		DB	'Yes',0FFh,3,'No',0

		.DATA
YesNo_D		DB	5, 0
		DB	4, 6
		DB	0
Quit_P		DB	Box_C, BoxCu_C, BoxCu_C, 0
		DW	Quit1_S
		DW	Quit2_S
		DW	YesNo_S
		DW	0
Quit_B		BOXHD	< Quit_P, 500h, 0, Dialog_K, Fls_M, Quit_H, 0 >
		BOXMN	< 4, 0,0,0,0, 3FBh, YesNo_D, 0 >
		DB	0
Name_T		DW	Name_S

		.CODE
F30_00:		CMP	ES:QuitAck,0		; F10 - quit
		JZ	F30_01
		MOV	DX,1
		MOV	AX,168Fh
		INT	2Fh
		MOV	DX,OFFSET Name_T
		MOV	BP,OFFSET Quit_B
		CALL	Dialog
		JE	F30_01
		INT	2Fh
		XOR	DX,DX
		MOV	AX,168Fh
		INT	2Fh
		MOV	DX,300h
		MOV	AX,168Fh
		INT	2Fh
		JMP	Func04
F30_01:		CALL	DelAllTempArcs
		JC	F30_02
		CALL	InvWCB
		CALL	DelAllTempArcs
F30_02:		PUSH	ES
		POP	DS
		MOV	SI,OFFSET DirInfoFile
		MOV	AX,DS:DirInfoNumb
		CALL	DelTempArcFile
		CALL	DelTempQuickVw
		JMP	ExitVC

DriveType	EQU	WORD PTR TmpBuf+26
DriveOffset	EQU	WORD PTR TmpBuf+28
DriveWin	EQU	WORD PTR TmpBuf+30
DriveNum	EQU	WORD PTR TmpBuf+32
DrivePoint	EQU	WORD PTR TmpBuf+34
DriveList	EQU	BYTE PTR TmpBuf+36

		.CONST
Drive1_S	DB	' Drive letter ',0
Drive2_S	DB	'Choose ^b^0C^b drive:',0
Drive3_S	DB	'^1E',0
Left_S		DB	'left',0
Right_S		DB	'right',0

		.DATA
Drive_P		DB	Box_C, BoxCu_C, BoxBr_C, 0
		DW	Drive1_S
		DW	Drive2_S
		DW	Drive3_S
		DW	0
Drive_B		BOXHD	< Drive_P, 501h, DriveSub, Dialog_K, Def_M, Drive_H, 0 >
		DB	0
Left_T		DW	Left_S , DriveList
Right_T		DW	Right_S, DriveList
Drive_T		DW	0FFFFh, DriveInit
		DW	 0E08h, DriveLeft
		DW	 4B00h, DriveLeft
		DW	 0020h, DriveRight
		DW	 4D00h, DriveRight
		DW	 4700h, DriveHome
		DW	 4F00h, DriveEnd
		DW	 0

		.CODE
F61_00:		PUSH	DS			; Alt F1
		MOV	DS,ES:WCB1seg
		MOV	CL,Left_M
		JMP	F62_01

F62_00:		PUSH	DS			; Alt F2
		MOV	DS,ES:WCB2seg
		MOV	CL,Right_M
F62_01:		PUSH	CX
		CALL	HidCur
		MOV	AL,CL
		CALL	PutMnu
		MOV	DI,OFFSET TmpBuf
		MOV	BL,'A'
		MOV	SI,OFFSET Left_T
		CMP	CL,Left_M
		JE	F62_02
		MOV	SI,OFFSET Right_T
F62_02:		MOV	AL,BL
		CALL	TstDrv
		JC	F62_05
		CMP	BL,'B'
		JA	F62_04
		CMP	ES:DriveB,0
		JNZ	F62_04
		CMP	AL,Logical
		JE	F62_05
F62_04:		MOV	AL,BL
		STOSB
F62_05:		INC	BX
		CMP	BL,'Z'
		JBE	F62_02
		SUB	DI,OFFSET TmpBuf
		MOV	ES:DriveNum,DI
		CALL	SizDX
		MOV	DH,0
		MOV	CX,4
F62_06:		MOV	AX,DI
		MUL	CL
		MOV	BX,CX
		DEC	BX
		AND	BX,1
		SUB	AX,BX
		MOV	ES:DriveOffset,AX
		ADD	AX,12
		CMP	AX,DX
		JB	F62_07
		LOOP	F62_06
		INC	CX
F62_07:		DEC	CX
		MOV	ES:DriveType,CX
		PUSH	DI
		PUSH	SI
		MOV	BX,DI
		MOV	SI,OFFSET TmpBuf
		MOV	DI,OFFSET DriveList
		PUSH	SI
		XOR	DX,DX
		CMP	CL,2
		JB	F62_09
		MOV	DL,' '
		MOV	AX,DX
		STOSB
		JMP	F62_09
F62_08:		JCXZ	F62_09
		PUSH	CX
		MOV	AL,' '
		REP	STOSB
		POP	CX
F62_09:		MOVS	BYTE PTR ES:[SI],ES:[DI]
		DEC	BX
		JNE	F62_08
		MOV	AX,DX
		STOSW
		POP	DI
		POP	DX
		POP	CX
		MOV	AL,DS:WinShortPath[0]
		REPNE	SCASB
		JE	F62_10
		MOV	DI,OFFSET TmpBuf+1
F62_10:		SUB	DI,OFFSET TmpBuf+1
		MOV	ES:DrivePoint,DI
		MOV	BP,OFFSET Drive_B
		CALL	Dialog
		POP	CX
		MOV	AL,0
		JNZ	F62_20
		MOV	SI,ES:DrivePoint
		MOV	AL,ES:TmpBuf[SI]
F62_20:		CMP	AL,0
		JE	F62_25
		SUB	AL,'A'-1
		MOV	DL,AL
		MOV	BL,AL
		MOV	AX,440Fh
		INT	21h
		MOV	SI,OFFSET WinShortPath[3]
		MOV	ES:BlocEr,1
		MOV	AH,47h
		CALL	Intr21
		MOV	ES:BlocEr,0
		JNC	F62_21
		MOV	AL,DL
		ADD	AL,'A'-1
		CALL	ErrorDrive
		JMP	F62_20
F62_21:		DEC	DX
		MOV	AL,DL
		ADD	AL,'A'
		CMP	AL,DS:WinShortPath[0]
		JE	F62_22
		MOV	DS:WinShortPath[0],AL
		CALL	DelAllTempArcs
		MOV	DS:DiskBuf,OFFSET ArcFiles
		MOV	DS:ArcPnt,0
F62_22:		PUSH	DS
		MOV	AX,DS
		CMP	AX,ES:WCBcrn
		JE	F62_23
		CALL	InvWCB
		CMP	DS:Visible,0
		JZ	F62_23
		CMP	DS:WinTyp,p_File
		JE	F62_24
F62_23:		MOV	AH,0Eh
		INT	21h
F62_24:		POP	DS
		MOV	DS:WinTyp,p_File
		MOV	DS:ChgFlg,1
		MOV	ES:BlocEr,1
		MOV	AX,DS:DiskBuf
		MOV	DS:EndBuf,AX
		MOV	DS:FileCur,0
		JMP	M07_10
F62_25:		POP	DS
		JMP	Func04

		.CODE	Subs
DriveSub	PROC	NEAR
		CMP	AH,0FCh
		JE	@@Mouse
		MOV	CX,ES:DrivePoint
		MOV	SI,OFFSET Drive_T
		CALL	Case
		CMP	AL,' '
		JBE	@@Exit
		CMP	AL,7Fh
		JAE	@@Exit
		CALL	UpCase
		MOV	DI,OFFSET TmpBuf
		MOV	CX,ES:DriveNum
		REPNE	SCASB
		JNE	@@Exit
		SUB	DI,OFFSET TmpBuf+1
		MOV	ES:DrivePoint,DI
  @@Enter:	MOV	AX,1C0Dh
		JMP	@@Exit
  @@Mouse:	CMP	AL,1
		JNE	@@Exit
		ADD	BX,304h
		ADD	BX,ES:DriveOffset
		MOV	DX,ES:DriveWin
		MOV	CX,0FFh
  @@GetMouse:	MOV	SI,CX
		CALL	GetMous
		JZ	@@ExitMouse
		MOV	AH,0FCh
		CMP	AL,1
		JNE	@@Exit
		CALL	InWind
		JNC	@@ChkMouse
		CMP	SI,0FFh
		JE	@@Exit
		MOV	CX,SI
		MOV	AH,0
		JMP	@@CurMouse
  @@ChkMouse:	SUB	CX,BX
		MOV	AX,CX
		MOV	CX,ES:DriveType
		INC	CX
		DIV	CL
		MOV	CH,1
		CMP	CL,1
		JE	@@LocMouse
		MOV	CH,3
  @@LocMouse:	CMP	AH,CH
		MOV	AH,0
		MOV	CX,SI
		JAE	@@CurMouse
		CMP	CL,0FFh
		JNE	@@CmpMouse
		MOV	ES:DrivePoint,AX
		CALL	DriveCursor
		MOV	CL,AL
  @@CmpMouse:	CMP	AL,CL
		JNE	@@CurMouse
		MOV	AH,ES:Pointers[4]
  @@CurMouse:	CMP	AH,CH
		JE	@@LeaveTime
		MOV	CH,AH
		MOV	AL,AH
		CALL	TypMous
		JMP	@@GetMouse
  @@LeaveTime:	CALL	LeaveTm
		JMP	@@GetMouse
  @@ExitMouse:	MOV	AX,SI
		OR	AH,AH
		JE	@@Done
		MOV	AH,0
		MOV	ES:DrivePoint,AX
		MOV	AL,0
		CALL	TypMous
		JMP	@@Enter
DriveInit:	MOV	CX,ES:DriveOffset
		MOV	AL,DL
		MOV	AH,0
		SUB	AX,11
		SHR	AX,1
		PUSH	CX
		INC	CX
		SHR	CX,1
		SUB	AX,CX
		POP	CX
		CMP	ES:DriveType,1
		JNE	@@SetOffset
		INC	CX
		INC	CX
		DEC	AX
  @@SetOffset:	INC	AX
		MOV	CH,1
		MOV	ES:DriveOffset,AX
		MOV	ES:DriveWin,CX
		JMP	@@WriteCur
DriveLeft:	OR	CX,CX			; Left
		JNE	@@DecDrv
DriveEnd:	MOV	CX,ES:DriveNum		; End
  @@DecDrv:	DEC	CX
		JMP	@@ChgCur
DriveRight:	INC	CX			; Right
		CMP	CX,ES:DriveNum
		JB	@@ChgCur
DriveHome:	XOR	CX,CX			; Home
  @@ChgCur:	CMP	CX,ES:DrivePoint
		JE	@@Done
		MOV	ES:DrivePoint,CX
  @@WriteCur:	ADD	BX,304h
		ADD	BX,ES:DriveOffset
		MOV	DX,ES:DriveWin
		CALL	DriveCursor
  @@Done:	XOR	AX,AX
  @@Exit:	RET
DriveSub	ENDP

DriveCursor	PROC	NEAR
		USES	AX,CX,ES
		MOV	CL,DL
		MOV	CH,0
		MOV	AH,CS:Drive_P[0]
		CALL	Color
		MOV	AL,AH
		PUSH	ES
		CALL	BasAdr
		CALL	MvsColr
		POP	ES
		PUSH	BX
		MOV	AX,ES:DriveType
		INC	AX
		MUL	BYTE PTR ES:DrivePoint
		ADD	BL,AL
		MOV	CL,1
		CMP	ES:DriveType,0
		JE	@@SetCur
		MOV	CL,3
  @@SetCur:	MOV	AH,CS:Drive_P[1]
		CALL	Color
		MOV	AL,AH
		CALL	BasAdr
		CALL	MvsColr
		POP	BX
		CALL	Window
		RET
DriveCursor	ENDP

		.CONST
History_S	DB	' History ',0

		.DATA
History_P	DB	Hist_C, HstCu_C, 0, 0
		DW	History_S
		DW	0FFFEh
		DW	0
History_B	BOXHD	< History_P, 0FF00h, DialogList, Dialog_K, Cmd_M, History_H, 0 >
		DB	0

		.CODE
F68_00:		PUSH	ES			; Alt F8 - History
		POP	DS
		XOR	AX,AX
		XOR	BX,BX
		MOV	DX,32
		MOV	DI,DS:HistPoint
F68_01:		INC	BX
		MOV	SI,DI
		MOV	CH,1
		REPNE	SCASB
		SUB	SI,DI
		NOT	SI
		CMP	SI,DX
		JBE	F68_02
		MOV	DX,SI
F68_02:		CMP	AL,DS:[DI]
		JNE	F68_01
		MOV	AL,DS:Columns
		SUB	AL,3+2+10
		CMP	AX,DX
		JBE	F68_03
		MOV	AX,DX
F68_03:		MOV	DS:ListWindow.X,AL
		MOV	AL,DS:Lines
		SUB	AL,4+5
		CMP	AX,BX
		JBE	F68_04
		MOV	AX,BX
F68_04:		MOV	DS:ListWindow.Y,AL
		MOV	DS:ListTotal,BX
		DEC	BX
		MOV	DS:ListCursor,BX
		MOV	DS:ListFirst,0
		MOV	DS:ListBase,205h
		MOV	DS:ListLine,OFFSET HistoryLine
		MOV	DS:ListHotKey,OFFSET HistoryHotKey
		MOV	BP,OFFSET History_B
		CALL	Dialog
		JNZ	Func04
		MOV	BP,DS:ListKey
		XOR	AX,AX
		MOV	DI,DS:HistPoint
		MOV	CX,DS:ListTotal
		SUB	CX,DS:ListCursor
		DEC	CX
		JCXZ	F68_06
F68_05:		PUSH	CX
		MOV	CH,1
		REPNE	SCASB
		POP	CX
		LOOP	F68_05
F68_06:		MOV	DS:DosBuf[0],AL
		XOR	DX,DX
		MOV	BX,DI
		JMP	F21_40

		.CODE	Subs
HistoryLine	PROC	NEAR			; Line of history
		USES	AX,CX,SI,DI,ES
		MOV	CX,DS:ListTotal
		SUB	CX,SI
		DEC	CX
		MOV	AX,SI
		SUB	AX,DI
		ADD	BX,DS:ListBase
		ADD	BH,AL
		DEC	BX
		MOV	DI,ES:HistPoint
		MOV	AL,0
		JCXZ	@@PutLine
  @@FindLine:	PUSH	CX
		MOV	CH,1
		REPNE	SCASB
		POP	CX
		LOOP	@@FindLine
  @@PutLine:	MOV	SI,DI
		MOV	AH,CS:[BP]
		CALL	Color
		MOV	CL,DS:ListWindow.X
		MOV	DL,CL
		MOV	DH,1
		INC	DX
		INC	DX
		CALL	BasAdr
		MOV	AL,' '
		STOSW
  @@GetChar:	LODSB
		CMP	AL,0
		JNE	@@PutChar
		DEC	SI
		MOV	AL,' '
  @@PutChar:	STOSW
		LOOP	@@GetChar
		MOV	AL,' '
		STOSW
		CLC
		RET
HistoryLine	ENDP

HistoryHotKey	PROC	NEAR			; Hot keys
		USES	AX
		CALL	UpCase
		MOV	AH,AL
		MOV	SI,DS:ListCursor
		INC	SI
		CMP	SI,DS:ListTotal
		JB	@@SetCur
  @@Circle:	XOR	SI,SI
  @@SetCur:	MOV	DI,DS:HistPoint
		MOV	CX,DS:ListTotal
		SUB	CX,SI
		DEC	CX
		JCXZ	@@Next
		MOV	AL,0
  @@FindCur:	PUSH	CX
		MOV	CH,1
		REPNE	SCASB
		POP	CX
		LOOP	@@FindCur
  @@FindHotKey:	MOV	AL,DS:[DI]
		CALL	UpCase
		CMP	AL,AH
		JE	@@Found
  @@Next:	CMP	SI,DS:ListCursor
		JE	@@NotFound
		INC	SI
		DEC	DI
		CMP	DI,DS:HistPoint
		JB	@@Circle
		JE	@@Next
		DEC	DI
		STD
		MOV	CH,1
		MOV	AL,0
		REPNE	SCASB
		CLD
		INC	DI
		INC	DI
		JMP	@@FindHotKey
  @@NotFound:	STC
  @@Found:	RET
HistoryHotKey	ENDP

		.CODE
F81_00:		MOV	DS,ES:WCB1seg		; ^[ - insert left path
		JMP	F82_01

F82_00:		MOV	DS,ES:WCB2seg		; ^] - insert right path
F82_01:		CMP	DS:Visible,0
		JZ	Func04
		CALL	KeybStat
		CMP	DS:WinTyp,p_Tree
		JA	Func04			; Info or QuickView panel
		MOV	DI,OFFSET LineBuf1
		JB	F82_02
		MOV	SI,OFFSET CrnPath
		TEST	AL,3
		JNZ	F82_03
		CALL	TreePath
		JMP	F82_04
F82_02:		MOV	SI,OFFSET ArcPath
		CMP	DS:ArcPnt,0
		JNE	F82_03
		MOV	SI,OFFSET WinShortPath
		TEST	AL,3
		JNZ	F82_03
		CALL	WinPath
F82_03:		PUSH	DI
		CALL	strcpy
		POP	DI
F82_04:		PUSH	ES
		POP	DS
		MOV	SI,DI
		MOV	DI,OFFSET FileSymb2_S
		CALL	ChkForSymb
		MOV	BX,OFFSET TmpBuf
		MOV	DI,BX
		MOV	AL,0
		JNC	F82_05
		MOV	AL,'"'
		STOSB
F82_05:		PUSH	AX
		CALL	strcpy
		CMP	DI,BX
		JE	F82_06
		MOV	AL,'\'
		CMP	DS:[DI-1],AL
		JE	F82_06
		STOSB
F82_06:		POP	AX
		CMP	AL,0
		JE	F82_07
		STOSB
F82_07:		MOV	AL,0
		STOSB
		MOV	DL,DS:DosCol
		MOV	DH,0
		XOR	BP,BP
		JMP	F21_40

F74_00:		CMP	ES:DosBuf,0		; Esc
		JNE	F74_01
		CMP	ES:EscBar,0
		JZ	F74_01
		JMP	F11_00
F74_01:		MOV	AX,1519h

F73_00:		PUSH	ES			; Input command line
		POP	DS
		MOV	SI,OFFSET DosBuf
		MOV	DL,ES:DosCol
		MOV	DH,0
		MOV	CX,126
		CALL	EdLin
		JNC	F73_01
		MOV	ES:DosCol,DL
		CALL	DosCom
F73_01:		JMP	Func05

F75_00:		CMP	ES:MoveCurFlag,2	; Auto CD / Quick view
		JNE	F21_10
		CALL	InvWCB
		MOV	DS:ChgFlg,1
		JMP	Panels00

F84_00:		MOV	CH,0FEh			; Change size of panels
		JMP	F84_02
F84_01:		MOV	CH,0
F84_02:		CMP	DS:Visible,0
		JZ	Func10
		CMP	ES:PanelSize1,0
		JNE	F84_03
		MOV	AL,ES:PanelSize
		MOV	ES:PanelSize1,AL
F84_03:		CALL	BegBX
		CALL	SizDX
		ADD	CH,BH
		ADD	CH,DH
		CALL	F84_10
		JMP	Func10

F84_10		PROC	NEAR			; Set size of panels
		CMP	CH,6
		JAE	F84_11
		MOV	CH,6
F84_11:		MOV	AL,ES:Lines
		SUB	AL,3
		CMP	CH,AL
		JBE	F84_12
		MOV	CH,AL
F84_12:		CALL	BegBX
		CALL	SizDX
		MOV	AL,BH
		ADD	AL,DH
		DEC	AX
		CMP	AL,CH
		JE	F84_16
		INC	CH
		MOV	AH,CH
		MOV	AL,0
		MOV	CL,ES:Lines
		MOV	CH,0
		DEC	CX
		DEC	CX
		XOR	DX,DX
		DIV	CX
		OR	DX,DX
		JE	F84_13
		INC	AX
F84_13:		MOV	ES:FulScr,AH
		OR	AH,AH
		JNZ	F84_14
		MOV	ES:PanelSize,AL
F84_14:		MOV	BL,0
		MOV	DH,CL
		SUB	DH,BH
		MOV	DL,ES:Columns
		CALL	ResUser
		MOV	DS,ES:WCBcrn
		CALL	Panel
		CALL	PutPath
		CALL	PutFls
		MOV	DS,ES:WCBsec
		CMP	DS:Visible,0
		JZ	F84_15
		CALL	Panel
		CALL	PutPath
		CALL	PutFls
F84_15:		CALL	PutPrompt
		CALL	Cursor1
		XOR	BX,BX
		MOV	DH,ES:Lines
		CALL	Window
		STC
F84_16:		RET
F84_10		ENDP

F84_20		PROC	NEAR			; Save panel size
		USES	AX
		MOV	AL,0
		XCHG	AL,ES:PanelSize1
		CMP	AL,0
		JE	F84_21
		CMP	ES:FulScr,0
		JZ	F84_21
		MOV	ES:PanelSize,AL
F84_21:		RET
F84_20		ENDP

F85_00:		MOV	AH,0FFh			; Change width of long name
		JMP	F85_02
F85_01:		MOV	AH,1
F85_02:		CMP	DS:Visible,0
		JZ	Func04
		CMP	DS:WinTyp,p_File
		JNE	Func04
		CMP	DS:LongNames,0
		JZ	Func04
		CALL	NameWidthAL
		ADD	AL,AH
		CMP	AL,12
		JB	Func04
		CALL	SizDX
		DEC	DX
		DEC	DX
		CMP	AL,DL
		JA	Func04
		MOV	DS:NameWidth,AL
		CALL	BegBX
		CALL	Panel
		CALL	PutPath
		CALL	PutFls
		CALL	Window
		JMP	Func04

F86_00:		CMP	DS:Visible,0		; Hidden files
		JZ	Func04
		CMP	DS:WinTyp,p_Tree
		JA	Func04			; Info or QuickView panel
		PUSH	DS
		JE	F86_02
		TEST	DS:FiltAttr,2		; File panel
		JZ	F86_01
		AND	DS:FiltAttr,0F9h
		JMP	M18_20
F86_01:		OR	DS:FiltAttr,6
		JMP	M18_20
F86_02:		CALL	F86_10
		CALL	SizDX
		SUB	DH,4
		CALL	SetTreeParms
		PUSH	DS
		CALL	InvWCB
		CMP	DS:WinTyp,p_Tree
		JNE	F86_03
		CALL	SetTreeParms
F86_03:		POP	DS
		JMP	M04_05

F86_10		PROC	FAR
		USES	BX,DS
		MOV	BX,OFFSET HidDirs
		CALL	InvVar
		MOV	DS,ES:TreeSeg
		MOV	SI,DS:TreeSize
		CALL	TreeAdr2NumbH
		MOV	DS:TreeLenH,SI
		RET
F86_10		ENDP

SearchCur0	EQU	WORD PTR LineBuf1[0]
SearchBuf0	EQU	BYTE PTR LineBuf1[2]
SearchCur1	EQU	WORD PTR LineBuf1[40]
SearchBuf1	EQU	BYTE PTR LineBuf1[42]
SearchBuf2	EQU	BYTE PTR LineBuf1[80]

		.CONST
AltSr_S		DB	'Search:',0

		.DATA
AltSrLen	EQU	12+12+11+10+4
AltSr_T		DB	78h, 79h, 7Ah, 7Bh, 7Ch, 7Dh, 7Eh, 7Fh, 80h, 81h, 82h, 83h
		DB	10h, 11h, 12h, 13h, 14h, 15h, 16h, 17h, 18h, 19h, 1Ah, 1Bh
		DB	1Eh, 1Fh, 20h, 21h, 22h, 23h, 24h, 25h, 26h, 27h, 28h
		DB	2Ch, 2Dh, 2Eh, 2Fh, 30h, 31h, 32h, 33h, 34h, 35h
		DB	37h, 4Ah, 4Eh,0A4h
Ascii_T		DB	"1234567890-="
		DB	"qwertyuiop[]"
		DB	"asdfghjkl;'"
		DB	"zxcvbnm,.?"
		DB	"*-+/"
Srch_T		DW	0013h, F77_10		; ^S
		DW	0004h, F77_10		; ^D
		DW	7300h, F77_10		; ^Left
		DW	7400h, F77_10		; ^Right
		DW	7700h, F77_10		; ^Home
		DW	7500h, F77_10		; ^End
		DW	0E08h, F77_10		; BackSpace
		DW	0007h, F77_10		; ^G
		DW	5300h, F77_10		; Del
		DW	0019h, F77_10		; ^Y
		DW	000Bh, F77_10		; ^K
		DW	0017h, F77_10		; ^W
		DW	007Fh, F77_10		; ^BackSpace
		DW	0014h, F77_10		; ^T
		DW	9300h, F77_10		; ^Del
		DW	000Ah, F77_10		; ^Enter
		DW	001Bh, F77_15		; Esc
		DW	0
AltSr_P		DB	Box_C, BoxCu_C

		.CODE
F77_00:		CALL	F84_20			; Speed search
		PUSH	AX
		MOV	AL,AltSearch_K
		CALL	PutKey
		CALL	BegBX
		CALL	SizDX
		MOV	AX,DX
		MOV	DX,300h+LenSearch+12
		SUB	AL,DL
		SHR	AL,1
		ADD	BX,AX
		DEC	BH
		CALL	SavWin
		PUSH	DS
		PUSH	DX
		PUSH	BX
		MOV	DS,CS:SubsSeg
		XOR	CX,CX
		DEC	DX
		DEC	DX
		MOV	DH,0
		MOV	DI,1
		MOV	BP,OFFSET AltSr_P
		MOV	AH,DS:[BP]
		CALL	Color
		CALL	BoxDubl
		ADD	BX,0FE02h
		MOV	SI,OFFSET AltSr_S
		MOV	AL,DL
		CALL	NgLine
		POP	BX
		POP	DX
		POP	DS
		CALL	Window
		POP	AX
		XOR	CX,CX
		MOV	ES:SearchCur0,CX
		MOV	ES:SearchBuf0,CL
		XCHG	CL,ES:KeyBar
		PUSH	CX
		JMP	F77_02
F77_01:		PUSH	DS
		PUSH	DX
		PUSH	BX
		ADD	BX,10Ah
		MOV	DX,100h+LenSearch
		MOV	CL,DL
		MOV	CH,0
		MOV	SI,OFFSET SearchBuf0
		MOV	DS,CS:SubsSeg
		MOV	AH,DS:[BP+1]
		PUSH	ES
		POP	DS
		PUSH	BP
		XOR	BP,BP
		CALL	InpStrOut
		POP	BP
		CALL	Window
		MOV	AX,ES:SearchCur0
		ADD	AX,BX
		CALL	Cursor
		POP	BX
		POP	DX
		POP	DS
		CALL	Input
F77_02:		CMP	AH,0FCh
		JE	F77_20
		CMP	AH,0F0h
		JAE	F77_01
		MOV	SI,OFFSET Srch_T
		CALL	Case
		CMP	AL,0
		JNE	F77_03
		PUSH	ES
		MOV	ES,CS:SubsSeg
		MOV	DI,OFFSET AltSr_T
		MOV	CX,AltSrLen
		XCHG	AL,AH
		REPNE	SCASB
		XCHG	AL,AH
		POP	ES
		JNE	F77_20
		SUB	DI,OFFSET AltSr_T+1
		PUSH	DS
		MOV	DS,CS:SubsSeg
		MOV	AL,DS:Ascii_T[DI]
		POP	DS
F77_03:		CMP	AL,' '
		JB	F77_20
F77_10:		PUSH	BP
		PUSH	DX
		PUSH	BX
		PUSH	DS
		MOV	BP,DS:EndBuf
		MOV	BX,DS:DiskBuf
		MOV	DX,SIZE FilEnt
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	DS:WinTyp,p_File
		JE	F77_11
		MOV	SI,DS:TreeCur
		CALL	F77_30
F77_11:		CALL	SpeedSearch
		POP	DS
		JZ	F77_14
		CMP	DS:WinTyp,p_File
		JNE	F77_12
		CALL	Addr2Numb
		MOV	DI,DS:FileFirst
		JMP	F77_13
F77_12:		CALL	TreeAdr2NumbH
		MOV	DI,DS:TreeFirst
F77_13:		CALL	F00_50
F77_14:		POP	BX
		POP	DX
		POP	BP
		JMP	F77_01
F77_15:		XOR	AX,AX
F77_20:		POP	CX
		MOV	ES:KeyBar,CL
		CALL	ResWin
		CALL	Window
		CALL	Cursor1
		JMP	Func40

F77_30		PROC	FAR
		MOV	DS,ES:TreeSeg
		MOV	BP,DS:TreeSize
		CALL	TreeNumb2AdrH
		MOV	BX,OFFSET TreeList
		MOV	DX,SIZE DirEnt
		RET
F77_30		ENDP

;---------------------------------------------------------------
;	Speed search
;		   SI - current address
;		   BX - begin
;		   BP - end
;		   DX - add
;		   AX - key code
;	Return:	   ZR - not found
;		   NZ - found
;		   SI - new address
;---------------------------------------------------------------
SpeedSearch	PROC	FAR
		USES	DI
		PUSH	DS
		PUSH	SI
		CMP	AL,0Ah
		JNE	@@EditLine
		CMP	SI,BP
		JAE	@@EditLine
		CALL	SearchAdd
  @@EditLine:	PUSH	ES
		POP	DS
		PUSH	SI
		MOV	SI,OFFSET SearchCur0
		MOV	DI,OFFSET SearchCur1
		MOV	CX,LenSearch+2+1
		REP	MOVSB
		CMP	AL,0Ah
		JE	@@Convert
		PUSH	DX
		MOV	CX,LenSearch
		MOV	SI,OFFSET SearchBuf1
		MOV	DX,ES:SearchCur1
		CALL	EdLin
		MOV	ES:SearchCur1,DX
		POP	DX
  @@Convert:	MOV	SI,OFFSET SearchBuf1
		MOV	DI,OFFSET SearchBuf2
		PUSH	DI
  @@MoveLine1:	MOV	CX,DI
  @@MoveLine2:	LODSB
		STOSB
		CMP	AL,','
		JE	@@MoveLine1
		CMP	AL,';'
		JE	@@MoveLine1
		CMP	AL,0
		JNE	@@MoveLine2
		CMP	CX,OFFSET SearchBuf2
		JBE	@@MoveLine3
		MOV	SI,CX
		CALL	FndLet
		CMP	AL,0
		JE	@@MoveLine4
  @@MoveLine3:	DEC	DI
		MOV	AX,'*'
		STOSW
  @@MoveLine4:	POP	DI
		POP	SI
		POP	AX
		POP	DS
		CMP	BP,BX
		JBE	@@Exit
  @@Next:	PUSH	SI
		CMP	DX,SIZE FilEnt
		JNE	@@NameOffs
		CALL	NameOffset1
		JMP	@@CmpName
  @@NameOffs:	PUSH	AX
		PUSH	DS
		MOV	DS,ES:WCBcrn
		MOV	AL,DS:LongNames
		POP	DS
		CALL	DirOffset1
		POP	AX
  @@CmpName:	CALL	TestMask
		POP	SI
		JE	@@Save
		CALL	SearchAdd
		CMP	SI,AX
		JNE	@@Next
		JMP	@@Exit
  @@Save:	PUSH	DS
		PUSH	SI
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET SearchCur1
		MOV	DI,OFFSET SearchCur0
		MOV	CX,LenSearch+2+1
		REP	MOVSB
		POP	SI
		POP	DS
  @@Exit:	CMP	SI,AX
		RET
SpeedSearch	ENDP

SearchAdd	PROC	NEAR
		CMP	DX,SIZE FilEnt
		JNE	@@AddTree
		CALL	NextFilEnt
		JMP	@@ChkEnd
  @@AddTree:	CALL	NextDirEntH
  @@ChkEnd:	CMP	SI,BP
		JB	@@Exit
		MOV	SI,BX
  @@Exit:	RET
SearchAdd	ENDP
