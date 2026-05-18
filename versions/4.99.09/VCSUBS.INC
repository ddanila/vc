;---------------------------------------------------------------
;		Subroutines
;
;		Version:	16.07.2000
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Begin of panel
;		   DS - WCBseg
;	Return:	BH,BL - begin
;---------------------------------------------------------------
		.CODE	Subs
BegBX		PROC	FAR
		MOV	BX,DS
		CMP	BX,ES:WCB1seg
		MOV	BX,0
		JE	@@Menu
		MOV	BL,ES:Columns
  		SHR	BL,1
  @@Menu:	CMP	ES:MenuVs,BH
		JZ	@@Exit
		INC	BH
  @@Exit:	RET
BegBX		ENDP

;---------------------------------------------------------------
;	Size of main windows
;	Return:	DH,DL - size
;---------------------------------------------------------------
SizDX		PROC	FAR
		USES	AX
		MOV	DH,ES:Lines
		SUB	DH,2
		CMP	ES:FulScr,0
		JNZ	@@Menu
		MOV	AL,ES:PanelSize
		MUL	DH
		MOV	DH,AH
		CMP	DH,7
		JAE	@@Menu
		MOV	DH,7
  @@Menu:	CMP	ES:MenuVs,0
		JZ	@@Width
		DEC	DH
  @@Width:	MOV	DL,ES:Columns
  		SHR	DL,1
		RET
SizDX		ENDP

;---------------------------------------------------------------
;	Files in main windows
;	Return:	   AX - lines on file panel
;---------------------------------------------------------------
LnsAX		PROC	FAR
		USES	DX
		CALL	SizDX
		MOV	AL,DH
		CBW
		SUB	AL,3
		CMP	ES:MiniSta,AH
		JZ	@@Exit
		SUB	AL,2
  @@Exit:	RET
LnsAX		ENDP

;---------------------------------------------------------------
;	File name width
;	Return:	   AL - file name width
;---------------------------------------------------------------
NameWidthAL	PROC	FAR
		USES	DX
		MOV	AL,12
		CMP	DS:LongNames,0
		JZ	@@Exit
		MOV	AL,DS:NameWidth
		CALL	SizDX
		DEC	DX
		DEC	DX
		CMP	AL,DL
		JBE	@@Exit
		MOV	AL,DL
  @@Exit:	RET
NameWidthAL	ENDP

;---------------------------------------------------------------
;	Output key bar
;		   AL - Case
;---------------------------------------------------------------
Error_K		EQU	0
Dialog_K	EQU	1
MainNone_K	EQU	2
MainFile_K	EQU	4
MainDirInfo_K	EQU	6
MainView_K	EQU	10
ViewFile_K	EQU	18
Edit_K		EQU	30
AltSearch_K	EQU	33
Table_K		EQU	34
Tree_K		EQU	35
CopyMove_K	EQU	36
FindFile1_K	EQU	37
FindFile2_K	EQU	38
Empty_K		EQU	0FFh

b_Help		EQU	1
b_Menu		EQU	2
bView		EQU	3
bAView		EQU	4
bEdit		EQU	5
bAEdit		EQU	6
bCopy		EQU	7
bRenMov		EQU	8
bMkDir		EQU	9
bDelete		EQU	10
bPullDn		EQU	11
bQuit		EQU	12
bMain		EQU	13
bSavSet		EQU	14
bLeft		EQU	15
bRight		EQU	16
bLabel		EQU	17
bMkArc		EQU	18
bMode		EQU	19
bMemory		EQU	20
bDirSiz		EQU	21
bFind		EQU	22
bHistory	EQU	23
bEGALn		EQU	24
bTree		EQU	25
bWrap		EQU	26
bUnwrap		EQU	27
bZoom		EQU	28
bUnzoom		EQU	29
bText		EQU	30
bHex		EQU	31
bGoto		EQU	32
bSearch		EQU	33
bNext		EQU	34
bPrev		EQU	35
bTable		EQU	36
bShift		EQU	37
bSave		EQU	38
bASave		EQU	39
bQSave		EQU	40
bRescan		EQU	41

KbQuit		EQU	1
KbDialog	EQU	2
KbMain		EQU	3
KbMain_shift	EQU	4
KbMain_ctrl	EQU	5
KbQView_alt_cga	EQU	6
KbQView_alt	EQU	7
KbMain_alt_cga	EQU	8
KbMain_alt	EQU	9
KbInfo_dir	EQU	10
KbQView_shift	EQU	11
KbQView_shiftX  EQU	12
KbQView		EQU	13
KbQView_hex	EQU	14
KbQView_shift2  EQU	15
KbQView_shiftX2	EQU	16
KbQView_ctrl	EQU	17
KbQView_hex_zm	EQU	18
KbQVw_unwrp_zm	EQU	19
KbQView_zm	EQU	20
KbView_hex	EQU	21
KbView_unwrp	EQU	22
KbView		EQU	23
KbView_shift	EQU	24
KbView_shiftX	EQU	25
KbView_ctrl	EQU	26
KbEditX		EQU	27
KbEdit		EQU	28
KbEdit_hex	EQU	29
KbEdit_alt	EQU	30
KbTable		EQU	31
KbTree		EQU	32
KbCopyMov	EQU	33
KbCopyMov_alt	EQU	34
KbFind		EQU	35
KbFind_res	EQU	36
KbFind_res_alt	EQU	37

NmbKeyBar	EQU	10
LenKeyText	EQU	6
LenKeyCell	EQU	8		; F-key number + label + separator

		.DATA
; prefixes: Kbs - keybar set, Kb - keybar, b - button
; keybars with X in their prefix are unused
;
; KbsInfo_dir is keybar set for the active Info panel if dirinfo file present
; KbsMainAltSrch is for AltSearch feature in the main panel
; KbsFind is for Find Files request dialog
; KbsFind_res is for Find Files results panel
; KbsTable is used in Viewer, Editor(disabled) and Find Files request dialog
; KbsQView*_zm are for zoomed in QuickView
; KbsEdit is for the internal editor(WIP / disabled)

KeyB_T		DB KbQuit        ,0              ,0           ,0		; 0 (KbsError)
KbsDialog	DB KbDialog      ,0              ,0           ,0		; 1
KbsInfo_cga	DB KbMain        ,KbMain_shift   ,KbMain_ctrl ,KbQView_alt_cga	; 2
KbsInfo		DB KbMain        ,KbMain_shift   ,KbMain_ctrl ,KbQView_alt	; 3
KbsMain_cga	DB KbMain        ,KbMain_shift   ,KbMain_ctrl ,KbMain_alt_cga	; 4
KbsMain		DB KbMain        ,KbMain_shift   ,KbMain_ctrl ,KbMain_alt	; 5
KbsInfo_dir_cga	DB KbInfo_dir    ,KbQView_shift  ,KbQView_ctrl,KbQView_alt_cga	; 6
KbsInfo_dir	DB KbInfo_dir    ,KbQView_shift  ,KbQView_ctrl,KbQView_alt	; 7
		DB KbInfo_dir    ,KbQView_shiftX ,KbQView_ctrl,KbQView_alt_cga	; 8
		DB KbInfo_dir    ,KbQView_shiftX ,KbQView_ctrl,KbQView_alt	; 9
KbsQView_cga	DB KbQView       ,KbQView_shift2 ,KbQView_ctrl,KbQView_alt_cga	; 10
KbsQView	DB KbQView       ,KbQView_shift2 ,KbQView_ctrl,KbQView_alt	; 11
		DB KbQView       ,KbQView_shiftX2,KbQView_ctrl,KbQView_alt_cga	; 12
		DB KbQView       ,KbQView_shiftX2,KbQView_ctrl,KbQView_alt	; 13
KbsQVw_hex_cga	DB KbQView_hex   ,KbQView_shift2 ,KbQView_ctrl,KbQView_alt_cga	; 14
KbsQView_hex	DB KbQView_hex   ,KbQView_shift2 ,KbQView_ctrl,KbQView_alt	; 15
		DB KbQView_hex   ,KbQView_shiftX2,KbQView_ctrl,KbQView_alt_cga	; 16
		DB KbQView_hex   ,KbQView_shiftX2,KbQView_ctrl,KbQView_alt	; 17
KbsQView_hex_zm	DB KbQView_hex_zm,KbView_shift   ,KbView_ctrl ,0		; 18
KbsQVw_unwrp_zm	DB KbQVw_unwrp_zm,KbView_shift   ,KbView_ctrl ,0		; 19
KbsQView_zm	DB KbQView_zm    ,KbView_shift   ,KbView_ctrl ,0		; 20
		DB KbQView_hex_zm,KbView_shiftX  ,KbView_ctrl ,0		; 21
		DB KbQView_hex_zm,KbView_shiftX  ,KbView_ctrl ,0		; 22
		DB KbQView_zm    ,KbView_shiftX  ,KbView_ctrl ,0		; 23
KbsView_hex	DB KbView_hex    ,KbView_shift   ,KbView_ctrl ,0		; 24
KbsView_unwrp	DB KbView_unwrp  ,KbView_shift   ,KbView_ctrl ,0		; 25
KbsView		DB KbView        ,KbView_shift   ,KbView_ctrl ,0		; 26
		DB KbView_hex    ,KbView_shiftX  ,KbView_ctrl ,0		; 27
		DB KbView_unwrp  ,KbView_shiftX  ,KbView_ctrl ,0		; 28
		DB KbView        ,KbView_shiftX  ,KbView_ctrl ,0		; 29
KbsEditX	DB KbEditX       ,KbEdit_alt     ,0           ,0		; 30
		DB KbEdit        ,KbEdit_alt     ,0           ,0		; 31
KbsEdit_hex	DB KbEdit_hex    ,KbEdit_alt     ,0           ,0		; 32
KbsMainAltSrch	DB KbMain        ,KbMain         ,KbMain      ,KbMain		; 33
KbsTable	DB KbTable       ,0              ,0           ,0		; 34
KbsTree		DB KbTree        ,0              ,0           ,0		; 35
KbsCopyMov	DB KbCopyMov     ,0              ,0           ,KbCopyMov_alt	; 36
KbsFind		DB KbFind        ,0              ,0           ,0		; 37
KbsFind_res	DB KbFind_res    ,0              ,0           ,KbFind_res_alt	; 38
LenKeyB = $ - KeyB_T

KeyC_T	DB 0     ,0      ,0      ,0     ,0      ,0      ,0      ,0       ,0      ,0	; 0
	DB 0     ,0      ,0      ,0     ,0      ,0      ,0      ,0       ,0      ,bQuit	; KbQuit
	DB b_Help,0      ,0      ,0     ,0      ,0      ,0      ,0       ,0      ,bQuit	; KbDialog
	DB b_Help,b_Menu ,bView  ,bEdit ,bCopy  ,bRenMov,bMkDir ,bDelete ,bPullDn,bQuit	; KbMain
	DB 0     ,bMain  ,bView  ,bEdit ,bCopy  ,bRenMov,bMkDir ,bDelete ,bSavSet,bPullDn ; KbMain_shift
	DB bLeft ,bRight ,0      ,bLabel,bCopy  ,bRenMov,bMkArc ,bDelete ,bMode  ,0	; KbMain_ctrl
	DB bLeft ,bRight ,bAView ,bAEdit,bMemory,0      ,bFind  ,bHistory,0      ,bTree	; KbQView_alt_cga
	DB bLeft ,bRight ,bAView ,bAEdit,bMemory,0      ,bFind  ,bHistory,bEGALn ,bTree	; KbQView_alt
	DB bLeft ,bRight ,bAView ,bAEdit,bMemory,bDirSiz,bFind  ,bHistory,0      ,bTree	; KbMain_alt_cga
	DB bLeft ,bRight ,bAView ,bAEdit,bMemory,bDirSiz,bFind  ,bHistory,bEGALn ,bTree	; KbMain_alt
	DB b_Help,b_Menu ,bZoom  ,bEdit ,0      ,0      ,bSearch,bTable  ,bPullDn,bQuit	; KbInfo_dir
	DB 0     ,bMain  ,bView  ,bEdit ,0      ,0      ,bNext  ,0       ,bSavSet,bPullDn ; KbQView_shift
	DB 0     ,bMain  ,bView  ,bEdit ,0      ,0      ,bNext  ,bShift  ,bSavSet,bPullDn ; KbQView_shiftX
	DB b_Help,b_Menu ,bZoom  ,bHex  ,0      ,0      ,bSearch,bTable  ,bPullDn,bQuit	; KbQView
	DB b_Help,b_Menu ,bZoom  ,bText ,bGoto  ,0      ,bSearch,bTable  ,bPullDn,bQuit	; KbQView_hex
	DB 0     ,bMain  ,bView  ,bEdit ,0      ,0      ,bNext  ,0       ,bSavSet,bPullDn ; KbQView_shift2 == KbQView_shift
	DB 0     ,bMain  ,bView  ,bEdit ,0      ,0      ,bNext  ,bShift  ,bSavSet,bPullDn ; KbQView_shiftX2 == KbQView_shiftX
	DB bLeft ,bRight ,0      ,bLabel,0      ,0      ,bPrev  ,0       ,bMode  ,0	; KbQView_ctrl
	DB b_Help,0      ,bUnzoom,bText ,bGoto  ,0      ,bSearch,bTable  ,0      ,bQuit	; KbQView_hex_zm
	DB b_Help,bWrap  ,bUnzoom,bHex  ,0      ,0      ,bSearch,bTable  ,0      ,bQuit	; KbQVw_unwrp_zm
	DB b_Help,bUnwrap,bUnzoom,bHex  ,0      ,0      ,bSearch,bTable  ,0      ,bQuit	; KbQView_zm
	DB b_Help,0      ,bQuit  ,bText ,bGoto  ,0      ,bSearch,bTable  ,0      ,bQuit	; KbView_hex
	DB b_Help,bWrap  ,bQuit  ,bHex  ,0      ,0      ,bSearch,bTable  ,0      ,bQuit	; KbView_unwrp
	DB b_Help,bUnwrap,bQuit  ,bHex  ,0      ,0      ,bSearch,bTable  ,0      ,bQuit	; KbView
	DB 0     ,0      ,0      ,0     ,0      ,0      ,bNext  ,0       ,0      ,0	; KbView_shift
	DB 0     ,0      ,0      ,0     ,0      ,0      ,bNext  ,bShift  ,0      ,0	; KbView_shiftX
	DB 0     ,0      ,0      ,0     ,0      ,0      ,bPrev  ,0       ,0      ,0	; KbView_ctrl
	DB b_Help,bSave  ,0      ,0     ,0      ,0      ,bSearch,0       ,0      ,bQuit	; KbEditX
	DB b_Help,bSave  ,0      ,bHex  ,0      ,0      ,bSearch,0       ,0      ,bQuit	; KbEdit
	DB b_Help,bSave  ,0      ,bText ,0      ,0      ,bSearch,0       ,0      ,bQuit	; KbEdit_hex
	DB 0     ,bASave ,0      ,0     ,0      ,0      ,bNext  ,0       ,0      ,bQSave ; KbEdit_alt
	DB b_Help,0      ,0      ,0     ,0      ,0      ,0      ,bNext   ,0      ,bQuit	; KbTable
	DB b_Help,bRescan,0      ,0     ,0      ,0      ,0      ,0       ,0      ,bQuit	; KbTree
	DB b_Help,0      ,0      ,0     ,0      ,0      ,0      ,0       ,0      ,bTree	; KbCopyMov
	DB 0     ,0      ,0      ,0     ,0      ,0      ,0      ,0       ,0      ,bTree	; KbCopyMov_alt
	DB b_Help,0      ,0      ,0     ,0      ,0      ,0      ,bTable  ,0      ,bQuit	; KbFind
	DB b_Help,0      ,bView  ,0     ,bGoto  ,0      ,0      ,0       ,0      ,bQuit	; KbFind_res
	DB 0     ,0      ,bAView ,0     ,0      ,0      ,0      ,0       ,0      ,0	; KbFind_res_alt

		.CONST
Key_S		DB	'      '		;  0
		DB	'Help  '
		DB	'Menu  '
		DB	'View  '
		DB	'View..'
		DB	'Edit  '
		DB	'Edit..'
		DB	'Copy  '
		DB	'RenMov'
		DB	'MkDir '
		DB	'Delete'
		DB	'PullDn'
		DB	'Quit  '
		DB	'Main  '
		DB	'SavSet'
		DB	'Left  '
		DB	'Right '
		DB	'Label '
		DB	'MkArc '
		DB	'Mode  '
		DB	'Memory'
		DB	'DirSiz'
		DB	'Find  '
		DB	'Histry'
		DB	'EGA Ln'
		DB	'Tree  '
		DB	'Wrap  '
		DB	'Unwrap'
		DB	'Zoom  '
		DB	'Unzoom'
		DB	'Text  '
		DB	'Hex   '
		DB	'Goto  '
		DB	'Search'
		DB	'Next  '
		DB	'Prev  '
		DB	'Table '
		DB	'Shift '
		DB	'Save  '
		DB	'Save..'
		DB	'Q&Save'
		DB	'Rescan'

		.CODE	Subs
PutKey		PROC	FAR
		CMP	ES:KeyBar,0
		JNZ	@@KeyBarOn
		MOV	ES:BarPrev,0FFh
		RET
  @@KeyBarOn:	PUSH	BX
		PUSH	AX
		MOV	BL,AL
		CALL	KeybStat
		MOV	AH,3
		TEST	AL,KbdAltMask
		JNZ	@@Output
		DEC	AH
		TEST	AL,KbdCtrlMask
		JNZ	@@Output
		DEC	AH
		TEST	AL,KbdShiftMask
		JNZ	@@Output
		DEC	AH
  @@Output:	MOV	AL,BL
		CALL	PutKey0
		POP	AX
		POP	BX
		RET
PutKey		ENDP

PutKey0		PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,DS
		PUSH	CS
		POP	DS
		SHL	AL,1
		SHL	AL,1
		OR	AL,AH
		MOV	AH,0
		MOV	BX,AX
		MOV	AL,0
		CMP	BL,LenKeyB
		JAE	@@CmpBar
		MOV	AL,DS:KeyB_T[BX] ; KeyB_T[IKeyBar][keybar_mod]
  @@CmpBar:	CMP	AL,ES:BarPrev
		JE	@@Exit ; keybar not changed
		MOV	ES:BarPrev,AL
		MOV	AH,NmbKeyBar
		MUL	AH
		MOV	SI,AX
		MOV	BH,ES:Lines
		DEC	BH
		MOV	BL,0
		MOV	AL,ES:Columns
		MOV	AH,0
		MOV	DH,NmbKeyBar
		DIV	DH
		MOV	CX,AX
		MOV	AH,KeyTi_C
		CALL	Color
		MOV	DH,AH
		MOV	AH,KeyNu_C
		CALL	Color
		PUSH	ES
		CALL	BasAdr
		MOV	DL,'1'
		OR	CH,CH
		JE	@@Next
		PUSH	CX
		MOV	AL,' '
		DEC	CH
		JE	@@Space
		MOV	CL,CH
		MOV	CH,0
		XCHG	AH,DH
		REP	STOSW
		XCHG	AH,DH
  @@Space:	STOSW
		POP	CX
		MOV	CH,0
  @@Next:	MOV	AL,DL
  @@F10:	STOSW
		XCHG	AH,DH
		PUSH	SI
		PUSH	CX
		PUSH	AX
		MOV	AL,DS:KeyC_T[SI] ; Fn_desc = KeyC_T[KeyB_T[IKeyBar][keybar_mod]]
		MOV	CL,LenKeyText
		MUL	CL
		ADD	AX,OFFSET Key_S ; Key_S[Fn_desc++]
		MOV	SI,AX
		POP	AX
  @@Write:	LODSB
		STOSW
		LOOP	@@Write
		POP	CX
		MOV	AL,' '
		PUSH	CX
		SUB	CL,LenKeyCell
		JBE	@@Number
		REP	STOSW
  @@Number:	POP	CX
		POP	SI
		INC	SI
		INC	DX
		CMP	DL,'9'+1
		JA	@@Output
		XCHG	AH,DH
		STOSW
		JB	@@Next ; for F1..F10
		MOV	AL,'1'
		STOSW
		MOV	AL,'0'
		JMP	@@F10
  @@Output:	POP	ES
		MOV	DL,ES:Columns ; Wnd.w = Columns
		MOV	DH,1 ; Wnd.h = 1
		CALL	Window ; display keybar
  @@Exit:	RET
PutKey0		ENDP

;---------------------------------------------------------------
;	Output menu bar
;		   AL - Case
;---------------------------------------------------------------
None_M		EQU	0
Left_M		EQU	1
Fls_M		EQU	2
Cmd_M		EQU	3
Opt_M		EQU	4
Right_M		EQU	5
Def_M		EQU	255

		.CONST
PtMnu_S		DB	'  Left  ',0
		DB	'  Files  ',0
		DB	'  Commands  ',0
		DB	'  Options  ',0
		DB	'  Right'
BegMnu_S	DB	'  ',0

		.CODE	Subs
PutMnu		PROC	FAR
		CMP	ES:MenuVs,0
		JNZ	PutMnu1
		RET
PutMnu		ENDP

PutMnu1		PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,DS,ES
		PUSH	ES
		PUSH	CS
		POP	DS
		MOV	CH,AL
		MOV	AL,ES:Columns
		MOV	AH,0
		PUSH	AX
		XOR	BX,BX
		MOV	AH,MnCur_C
		CALL	Color
		MOV	DL,AH
		MOV	AH,Menu_C
		CALL	Color
		MOV	SI,OFFSET BegMnu_S
		CALL	WriteCoLnBX
		MOV	CL,1
		MOV	SI,OFFSET PtMnu_S
  @@NextItem:	PUSH	AX
		CMP	CL,CH
  		JNE	@@PutItem
		MOV	AH,DL
  @@PutItem:	CALL	WriteCoLine
		POP	AX
		INC	CX
		CMP	SI,OFFSET BegMnu_S
		JB	@@NextItem
		POP	CX
		MOV	DL,CL
		MOV	SI,DI
		SHR	SI,1
		SUB	CX,SI
		MOV	AL,' '
		REP	STOSW
		POP	ES
		MOV	DH,1
		CALL	Window
		RET
PutMnu1		ENDP

;---------------------------------------------------------------
;	Output prompt & command line
;---------------------------------------------------------------
PutPrompt	PROC	FAR
		USES	AX,BX,DX,SI,DI,DS
		LOCAL	PathBuf:BYTE:LenPath
		MOV	DS,ES:WCBsec
		CALL	BegBX
		CALL	SizDX
		ADD	DH,BH
		MOV	BH,ES:DosLin
		CMP	BH,DH
		JAE	@@Output
		MOV	AL,DS:Visible
		CALL	InvWCB
		CMP	DS:Visible,0
		JZ	@@Output
		PUSH	DX
		CMP	AL,0
		JNZ	@@NewLine
		MOV	DH,1
		CALL	ResUser
		CALL	Window
  @@NewLine:	POP	BX
		MOV	ES:DosLin,BH
  @@Output:	PUSH	ES
		POP	DS
		MOV	BL,0
		PUSH	ES
		CALL	BasAdr
		PUSH	DI
		MOV	AH,DS:CrntCol
		MOV	SI,OFFSET DosPath
		LODSB
		STOSW
		CMP	DS:Prompt,0
		JZ	@@ChkErr
		PUSH	ES
		PUSH	DI
		PUSH	AX
		PUSH	SS
		POP	ES
		DEC	SI
		LEA	DI,PathBuf
		PUSH	DI
		CALL	strcpy
		POP	SI
		MOV	AL,DS:Columns
		MOV	AH,0
		SHR	AX,1
		CALL	ShrtLin
		INC	SI
		POP	AX
		POP	DI
		POP	ES
		PUSH	DS
		PUSH	SS
		POP	DS
		CALL	WriteCoLine
		POP	DS
  @@ChkErr:	CMP	DS:PathEr,0
		JZ	@@EndPrompt
		MOV	AL,'?'
		STOSW
  @@EndPrompt:	MOV	AL,'>'
		STOSW
		MOV	AX,DI
		POP	DI
		POP	ES
		SUB	AX,DI
		SHR	AX,1
		MOV	DS:BegCol,AL
		CALL	DosCom
		RET
PutPrompt	ENDP

;---------------------------------------------------------------
;	Output command line
;---------------------------------------------------------------
DosCom		PROC	FAR
		PUSH	DS
		PUSH	SI
		PUSH	DI
		PUSH	DX
		PUSH	CX
		PUSH	BX
		PUSH	AX
		PUSH	ES
		POP	DS
		MOV	AL,DS:DosCol
		MOV	BH,AL
		MOV	BL,0
		SUB	AL,5
		JB	DosCm1
		MOV	BL,DS:OutCol
		CMP	AL,BL
		JAE	DosCm1
		MOV	BL,AL
DosCm1:		MOV	CL,DS:Columns
		MOV	CH,0
		DEC	CX
		SUB	CL,DS:BegCol
		SUB	BH,CL
		JB	DosCm2
		CMP	BH,BL
		JBE	DosCm2
		MOV	BL,BH
DosCm2:		MOV	DS:OutCol,BL
		MOV	BH,0
		LEA	SI,[BX].DosBuf
		INC	CX
		MOV	BH,DS:DosLin
		MOV	BL,DS:BegCol
		PUSH	ES
		CALL	BasAdr
		MOV	AH,DS:CrntCol
  @@Next:	LODSB
		CMP	AL,0
		JNE	@@Write
		DEC	SI
		MOV	AL,' '
  @@Write:	STOSW
		LOOP	@@Next
		POP	ES
		MOV	BL,0
		MOV	DL,DS:Columns
		MOV	DH,1
		CALL	Window
		POP	AX
		POP	BX
		POP	CX
		POP	DX
		POP	DI
		POP	SI
		POP	DS
		RET
DosCom		ENDP

;---------------------------------------------------------------
;	Panel
;		   DS - WCBseg
;---------------------------------------------------------------
		.CONST
PanName_S	DB	'Name',0
PanRatio_S	DB	'Ratio',0
PanSize_S	DB	'Size',0
PanDate_S	DB	'Date',0
PanTime_S	DB	'Time',0
PanDescr_S	DB	'Description',0
PanInfo_S	DB	'The ',PrgName,', Version 4.99',0
PanInfoLen	=	$ - PanInfo_S - 1

		.CODE	Subs
Panel		PROC	FAR
		PUSH	SI
		PUSH	DI
		PUSH	DX
		PUSH	CX
		PUSH	BX
		PUSH	AX
		CALL	BegBX
		CALL	SizDX
		PUSH	DX
		MOV	DL,DH
		MOV	DH,0
		MOV	DI,DX
		DEC	DI
		DEC	DI
		XOR	CX,CX
		POP	DX
		MOV	DH,0
		DEC	DX
		DEC	DX
		MOV	AH,Back_C
		CALL	Color
		PUSH	BX
		CALL	BoxDubl
		SUB	BH,3
		XOR	SI,SI
		MOV	CX,WORD PTR ES:Borders[8]
		MOV	AL,ES:Borders[10]
		CMP	DS:WinTyp,p_Info
		JE	@@InfoPanel
		CMP	DS:WinTyp,p_File
		JNE	@@DevLine
		CMP	ES:MiniSta,0
		JZ	@@FilePanel
		DEC	DI
		DEC	DI
		JMP	@@DevLine
  @@InfoPanel:	POP	BX
		PUSH	DS
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	CX
		PUSH	BX
		PUSH	AX
		PUSH	CS
		POP	DS
		MOV	CX,DX
		SUB	CX,PanInfoLen
		SHR	CX,1
		ADD	BX,CX
		ADD	BX,101h
		MOV	SI,OFFSET PanInfo_S
		CALL	WriteCoLnBX
		POP	AX
		POP	BX
		POP	CX
		POP	SI
		POP	DI
		POP	ES
		POP	DS
		PUSH	BX
		ADD	BH,2
		CMP	DI,10
		JB	@@DevLine
		CALL	BoxLine
		ADD	BH,7
  @@DevLine:	CALL	BoxLine
  @@FilePanel:	POP	BX
		CMP	DS:WinTyp,p_File
		JNE	@@Exit
		ADD	BX,101h
		MOV	AH,Title_C
		CALL	Color
  @@NextCol:	CALL	NameWidthAL
		MOV	SI,OFFSET PanName_S
		CALL	PanelTitle
		JE	@@Exit
		CMP	DS:BrfFul,1
		JB	@@NextCol
		JA	@@Descript
		CMP	DS:ShowSize,0
		JZ	@@Ratio
		MOV	AL,9
		MOV	SI,OFFSET PanSize_S
		CALL	PanelTitle
		JE	@@Exit
  @@Ratio:	CMP	DS:ShowRatio,0
		JZ	@@Date
		JMP	@@Date			; ???
		MOV	AL,5
		MOV	SI,OFFSET PanRatio_S
		CALL	PanelTitle
		JE	@@Exit
  @@Date:	CMP	DS:ShowDate,0
		JZ	@@Time
		MOV	AL,8
		MOV	SI,OFFSET PanDate_S
		CALL	PanelTitle
		JE	@@Exit
  @@Time:	CMP	DS:ShowTime,0
		JZ	@@Descript
		MOV	AL,6
		MOV	SI,OFFSET PanTime_S
		CALL	PanelTitle
		JE	@@Exit
  @@Descript:	MOV	AL,DL
		MOV	SI,OFFSET PanDescr_S
		CALL	PanelTitle
  @@Exit:	POP	AX
		POP	BX
		POP	CX
		POP	DX
		POP	DI
		POP	SI
		RET
Panel		ENDP

PanelTitle	PROC	NEAR
		USES	DS,ES
		PUSH	DI
		PUSH	ES
		POP	DS
		PUSH	AX
		PUSH	CS
		POP	ES
		MOV	DI,SI
		CALL	StrLen
		DEC	CX
		POP	AX
		CALL	BasAdr
		SUB	AL,CL
		MOV	CL,0
		JBE	@@Next
		SHR	AL,1
		MOV	CL,AL
		ADC	CL,0
		JE	@@Next
  @@Space1:	INC	DI
		INC	DI
		INC	BX
		DEC	DX
		JE	@@Exit
		LOOP	@@Space1
		MOV	CL,AL
  @@Next:	LODS	BYTE PTR CS:[SI]
		CMP	AL,0
		JE	@@EndLine
		STOSW
		INC	BX
		DEC	DX
		JNE	@@Next
		JMP	@@Exit
  @@EndLine:	JCXZ	@@Lines
  @@Space2:	INC	DI
		INC	DI
		INC	BX
		DEC	DX
		JE	@@Exit
		LOOP	@@Space2
  @@Lines:	MOV	CL,DS:Columns
		MOV	CH,0
		ADD	CX,CX
		MOV	SI,CX
		POP	CX
		PUSH	CX
		SUB	DI,SI
		MOV	AL,DS:Borders[11]
		MOV	ES:[DI],AL
		ADD	DI,SI
		MOV	AL,DS:Borders[12]
  @@NextRow:	MOV	ES:[DI],AL
		ADD	DI,SI
		LOOP	@@NextRow
		MOV	AL,DS:Borders[14]
		CMP	DS:MiniSta,0
		JZ	@@Write
		MOV	AL,DS:Borders[13]
  @@Write:	MOV	ES:[DI],AL
		INC	BX
		DEC	DX
  @@Exit:	POP	DI
		RET
PanelTitle	ENDP

;---------------------------------------------------------------
;	Test executable file
;		DS:SI - address of file
;	Return:	 CY=1 - executable file
;		 CY=0 - no
;---------------------------------------------------------------
		.CONST
Ext_S		DB	'COM',0
		DB	'EXE',0
		DB	'BAT',0

		.CODE	Subs
TstExe		PROC	FAR
		USES	AX,CX,SI,DI,ES
		CALL	FindFileExt
		PUSH	CS
		POP	ES
		MOV	DI,OFFSET Ext_S
		MOV	CX,3
  @@CmpExt:	CALL	CompNormLines
		STC
		JE	@@Exit
		ADD	DI,4
		LOOP	@@CmpExt
  @@Exit:	RET
TstExe		ENDP

;---------------------------------------------------------------
;	File sort
;		   DS - WCBseg
;---------------------------------------------------------------
FileSort	PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,BP
		MOV	DI,OFFSET TmpBuf	; Prepare masks
		MOV	CL,DS:FiltTyp
		MOV	CH,0
		PUSH	DI
		PUSH	DS
		PUSH	ES
		POP	DS
		CMP	CL,MaxFilts
		JAE	@@Custom
		MOV	SI,DS:FiltsPoint
		CALL	FndItem
		JC	@@Custom
		JCXZ	@@FindMask
  @@FindFilter:	CALL	FindNextItem
		JC	@@Custom
		LOOP	@@FindFilter
  @@FindMask:	LODSB
		CMP	AL,13
		JE	@@MaskFound
		CMP	AL,10
		JE	@@MaskFound
		CMP	AL,1Ah
		JNE	@@FindMask
  @@MaskFound:	DEC	SI
		MOV	DL,1
		JMP	@@NextMask
  @@Custom:	POP	DS
		PUSH	DS
		MOV	SI,OFFSET DskMas
		MOV	DL,0
  @@NextMask:	MOV	CX,DI
		OR	DL,DL
		JNZ	@@FndMask
		CALL	FndLet
		CMP	AL,0
		JNE	@@GetChar
		JMP	@@EndMask
  @@FndMask:	CALL	FindCommand
		JC	@@EndMask
  @@GetChar:	LODSB
		OR	DL,DL
		JNZ	@@CmpEnd
		CMP	AL,','
		JE	@@ConvMask
		CMP	AL,';'
		JE	@@ConvMask
		CMP	AL,0
		JNE	@@SaveChar
		JMP	@@PrevAdr
  @@CmpEnd:	CMP	AL,13
		JE	@@PrevAdr
		CMP	AL,10
		JE	@@PrevAdr
		CMP	AL,1Ah
		JE	@@PrevAdr
  @@SaveChar:	CMP	DI,OFFSET TmpBuf+512-1
		JAE	@@Overflow
		STOSB
		JMP	@@GetChar
  @@PrevAdr:	DEC	SI
  @@ConvMask:	CMP	DI,CX
		JBE	@@NextMask
		MOV	AL,';'
		STOSB
		JMP	@@NextMask
  @@Overflow:	MOV	DI,CX
  @@EndMask:	MOV	AL,0
		STOSB
		POP	DS

		MOV	BP,ES:ScreenBuf		; Index files
		MOV	AX,ES:UserScreen
		SUB	AX,BP
		ADD	BP,AX
		ADD	BP,AX
		PUSH	ES
		MOV	ES,BP
		MOV	SI,DS:DiskBuf
		XOR	DI,DI
  @@Index:	CMP	SI,DS:EndFul
		JAE	@@EndIndex
		MOV	AX,SI
		STOSW
		CALL	NextFilEnt
		JMP	@@Index
  @@EndIndex:	POP	ES

		MOV	BX,DI			; Filter
		XOR	SI,SI
		POP	DI
		OR	BX,BX
		JE	@@Exit
		PUSH	BX
  @@SubEnd:	DEC	BX
		DEC	BX
  @@ChkAttr:	PUSH	SI
		PUSH	ES
		MOV	ES,BP
		MOV	SI,ES:[SI]
		POP	ES
		MOV	AH,DS:[SI].AttrF
		XOR	AH,20h
		MOV	AL,DS:FiltAttr
		NOT	AL
		AND	AL,3Fh
		TEST	AH,10h
		JZ	@@CmpAttr
		AND	AL,17h
  @@CmpAttr:	TEST	AL,AH
		JNZ	@@HideFile
		TEST	DS:[SI].AttrF,10h
		JNZ	@@Next
		CMP	BYTE PTR ES:[DI],0
		JE	@@Next
		PUSH	SI
		ADD	SI,OFFSET NameF
		CALL	TestMask
		POP	SI
		JZ	@@Next
		CMP	DS:[SI].LenLongNamF,0
		JE	@@HideFile
		PUSH	DS
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JNE	@@HideFile
		ADD	SI,DS:[SI].LenShortNamF
		ADD	SI,OFFSET NameF
		CALL	TestMask
		JZ	@@Next
  @@HideFile:	POP	SI
		CMP	SI,BX
		JE	@@ExitFilt
		PUSH	ES
		PUSH	DI
		MOV	ES,BP
		MOV	DI,ES:[BX]
		XCHG	DI,ES:[SI]
		MOV	ES:[BX],DI
		AND	DS:[DI].AttrF,3Fh
		POP	DI
		POP	ES
		JMP	@@SubEnd
  @@Next:	POP	SI
		INC	SI
		INC	SI
		CMP	SI,BX
		JBE	@@ChkAttr
  @@ExitFilt:	SHR	SI,1

		MOV	BX,SI			; Shell sort
  @@Sort1:	SHR	BX,1
		OR	BX,BX
		JE	@@Move1
		PUSH	SI
		MOV	CX,SI
  @@Sort2:	MOV	DX,0FFFFh
		XOR	SI,SI
  @@Sort3:	LEA	DI,[SI+BX]
		CMP	DI,CX
		JAE	@@Sort5
		PUSH	DI
		PUSH	SI
		PUSH	ES
		MOV	ES,BP
		ADD	SI,SI
		ADD	DI,DI
		MOV	SI,ES:[SI]
		MOV	DI,ES:[DI]
		POP	ES
		CALL	CompEntries
		POP	SI
		POP	DI
		JNC	@@Sort4
		PUSH	ES
		PUSH	DI
		PUSH	SI
		ADD	SI,SI
		ADD	DI,DI
		MOV	ES,BP
		MOV	DX,ES:[SI]
		XCHG	DX,ES:[DI]
		MOV	ES:[SI],DX
		POP	SI
		POP	DI
		POP	ES
		MOV	DX,DI
  @@Sort4:	INC	SI
		JMP	@@Sort3
  @@Sort5:	MOV	CX,DX
		CMP	CX,0FFFFh
		JNE	@@Sort2
		POP	SI
		JMP	@@Sort1

  @@Move1:	POP	DX			; Move files
		SHR	DX,1
		PUSH	ES
		PUSH	SI
		XOR	BX,BX
		MOV	AX,DS:EndFul
		MOV	DS:EndBuf,AX
		MOV	DI,OFFSET TmpBuf
		MOV	SI,ES:WorkBuffer
		MOV	AX,ES
		ADD	AX,ES:SizeDataSeg
		SUB	AX,SI
		MOV	CX,512
		CMP	AX,512/16
		JBE	@@Move2
		MOV	ES,SI
		XOR	DI,DI
		MOV	CX,0FFFFh
		CMP	AX,1000h
		JAE	@@Move2
		MOV	CL,4
		SHL	AX,CL
		MOV	CX,AX
  @@Move2:	PUSH	CX
		PUSH	DI
		MOV	AX,CX
  @@Move3:	PUSH	DS
		MOV	DS,BP
		MOV	SI,DS:[BX]
		POP	DS
		MOV	CX,SIZE FilEnt
		ADD	CX,DS:[SI].LenShortNamF
		ADD	CX,DS:[SI].LenLongNamF
		CMP	CX,AX
		JA	@@Move4
		SUB	AX,CX
		PUSH	SI
		REP	MOVSB
		POP	SI
		MOV	DS:[SI].NameF,0
		INC	BX
		INC	BX
		DEC	DX
		JNE	@@Move3
  @@Move4:	PUSH	ES
		PUSH	DS
		POP	ES
		MOV	SI,DS:DiskBuf
		MOV	DI,SI
  @@Move5:	CMP	SI,DS:EndBuf
		JAE	@@Move7
		MOV	AX,SIZE FilEnt
		ADD	AX,DS:[SI].LenShortNamF
		ADD	AX,DS:[SI].LenLongNamF
		CMP	DS:[SI].NameF,0
		JNE	@@Move6
		ADD	SI,AX
		JMP	@@Move5
  @@Move6:	PUSH	ES
		PUSH	SI
		PUSH	AX
		PUSH	DI
		MOV	ES,BP
		MOV	AX,SI
		MOV	DI,BX
		MOV	CX,DX
		REPNE	SCASW
		MOV	SI,DI
		POP	DI
		MOV	ES:[SI-2],DI
		POP	CX
		POP	SI
		POP	ES
		REP	MOVSB
		JMP	@@Move5
  @@Move7:	MOV	DS:EndBuf,DI
		MOV	CX,DS:EndFul
		SUB	CX,SI
		JBE	@@Move8
		REP	MOVSB
  @@Move8:	MOV	CX,DS:EndFul
		SUB	CX,DI
		POP	DS
		POP	SI
		PUSH	SI
		REP	MOVSB
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		POP	DI
		POP	CX
		OR	DX,DX
		JNE	@@Move2
		POP	SI
		POP	ES

		CALL	Numb2Addr
		MOV	DS:EndBuf,SI
  @@Exit:	RET
FileSort	ENDP

CompEntries	PROC	NEAR
		USES	AX,CX
		MOV	AL,DS:SortTyp
		CMP	AL,4
		JB	@@CmpDirAttr
		MOV	AX,DS:[DI].NumbF
		CMP	AX,DS:[SI].NumbF
		RET
  @@CmpDirAttr:	MOV	CL,DS:[SI].AttrF
		MOV	CH,DS:[DI].AttrF
		TEST	CL,10h
		JNZ	@@ChkUpDir
		TEST	CH,10h
		JZ	@@ChkSystem
		STC
		RET
  @@ChkUpDir:	TEST	CH,10h
		JZ	@@Exit
		PUSH	SI
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		POP	SI
		JE	@@Exit
		PUSH	SI
		LEA	SI,[DI].NameF
		CALL	ChkUpDir
		POP	SI
		JNE	@@ByName
		STC
		RET
  @@ChkSystem:	TEST	CL,4
		JNZ	@@ChkSys2
		TEST	CH,4
		JZ	@@ByName
		STC
		RET
  @@ChkSys2:	TEST	CH,4
		JZ	@@Exit
  @@ByName:	CMP	AL,1
		JAE	@@ByExt
  @@CmpNames:	PUSH	SI
		PUSH	DI
		XCHG	SI,DI
		CALL	NameOffset1
		XCHG	SI,DI
		CALL	NameOffset1
		CALL	CompName
		POP	DI
		POP	SI
		JMP	@@ChkRev
  @@ByExt:	JNE	@@ByTime
		PUSH	SI
		PUSH	DI
		CALL	NameOffset1
		CALL	FindFileExt
		XCHG	SI,DI
		CALL	NameOffset1
		CALL	FindFileExt
		XCHG	SI,DI
		CALL	CompName
		POP	DI
		POP	SI
		JE	@@CmpNames
		JMP	@@ChkRev
  @@ByTime:	CMP	AL,2
		JNE	@@BySize
		MOV	AX,DS:[SI].DateF
		CMP	AX,DS:[DI].DateF
		JNE	@@ChkRev
		MOV	AX,DS:[SI].TimeF
		CMP	AX,DS:[DI].TimeF
		JE	@@CmpNames
		JMP	@@ChkRev
  @@BySize:	MOV	AX,DS:[SI].SizeF.Hi
		CMP	AX,DS:[DI].SizeF.Hi
		JNE	@@ChkRev
		MOV	AX,DS:[SI].SizeF.Lo
		CMP	AX,DS:[DI].SizeF.Lo
		JE	@@CmpNames
  @@ChkRev:	PUSHF
		CMP	DS:SortRev,0
		JE	@@Reverse
		POPF
		CMC
		PUSHF
  @@Reverse:	POPF
  @@Exit:	RET
CompEntries	ENDP

CompName	PROC	NEAR
		MOV	CL,80h
  @@GetChar1:	LODSB
		CALL	UpCase
		CMP	AL,'.'
		JNE	@@GetChar2
		MOV	AL,1
  @@GetChar2:	MOV	AH,AL
		MOV	AL,DS:[DI]
		INC	DI
		CALL	UpCase
		CMP	AL,'.'
		JNE	@@ChkNumb
		MOV	AL,1
  @@ChkNumb:	CMP	ES:NumbSort,0
		JE	@@CompChar
		CMP	AL,'9'
		JA	@@CompChar
		CMP	AL,'0'
		JB	@@CompChar
		CMP	AH,'9'
		JA	@@CompChar
		CMP	AH,'0'
		JB	@@CompChar
		PUSH	ES
		PUSH	DX
		PUSH	CX
		PUSH	BX
		PUSH	DS
		POP	ES
		XCHG	SI,DI
		CALL	CmpNumbLen
		MOV	DX,CX
		XCHG	SI,DI
		CALL	CmpNumbLen
		MOV	BX,CX
		DEC	SI
		DEC	DI
		MOV	AL,0
		SUB	CX,DX
		JE	@@CompNumb
		MOV	AL,'0'
		JB	@@ChkZeros
		XCHG	SI,DI
		REPE	SCASB
		XCHG	SI,DI
		MOV	AL,3
		JE	@@CompNumb
		STC
		JMP	@@ExitNumb
  @@ChkZeros:	NEG	CX
		MOV	DX,BX
		REPE	SCASB
		MOV	AL,2
		JE	@@CompNumb
		CLC
		JMP	@@ExitNumb
  @@CompNumb:	MOV	CX,DX
		REPE	CMPSB
		CMC
  @@ExitNumb:	POP	BX
		POP	CX
		POP	DX
		POP	ES
		JNE	@@Exit
		TEST	CL,2
		JNE	@@GetChar1
		OR	CL,AL
		JMP	@@GetChar1
  @@CompChar:	CMP	AL,AH
		JNE	@@Exit
		CMP	AL,0
		JNE	@@GetChar1
		TEST	CL,2
		JE	@@Exit
		SHR	CL,1
  @@Exit:	RET
CompName	ENDP

CmpNumbLen	PROC	NEAR
		USES	SI
		XOR	CX,CX
  @@Char:	INC	CX
		LODSB
		CMP	AL,'0'
		JB	@@Exit
		CMP	AL,'9'
		JBE	@@Char
  @@Exit:	RET
CmpNumbLen	ENDP

;---------------------------------------------------------------
;	Find file
;		   AX - number of file
;		   DS - WCBseg
;		ES:DI - file
;	Return:	DS:SI - Address of file, CF=0
;			Not found,       CF=1
;---------------------------------------------------------------
FindFile	PROC	FAR
		USES	AX
		MOV	SI,DS:DiskBuf
  @@FindFile:	CALL	FindFileNext
		JC	@@FindOffln1
		TEST	DS:[SI].AttrF,10h
		JNZ	@@ChkNumb
		TEST	DS:[SI].AttrF,8
		JZ	@@ChkNumb
		CALL	NextFilEnt
		JMP	@@FindFile
  @@ChkNumb:	OR	AX,AX
		JE	@@Exit
		DEC	AX
  @@FindOffln1:	INC	AX
		MOV	SI,DS:DiskBuf
  @@FindOffln2:	CALL	FindFileNext
		JC	@@Exit
		TEST	DS:[SI].AttrF,10h
		JNZ	@@Next
		TEST	DS:[SI].AttrF,8
		JZ	@@Next
		DEC	AX
		JE	@@Exit
  @@Next:	CALL	NextFilEnt
		JMP	@@FindOffln2
  @@Exit:	RET
FindFile	ENDP

;---------------------------------------------------------------
;	Find file
;		ES:DI - file
;		DS:SI - start entry
;	Return:	DS:SI - Address of file, CF=0
;			Not found,       CF=1
;---------------------------------------------------------------
FindFileNext	PROC	NEAR
		USES	AX,CX,DI,ES
		LOCAL	NameBuf:BYTE:257
		PUSH	DS
		PUSH	SI
		PUSH	SS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	CX,257
		MOV	SI,DI
		LEA	DI,NameBuf
		PUSH	DI
  @@MoveName:	LODSB
		CALL	UpCase
		STOSB
		CMP	AL,0
		LOOPNE	@@MoveName
		POP	DI
		POP	SI
		POP	DS
		JNE	@@NotFound
  @@CompFile:	CMP	SI,DS:EndBuf
		JAE	@@NotFound
		PUSH	SI
		ADD	SI,OFFSET NameF
		CALL	FindFileCmp
		POP	SI
		JE	@@Exit
		CMP	DS:[SI].LenLongNamF,0
		JE	@@NextFile
		PUSH	SI
		CALL	LongNameOffset1
		CALL	FindFileCmp
		POP	SI
		JE	@@Exit
  @@NextFile:	CALL	NextFilEnt
		JMP	@@CompFile
  @@NotFound:	STC
  @@Exit:	RET
FindFileNext	ENDP

FindFileCmp	PROC	NEAR
		USES	DI
  @@NextChar:	LODSB
		CALL	UpCase
		SCASB
		JNE	@@Exit
		CMP	AL,0
		JNE	@@NextChar
  @@Exit:	RET
FindFileCmp	ENDP

;---------------------------------------------------------------
;	Find file entry
;		   DS - WCBseg
;		ES:DI - file entry
;	Return:	DS:SI - Address of file, CF=0
;			Not found,       CF=1
;---------------------------------------------------------------
FindFileEntry	PROC	FAR
		USES	AX,BX,CX,DI
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,DI
		CALL	FileNumb
		MOV	BX,DS:[SI].LenShortNamF
		MOV	CX,DS:[SI].LenLongNamF
		POP	DS
		ADD	DI,OFFSET NameF
		JCXZ	@@ShortName
		PUSH	DI
		ADD	DI,BX
		CALL	FindFile
		POP	DI
		JNC	@@Exit
  @@ShortName:	CALL	FindFile
  @@Exit:	RET
FindFileEntry	ENDP

;---------------------------------------------------------------
;	Find near file
;		   SI - File number
;		ES:BX - End
;		   DS - WCBseg
;	Return:	   SI - File number, CF=0
;			Not found  , CF=1
;---------------------------------------------------------------
FindFileNear	PROC	NEAR
		PUSH	SI
		PUSH	DS
		PUSH	ES
		POP	DS
		CALL	Numb2Addr
		MOV	DI,SI
		POP	DS
		CMP	DI,BX
		CMC
		JC	@@Exit
  @@FindForw:	CALL	FindFileEntry
		JNC	@@Found
		PUSH	AX
		MOV	AX,ES:[DI].LenShortNamF
		ADD	DI,ES:[DI].LenLongNamF
		ADD	DI,SIZE FilEnt
		ADD	DI,AX
		POP	AX
		CMP	DI,BX
		JB	@@FindForw
		POP	DI
		PUSH	DI
  @@FindBack:	PUSH	DI
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,DI
		CALL	Numb2Addr
		POP	DS
		MOV	DI,SI
		CALL	FindFileEntry
		POP	DI
		JNC	@@Found
		SUB	DI,1
		JAE	@@FindBack
		JMP	@@Exit
  @@Found:	CALL	Addr2Numb
		CLC
  @@Exit:	POP	DI
		RET
FindFileNear	ENDP

;---------------------------------------------------------------
;	Number of file
;		DS:SI - file entry
;	Return:	   AX - number of file
;---------------------------------------------------------------
FileNumb	PROC	FAR
		USES	BX,CX,SI,DI,ES
		XOR	AX,AX
		TEST	DS:[SI].AttrF,10h
		JNZ	@@Exit
		TEST	DS:[SI].AttrF,8
		JZ	@@Exit
		PUSH	DS
		POP	ES
		LEA	DI,[SI].NameF
		MOV	CX,SI
		MOV	SI,DS:DiskBuf
  @@FindFile:	CALL	FindFileNext
		JC	@@Exit
		MOV	BX,SI
		CALL	NextFilEnt
		TEST	DS:[BX].AttrF,10h
		JNZ	@@Next
		TEST	DS:[BX].AttrF,8
		JZ	@@Next
		CMP	BX,CX
		JAE	@@FindFile
  @@Next:	INC	AX
		JMP	@@FindFile
  @@Exit:	RET
FileNumb	ENDP

;---------------------------------------------------------------
;	Set parent directory entry
;		   DS - WCB segment
;	Return:	   DI - end of entry
;---------------------------------------------------------------
SetParentDir	PROC	FAR
		PUSH	DS
		POP	ES
		MOV	DI,DS:DiskBuf
		MOV	SI,DI
		MOV	CX,SIZE FilEnt
		MOV	AL,0
		REP	STOSB
		MOV	AX,'.'
		STOSB
		STOSW
		MOV	DS:[SI].LenShortNamF,3
		MOV	DS:[SI].AttrF,10h
		RET
SetParentDir	ENDP

;---------------------------------------------------------------
;	Output path in the middle
;		   DS - WCBseg
;---------------------------------------------------------------
		.CONST
Tree_S		DB	'Tree',0
Info_S		DB	'Info',0

		.CODE	Subs
PutPath		PROC	FAR
		LOCAL	PathTitle:BYTE:LenPath+4+13+1
		LOCAL	FiltTitle:BYTE:16
		USES	AX,BX,CX,DX,SI,DI,DS,ES
		MOV	AL,DS:WinTyp
		MOV	AH,Back_C
		MOV	BX,DS
		CMP	BX,ES:WCBcrn
		JNE	@@ChkType
		MOV	AH,Cursr_C
  @@ChkType:	CALL	BegBX
		CALL	SizDX
		LEA	DI,FiltTitle
		PUSH	AX
		PUSH	ES
		PUSH	DI
		CMP	AL,0
		JE	@@FilePanel
		PUSH	CS
		PUSH	SS
		POP	ES
		POP	DS
		MOV	SI,OFFSET Tree_S
		CMP	AL,p_Info
		JB	@@EndFlt1
		MOV	SI,OFFSET Info_S
		JE	@@EndFlt1
		MOV	SI,OFFSET View0_S
		JMP	@@EndFlt1
  @@FilePanel:	MOV	CL,DS:FiltTyp
		MOV	CH,0
		INC	CX
		PUSH	DS
		PUSH	ES
		PUSH	SS
		POP	ES
		POP	DS
		MOV	SI,DS:FiltsPoint
		CALL	FndItem
		JMP	@@ChkEnd
  @@Custom:	POP	DS
		MOV	SI,OFFSET DskMas
		CMP	BYTE PTR DS:[SI],0
		JE	@@EndFilt
		MOV	CX,14
		MOV	AL,DS:WinShortPath[3]
		CMP	DS:ArcPnt,0
		JE	@@AddSlash
		MOV	AL,DS:ArcPath[0]
  @@AddSlash:	CMP	AL,0
		JE	@@CustMove
		MOV	AL,'\'
		STOSB
  @@CustMove:	LODSB
		CMP	AL,0
		JE	@@EndFilt
		CALL	LowCase
		STOSB
		LOOP	@@CustMove
		SUB	DI,3
		MOV	AL,'.'
		STOSB
		STOSB
		STOSB
		JMP	@@EndFilt
  @@NextItem:	CALL	FindNextItem
  @@ChkEnd:	JC	@@Custom
		LOOP	@@NextItem
		PUSH	SI
  @@ChkLine1:	MOV	CX,SI
  @@ChkLine2:	LODSB
		CMP	AL,13
		JE	@@EmptyFilt
		CMP	AL,10
		JE	@@EmptyFilt
		CMP	AL,1Ah
		JE	@@EmptyFilt
		CMP	AL,' '
		JE	@@ChkLine2
		CMP	AL,9
		JE	@@ChkLine2
		CMP	AL,':'
		JNE	@@ChkLine1
		POP	SI
		SUB	CX,SI
		CMP	CX,14
		JBE	@@MoveFilt
		MOV	CX,14
  @@MoveFilt:	MOV	AL,''
		STOSB
		REP	MOVSB
		PUSH	SI
  @@EmptyFilt:	POP	SI
		POP	DS
  @@EndFilt:	CALL	WinPath1
		MOV	AL,0
  @@EndFlt1:	POP	CX
		MOV	AH,AL
		MOV	AL,0
		STOSB
		SUB	DI,CX
		MOV	CX,DI
		LEA	DI,PathTitle
		PUSH	DI
		OR	AH,AH
		JNE	@@MovePath
		CMP	DS:ArcPnt,0
		JE	@@MovePath
		MOV	SI,OFFSET ArcExt
		CALL	strcpy
		MOV	AL,':'
		STOSB
		MOV	SI,DS:ArcPnt
		LODSW
		CALL	CutPath
		CALL	strcpy
		MOV	AL,'\'
		STOSB
		MOV	SI,OFFSET ArcPath
  @@MovePath:	PUSH	CX
		PUSH	DI
		LEA	CX,PathTitle+LenPath+4+13
		SUB	CX,DI
		REP	MOVSB
		MOV	AL,0
		STOSB
		POP	DI
		MOV	CH,0FFh
		REPNE	SCASB
		DEC	DI
		POP	CX
		POP	AX
		POP	ES
		SUB	DI,AX
		INC	DI
		MOV	DH,1
		OR	BH,BH
		JNE	@@CmpOffs
		CMP	ES:Clock,0
		JZ	@@CmpOffs
		MOV	SI,DS
		CMP	SI,ES:WCB2seg
		JNE	@@CmpOffs
		MOV	DH,7
		CMP	ES:Country.TimeFmt,0
		JZ	@@CmpOffs
		DEC	DH
  @@CmpOffs:	LEA	SI,PathTitle
		MOV	AH,0
		MOV	AL,DL
		SUB	AL,DH
		DEC	AX
		SUB	AX,CX
		CMP	DI,AX
		JBE	@@Offset
		MOV	DI,AX
		DEC	AX
		PUSH	ES
		PUSH	SS
		POP	ES
		CALL	ShrtLin
		POP	ES
  @@Offset:	ADD	CX,DI
		SUB	DL,CL
		MOV	AL,DL
		SHR	AL,1
		CMP	AL,DH
		JAE	@@OutPath
		MOV	AL,DL
		SUB	AL,DH
  @@OutPath:	ADD	BL,AL
		PUSH	SS
		POP	DS
		POP	AX
		CALL	Color
		CALL	BasAdr
		MOV	AL,' '
		STOSW
		CALL	WriteCoLine
		LEA	SI,FiltTitle
		CALL	WriteCoLine
		MOV	AL,' '
		STOSW
		RET
PutPath		ENDP

;---------------------------------------------------------------
;	Output file name
;		DS:SI - address of disk buffer
;		ES:DI - address of buffer
;		   AL - 0=auto format, 1=long format
;---------------------------------------------------------------
PutName		PROC	NEAR
		USES	AX,BX,CX,DX,SI
		MOV	BL,AL
		MOV	AH,DS:[SI].AttrF
		CALL	NameOffset1
		MOV	CX,SI
		XOR	DX,DX
  @@NameLen1:	LODSB
		CMP	AL,0
		JE	@@NameLen2
		CMP	AL,'.'
		JNE	@@NameLen1
		MOV	DX,SI
		JMP	@@NameLen1
  @@NameLen2:	DEC	SI
		CMP	DS:LongNames,0
		JZ	@@NameLen5
		CALL	NameWidthAL
  @@NameLen3:	MOV	DX,SI
		SUB	DX,CX
		MOV	SI,CX
		MOV	CL,AL
		MOV	CH,0
		OR	BL,BL
		JE	@@NameLen4
		CMP	DX,CX
		JBE	@@NameLen4
		SUB	DX,CX
		ADD	SI,DX
		INC	SI
		DEC	CX
		MOV	DX,CX
		MOV	AL,ES:Scrolls[0]
		STOSB
  @@NameLen4:	CALL	PutNam1
		OR	BL,BL
		JNE	@@Exit
		CALL	PutNam2
		CMP	AL,' '
		JE	@@Exit
		DEC	DI
		STOSB
		JMP	@@Exit
  @@NameLen5:	MOV	AL,12
		OR	BL,BL
		JNE	@@NameLen3
		SUB	SI,DX
		OR	DX,DX
		JNE	@@NameLen6
		MOV	DX,SI
		INC	DX
		XOR	SI,SI
  @@NameLen6:	DEC	DX
		SUB	DX,CX
		XCHG	SI,CX
		PUSH	SI
		PUSH	DX
		INC	DX
  @@NameLen7:	DEC	DX
		JE	@@NameLen8
		LODSB
		CMP	AL,'.'
		JE	@@NameLen7
  @@NameLen8:	POP	DX
		POP	SI
		JNE	@@NameLen9
		ADD	DX,CX
		INC	DX
		XOR	CX,CX
  @@NameLen9:	PUSH	CX
		MOV	CX,8
		CALL	PutNam1
		CALL	PutNam2
		STOSB
		POP	DX
		INC	SI
		MOV	CX,3
		CALL	PutNam1
  @@Exit:	RET
PutName		ENDP

PutNam1		PROC	NEAR
  @@Next:	MOV	AL,' '
		OR	DX,DX
		JE	@@Write
		LODSB
		DEC	DX
  @@Write:	STOSB
		LOOP	@@Next
		OR	DX,DX
		JE	@@Exit
		ADD	SI,DX
		DEC	DI
		MOV	AL,ES:Scrolls[1]
		STOSB
  @@Exit:	RET
PutNam1		ENDP

PutNam2		PROC	NEAR
		MOV	CL,' '
		MOV	CH,ES:FileMarks[0]
		CMP	ES:ColTyp,2
		JNE	@@NoLapTop
		TEST	AH,40h
		JZ	@@NoLapTop
		MOV	CX,WORD PTR ES:FileMarks[1]
  @@NoLapTop:	TEST	AH,6
		JZ	@@SetAttr
		MOV	CL,CH
  @@SetAttr:	MOV	AL,CL
		RET
PutNam2		ENDP

;---------------------------------------------------------------
;	Output size, date & time
;		DS:SI - address of disk buffer
;		ES:DI - address of buffer
;		   AL - divider
;---------------------------------------------------------------
SzDtTm		PROC	NEAR
		USES	AX
		CMP	DS:ShowSize,0
		JZ	@@SkipSize
		STOSB
		CALL	PutSize
  @@SkipSize:
;		CMP	DS:ShowRatio,0
;		JZ	@@SkipRatio
;		STOSB
;		CALL	PutRatio
; @@SkipRatio:
		CMP	DS:ShowDate,0
		JZ	@@SkipDate
		STOSB
		PUSH	AX
		MOV	AL,0
		CALL	PutDate
		POP	AX
  @@SkipDate:	CMP	DS:ShowTime,0
		JZ	@@Exit
		STOSB
		CALL	PutTime
		MOV	AL,ES:[DI-1]
		SUB	DI,3
		CMP	AL,'A'
		JB	@@Exit
		DEC	DI
		STOSB
  @@Exit:	RET
SzDtTm		ENDP

;---------------------------------------------------------------
;	Output size
;		DS:SI - address of disk buffer
;		ES:DI - address of buffer
;---------------------------------------------------------------
		.CONST
UpDir		DB	'UP--DIR'
SubDir		DB	'SUB-DIR'
Absent		DB	'  absent '

		.CODE	Subs
PutSize		PROC	NEAR
		USES	AX,BX,CX,SI,DS
		MOV	AL,DS:[SI].AttrF
		AND	AL,18h
		JZ	@@Size
		CMP	AL,18h
		JZ	@@Size
		MOV	BX,OFFSET Absent
		CMP	AL,8
		JZ	@@DirType
		MOV	BX,OFFSET UpDir
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		JE	@@DirType
		MOV	BX,OFFSET SubDir
  @@DirType:	MOV	SI,BX
		PUSH	CS
		POP	DS
		JMP	@@Write
  @@Size:	MOV	CX,DS:[SI].SizeF.Lo
		MOV	BX,DS:[SI].SizeF.Hi
		MOV	SI,DI
		MOV	AL,2
		CALL	Number
		MOV	DI,SI
		INC	SI
		PUSH	ES
		POP	DS
		CMP	BYTE PTR DS:[DI],' '
		JE	@@Write
		MOV	CX,9
		MOV	AL,'9'
		REP	STOSB
		JMP	@@Exit
  @@Write:	MOV	CX,9
		REP	MOVSB
  @@Exit:	RET
PutSize		ENDP

;---------------------------------------------------------------
;	Output date
;		DS:SI - address of disk buffer
;		ES:DI - address of buffer
;		   AL - year format: 0=YY, 1=YYYY
;---------------------------------------------------------------
PutDate		PROC	FAR
		USES	AX,BX,CX,DX,SI
		MOV	DL,AL
		MOV	AX,DS:[SI].DateF
		PUSH	AX
		AND	AX,1Fh
		CALL	HexDec
		MOV	SI,AX
		POP	AX
		MOV	CL,5
		SHR	AX,CL
		PUSH	AX
		AND	AX,0Fh
		CALL	HexDec
		MOV	BX,AX
		POP	AX
		MOV	CL,4
		SHR	AX,CL
		AND	AX,7Fh
		ADD	AL,80
		MOV	CL,100
		DIV	CL
		MOV	CL,AH
		ADD	AL,19
		CALL	HexDec
		XCHG	CX,AX
		CALL	HexDec
		CMP	ES:Country.DateFmt,1
		JE	@@Swap
		JB	@@ChkSpace
		XCHG	SI,AX
  @@Swap:	XCHG	SI,BX
  @@ChkSpace:	XCHG	CX,AX
		JA	@@Write
		CMP	BL,'0'
		JNE	@@Write
		MOV	BL,' '
  @@Write:	OR	DL,DL
		JE	@@Put1
		CMP	ES:Country.DateFmt,1
		JBE	@@Put1
		STOSW
  @@Put1:	XCHG	AX,BX
		STOSW
		MOV	AL,ES:Country.DateSep
		STOSB
		PUSH	AX
		MOV	AX,SI
		STOSW
		POP	AX
		STOSB
		OR	DL,DL
		JE	@@Put3
		CMP	ES:Country.DateFmt,1
		JA	@@Put3
		MOV	AX,BX
		STOSW
  @@Put3:	MOV	AX,CX
		STOSW
		MOV	BYTE PTR ES:[DI],0
		RET
PutDate		ENDP

;---------------------------------------------------------------
;	Output time
;		DS:SI - address of disk buffer
;		ES:DI - address of buffer
;---------------------------------------------------------------
PutTime		PROC	FAR
		USES	AX,BX,CX,DX
		CMP	ES:Country.TimeFmt,0
		JZ	@@Space
		MOV	AL,' '
		STOSB
  @@Space:	MOV	AX,DS:[SI].TimeF
		PUSH	AX
		AND	AX,1Fh
		ADD	AX,AX
		CALL	HexDec
		MOV	BX,AX
		POP	AX
		MOV	CL,5
		SHR	AX,CL
		PUSH	AX
		AND	AX,3Fh
		CALL	HexDec
		MOV	DX,AX
		POP	AX
		MOV	CL,6
		SHR	AX,CL
		AND	AX,1Fh
		CMP	ES:Country.TimeFmt,0
		JNE	@@ConvHours
		MOV	CL,12
		DIV	CL
		OR	AH,AH
		JNE	@@ChkAP
		MOV	AH,12
  @@ChkAP:	CMP	AL,0
		MOV	AL,AH
		MOV	CH,'a'
		JE	@@ConvHours
		MOV	CH,'p'
  @@ConvHours:	CALL	HexDec
		CMP	AL,'0'
		JNE	@@Write
		MOV	AL,' '
  @@Write:	STOSW
		MOV	AL,ES:Country.TimeSep
		PUSH	AX
		STOSB
		MOV	AX,DX
		STOSW
		POP	AX
		STOSB
		MOV	AX,BX
		STOSW
		CMP	ES:Country.TimeFmt,0
		JNE	@@Exit
		MOV	AL,CH
		STOSB
  @@Exit:	MOV	BYTE PTR ES:[DI],0
		RET
PutTime		ENDP

;---------------------------------------------------------------
;	Output attributes
;		   AL - attributes
;		ES:DI - address of buffer
;---------------------------------------------------------------
		.CONST
Attributes_S	DB	'ASHR'

		.CODE	Subs
PutAttr		PROC	NEAR
		USES	AX,SI
		MOV	AH,AL
		MOV	SI,OFFSET Attributes_S
		SHL	AH,1
		SHL	AH,1
		CALL	SetAttrib
		SHL	AH,1
		SHL	AH,1
		CALL	SetAttrib
		CALL	SetAttrib
		CALL	SetAttrib
		RET
PutAttr		ENDP

SetAttrib	PROC	NEAR
		LODS	BYTE PTR CS:[SI]
		SHL	AH,1
		JC	@@Put
		MOV	AL,'.'
  @@Put:	STOSB
		RET
SetAttrib	ENDP

;---------------------------------------------------------------
;	Output description
;		DS:SI - address of disk buffer
;		ES:DI - address of buffer
;		   CX - length
;---------------------------------------------------------------
		.CODE	Subs
PutDescr	PROC	NEAR
		USES	AX,CX,SI
		INC	CX
		MOV	SI,DS:[SI].DescrF
		OR	SI,SI
		JE	@@EndDescr
  @@MoveDescr:	LODSW
		CMP	AX,0D0Ah
		JE	@@ChkEnd
		CMP	AX,0A0Dh
		JE	@@ChkEnd
		DEC	SI
		CMP	AL,13
		JE	@@ChkEnd
		CMP	AL,10
		JE	@@ChkEnd
		CMP	AL,4
		JE	@@EndDescr
		CMP	AL,9
		JNE	@@SaveChar
  @@Space:	MOV	AL,' '
  @@SaveChar:	STOSB
		LOOP	@@MoveDescr
		DEC	DI
		DEC	DI
		MOV	AL,ES:Scrolls[1]
		STOSB
		JMP	@@Exit
  @@ChkEnd:	LODSB
		CMP	AL,'+'
		JE	@@Space
		CMP	AL,' '
		JE	@@ChkSpace
		CMP	AL,9
		JNE	@@EndDescr
  @@ChkSpace:	LODSW
		CMP	AX,0D0Ah
		JE	@@ChkEnd
		CMP	AX,0A0Dh
		JE	@@ChkEnd
		DEC	SI
		CMP	AL,13
		JE	@@ChkEnd
		CMP	AL,10
		JE	@@ChkEnd
		CMP	AL,' '
		JE	@@ChkSpace
		CMP	AL,9
		JE	@@ChkSpace
		CMP	AL,4
		JE	@@EndDescr
		DEC	SI
		JMP	@@Space
  @@EndDescr:	DEC	CX
		JE	@@Exit
		MOV	AL,' '
		REP	STOSB
  @@Exit:	RET
PutDescr	ENDP

;---------------------------------------------------------------
;	Convert byte to hex form
;		   AL - byte
;	Return:	AL,AH - high and low digits
;---------------------------------------------------------------
		.CODE	Subs
HexDec		PROC	FAR
		USES	CX
		MOV	AH,0
		MOV	CL,10
		DIV	CL
		ADD	AX,'00'
		RET
HexDec		ENDP

;---------------------------------------------------------------
;	Correct first parameter
;		   DS - WCBseg
;---------------------------------------------------------------
CorPar		PROC	FAR
		USES	AX,BX,CX,DX,SI,DI
		CMP	DS:WinTyp,p_Tree
		JA	@@Exit			; Info or QuickView panel
		CALL	AdrNum
		CMP	SI,DI
		JAE	@@ChkFirst
		MOV	DI,SI
  @@ChkFirst:	MOV	CL,DS:WinTyp
		OR	CL,DS:BrfFul
		JNZ	@@SizeWin
		CALL	BriefNumFiles
  @@SizeWin:	DEC	AX
		MOV	CX,SI
		SUB	CX,DI
		SUB	CX,AX
		JBE	@@ChkEnd
		ADD	DI,CX
  @@ChkEnd:	SUB	DX,DI
		SUB	AX,DX
		JBE	@@ConvAddr
		SUB	DI,AX
		JAE	@@ConvAddr
		XOR	DI,DI
  @@ConvAddr:	CALL	ChgAdr
  @@Exit:	RET
CorPar		ENDP

;---------------------------------------------------------------
;	Set cursor in main window
;		   DS - WCBseg
;		 AL=0 - reset cursor
;		 AL=1 - set cursor
;---------------------------------------------------------------
SetCur		PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,ES
		MOV	AH,Back_C
		CMP	AL,0
		JZ	@@SetColor
		MOV	AH,Cursr_C
  @@SetColor:	PUSH	AX
		CALL	AdrNum
		POP	AX
		MOV	CX,SI
		SUB	CX,DI
		CMP	DS:WinTyp,p_Tree
		JA	@@Exit			; Info or QuickView panel
		JE	@@TreeCur
		MOV	SI,DS:FileCur		; File panel
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	@@Exit
		CMP	AL,0
		JNE	@@ColorDir
		CMP	ES:ColorDirs,0
		JE	@@ColorDir
		TEST	DS:[SI].AttrF,10h
		JE	@@ColorDir
		MOV	AH,Dirs_C
  @@ColorDir:	PUSH	AX
		MOV	AX,BX
		CALL	BegBX
		CALL	SizDX
		ADD	BX,201h
		DEC	DX
		DEC	DX
		CMP	DS:BrfFul,0
		JNZ	@@SetXY
		XCHG	AX,CX
		DIV	CL
		MOV	CL,AH
		MOV	AH,AL
		MOV	CH,DL
		CALL	NameWidthAL
		MOV	DL,AL
		INC	AX
		MUL	AH
		ADD	BL,AL
		SUB	CH,AL
		CMP	DL,CH
		JBE	@@SetXY
		MOV	DL,CH
  @@SetXY:	ADD	BH,CL
		POP	AX
		CALL	BasAdr
		TEST	DS:[SI].AttrF,40h
		JZ	@@WriteColor
		JMP	@@SelColor
  @@TreeCur:	PUSH	DS			; Tree
		MOV	DS,ES:TreeSeg
		CMP	SI,DS:TreeLenH
		POP	DS
		JAE	@@Exit
		CALL	BegBX
		CALL	SizDX
		ADD	BX,101h
		ADD	BH,CL
		PUSH	AX
		PUSH	DS
		MOV	AL,DS:LongNames
		MOV	SI,DS:TreeCur
		MOV	DS,ES:TreeSeg
		CALL	TreeNumb2AdrH
		CALL	DirLen1
		INC	AX
		MOV	CL,DS:[SI].DirLevel
		POP	DS
		MOV	CH,CL
		ADD	CH,CL
		CMP	DS:TreeTyp,0
		JNZ	@@SetChars
		ADD	CH,CL
  @@SetChars:	SUB	CH,DS:TreeOffset
		ADD	BL,CH
		SUB	DL,CH
		MOV	DH,0
		DEC	DX
		DEC	DX
		CMP	DX,AX
		JBE	@@CutLen
		MOV	DX,AX
  @@CutLen:	POP	AX
		CALL	BasAdr
		MOV	BYTE PTR ES:[DI],' '
		CMP	SI,OFFSET TreeList
		JBE	@@CurColor
		CMP	AL,0
		JNZ	@@CurColor
		PUSH	AX
		MOV	AH,Back_C
		CALL	Color
		MOV	AL,0C4h ; '─'
		STOSW
		POP	AX
		DEC	DX
  @@CurColor:	CMP	SI,DS:TreeCrnt
		JNE	@@WriteColor
  @@SelColor:	MOV	AH,Slct_C
		CMP	AL,0
		JZ	@@WriteColor
		MOV	AH,CurSl_C
  @@WriteColor:	CALL	Color
		MOV	DH,1
		MOV	CL,DL
		MOV	CH,0
		MOV	AL,AH
		CALL	MvsColr
		INC	DX
		CALL	Window
  @@Exit:	RET
SetCur		ENDP

;---------------------------------------------------------------
;	Test info
;---------------------------------------------------------------
TstInfo		PROC	FAR
		USES	AX,DS
		MOV	AL,DS:WinTyp
		CALL	InvWCB
		CMP	AL,DS:WinTyp
		JAE	@@Exit
		MOV	DS:ChgFlg,1
  @@Exit:	RET
TstInfo		ENDP

;---------------------------------------------------------------
;	BX files and DX directories
;		   AL - display if 0
;		   BX - number of files
;		   DX - number of directories
;		ES:DI - buffer
;---------------------------------------------------------------
		.CONST
FilDi_S	DB	' files and  directoriesy'

		.CODE	Subs
FilDir		PROC	FAR
		USES	AX,CX,SI,DS
		PUSH	CS
		POP	DS
		MOV	AH,AL
		OR	AL,DL
		OR	AL,DH
		JE	@@DirNum
		PUSH	BX
		XOR	BX,BX
		MOV	CX,DX
		MOV	AL,1
		CALL	Number
		POP	BX
		MOV	SI,OFFSET FilDi_S
		MOV	CX,6
		REP	MOVSB
		CMP	DX,1
		JNE	@@And
		DEC	DI
  @@And:	OR	AH,BL
		OR	AH,BH
		JE	@@Exit
		MOV	CX,5
		REP	MOVSB
  @@DirNum:	PUSH	BX
		MOV	CX,BX
		XOR	BX,BX
		MOV	AL,1
		CALL	Number
		POP	BX
		MOV	SI,OFFSET FilDi_S+11
		MOV	CX,12
		REP	MOVSB
		CMP	BX,1
		JNE	@@Exit
		SUB	DI,3
		MOVSB
  @@Exit:	MOV	BYTE PTR ES:[DI],0
		RET
FilDir		ENDP

;---------------------------------------------------------------
;	Selected files and directories
;		   DS - WCB segment
;	Return:	   DX - files
;		   BX - directories
;---------------------------------------------------------------
TotSel		PROC	FAR
		USES	SI
		MOV	SI,DS:DiskBuf
		XOR	BX,BX
		XOR	DX,DX
  @@ChkEnd:	CMP	SI,DS:EndBuf
		JAE	@@Exit
		TEST	DS:[SI].AttrF,40h
		JZ	@@Next
		INC	DX
		TEST	DS:[SI].AttrF,10h
		JZ	@@Next
		DEC	DX
		INC	BX
  @@Next:	CALL	NextFilEnt
		JMP	@@ChkEnd
  @@Exit:	RET
TotSel		ENDP

;---------------------------------------------------------------
;	Another WCB
;---------------------------------------------------------------
InvWCB		PROC	FAR
		PUSH	AX
		MOV	AX,DS
		MOV	DS,ES:WCB1seg
		CMP	AX,ES:WCB1seg
		JNE	@@Exit
		MOV	DS,ES:WCB2seg
  @@Exit:	POP	AX
		RET
InvWCB		ENDP

;---------------------------------------------------------------
;	Find item in extension file
;		DS:SI - file buffer
;		DS:AX - pointer or 0
;	Return:	DS:SI - address of item
;		DS:AX - address of first ext line in group
;		   CY - if not found
;---------------------------------------------------------------
FndItem		PROC	FAR
		MOV	AL,DS:[SI]
		CMP	AL," "
		JE	FindNextItem
		CMP	AL,9
		JE	FindNextItem
		CMP	AL,"'"
		JE	FindNextItem
		CMP	AL,13
		JE	FindNextItem
		CMP	AL,10
		JE	FindNextItem
		CMP	AL,1Ah
		JE	NotFoundItem
		CLC
		RET
FndItem		ENDP

FindNextItem	PROC	FAR
		LODSB
		CMP	AL,13
		JE	FndItem
		CMP	AL,10
		JE	FndItem
		CMP	AL,1Ah
		JNE	FindNextItem
  NotFoundItem:	STC
		RET
FindNextItem	ENDP

;---------------------------------------------------------------
;	Find string in extension file
;		DS:SI - ext file
;		ES:DI - filename
;		   AL - 0-find mask, 1-extension, 2-only main extension
;	Return:	   CY - not found
;		DS:SI - string, 0 - if not found
;		DS:AX - main extension
;---------------------------------------------------------------
FindExt		PROC	FAR
		USES	BX,CX,DX
		LOCAL	NameBuf:BYTE:256+1
		LOCAL	ExtMain:WORD
		MOV	DL,AL
		CALL	FndItem
  @@Item:	JC	@@NotFound
		PUSH	ES
		PUSH	DI
		PUSH	SS
		POP	ES
		LEA	DI,NameBuf
  @@NextMask:	MOV	DH,0
		CMP	BYTE PTR DS:[SI],'|'
		JNE	@@SkipInv
		INC	SI
		CALL	FndLet
		OR	DH,1
  @@SkipInv:	MOV	BX,SI
  @@ChkMask:	LODSB
		CMP	AL,13
		JE	@@MoveMask
		CMP	AL,10
		JE	@@MoveMask
		CMP	AL,1Ah
		JE	@@MoveMask
		CMP	AL,','
		JE	@@MoveMask
		CMP	AL,';'
		JE	@@MoveMask
		CMP	AL,':'
		JE	@@MoveMask
		CMP	AL,'.'
		JNE	@@ChkMask
		OR	DH,2
		JMP	@@ChkMask
  @@MoveMask:	DEC	SI
		PUSH	SI
		MOV	CX,SI
		LEA	SI,NameBuf
		CMP	DI,SI
		JNE	@@ChkFlags
		MOV	ExtMain,BX
  @@ChkFlags:	CMP	DL,1
		JB	@@ChkSize
		JE	@@ChkName
		CMP	DI,SI
		JNE	@@SkipMask
  @@ChkName:	TEST	DH,3
		JNZ	@@SkipMask
  @@ChkSize:	SUB	CX,BX
		MOV	AL,DH
		XOR	AL,2
		CBW
		ADD	AX,CX
		ADD	AX,DI
		ADD	SI,256
		CMP	AX,SI
		JAE	@@SkipMask
		TEST	DH,1
		JZ	@@AddName
		MOV	AL,'|'
		STOSB
  @@AddName:	TEST	DH,2
		JNZ	@@AddExt
		MOV	AX,'.*'
		STOSW
  @@AddExt:	MOV	SI,BX
		REP	MOVSB
		MOV	AL,';'
		STOSB
  @@SkipMask:	POP	SI
		LODSB
		CMP	AL,':'
		JE	@@FindCmd
		CMP	AL,','
		JE	@@Next
		CMP	AL,';'
		JNE	@@CrLf
  @@Next:	CALL	FndLet
		LODSB
  @@CrLf:	CMP	AL,13
		JE	@@FindItem
		CMP	AL,10
		JE	@@FindItem
		CMP	AL,1Ah
		JE	@@ExitMask
		DEC	SI
		JMP	@@NextMask
  @@FindItem:	LODSB
		CMP	AL,13
		JE	@@FindItem
		CMP	AL,10
		JE	@@FindItem
		CMP	AL,1Ah
		JE	@@ExitMask
		CMP	AL,' '
		JE	@@FindCmd
		CMP	AL,9
		JE	@@FindCmd
		DEC	SI
		CMP	AL,"'"
		JNE	@@NextMask
  @@SkipLine:	LODSB
		CMP	AL,13
		JE	@@FindItem
		CMP	AL,10
		JE	@@FindItem
		CMP	AL,1Ah
		JNE	@@SkipLine
		JMP	@@ExitMask
  @@FindCmd:	CALL	FindCommand
		JNC	@@ExitMask
		JNZ	@@NextMask
  @@ExitMask:	CMP	AL,1Ah
		MOV	AL,0
		STOSB
		POP	DI
		POP	ES
		JE	@@NotFound
		CMP	NameBuf[0],0
		JE	@@NextItem
		PUSH	DS
		PUSH	ES
		PUSH	SI
		PUSH	DI
		PUSH	SS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	SI,DI
		LEA	DI,NameBuf
		CALL	TestMask
		POP	DI
		POP	SI
		POP	ES
		POP	DS
		MOV	AX,ExtMain
		JZ	@@Exit
  @@NextItem:	CALL	FindNextItem
		JMP	@@Item
  @@NotFound:	XOR	SI,SI
		STC
  @@Exit:	RET
FindExt		ENDP

;---------------------------------------------------------------
;	Find command in ext file
;		DS:SI - ext file
;	Return:	DS:SI - command
;		   NC - found
;		   CY - not found, ZR - EOF
;---------------------------------------------------------------
FindCommand	PROC	FAR
  @@ChkCommand:	CALL	FndLet
		LODSB
		CMP	AL,13
		JE	@@ChkLine
		CMP	AL,10
		JE	@@ChkLine
		CMP	AL,1Ah
		JE	@@Exit
		DEC	SI
		CLC
		RET
  @@ChkLine:	MOV	AL,DS:[SI]
		CMP	AL,' '
		JE	@@ChkCommand
		CMP	AL,9
		JE	@@ChkCommand
		CMP	AL,13
		JE	@@SkipLine
		CMP	AL,10
		JE	@@SkipLine
		CMP	AL,"'"
		JNE	@@Exit
  @@SkipLine:	LODSB
		CMP	AL,13
		JE	@@ChkLine
		CMP	AL,10
		JE	@@ChkLine
		CMP	AL,1Ah
		JNE	@@SkipLine
  @@Exit:	STC
		RET
FindCommand	ENDP

;---------------------------------------------------------------
;	Check for single line command in ext file
;		DS:SI - ext file contents
;		CS:BX - extension to compare
;		   CX - extension size to compare
;	Return:    NC - not single line
;		   CY - single line, ES:DX = TmpBuf - result file
;				     AL - 0=no path, 1=path present
;---------------------------------------------------------------
SingleLine	PROC	FAR
		USES	BP
		XOR	BP,BP
		CMP	BYTE PTR DS:[SI],'"'
		JNE	@@ChkDrv
		INC	SI
		INC	BP
  @@ChkDrv:	MOV	DI,SI
		MOV	AX,DS:[SI]
		CMP	AH,':'
		JNE	@@ChkName
		CALL	UpCase
		CMP	AL,'A'
		JB	@@Error
		CMP	AL,'Z'
		JA	@@Error
		INC	SI
		INC	SI
  @@ChkName:	LODSB
		CMP	AL,13
		JE	@@ChkQuote
		CMP	AL,10
		JE	@@ChkQuote
		CMP	AL,1Ah
		JE	@@ChkQuote
		CMP	AL,'"'
		JE	@@Quote
		OR	BP,BP
		JNE	@@ChkName
		CMP	AL,' '
		JE	@@ChkExt
		CMP	AL,9
		JE	@@ChkExt
		JMP	@@ChkName
  @@Quote:	OR	BP,BP
		JE	@@Error
		JMP	@@ChkExt
  @@ChkQuote:	OR	BP,BP
		JNE	@@Error
  @@ChkExt:	DEC	SI
		PUSH	SI
		SUB	SI,4
  @@CmpExt:	LODSB
		CALL	UpCase
		INC	BX
		CMP	AL,CS:[BX]
		LOOPE	@@CmpExt
		POP	SI
		JNE	@@Error
		PUSH	SI
		ADD	SI,BP
		CALL	FindCommand
		POP	SI
		JNC	@@Error
		MOV	CX,SI
		SUB	CX,DI
		CMP	CX,LenPath
		JAE	@@Error
		MOV	BX,SI
		MOV	CH,0
		XCHG	CH,DS:[BX]
		MOV	SI,DI
		CALL	CutPath
		CMP	SI,DI
		MOV	DX,OFFSET TmpBuf
		MOV	SI,DX
		MOV	CL,1
		JNE	@@MoveName
		PUSH	DS
		PUSH	ES
		POP	DS
		CALL	CutPath
		POP	DS
		MOV	CL,0
  @@MoveName:	XCHG	SI,DI
		CALL	strcpy
		MOV	DS:[BX],CH
		MOV	AL,CL
		STC
		JMP	@@Exit
  @@Error:	MOV	SI,DI
		SUB	SI,BP
  @@Exit:	RET
SingleLine	ENDP

;---------------------------------------------------------------
;	Leave time
;---------------------------------------------------------------
LeaveTm		PROC	FAR
		USES	AX
		CMP	ES:TaskStack,0
		JE	@@LeaveTime
		CALL	TaskSwitch
		JMP	@@Exit
  @@LeaveTime:	INT	28h
		CMP	ES:WinLeave,0
		JNZ	@@DesqView
		MOV	AX,1680h
		INT	2Fh
		CMP	AL,80h
		JNE	@@DesqView
		MOV	ES:WinLeave,1
  @@DesqView:	CMP	ES:DESQvie,0
		JZ	@@Exit
		MOV	AX,101Ah
		INT	15h
		MOV	AX,1000h
		INT	15h
		MOV	AX,1025h
		INT	15h
  @@Exit:	RET
LeaveTm		ENDP

;---------------------------------------------------------------
;	Task switch
;---------------------------------------------------------------
TaskSwitch	PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,DS,ES
		PUSH	BP
		PUSHF
		XCHG	SP,ES:TaskStack
		POPF
		POP	BP
		RET
TaskSwitch	ENDP

;---------------------------------------------------------------
;	Edit line
;		DS:SI - begin of line
;		   DX - cursor
;		   CX - length of line (high bit means alter edit)
;		   AX - key code
;	Return:	 CY=0 - no change
;		 CY=1 - change
;		   DX - cursor
;---------------------------------------------------------------
		.DATA
EdKeys1		DW	0013h, ELLeftChar	; ^S
		DW	4B00h, ELLeftChar	; Left
		DW	0004h, ELRightChar	; ^D
		DW	4D00h, ELRightChar	; Right
		DW	7300h, ELLeftWord	; ^Left
		DW	7400h, ELRightWord	; ^Right
		DW	4700h, ELHome		; Home
		DW	4F00h, ELEnd		; End
		DW	7700h, ELHome		; ^Home
		DW	7500h, ELEnd		; ^End
		DW	0011h, ELQuote		; ^Q
		DW	0
EdKeys2		DW	0E08h, ELBackSpace	; BackSpace
		DW	0007h, ELDelChar	; ^G
		DW	5300h, ELDelChar	; Del
		DW	0019h, ELDelLine	; ^Y
		DW	000Bh, ELDelEnd		; ^K
		DW	0017h, ELDelWordLeft	; ^W
		DW	007Fh, ELDelWordLeft	; ^BackSpace
		DW	0014h, ELDelWordRight	; ^T
		DW	9300h, ELDelWordRight	; ^Del
		DW	0

		.CODE	Subs
EdLin		PROC	FAR
		USES	AX,BX,CX,SI,DI,DS
		MOV	BX,CX
		AND	CH,7Fh
		MOV	DI,SI
		CMP	ES:Quote,0
		JNZ	@@QuoteChar
		MOV	SI,OFFSET EdKeys1
		CALL	Case
		TEST	BH,80h
		JNE	@@ChkChar
		MOV	SI,OFFSET EdKeys2
		CALL	Case
  @@ChkChar:	OR	AX,AX
		JE	@@NoChanges
		OR	AH,AH
		JE	@@Char
		CMP	AL,' '
		JB	@@NoChanges
  @@Char:	CMP	DX,CX			; Symbols
		JB	@@InsChar
		CALL	Beep
		JMP	@@NoChanges
  @@InsChar:	PUSH	ES
		PUSH	DI
		PUSH	DS
		POP	ES
		TEST	BH,80h
		JNE	@@SetChar
		ADD	DI,CX
		MOV	SI,DI
		DEC	SI
		PUSH	DI
		SUB	CX,DX
		STD
		REP	MOVSB
		CLD
		POP	DI
		MOV	BYTE PTR DS:[DI],0
  @@SetChar:	POP	SI
		POP	ES
		ADD	SI,DX
		XCHG	AL,DS:[SI]
		INC	DX
		OR	AL,AL
		JNE	@@Changes
		MOV	DS:[SI+1],AL
		JMP	@@Changes
  @@QuoteChar:	MOV	ES:Quote,0
		CMP	AL,0
		JNE	@@Char
		JMP	@@NoChanges
  @@Delete:	LODSB				; Delete
		MOV	DS:[DI],AL
		INC	DI
		CMP	AL,0
		JNE	@@Delete
  @@Changes:	STC
		JMP	@@Exit
ELLeftChar:	OR	DX,DX			; Left,   ^S
		JE	@@NoChanges
		DEC	DX
		JMP	@@Changes
ELRightChar:	ADD	DI,DX			; Right,  ^D
		CMP	BYTE PTR DS:[DI],0
		JE	@@NoChanges
		INC	DX
		JMP	@@Changes
ELLeftWord:	CALL	ELWordLeft		; ^Left,  ^A
		JMP	@@Changes
ELRightWord:	CALL	ELWordRight		; ^Right, ^F
		JMP	@@Changes
ELHome:		XOR	DX,DX			; Home
		JMP	@@Changes
ELEnd:		MOV	SI,DI			; End
		MOV	AL,0
		INC	CX
		PUSH	ES
		PUSH	DS
		POP	ES
		REPNE	SCASB
		POP	ES
		DEC	DI
		MOV	DX,DI
		SUB	DX,SI
		JMP	@@Changes
ELBackSpace:	OR	DX,DX			; BackSpace
		JE	@@NoChanges
		DEC	DX
ELDelChar:	ADD	DI,DX			; Del, ^G
		CMP	BYTE PTR DS:[DI],0
		JE	@@NoChanges
		LEA	SI,[DI+1]
		JMP	@@Delete
ELDelLine:	XOR	DX,DX			; ^Y
ELDelEnd:	ADD	DI,DX			; ^K
		MOV	BYTE PTR DS:[DI],0
		JMP	@@Changes
ELDelWordLeft:	CALL	ELWordLeft		; ^BackSpace, ^W
		JMP	@@Delete
ELDelWordRight:	PUSH	DX			; ^T
		CALL	ELWordRight
		POP	DX
		JMP	@@Delete
ELQuote:	MOV	ES:Quote,1		; ^Q
  @@NoChanges:	CLC
  @@Exit:	RET
EdLin		ENDP

ELWordLeft	PROC	NEAR			; Find word left
		ADD	DI,DX
		MOV	SI,DI
  @@SkipSpace:	OR	DX,DX
		JE	@@Exit
		DEC	DX
		DEC	DI
		MOV	AL,DS:[DI]
		CALL	CmpSym
		JE	@@SkipSpace
  @@SkipWord:	OR	DX,DX
		JE	@@Exit
		DEC	DX
		DEC	DI
		MOV	AL,DS:[DI]
		CALL	CmpSym
		JNE	@@SkipWord
		INC	DX
		INC	DI
  @@Exit:	CMP	SI,DI
		RET
ELWordLeft	ENDP

ELWordRight	PROC	NEAR			; Find word right
		ADD	DI,DX
		MOV	SI,DI
  @@SkipSpace:	CMP	BYTE PTR DS:[SI],0
		JE	@@Exit
		MOV	AL,DS:[SI]
		CALL	CmpSym
		JNE	@@SkipWord
		INC	SI
		INC	DX
		JMP	@@SkipSpace
  @@SkipWord:	CMP	BYTE PTR DS:[SI],0
		JE	@@Exit
		INC	SI
		INC	DX
		MOV	AL,DS:[SI]
		CALL	CmpSym
		JNE	@@SkipWord
  @@Exit:	CMP	SI,DI
		RET
ELWordRight	ENDP

;---------------------------------------------------------------
;	Compare symbol
;		   AL - Symbol
;---------------------------------------------------------------
		.DATA
CtEn_T		DB	' +:.,><|\"',9

		.CODE	Subs
CmpSym		PROC	FAR
		USES	CX,DI,ES
		PUSH	CS
		POP	ES
		MOV	DI,OFFSET CtEn_T
		MOV	CX,11
		REPNE	SCASB
		RET
CmpSym		ENDP

;---------------------------------------------------------------
;	Page Up
;		   SI - cursor
;		   DI - first
;		   AX - size of page
;---------------------------------------------------------------
PageUp		PROC	FAR
		OR	DI,DI
		JNE	@@FirstUp
		XOR	SI,SI
		JMP	@@Exit
  @@FirstUp:	DEC	AX
		SUB	DI,AX
		JAE	@@CurUp
		XOR	DI,DI
  @@CurUp:	ADD	AX,DI
		CMP	SI,AX
		JBE	@@Exit
		MOV	SI,AX
  @@Exit:	RET
PageUp		ENDP

;---------------------------------------------------------------
;	Page Down
;		   SI - cursor
;		   DI - first
;		   AX - size of page
;		   BP - max line
;---------------------------------------------------------------
PageDown	PROC	FAR
		PUSH	SI
		MOV	SI,BP
		SUB	BP,AX
		JAE	@@ChkEnd
		XOR	BP,BP
  @@ChkEnd:	DEC	AX
		CMP	DI,BP
		JB	@@FirstDown
		POP	AX
		OR	SI,SI
		JE	@@Exit
		DEC	SI
		JMP	@@Exit
  @@FirstDown:	POP	SI
		ADD	DI,AX
		JC	@@SetFirst
		CMP	DI,BP
		JBE	@@CurDown
  @@SetFirst:	MOV	DI,BP
  @@CurDown:	CMP	SI,DI
		JAE	@@Exit
		MOV	SI,DI
  @@Exit:	RET
PageDown	ENDP

;---------------------------------------------------------------
;	Find file in path
;		DS:SI - filename
;	Return:	DS:SI - full filename
;		   AX - error code (CY=1)
;---------------------------------------------------------------
		.CONST
PathEnv		DB	'PATH='
ExeExt_S	DB	'.exe',0
ComExt_S	DB	'.com',0

		.CODE	Subs
FindPath	PROC	FAR
		USES	BX,CX,DX,SI,DI,BP,DS,ES
		PUSH	DS			; Check file without path
		POP	ES
		MOV	DX,SI
		MOV	BX,SI
		CALL	CutPath
		CMP	SI,DX
		JBE	@@Find
		MOV	DI,SI
		CALL	FindPathMove
		JNZ	@@Exit
		PUSH	CS
		MOV	SI,OFFSET Empty_S
		MOV	BP,2
		JMP	@@CheckExt

  @@Find:	MOV	SI,OFFSET VcSet		; Find in PATH
		XOR	BP,BP
  @@AddPath:	CALL	FndLet
		LODSB
		CMP	AL,';'
		JE	@@AddPath
		DEC	SI
		CMP	AL,0
		JE	@@NextPath
		PUSH	DS
		PUSH	SI
		PUSH	ES
		POP	DS
		MOV	SI,BX
		MOV	DI,OFFSET TmpErr
		CALL	strcpy
		POP	SI
		POP	DS
		MOV	CX,SI
  @@PathLen:	LODSB
		CMP	AL,';'
		JE	@@MovePath
		CMP	AL,0
		JNE	@@PathLen
  @@MovePath:	DEC	SI
		MOV	AX,SI
		SUB	AX,CX
		CMP	AX,LenPath
		JA	@@AddPath
		MOV	SI,CX
		MOV	DI,DX
		MOV	CX,AX
		REP	MOVSB
		CALL	AddPathSep
		MOV	BX,DI
		PUSH	SI
		MOV	SI,OFFSET TmpErr
		CALL	FindPathMove
		POP	SI
		PUSH	DS
		JNZ	@@CheckFile
  @@CheckExt:	PUSH	SI
		PUSH	DI
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET ExeExt_S
		CALL	strcpy
		POP	DI
		POP	SI
		PUSH	ES
		POP	DS
		CALL	ChgDisk
		POP	DS
		MOV	AX,53h
		JC	@@Error
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	AX,4300h
		CALL	Intr21
		POP	DS
		JC	@@NotCom
		TEST	CX,18h
		JZ	@@Exit
		MOV	AL,0
  @@NotCom:	CMP	AX,53h
		JE	@@Error
		PUSH	DS
		PUSH	SI
		PUSH	DI
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET ComExt_S
		CALL	strcpy
		POP	DI
		POP	SI
  @@CheckFile:	PUSH	ES
		POP	DS
		CALL	ChgDisk
		POP	DS
		MOV	AX,53h
		JC	@@Error
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	AX,4300h
		CALL	Intr21
		POP	DS
		JC	@@NotExe
		TEST	CX,18h
		JZ	@@Exit
		MOV	AL,0
  @@NotExe:	MOV	BYTE PTR ES:[DI],0
		CMP	AX,53h
		JE	@@Error
		JMP	@@AddPath

  @@NextPath:	INC	BP
		XOR	AX,AX
		MOV	SI,OFFSET VcDir
		CMP	BP,2
		JB	@@AddPath
		JA	@@Error

		MOV	DS,CS:ResidentStack.Segm ; Find env var 'PATH='
		MOV	DS,DS:[2Ch]
		MOV	DI,OFFSET PathEnv
		MOV	CX,5
		XOR	SI,SI
		XOR	AX,AX
  @@FindEnv1:	CMP	AL,DS:[SI]
		JE	@@Error
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	CX
		PUSH	CS
		POP	ES
		REPE	CMPSB
		POP	CX
		POP	SI
		POP	DI
		POP	ES
		JE	@@FindEnv3
  @@FindEnv2:	LODSB
		CMP	AL,0
		JNE	@@FindEnv2
		JMP	@@FindEnv1
  @@FindEnv3:	ADD	SI,CX
		JMP	@@AddPath

  @@Error:	PUSH	AX
		MOV	SI,BX
		MOV	DI,DX
		CALL	FindPathMove
		POP	AX
		STC
  @@Exit:	RET
FindPath	ENDP

FindPathMove	PROC	NEAR
		MOV	AH,0
  @@GetChar:	LODS	BYTE PTR ES:[SI]
		CMP	AL,'.'
		JNE	@@SaveChar
		MOV	AH,AL
  @@SaveChar:	STOSB
		CMP	AL,0
		JNE	@@GetChar
		DEC	DI
		OR	AH,AH
		RET
FindPathMove	ENDP

;---------------------------------------------------------------
;	Get disk info
;		   AL - disk letter
;		ES:DI - buffer (cluster, total, free)
;	Return:	   CY - error, AX - code
;---------------------------------------------------------------
GetDiskInfo	PROC	FAR
		USES	BX,CX,DX,SI,DI,DS
		LOCAL	DiskBuffer:BYTE:4
		LOCAL	InfoBuffer:BYTE:3Ch
		MOV	BL,AL
		CALL	Phantom
		MOV	AX,53h
		JC	@@Exit
		PUSH	ES			; FAT 32
		PUSH	DI
		PUSH	SS
		PUSH	SS
		POP	DS
		POP	ES
		LEA	DX,DiskBuffer
		MOV	DI,DX
		MOV	AL,BL
		MOV	AH,':'
		STOSW
		MOV	AX,'\'
		STOSW
		LEA	DI,InfoBuffer
		MOV	WORD PTR DS:[DI+2],0
		MOV	CX,3Ch
		MOV	AX,7303h
		CALL	Intr21
		POP	SI
		POP	ES
		JNC	@@SaveInfo
		CMP	AX,53h
		JNE	@@Fat16
		STC
		JMP	@@Exit
  @@SaveInfo:	MOV	AX,DS:[DI+10h].Lo
		MOV	ES:[SI+4].Lo,AX
		MOV	AX,DS:[DI+10h].Hi
		MOV	ES:[SI+4].Hi,AX
		MOV	AX,DS:[DI+0Ch].Lo
		MOV	ES:[SI+8].Lo,AX
		MOV	AX,DS:[DI+0Ch].Hi
		MOV	ES:[SI+8].Hi,AX
		PUSH	ES
		PUSH	SI
		PUSH	SS
		POP	ES
		LEA	BX,[DI+4]
		LEA	SI,[DI+8]
		LEA	DI,[DI+34h]
		CALL	LongMul
		POP	SI
		POP	ES
		MOV	AX,DS:[DI+0]
		MOV	DX,DS:[DI+2]
		MOV	BX,DS:[DI+4]
		OR	BX,DS:[DI+6]
		JE	@@SetCluster
		MOV	AX,0FFFFh
		MOV	DX,AX
		JMP	@@SetCluster
  @@Fat16:	MOV	DL,DiskBuffer[0]	; FAT 16
		SUB	DL,'A'-1
		MOV	AH,36h
		CALL	Intr21
		JC	@@Exit
		MOV	ES:[SI+4].Lo,DX
		MOV	ES:[SI+4].Hi,0
		MOV	ES:[SI+8].Lo,BX
		MOV	ES:[SI+8].Hi,0
		MUL	CX
  @@SetCluster:	MOV	ES:[SI+0].Lo,AX
		MOV	ES:[SI+0].Hi,DX
		MOV	CL,DiskBuffer[0]	; Check CD-ROM size
		SUB	CL,'A'
		MOV	CH,0
		XOR	BX,BX
		MOV	AX,150Bh
		INT	2Fh
		CMP	BX,0ADADh
		JNE	@@ChkCluster
		OR	AX,AX
		JE	@@ChkCluster
		PUSH	ES
		PUSH	SS
		POP	ES
		LEA	DI,InfoBuffer
		PUSH	DI
		PUSH	CX
		MOV	CX,5+14h
		MOV	AL,0
		REP	STOSB
		POP	CX
		POP	DI
		MOV	BYTE PTR DS:[DI],8
		LEA	BX,[DI+5]
		MOV	BYTE PTR DS:[BX+0],14h
		MOV	BYTE PTR DS:[BX+2],3
		MOV	DS:[BX+0Eh].Offs,DI
		MOV	DS:[BX+0Eh].Segm,ES
		MOV	WORD PTR DS:[BX+12h],5
		MOV	AX,1510h
		INT	2Fh
		POP	ES
		TEST	WORD PTR DS:[BX+3],8000h
		JNZ	@@ChkCluster
		MOV	AX,DS:[DI+1].Lo
		MOV	BX,DS:[DI+1].Hi
		MOV	CX,AX
		OR	CX,BX
		JE	@@ChkCluster
		MOV	ES:[SI+0].Lo,800h
		MOV	ES:[SI+0].Hi,0
		MOV	ES:[SI+4].Lo,AX
		MOV	ES:[SI+4].Hi,BX
  @@ChkCluster:	MOV	AX,ES:[SI+0].Lo
		OR	AX,ES:[SI+0].Hi
		JNE	@@Exit
		MOV	ES:[SI+0].Lo,1
  @@Exit:	RET
GetDiskInfo	ENDP
