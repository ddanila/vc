;-------------------------------------------------------
;		Version :	16.10.1999
;-------------------------------------------------------

	.CONST
MkDir_S	DB	" Make directory ",0
MkDi1_S	DB	"^I",0FFh,LenStr-20,0
CrDir_S	DB	"Create the directory",0
MkErr_S	DB	"Can't create the directory",0

	.DATA
MkDi1_P	DB	Box_C, BoxCu_C, 0, 0
	DB	3, 0
	DW	WinBuf
	DW	MkDir_S
	DW	MkDi1_S
	DW	Empty_S
MkDi1_B	DW	MkDi1_P, 528h, 0
	DB	1, 2, 22, 0
	DB	1
	DB	0, 0, 0, 0
	DW	305h, TmpBuf, 200h+LenStr
	DB	0
MkDi2_P	DB	Box_C, BoxCu_C, 0, 0
	DB	3, 0
	DW	WinBuf
	DW	MkDir_S
	DW	Inc_S
	DW	Empty_S
MkDi2_B	DW	MkDi2_P, 528h, 0
	DB	1, 2, 22, 0
	DB	1
	DB	0, 0, 0, 0
	DW	305h, TmpBuf, 20Ch
	DB	0
MkErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 1
	DW	OFFSET ErrBuf
	DW	TmpBuf+130
	DB	6, 64
	DW	Inc_S
	DW	MkErr_S
	DW	Var_S
	DW	Ok_S
MkErr_B	DW	MkErr_P, 928h, 0
	DB	1, 0FFh, 0, 6
	DB	4
	DB	0, 0, 0, 0
	DW	426h, TmpBuf+200, OkMn_D
	DB	0

	.CODE
F37_00:	CMP	DS:WinTyp,1		; F7 - make directory
	JNE	F47_00
	CMP	DS:Visible,0
	JZ	F47_00
	MOV	BP,OFFSET MkDi2_B
	JMP	F47_01
F37_01:	JMP	Func04

F47_00:	MOV	BP,OFFSET MkDi1_B	; Shift F7 - make directory
F47_01:	PUSH	ES
	POP	DS
	MOV	SI,OFFSET TmpBuf
	MOV	BYTE PTR DS:[SI],0
	MOV	DX,OFFSET CrDir_S
	CALL	Dialog
	JNE	F37_01
	CMP	BYTE PTR DS:[SI],0
	JE	F37_01
	MOV	DX,SI
	CALL	CutPath
	MOV	DI,OFFSET TmpBuf+100
	CALL	Convert
	XCHG	SI,DI
	CALL	UnConv
	CMP	BP,OFFSET MkDi2_B
	JNE	F47_02
	MOV	SI,DX
	CALL	CutPath
	CMP	SI,DX
	JNE	F47_03
	PUSH	DX
	MOV	SI,OFFSET CrnPath+2
	MOV	DI,OFFSET TmpBuf+100
	MOV	CX,LenPath
	MOV	DX,DI
	CALL	MvzLine
	POP	SI
	CALL	AddFile
	PUSH	SI
	MOV	CX,13
	REP	MOVSB
	POP	SI
F47_02:	CALL	ChgDisk
	JC	F47_04
	MOV	AH,39h
	CALL	Intr21
	JNC	F47_05
	CMP	AX,53h
	JE	F47_04
F47_03:	MOV	SI,OFFSET TmpBuf
	MOV	DI,OFFSET TmpBuf+130
	MOV	CX,LenPath+13
	REP	MOVSB
	MOV	DX,OFFSET MkDir_S
	MOV	BP,OFFSET MkErr_B
	CALL	Dialog
F47_04:	JMP	Func04
F47_05:	MOV	SI,DX
	CMP	BP,OFFSET MkDi2_B
	JNE	F47_06
	MOV	DI,OFFSET CrnPath+2
	PUSH	DI
	MOV	CX,LenPath-2
	REP	MOVSB
	MOV	AL,0
	STOSB
	POP	SI
	CALL	PathLin
	JMP	F47_07
F47_06:	CALL	CutPath
	CMP	SI,DX
	JNE	F47_07
	CALL	PathLin
	MOV	ES,ES:WCBcrn
	MOV	AX,ES:EndBuf
	MOV	DI,AX
	MOV	ES:CurAdr,AX
	ADD	AX,SIZE FilEnt
	MOV	ES:EndBuf,AX
	MOV	CX,12
	REP	MOVSB
	MOV	AX,1000h
	STOSW
	PUSH	DS
	POP	ES
F47_07:	MOV	SI,DX
	CALL	DelTree
	JMP	FncEx1

	.CONST
Del_S	DB	" Delete ",0
DlMas_S	DB	"Delete the file:",0FFh,LenStr-16,0
DelFl_S	DB	"Deleting the file^I",0
DelFs_S	DB	"s",0
DelDr_S	DB	"Deleting the subdirectory",0
DelSl_S	DB	"You have selected",0
Del1_S	DB	"Do you wish to delete^I",0
Subdi_S	DB	" the directory",0
RdOn_S	DB	"The following file is marked read-only.",0
NoEmp_S	DB	"The following subdirectory is not empty.",0
StDel_S	DB	"Do you still wish to delete it?",0
CnDl_S	DB	"Can't delete the file",0
CnSb_S	DB	"Can't delete the subdirectory",0
DlWr1_S	DB	"WARNING!",0
DlWr2_S	DB	"Do you want to delete all the files",0
DlWr3_S	DB	"^V?",0
DlWr4_S	DB	"in the current directory",0
DlSel_S	DB	"You are ^uDELETING^u",0
From_S	DB	"^V from",0
Var2_S	DB	"^W",0
DelCa_S	DB	" Delete",0FFh,3,"Cancel ",0
DelAl_S	DB	" Delete",0FFh,3,"All",0FFh,3,"Cancel ",0
DelSk_S	DB	"Delete",0FFh,3,"Skip",0FFh,3,"Cancel",0

	.DATA
DelCa_D	DB	8, 0
	DB	8, 9
	DB	0
DelAl_D	DB	8, 0
	DB	5, 9
	DB	8,15
	DB	0
DelSk_D	DB	8, 0
	DB	6, 9
	DB	8,16
	DB	0
DelFl_P	DB	Box_C, 0, 0, 0
	DB	3, 1
	DW	WinBuf
	DW	TmpBuf
	DB	7, 64
	DW	Del_S
	DW	DelFl_S
	DW	Var_S
DelDr_P	DB	Box_C, 0, 0, 0
	DB	3, 1
	DW	WinBuf
	DW	TmpBuf
	DB	6, 64
	DW	Inc_S
	DW	DelDr_S
	DW	Var_S
DlMas_P	DB	Box_C, BoxCu_C, 0, 0
	DB	3, 0
	DW	WinBuf
	DW	Del_S
	DW	DlMas_S
	DW	Empty_S
DlMas_B	DW	DlMas_P, 528h, 0
	DB	1, 2, 23, 0
	DB	1
	DB	0, 0, 0, 0
	DW	305h, ViewFil, LenStr
	DB	0
DelSe_P	DB	Box_C, BoxCu_C, BoxCu_C, 0
	DB	4, 1
	DW	WinBuf
	DW	TmpBuf
	DB	0, 64
	DW	Del_S
	DW	DelSl_S
	DW	Var_S
	DW	DelCa_S
DelSe_B	DW	DelSe_P, 528h, F38_10
	DB	1, 2, 23, 1
	DB	4
	DB	0, 0, 0, 0
	DW	41Fh, TmpBuf+200, DelCa_D
	DB	0
DelCo_P	DB	Err_C, ErInv_C, ErInv_C, ErBr_C
	DB	5, 2
	DW	ErrBuf
	DW	TmpBuf
	DB	0, 64
	DW	TmpBuf+100
	DB	6, 64
	DW	Del_S
	DW	DlSel_S
	DW	From_S
	DW	Var2_S
	DW	DelAl_S
DelCo_B	DW	DelCo_P, 928h, 0
	DB	1, 0FFh, 0, 1
	DB	4
	DB	0, 0, 0, 0
	DW	51Ch, TmpBuf+100, DelAl_D
	DB	0
DelFi_P	DB	Box_C, BoxCu_C, BoxCu_C, 0
	DB	4, 1
	DW	WinBuf
	DW	TmpBuf
	DB	7, 64
	DW	Del_S
	DW	Del1_S
	DW	Var_S
	DW	DelCa_S
DelFi_B	DW	DelFi_P, 528h, 0
	DB	1, 2, 23, 1
	DB	4
	DB	0, 0, 0, 0
	DW	41Fh, TmpBuf+100, DelCa_D
	DB	0
DelDi_P	DB	Box_C, BoxCu_C, BoxCu_C, 0
	DB	4, 1
	DW	WinBuf
	DW	TmpBuf
	DB	6, 62
	DW	Del_S
	DW	Del1_S
	DW	Kov_S
	DW	DelCa_S
DelDi_B	DW	DelDi_P, 528h, 0
	DB	1, 2, 23, 1
	DB	4
	DB	0, 0, 0, 0
	DW	41Fh, TmpBuf+100, DelCa_D
	DB	0
RdOnl_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	5, 1
	DW	ErrBuf
	DW	TmpBuf
	DB	7, 64
	DW	Inc_S
	DW	RdOn_S
	DW	Var_S
	DW	StDel_S
	DW	DelSk_S
RdOnl_B	DW	RdOnl_P, 928h, 0
	DB	1, 0FFh, 0, 5
	DB	4
	DB	0, 0, 0, 0
	DW	51Ch, TmpBuf+230, DelSk_D
	DB	0
NoEmp_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	5, 1
	DW	ErrBuf
	DW	TmpBuf
	DB	6, 64
	DW	Del_S
	DW	NoEmp_S
	DW	Var_S
	DW	StDel_S
	DW	DelAl_S
NoEmp_B	DW	NoEmp_P, 928h, 0
	DB	1, 0FFh, 0, 1
	DB	4
	DB	0, 0, 0, 0
	DW	51Ch, TmpBuf+100, DelAl_D
	DB	0
DlWrn_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	5, 1
	DW	ErrBuf
	DW	TmpBuf+100
	DB	4, 64
	DW	Del_S
	DW	DlWr1_S
	DW	DlWr2_S
	DW	DlWr3_S
	DW	DelCa_S
DlWrn_B	DW	DlWrn_P, 528h, 0
	DB	1, 0FFh, 0, 1
	DB	4
	DB	0, 0, 0, 0
	DW	51Fh, TmpBuf+100, DelCa_D
	DB	0
DelEr_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 1
	DW	ErrBuf
	DW	TmpBuf+130
	DB	7, 64
	DW	Inc_S
	DW	CnDl_S
	DW	Var_S
	DW	OkCan_S
DelEr_B	DW	DelEr_P, 928h, 0
	DB	1, 0FFh, 0, 6
	DB	4
	DB	0, 0, 0, 0
	DW	422h, TmpBuf+230, OkCan_D
	DB	0
CntDi_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 1
	DW	ErrBuf
	DW	TmpBuf
	DB	6, 64
	DW	Inc_S
	DW	CnSb_S
	DW	Var_S
	DW	OkCan_S
CntDi_B	DW	CntDi_P, 928h, 0
	DB	1, 0FFh, 0, 2
	DB	4
	DB	0, 0, 0, 0
	DW	422h, TmpBuf+100, OkCan_D
	DB	0

	.CODE
F38_00:	MOV	AL,0			; F8 - delete
	MOV	ES:[TmpBuf+90],AL
	MOV	ES:[TmpBuf+91],AL
	MOV	ES:[TmpBuf+92],AL
	MOV	ES:FncAddr,OFFSET F38_40
	CMP	DS:Visible,AL
	JZ	F38_03
	CMP	DS:WinTyp,1
	JB	F38_04
	JNE	F38_03
	PUSH	ES			; Delete from tree panel
	POP	DS
	MOV	SI,OFFSET CrnPath
	CMP	BYTE PTR DS:[SI+3],0
	JE	F38_02
	MOV	DI,OFFSET TmpBuf
	MOV	CX,LenPath
	REP	MOVSB
	MOV	DX,OFFSET Subdi_S
	MOV	BP,OFFSET DelDi_B
	CALL	Dialog
	JNE	F38_05
	MOV	DS:ZoomOne,AL
	MOV	AL,10h
	CALL	F38_40
	CMP	AL,0
	JE	F38_02
	MOV	SI,OFFSET CrnPath
	CALL	CutPath
	CMP	SI,OFFSET CrnPath+3
	JBE	F38_01
	DEC	SI
F38_01:	MOV	BYTE PTR DS:[SI],0
F38_02:	JMP	FncExt
F38_03:	JMP	F48_00
F38_04:	CALL	TotSel
	MOV	AX,BX
	OR	AX,DX
	MOV	DI,OFFSET TmpBuf
	JE	F38_06
	CMP	ES:OneFile,0
	JNZ	F38_06
	CALL	FilDir			; Delete selected files
	MOV	SI,OFFSET WinPath
	MOV	DI,OFFSET TmpBuf+100
	MOV	CX,LenPath
	REP	MOVSB
	PUSH	ES
	POP	DS
	MOV	BP,OFFSET DelSe_B
	JMP	F38_08
F38_05:	JMP	Func04
F38_06:	MOV	SI,DS:CurAdr		; Delete file / directory
	CMP	SI,DS:EndBuf
	JAE	F38_02
	CMP	DS:[SI].NameF,'.'
	JE	F38_02
	MOV	DX,OFFSET Empty_S
	MOV	BP,OFFSET DelFi_B
	TEST	DS:[SI].AttrF,10h
	JZ	F38_07
	MOV	DX,OFFSET Subdi_S
	MOV	BP,OFFSET DelDi_B
F38_07:	MOV	CX,13
	REP	MOVSB
	PUSH	ES
	POP	DS
F38_08:	CALL	Dialog
	JNE	F38_05
	JMP	FncSel

F38_10	PROC	NEAR
	CMP	AL,0
	JNE	F38_11
	MOV	BP,OFFSET DelCo_B
	CALL	Dialog
	CMP	AL,1
	JA	F38_11
	MOV	DS:[TmpBuf+90],AL
	MOV	AL,0
F38_11:	CLC
	RET
F38_10	ENDP

F38_40	PROC	NEAR			; Delete file or directory
	PUSH	DS
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	ES
	POP	DS
	TEST	AL,10h
	JZ	F38_41
	JMP	F38_50
F38_41:	MOV	DX,OFFSET Empty_S
	MOV	BX,528h
	MOV	BP,OFFSET DelFl_P
	CALL	OncZoom
	PUSH	SI
	PUSH	DX
	PUSH	BX
	MOV	DX,OFFSET TmpBuf
	CALL	ChgDisk
	JC	F38_45
	MOV	AH,41h
	CALL	Intr21
	JNC	F38_46
	CMP	AX,5
	JNE	F38_42
	MOV	AX,4300h
	CALL	Intr21
	JC	F38_42
	MOV	AL,0
	TEST	CL,1
	JZ	F38_42
	PUSH	DX
	MOV	DX,OFFSET Del_S
	MOV	BP,OFFSET RdOnl_B
	CALL	Dialog
	POP	DX
	CMP	AL,1
	JE	F38_44
	JA	F38_45
	MOV	CX,20h
	MOV	AX,4301h
	CALL	Intr21
	JC	F38_42
	MOV	BYTE PTR DS:[TmpBuf+91],1
	MOV	AH,41h
	CALL	Intr21
	JNC	F38_46
F38_42:	MOV	SI,DX
	MOV	DI,OFFSET TmpBuf+130
	MOV	CX,LenPath+13
	REP	MOVSB
	MOV	DX,OFFSET Del_S
	MOV	BP,OFFSET DelEr_B
F38_43:	CMP	AX,53h
	JE	F38_45
	CALL	Dialog
	JNZ	F38_45
F38_44:	XOR	AX,AX
	JMP	F38_56
F38_45:	JMP	F38_53
F38_46:	JMP	F38_55
F38_50:	MOV	BX,528h
	MOV	DX,OFFSET Del_S
	MOV	BP,OFFSET DelDr_P
	CALL	OncZoom
	PUSH	SI
	PUSH	DX
	PUSH	BX
	MOV	DX,OFFSET TmpBuf
	CALL	ChgDisk
	JC	F38_45
	CALL	F38_60
	JNC	F38_54
	MOV	DX,OFFSET Del_S
	MOV	BP,OFFSET CntDi_B
	CMP	AX,100
	JAE	F38_51
	CMP	AL,5
	JNE	F38_43
F38_51:	CMP	BYTE PTR DS:[TmpBuf+90],0
	JNE	F38_52
	MOV	BP,OFFSET NoEmp_B
	CALL	Dialog
	CMP	AL,1
	JA	F38_44
	JB	F38_52
	MOV	BYTE PTR DS:[TmpBuf+90],1
F38_52:	POP	BX
	POP	DX
	POP	SI
	CALL	ResWin
	CALL	Window
	MOV	DI,OFFSET TmpBuf
	MOV	CX,LenPath+13
	MOV	AL,0
	REPNE	SCASB
	DEC	DI
	PUSH	DI
	PUSH	CS
	POP	DS
	MOV	SI,OFFSET Mask_S
	MOV	CX,5
	REP	MOVSB
	POP	DI
	CALL	F38_70
	MOV	BYTE PTR ES:[DI],0
	OR	AH,AH
	JNZ	F38_57
	CALL	DirFunc
	PUSH	ES
	POP	DS
	OR	AH,AH
	JNZ	F38_57
	MOV	BX,528h
	MOV	DX,OFFSET Del_S
	MOV	BP,OFFSET DelDr_P
	CALL	OncZoom
	PUSH	SI
	PUSH	DX
	PUSH	BX
	MOV	DX,OFFSET TmpBuf
	CALL	F38_60
	JNC	F38_54
	CMP	AX,53h
	JE	F38_53
	JMP	F38_44
F38_53:	MOV	AX,100h
	JMP	F38_56
F38_54:	MOV	BYTE PTR DS:[TmpBuf+91],1
	CMP	BYTE PTR DS:[TmpBuf+92],0
	JNZ	F38_55
	MOV	BYTE PTR DS:[TmpBuf+92],1
	MOV	SI,OFFSET TmpBuf
	CALL	DelTree
	JNC	F38_55
	CMP	AX,53h
	JE	F38_53
F38_55:	MOV	AX,1
F38_56:	POP	BX
	POP	DX
	POP	SI
	CALL	ResWin
	CALL	Window
F38_57:	OR	ES:[TmpBuf+91],AL
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	POP	DS
	RET
F38_40	ENDP

F38_60	PROC	NEAR
	MOV	AH,3Ah
	CALL	Intr21
	JNC	F38_61
	CMP	AX,10h
	STC
	JNE	F38_61
	MOV	SI,DX
	MOV	DI,OFFSET TmpErr
	MOV	AH,60h
	CALL	Intr21
	JC	F38_61
	PUSH	SI
	PUSH	DI
	XCHG	SI,DI
	MOV	CX,LenPath+13
	REP	MOVSB
	POP	DI
	POP	SI
	MOV	DX,DI
	MOV	AX,'.'
	STOSB
	STOSW
	MOV	AH,3Bh
	CALL	Intr21
	JC	F38_61
	MOV	DX,SI
	MOV	AH,3Ah
	CALL	Intr21
F38_61:	RET
F38_60	ENDP

F38_70	PROC	NEAR			; Delete files
	PUSH	DS
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	ES
	POP	DS
	MOV	DI,OFFSET TmpBuf+200
	MOV	AL,0FFh
	STOSB
	ADD	DI,5
	MOV	AL,26h
	STOSB
	MOV	AX,WORD PTR DS:TmpBuf[0]
	CMP	AH,':'
	JE	F38_71
	MOV	AL,'A'-1
F38_71:	SUB	AL,'A'-1
	STOSB
	MOV	SI,OFFSET TmpBuf
	CALL	CutPath
	MOV	CX,SI
	DEC	DI
	MOV	AX,2902h
	INT	21h
	CMP	AL,1
	MOV	AX,0
	JE	F38_72
	JMP	F38_80
F38_72:	MOV	DX,OFFSET DelFs_S
	MOV	BX,528h
	MOV	BP,OFFSET DelFl_P
	CALL	OncZoom
	PUSH	SI
	PUSH	DX
	PUSH	BX
	MOV	SI,CX
	MOV	DX,OFFSET TmpBuf
	CALL	ChgDisk
	JC	F38_76
	MOV	DI,OFFSET TmpBuf+100
	MOV	BYTE PTR DS:[DI],0
	CMP	SI,OFFSET TmpBuf
	JBE	F38_77
	CMP	BYTE PTR DS:[SI-1],'\'
	JNE	F38_77
	PUSH	SI
	MOV	AL,DS:TmpBuf+200[7]
	MOV	DL,AL
	CMP	AL,0
	JE	F38_73
	ADD	AL,'A'-1
	MOV	AH,':'
	STOSW
F38_73:	MOV	AL,'\'
	STOSB
	MOV	SI,DI
	MOV	AH,47h
	CALL	Intr21
	POP	SI
	JC	F38_75
	CMP	SI,OFFSET TmpBuf+1
	JBE	F38_74
	CMP	BYTE PTR DS:[SI-2],':'
	JE	F38_74
	DEC	SI
F38_74:	MOV	BL,0
	XCHG	BL,DS:[SI]
	MOV	DX,OFFSET TmpBuf
	MOV	AH,3Bh
	CALL	Intr21
	MOV	DS:[SI],BL
	JNC	F38_77
F38_75:	CMP	AX,53h
	JE	F38_76
	XOR	AX,AX
	JMP	F38_79
F38_76:	MOV	AX,100h
	JMP	F38_79
F38_77:	MOV	BYTE PTR DS:[TmpBuf+91],1
	MOV	DX,OFFSET TmpBuf+200
	MOV	AH,13h
	CALL	Intr21
	CMP	BYTE PTR DS:TmpBuf[100],0
	JE	F38_78
	MOV	BL,AL
	MOV	DX,OFFSET TmpBuf+100
	MOV	AH,3Bh
	CALL	Intr21
	JC	F38_76
	MOV	AL,BL
F38_78:	INC	AX
	CBW
F38_79:	POP	BX
	POP	DX
	POP	SI
	CALL	ResWin
	CALL	Window
F38_80:	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	POP	DS
	RET
F38_70	ENDP

F48_00:	PUSH	ES			; Shift F8 - delete
	POP	DS
	MOV	BP,OFFSET DlMas_B
	CALL	Dialog
	JNE	F48_03
	MOV	SI,OFFSET ViewFil
	MOV	DI,OFFSET TmpBuf
	MOV	CX,LenStr+1
	PUSH	DI
	REP	MOVSB
	POP	SI
	CALL	NormStr
	CALL	LetLin
	CMP	BYTE PTR DS:[SI],0
	JE	F48_03
	CALL	CutPath
	CMP	WORD PTR DS:[SI],'.'
	JE	F48_01
	PUSH	SI
	MOV	DI,OFFSET TmpBuf+100
	MOV	AH,29h
	INT	21h
	POP	SI
	CMP	AL,1
	JNE	F48_04
	INC	DI
	MOV	CX,11
	MOV	AL,'?'
	REPE	SCASB
	JNE	F48_04
F48_01:	PUSH	CS
	POP	DS
	MOV	DI,SI
	MOV	SI,OFFSET Mask_S+1
	MOV	CX,4
	REP	MOVSB
	PUSH	ES
	POP	DS
	MOV	SI,OFFSET TmpBuf
	MOV	DI,SI
	CALL	CutPath
	XCHG	SI,DI
	CMP	SI,DI
	JNE	F48_02
	PUSH	CS
	POP	DS
	MOV	SI,OFFSET DlWr4_S
F48_02:	MOV	DI,OFFSET TmpBuf+100
	MOV	CX,LenPath+13
	REP	MOVSB
	PUSH	ES
	POP	DS
	MOV	BP,OFFSET DlWrn_B
	CALL	Dialog
	JE	F48_04
F48_03:	JMP	Func04
F48_04:	MOV	AL,0
	MOV	DS:ZoomOne,AL
	MOV	DS:[TmpBuf+90],AL
	MOV	DS:[TmpBuf+91],AL
	MOV	DS:[TmpBuf+92],AL
	MOV	DS:FncAddr,OFFSET F38_40
	CALL	F38_70
	OR	AH,AH
	JNZ	F48_06
	XOR	BX,BX
	CMP	AL,0
	JNZ	F48_05
	MOV	BX,OFFSET Del_S
F48_05:	JMP	FncMs1
F48_06:	JMP	FncExt

F58_00:	MOV	ES:OneFile,1		; ^F8 - delete one file
	JMP	F38_00

	.CONST
Attr_S	DB	" Attributes ",0
Attr1_S	DB	"Change file attributes^I",0
Attr2_S	DB	"^uSet  Clear^u",0FFh,21,0
Attr3_S	DB	" [ ]  [ ] Read only",0FFh,4,"Date",0FFh,6,0
Attr4_S	DB	" [ ]  [ ] Archive",0FFh,16,0
Attr5_S	DB	" [ ]  [ ] Hidden",0FFh,7,"Time",0FFh,6,0
Attr6_S	DB	" [ ]  [ ] System",0FFh,17,0
Attr7_S	DB	"[ Set ]   [ Cancel ]",0
Attr8_S	DB	" for",0
StAtr_S	DB	"Setting attributes:",0
NoDt1_S	DB	"Can't set the attributes.",0
NoDt2_S	DB	"Illegal date or time.",0
NoFnd_S	DB	"Can't find the file",0
NoAtr_S	DB	"Can't set attributes",0

	.DATA
StAt1_P	DB	Box_C, BoxCu_C, Box_C, BoxBr_C
	DB	9, 1
	DW	WinBuf
	DW	TmpBuf+100
	DB	7, 62
	DW	Attr_S
	DW	Attr1_S
	DW	Kov_S
	DW	Attr3_S + 5
	DW	Attr4_S + 5
	DW	Attr5_S + 5
	DW	Attr6_S + 5
	DW	0
	DW	Attr7_S
StAt1_B	DW	StAt1_P, 428h, M30_60
	DB	1, 2, 24, 0
	DB	3			; Read only
	DB	6, 1, 0, 4
	DW	407h, TmpBuf+0, TmpBuf+0
	DB	3			; Archive
	DB	0, 2, 1, 4
	DW	507h, TmpBuf+1, TmpBuf+1
	DB	3			; Hidden
	DB	1, 3, 2, 5
	DW	607h, TmpBuf+2, TmpBuf+2
	DB	3			; System
	DB	2, 6, 3, 5
	DW	707h, TmpBuf+3, TmpBuf+3
	DB	1			; Date
	DB	3, 5, 1, 4
	DW	517h, TmpBuf+15, 00Ah
	DB	1			; Time
	DB	4, 6, 3, 5
	DW	717h, TmpBuf+30, 10Ah
	DB	4			; Set / Cancel
	DB	5, 0, 6, 6
	DW	91Eh, TmpBuf+8, SetCa_D
	DB	0
StAt2_P	DB	Box_C, BoxCu_C, Box_C, BoxBr_C
	DB	9, 0
	DW	WinBuf
	DW	Attr_S
	DW	Attr1_S
	DW	Attr2_S
	DW	Attr3_S
	DW	Attr4_S
	DW	Attr5_S
	DW	Attr6_S
	DW	0
	DW	Attr7_S
StAt2_B	DW	StAt2_P, 428h, M30_60
	DB	1, 2, 24, 0
	DB	3			; Read only
	DB	10, 1, 0, 4
	DW	407h, TmpBuf+0, TmpBuf+0
	DB	3			; Archive
	DB	0, 2, 1, 5
	DW	507h, TmpBuf+1, TmpBuf+1
	DB	3			; Hidden
	DB	1, 3, 2, 6
	DW	607h, TmpBuf+2, TmpBuf+2
	DB	3			; System
	DB	2, 10, 3, 7
	DW	707h, TmpBuf+3, TmpBuf+3
	DB	3			; Read only
	DB	3, 5, 0, 8
	DW	40Ch, TmpBuf+10, TmpBuf+10
	DB	3			; Archive
	DB	4, 6, 1, 8
	DW	50Ch, TmpBuf+11, TmpBuf+11
	DB	3			; System
	DB	5, 7, 2, 9
	DW	60Ch, TmpBuf+12, TmpBuf+12
	DB	3			; Hidden
	DB	6, 10, 3, 9
	DW	70Ch, TmpBuf+13, TmpBuf+13
	DB	1			; Date
	DB	7, 9, 5, 8
	DW	51Ch, TmpBuf+15, 00Ah
	DB	1			; Time
	DB	8, 10, 7, 9
	DW	71Ch, TmpBuf+30, 10Ah
	DB	4			; Set / Cancel
	DB	9, 0, 10, 10
	DW	91Eh, TmpBuf+8, SetCa_D
	DB	0
StAtr_P	DB	Box_C, 0, 0, 0
	DB	3, 1
	DW	WinBuf
	DW	TmpBuf
	DB	7, 64
	DW	Attr_S
	DW	StAtr_S
	DW	Var_S
NoDat_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 0
	DW	ErrBuf
	DW	Attr_S
	DW	NoDt1_S
	DW	NoDt2_S
	DW	Ok_S
NoDat_B	DW	NoDat_P, 0C28h, 0
	DB	1, 0FFh, 0, 0
	DB	4
	DB	0, 0, 0, 0
	DW	426h, TmpBuf+100, OkMn_D
	DB	0
AtrEr_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 1
	DW	ErrBuf
	DW	TmpBuf
	DB	7, 64
	DW	Attr_S
	DW	Inc_S
	DW	Var_S
	DW	OkCan_S
AtrEr_B	DW	AtrEr_P, 928h, 0
	DB	1, 0FFh, 0, 2
	DB	4
	DB	0, 0, 0, 0
	DW	422h, TmpBuf+100, OkCan_D
	DB	0
SetCa_D	DB	 7,  0
	DB	10, 10
	DB	 0

AttrDat	EQU	WORD PTR TmpBuf[96]
AttrTim	EQU	WORD PTR TmpBuf[98]

	.CODE
M30_00:	CALL	FilePan			; Attributes...
	JNC	M30_02
	CALL	TotSel
	OR	BX,DX
	JE	M30_03
M30_01:	MOV	DI,OFFSET TmpBuf	; Selected files
	MOV	CX,100
	MOV	AL,0
	REP	STOSB
	PUSH	ES
	POP	DS
	MOV	DX,OFFSET Empty_S
	MOV	BP,OFFSET StAt2_B
	CALL	Dialog
	JNE	M30_02
	MOV	SI,OFFSET TmpBuf
	CALL	M30_82
	MOV	AH,AL
	MOV	SI,OFFSET TmpBuf+10
	CALL	M30_82
	NOT	AL
	JMP	M30_07
M30_02:	JMP	Func04
M30_03:	MOV	SI,DS:CurAdr		; One file
	CMP	SI,DS:EndBuf
	JAE	M30_02
	CMP	DS:[SI].NameF,'.'
	JE	M30_02
	MOV	CL,DS:[SI].AttrF
	TEST	CL,10h
	JNZ	M30_01
	MOV	DI,OFFSET TmpBuf
	MOV	AL,1
	CALL	M30_80
	MOV	AL,20h
	CALL	M30_80
	MOV	AL,2
	CALL	M30_80
	MOV	AL,4
	CALL	M30_80
	MOV	AX,DS:[SI].DateF
	CALL	PutDate
	MOV	DI,OFFSET TmpBuf+15
	CMP	BL,' '
	JNE	M30_04
	DEC	DI
M30_04:	CALL	BufDate
	MOV	AL,0
	STOSB
	MOV	AX,DS:[SI].TimeF
	CALL	PutTime
	MOV	DI,OFFSET TmpBuf+30
	CMP	BL,' '
	JNE	M30_05
	DEC	DI
M30_05:	CALL	BufTime
	CMP	AL,' '
	JNE	M30_06
	DEC	DI
M30_06:	MOV	AL,0
	STOSB
	MOV	DI,OFFSET TmpBuf+100
	MOV	CX,13
	REP	MOVSB
	PUSH	ES
	POP	DS
	MOV	DX,OFFSET Attr8_S
	MOV	BP,OFFSET StAt1_B
	CALL	Dialog
	JNE	M30_08
	MOV	SI,OFFSET TmpBuf
	CALL	M30_82
	MOV	AH,AL
M30_07:	MOV	WORD PTR DS:[TmpBuf+94],AX
	MOV	DS:FncAddr,OFFSET M30_50
	JMP	FncSel
M30_08:	JMP	Func04

M30_50	PROC	NEAR			; Set attributes
	PUSH	DS
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	ES
	POP	DS
	TEST	AL,10h
	JZ	M30_51
	CALL	DirFunc
	JMP	M30_59
M30_51:	MOV	BX,528h
	MOV	BP,OFFSET StAtr_P
	CALL	OncZoom
	PUSH	SI
	PUSH	DX
	PUSH	BX
	MOV	DX,OFFSET TmpBuf
	CALL	ChgDisk
	JC	M30_55
	MOV	AX,4300h
	CALL	Intr21
	JC	M30_56
	MOV	DI,CX
	MOV	AX,DS:AttrDat
	AND	AX,DS:AttrTim
	INC	AX
	JE	M30_54
	AND	CL,0FEh
	MOV	AX,4301h
	CALL	Intr21
	JC	M30_56
	MOV	BYTE PTR DS:[TmpBuf+91],1
	MOV	AL,41h
	CALL	OpenFil
	JC	M30_56
	MOV	BX,AX
	PUSH	DX
	MOV	AX,5700h
	CALL	Intr21
	MOV	AX,DS:AttrDat
	CMP	AX,0FFFFh
	JE	M30_52
	MOV	DX,AX
M30_52:	MOV	AX,DS:AttrTim
	CMP	AX,0FFFFh
	JE	M30_53
	MOV	CX,AX
M30_53:	MOV	AX,5701h
	CALL	Intr21
	POP	DX
	MOV	AH,3Eh
	CALL	Intr21
	JC	M30_56
M30_54:	MOV	CX,DI
	AND	CL,DS:[TmpBuf+94]
	OR	CL,DS:[TmpBuf+95]
	MOV	AX,4301h
	CALL	Intr21
	JC	M30_56
	MOV	AX,1
	JMP	M30_58
M30_55:	MOV	AX,100h
	JMP	M30_58
M30_56:	CMP	AX,53h
	JE	M30_55
	MOV	DX,OFFSET NoFnd_S
	CMP	AX,3
	JBE	M30_57
	MOV	DX,OFFSET NoAtr_S
M30_57:	MOV	BP,OFFSET AtrEr_B
	CALL	Dialog
	JNE	M30_55
	XOR	AX,AX
M30_58:	OR	DS:[TmpBuf+91],AL
	POP	BX
	POP	DX
	POP	SI
	CALL	ResWin
	CALL	Window
M30_59:	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	POP	DS
	RET
M30_50	ENDP

M30_60	PROC	NEAR			; Test date & time
	CMP	AL,0
	JNE	M30_76
	MOV	DS:AttrDat,0FFFFh
	MOV	DS:AttrTim,0FFFFh
	MOV	SI,OFFSET DosBuf+LenCmd+10+3
	CALL	FndLet
	CMP	AL,0
	JE	M30_70
	CALL	TxtNum
	JC	M30_62
	MOV	CX,AX
	CALL	FndLet
	CMP	AL,DS:Country.DateSep
	JNE	M30_61
	INC	SI
	CALL	FndLet
	CALL	TxtNum
	JC	M30_62
	MOV	DX,AX
	CALL	FndLet
	CMP	AL,DS:Country.DateSep
M30_61:	JNE	M30_67
	INC	SI
	CALL	FndLet
	CALL	TxtNum
M30_62:	JC	M30_67
	MOV	BX,AX
	CALL	FndLet
	CMP	AL,0
	JNE	M30_67
	CMP	DS:Country.DateFmt,0
	JE	M30_63
	XCHG	CX,DX
	CMP	DS:Country.DateFmt,1
	JE	M30_63
	XCHG	BX,DX
M30_63:	SUB	BX,1980
	JAE	M30_64
	ADD	BX,1980
	CMP	BX,100
	JAE	M30_67
	SUB	BL,80
	JAE	M30_64
	ADD	BL,100
M30_64:	CMP	BX,128
	JAE	M30_67
	CMP	CX,12
	JA	M30_67
	CMP	CL,1
	JB	M30_67
	MOV	AX,CX
	CMP	AL,7
	JBE	M30_65
	INC	AX
M30_65:	AND	AL,1
	ADD	AL,30
	CMP	CL,2
	JNE	M30_66
	MOV	AL,28
	TEST	BL,3
	JNZ	M30_66
	INC	AX
M30_66:	CMP	DX,AX
	JA	M30_67
	OR	DX,DX
	JE	M30_67
	MOV	AL,CL
	MOV	AH,0
	MOV	CL,5
	SHL	AX,CL
	SHL	BL,1
	ADD	AL,DL
	ADD	AH,BL
	MOV	DS:AttrDat,AX
	JMP	M30_70
M30_67:	JMP	M30_75
M30_70:	MOV	SI,OFFSET DosBuf+LenCmd+10+100+3
	CALL	FndLet
	CMP	AL,0
	JE	M30_76
	XOR	CX,CX
	XOR	BX,BX
	CALL	TxtNum
	JC	M30_67
	MOV	DX,AX
	CALL	FndLet
	CMP	AL,DS:Country.TimeSep
	JNE	M30_71
	INC	SI
	CALL	FndLet
	CALL	TxtNum
	JC	M30_67
	CMP	AX,60
	JAE	M30_67
	MOV	CH,AL
	CALL	FndLet
	CMP	AL,DS:Country.TimeSep
	JNE	M30_71
	INC	SI
	CALL	FndLet
	CALL	TxtNum
	JC	M30_67
	CMP	AX,60
	JAE	M30_67
	MOV	BL,AL
	CALL	FndLet
M30_71:	CMP	AL,0
	JE	M30_72
	CALL	UpCase
	MOV	BH,AL
	INC	SI
	CALL	FndLet
	CMP	AL,0
	JNE	M30_67
M30_72:	MOV	AX,24
	CMP	BH,0
	JE	M30_73
	MOV	AL,12
	CMP	BH,'A'
	JE	M30_73
	MOV	CL,12
	CMP	BH,'P'
	JNE	M30_75
M30_73:	CMP	DX,AX
	JA	M30_75
	JNE	M30_74
	XOR	DX,DX
M30_74:	MOV	AH,DL
	ADD	AH,CL
	MOV	CL,3
	SHL	AH,CL
	MOV	AL,BL
	SHR	AL,1
	MOV	BH,CH
	MOV	BL,0
	SHR	BX,CL
	ADD	AX,BX
	MOV	DS:AttrTim,AX
	MOV	AL,0
	CLC
	JMP	M30_76
M30_75:	MOV	BP,OFFSET NoDat_B
	CALL	Dialog
	STC
M30_76:	RET
M30_60	ENDP

M30_80	PROC	NEAR
	TEST	AL,CL
	MOV	AL,0
	JZ	M30_81
	MOV	AL,1
M30_81:	STOSB
	RET
M30_80	ENDP

M30_82	PROC	NEAR
	PUSH	BX
	MOV	AL,0
	MOV	BL,1
	CALL	M30_83
	MOV	BL,20h
	CALL	M30_83
	MOV	BL,2
	CALL	M30_83
	MOV	BL,4
	CALL	M30_83
	POP	BX
	RET
M30_82	ENDP

M30_83	PROC	NEAR
	CMP	BYTE PTR DS:[SI],0
	JE	M30_84
	OR	AL,BL
M30_84:	INC	SI
	RET
M30_83	ENDP

	.CONST
Lab0_S	DB	' Volume Label ',0
Lab1_S	DB	'Change volume label^V to',0
Lab2_S	DB	'Do you wish to delete volume label',0
Lab3_S	DB	'^V ?',0
ErVol_S	DB	'Could not set the volume label',0

	.DATA
Label_P	DB	Box_C, BoxCu_C, 0, 0
	DB	3, 1
	DW	WinBuf
	DW	TmpBuf+20
	DB	0, 30
	DW	Lab0_S
	DW	Lab1_S
	DW	Empty_S
Label_B	DW	Label_P, 528h, 0
	DB	1, 3, 29, 0
	DB	1
	DB	0, 0, 0, 0
	DW	305h, TmpBuf+1, 20Bh
	DB	0
DelVo_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 1
	DW	ErrBuf
	DW	Volume
	DB	0, 15
	DW	Lab0_S
	DW	Lab2_S
	DW	Lab3_S
	DW	DelCa_S
DelVo_B	DW	DelVo_P, 728h, 0
	DB	1, 0FFh, 0, 5
	DB	4
	DB	0, 0, 0, 0
	DW	41Fh, TmpBuf+100, DelCa_D
	DB	0
ErVol_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 1
	DW	ErrBuf
	DW	TmpBuf+1
	DB	6, 15
	DW	Lab0_S
	DW	ErVol_S
	DW	Var_S
	DW	Ok_S
ErVol_B	DW	ErVol_P, 728h, 0
	DB	1, 0FFh, 0, 2
	DB	4
	DB	0, 0, 0, 0
	DW	426h, TmpBuf+100, OkMn_D
	DB	0

	.CODE
F54_00:	CALL	HidCur			; Change label
	PUSH	ES
	POP	DS
	MOV	DX,OFFSET DosPth
	CALL	ChgDisk
	MOV	SI,OFFSET Volume
	CALL	GetVol
	JNC	F54_02
	MOV	DI,OFFSET TmpBuf
	MOV	AL,'\'
	STOSB
	MOV	CX,12
	PUSH	SI
	PUSH	CX
	REP	MOVSB
	POP	CX
	POP	SI
	MOV	DI,OFFSET TmpBuf+20
	MOV	BYTE PTR DS:[DI],0
	CMP	BYTE PTR DS:Volume[0],0
	JE	F54_01
	MOV	AX,'" '
	STOSW
	CALL	MvzLine
	MOV	AX,'"'
	STOSW
F54_01:	MOV	BP,OFFSET Label_B
	CALL	Dialog
	JNE	F54_02
	MOV	DX,OFFSET TmpBuf+20
	MOV	SI,OFFSET Volume
	MOV	DI,DX
	MOV	AL,0FFh
	STOSB
	ADD	DI,5
	MOV	AX,8
	STOSW
	MOV	CX,12
	PUSH	CX
	CALL	MvzLine
	POP	CX
	MOV	AL,' '
	REP	STOSB
	CMP	BYTE PTR DS:TmpBuf[1],0
	JNE	F54_03
	CMP	BYTE PTR DS:Volume[0],0
	JE	F54_02
	MOV	BP,OFFSET DelVo_B
	CALL	Dialog
	JNE	F54_02
	MOV	AH,13h
	CALL	Intr21
	JMP	FncExt
F54_02:	JMP	Func04
F54_03:	MOV	AH,13h
	CALL	Intr21
	MOV	AX,WORD PTR DS:TmpBuf[11]
	MOV	WORD PTR DS:TmpBuf[12],AX
	MOV	AX,WORD PTR DS:TmpBuf[9]
	MOV	WORD PTR DS:TmpBuf[10],AX
	MOV	BYTE PTR DS:TmpBuf[9],'.'
	MOV	DX,OFFSET TmpBuf
	MOV	CX,8
	MOV	AH,3Ch
	CALL	Intr21
	JC	F54_04
	MOV	BX,AX
	MOV	AH,3Eh
	CALL	Intr21
	JMP	FncExt
F54_04:	MOV	AX,WORD PTR DS:TmpBuf[10]
	MOV	WORD PTR DS:TmpBuf[9],AX
	MOV	AX,WORD PTR DS:TmpBuf[12]
	MOV	WORD PTR DS:TmpBuf[11],AX
	MOV	BP,OFFSET ErVol_B
	CALL	Dialog
	JMP	FncExt

	.CONST
PthEr_S	DB	'Path too long', 0

	.DATA
PthEr_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 1
	DW	OFFSET ErrBuf
	DW	TmpBuf
	DB	6, 64
	DW	Err_S
	DW	PthEr_S
	DW	Var_S
	DW	Ok_S
PthEr_B	DW	PthEr_P, 528h, 0
	DB	1, 0FFh, 0, 6
	DB	4
	DB	0, 0, 0, 0
	DW	426h, TmpBuf+200, OkMn_D
	DB	0

DirFunc	PROC	NEAR			; Find files in directory
	PUSH	ES
	POP	DS
	MOV	DI,OFFSET TmpBuf
	MOV	DX,DI
	MOV	CX,LenPath
	XOR	AX,AX
	REPNE	SCASB
	JE	DiFu01
	CMP	ES:FncAddr,OFFSET DiSi10
	JE	DiFu00
	MOV	BP,OFFSET PthEr_B
	CALL	Dialog
DiFu00:	XOR	AX,AX
	JMP	DiFu14
DiFu01:	DEC	DI
	PUSH	CS
	POP	DS
	MOV	SI,OFFSET Mask_S+1
	CMP	DI,DX
	JE	DiFu02
	MOV	AL,ES:[DI-1]
	CMP	AL,':'
	JE	DiFu02
	CMP	AL,'\'
	JE	DiFu02
	DEC	SI
DiFu02:	MOV	CX,5
	REP	MOVSB
	PUSH	ES
	POP	DS
	CALL	SetDTA
	MOV	BX,1
	MOV	CX,37h
	MOV	AH,4Eh
	JMP	DiFu05
DiFu03:	MOV	SI,OFFSET DTA.FilName
	CMP	BYTE PTR DS:[SI],'.'
	JE	DiFu04
	MOV	DI,SI
	MOV	SI,OFFSET TmpBuf
	CALL	CutPath
	XCHG	SI,DI
	MOV	CX,13
	REP	MOVSB
	CALL	TstBrk
	JC	DiFu06
	MOV	AL,DS:DTA.FilAttr
	CALL	DS:FncAddr
	PUSH	ES
	POP	DS
	CMP	AH,0
	JNZ	DiFu06
	CMP	AL,0
	JNZ	DiFu04
	MOV	BL,0
DiFu04:	MOV	AH,4Fh
DiFu05:	CALL	ChgDisk
	JC	DiFu06
	CALL	Intr21
	JNC	DiFu03
	CMP	AX,53h
	JNE	DiFu07
DiFu06:	MOV	AX,100h
	JMP	DiFu14
DiFu07:	MOV	SI,OFFSET TmpBuf
	MOV	DX,SI
	CALL	CutPath
	CMP	SI,DX
	JBE	DiFu06
	MOV	AL,DS:[SI-1]
	CMP	AL,'\'
	JNE	DiFu08
	DEC	SI
	CMP	SI,DX
	JBE	DiFu06
	MOV	BYTE PTR DS:[SI],0
	MOV	AL,DS:[SI-1]
DiFu08:	CMP	AL,':'
	JE	DiFu13
	MOV	SI,DX
	CALL	CutPath
	PUSH	SI
	MOV	DI,OFFSET MenuBuf
	MOV	CX,13
	REP	MOVSB
	POP	DI
	PUSH	CS
	POP	DS
	MOV	SI,OFFSET Mask_S+1
	MOV	CX,4
	REP	MOVSB
	PUSH	ES
	POP	DS
	CALL	ChgDisk
	JC	DiFu06
	MOV	CX,37h
	MOV	AH,4Eh
	JMP	DiFu11
DiFu09:	TEST	DS:DTA.FilAttr,10h
	JZ	DiFu10
	MOV	DI,OFFSET MenuBuf
	MOV	CX,13
	MOV	AL,0
	PUSH	DI
	REPNE	SCASB
	MOV	CX,DI
	POP	DI
	SUB	CX,DI
	MOV	SI,OFFSET DTA.FilName
	REPE	CMPSB
	JE	DiFu12
DiFu10:	MOV	AH,4Fh
DiFu11:	CALL	Intr21
	JNC	DiFu09
	JMP	DiFu06
DiFu12:	MOV	SI,OFFSET TmpBuf
	CALL	CutPath
	MOV	DI,SI
	MOV	SI,OFFSET MenuBuf
	MOV	CX,13
	REP	MOVSB
DiFu13:	MOV	AX,BX
DiFu14:	RET
DirFunc	ENDP

FncSel:	MOV	AL,0			; Selected files and directories
	MOV	ES:ZoomOne,AL
	MOV	ES:[TmpBuf+91],AL
	MOV	DS,ES:WCBcrn
	MOV	SI,DS:CurAdr
	CMP	ES:OneFile,0
	JNZ	FncSl5
	MOV	SI,OFFSET DskBuf
FncSl1:	CMP	SI,DS:EndBuf
	JAE	FncSl2
	AND	DS:[SI].AttrF,7Fh
	ADD	SI,SIZE FilEnt
	JMP	FncSl1
FncSl2:	CALL	TotSel
	MOV	SI,DS:CurAdr
	OR	BX,DX
	JE	FncSl5
	MOV	SI,OFFSET DskBuf
FncSl3:	CMP	SI,DS:EndBuf
	JB	FncSl4
	JMP	FncExt
FncSl4:	TEST	DS:[SI].AttrF,40h
	JNZ	FncSl5
	JMP	FncSl9
FncSl5:	PUSH	SI
	MOV	DI,OFFSET TmpBuf
	MOV	CX,13
	REP	MOVSB
	POP	SI
	MOV	AL,DS:[SI].AttrF
	CALL	ES:FncAddr
	OR	ES:[TmpBuf+91],AL
	MOV	DS,ES:WCBcrn
	CMP	ES:OneFile,0
	JNZ	FncExt
	PUSH	AX
	CMP	AL,0
	JZ	FncSl8
	TEST	DS:[SI].AttrF,40h
	JZ	FncSl8
	AND	DS:[SI].AttrF,3Fh
	OR	DS:[SI].AttrF,80h
	CMP	ES:UnSlct,0
	JZ	FncSl8
	CALL	PutFls
	CALL	LnsAX
	MOV	CX,AX
	MOV	AX,SI
	SUB	AX,DS:First
	MOV	BX,SIZE FilEnt
	XOR	DX,DX
	DIV	BX
	XOR	DX,DX
	DIV	CX
	XOR	CX,CX
	CMP	DS:BrfFul,CL
	JNE	FncSl6
	MOV	CL,2
FncSl6:	CMP	AX,CX
	JA	FncSl7
	CALL	BegBX
	ADD	BX,201h
	ADD	BH,DL
	MOV	DX,126h
	CALL	Window
FncSl7:	CMP	ES:MiniSta,0
	JZ	FncSl8
	CALL	BegBX
	CALL	SizDX
	ADD	BH,DH
	SUB	BH,2
	MOV	DX,128h
	CALL	Window
FncSl8:	POP	AX
	CALL	TstBrk
	JC	FncExt
	OR	AH,AH
	JNZ	FncExt
FncSl9:	ADD	SI,SIZE FilEnt
	JMP	FncSl3

FncExt:	CMP	BYTE PTR ES:[TmpBuf+91],0 ; Exit
	JE	FncEx2
FncEx1:	MOV	DS,ES:WCBcrn
	MOV	DS:ChgFlg,1
	CALL	InvWCB
	MOV	DS:ChgFlg,1
	JMP	Init50
FncEx2:	JMP	Func04

	.CONST
NoFnc_S	DB	"Can't find file^W",0

	.DATA
NoFnc_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 2
	DW	ErrBuf
	DW	TmpBuf
	DB	7, 64
	DW	TmpBuf+100
	DB	0, 50
	DW	Inc_S
	DW	NoFnc_S
	DW	Var_S
	DW	Ok_S
NoFnc_B	DW	NoFnc_P, 928h, 0
	DB	1, 0FFh, 0, 2
	DB	4
	DB	0, 0, 0, 0
	DW	426h, TmpBuf+100, OkMn_D
	DB	0

	.CODE
FncMas:	PUSH	ES			; Masked files
	POP	DS
	MOV	AL,0
	MOV	DS:ZoomOne,AL
	MOV	DS:[TmpBuf+91],AL
FncMs1:	CALL	SetDTA
	MOV	DX,OFFSET TmpBuf
	MOV	CX,37h
	MOV	AH,4Eh
	JMP	FncMs4
FncMs2:	TEST	DS:DTA.FilAttr,16h
	JNZ	FncMs3
	XOR	BX,BX
	MOV	SI,OFFSET TmpBuf
	CALL	CutPath
	MOV	DI,SI
	MOV	SI,OFFSET DTA.FilName
	MOV	CX,13
	REP	MOVSB
	CALL	TstBrk
	JC	FncMs8
	MOV	AL,0
	CALL	DS:FncAddr
	PUSH	ES
	POP	DS
	OR	DS:[TmpBuf+91],AL
	CMP	AH,0
	JNZ	FncMs8
FncMs3:	MOV	AH,4Fh
FncMs4:	CALL	ChgDisk
	JC	FncMs8
	CALL	Intr21
	JNC	FncMs2
	CMP	AX,12h
	JE	FncMs5
	CMP	AX,3
	JA	FncMs8
FncMs5:	OR	BX,BX
	JZ	FncMs8
	MOV	DX,BX
	MOV	BX,'s'
	MOV	SI,OFFSET TmpBuf
FncMs6:	LODSB
	CMP	AL,'*'
	JE	FncMs7
	CMP	AL,'?'
	JE	FncMs7
	CMP	AL,0
	JNE	FncMs6
	MOV	BL,0
FncMs7:	MOV	WORD PTR DS:[TmpBuf+100],BX
	MOV	BP,OFFSET NoFnc_B
	CALL	Dialog
FncMs8:	JMP	FncExt

DelTree	PROC	NEAR
	CMP	ES:ChgTree,0
	JZ	DelTr5
DelTr0:	PUSH	DS
	MOV	AX,ES:[SI]
	CMP	AH,':'
	JE	DelTr1
	MOV	AL,ES:[DosPth]
DelTr1:	MOV	DX,OFFSET TmpErr
	MOV	DI,DX
	MOV	AH,':'
	STOSW
	MOV	SI,OFFSET Tree_F
	MOV	CX,13+1
	PUSH	CS
	POP	DS
	REP	MOVSB
	PUSH	ES
	POP	DS
	MOV	DS:BlocEr,1
	MOV	AL,11h
	CALL	OpenFil
	JNC	DelTr3
	CMP	AX,5
	STC
	JNE	DelTr4
DelTr2:	CALL	Beep
	CLC
	JMP	DelTr4
DelTr3:	MOV	BX,AX
	MOV	SI,DX
	MOV	BYTE PTR DS:[SI],0
	MOV	CX,1
	MOV	AH,40h
	CALL	Intr21
	PUSHF
	PUSH	AX
	MOV	AH,3Eh
	CALL	Intr21
	POP	AX
	POP	BX
	JC	DelTr4
	PUSH	BX
	POPF
	JC	DelTr4
	CMP	AX,CX
	JNE	DelTr2
DelTr4:	MOV	DS:BlocEr,0
	POP	DS
DelTr5:	RET
DelTree	ENDP
