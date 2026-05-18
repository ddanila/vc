;---------------------------------------------------------------
;		Pull Down Menu
;
;		Version:	22.06.2000
;---------------------------------------------------------------

		.DATA
MnuTab		DW	102h, 161Bh, MenuLn1, MnuHlp1
		DW	10Ah, 141Eh, MenuLn2, MnuHlp2
		DW	113h, 1123h, MenuLn3, MnuHlp3
		DW	11Fh, 1622h, MenuLn4, MnuHlp4
		DW	12Ah, 161Bh, MenuLn1, MnuHlp1
MnuHlp1		DB	LeftRight_H
		DB	BriefFull_H
		DB	BriefFull_H
		DB	BriefFull_H
		DB	Info_H
		DB	Tree_H
		DB	QuickView_H
		DB	OnOff_H
		DB	LongNames_H
		DB	0
		DB	Sort_H
		DB	Sort_H
		DB	Sort_H
		DB	Sort_H
		DB	Sort_H
		DB	Sort_H
		DB	0
		DB	ReRead_H
		DB	Filter_H
		DB	Drive_H
MnuHlp2		DB	Files_H
		DB	0
		DB	Menu_H
		DB	View_H
		DB	Edit_H
		DB	Copy_H
		DB	Move_H
		DB	MkDir_H
		DB	Delete_H
		DB	0
		DB	Attrib_H
		DB	0
		DB	Select_H
		DB	Select_H
		DB	Select_H
		DB	Select_H
		DB	0
		DB	Quit_H
MnuHlp3		DB	Commands_H
		DB	ChgTree_H
		DB	Label_H
		DB	Memory_H
		DB	DirSize_H
		DB	FileFind_H
		DB	History_H
		DB	EgaLines_H
		DB	Mode_H
		DB	0
		DB	SwapPan_H
		DB	OnOffPan_H
		DB	CompDirs_H
		DB	0
		DB	EditMenu_H
MnuHlp4		DB	Options_H
		DB	General_H
		DB	Interface_H
		DB	Chars_H
		DB	PanelOpt_H
		DB	Descript_H
		DB	ScrSavOpt_H
		DB	CopyOpt_H
		DB	0
		DB	EditExt_H
		DB	EditView_H
		DB	EditEdit_H
		DB	0
		DB	AutoMenu_H
		DB	KeyBar_H
		DB	FullScr_H
		DB	Status_H
		DB	Alloc_H
		DB	0
		DB	SaveSet_H
MnuKey		DB	'LFCOR'
KeyTab		DW	5E00h,0701h, 310Eh,0801h, 1312h,1101h, 2106h,1201h
		DW	6800h,1301h, 3B00h,0102h, 3C00h,0202h, 3D00h,0302h
		DW	3E00h,0402h, 3F00h,0502h, 4000h,0602h, 4100h,0702h
		DW	4200h,0802h, 5500h,0202h, 5600h,0302h, 5700h,0402h
		DW	5800h,0502h, 5900h,0602h, 5A00h,0702h, 5B00h,0802h
		DW	6200h,0502h, 6300h,0602h, 6500h,0802h, 1E01h,0A02h
		DW	4E2Bh,0C02h, 4A2Dh,0D02h, 372Ah,0E02h, 320Dh,0F02h
		DW	9000h,0C02h, 8E00h,0D02h, 9600h,0E02h, 4400h,1102h
		DW	7100h,0103h, 6100h,0203h, 6C00h,0303h, 6D00h,0403h
		DW	6E00h,0503h, 6F00h,0603h, 7000h,0703h, 6600h,0803h
		DW	1615h,0A03h, 180Fh,0B03h, 2E03h,0C03h, 3002h,0E04h
		DW	5C00h,1304h, 5F00h,0705h, 310Eh,0805h, 1312h,1105h
		DW	2106h,1205h, 6900h,1305h
KeyTabE		LABEL	WORD

		.CONST
MenuLn1		DB	' ^Brief'			,0
		DB	' ^Full'			,0
		DB	' D^escription'			,0
		DB	' ^Info'			,0
		DB	' ^Tree'			,0
		DB	' ^Quick view'			,0
		DB	' ^On/Off'    ,0FFh,7,'Ctrl-F1'	,0
		DB	' Long n^ames',0FFh,3,'Ctrl-N'	,0
		DB					 0
		DB	' ^Name'			,0
		DB	' E^xtension'			,0
		DB	' Ti^me'			,0
		DB	' ^Size'			,0
		DB	' ^Unsorted'			,0
		DB	' Re^verse'			,0
		DB					 0
		DB	' ^Re-read'  ,0FFh,6,'Ctrl-R'	,0
		DB	' Fi^lter...',0FFh,4,'Ctrl-F'	,0
		DB	' ^Drive...' ,0FFh,5,'Alt-F1'	,0
MenuLn2		DB	'^Help'		 ,0FFh,14,'F1'	 ,0
		DB	'^User menu'	 ,0FFh, 9,'F2'	 ,0
		DB	'^View'		 ,0FFh,14,'F3'	 ,0
		DB	'^Edit'		 ,0FFh,14,'F4'	 ,0
		DB	'^Copy'		 ,0FFh,14,'F5'	 ,0
		DB	'^Rename or move',0FFh, 4,'F6'	 ,0
		DB	'^Make directory',0FFh, 4,'F7'	 ,0
		DB	'^Delete'	 ,0FFh,12,'F8'	 ,0
		DB					  0
		DB	'^Attributes'  ,0FFh,8,'Ctrl-A'	 ,0
		DB					  0
		DB	'Select ^group'	 ,0FFh,6,'Gray +',0
		DB	'U^nselect group',0FFh,4,'Gray -',0
		DB	'^Invert group'	 ,0FFh,6,'Gray *',0
		DB	'Re^store selection Ctrl-M'	 ,0
		DB					  0
		DB	'^Quit'		 ,0FFh,14,'F10'	 ,0
MenuLn3		DB	'^Tree'		      ,0FFh,18,'Alt-F10',0
		DB	'Volume ^label'	      ,0FFh,10,'Ctrl-F4',0
		DB	'Memory ^info'	      ,0FFh,11,'Alt-F5'	,0
		DB	'^Directory sizes'    ,0FFh, 7,'Alt-F6'	,0
		DB	'^Find file'	      ,0FFh,13,'Alt-F7'	,0
		DB	'^History'	      ,0FFh,15,'Alt-F8'	,0
		DB	'^EGA lines'	      ,0FFh,13,'Alt-F9'	,0
		DB	'^Video mode'	      ,0FFh,12,'Ctrl-F9',0
		DB					     0
		DB	'^Swap panels'	      ,0FFh,11,'Ctrl-U'	,0
		DB	'^Panels on/off'      ,0FFh, 9,'Ctrl-O'	,0
		DB	'^Compare directories',0FFh, 3,'Ctrl-C'	,0
		DB						 0
		DB	'^Menu file edit'			,0
MenuLn4		DB	' ^General...'			,0
		DB	' ^Interface...'		,0
		DB	' C^haracters...'		,0
		DB	' ^Panels...'			,0
		DB	' ^Descriptions...'		,0
		DB	' Sc^reen saver...'		,0
		DB	' ^Copy and Move...'		,0
		DB					 0
		DB	' E^xtension file edit...'	,0
		DB	' ^Viewers...'			,0
		DB	' ^Editors...'			,0
		DB					 0
		DB	' ^Auto menus'			,0
		DB	' ^Key bar'    ,0FFh,12,'Ctrl-B',0
		DB	' ^Full screen'			,0
		DB	' Mi^ni status'			,0
		DB	' ^Memory allocation'		,0
		DB					 0
		DB	' ^Save setup',0FFh,9,'Shift-F9',0

		.DATA
Menu_T		DW	Menu1_T
		DW	Menu2_T
		DW	Menu3_T
		DW	Menu4_T
		DW	MenuS_T
		DW	MenuC_T
Menu1_T		DW	M01_00			; Brief
		DW	M01_00			; Full
		DW	M01_00			; Description
		DW	M04_00			; Info
		DW	M04_00			; Tree
		DW	M04_00			; Quick view
		DW	M07_00			; On/Off
		DW	M08_01			; Long names
		DW	0
		DW	M10_00			; Name
		DW	M10_00			; Extension
		DW	M10_00			; Time
		DW	M10_00			; Size
		DW	M10_00			; Unsorted
		DW	M15_00			; Reverse
		DW	0
		DW	F17_01			; Re-read
		DW	M18_01			; Filter...
		DW	F62_01			; Drive...
Menu2_T		DW	0	;M21_00		; Help
		DW	F32_00			; Menu
		DW	F33_00			; View
		DW	F34_00			; Edit
		DW	F35_00			; Copy
		DW	F36_00			; Rename or move
		DW	F37_00			; Make directory
		DW	F38_00			; Delete
		DW	0
		DW	F87_00			; Attributes
		DW	0
		DW	F09_00			; Gray +
		DW	F10_00			; Gray -
		DW	F76_10			; Gray *
		DW	F78_00			; ^M
		DW	0
		DW	F30_00			; Quit
Menu3_T		DW	F60_00			; Tree
		DW	F54_00			; Volume label
		DW	F65_00			; Memory info
		DW	F66_00			; Directory sizes
		DW	F67_00			; Find file
		DW	F68_00			; History
		DW	F69_00			; EGA lines
		DW	F59_00			; Video mode
		DW	0
		DW	F15_00			; Swap panels
		DW	F11_00			; Panels on/off
		DW	M52_01			; Compare directories
		DW	0
		DW	0	;M54_00		; Menu file edit...
Menu4_T		DW	M61_00			; General...
		DW	M62_00			; Interface...
		DW	M63_00			; Characters...
		DW	M64_00			; Panels...
		DW	M65_00			; Descriptions...
		DW	0	;M66_00		; Screen saver...
		DW	M67_00			; Copy and Move...
		DW	0
		DW	0	;M69_00		; Extension file edit...
		DW	0	;M70_00		; Viewer...
		DW	0	;M71_00		; Editor...
		DW	0
		DW	M73_00			; Auto menus
		DW	F16_00			; Key bar
		DW	M75_00			; Full screen
		DW	M76_00			; Mini status
		DW	M77_00			; Memory location
		DW	0
		DW	F49_00			; Save setup
MenuS_T		DW	0	;M21_00		; Help
		DW	F42_00			; Menu
		DW	F43_00			; View...
		DW	F44_00			; Edit...
		DW	F45_00			; Copy...
		DW	F46_00			; Rename or move...
		DW	F47_00			; Make directory...
		DW	F48_00			; Delete...
		DW	0
		DW	F87_00			; Attributes
		DW	0
		DW	F09_01			; Gray +
		DW	F10_00			; Gray -
		DW	F76_01			; Gray *
MenuC_T		DW	0	;M21_00		; Help
		DW	F32_00			; Menu
		DW	F33_00			; View...
		DW	F34_00			; Edit...
		DW	F55_00			; Copy...
		DW	F56_00			; Rename or move...
		DW	F37_00			; Make directory...
		DW	F58_00			; Delete...
		DW	0
		DW	F87_00			; Attributes
		DW	0
		DW	F09_02			; Gray +
		DW	F10_02			; Gray -
		DW	F76_02			; Gray *

MenuTab		DW	4B00h, F39_26		; Left
		DW	4D00h, F39_27		; Right
		DW	4700h, F39_30		; Home
		DW	4900h, F39_30		; PgUp
		DW	4F00h, F39_32		; End
		DW	5100h, F39_32		; PgDn
		DW	4800h, F39_33		; Up
		DW	5000h, F39_37		; Down
		DW	000Dh, F39_43		; Enter
		DW	000Ah, F39_43		; ^Enter
		DW	001Bh, F39_46		; Esc
		DW	0FD00h,F39_46		; Alt
		DW	0

MnuList		EQU	BYTE PTR TmpBuf+0	; List of hot keys
MnuMous		EQU	BYTE PTR TmpBuf+200	; Mouse status

		.CODE
F40_00:		MOV	CX,ES:LastMnu		; Shift F10 - pull down menu
		JMP	F39_01

F39_00:		MOV	CX,1			; F9 - pull down menu
		CMP	CH,DS:Visible
		JE	F39_01
		CMP	CH,ES:Active
		JE	F39_01
		MOV	CL,5
F39_01:		CALL	HidCur
		PUSH	DS
		PUSH	ES
		POP	DS
		XOR	BX,BX
		MOV	DL,DS:Columns
		MOV	DH,1
		CALL	SavWin
		MOV	AL,Dialog_K
		CALL	PutKey
F39_02:		CALL	F40_40
		JZ	F39_05
		OR	AH,AH
		JNE	F39_05
		CALL	F40_50
		MOV	CL,AL
		CMP	AL,0
		JNE	F39_05
F39_03:		MOV	AL,None_M
		CALL	PutMnu1
		MOV	CH,0
F39_04:		CALL	LeaveTm
		CALL	F40_40
		JZ	F39_07
		OR	AH,AH
		JNE	F39_04
		CALL	F40_50
		CMP	AL,0
		JE	F39_04
F39_05:		OR	CH,CH
		JE	F39_08
F39_06:		JMP	F39_51
F39_07:		JMP	F39_90
F39_08:		MOV	AL,CL
		CALL	PutMnu1
F39_10:		PUSH	DS			; Input
		MOV	DS,CS:SubsSeg
		MOV	AL,8
		MUL	CL
		MOV	SI,AX
		MOV	SI,DS:MnuTab[SI-2]
		MOV	AL,CH
		CMP	AL,0FFh
		JE	F39_11
		ADD	SI,AX
F39_11:		MOV	AL,DS:[SI]
		POP	DS
		MOV	DS:HlpPage,AL
		CALL	F40_40			; Check mouse
		JZ	F39_15
		OR	AH,AH
		JNE	F39_17
		CALL	F40_50
		OR	CH,CH
		JE	F39_14
		CMP	AL,CL
		JNE	F39_13
F39_12:		MOV	AL,0FFh
		JMP	F39_18
F39_13:		CALL	ResWin
		CMP	AL,0
		JNE	F39_14
		CALL	Window
F39_14:		MOV	CL,AL
		MOV	CH,0FFh
		CMP	AL,0
		JNE	F39_06
		JMP	F39_02
F39_15:		TEST	SI,7
		JZ	F39_19
		OR	AH,AH
		JNE	F39_16
		CMP	CH,0FFh
		JNE	F39_19
		JMP	F39_39
F39_16:		MOV	AX,SI
		MOV	DS:MnuMous,AL
		JMP	F39_44
F39_17:		OR	CH,CH
		JE	F39_03
		PUSH	DX
		PUSH	CX
		PUSH	BX
		ADD	BX,101h
		SUB	DX,304h
		MOV	CX,AX
		CALL	InWind
		POP	BX
		POP	CX
		POP	DX
		JC	F39_12
		MOV	AL,AH
		DEC	AX
		CBW
		MOV	SI,AX
		CMP	DS:MnuList[SI-1],0
		JE	F39_12
F39_18:		CMP	AL,CH
		JNE	F39_41
		CALL	LeaveTm
		JMP	F39_10
F39_19:		MOV	AL,Dialog_K
		CALL	Input
		MOV	SI,OFFSET MenuTab
		CALL	Case
		CALL	UpCase			; Letters
		OR	CH,CH
		JNE	F39_21
		PUSH	CX
		MOV	ES,CS:SubsSeg
		MOV	DI,OFFSET MnuKey
		MOV	CX,5
		REPNE	SCASB
		PUSH	DS
		POP	ES
		POP	CX
		JNE	F39_20
		MOV	AX,DI
		SUB	AX,OFFSET MnuKey
		MOV	CL,AL
		JMP	F39_50
F39_20:		CMP	AX,4400h
		JNE	F39_31
		JMP	F39_46
F39_21:		MOV	ES,CS:SubsSeg
		MOV	DI,OFFSET KeyTab
F39_22:		SCASW
		JE	F39_25
F39_23:		INC	DI
		INC	DI
		CMP	DI,OFFSET KeyTabE
		JB	F39_22
F39_24:		PUSH	DS
		POP	ES
		CMP	AL,0
		JE	F39_20
		PUSH	CX
		MOV	DI,OFFSET MnuList
		MOV	CL,DH
		SUB	CL,3
		MOV	CH,0
		REPNE	SCASB
		POP	CX
		JNE	F39_31
		MOV	AX,DI
		SUB	AX,OFFSET MnuList
		MOV	CH,AL
		JMP	F39_44
F39_25:		CMP	CL,ES:[DI]
		JNE	F39_23
		MOV	AL,ES:[DI+1]
		CBW
		MOV	SI,AX
		CMP	AH,DS:MnuList[SI-1]
		JE	F39_24
		MOV	CX,ES:[DI]
		PUSH	DS
		POP	ES
		JMP	F39_44
F39_26:		DEC	CL			; Left
		JNE	F39_28
		MOV	CL,5
		JMP	F39_28
F39_27:		INC	CX			; Right
		CMP	CL,5
		JBE	F39_28
		MOV	CL,1
F39_28:		OR	CH,CH
		JE	F39_08
		CALL	ResWin
F39_29:		JMP	F39_50
F39_30:		OR	CH,CH			; Home, PgUp
		JNE	F39_39
F39_31:		JMP	F39_10
F39_32:		OR	CH,CH			; End, PgDn
		JNE	F39_35
		JMP	F39_10
F39_33:		OR	CH,CH			; Up
		JE	F39_31
		MOV	AL,CH
F39_34:		DEC	AL
		JNE	F39_36
F39_35:		MOV	AL,DH
		SUB	AL,3
F39_36:		CBW
		MOV	SI,AX
		CMP	AH,DS:MnuList[SI-1]
		JE	F39_34
		JMP	F39_41
F39_37:		OR	CH,CH			; Down
		JE	F39_29
		MOV	AL,CH
F39_38:		INC	AX
		MOV	AH,DH
		SUB	AH,2
		CMP	AL,AH
		JB	F39_40
F39_39:		MOV	AL,1
F39_40:		CBW
		MOV	SI,AX
		CMP	AH,DS:MnuList[SI-1]
		JE	F39_38
F39_41:		CMP	AL,CH
		JE	F39_31
		PUSH	AX			; Delete cursor
		MOV	AH,MnuTx_C
		MOV	AL,MnuBr_C
		CALL	F40_10
		POP	AX
		MOV	CH,AL
F39_42:		MOV	AH,MnuCu_C		; Set cursor
		MOV	AL,MnuCB_C
		CALL	F40_10
		JMP	F39_10
F39_43:		OR	CH,CH			; Enter
		JE	F39_50
F39_44:		CALL	ResWin
		CALL	Window
F39_45:		JMP	F39_90
F39_46:		OR	CH,CH			; Esc, Alt
		JE	F39_45
		CALL	ResWin
		CALL	Window
		MOV	CH,0
		CMP	AX,0FD00h
		JNE	F39_45
		JMP	F39_10
F39_50:		MOV	CH,1			; Window
F39_51:		MOV	AL,CL
		CALL	PutMnu1
		PUSH	CX
		DEC	CX
		MOV	AL,8
		MUL	CL
		MOV	SI,AX
		ADD	SI,OFFSET MnuTab
		MOV	DS,CS:SubsSeg
		LODSW
		MOV	BX,AX
		LODSW
		MOV	DX,AX
		LODSW
		CALL	SavWin
		PUSH	DX
		PUSH	BX
		PUSH	AX
		MOV	AH,MnuBx_C
		CALL	Color
		PUSH	DX
		SUB	DL,4
		MOV	DH,0
		XOR	SI,SI
		MOV	CX,WORD PTR ES:MenuBords[0]
		MOV	AL,ES:MenuBords[2]
		CALL	BoxLine
		POP	CX
		POP	SI
		MOV	CL,CH
		SUB	CL,3
		MOV	CH,0
		MOV	BP,OFFSET MnuList
F39_52:		PUSH	CX
		PUSH	ES
		PUSH	BP
		PUSH	AX
		PUSH	SI
		XOR	SI,SI
		MOV	CX,WORD PTR ES:MenuBords[6]
		MOV	AL,0
		CALL	ShadLin
		DEC	BH
		POP	SI
		MOV	AL,ES:MenuBords[8]
		CALL	BasAdr
		ADD	DI,4
		MOV	CX,DX
		DEC	CX
		DEC	CX
		XOR	BP,BP
		CMP	BYTE PTR DS:[SI],0
		JNE	F39_54
		INC	SI
		REP	STOSW
		JMP	F39_57
F39_53:		LODSB
		SUB	CL,AL
		PUSH	CX
		MOV	CL,AL
		MOV	CH,0
		MOV	AL,' '
		REP	STOSW
		POP	CX
		JMP	F39_55
F39_54:		MOV	AH,MnuTx_C
		CALL	Color
		MOV	ES:[DI-1],AH
F39_55:		LODSB
		CMP	AL,0FFh
		JE	F39_53
		PUSH	AX
		CMP	AL,'^'
		JNE	F39_56
		LODSB
		PUSH	AX
		CALL	UpCase
		MOV	BP,AX
		POP	AX
		MOV	AH,MnuBr_C
		CALL	Color
F39_56:		STOSW
		POP	AX
		DEC	CX
		CMP	BYTE PTR DS:[SI],0
		JNE	F39_55
		INC	SI
		INC	CX
		MOV	AL,' '
		REP	STOSW
F39_57:		MOV	CX,BP
		POP	AX
		POP	BP
		POP	ES
		MOV	ES:[BP],CL
		INC	BP
		INC	BH
		POP	CX
		LOOP	F39_52
		XOR	SI,SI
		MOV	CX,WORD PTR ES:MenuBords[3]
		MOV	AL,ES:MenuBords[5]
		CALL	ShadLin
		INC	BX
		INC	BX
		PUSH	ES
		CALL	BasAdr
		MOV	CX,DX
		INC	CX
		INC	CX
F39_58:		CALL	Shadow
		LOOP	F39_58
		POP	ES
		PUSH	ES
		POP	DS
		POP	BX
		POP	DX
		POP	CX
		CMP	CL,Opt_M		; Options
		JNE	F39_59
		PUSH	BX
		ADD	BX,0D01h
		MOV	AL,DS:AutoMnu
		CALL	F40_20
		MOV	AL,DS:KeyBar
		CALL	F40_20
		MOV	AL,DS:FulScr
		CALL	F40_20
		MOV	AL,DS:MiniSta
		CALL	F40_20
		MOV	AL,DS:LoadFl
		CALL	F40_20
		CMP	ES:DataLocation,2
		JB	F39_85
		POP	BX
		MOV	AL,17
		CALL	F40_30
		PUSH	BX
		JMP	F39_85
F39_59:		POP	DS			; Files
		PUSH	DS
		PUSH	BX
		MOV	AH,0
		CMP	CL,Fls_M
		JNE	F39_66
		CMP	AH,DS:Visible
		JE	F39_62
		CMP	AH,DS:WinTyp		; p_File
		JE	F39_63
		CMP	DS:WinTyp,p_Info
		JB	F39_62			; Tree panel
		JA	F39_60			; QuickView panel
		CMP	ES:DirInfoSeg,0
		JNE	F39_61
		JMP	F39_62
F39_60:		MOV	SI,ES:ViewSeg
		OR	SI,SI
		JE	F39_62
		PUSH	DS
		MOV	DS,SI
		TEST	DS:ViewAttr,19h
		POP	DS
		JNZ	F39_62
F39_61:		MOV	AL,5
		CALL	F40_30
		INC	AX
		CALL	F40_30
		INC	AX
		CALL	F40_30
		INC	AX
		CALL	F40_30
F39_62:		MOV	AL,12
		CALL	F40_30
		INC	AX
		CALL	F40_30
		INC	AX
		CALL	F40_30
		INC	AX
		CALL	F40_30
		CMP	AH,DS:Visible
		JE	F39_64
		CMP	DS:WinTyp,p_Tree
		JA	F39_64			; Info or QuickView panel
		JB	F39_63			; File panel
		CMP	DS:CrnPath[3],0
		JNE	F39_65
		JMP	F39_64
F39_63:		CMP	DS:ArcPnt,0
		JNE	F39_64
		PUSH	DX
		PUSH	BX
		CALL	TotSel
		OR	BX,DX
		POP	BX
		POP	DX
		JNE	F39_65
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	F39_64
		MOV	AL,DS:[SI].AttrF
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		JE	F39_64
		TEST	AL,10h
		JNZ	F39_65
		TEST	AL,8
		JZ	F39_65
F39_64:		MOV	AL,10
		CALL	F40_30
F39_65:		JMP	F39_85
F39_66:		CMP	CL,Cmd_M		; Commands
		JNE	F39_72
		CMP	DS:Visible,AH
		JNZ	F39_68
		MOV	AL,10
		CALL	F40_30
F39_67:		MOV	AL,4
		CALL	F40_30
		JMP	F39_69
F39_68:		CMP	DS:WinTyp,AH
		JNE	F39_67
		CALL	InvWCB
		CMP	DS:Visible,AH
		JZ	F39_69
		CMP	DS:WinTyp,AH
		JE	F39_70
F39_69:		MOV	AL,12
		CALL	F40_30
F39_70:		CMP	ES:TstEGA,AH
		JNZ	F39_71
		MOV	AL,7
		CALL	F40_30
F39_71:		JMP	F39_85
F39_72:		MOV	AL,ES:ItemOn[0]		; Left / Right
		MOV	DS,ES:WCB1seg
		PUSH	ES
		CMP	CL,Left_M
		JE	F39_73
		MOV	DS,ES:WCB2seg
		PUSH	BX
		ADD	BX,716h
		CALL	BasAdr
		INC	BYTE PTR ES:[DI]
		ADD	BX,0BFFh
		CALL	BasAdr
		INC	BYTE PTR ES:[DI]
		POP	BX
F39_73:		PUSH	BX
		PUSH	AX
		ADD	BX,701h
		CMP	DS:Visible,AH
		JZ	F39_76
		SUB	BH,6
		MOV	AL,DS:WinTyp
		CMP	AL,0			; p_File
		JNE	F39_74
		CMP	DS:BrfFul,AL
		JE	F39_76
		INC	BH
		CMP	DS:BrfFul,1
		JMP	F39_75
F39_74:		ADD	BH,3
		CMP	AL,2
		JE	F39_76
		INC	BH
		CMP	AL,1
F39_75:		JE	F39_76
		INC	BH
F39_76:		CALL	BasAdr
		POP	AX
		POP	BX
		STOSB
		CMP	DS:LongNames,0
		JZ	F39_77
		PUSH	BX
		ADD	BX,801h
		CALL	BasAdr
		POP	BX
		STOSB
F39_77:		CMP	DS:Visible,AH
		JZ	F39_80
		CMP	DS:WinTyp,p_Tree
		JAE	F39_80
		PUSH	BX			; File panel
		PUSH	AX
		MOV	AL,DS:SortTyp
		CMP	AL,4
		JBE	F39_78
		MOV	AL,4
F39_78:		ADD	BH,AL
		ADD	BX,0A01h
		CALL	BasAdr
		POP	AX
		POP	BX
		STOSB
		CMP	DS:SortRev,0
		JZ	F39_79
		PUSH	BX
		ADD	BX,0F01h
		CALL	BasAdr
		POP	BX
		STOSB
F39_79:		POP	ES
		CMP	DS:SortTyp,4
		JAE	F39_82
		JMP	F39_83
F39_80:		POP	ES
		JE	F39_81
		MOV	AL,8
		CALL	F40_30
F39_81:		MOV	AL,18
		CALL	F40_30
F39_82:		MOV	AL,15
		CALL	F40_30
F39_83:		MOV	AH,0
		PUSH	DS
		CALL	InvWCB
		CMP	DS:Visible,AH
		JZ	F39_84
		CMP	DS:WinTyp,p_Tree
		JNE	F39_84
		MOV	AL,5
		CALL	F40_30
F39_84:		POP	DS
F39_85:		PUSH	ES			; Output menu
		POP	DS
		POP	BX
		PUSH	DX
		PUSH	BX
		MOV	BX,100h
		MOV	DX,1650h
		CALL	Window
		POP	BX
		POP	DX
		CMP	CH,0FFh
		JE	F39_10
F39_86:		MOV	AL,CH
		CBW
		MOV	SI,AX
		CMP	AH,DS:MnuList[SI-1]
		JNE	F39_42
		INC	CH
		MOV	AL,DH
		SUB	AL,2
		CMP	CH,AL
		JB	F39_86
		MOV	CH,1
		JMP	F39_86

F39_90:		CALL	ResWin			; Exit
		CALL	Window
		POP	DS
		MOV	AX,CX
		OR	AH,AH
		JE	F39_96
		CMP	AH,0FFh
		JE	F39_96
		MOV	ES:LastMnu,AX
		MOV	DI,ES:WCB1seg
		CMP	AL,Right_M
		JNE	F39_91
		MOV	AL,Left_M
		MOV	DI,ES:WCB2seg
F39_91:		CMP	AL,Left_M
		JNE	F39_92
		PUSH	DS
		MOV	DS,DI
F39_92:		CMP	AL,2
		JNE	F39_95
		CMP	AH,14
		JA	F39_95
		PUSH	AX
		CALL	KeybStat
		MOV	DL,AL
		POP	AX
		TEST	DL,4
		JNZ	F39_94
		TEST	DL,3
		JNZ	F39_93
		TEST	ES:MnuMous,4
		JNZ	F39_94
		TEST	ES:MnuMous,2
		JZ	F39_95
F39_93:		MOV	AL,5
		JMP	F39_95
F39_94:		MOV	AL,6
F39_95:		PUSH	DS
		MOV	DS,CS:SubsSeg
		MOV	BL,AL
		ADD	BL,BL
		MOV	BH,0
		MOV	SI,DS:Menu_T[BX-2]
		MOV	BL,AH
		DEC	BX
		ADD	BL,BL
		MOV	BX,DS:[BX+SI]
		POP	DS
		PUSH	CX
		MOV	DI,OFFSET TmpBuf
		MOV	CX,256
		MOV	AL,0
		REP	STOSB
		POP	CX
	OR	BX,BX
	JE	F39_96
		JMP	BX
F39_96:		JMP	Func04

F40_10		PROC	NEAR
		USES	BX,CX,DX,ES
		CMP	CH,0FFh
		JE	F40_12
		PUSH	AX
		MOV	AL,CH
		CBW
		MOV	SI,AX
		POP	AX
		PUSH	AX
		MOV	BH,CH
		INC	BH
		INC	BL
		MOV	DH,1
		SUB	DL,4
		MOV	CL,DL
		MOV	CH,0
		CALL	Color
		MOV	AL,AH
		CALL	BasAdr
		PUSH	DI
		PUSH	CX
		CALL	MvsColr
		POP	CX
		POP	DI
F40_11:		MOV	AL,ES:[DI]
		INC	DI
		INC	DI
		CALL	UpCase
		CMP	AL,DS:MnuList[SI-1]
		JNE	F40_11
		POP	AX
		MOV	AH,AL
		CALL	Color
		MOV	ES:[DI-1],AH
		CALL	Window
F40_12:		RET
F40_10		ENDP

F40_20		PROC	NEAR
		USES	DI,ES
		CMP	AL,0
		JE	F40_21
		MOV	AL,ES:ItemOn[0]
		CALL	BasAdr
		MOV	ES:[DI],AL
F40_21:		INC	BH
		RET
F40_20		ENDP

F40_30		PROC	NEAR			; Dark line
		USES	AX,BX,CX,DI,ES
		CBW
		MOV	DI,AX
		MOV	ES:MnuList[DI-1],0
		ADD	BH,AL
		INC	BX
		CALL	BasAdr
		MOV	BX,DI
		CMP	CL,2
		JE	F40_31
		CMP	CL,3
		JE	F40_31
		INC	BX
		INC	BX
F40_31:		MOV	AH,MnuDr_C
		CALL	Color
		MOV	AL,AH
		MOV	CL,DL
		SUB	CL,4
		MOV	CH,0
		CALL	MvsColr
		MOV	AH,MnuMi_C
		CALL	Color
		MOV	AL,'-'
		MOV	ES:[BX],AX
		RET
F40_30		ENDP

F40_40		PROC	NEAR			; Get mouse cursor
		USES	CX
		XOR	AX,AX
		CALL	GetMous
		XCHG	AL,DS:MnuMous
		MOV	SI,AX
		MOV	AX,CX
		RET
F40_40		ENDP

F40_50		PROC	NEAR			; Menu position
		USES	BX,SI,DS
		MOV	DS,CS:SubsSeg
		MOV	AH,AL
		MOV	SI,OFFSET PtMnu_S
		MOV	BX,2
F40_51:		CMP	AH,BL
		JB	F40_54
		INC	BH
		CMP	BH,5
		JA	F40_53
F40_52:		LODSB
		INC	BX
		OR	AL,AL
		JNZ	F40_52
		DEC	BX
		JMP	F40_51
F40_53:		MOV	BH,0
F40_54:		MOV	AL,BH
		RET
F40_50		ENDP

M01_00:		DEC	CH			; Brief / Full
		MOV	CL,DS:SortTyp
M01_01:		CALL	HidCur
		CMP	DS:WinTyp,p_File
		JE	M01_03
		MOV	BP,DS
		CMP	BP,ES:WCBcrn
		JNE	M01_02
		CALL	M02_10
		JNC	M01_02
		POP	DS
		JMP	Panels00
M01_02:		MOV	DS:WinTyp,p_File
		MOV	DS:ChgFlg,1
M01_03:		MOV	AL,CL
		XCHG	DS:SortTyp,AL
		MOV	DS:BrfFul,CH
		CMP	AL,CL
		JE	M01_04
		CMP	DS:ChgFlg,0
		JNZ	M01_04
		JMP	M18_20
M01_04:		JMP	M04_05

M02_10		PROC	NEAR
		USES	AX
		MOV	AL,DS:WinShortPath[0]
		CALL	Phantom
		JC	M02_12
		MOV	DX,OFFSET WinShortPath
		MOV	AH,3Bh
		CALL	Intr21
		JNC	M02_11
		CMP	AX,3
		STC
		JNE	M02_12
M02_11:		MOV	DL,DS:WinShortPath
		SUB	DL,'A'
		MOV	AH,0Eh
		INT	21h
		CALL	GetPath
		PUSHF
		CALL	PutPrompt
		POPF
M02_12:		RET
M02_10		ENDP

M04_00:		CALL	HidCur			; Info / Tree
		MOV	CL,6
		SUB	CL,CH
		JNE	M04_01
		MOV	CL,3
M04_01:		CMP	DS:Visible,0
		JZ	M04_02
		CMP	CL,DS:WinTyp		; p_QView
		JE	M04_04
M04_02:		MOV	DI,DS
		CMP	DI,ES:WCBcrn
		JNE	M04_03
		CALL	InvWCB
		CMP	DS:Visible,0
		JZ	M04_03
		CMP	DS:WinTyp,p_File
		JNE	M04_03
		CALL	M02_10
		JNC	M04_03
		POP	DS
		JMP	Func04
M04_03:		MOV	DS,DI
		MOV	AL,DS:WinTyp
		CMP	AL,CL
		JE	M04_04
		MOV	DS:Ctrl_L,AL
M04_04:		MOV	DS:WinTyp,CL
		MOV	DS:ChgFlg,1
		CMP	CL,1
		JNE	M04_05
		MOV	DS:CrnPath,0
M04_05:		MOV	AX,DS
		MOV	CL,Left_M
		CMP	AX,ES:WCB1seg
		JE	M04_06
		MOV	CL,Right_M
M04_06:		JMP	M07_10

M07_00:		MOV	AL,0			; Panel on/off
		CMP	AL,DS:Visible
		JZ	M07_10
		MOV	DS:Visible,AL		; Panel off
		CALL	BegBX
		CALL	SizDX
		CALL	ResUser
		MOV	ES:Ctrl_O,AL
		MOV	DI,DS
		MOV	SI,ES:WCBcrn
		CMP	SI,DI
		JNE	M07_01
		MOV	DS,ES:WCBsec
		CMP	AL,DS:Visible
		JZ	M07_01
		MOV	DS,SI
		MOV	AL,CL
		CALL	PutMnu
		CALL	F20_10
		CALL	PutPath
		CALL	PutFls
M07_01:		POP	DS
		JMP	Func02
M07_10:		CALL	HidCur			; Panels On
		MOV	DS:Visible,1
		MOV	DI,DS
		MOV	AX,ES:WCBcrn
		CMP	AX,DI
		JE	M07_11
		MOV	DS,AX
		CMP	DS:Visible,0
		MOV	DS,DI
		JNZ	M07_11
		POP	DS
		MOV	AL,CL
		CALL	PutMnu
		PUSH	CX
		CALL	F20_10
		POP	CX
		PUSH	DS
M07_11:		CMP	DS:ChgFlg,0
		JZ	M07_12
		MOV	AL,CL
		CALL	PutMnu
		CALL	TstInfo
M07_12:		POP	DS
		JMP	Panels00

M08_00:		CMP	DS:Visible,0		; Long names
		JZ	Func04
		CMP	DS:WinTyp,p_Tree
		JA	Func04			; Info or QuickView panel
		PUSH	DS
M08_01:		MOV	BX,OFFSET LongNames
		CALL	InvPanelVar
		CMP	DS:WinTyp,p_File
		JNE	M04_05
		CMP	ES:OS2box,0
		JE	M18_20
		CMP	DS:LongNames,0
		JE	M18_20
		CMP	DS:LongNamFlag,0
		JNE	M18_20
		MOV	DS:ChgFlg,1
		JMP	M18_20

M10_00:		MOV	CL,CH			; Sort
		SUB	CL,10
		MOV	DS:SortRev,0
		MOV	DS:SortTyp,0FFh
		MOV	CH,DS:BrfFul
		JMP	M01_01

M15_00:		MOV	BX,OFFSET SortRev	; Reverced sort
		CALL	InvPanelVar
		CALL	HidCur
		JMP	M18_20

		.CONST
Filter_S	DB	' Filter ',0
Filter0_S	DB	' ',0DAh,' ^bFilter^b ',0FFh,80h+20,0C4h,0BFh,'  ',0DAh,' ^bAttributes^b ',0FFh,80h+6,0C4h,0BFh,' ',0
Filter1_S	DB	0FFh,32,0B3h,'  [ ] Archive'  ,0FFh,5,0B3h,0
Filter2_S	DB	0FFh,32,0B3h,'  [ ] System'   ,0FFh,6,0B3h,0
Filter3_S	DB	0FFh,32,0B3h,'  [ ] Hidden'   ,0FFh,6,0B3h,0
Filter4_S	DB	0FFh,32,0B3h,'  [ ] Directory',0FFh,3,0B3h,0
Filter5_S	DB	0FFh,32,0B3h,'  [ ] Absent'   ,0FFh,6,0B3h,0
Filter6_S	DB	0FFh,32,0C0h,0FFh,80h+18,         0C4h,0D9h,0
Filter7_S	DB	0B3h,'  ( ) ^0E21< ',0B3h,0
Filter8_S	DB	0B3h,0FFh,    28, 0B3h,0
Filter9_S	DB	0C0h,0FFh,80h+28,0C4h,0D9h,0
Custom_S	DB	'Custom:',0
OkCn_S		DB	'[ Ok ]',0FFh,3,'[ Cancel ]',0

		.DATA
Filter_P	DB	Box_C, BoxCu_C, BoxBr_C, 0
		DW	Filter_S
		DW	Filter0_S
		DW	Filter1_S
		DW	Filter2_S
		DW	Filter3_S
		DW	Filter4_S
		DW	Filter5_S
		DW	Filter6_S
		DW	MaxFilts+1 DUP(0)
Filter_B	BOXHD	< Filter_P, 8500h, FilterSub, Dialog_K, 0, Filter_H, 0 >
		BOXMN	< 2,   7,1,0,2,  30Ah, TmpBuf+0, 0 >	  ; Filter
		BOXMN	< 1,0C0h,7,1,2,  409h, TmpBuf+1,80h+LenFilt>; Custom
		BOXMN	< 3, 1,3,80h,2,  32Ah, TmpBuf+35, 0 >	  ; Archive
		BOXMN	< 3, 2,4,80h,3,  42Ah, TmpBuf+32, 0 >	  ; System
		BOXMN	< 3, 3,5,80h,4,  52Ah, TmpBuf+31, 0 >	  ; Hidden
		BOXMN	< 3, 4,6,80h,5,  62Ah, TmpBuf+34, 0 >	  ; Directories
		BOXMN	< 3, 5,7,80h,6,  72Ah, TmpBuf+33, 0 >	  ; Absent
		BOXMN	< 4, 6,80h,7,7,0FCF6h, OkCn_D   , 0 >	  ; Ok / Cancel
		DB	0
Filter_T	DW	TmpErr
OkCn_D		DB	 6, 0
		DB	10, 9
		DB	 0

		.CODE	Subs
FilterSub	PROC	NEAR
		CMP	AX,0FFFEh
		JNE	@@Exit
		ADD	BX,306h
		MOV	SI,DS:FiltsPoint
		CALL	FndItem
		MOV	DX,OFFSET Filter_T
		MOV	CL,DS:TmpBuf[50]
		MOV	CH,0
  @@Filters:	PUSH	DS
		PUSH	SI
		PUSH	CX
		MOV	DI,SI
		CMP	CL,1
		JNE	@@Colon
		PUSH	CS
		POP	DS
		MOV	DI,OFFSET Custom_S
		JMP	@@MoveName
  @@Colon:	LODSB
		CMP	AL,13
		JE	@@MoveName
		CMP	AL,10
		JE	@@MoveName
		CMP	AL,1Ah
		JE	@@MoveName
		CMP	AL,':'
		JNE	@@Colon
		CALL	FndLet
		MOV	DI,SI
  @@MoveName:	MOV	SI,DI
		MOV	DI,OFFSET TmpErr
		MOV	CL,21
  @@GetChar:	LODSB
		CMP	AL,13
		JE	@@EndName
		CMP	AL,10
		JE	@@EndName
		CMP	AL,1Ah
		JE	@@EndName
		STOSB
		LOOP	@@GetChar
  @@EndName:	MOV	AL,0
		STOSB
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET Filter7_S
		MOV	AL,50
		CALL	NgLine
		POP	CX
		POP	SI
		POP	DS
		CALL	FindNextItem
		INC	BH
		LOOP	@@Filters
		PUSH	DS
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET Filter8_S
		MOV	AL,50
		CALL	NgLine
		INC	BH
		MOV	SI,OFFSET Filter9_S
		MOV	AL,50
		CALL	NgLine
		POP	DS
  @@Exit:	RET
FilterSub	ENDP

		.CODE
M18_00:		CMP	DS:Visible,0		; Filter...
		JZ	Func04
		CMP	DS:WinTyp,p_File
		JNE	Func04
		PUSH	DS
M18_01:		MOV	DI,OFFSET TmpBuf
		MOV	AL,DS:FiltTyp
		STOSB
		MOV	SI,OFFSET DskMas
		MOV	CX,29
		REP	MOVSB
		MOV	AH,DS:FiltAttr
		MOV	CL,6
M18_02:		MOV	AL,AH
		AND	AL,1
		STOSB
		SHR	AH,1
		LOOP	M18_02
		MOV	DX,DS
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,DS:FiltsPoint
		CALL	FndItem
		JC	M18_04
M18_03:		INC	CX
		CALL	FindNextItem
		JNC	M18_03
M18_04:		CMP	CX,MaxFilts
		JBE	M18_05
		MOV	CL,MaxFilts
M18_05:		CMP	DS:TmpBuf[0],CL
		JBE	M18_06
		MOV	DS:TmpBuf[0],CL
M18_06:		INC	CX
		MOV	DS:TmpBuf[50],CL
		MOV	CH,CL
		CMP	CH,4
		JAE	M18_07
		MOV	CH,4
M18_07:		MOV	BP,OFFSET Filter_B
		MOV	ES,CS:SubsSeg
		MOV	DI,OFFSET Filter_P+4+2*8
		PUSH	CX
		MOV	CL,CH
		SUB	CL,4
		JBE	M18_08
		MOV	CH,0
		MOV	AX,OFFSET Empty_S
		REP	STOSW
M18_08:		POP	CX
		MOV	AX,0FFFFh
		STOSW
		MOV	AX,OFFSET OkCn_S
		STOSW
		XOR	AX,AX
		STOSW
		MOV	AL,CL
		MOV	ES:[BP+SIZE BoxHd+0*SIZE BoxMn].mSelTot,AL
		ADD	AL,3
		MOV	ES:[BP+SIZE BoxHd+1*SIZE BoxMn].mBase.Y,AL
		MOV	AL,80h+Left_M
		CMP	DX,DS:WCB1seg
		JE	M18_09
		MOV	AL,80h+Right_M
M18_09:		MOV	ES:[BP].bMenu,AL
		PUSH	DS
		POP	ES
		CALL	Dialog
		JE	M18_10
		POP	DS
		POP	DS
		JMP	Func04
M18_10:		POP	ES
		MOV	SI,OFFSET TmpBuf
		LODSB
		MOV	CL,DS:TmpBuf[50]
		DEC	CX
		CMP	AL,CL
		JB	M18_11
		MOV	AL,0FFh
M18_11:		MOV	ES:FiltTyp,AL
		MOV	DI,OFFSET DskMas
		MOV	CX,LenFilt+2
		REP	MOVSB
		MOV	SI,OFFSET TmpBuf[30]
		MOV	CL,6
		MOV	AH,0
M18_12:		LODSB
		CMP	AL,1
		CMC
		RCR	AH,1
		LOOP	M18_12
		SHR	AH,1
		SHR	AH,1
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	DS:FiltAttr,AH
M18_20:		CALL	M18_30
		JMP	M04_05

M18_30		PROC	NEAR
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CALL	FileNumb
		ADD	SI,OFFSET NameF
		MOV	DI,OFFSET LineBuf1
		STOSW
		CALL	strcpy
		MOV	SI,DS:FileFirst
		CALL	Numb2Addr
		CALL	FileNumb
		ADD	SI,OFFSET NameF
		MOV	DI,OFFSET LineBuf2
		STOSW
		CALL	strcpy
		MOV	BX,DS:EndBuf
		CALL	FileSort
		MOV	CX,DS:DiskBuf
		MOV	DX,CX
		CMP	BX,CX
		JBE	M18_32
		MOV	AX,WORD PTR ES:LineBuf1
		MOV	DI,OFFSET LineBuf1+2
		CALL	FindFile
		JC	M18_31
		MOV	CX,SI
M18_31:		MOV	AX,WORD PTR ES:LineBuf2
		MOV	DI,OFFSET LineBuf2+2
		CALL	FindFile
		JNC	M18_32
		MOV	SI,DX
M18_32:		CALL	Addr2Numb
		MOV	DS:FileFirst,SI
		MOV	SI,CX
		CALL	Addr2Numb
		MOV	DS:FileCur,SI
		RET
M18_30		ENDP

COMMENT	|
M21_00:	CALL	Help			; Help
	JMP	Func04
|

F60_00:		PUSH	DS			; Alt F10 - Tree
		PUSH	ES
		POP	DS
		MOV	AL,Cmd_M
		CALL	PutMnu
		MOV	DS:HlpPage,ChgTree_H
		MOV	DL,DS:Columns
		MOV	DH,DS:Lines
		SUB	DX,304h
		MOV	BX,203h
		MOV	SI,OFFSET DosShortPath
		CALL	ChooseDir
		POP	DS
		JC	Panels00
		PUSH	DS
		PUSH	ES
		MOV	DX,OFFSET TmpBuf
		XOR	BP,BP
		JMP	F24_01

		.CONST
DirSize1_S	DB	'Scanning sizes of subdirectories',0
DirSize2_S	DB	'on drive ^0E:',0

		.DATA
DirSize_P	DB	Box_C, 0, 0, 0
		DW	Tree_S
		DW	DirSize1_S
		DW	DirSize2_S
		DW	0
DirSize_T	DW	TmpBuf

		.CODE
F66_00:		CMP	DS:Visible,0		; Alt F6 - Directory sizes
		JZ	F66_03
		CMP	DS:WinTyp,p_File
		JNE	F66_03
		CMP	DS:ArcPnt,0
		JNE	F66_03
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	F66_03
		CALL	TotSel
		OR	BX,BX
		JNE	F66_01
		TEST	DS:[SI].AttrF,10h
		JZ	F66_03
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		JE	F66_03
F66_01:		MOV	AL,Cmd_M
		CALL	PutMnu
		MOV	AL,DS:WinShortPath[0]
		MOV	AH,0
		MOV	WORD PTR ES:TmpBuf[0],AX
		MOV	BX,500h
		MOV	DX,OFFSET DirSize_T
		MOV	BP,OFFSET DirSize_P
		CALL	DialBox
		CALL	Window
		MOV	AL,1
		CALL	DirSize
		CMP	DS:SortTyp,3
		JNE	F66_02
		CALL	M18_30
F66_02:		CALL	ResWin
		CALL	Window
F66_03:		JMP	Panels00

		.CONST
Mode_S		DB	' Video Mode ',0
ModeLine_S	DB	' Mode ^0E4< ^1I3>x^2I2> ^3Pcolor/mono / ',0
ModeError1_S	DB	'There is not enough memory',0
ModeError2_S	DB	'to initialize video mode.',0
ModeNoVESA1_S	DB	'There are no text modes supported',0
ModeNoVESA2_S	DB	'by VESA SuperVGA BIOS.',0

		.DATA
Mode_P		DB	Hist_C, HstCu_C, 0, 0
		DW	Mode_S
		DW	0FFFEh
		DW	0
Mode_B		BOXHD	< Mode_P, 8500h, DialogList, Dialog_K, Cmd_M, Default_H, 0 >
		DB	0
ModeLine_T	DW	TmpErr, TmpErr+5, TmpErr+7, TmpErr+9
ModeError_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Mode_S
		DW	ModeError1_S
		DW	ModeError2_S
		DW	Ok_S
		DW	0
ModeError_B	BOXHD	< ModeError_P, 900h, 0, Dialog_K, Cmd_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
ModeNoVESA_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Mode_S
		DW	ModeNoVESA1_S
		DW	ModeNoVESA2_S
		DW	Ok_S
		DW	0
ModeNoVESA_B	BOXHD	< ModeNoVESA_P, 900h, 0, Dialog_K, Cmd_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0

		.CODE	Subs
ModeLine	PROC	NEAR
		USES	AX,CX,SI,DI
		MOV	AX,SI
		SUB	AX,DI
		ADD	BX,DS:ListBase
		ADD	BH,AL
		MOV	AX,SI
		ADD	AX,'1'
		CMP	AX,'9'
		JBE	@@HotKey
		ADD	AX,'A'-'9'-1
		CMP	AX,'Z'
		JBE	@@HotKey
		MOV	AL,' '
  @@HotKey:	PUSH	ES
		PUSH	DI
		CALL	BasAdr
		MOV	ES:[DI],AL
		POP	DI
		POP	ES
		PUSH	BX
		ADD	SI,SI
		MOV	CX,WORD PTR DS:TmpBuf[SI]
		MOV	DI,OFFSET TmpErr
		CMP	CX,7
		JA	@@VesaInfo
		MOV	AX,80
		MOV	DX,25
		MOV	BL,0
		JE	@@ConvMode
		MOV	BL,1
		JMP	@@ConvMode
  @@VesaInfo:	MOV	AX,4F01h
		INT	10h
		MOV	AX,DS:[DI+12h]
		MOV	DX,DS:[DI+14h]
		OR	AH,AH
		JE	@@GetAttr
		XOR	DX,DX
		MOV	BL,DS:[DI+16h]
		MOV	BH,0
		DIV	BX
		PUSH	AX
		MOV	AX,DS:[DI+14h]
		MOV	BL,DS:[DI+17h]
		DIV	BX
		MOV	DX,AX
		POP	AX
  @@GetAttr:	MOV	BL,DS:[DI]
		SHR	BL,1
		SHR	BL,1
		SHR	BL,1
		AND	BL,1
  @@ConvMode:	MOV	WORD PTR DS:TmpErr[5],AX
		MOV	WORD PTR DS:TmpErr[7],DX
		MOV	BH,0
		MOV	WORD PTR DS:TmpErr[9],BX
		MOV	SI,DI
		MOV	AL,CH
		CALL	HexByt
		STOSW
		MOV	AL,CL
		CALL	HexByt
		STOSW
		MOV	AL,0
		STOSB
		POP	BX
		MOV	DI,SI
		MOV	CH,1
		MOV	AL,'0'
		REPE	SCASB
		DEC	DI
		XCHG	SI,DI
		MOV	CX,5
		REP	MOVSB
		MOV	DX,OFFSET ModeLine_T
		MOV	SI,OFFSET ModeLine_S
		MOV	AL,DS:ListWindow.X
		PUSH	DS
		PUSH	BX
		PUSH	CS
		POP	DS
		INC	BX
		INC	BX
		CALL	NgLine
		POP	BX
		POP	DS
		MOV	DL,DS:ListWindow.X
		MOV	DH,1
		CLC
		RET
ModeLine	ENDP

ModeHotKey	PROC	NEAR
		MOV	CX,AX
		CALL	UpCase
		CMP	AL,'1'
		JB	@@Exit
		CMP	AL,'9'
		JBE	@@Enter
		CMP	AL,'A'
		JB	@@Exit
		CMP	AL,'Z'
		JA	@@Exit
		SUB	AL,'A'-'1'-9
  @@Enter:	SUB	AL,'1'
		MOV	AH,0
		CMP	AX,DS:ListTotal
		JAE	@@Exit
		MOV	DS:ListCursor,AX
		MOV	CX,1C0Dh
  @@Exit:	MOV	AX,CX
		STC
		RET
ModeHotKey	ENDP

		.CODE
F59_00:		PUSH	ES			; ^F9 - video mode
		POP	DS
		MOV	AX,1A00h
		INT	10h
		MOV	CX,101h
		CMP	AL,1Ah
		JE	F59_01
		MOV	CL,0
		MOV	AH,0Fh
		INT	10h
		CMP	AL,7
		JE	F59_01
		XCHG	CL,CH
F59_01:		MOV	DI,OFFSET TmpBuf
		OR	CL,CL
		JZ	F59_02
		MOV	AX,3
		STOSW
F59_02:		OR	CH,CH
		JZ	F59_03
		MOV	AX,7
		STOSW
F59_03:		PUSH	DI
		MOV	DI,OFFSET TmpErr
		MOV	AX,4F00h
		INT	10h
		LDS	SI,ES:[DI+0Eh]
		POP	DI
		CMP	AX,4Fh
		JNE	F59_10
		PUSH	DI
F59_04:		LODSW
		CMP	AX,0FFFFh
		JE	F59_05
		CMP	AX,7
		JBE	F59_04
		PUSH	DI
		MOV	DI,OFFSET TmpBuf+256
		MOV	CX,AX
		MOV	AX,4F01h
		INT	10h
		CMP	AX,4Fh
		MOV	AX,ES:[DI]
		POP	DI
		JNE	F59_04
		AND	AL,13h
		CMP	AL,3
		JNE	F59_04
		MOV	AX,CX
		STOSW
		CMP	DI,OFFSET TmpBuf+256
		JB	F59_04
F59_05:		POP	AX
		CMP	AX,DI
		JE	F59_10
		PUSH	ES
		POP	DS
		SUB	DI,OFFSET TmpBuf
		SHR	DI,1
		MOV	AL,DS:Lines
		SUB	AL,5+4
		MOV	AH,0
		CMP	AX,DI
		JBE	F59_06
		MOV	AX,DI
F59_06:		MOV	AH,AL
		MOV	AL,26
		MOV	DS:ListWindow,AX
		MOV	DS:ListBase,205h
		MOV	DS:ListTotal,DI
		MOV	DS:ListCursor,0
		MOV	DS:ListFirst,0
		MOV	DS:ListLine,OFFSET ModeLine
		MOV	DS:ListHotKey,OFFSET ModeHotKey
		MOV	BP,OFFSET Mode_B
		CALL	Dialog
		JNZ	Func04
		CALL	SavMous
		MOV	AL,0
		CALL	CurMous
		MOV	SI,DS:ListCursor
		ADD	SI,SI
		MOV	AX,WORD PTR TmpBuf[SI]
		OR	AH,AH
		JZ	F59_07
		MOV	BX,AX
		MOV	AX,4F02h
F59_07:		INT	10h
		JMP	F69_01
F59_10:		MOV	BP,OFFSET ModeNoVESA_B
		CALL	Dialog
		JMP	Func04

F69_00:		PUSH	ES			; Alt F9 - EGA lines
		POP	DS
		CMP	DS:TstEGA,0
		JZ	Func04
		CALL	SavMous
		MOV	AL,0
		CALL	CurMous
		MOV	AL,DS:ScrMod
		MOV	AH,0
		INT	10h
		CMP	DS:CharSize,14
		JB	F69_01
		MOV	BL,0
		MOV	AX,1112h
		INT	10h
		MOV	AH,12h
		MOV	BL,20h
		INT	10h
F69_01:		XOR	SI,SI
F69_10:		MOV	DS:ModeFlag,0
		MOV	DS:BarPrev,0FFh
		XOR	BX,BX
		MOV	DL,DS:Columns
		MOV	DH,DS:Lines
		CALL	ResUser
		OR	SI,SI
		JNE	F69_12
F69_11:		PUSH	ES
		MOV	BH,0
		MOV	AX,1130h
		INT	10h
		POP	ES
		INC	DX
		MOV	AH,0Fh
		INT	10h
		OR	SI,SI
		JNE	F69_13
		CMP	DS:ModeFlag,0
		JNZ	F69_13
		PUSH	AX
		MOV	AL,DL
		CALL	F69_30
		POP	AX
		JNC	F69_13
		MOV	DS:ModeFlag,1
F69_12:		CALL	F69_40
		JMP	F69_11
F69_13:		AND	AL,7Fh
		MOV	DS:ScrMod,AL
		MOV	DS:ScrPage,BH
		MOV	DS:CharSize,CL
		MOV	AL,DL
		PUSH	AX
		CALL	ScreenParms
		MOV	BX,DS:UserScreen	; New buffers
		MOV	SI,OFFSET GlobalSegs_L
		CALL	CompressBlock
		MOV	BX,ES
		ADD	BX,DS:SizeDataSeg
		SUB	BX,DS:WorkBuffer
		SUB	BX,MinView/16
		MOV	AL,DS:Lines
		MUL	DS:Columns
		ADD	AX,AX
		DEC	AX
		MOV	CL,4
		SHR	AX,CL
		ADD	BX,AX
		SHR	BX,1
		SHR	BX,1
		POP	AX
		MOV	DX,AX
		MUL	AH
		ADD	AX,AX
		DEC	AX
		SHR	AX,CL
		INC	AX
		CMP	AX,BX
		JBE	F69_14
		MOV	AX,BX
		SHL	AX,CL
		SHR	AX,1
		DIV	DH
		MOV	DL,AL
F69_14:		MOV	AL,DS:Columns
		MOV	DS:Columns,DH
		MOV	AH,0
		MOV	BL,DH
		MOV	BH,0
		MOV	BP,AX
		CMP	BP,BX
		JBE	F69_15
		MOV	BP,BX
F69_15:		SUB	AX,BP
		SUB	BX,BP
		MOV	CH,0
		MOV	CL,DS:Lines
		MOV	DS:Lines,DL
		XOR	SI,SI
		XOR	DI,DI
		MOV	DH,DL
		SUB	DL,CL
		JAE	F69_17
		PUSH	AX
		ADD	AX,BP
		NEG	DL
		MUL	DL
		ADD	AX,AX
		MOV	SI,AX
		MOV	CL,DH
		SUB	DS:DosLin,DL
		JA	F69_16
		MOV	DS:DosLin,1
F69_16:		MOV	DL,0
		POP	AX
F69_17:		PUSH	ES
		PUSH	DS
		MOV	DH,DS:CrntCol
		MOV	ES,DS:WorkBuffer
		MOV	DS,DS:ScreenBuf
F69_18:		PUSH	CX
		MOV	CX,BP
		REP	MOVSW
		ADD	SI,AX
		ADD	SI,AX
		MOV	CX,BX
		JCXZ	F69_19
		PUSH	AX
		MOV	AH,DH
		MOV	AL,' '
		REP	STOSW
		POP	AX
F69_19:		POP	CX
		LOOP	F69_18
		MOV	AX,BX
		ADD	AX,BP
		MUL	DL
		MOV	CX,AX
		JCXZ	F69_20
		MOV	AH,DH
		MOV	AL,' '
		REP	STOSW
F69_20:		MOV	BX,DS
		MOV	DX,ES
		POP	DS
		POP	ES
		MOV	AL,DS:Lines
		MUL	DS:Columns
		PUSH	AX
		ADD	AX,AX
		DEC	AX
		MOV	CL,4
		SHR	AX,CL
		INC	AX
		POP	CX
		MOV	DS:ScreenBuf,DX
		PUSH	ES
		PUSH	DS
		MOV	DS,DX
		ADD	DX,AX
		MOV	ES,DX
		XOR	SI,SI
		XOR	DI,DI
		REP	MOVSW
		POP	DS
		POP	ES
		MOV	DS:UserScreen,DX
		ADD	DX,AX
		ADD	DX,AX
		ADD	DX,AX
		MOV	DS:WindowsPoint,DX
		MOV	DS:WorkBuffer,DX
		MOV	SI,OFFSET GlobalSegs_L
		CALL	CompressBlock
		CMP	DS:Mouse,0
		JE	F69_22
		MOV	AL,DS:Lines
		CBW
		MOV	CL,3
		SHL	AX,CL
		DEC	AX
		CMP	DS:MouseY,AX
		JBE	F69_21
		MOV	DS:MouseY,AX
F69_21:		CALL	IniMous
F69_22:		CALL	PutPrompt
		JMP	Panels00

F69_30		PROC	NEAR			; Compare size of screen buffer
		USES	AX,BX,CX,DX
		CALL	F69_31
		MOV	DX,AX
		MOV	AL,DS:Lines
		MOV	AH,DS:Columns
		CALL	F69_31
		MOV	BX,DS
		ADD	BX,DS:SizeDataSeg
		SUB	BX,DS:WorkBuffer
		ADD	AX,BX
		SUB	AX,DX
		JC	@@Exit
		CMP	AX,MinView/16
  @@Exit:	RET
F69_30		ENDP

F69_31		PROC	NEAR			; Size of screen buffer
		MUL	AH
		ADD	AX,AX
		DEC	AX
		MOV	CL,4
		SHR	AX,CL
		INC	AX
		SHL	AX,1
		SHL	AX,1
		RET
F69_31		ENDP

F69_40		PROC	NEAR			; Restore video mode
		MOV	AL,DS:ScrMod
		MOV	AH,0
		INT	10h
		PUSH	ES
		MOV	BH,0
		MOV	AX,1130h
		INT	10h
		POP	ES
		CMP	CL,DS:CharSize
		JBE	@@Exit
		MOV	BL,0
		MOV	AX,1112h
		INT	10h
		MOV	AH,12h
		MOV	BL,20h
		INT	10h
  @@Exit:	RET
F69_40		ENDP

		.CONST
Cmp0_S		DB	' Compare ',0
Cmp1_S		DB	'The two directories appear',0
Cmp2_S		DB	'to be identical.',0

		.DATA
CmpDi_P		DB	Box_C, BoxCu_C, BoxCu_C, 0
		DW	Cmp0_S
		DW	Cmp1_S
		DW	Cmp2_S
		DW	Ok_S
		DW	0
CmpDi_B		BOXHD	< CmpDi_P, 700h, 0, Dialog_K, Cmd_M, CompDirs_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0

		.CODE
M52_00:		CMP	DS:WinTyp,p_File	; Compare directories
		JNE	Func04
		CALL	InvWCB
		CMP	DS:WinTyp,p_File
		JNE	Func04
		CMP	DS:Visible,0
		JZ	Func04
M52_01:		MOV	CL,0
		PUSH	ES
		PUSH	DS
		CALL	InvWCB
		POP	ES
		CALL	M52_10
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		CALL	M52_10
		POP	ES
		OR	CL,CL
		JNE	M52_02
		CALL	PutFls
		CALL	InvWCB
		CALL	PutFls
		CALL	BegBX
		CALL	SizDX
		MOV	BL,0
		MOV	DL,ES:Columns
		CALL	Window
		MOV	BP,OFFSET CmpDi_B
		CALL	Dialog
M52_02:		JMP	F69_22

M52_10		PROC	NEAR
		MOV	DI,ES:DiskBuf
M52_11:		CMP	DI,ES:EndBuf
		JAE	M52_14
		AND	ES:[DI].AttrF,3Fh
		TEST	ES:[DI].AttrF,10h
		JNZ	M52_13
		CALL	FindFileEntry
		JC	M52_12
		TEST	DS:[SI].AttrF,10h
		JNZ	M52_12
		MOV	AX,DS:[SI].DateF
		CMP	AX,ES:[DI].DateF
		JB	M52_12
		JA	M52_13
		MOV	AX,DS:[SI].TimeF
		CMP	AX,ES:[DI].TimeF
		JAE	M52_13
M52_12:		OR	ES:[DI].AttrF,40h
		MOV	CL,1
M52_13:		MOV	AX,ES:[DI].LenShortNamF
		ADD	DI,ES:[DI].LenLongNamF
		ADD	DI,OFFSET NameF
		ADD	DI,AX
		JMP	M52_11
M52_14:		RET
M52_10		ENDP

		.CONST
General00_S	DB			' General options ',0
General01_S	DB	0DAh,' ^bExecution options^b '	   ,0FFh,80h+23,0C4h,0BFh,0
General02_S	DB	0B3h,'  [ ] Quick execute commands'		,0FFh,14,0B3h,0
General03_S	DB	0B3h,'  Command shell options:'		,0FFh,18,0B3h,0
General04_S	DB	0B3h,     0FFh,42,0B3h,0
General05_S	DB	0DAh,' ^bView and Edit^b '		   ,0FFh,80h+27,0C4h,0BFh,0
General06_S	DB	0B3h,'  [ ] Alternative viewer'		,0FFh,18,0B3h,0
General07_S	DB	0B3h,'  [ ] Alternative editor'		,0FFh,18,0B3h,0
General08_S	DB	0DAh,' ^bOther options^b '		   ,0FFh,80h+27,0C4h,0BFh,0
General09_S	DB	0B3h,'  [ ] Auto save setup'		,0FFh,21,0B3h,0
General10_S	DB	0B3h,'  [ ] Show logical drive B:'		,0FFh,15,0B3h,0
General11_S	DB	0B3h,'  [ ] Show short names in lower case'	,0FFh, 6,0B3h,0
General12_S	DB	' ',0C0h				   ,0FFh,80h+42,0C4h,0D9h,0
Prompt1_S	DB	'Press ^r Space ^r to change an option, ^r ', 18h, ' ^r and ^r ', 19h, ' ^r',0
Prompt2_S	DB	'to move between options',0

		.DATA
General_P	DB	Conf_C, CnfRe_C, CnfBo_C, 0
		DW	General00_S
		DW	General01_S
		DW	General02_S
		DW	General03_S
		DW	General04_S
		DW	General12_S
		DW	General05_S
		DW	General06_S
		DW	General07_S
		DW	General12_S
		DW	General08_S
		DW	General09_S
		DW	General10_S
		DW	General11_S
		DW	General12_S
		DW	Prompt1_S
		DW	Prompt2_S
		DW	0FFFFh
		DW	OkCn_S
		DW	0
General_B	BOXHD	< General_P, 100h, 0, Dialog_K, Opt_M, General_H, 0 >
		BOXMN	<3, 7,1,0,0, 30Ah, ExecTyp , 0>	; Quick execute commands
		BOXMN	<1, 0,2,1,1, 509h,Switches,80h+LenSwtch>; Command shell switches
		BOXMN	<3, 1,3,2,2, 80Ah, AltView , 0>	; Alternative viewer
		BOXMN	<3, 2,4,3,3, 90Ah, AltEdit , 0>	; Alternative editor
		BOXMN	<3, 3,5,4,4,0C0Ah, AutoSav , 0>	; Auto save setup
		BOXMN	<3, 4,6,5,5,0D0Ah, DriveB  , 0>	; Show logical drive B:
		BOXMN	<3, 5,7,6,6,0E0Ah, ConvCase, 0>	; Lowercase short names
		BOXMN	<4, 6,0,7,7,13F6h, OkCn_D  , 0>	; Ok / Cancel
		DB	0

		.CODE
M61_00:		PUSH	ES			; General options
		POP	DS
		MOV	BP,OFFSET General_B
M61_01:		CALL	Dialog
		JNE	Func04
M61_02:		MOV	DS,DS:WCBcrn
		MOV	DS:ChgFlg,1
		CALL	InvWCB
		MOV	DS:ChgFlg,1
		JMP	Panels00

		.CONST
Interface00_S	DB			' Interface options ',0
Interface01_S	DB	0DAh,' ^bGeneral options^b '   ,0FFh,80h+15,0C4h,0BFh,'  ',0DAh,' ^bScreen colors^b ',0FFh,80h+12,0C4h,0BFh,0
Interface02_S	DB	0B3h,'  [ ] Clock'			,0FFh,21,0B3h,'  ',0B3h,'  ( ) Black & White'     ,0FFh, 8,0B3h,0
Interface03_S	DB	0B3h,'  [ ] Path prompt'		,0FFh,15,0B3h,'  ',0B3h,'  ( ) Color'	       ,0FFh,16,0B3h,0
Interface04_S	DB	0B3h,'  [ ] Menu bar always visible',0FFh, 3,0B3h,'  ',0B3h,'  ( ) Laptop'	       ,0FFh,15,0B3h,0
Interface05_S	DB	0B3h,'  [ ] Alt alone selects menu'	,0FFh, 4,0B3h,'  ',0C0h			  ,0FFh,80h+27,0C4h,0D9h,0
Interface06_S	DB	0B3h,'  [ ] Esc toggles screen on/off ',0B3h,'  ',0DAh,' ^bScreen output^b '	  ,0FFh,80h+12,0C4h,0BFh,0
Interface07_S	DB	0B3h,'  [ ] Clear keyboard buffer'	,0FFh, 5,0B3h,'  ',0B3h,'  ( ) Synchronized access  ',0B3h,0
Interface08_S	DB	0B3h,'  [ ] Confirmation on quit'	,0FFh, 6,0B3h,'  ',0B3h,'  ( ) Asynchronized access ',0B3h,0
Interface09_S	DB	0B3h,'  [ ] Error sound'		,0FFh,15,0B3h,'  ',0B3h,'  ( ) Auto detect'       ,0FFh,10,0B3h,0
Interface10_S	DB	0DAh,' ^bMouse options^b '	   ,0FFh,80h+17,0C4h,0BFh,'  ',0DAh,' ^bZoom options^b ' ,0FFh,80h+13,0C4h,0BFh,0
Interface11_S	DB	0B3h,'  [ ] Left-handed mouse'	,0FFh, 9,0B3h,'  ',0B3h,'  [ ] Zooming boxes'     ,0FFh, 8,0B3h,0
Interface12_S	DB	0B3h,'  [ ] Fast mouse reset'	,0FFh,10,0B3h,'  ',0B3h,'  Zoom stages:'	       ,0FFh,13,0B3h,0
Interface13_S	DB	' ',0C0h		   ,0FFh,80h+32,0C4h,0D9h,'  ',0C0h		 ,0FFh,80h+27,0C4h,0D9h,' ',0
InterfErr1_S	DB	'Zoom stages value should be',0
InterfErr2_S	DB	'between 2 and 10. Try again.',0

		.DATA
Interface_P	DB	Conf_C, CnfRe_C, CnfBo_C, 0
		DW	Interface00_S
		DW	Interface01_S
		DW	Interface02_S
		DW	Interface03_S
		DW	Interface04_S
		DW	Interface05_S
		DW	Interface06_S
		DW	Interface07_S
		DW	Interface08_S
		DW	Interface09_S
		DW	Interface13_S
		DW	Interface10_S
		DW	Interface11_S
		DW	Interface12_S
		DW	Interface13_S
		DW	Prompt1_S
		DW	Prompt2_S
		DW	0FFFFh
		DW	OkCn_S
		DW	0
Interface_B	BOXHD	< Interface_P, 100h, M62_10, Dialog_K, Opt_M, Interface_H, 0 >
		BOXMN	<3,14, 1, 0,8Ah,  30Ah, Clock  , 0 >	; Clock
		BOXMN	<3, 0, 2, 1,8Ah,  40Ah, Prompt , 0 >	; Path prompt
		BOXMN	<3, 1, 3, 2,8Ah,  50Ah, MenuVs , 0 >	; Menu visible
		BOXMN	<3, 2, 4, 3,8Ah,  60Ah, AltBar , 0 >	; Alt selects menu
		BOXMN	<3, 3, 5, 4,8Bh,  70Ah, EscBar , 0 >	; Esc toggles panels
		BOXMN	<3, 4, 6, 5,8Bh,  80Ah, ClrBuf , 0 >	; Clear keyboard buffer
		BOXMN	<3, 5, 7, 6,8Bh,  90Ah, QuitAck, 0 >	; Confirm on quit
		BOXMN	<3, 6, 8, 7,8Bh, 0A0Ah, ErrSnd , 0 >	; Error sound
		BOXMN	<3, 7, 9, 8, 12, 0D0Ah, LftMous, 0 >	; Left handed mouse
		BOXMN	<3, 8,14, 9, 13, 0E0Ah, MousRes, 0 >	; Fast mouse reset
		BOXMN	<2, 9,8Bh, 0,10,  32Eh, ColTyp , 3 >	; Screen colors
		BOXMN	<2,0CAh,12,5,11,  82Eh, SnowMod, 3 >	; Snow mode
		BOXMN	<3,0CBh,13,8,12, 0D2Eh, ZoomWin, 0 >	; Zooming boxes
		BOXMN	<1, 12,14, 9,13, 0E3Ah, TmpBuf , 5 >	; Zoom stages
		BOXMN	<4, 13, 0,14,14, 13F6h, OkCn_D , 0 >	; Ok / Cancel
		DB	0
InterfErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Interface00_S
		DW	InterfErr1_S
		DW	InterfErr2_S
		DW	Ok_S
		DW	0
InterfErr_B	BOXHD	< InterfErr_P, 0C00h, 0, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0

		.CODE
M62_00:		PUSH	ES			; Interface options
		POP	DS
		MOV	DI,OFFSET TmpBuf
		MOV	AL,DS:ZoomStages
		CMP	AL,10
		JBE	M62_01
		MOV	AL,10
M62_01:		CALL	HexDec
		CMP	AL,'0'
		JE	M62_02
		STOSB
M62_02:		MOV	AL,AH
		STOSB
		XOR	AX,AX
		STOSW
		MOV	BP,OFFSET Interface_B
		CALL	Dialog
		JNZ	Func04
		JMP	F34_60

		.CODE	Subs
M62_10		PROC	NEAR			; Test zoom stages
		CMP	AH,0FCh
		JAE	@@Exit
		CMP	AL,0Dh
		JE	@@ChkNumb
		CMP	AL,0Ah
		JNE	@@Exit
  @@ChkNumb:	MOV	AX,011Bh
		OR	CL,CL
		JNE	@@Exit
		PUSH	SS
		POP	DS
		LEA	SI,[DI-21]
		CALL	FndLet
		CMP	AL,0
		JE	@@Enter
		CALL	TxtNum
		JC	@@Error
		CMP	AX,10
		JA	@@Error
		CMP	AL,2
		JB	@@Error
		PUSH	AX
		CALL	FndLet
		CMP	AL,0
		POP	AX
		JNE	@@Error
		MOV	ES:ZoomStages,AL
  @@Enter:	MOV	AX,1C0Dh
		JMP	@@Exit
  @@Error:	MOV	BP,OFFSET InterfErr_B
		CALL	Dialog
		XOR	AX,AX
  @@Exit:	RET
M62_10		ENDP

		.CONST
Chars0_S	DB			' Characters ',0
Chars1_S	DB	0DAh,' ^bBorders^b '		      ,0FFh,80h+36,0C4h,0BFh,0
Chars2_S	DB	0B3h,'  Panels and'	      ,0FFh,11,'Pull-down' ,0FFh,13,0B3h,0
Chars3_S	DB	0B3h,'  dialogs'	      ,0FFh,14,'menu'	   ,0FFh,18,0B3h,0
Chars4_S	DB	0DAh,' ^bOther^b '			      ,0FFh,80h+38,0C4h,0BFh,0
Chars5_S	DB	0B3h,'  Left/Right scroll',0FFh,4,'Scroll bars',0FFh,11,0B3h,0
Chars6_S	DB	0B3h,'  Mouse pointers'  ,0FFh,7,'Progress bars',0FFh,9,0B3h,0
Chars7_S	DB	0B3h,'  Items switched on',0FFh,4,'Hidden/selected files ',0B3h,0
Chars8_S	DB	0B3h					 ,0FFh,45,0B3h,0
Chars9_S	DB	' ',0C0h			      ,0FFh,80h+45,0C4h,0D9h,' ',0

		.DATA
Chars_P		DB	Conf_C, CnfRe_C, CnfBo_C, 0
		DW	Chars0_S
		DW	Chars1_S
		DW	Chars2_S
		DW	Chars3_S
		DW	Chars8_S
		DW	Chars8_S
		DW	Chars9_S
		DW	Chars4_S
		DW	Chars5_S
		DW	Chars8_S
		DW	Chars6_S
		DW	Chars8_S
		DW	Chars7_S
		DW	Chars8_S
		DW	Chars9_S
		DW	0FFFFh
		DW	OkCn_S
		DW	0
Chars_B		BOXHD	< Chars_P, 200h, M63_20, Dialog_K, Opt_M, Chars_H, 0 >
		BOXMN	<1, 13, 1, 0, 4, 0314h, TmpBuf   , 104h> ; Borders 1
		BOXMN	<1,  0, 2, 1, 5, 0414h, TmpBuf+10, 104h> ; Borders 2
		BOXMN	<1,  1, 3, 2, 6, 0514h, TmpBuf+20, 104h> ; Borders 3
		BOXMN	<1,  2, 7, 3, 6, 0614h, TmpBuf+30, 104h> ; Borders 4
		BOXMN	<1,  9, 5, 0, 4, 0328h, TmpBuf+40, 103h> ; Menu 1
		BOXMN	<1,  4, 6, 1, 5, 0428h, TmpBuf+50, 103h> ; Menu 2
		BOXMN	<1,  5,10, 2, 6, 0528h, TmpBuf+60, 103h> ; Menu 3
		BOXMN	<1,  3, 8, 7,10, 0A0Eh, TmpBuf+70, 102h> ; Scrolls
		BOXMN	<1,  7, 9, 8,11, 0C0Eh, TmpBuf+80, 107h> ; Pointers
		BOXMN	<1,  8,13, 9,12, 0E0Eh, TmpBuf+90 ,101h> ; Items on
		BOXMN	<1,  6,11, 7,10, 0A23h, TmpBuf+100,104h> ; Scroll bars
		BOXMN	<1, 10,12, 8,11, 0C23h, TmpBuf+110,102h> ; Progress bars
		BOXMN	<1, 11,13, 9,12, 0E23h, TmpBuf+120,103h> ; Hidden files
		BOXMN	<4, 12, 0,13,13, 11F6h, OkCn_D   , 0   > ; Ok / Cancel
		DB	0
CharBords_T	DB	0,  2, 11, 1
		DB	6,' ', 12, 7
		DB	8, 10, 13, 9
		DB	3,  5, 14, 4
CharMenus_T	DB	0, 2, 1
		DB	6, 8, 7
		DB	3, 5, 4
CharOther_T	DW	Scrolls,	2
		DW	Pointers,	7
		DW	ItemOn,		1
		DW	ScrollBars,	4
		DW	ProgressBars,	2
		DW	FileMarks,	3

		.CODE
M63_00:		PUSH	ES			; Characters
		POP	DS
		MOV	DI,OFFSET TmpBuf
		MOV	BX,OFFSET Borders
		MOV	SI,OFFSET CharBords_T
		MOV	DX,404h
		CALL	M63_10
		MOV	BX,OFFSET MenuBords
		MOV	SI,OFFSET CharMenus_T
		MOV	DX,303h
		CALL	M63_10
		MOV	BX,OFFSET CharOther_T
		MOV	DL,6
M63_01:		PUSH	DS
		MOV	DS,CS:SubsSeg
		MOV	SI,DS:[BX+0]
		MOV	AX,DS:[BX+2]
		MOV	CX,AX
		POP	DS
		REP	MOVSB
		MOV	CL,10
		SUB	CX,AX
		MOV	AL,0
		REP	STOSB
		ADD	BX,4
		DEC	DL
		JNE	M63_01
		MOV	BP,OFFSET Chars_B
		CALL	Dialog
		JNZ	Func04
		MOV	DI,OFFSET TmpBuf
		MOV	BX,OFFSET Borders
		MOV	SI,OFFSET CharBords_T
		MOV	DX,404h
		CALL	M63_11
		MOV	BX,OFFSET MenuBords
		MOV	SI,OFFSET CharMenus_T
		MOV	DX,303h
		CALL	M63_11
		MOV	BX,OFFSET CharOther_T
		MOV	DL,6
M63_02:		CALL	M63_12
		PUSH	DI
		PUSH	DS
		MOV	SI,DI
		MOV	DS,CS:SubsSeg
		MOV	DI,DS:[BX+0]
		MOV	CX,DS:[BX+2]
		POP	DS
		REP	MOVSB
		POP	DI
		ADD	DI,10
		ADD	BX,4
		DEC	DL
		JNE	M63_02
		JMP	F34_60

		.CODE	Subs
M63_10		PROC	FAR
  @@MoveLine:	MOV	CL,DL
		MOV	CH,0
  @@MoveChar:	PUSH	BX
		LODS	BYTE PTR CS:[SI]
		MOV	AH,0
		ADD	BX,AX
		CMP	AL,' '
		JAE	@@SaveChar
		MOV	AL,DS:[BX]
  @@SaveChar:	STOSB
		POP	BX
		LOOP	@@MoveChar
		MOV	CL,10
		SUB	CL,DL
		MOV	AL,0
		REP	STOSB
		DEC	DH
		JNE	@@MoveLine
		RET
M63_10		ENDP

M63_11		PROC	FAR
  @@MoveLine:	PUSH	DI
  		CALL	M63_12
		MOV	CL,DL
		MOV	CH,0
  @@MoveChar:	PUSH	BX
		LODS	BYTE PTR CS:[SI]
		MOV	AH,0
		ADD	BX,AX
		CMP	AL,' '
		JAE	@@NextChar
		MOV	AL,DS:[DI]
		MOV	DS:[BX],AL
  @@NextChar:	INC	DI
		POP	BX
		LOOP	@@MoveChar
		POP	DI
		ADD	DI,10
		DEC	DH
		JNE	@@MoveLine
		RET
M63_11		ENDP

M63_12		PROC	FAR
		USES	DI
		MOV	CX,10
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		MOV	AL,' '
		REP	STOSB
		RET
M63_12		ENDP

M63_20		PROC	NEAR
		USES	AX
		CMP	AX,0FFFEh
		JAE	@@Exit
		MOV	BP,DI
		LEA	DI,[SI]+SIZE BoxHd
		MOV	BX,BP
		MOV	CX,4
		CALL	M63_21
		ADD	DI,4*SIZE BoxMn
		MOV	CX,3
		CALL	M63_21
  @@Exit:	RET
M63_20		ENDP

M63_21		PROC	NEAR
		CALL	DialogBuf
		PUSH	BX
		PUSH	CX
		XCHG	SI,BX
		SUB	SI,BX
		MOV	DL,SS:[BX+1]
		MOV	DH,DL
		DEC	CX
  @@GetOffset:	SUB	BX,SI
		MOV	AL,SS:[BX+1]
		CMP	AL,DL
		JE	@@GetNext
		XCHG	DL,DH
		CMP	AL,DL
		JE	@@GetNext
		MOV	DH,AL
  @@GetNext:	LOOP	@@GetOffset
  		POP	CX
		POP	BX
  @@SetCur:	SUB	BX,SI
		MOV	SS:[BX+1],DH
		LOOP	@@SetCur
		RET
M63_21		ENDP

		.CONST
PanelOpt00_S	DB			' Panel options ',0
PanelOpt01_S	DB	' ',0DAh,' ^bFile panel^b '			,0FFh,80h+34,0C4h,0BFh,' ',0
PanelOpt02_S	DB	0B3h,'  [ ] Ins moves down'			      ,0FFh,26,0B3h,0
PanelOpt03_S	DB	0B3h,'  [ ] Ins calculates size of directory'     ,0FFh, 8,0B3h,0
PanelOpt04_S	DB	0B3h,'  [ ] Auto directory sizes'		      ,0FFh,20,0B3h,0
PanelOpt05_S	DB	0B3h,'  [ ] Unselect during group operations'     ,0FFh, 8,0B3h,0
PanelOpt06_S	DB	0B3h,'  [ ] Highlight subdirectories'	      ,0FFh,16,0B3h,0
PanelOpt07_S	DB	0DAh,' ^bTree panel^b '			 ,0FFh,80h+34,0C4h,0BFh,0
PanelOpt08_S	DB	0B3h,'  [ ] Show hidden subdirectories'	      ,0FFh,14,0B3h,0
PanelOpt09_S	DB	0B3h,'  [ ] Save TREEINFO in root directory'      ,0FFh, 9,0B3h,0
PanelOpt10_S	DB	0B3h,'  [ ] Save TREEINFO in home directory'      ,0FFh, 9,0B3h,0
PanelOpt11_S	DB	0B3h,'  [ ] Auto change directory'		      ,0FFh,19,0B3h,0
PanelOpt12_S	DB	0B3h,'  Auto change directory time delay:'	      ,0FFh,11,0B3h,0
PanelOpt13_S	DB	0C0h				 ,0FFh,80h+46,0C4h,0D9h,0
PanelErr1_S	DB	'Auto change directory time delay',0
PanelErr2_S	DB	'is incorrect. Try again.',0

		.DATA
PanelOpt_P	DB	Conf_C, CnfRe_C, CnfBo_C, 0
		DW	PanelOpt00_S
		DW	PanelOpt01_S
		DW	PanelOpt02_S
		DW	PanelOpt03_S
		DW	PanelOpt04_S
		DW	PanelOpt05_S
		DW	PanelOpt06_S
		DW	PanelOpt13_S
		DW	PanelOpt07_S
		DW	PanelOpt08_S
		DW	PanelOpt09_S
		DW	PanelOpt10_S
		DW	PanelOpt11_S
		DW	PanelOpt12_S
		DW	PanelOpt13_S
		DW	Prompt1_S
		DW	Prompt2_S
		DW	0FFFFh
		DW	OkCn_S
		DW	0
PanelOpt_B	BOXHD	< PanelOpt_P, 100h, M64_10, Dialog_K, Opt_M, PanelOpt_H, 0 >
		BOXMN	<3,10, 1, 0, 0,  30Ah, InsDwn    , 0 >	; Ins moves down
		BOXMN	<3, 0, 2, 1, 1,  40Ah, InsDirSize, 0 >	; Ins calculates size of directory
		BOXMN	<3, 1, 3, 2, 2,  50Ah, AutoSiz	 , 0 >	; Auto directory sizes
		BOXMN	<3, 2, 4, 3, 3,  60Ah, UnSlct    , 0 >	; Unselect during group operations
		BOXMN	<3, 3, 5, 4, 4,  70Ah, ColorDirs , 0 >	; Highlight subdirectories
		BOXMN	<3, 4, 6, 5, 5, 0A0Ah, HidDirs   , 0 >	; Show hidden subdirectories
		BOXMN	<3, 5, 7, 6, 6, 0B0Ah, TreeInRoot, 0 >	; Save TREEINFO in root
		BOXMN	<3, 6, 8, 7, 7, 0C0Ah, TreeInHome, 0 >	; Save TREEINFO in home
		BOXMN	<3, 7, 9, 8, 8, 0D0Ah, AutoCD    , 0 >	; Auto change directory
		BOXMN	<1, 8,10, 9, 9, 0E2Bh, TmpBuf    , 5 >	; Auto change directory delay time
		BOXMN	<4, 9, 0,10,10, 13F6h, OkCn_D    , 0 >	; Ok / Cancel
		DB	0
PanelErr_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	PanelOpt00_S
		DW	PanelErr1_S
		DW	PanelErr2_S
		DW	Ok_S
		DW	0
PanelErr_B	BOXHD	< PanelErr_P, 0C00h, 0, Dialog_K, Def_M, Default_H, 0 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0

		.CODE
M64_00:		PUSH	ES			; Panel options
		POP	DS
		MOV	DI,OFFSET TmpBuf
		MOV	AL,DS:DelayTm[1]
		CMP	AL,99
		JBE	M64_01
		MOV	AL,99
M64_01:		CALL	HexDec
		CMP	AL,'0'
		JE	M64_02
		STOSB
M64_02:		MOV	AL,AH
		MOV	AH,DS:Country.DecSep
		STOSW
		MOV	AL,DS:DelayTm[0]
		CMP	AL,99
		JBE	M64_03
		MOV	AL,99
M64_03:		CALL	HexDec
		STOSW
		XOR	AX,AX
		STOSW
		MOV	BP,OFFSET PanelOpt_B
		JMP	M61_01

		.CODE	Subs
M64_10		PROC	NEAR			; Test delay time
		CMP	AH,0FCh
		JAE	@@Exit
		CMP	AL,0Dh
		JE	@@ChkTime0
		CMP	AL,0Ah
		JNE	@@Exit
  @@ChkTime0:	MOV	AX,011Bh
		OR	CL,CL
		JNE	@@Exit
		PUSH	SS
		POP	DS
		LEA	SI,[DI-15]
		CALL	FndLet
		CMP	AL,0
		JE	@@Enter
		XOR	CX,CX
		CMP	AL,ES:Country.DecSep
		JE	@@ChkTime1
		CALL	TxtNum
		JC	@@Error
		CMP	AX,99
		JA	@@Error
		MOV	CH,AL
		PUSH	SI
		CALL	FndLet
		POP	SI
		CMP	AL,0
		JE	@@SaveTime1
		MOV	AL,DS:[SI]
		CMP	AL,ES:Country.DecSep
		JNE	@@Error
		PUSH	SI
		INC	SI
		CALL	FndLet
		POP	SI
		CMP	AL,0
		JE	@@SaveTime1
  @@ChkTime1:	INC	SI
		PUSH	SI
		MOV	CL,0FFh
  @@ChkTime2:	INC	CL
		LODSB
		CMP	AL,'0'
		JE	@@ChkTime2
		POP	SI
		CALL	TxtNum
		JC	@@Error
		PUSH	AX
		CALL	FndLet
		CMP	AL,0
		POP	AX
		JNE	@@Error
		CMP	AX,10
		JAE	@@ChkTime3
		MOV	AH,10
		MUL	AH
  @@ChkTime3:	CMP	CL,1
		MOV	BX,100
		JB	@@ChkTime4
		MOV	BX,10
		JE	@@ChkTime4
		MOV	BL,1
  @@ChkTime4:	CMP	AX,BX
		JB	@@SaveTime0
		XOR	DX,DX
		MOV	SI,10
		DIV	SI
		JMP	@@ChkTime4
  @@SaveTime0:	MOV	CL,AL
  @@SaveTime1:	MOV	WORD PTR ES:DelayTm,CX
  @@Enter:	MOV	AX,1C0Dh
		JMP	@@Exit
  @@Error:	MOV	BP,OFFSET PanelErr_B
		CALL	Dialog
		XOR	AX,AX
  @@Exit:	RET
M64_10		ENDP

		.CONST
Descript0_S	DB			' Descriptions ',0
Descript1_S	DB	0DAh,' ^bProcess descriptions^b ' ,0FFh,80h+38,0C4h,0BFh,0
Descript2_S	DB	0B3h,'  ( ) Do not update descriptions',0FFh,28,0B3h,0
Descript3_S	DB	0B3h,'  ( ) Update if displayed'	   ,0FFh,35,0B3h,0
Descript4_S	DB	0B3h,'  ( ) Always update'		   ,0FFh,41,0B3h,0
Descript5_S	DB	0C0h		      ,0FFh,80h+60,0C4h,0D9h,0
Descript6_S	DB	'Description files',0FFh,LenStr-17,0
Descript7_S	DB	'Directory information files',0FFh,LenStr-27,0

		.DATA
Descript_P	DB	Conf_C, CnfRe_C, CnfBo_C, 0
		DW	Descript0_S
		DW	Descript1_S
		DW	Descript2_S
		DW	Descript3_S
		DW	Descript4_S
		DW	Descript5_S
		DW	Prompt1_S
		DW	Prompt2_S
		DW	0FFFFh
		DW	Descript6_S
		DW	Empty_S
		DW	Empty_S
		DW	Descript7_S
		DW	Empty_S
		DW	0FFFFh
		DW	OkCn_S
		DW	0
Descript_B	BOXHD	< Descript_P, 300h, 0, Dialog_K, Opt_M, Descript_H, 0 >
		BOXMN	<2, 3, 1, 0, 0,  30Ah, DescrProcess, 3 >   ; Process descriptions
		BOXMN	<1, 0C0h,2,1,1, 0B05h,DescrFiles,LenStr>   ; Description files
		BOXMN	<1, 1, 3, 2, 2, 0E05h,DirInfoFiles,LenStr> ; Description files
		BOXMN	<4, 2,80h,3, 3, 10F6h, OkCn_D	   , 0 >   ; Ok / Cancel
		DB	0

		.CODE
M65_00:		PUSH	ES			; Descriptions
		POP	DS
		MOV	BP,OFFSET Descript_B
		JMP	M61_01

		.CONST
CopyOpt00_S	DB			' Copy and Move options ',0
CopyOpt01_S	DB	' ',0DAh,' ^bDescriptions^b '				   ,0FFh,80h+48,0C4h,0BFh,' ',0
CopyOpt02_S	DB	' ',0B3h,'  [ ] Auto create descriptions'			,0FFh,32,0B3h,' ',0
CopyOpt03_S	DB	' ',0B3h,'  [ ] Delete target description if source without description ',0B3h,' ',0
CopyOpt04_S	DB	' ',0B3h,'  [ ] Auto delete empty description files'		,0FFh,21,0B3h,' ',0
CopyOpt05_S	DB	' ',0DAh,' ^bOS/2 options^b '				   ,0FFh,80h+48,0C4h,0BFh,' ',0
CopyOpt06_S	DB	' ',0B3h,'  [ ] Copy extended attributes'			,0FFh,32,0B3h,' ',0
CopyOpt07_S	DB	' ',0DAh,' ^bOther options^b '				   ,0FFh,80h+47,0C4h,0BFh,' ',0
CopyOpt08_S	DB	' ',0B3h,'  [ ] Check target file and free space before copying',0FFh,9,0B3h,' ',0
CopyOpt09_S	DB	' ',0B3h,'  [ ] Clear read-only attribute when copy from CD-ROM',0FFh,9,0B3h,' ',0
CopyOpt10_S	DB	' ',0C0h					   ,0FFh,80h+62,0C4h,0D9h,' ',0

		.DATA
CopyOpt_P	DB	Conf_C, CnfRe_C, CnfBo_C, 0
		DW	CopyOpt00_S
		DW	CopyOpt01_S
		DW	CopyOpt02_S
		DW	CopyOpt03_S
		DW	CopyOpt04_S
		DW	CopyOpt10_S
		DW	CopyOpt05_S
		DW	CopyOpt06_S
		DW	CopyOpt10_S
		DW	CopyOpt07_S
		DW	CopyOpt08_S
		DW	CopyOpt09_S
		DW	CopyOpt10_S
		DW	Prompt1_S
		DW	Prompt2_S
		DW	0FFFFh
		DW	OkCn_S
		DW	0
CopyOpt_B	BOXHD	< CopyOpt_P, 200h, 0, Dialog_K, Opt_M, CopyOpt_H, 0 >
		BOXMN	<3, 6, 1, 0, 0,  30Ah, DescrCreate, 0 >	; Create target description file if not exists
		BOXMN	<3, 0, 2, 1, 1,  40Ah, DescrEmpty , 0 >	; Delete target description if source empty
		BOXMN	<3, 1, 3, 2, 2,  50Ah, DescrDel   , 0 >	; Delete empty description file
		BOXMN	<3, 2, 4, 3, 3,  80Ah, CopyEAs    , 0 >	; Copy OS/2 extended attributes
		BOXMN	<3, 3, 5, 4, 4, 0B0Ah, ChkTarget  , 0 >	; Check target
		BOXMN	<3, 4, 6, 5, 5, 0C0Ah, ClearRO	  , 0 >	; Clear R/O attribute
		BOXMN	<4, 5, 0, 6, 6, 11F6h, OkCn_D     , 0 >	; Ok / Cancel
		DB	0

		.CODE
M67_00:		PUSH	ES			; Copy options
		POP	DS
		MOV	BP,OFFSET CopyOpt_B
		CALL	Dialog
		JMP	Func04

COMMENT	|
	.CONST
ExFl_S	DB	' Extension File ',0
MnLc1_S	DB	'Do you wish to edit the main,',0
MnLc2_S	DB	'or a local ^I?',0
UsMnu_S	DB	'user menu',0
ExtFl_S	DB	'extension file',0
MnLc3_S	DB	'Main',0FFh,3,'Local',0FFh,3,'Cancel',0

	.DATA
MnLoc_P	DB	Box_C, BoxCu_C, BoxCu_C, 0
	DB	4, 1
	DW	OFFSET WinBuf
	DW	TmpBuf
	DB	0, 50
	DW	OFFSET Var_S
	DW	OFFSET MnLc1_S
	DW	OFFSET MnLc2_S
	DW	OFFSET MnLc3_S
MnLoc_B	BOXHD	< MnLoc_P, 528h, 0, Dialog_K, Def_M, Default_H, 0 >
	BOXMN	< 4, 0,0,0,0, 41Ch, TmpBuf, MnLoc_D>
	DB	0
MnLoc_D	DB	6, 0
	DB	7, 7
	DB	8,15
	DB	0

	.CODE
M54_00:	MOV	BX,OFFSET Menu_F	; Menu file edit
	MOV	SI,OFFSET User_S
	MOV	DX,OFFSET UsMnu_S
	MOV	AL,EditMenu_H
	JMP	M71_02

M69_00:	MOV	BX,OFFSET Ext_F		; Extension file edit
	MOV	AL,EditExt_H
	JMP	M71_01

M70_00:	MOV	BX,OFFSET ViewE_F	; Viewer file
	MOV	AL,EditView_H
	JMP	M71_01

M71_00:	MOV	BX,OFFSET EditE_F	; Editor file
	MOV	AL,EditEdit_H
M71_01:	MOV	SI,OFFSET ExFl_S
	MOV	DX,OFFSET ExtFl_S
M71_02:	MOV	ES:HlpPage,AL
	PUSH	CX
	MOV	AL,CL
	CALL	PutMnu
	PUSH	CS
	POP	DS
	MOV	DI,OFFSET TmpBuf
	MOV	CX,80
	REP	MOVSB
	PUSH	ES
	POP	DS
	MOV	BP,OFFSET MnLoc_B
	CALL	Dialog
	CMP	AL,2
	JB	M71_03
	POP	CX
	JMP	Func04
M71_03:	PUSH	CS
	POP	DS
	MOV	SI,BX
	MOV	DI,OFFSET TmpBuf
	PUSH	DI
	MOV	CX,13
	PUSH	DI
	REP	MOVSB
	POP	DI
	PUSH	ES
	POP	DS
	MOV	SI,BX
	CMP	AL,0
	JNE	M71_04
	CALL	MainFil
M71_04:	POP	SI
	CALL	LetLin
	MOV	DI,OFFSET SourceFile
	MOV	CX,LenStr
	REP	MOVSB
	MOV	AL,0
	STOSB
	POP	AX
	JMP	T00_01
|

M73_00:		MOV	BX,OFFSET AutoMnu	; Auto menus
M73_01:		CALL	InvVar
		JMP	F69_22

M75_00:		CALL	BegBX			; Full screen
		CALL	SizDX
		MOV	BL,0
		MOV	DL,ES:Columns
		CALL	ResUser
		MOV	BX,OFFSET FulScr
		CALL	InvVar
		JMP	F69_22

M76_00:		MOV	BX,OFFSET MiniSta	; Mini status
		JMP	M73_01

M77_00:		MOV	BX,OFFSET LoadFl	; Memory allocation
		JMP	M73_01

		.CONST
SvSt0_S		DB	' Setup ',0
SvSt1_S		DB	'Save the current setup as',0FFh,LenStr-25,0
SvSt2_S		DB	'[ Save ]',0FFh,3,'[ Cancel ]',0
StEr1_S		DB	'There was an error while',0
StEr2_S		DB	'saving the setup file',0

		.DATA
SvSt_P		DB	Box_C, BoxCu_C, BoxCu_C, 0
		DW	SvSt0_S
		DW	SvSt1_S
		DW	Empty_S
		DW	0FFFFh
		DW	SvSt2_S
		DW	0
SvSt_B		BOXHD	< SvSt_P, 500h, 0, Dialog_K, Opt_M, SaveSet_H, 0 >
		BOXMN	< 1, 1,1,0,0, 305h, LineBuf1, LenStrFull+LenStr >
		BOXMN	< 4, 0,0,1,1, 5F5h, SvSt_D, 0 >
		DB	0
SetEr_P		DB	Err_C, ErInv_C, ErInv_C, 0
		DW	SvSt0_S
		DW	StEr1_S
		DW	StEr2_S
		DW	Ok_S
		DW	0
SetEr_B		BOXHD	< SetEr_P, 900h, 0, Dialog_K, Def_M, Default_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0
SvSt_T		DW	IniFile
SvSt_D		DB	 8,  0
		DB	10, 11
		DB	0

		.CODE
F49_00:		PUSH	ES			; Save setup
		POP	DS
		MOV	SI,OFFSET IniFile
		MOV	DI,OFFSET LineBuf1
		PUSH	DI
		CALL	strcpy
		STOSB
		STOSB
		MOV	DX,OFFSET SvSt_T
		MOV	BP,OFFSET SvSt_B
		CALL	Dialog
		POP	DX
		JNE	Func04
		CALL	SaveSet
		JNC	F49_01
		CMP	AX,53h
		JE	F49_01
		MOV	BP,OFFSET SetEr_B
		CALL	Dialog
F49_01:		JMP	M61_02
