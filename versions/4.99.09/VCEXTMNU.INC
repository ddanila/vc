;---------------------------------------------------------------
;		Internal Viewer
;
;		Version:	19.06.2000
;---------------------------------------------------------------

ViewExecCursor	EQU	WORD PTR LineBuf1[0]
ViewExecEPB	EQU	BYTE PTR LineBuf1[2]
ViewExecCmdLn	EQU	BYTE PTR LineBuf2

		.CONST
ViewE_F		DB	"vcview.ext",0
EditE_F		DB	"vcedit.ext",0
View_S		DB	" View ",0
Edit_S		DB	" Edit ",0
View0_S		DB	"View",0
Edit0_S		DB	"Edit",0
EditDirInfo_S	DB	" Edit: ^r",0
Viewer_S	DB	"viewer",0
Editor_S	DB	"editor",0
ViewFile_S	DB	"^3C the file:",0FFh,LenStr-14,0
ViewEdit_S	DB	"Loading the ^2C:",0
RunEr_S		DB	"Not enough memory to load",0
Run1_S		DB	"Can't run commands for some reason.",0
Run2_S		DB	"Try rebooting.",0
NoFnd_S		DB	"Can't find the file",0

		.DATA
ViewExecParms	DB	0,'Socha',0
		DW	11Fh
ViewFile_P	DB	Box_C, BoxCu_C, 0, 0
		DW	Inc0_S
		DW	ViewFile_S
		DW	Empty_S
		DW	0
ViewFile_B	BOXHD	< ViewFile_P, 500h, 0, Dialog_K, Fls_M, Default_H, 0 >
		BOXMN	< 1, 0,0,0,0, 305h, SourceFile, LenStrFull+LenStr >
		DB	0
ViewEdit_P	DB	Box_C, 0, 0, 0
		DW	Inc0_S
		DW	ViewEdit_S
		DW	Line1_S
		DW	0
RunEr1_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Error_S
		DW	RunEr_S
		DW	Line1_S
		DW	Ok_S
		DW	0
RunEr1_B	BOXHD	< RunEr1_P, 900h, 0, Error_K, Def_M, None_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
RunEr2_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Error_S
		DW	Run1_S
		DW	Run2_S
		DW	Ok_S
		DW	0
RunEr2_B	BOXHD	< RunEr2_P, 900h, 0, Error_K, Def_M, None_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
FndEr_P		DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	NoFnd_S
		DW	Line1_S
		DW	Ok_S
		DW	0
FndEr_B		BOXHD	< FndEr_P, 900h, 0, Dialog_K, Def_M, Default_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
ViewFile_T	DW	View_S, TmpBuf, Viewer_S, View0_S
EditFile_T	DW	Edit_S, TmpBuf, Editor_S, Edit0_S
ReadView_T	DW	View_S, SourceFile, ExecArcProgram
ReadEdit_T	DW	Edit_S, SourceFile, ExecArcProgram

		.CODE
F43_00:		XOR	AX,AX			; View...
		JMP	F44_01

F44_00:		MOV	AX,1			; Edit...
F44_01:		PUSH	ES
		POP	DS
		MOV	DX,OFFSET ViewFile_T
		CMP	AL,0
		JZ	F44_02
		MOV	DX,OFFSET EditFile_T
F44_02:		PUSH	AX
		ADD	AL,View_H
		MOV	DS:HlpPage,AL
		MOV	BP,OFFSET ViewFile_B
		CALL	Dialog
		POP	AX
		JNE	F44_03
		CMP	DS:SourceFile,0
		JNE	F34_20
F44_03:		JMP	Func04

F63_00:		MOV	AX,100h			; Internal viewer
		JMP	F34_01

F64_00:		MOV	AX,101h			; Internal editor
		JMP	F34_01

F33_00:		XOR	AX,AX			; View
		JMP	F34_01

F34_00:		MOV	AX,1			; Edit
F34_01:		CMP	DS:Visible,0
		JZ	F44_01
		CMP	DS:WinTyp,p_File
		JNE	F34_03
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	F44_03
		TEST	DS:[SI].AttrF,18h
		JNZ	F44_03
		PUSH	AX
		MOV	DI,OFFSET LineBuf1
		PUSH	DI
		CMP	DS:ArcPnt,0
		JE	F34_02
		CALL	PutDescriptor
		CALL	AddFile
F34_02:		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CALL	NameOffset
		CALL	MvzLine
		POP	SI
		POP	AX
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	DI,OFFSET SourceFile
		MOV	CX,LenStrFull+2
		CALL	SaveHistory
		POP	DS
		JMP	F34_20
F34_03:		MOV	BP,DS			; Quick View
		CMP	DS:WinTyp,p_Info
		JB	F44_01			; File or Tree panel
		JE	F34_10
		CALL	InvWCB                  ; QuickView panel
		MOV	CL,DS:LongNames
		MOV	BX,ES:ViewSeg
		OR	BX,BX
		JE	F44_01
		MOV	DS,BX
		TEST	DS:ViewAttr,19h
		JNZ	F44_01
		MOV	SI,OFFSET ViewName2
		TEST	DS:ViewAttr,2
		JNZ	F34_04
		OR	CL,CL
		JNZ	F34_04
		MOV	SI,OFFSET ViewName1
F34_04:		MOV	DI,OFFSET SourceFile
		MOV	CX,LenStrFull+2
		CALL	SaveHistory
		MOV	DS,BP
		OR	AX,AX
		MOV	AH,0
		JNE	F34_20
		ADD	BX,(SIZE QuickViewData-1)/16+1
F34_05:		MOV	DS,BX
		MOV	AL,ES:Columns
		MOV	AH,ES:Lines
		SUB	AH,2
		MOV	DS:VwSize,AX
		MOV	DS:VwBase,100h
		CALL	HidCur
		JMP	F34_71
F34_10:		MOV	BX,ES:DirinfoSeg	; Create dirinfo
		OR	BX,BX
		JNE	F34_15
		CMP	AL,0
		JE	F44_01
		CALL	InvWCB
		CMP	DS:Visible,0
		JE	F34_11
		CMP	DS:WinTyp,p_File
		JNE	F34_11
		CMP	DS:ArcPnt,0
		JNE	F44_01
F34_11:		PUSH	ES
		POP	DS
		MOV	SI,OFFSET DirInfoFiles
		CALL	FndLet
		MOV	DI,SI
F34_12:		MOV	CX,SI
F34_13:		LODSB
		CMP	AL,0
		JE	F34_14
		CMP	AL,';'
		JE	F34_14
		CMP	AL,','
		JE	F34_14
		CMP	AL,'*'
		JE	F44_01
		CMP	AL,'?'
		JE	F44_01
		CMP	AL,' '
		JNE	F34_12
		JMP	F34_13
F34_14:		MOV	SI,DI
		SUB	CX,SI
		JBE	F44_01
		MOV	DI,OFFSET DirInfoFile
		REP	MOVSB
		MOV	AL,0
		STOSB
		MOV	AL,1
F34_15:		PUSH	AX
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET DirInfoFile
		MOV	AX,DS:DirInfoNumb
		CMP	AX,0FFFFh
		JE	F34_16
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		PUSH	SI
		MOV	SI,OFFSET VcTempArc_S
		CALL	MakePathName
		CALL	AddFile
		POP	SI
		CALL	MvzLine
		POP	SI
F34_16:		MOV	DI,OFFSET SourceFile
		MOV	CX,LenStrFull+2
		CALL	SaveHistory
		POP	AX
		MOV	DS,BP
		OR	AH,AH
		MOV	AH,0
		JNE	F34_20
		CMP	AL,0
		JE	F34_05
		CALL	SizDX			; Edit dirinfo
		SUB	DH,12
		JBE	F34_20
		DEC	DX
		DEC	DX
		CALL	BegBX
		ADD	BX,0B01h
		MOV	DS,CS:SubsSeg
		MOV	DI,OFFSET TmpBuf
		MOV	SI,OFFSET EditDirInfo_S
		PUSH	DI
		CALL	MvzLine
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET DirInfoFile
		CALL	MvzLine
		MOV	AX,' '
		STOSW
		POP	DI
		MOV	SI,DI
		CALL	StrLen
		DEC	CX
		MOV	AL,DL
		CMP	CX,AX
		JBE	F34_17
		ADD	DI,AX
		SUB	DI,4
		MOV	AL,'.'
		STOSB
		STOSB
		STOSB
		MOV	AX,' '
		STOSW
F34_17:		PUSH	BX
		DEC	BH
		MOV	AL,DL
		SHR	AL,1
		ADD	BL,AL
		MOV	BP,OFFSET PtIn_P
		CALL	CntLine
		POP	BX
		MOV	AL,Fls_M
		JMP	T00_02
F34_20:		MOV	BP,AX			; View in archive
		MOV	DX,OFFSET ReadView_T
		OR	AL,AL
		JZ	F34_21
		MOV	DX,OFFSET ReadEdit_T
F34_21:		MOV	SI,OFFSET SourceFile
		CALL	ViewArcExtract
		JNC	F34_22
		OR	AX,AX
		JE	Func04
		MOV	BP,AX
		CALL	Dialog
		JMP	Func04
F34_22:		MOV	ES:TempDirNumb,AX
		INC	AX
		OR	AL,AH
		MOV	BH,AL
		MOV	AX,BP			; Alternative viewer/editor
		MOV	DI,OFFSET AltView
		CMP	AL,0
		JZ	F34_31
		MOV	DI,OFFSET AltEdit
F34_31:		CMP	BYTE PTR ES:[DI],0
		JNZ	F34_32
		OR	AH,AH
		MOV	AH,0
		JNZ	F34_32
		MOV	AH,1
F34_32:		MOV	BL,AL
		OR	AH,AH
		JNZ	F34_34
		CALL	HidCur			; Load extension file
		MOV	AL,Fls_M
		CALL	PutMnu
		MOV	BP,OFFSET ViewE_F
		OR	BL,BL
		JZ	F34_33
		MOV	BP,OFFSET EditE_F
F34_33:		MOV	DS,CS:SubsSeg
		MOV	DX,OFFSET TmpBuf
		MOV	SI,BP
		MOV	DI,DX
		CALL	MvzLine
		PUSH	ES
		POP	DS
		CALL	ChgDisk
		JC	F34_36
		MOV	AL,40h
		CALL	OpenFil
		JNC	F34_37
		CMP	AL,2
		JNE	F34_36
		MOV	SI,BP
		MOV	DI,DX
		CALL	MainFil
		CALL	ChgDisk
		JNC	F34_35
F34_34:		OR	BL,BL
		JNZ	T00_00
F34_70:		MOV	AL,Fls_M
		CALL	PutMnu
		XOR	AX,AX
		MOV	DS,AX
		CALL	ViewLoad
		JC	Func04
F34_71:		MOV	SI,0FFFFh
		MOV	DI,SI
		CALL	ViewFile
		JMP	F34_60
F34_35:		MOV	AL,40h
		CALL	OpenFil
		JNC	F34_37
		CMP	AL,3
		JBE	F34_34
F34_36:		JMP	Func04
F34_37:		MOV	DI,BX
		MOV	BX,AX
		MOV	AX,ES
		ADD	AX,ES:SizeDataSeg
		SUB	AX,ES:WorkBuffer
		CMP	AX,0FFFh
		JBE	F34_38
		MOV	AX,0FFFh
F34_38:		MOV	CL,4
		SHL	AX,CL
		DEC	AX
		MOV	CX,AX
		MOV	DS,ES:WorkBuffer
		XOR	DX,DX
		MOV	AH,3Fh
		CALL	Intr21
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	BX
		POP	CX
		JC	F34_36
		PUSH	CX
		POPF
		JC	F34_36
		MOV	BYTE PTR DS:[BX],1Ah
		MOV	BX,DI
		PUSH	DS			; Test extension file
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET SourceFile
		CALL	CutPath
		MOV	DI,SI
		POP	DS
		MOV	AL,0
		XOR	SI,SI
		CALL	FindExt
		JC	F34_34
		MOV	ES:MenuViewArc,BH
		MOV	ES:MenuViewEdit,OFFSET ViewFile_T
		OR	BL,BL
		JE	F32_30
		MOV	ES:MenuViewEdit,OFFSET EditFile_T
		JMP	F32_30

F34_40:		PUSH	ES			; Check standard viewer
		POP	DS
		CMP	AL,0
		JE	F34_41
		MOV	AX,4300h
		CALL	Intr21
		JC	F34_44
		TEST	CL,10h
		JNZ	F34_34
		JMP	F34_50
F34_41:		MOV	AX,4300h
		CALL	Intr21
		JNC	F34_42
		CMP	AX,53h
		JNE	F34_43
		JMP	Func04
F34_42:		TEST	CL,10h
		JZ	F34_50
F34_43:		MOV	SI,DX
		MOV	DI,DX
		CALL	CutPath
		CALL	MvzLine
		MOV	SI,DX
		CALL	FindPath
		JNC	F34_50
F34_44:		CMP	AX,53h
		JNE	F34_34
		JMP	Func04

F34_50:		MOV	DX,DS:MenuViewEdit	; Execute standard viewer
		MOV	BP,OFFSET ViewEdit_P
		MOV	BX,500h
		CALL	DialBox
		CALL	Window
		MOV	BH,0
		MOV	AH,3
		INT	10h
		MOV	DS:ViewExecCursor,CX
		MOV	SI,OFFSET TmpBuf
		MOV	DI,OFFSET LineBuf4
		CALL	ShortName
		JNC	F34_51
		CMP	AX,53h
		JE	Func04
F34_51:		MOV	SI,OFFSET SourceFile
		MOV	DI,OFFSET LineBuf3
		PUSH	DI
		CMP	DS:ArcPathName,0
		JE	F34_52
		PUSH	SI
		MOV	AX,DS:TempDirNumb
		MOV	SI,OFFSET VcTempArc_S
		CALL	MakePathName
		CALL	AddFile
		POP	SI
		CALL	CutPath
F34_52:		CALL	MvzLine
		POP	SI
		MOV	DI,OFFSET ViewExecCmdLn+1
		CALL	ShortName
		JNC	F34_53
		CMP	AX,53h
		JE	Func04
F34_53:		CALL	StrLen
		DEC	CX
		MOV	AX,126-45
		CMP	AX,CX
		JBE	F34_54
		MOV	AX,CX
F34_54:		ADD	DI,AX
		ADD	AL,45
		MOV	DS:ViewExecCmdLn[0],AL
		PUSH	DS
		MOV	DS,CS:SubsSeg
		MOV	SI,OFFSET ViewExecParms
		MOV	CX,9
		PUSH	DI
		REP	MOVSB
		MOV	CL,36
		MOV	AL,0
		REP	STOSB
		MOV	AL,0Dh
		STOSB
		POP	DI
		POP	DS
		MOV	BYTE PTR DS:[DI+15],0
		MOV	AL,DS:ColTyp
		MOV	DS:[DI+9],AL
		CMP	DS:Mouse,0
		JE	F34_55
		MOV	BYTE PTR DS:[DI+11],0FFh
		MOV	AL,DS:LftMous
		MOV	DS:[DI+12],AL
		MOV	BYTE PTR DS:[DI+13],1
F34_55:		MOV	BX,DS:WorkBuffer
		MOV	AX,ES
		SUB	BX,AX
		ADD	BX,(MinView SHR 4)+1
		MOV	AH,4Ah
		INT	21h
		MOV	BX,OFFSET ViewExecEPB
		MOV	DS:[BX].EnvSeg,0
		MOV	DS:[BX].CmdLine.Offs,OFFSET ViewExecCmdLn
		MOV	DS:[BX].CmdLine.Segm,ES
		MOV	DS:[BX].FCB1.Offs,5Ch
		MOV	DS:[BX].FCB1.Segm,ES
		MOV	DS:[BX].FCB2.Offs,6Ch
		MOV	DS:[BX].FCB2.Segm,ES
		MOV	DX,OFFSET LineBuf4
		MOV	AX,4B00h
		CALL	Intr21
		PUSHF
		PUSH	AX
		MOV	BX,0FFFFh
		MOV	AH,4Ah
		INT	21h
		MOV	AH,4Ah
		INT	21h
		CALL	HidCur
		PUSH	ES
		POP	DS
		CALL	SavMous
		CALL	IniMous
		POP	AX
		POPF
		JNC	F34_57
		CMP	AX,53h
		JE	F34_57
		MOV	DX,OFFSET ViewFile_T
		MOV	BP,OFFSET RunEr1_B
		CMP	AX,8
		JE	F34_56
		CMP	AX,7
		JE	F34_56
		MOV	BP,OFFSET RunEr2_B
F34_56:		CALL	Dialog
F34_57:		CALL	ResWin
		MOV	DS,ES:WCB1seg
		MOV	DS:ChgFlg,1
		MOV	DS,ES:WCB2seg
		MOV	DS:ChgFlg,1
		PUSH	ES			; Restore screen
		POP	DS
		MOV	DL,25
		CMP	DS:TstEGA,0
		JZ	F34_58
		PUSH	ES
		MOV	BH,0
		MOV	AX,1130h
		INT	10h
		INC	DX
		POP	ES
F34_58:		MOV	AH,0Fh
		INT	10h
		CMP	AL,DS:ScrMod
		JNE	F34_63
		CMP	AH,DS:Columns
		JNE	F34_63
		CMP	DL,DS:Lines
		JNE	F34_63
		CMP	BH,DS:ScrPage
		JNE	F34_63
		MOV	CX,DS:ViewExecCursor
		MOV	AH,1
		INT	10h
F34_60:		XOR	BX,BX
		MOV	DL,ES:Columns
		MOV	DH,ES:Lines
		CALL	ResUser
		MOV	DS,ES:WCB1seg
		CMP	DS:Visible,0
		JZ	F34_61
		CALL	Panel
		CALL	PutPath
		CALL	PutFls
F34_61:		MOV	DS,ES:WCB2seg
		CMP	DS:Visible,0
		JZ	F34_62
		CALL	Panel
		CALL	PutPath
		CALL	PutFls
F34_62:		CALL	PutPrompt
		MOV	AL,None_M
		CALL	PutMnu
		MOV	AL,Empty_K
		CALL	PutKey
		CALL	Window
		PUSH	ES
		POP	DS
		JMP	Panels00
F34_63:		CALL	SavMous
		MOV	AL,0
		CALL	CurMous
		MOV	SI,1
		JMP	F69_10

MenuViewEdit	EQU	WORD PTR LineBuf1[0]
MenuIndex	EQU	WORD PTR LineBuf1[2]
MenuHotWidth	EQU	BYTE PTR LineBuf1[4]
MenuViewArc	EQU	BYTE PTR LineBuf1[5]

		.CONST
Menu_F		DB	'VC.MNU',0
User_S		DB	' User Menu ',0
UserErr_S	DB	'Could not find the menu file',0

		.DATA
User_P		DB	Hist_C, HstCu_C, 0, 0
		DW	Inc0_S
		DW	0FFFEh
		DW	0
User_B		BOXHD	< User_P, 8500h, DialogList, Dialog_K, Def_M, Menu_H, 0 >
		DB	0
UserErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Inc0_S
		DW	UserErr_S
		DW	Line1_S
		DW	Ok_S
		DW	0
UserErr_B	BOXHD	< UserErr_P, 500h, 0, Dialog_K, Def_M, Menu_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
User_T		DW	User_S, TmpBuf

		.CODE	Subs
MenuLine	PROC	NEAR
		USES	AX,CX,SI,DI,DS,ES
		MOV	AX,SI
		SUB	AX,DI
		ADD	BX,DS:ListBase
		ADD	BH,AL
		MOV	DL,DS:MenuHotWidth
		MOV	DH,DS:ListWindow.X
		MOV	DI,DS:MenuIndex
		INC	SI
		ADD	SI,SI
		ADD	SI,SI
		SUB	DI,SI
		MOV	DS,DS:WorkBuffer
		MOV	SI,DS:[DI+2]
		CALL	BasAdr
		PUSH	DI			; Clear hot key
		MOV	CX,4
		MOV	AL,' '
  @@ClearHot:	STOSB
		INC	DI
		LOOP	@@ClearHot
		POP	DI
		PUSH	DI			; Write hot key
		MOV	AX,DS:[SI]
		CMP	AL,7Fh
		JE	@@PutLine
		CMP	AL,0FFh
		JE	@@PutLine
		CALL	UpCase
		CMP	AH,':'
		JNE	@@ChkFnn
		CMP	AL,' '
		JAE	@@WriteHot
		MOV	BYTE PTR ES:[DI],'^'
		ADD	AL,40h
		MOV	ES:[DI+2],AL
		INC	SI
		INC	SI
		JMP	@@Space
  @@ChkFnn:	CMP	AL,'F'
		JNE	@@PutLine
		CMP	AH,'1'
		JB	@@PutLine
		CMP	AH,'9'
		JA	@@PutLine
		MOV	CX,DS:[SI+2]
		CMP	CL,':'
		JE	@@WriteHot
		CMP	AH,'1'
		JNE	@@PutLine
		CMP	CH,':'
		JNE	@@PutLine
		CMP	CL,'2'
		JA	@@PutLine
		CMP	CL,'0'
		JB	@@PutLine
  @@WriteHot:	LODSB
		CMP	AL,':'
		JE	@@Space
		CALL	UpCase
		STOSB
		INC	DI
		JMP	@@WriteHot
  @@Space:	LODSB
		CMP	AL,' '
		JE	@@Space
		CMP	AL,9
		JE	@@Space
		DEC	SI
  @@PutLine:	POP	DI
		MOV	CL,DH
		MOV	DH,0
		ADD	DI,DX
		ADD	DI,DX
		MOV	AH,CS:[BP]
		CALL	Color
		PUSH	CX
		SUB	CL,DL
		MOV	CH,0
		DEC	CX
		DEC	CX
		POP	DX
		MOV	AL,' '
		STOSW
  @@GetChar:	MOV	DH,AH
		LODSB
		CMP	AL,13
		JE	@@ClrEnd
		CMP	AL,10
		JE	@@ClrEnd
		CMP	AL,1Ah
		JNE	@@PutChar
  @@ClrEnd:	DEC	SI
		MOV	AL,' '
  @@PutChar:	MOV	AH,DH
		STOSW
		LOOP	@@GetChar
		MOV	DH,1
		MOV	AL,' '
		STOSW
		CLC
		RET
MenuLine	ENDP

MenuHotKey	PROC	NEAR
		USES	DS
		CALL	UpCase
		MOV	DI,DS:MenuIndex
		MOV	CX,DS:ListTotal
		MOV	DS,DS:WorkBuffer
		XOR	SI,SI
  @@ChkKey:	SUB	DI,4
		MOV	DX,DS:[DI]
		CMP	AL,DL
		JNE	@@Next
		CMP	AL,0
		JNE	@@Found
		CMP	AX,DX
		JE	@@Found
  @@Next:	INC	SI
		LOOP	@@ChkKey
		JMP	@@Exit
  @@Found:	MOV	ES:ListCursor,SI
		MOV	AX,1C0Dh
  @@Exit:	STC
		RET
MenuHotKey	ENDP

		.CODE
F42_00:		CALL	HidCur			; Shift-F2 - main user menu
		MOV	AL,Fls_M
		CALL	PutMnu
		MOV	ES:MenuViewEdit,OFFSET User_T
		JMP	F32_01

F32_00:		CALL	HidCur			; F2 - user menu
		MOV	AL,Fls_M
		CALL	PutMnu
		MOV	ES:MenuViewEdit,OFFSET User_T
		MOV	DS,CS:SubsSeg		; Reading
		MOV	SI,OFFSET Menu_F
		MOV	DX,OFFSET TmpBuf
		MOV	DI,DX
		CALL	MvzLine
		PUSH	ES
		POP	DS
		CALL	ChgDisk
		JC	F32_04
		MOV	AL,40h
		CALL	OpenFil
		JNC	F32_05
		CMP	AL,2
		JE	F32_01
		CMP	AL,5
		JNE	F32_04
F32_01:		MOV	SI,OFFSET Menu_F
		MOV	DI,OFFSET TmpBuf
		CALL	MainFil
F32_02:		PUSH	ES
		POP	DS
		MOV	DX,OFFSET TmpBuf
		CALL	ChgDisk
		JC	F32_04
		MOV	AL,40h
		CALL	OpenFil
		JNC	F32_05
		CMP	AL,3
		JA	F32_04
F32_03:		MOV	DX,ES:MenuViewEdit
		MOV	BP,OFFSET UserErr_B
		CALL	Dialog
F32_04:		JMP	Func04
F32_05:		PUSH	ES
		POP	DS
		MOV	BX,AX
		MOV	AX,ES
		ADD	AX,ES:SizeDataSeg
		SUB	AX,ES:WorkBuffer
		CMP	AX,0FFFh
		JBE	F32_06
		MOV	AX,0FFFh
F32_06:		MOV	CL,4
		SHL	AX,CL
		DEC	AX
		MOV	CX,AX
		MOV	DS,ES:WorkBuffer
		XOR	DX,DX
		MOV	AH,3Fh
		CALL	Intr21
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	BX
		POP	DX
		JC	F32_04
		PUSH	DX
		POPF
		JC	F32_04
		MOV	BYTE PTR DS:[BX],1Ah
		XOR	SI,SI
		CALL	FndItem
		JC	F32_03
		XOR	BP,BP			; Check menu
		MOV	BL,0
		MOV	DI,CX
		INC	DI
		MOV	ES:MenuIndex,DI
		PUSH	DI
F32_10:		LEA	AX,[DI-5]
		CMP	SI,AX
		JAE	F32_18
		DEC	DI
		DEC	DI
		MOV	DS:[DI],SI
		DEC	DI
		DEC	DI
		MOV	BYTE PTR DS:[DI-1],1Ah
		XOR	CX,CX
		MOV	DS:[DI],CX
		MOV	AX,DS:[SI]
		CMP	AL,7Fh
		JE	F32_14
		CMP	AL,0FFh
		JE	F32_14
		CALL	UpCase
		MOV	BH,2+1
		CMP	AH,':'
		JNE	F32_11
		MOV	CH,3
		CMP	AL,0
		JE	F32_12
		MOV	CL,AL
		CMP	AL,' '
		JB	F32_12
		MOV	BH,1+1
		JMP	F32_12
F32_11:		CMP	AL,'F'
		JNE	F32_14
		CMP	AH,'1'
		JB	F32_14
		CMP	AH,'9'
		JA	F32_14
		MOV	CH,AH
		ADD	CH,3Bh-'1'
		MOV	DX,DS:[SI+2]
		CMP	DL,':'
		JE	F32_12
		CMP	AH,'1'
		JNE	F32_14
		CMP	DH,':'
		JNE	F32_14
		CMP	DL,'2'
		JA	F32_14
		MOV	BH,3+1
		MOV	CH,44h
		CMP	DL,'0'
		JE	F32_12
		JB	F32_14
		MOV	CH,DL
		ADD	CH,85h-'1'
F32_12:		LODSB
		CMP	AL,':'
		JNE	F32_12
F32_13:		LODSB
		CMP	AL,' '
		JE	F32_13
		CMP	AL,9
		JE	F32_13
		DEC	SI
		MOV	DS:[DI],CX
		CMP	BL,BH
		JAE	F32_14
		MOV	BL,BH
F32_14:		MOV	DX,SI
F32_15:		LODSB
		CMP	AL,13
		JE	F32_16
		CMP	AL,10
		JE	F32_16
		CMP	AL,1Ah
		JNE	F32_15
F32_16:		DEC	SI
		MOV	AX,SI
		SUB	AX,DX
		CMP	BP,AX
		JAE	F32_17
		MOV	BP,AX
F32_17:		CALL	FndItem
		JNC	F32_10
F32_18:		PUSH	ES
		POP	DS
		POP	AX
		SUB	AX,DI
		SHR	AX,1
		SHR	AX,1
		MOV	DS:ListTotal,AX
		MOV	DS:MenuHotWidth,BL
		MOV	BH,0
		ADD	BP,BX
		INC	BP
		INC	BP
		MOV	BL,DS:Lines
		SUB	BL,5+4
		CMP	AX,BX
		JBE	F32_19
		MOV	AX,BX
F32_19:		MOV	DS:ListWindow.Y,AL
		MOV	AL,DS:Columns
		SUB	AL,12+3
		CMP	AX,BP
		JBE	F32_20
		MOV	AX,BP
F32_20:		CMP	AL,21
		JAE	F32_21
		MOV	AL,21
F32_21:		MOV	DS:ListWindow.X,AL
		MOV	DS:ListBase,205h
		MOV	DS:ListCursor,0
		MOV	DS:ListFirst,0
		MOV	DS:ListLine,OFFSET MenuLine
		MOV	DS:ListHotKey,OFFSET MenuHotKey
		MOV	DX,DS:MenuViewEdit	; Dialog
		MOV	BP,OFFSET User_B
		CALL	Dialog
		JNZ	F32_25
		MOV	DI,DS:MenuIndex		; Command
		MOV	AX,DS:ListCursor
		MOV	DS,DS:WorkBuffer
		INC	AX
		ADD	AX,AX
		ADD	AX,AX
		SUB	DI,AX
		MOV	SI,DS:[DI+2]
F32_22:		LODSB
		CMP	AL,13
		JE	F32_23
		CMP	AL,10
		JE	F32_23
		CMP	AL,1Ah
		JNE	F32_22
F32_23:		DEC	SI
		CALL	FindCommand
		JNC	F32_30
		CMP	ES:MenuViewEdit,OFFSET RunExt_T ; Empty menu
		JNE	F32_24
		MOV	DS,ES:WCBcrn
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CALL	NameOffset
		JMP	F21_30
F32_24:		CMP	ES:MenuViewEdit,OFFSET User_T
		JNE	F32_26
		MOV	AL,ES:AutoMnu
		MOV	ES:MenuFl,AL
F32_25:		JMP	Func04
F32_26:		CMP	ES:MenuViewEdit,OFFSET ViewFile_T
		JE	F34_70
		JMP	T00_00
F32_30:		MOV	BX,OFFSET Menu_F+1	; Check menu file
		MOV	CX,4
		CALL	SingleLine
		JC	F32_02
		MOV	AL,0
		CMP	ES:MenuViewEdit,OFFSET User_T
		JNE	F32_31
		MOV	AL,ES:AutoMnu
		MOV	ES:MenuFl,AL
		JMP	Run_00
F32_31:		CMP	ES:MenuViewEdit,OFFSET RunExt_T
		JE	F32_32
		MOV	BX,OFFSET ViewE_F+5
		MOV	CX,1
		CALL	SingleLine
		JC	F32_33
		MOV	AL,ES:MenuViewArc
		INC	AX
F32_32:		MOV	ES:RestoreMode,1
		JMP	Run_00
F32_33:		MOV	BL,0
		CMP	ES:MenuViewEdit,OFFSET ViewFile_T
		JE	F34_40
		MOV	BL,1
		JMP	F34_40
