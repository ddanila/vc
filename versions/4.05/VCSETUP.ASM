;-------------------------------------------------------
;		Version :	14.06.2000
;-------------------------------------------------------

	.MODEL	Tiny

PrgName	EQU	'Volkov Commander'
VcVers	EQU	'4.05 freeware'
HelpVer	EQU	'VC 4.05 Help'
VcApiVr	EQU	42

MoDelay	EQU	40			; Mouse double click delay
MiliSec	EQU	60			; auto CD time : sec x 0.01
Sec	EQU	0			;                sec x 1
LenPath	EQU	68			; Length of path name
LenColr	EQU	40			; Length of color table
LenStr	EQU	40			; Length of string
MaxLins	EQU	50			; Maximum screen lines
NmbStrs	EQU	32			; Number of stars

;------------------ Global structures ------------------
TYPES	UNION
  Byt	DB	?			; Byte definition
  Wrd	DW	?			; Word definition
TYPES	ENDS

; Far address structure definition.
FARAD	STRUC
  Offs	DW	?			; Offset
  Segm	DW	?			; Segment
FARAD	ENDS

; Long type structure definition.
LONG	STRUC
  Lo	DW	?			; Low word
  Hi	DW	?			; High word
LONG	ENDS

; Country information structure definition.
CNTRY	STRUC
  DateFmt DW	?			; Date format
	  DB	5 DUP(?)		; Currency
  Sep1000 DB	2 DUP(?)		; Thousand separator
  DecSep  DB	2 DUP(?)		; Decimal separator
  DateSep DB	2 DUP(?)		; Date separator
  TimeSep DB	2 DUP(?)		; Time separator
	  DB	?			; Currency format
	  DB	?			; Number of meaning digits in currency
  TimeFmt DB	?			; Time format
  CaseMap DD	?			; Addres of case map
	  DB	2 DUP(?)		; Data separator
	  DB	8 DUP(?)		; Reserved
CNTRY	ENDS

; The dialog box header structure definition.
BOXHD	STRUC
  pDiBox  DW	?			; Pointer to DialBox structure
  BoxBase DW	?			; The dialog box begin-center
  SubFunc DW	?			; Subfunction
  bKeyBar DB	?			; KeyBar
  bMenu   DB	0FFh			; PutMnu
  bHelp   DB	0			; HlpPage
  bFlags  DB	0			; Flags
BOXHD	ENDS

; The dialog box header structure definition.
BOXMN	STRUC
  mType	  DB	?			; Menu type
  mUp	  DB	0			; Up link
  mDown	  DB	0			; Down link
  mLeft	  DB	0			; Left link
  mRight  DB	0			; Right link
  mBase	  DW	?			; Menu base
  mPoint  DW	?
  mSelTot LABEL	BYTE
  mCtrl	  DW	?
BOXMN	ENDS

; The star structure definition.
STAR	STRUC
  StarClk  LABEL   BYTE			; Time counter
  StarMod  DW	   ?			; Low byte - counter, High - explosion
  StarPos  DW	   ?			; Coordinates of star
STAR	ENDS

;-------------------- WCB definition -------------------
WCBdef	STRUC
WinTyp	DB	?
BrfFul	DB	?
Visible	DB	?
Hidden	DB	?
MasTyp	DB	?
SortTyp	DB	?
Ctrl_L	DB	?
DskMas	DB	13	DUP(?)
WinPath	DB	LenPath	DUP(?)
WCBini	=	$			; End of WCB part saved in VC.INI
WCBdef	ENDS

;-------------------- Data segment ---------------------
	.DATA?
IniData	DB	3 DUP(?)		; 'VVV' header
WCB1	DB	WCBini DUP(?)		; Left window control block
WCB2	DB	WCBini DUP(?)		; Right window control block
ColTab	DB	6*LenColr DUP(?)	; Color table (6 pallets)
FulScr	DB	?			; Full screen
MenuVs	DB	?			; Menu always visible
KeyBar	DB	?			; Key bar
Prompt	DB	?			; Path prompt
InsDwn	DB	?			; Ins moves down
AutoMnu	DB	?			; Auto user menus
MiniSta	DB	?			; Mini status
Clock	DB	?			; Clock
AutoCD	DB	?			; Auto change dir
AutoSav	DB	?			; Auto save setup
ColTyp	DB	?			; Monochrome / Color / LapTop
BlDelay	DB	?			; Screen blank delay
LoadFl	DB	?			; Small mode
UnSlct	DB	?			; Unselect files when group operation
ErrSnd	DB	?			; Beep when error
LftMous	DB	?			; Left handed mouse
ZoomWin	DB	?			; Zomming windows
AltBar	DB	?			; Alt alone selects menu
EscBar	DB	?			; Esc toggles panels on/off
ExecTyp	DB	?			; Exec command without COMMAND.COM
AutoSiz	DB	?			; Auto calculate directory sizes
HidDirs	DB	?			; Show hidden subdirs in tree panel
QuitAck	DB	?			; Confirmation on Quit
SnowMod	DB	?			; Snow mode (2=auto)
AltView	DB	?			; Alt-F3 - external viewer
AltEdit	DB	?			; Alt-F4 - external editor
ClrBuf	DB	?			; Clear buffer flag
Active	DB	?			; Active panel
Ctrl_O	DB	?			; Visible status of unactive panel
ChkTSR	DB	?			; TSR manager
ChgTree	DB	?			; Auto rescan tree
DriveB	DB	?			; Show logical drive B:
DelayTm	DB	?, ?			; Auto change dir delay time
StarTab	DB	5 DUP (?)		; Star table
	DB	51 DUP (?)		; Reserved
CmnIni	=	$ - ColTab		; Sise of main data' part of VC.INI
LoadRes	LABEL	BYTE			; Original LoadFl
	DW	?			; Check sum
IniLen	=	$ - IniData		; Length of VC.INI
	DB	?

IniStat	DB	?			; 0 - not found, 1 - Ok, 2 - bad
MnItem	DB	?			; Item of main menu
CurType	DW	?			; Shape of cursor
SrcIni	DB	CmnIni DUP(?)		; Source VC.INI

ScrMod	DB	?			; Screen mode
Mouse	DB	?			; 1 if Mouse was found
BreakFl	DB	?			; Break DOS's status
DosLin	DB	?			; Line number of DOS's command line
ScrPage	DB	?			; Current screen page number
TstEGA	DB	?			; 1 if EGA or VGA present
Lines	DB	?			; Number of lines on screen
Snow	DB	?			; Snow on/off
TopView	DB	?			; TopView output
DesqVie	DB	?			; running under DESQVIEW
Quote	DB	?			; Quote next character
BarPrev	DB	?			; Previous Key Bar
WinMous	DB	?
MousEnt	DB	?
HlpPage	DB	?
BackC	DB	?
HistC	DB	?
HstCuC	DB	?
ColCur	DW	?
ColFrst	DW	?
ColNum	DW	?
Rand1	DW	?
Rand2	DW	?
MousPos	DW	?
MousTim	DD	?
AutoTim	DD	?
Screen	DD	?			; Address of screen
BrkVct	DD	?			; Original vector 1Bh
NcPath	DB	LenPath+13 DUP(?)	; The path from VC= variable
Country	CNTRY	?			; Country information
LowTab	DB	128 DUP(?)		; Lower case table
TmpBuf	DB	256 DUP(?)
TmpErr	DB	256 DUP(?)
Stars	DW	2*NmbStrs DUP(?)
ScrBuf	DW	MaxLins * 80 DUP(?)
UserScr	DW	MaxLins * 80 DUP(?)
WinBuf	DW	(MaxLins-2) * 77 DUP(?)
BoxBuf	DW	(MaxLins-2) * 77 DUP(?)
HelpBuf	LABEL	BYTE

;-------------------------------------------------------
	.DATA
Back_C	EQU	0
Cursr_C	EQU	1
Slct_C	EQU	2
CurSl_C	EQU	3
Title_C	EQU	4
KeyNu_C	EQU	5
KeyTi_C	EQU	6
Menu_C	EQU	7
MnCur_C	EQU	8
MnuBx_C	EQU	9
MnuTx_C	EQU	10
MnuBr_C	EQU	11
MnuCu_C	EQU	12
MnuCB_C	EQU	13
MnuDr_C	EQU	14
MnuMi_C	EQU	15
Box_C	EQU	16
BoxBr_C	EQU	17
BoxCu_C	EQU	18
Conf_C	EQU	19
CnfRe_C	EQU	20
CnfBo_C	EQU	21
CnfUn_C	EQU	22
Hist_C	EQU	23
HstCu_C	EQU	24
HlBrd_C	EQU	25
HlpMn_C	EQU	26
Help_C	EQU	27
HlpBo_C	EQU	28
HlpRe_C	EQU	29
HlpUn_C	EQU	30
Err_C	EQU	31
ErInv_C	EQU	32
ErBr_C	EQU	33
Clock_C	EQU	34
Arrow_C	EQU	35
Sky1_C	EQU	36
Sky2_C	EQU	37
ShadFl	EQU	38
Palets	DB	07h,07h,1Bh,07h,07h,07h	;  0 - Main windows
	DB	70h,70h,30h,70h,70h,70h	;  1 - Cursor
	DB	0Fh,0Fh,1Eh,0Fh,0Fh,0Fh	;  2 - Selected files
	DB	7Fh,09h,3Eh,09h,70h,70h	;  3 - Cursor for selected files
	DB	0Fh,0Fh,1Eh,0Fh,0Fh,0Fh	;  4 - "Name   Size ..."
	DB	07h,07h,07h,07h,07h,07h	;  5 - Key bar numbers
	DB	70h,70h,30h,70h,70h,70h	;  6 - Key bar titles
	DB	70h,70h,30h,70h,70h,70h	;  7 - Menu bar
	DB	0Fh,0Fh,0Fh,0Fh,0Fh,0Fh	;  8 - Menu bar cursor
	DB	70h,70h,30h,70h,70h,70h	;  9 - Menu box border
	DB	70h,70h,3Fh,70h,70h,70h	; 10 - Menu box text
	DB	7Fh,70h,3Eh,70h,70h,70h	; 11 - Menu box high letters
	DB	07h,0Fh,0Fh,0Fh,0Fh,0Fh	; 12 - Menu box text cursor
	DB	0Fh,0Fh,0Eh,0Fh,0Fh,0Fh	; 13 - Menu box high letters cursor
	DB	70h,70h,30h,70h,70h,70h	; 14 - Menu box dark lines
	DB	70h,70h,33h,70h,70h,70h	; 15 - Menu box dark lines '-'
	DB	07h,07h,70h,07h,07h,07h	; 16 - Function box
	DB	0Fh,0Fh,7Eh,0Fh,0Fh,0Fh	; 17 - Function box bright
	DB	70h,70h,30h,70h,70h,70h	; 18 - Function box cursor
	DB	07h,07h,3Fh,07h,07h,07h	; 19 - Configuration window
	DB	70h,70h,0Fh,70h,70h,70h	; 20 - Configuration cursor
	DB	0Fh,0Fh,3Eh,0Fh,0Fh,0Fh	; 21 - Configuration window bright
	DB	7Fh,09h,0Eh,09h,70h,70h	; 22 - Configuration cursor bright
	DB	70h,70h,3Fh,70h,70h,70h	; 23 - History window
	DB	07h,07h,0Fh,07h,07h,07h	; 24 - History cursor
	DB	07h,07h,13h,07h,07h,07h	; 25 - Menu help border
	DB	70h,70h,30h,70h,70h,70h	; 26 - Menu help window
	DB	07h,07h,30h,07h,07h,07h	; 27 - Help window
	DB	0Fh,0Fh,3Fh,0Fh,0Fh,0Fh	; 28 - Help window bold
	DB	70h,70h,0Fh,70h,70h,70h	; 29 - Help reversive
	DB	0Fh,09h,3Eh,09h,0Fh,0Fh	; 30 - Help underline
	DB	07h,07h,4Fh,07h,07h,07h	; 31 - Error windows
	DB	70h,70h,70h,70h,70h,70h	; 32 - Error window cursor
	DB	0Fh,0Fh,4Eh,0Fh,0Fh,0Fh	; 33 - Error window bright
	DB	70h,70h,30h,70h,70h,70h	; 34 - Clock
	DB	0Fh,0Fh,1Eh,0Fh,0Fh,0Fh	; 35 - Arrows in view/edit
	DB	07h,07h,0Bh,07h,07h,07h	; 36 - Screen blank
	DB	0Fh,0Fh,0Fh,0Fh,0Fh,0Fh	; 37 - Screen blank light stars
	DB	02h,02h,01h,02h,00h,00h	; 38 - Shadow flag
	DB	02h,02h,02h,02h,02h,02h	; 39 - Blink / High background / None
DefInit	DB	1			; FulScr
	DB	0			; MenuVs
	DB	1			; KeyBar
	DB	1			; Prompt
	DB	1			; InsDwn
	DB	0			; AutoMnu
	DB	1			; MiniSta
	DB	0			; Clock
	DB	1			; AutoCD
	DB	1			; AutoSav
	DB	1			; ColTyp
	DB	5			; BlDelay
	DB	1			; LoadFl
	DB	1			; UnSlct
	DB	1			; ErrSnd
	DB	0			; LftMous
	DB	1			; ZoomWin
	DB	1			; AltBar
	DB	1			; EscBar
	DB	0			; ExecTyp
	DB	0			; AutoSiz
	DB	1			; HidDirs
	DB	1			; QuitAck
	DB	2			; SnowMod
	DB	1			; AltView
	DB	1			; AltEdit
	DB	1			; ClrBuf
	DB	1			; Active
	DB	0			; Ctrl_O
	DB	1			; ChkTSR
	DB	1			; ChgTree
	DB	1			; DriveB
	DB	MiliSec, Sec		; DelayTm
	DB	'˙˘'			; StarTab
	DB	51 DUP (0)		; Reserved
DefWCB	DB	0			; WinTyp
	DB	0			; BrfFul
	DB	1			; Visible
	DB	1			; Hidden
	DB	0			; MasTyp
	DB	8			; SortTyp
	DB	0			; Ctrl_L
	DB	13 DUP(0)		; DskMas
	DB	'A:\',0			; WinPath

	.CONST
	DB	13,10
Init_S	DB	       'The ',PrgName,' Setup, Version ',VcVers
	DB	13,10, 'Copyright (C) 1991-2000 Vsevolod V. Volkov'
	DB	13,10
	DB	13,10, '$'
Vers_S	DB	13,10, 'This program requires DOS 3.20 or later.'
	DB	13,10, '$'
Aloc_S	DB	13,10, 'Program too big to fit in memory'
	DB	13,10, '$'
Init_F	DB	'VC.INI'
Empty_S	DB	0
NcEnv	DB	'VC='
NcParm	DB	0,'Socha',0
	DW	11Fh

;------------------- Begin of program ------------------
	.CODE
	.STARTUP

	CLD
	MOV	AX,3000h		; Check DOS version
	INT	21h
	XCHG	AL,AH
	CMP	AX,314h			; 3.20 ?
	MOV	DX,OFFSET Vers_S
	MOV	AL,2
	JB	Init01
	PUSH	AX			; Allocate segments
	MOV	BX,1000h
	MOV	AH,4Ah
	INT	21h
	POP	AX
	JNC	Init03
	MOV	DX,OFFSET Aloc_S	; Not enough memory
	MOV	AL,1
Init01:	PUSH	AX
	MOV	AH,9
	INT	21h
	POP	AX
	CMP	AH,3
	JB	Init02
	MOV	AH,4Ch
	INT	21h
Init02:	RETN
Init03:	MOV	SP,0FFFEh

;--------------------- Output Report -------------------
	MOV	AH,0Fh
	INT	10h
	CMP	AH,80
	JNE	Init04
	AND	AL,7Fh
	CMP	AL,2
	JE	Init05
	CMP	AL,3
	JE	Init05
	CMP	AL,7
	JE	Init05
Init04:	MOV	AX,3
	INT	10h
Init05:	MOV	DX,OFFSET Init_S
	MOV	AH,9
	INT	21h

;------------------ Read configuration -----------------
	MOV	ES,DS:[2Ch]
	MOV	DX,OFFSET NcPath
	XOR	DI,DI
	MOV	CL,1
	MOV	AL,0
	MOV	DS:IniStat,AL
Init10:	CMP	AL,ES:[DI]
	JE	Init11
	MOV	SI,OFFSET NcEnv
	MOV	CX,3
	PUSH	DI
	REPE	CMPSB
	POP	DI
	JE	Init11
	MOV	CX,0FFFFh
	REPNE	SCASB
	JMP	Init10
Init11:	PUSH	CX
	PUSH	CS
	PUSH	ES
	POP	DS
	POP	ES
	LEA	SI,[DI+3]
	MOV	DI,DX
	MOV	CX,LenPath-1
	PUSH	DI
	REP	MOVSB
	PUSH	CS
	POP	DS
	POP	SI
	MOV	DS:[DI],CL
Init12:	MOV	BX,SI
Init13:	LODSB
	CMP	AL,'\'
	JE	Init12
	CMP	AL,':'
	JE	Init12
	CMP	AL,0
	JNE	Init13
	MOV	DI,SI
	DEC	DI
	POP	CX
	JCXZ	Init14
	MOV	DI,BX
Init14:	CMP	DI,DX
	JBE	Init15
	CMP	BYTE PTR DS:[DI-1],':'
	JE	Init15
	MOV	AL,'\'
	CMP	AL,DS:[DI-1]
	JE	Init15
	STOSB
Init15:	PUSH	DI			; Read VC.HLP
	PUSH	DX
	CALL	RdHelp
	POP	DX
	POP	DI
	MOV	SI,OFFSET Init_F
	MOV	CX,13
	REP	MOVSB
	MOV	AX,3D40h		; Open VC.INI
	INT	21h
	JC	Init18
	MOV	BX,AX
	MOV	DX,OFFSET IniData
	MOV	CX,IniLen+1
	MOV	AH,3Fh
	INT	21h
	PUSH	AX
	PUSHF
	MOV	AH,3Eh
	INT	21h
	POP	BP
	POP	AX
	JC	Init18
	PUSH	BP
	POPF
	JC	Init18
	INC	DS:IniStat
	DEC	CX			; Check VC.INI length
	CMP	AX,CX
	JNE	Init17
	MOV	DI,DX			; Check header
	MOV	AL,'V'
	PUSH	CX
	MOV	CX,3
	REPE	SCASB
	POP	CX
	JNE	Init17
	DEC	CX			; Check sum
	DEC	CX
	MOV	SI,DX
	XOR	DX,DX
	MOV	AH,0
Init16:	LODSB
	ADD	DX,AX
	LOOP	Init16
	LODSW
	CMP	AX,DX
	JE	Init20
Init17:	INC	DS:IniStat		; Bad data in VC.INI
Init18:	CALL	Default

;---------------- Initialize data segment --------------
Init20:	MOV	SI,OFFSET ColTab	; Save source VC.INI
	MOV	DI,OFFSET SrcIni
	MOV	CX,CmnIni
	REP	MOVSB
	MOV	AX,3300h		; Save BREAK status
	INT	21h
	MOV	DS:BreakFl,DL
	MOV	DL,0
	MOV	AX,3301h
	INT	21h
	PUSH	ES			; Set break vectors
	MOV	AX,351Bh
	INT	21h
	MOV	DS:BrkVct.Offs,BX
	MOV	DS:BrkVct.Segm,ES
	MOV	DX,OFFSET CtrlC
	MOV	AH,25h
	INT	21h
	MOV	AX,2523h
	INT	21h
	POP	ES

	MOV	DX,OFFSET Country	; Country
	MOV	AX,3800h
	INT	21h
	MOV	DI,OFFSET TmpBuf	; Lower case table
	MOV	SI,OFFSET LowTab
	MOV	CX,80h
	PUSH	DI
	PUSH	CX
Init21:	MOV	AL,CL
	NEG	AL
	MOV	DS:[SI],AL
	INC	SI
	CALL	DS:Country.CaseMap
	STOSB
	LOOP	Init21
	POP	CX
	POP	SI
	MOV	BH,0
	MOV	AH,80h
Init22:	LODSB
	CMP	AL,AH
	JE	Init23
	CMP	AL,80h
	JB	Init23
	MOV	BL,AL
	MOV	DS:LowTab[BX-128],AH
Init23:	INC	AH
	LOOP	Init22

	MOV	DS:Quote,0		; Quote
	MOV	DS:MnItem,0		; MnItem
	MOV	DS:BarPrev,0FFh		; BarPrev

	MOV	AH,0Fh			; Get screen mode
	INT	10h
	AND	AL,7Fh
	MOV	DS:ScrMod,AL
	MOV	DS:ScrPage,BH
	MOV	AH,3			; Save cursor position
	INT	10h
	MOV	DS:DosLin,DH
	CMP	CH,0Fh			; Correct cursor size
	JBE	Init24
	MOV	CX,607h
Init24:	CMP	CX,67h
	JNE	Init25
	MOV	CX,607h
Init25:	CMP	DS:ScrMod,7
	JNE	Init26
	CMP	CX,607h
	JNE	Init26
	MOV	CX,0B0Ch
Init26:	MOV	DS:CurType,CX
	MOV	DS:TstEGA,0		; TstEGA
	MOV	AH,12h
	MOV	BL,10h
	INT	10h
	PUSH	CX
	PUSH	BX
	NOT	BX
	NOT	CX
	MOV	AH,12h
	MOV	BL,10h
	INT	10h
	POP	AX
	POP	DX
	XOR	AX,BX
	XOR	DX,CX
	OR	AX,DX
	JNE	Init27
	MOV	DS:TstEGA,1
Init27:	MOV	AL,25			; Lines
	MOV	DL,0
	CMP	DL,DS:TstEGA
	JE	Init28
	PUSH	ES
	PUSH	AX
	MOV	BH,0
	MOV	AX,1130h
	INT	10h
	POP	AX
	POP	ES
	OR	DL,DL
	JE	Init28
	INC	DX
	MOV	AL,MaxLins
	CMP	AL,DL
	JBE	Init28
	MOV	AL,DL
Init28:	MOV	DS:Lines,AL
	PUSH	DS			; Screen Offset
	XOR	SI,SI
	MOV	AH,0Fh
	INT	10h
	OR	BH,BH
	JE	Init29
	MOV	DS,SI
	MOV	SI,DS:[44Eh]
Init29:	POP	DS
	MOV	DS:Screen.Offs,SI
	MOV	BX,0B800h		; Screen Segment
	CMP	DS:ScrMod,7
	JNE	Init30
	MOV	BX,0B000h
Init30:	MOV	DS:Screen.Segm,BX
	PUSH	ES			; Virtual screen buffer
	MOV	ES,BX
	MOV	AH,0FEh
	INT	10h
	MOV	AX,ES
	POP	ES
	MOV	CL,0
	CMP	AX,BX
	JE	Init31
	MOV	DS:Snow,1
	MOV	DS:Screen.Segm,AX
	MOV	DS:Screen.Offs,DI
	MOV	CL,1
Init31:	MOV	DS:TopView,CL
	MOV	CX,'DE'			; Check DESQview
	MOV	DX,'SQ'
	MOV	AX,2B01h
	INT	21h
	INC	AX
	MOV	DS:DesqVie,AL
	CALL	SetSnow			; Snow

	CALL	HidCur			; Hide cursor
	CALL	SavScr			; Save screen
	MOV	SI,OFFSET ScrBuf	; Restore screen
	MOV	DI,OFFSET UserScr
	MOV	AL,80
	MUL	DS:Lines
	MOV	CX,AX
	REP	MOVSW

	MOV	AX,21h			; Init mouse
	INT	33h
	CMP	AX,21h
	JNE	Init33
	XOR	AX,AX
	INT	33h
Init33:	CMP	AX,0FFFFh
	CMC
	MOV	AL,0
	ADC	AL,AL
	MOV	DS:Mouse,AL
	OR	AL,AL
	JZ	Init35
	MOV	AL,0
	CALL	TypMous
	MOV	DL,DS:Lines
	CMP	DL,25
	JE	Init34
	MOV	DH,0
	MOV	CL,3
	SHL	DX,CL
	DEC	DX
	XOR	CX,CX
	MOV	AX,8
	INT	33h
Init34:	MOV	AL,1
	CALL	CurMous
Init35:

;------------------------ Error ------------------------
	.CONST
Err_S	DB	' Error ',0
StFil_S	DB	'The setup file ^V',0
NoFnd_S	DB	'not found.',0
NoIni_S	DB	'contains data that is not correct.',0
OkEx_S	DB	'Ok',0FFh,3,'Exit',0
Inc_S	DB	'^I',0
Var_S	DB	'^V',0

	.DATA
IniEr_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 1
	DW	WinBuf
	DW	NcPath
	DB	7, 64
	DW	Err_S
	DW	StFil_S
	DW	Inc_S
	DW	OkEx_S
IniEr_B	BOXHD	< IniEr_P, 928h, 0, 1, 0FFh, 0FFh, 5>
	BOXMN	< 4, 0,0,0,0, 422h, TmpErr, OkEx_D>
	DB	0
OkEx_D	DB	4, 0
	DB	6, 5
	DB	0

	.CODE
	CMP	DS:IniStat,1
	JE	Err3
	MOV	DX,OFFSET NoFnd_S
	JB	Err1
	MOV	DX,OFFSET NoIni_S
Err1:	MOV	BP,OFFSET IniEr_B
	CALL	Dialog
	JZ	Err2
	JMP	Exit
Err2:	MOV	DI,OFFSET SrcIni
	MOV	AL,0FFh
	REP	STOSB
Err3:	JMP	Main00

;---------------------- Main Menu ----------------------
	.CONST
SetUp_S	DB	' Setup ',0
Main1_S	DB	'F2   Configuration',     0FFh,13,0
Main2_S	DB	'F3   Screen and Mouse',  0FFh,10,0
Main3_S	DB	'F4   Panel options',     0FFh,13,0
Main4_S	DB	'F5   Set Black & White palette ',0
Main5_S	DB	'F6   Set Color palette',  0FFh,9,0
Main6_S	DB	'F7   Set Laptop palette', 0FFh,8,0
Main7_S	DB	'F8   Set default configuration ',0
Main8_S	DB	'F9   Country info',      0FFh,14,0
Main9_S	DB	'F10  Exit',              0FFh,22,0

	.DATA
MainHlp	DB	1, 2, 3, 4, 4, 4, 5, 6, 7
Main_P	DB	Hist_C, 0, 0, 0
	DB	10, 0
	DW	WinBuf
	DW	SetUp_S
	DW	Main1_S
	DW	Main2_S
	DW	Main3_S
	DW	Main4_S
	DW	Main5_S
	DW	Main6_S
	DW	Main7_S
	DW	Main8_S
	DW	Main9_S
Main_T	DW	4800h, Main10
	DW	5000h, Main11
	DW	4700h, Main12
	DW	4F00h, Main13
	DW	4900h, Main12
	DW	5100h, Main13
	DW	000Dh, Main32
	DW	001Bh, Main31
	DW	4400h, Main31
	DW	3C00h, Main20
	DW	3D00h, Main21
	DW	3E00h, Main22
	DW	3F00h, Main23
	DW	4000h, Main24
	DW	4100h, Main25
	DW	4200h, Main26
	DW	4300h, Main27
	DW	0
Menu_T	DW	1, F02_00
	DW	2, F03_00
	DW	3, F04_00
	DW	4, F05_00
	DW	5, F06_00
	DW	6, F07_00
	DW	7, F08_00
	DW	8, F09_00
	DW	0

	.CODE
Main00:	MOV	AL,0
	CALL	PutKey
	MOV	BX,528h
	MOV	BP,OFFSET Main_P
	CALL	DialBox
	CALL	Window
	PUSH	SI
	MOV	CH,DS:MnItem
	MOV	DS:MousTim.Hi,0FFFFh
	MOV	DS:MousEnt,0
Main01:	MOV	AH,HstCu_C		; Set cursor
	CALL	Main40
	MOV	AL,CH
	CBW
	MOV	SI,AX
	MOV	AL,DS:MainHlp[SI]
	MOV	DS:HlpPage,AL
Main02:	PUSH	CX
	CALL	GetMous
	POP	CX
	JNZ	Main30
	MOV	DS:MousEnt,0
	MOV	AL,0			; Input
	CALL	Input
	CMP	AH,0FCh
	JE	Main30
	MOV	SI,OFFSET Main_T
	CALL	Case
	JMP	Main02
Main10:	MOV	AL,CH			; Up
	CMP	AL,0
	JE	Main13
	DEC	AX
	JMP	Main14
Main11:	MOV	AL,DH			; Down
	SUB	AL,6
	CMP	CH,AL
	JAE	Main12
	MOV	AL,CH
	INC	AX
	JMP	Main14
Main12:	MOV	AL,0			; Home, PgUp
	JMP	Main14
Main13:	MOV	AL,DH			; End, PgDn
	SUB	AL,6
Main14:	CMP	AL,CH
	JE	Main02
	MOV	AH,Hist_C
	CALL	Main40
	MOV	CH,AL
	JMP	Main01
Main20:	MOV	CH,0			; Configuration
	JMP	Main32
Main21:	MOV	CH,1			; Screen and Mouse
	JMP	Main32
Main22:	MOV	CH,2			; Panel options
	JMP	Main32
Main23:	MOV	CH,3			; Set Black & White palette
	JMP	Main32
Main24:	MOV	CH,4			; Set Color palette
	JMP	Main32
Main25:	MOV	CH,5			; Set Laptop palette
	JMP	Main32
Main26:	MOV	CH,6			; Set default configuration
	JMP	Main32
Main27:	MOV	CH,7			; Country info
	JMP	Main32
Main30:	CALL	Main50			; Mouse
	CMP	AH,1
	JB	Main02
	JE	Main14
	CMP	AH,1Ch
	JE	Main32
Main31:	MOV	CH,40h			; Exit
Main32:	POP	SI			; Enter
	CALL	ResWin
	CALL	Window
	MOV	AL,CH
	MOV	DS:MnItem,AL
	MOV	SI,OFFSET Menu_T
	MOV	AH,1
	INC	AX
	CALL	Case
	JMP	Exit

Main40	PROC	NEAR
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	ADD	BX,209h
	ADD	BH,CH
	MOV	DH,1
	SUB	DL,16
	MOV	CL,DL
	MOV	CH,0
	CALL	BasAdr
	CALL	Color
	MOV	AL,AH
	CALL	MvsColr
	CALL	Window
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	RET
Main40	ENDP

Main50	PROC	NEAR			; Mouse
	PUSH	CX
	XOR	SI,SI
Main51:	CALL	GetMous
	JZ	Main54
	MOV	BP,CX
	CALL	ClrKbd
	MOV	AH,'x'
	TEST	AL,3
	JZ	Main52
	MOV	AH,0
Main52:	MOV	CX,SI
	MOV	SI,AX
	CMP	AH,CH
	JE	Main53
	MOV	AL,AH
	CALL	TypMous
Main53:	TEST	SI,3
	JZ	Main51
	PUSH	BX
	PUSH	DX
	ADD	BX,204h
	SUB	DX,50Ah
	MOV	CX,BP
	CALL	InWind
	POP	DX
	POP	BX
	JC	Main51
	JMP	Main57
Main54:	MOV	AL,0
	CALL	TypMous
	POP	CX
	MOV	AX,SI
	CMP	AL,2
	JE	Main56
	CMP	AL,1
	JB	Main56
	JNE	Main55
	PUSH	DX
	PUSH	CX
	MOV	CX,BP
	SUB	DX,102h
	CALL	InWind
	POP	CX
	POP	DX
	JNC	Main56
Main55:	MOV	AH,0FFh
	RET
Main56:	MOV	AH,0
	RET
Main57:	SUB	CX,BX
	MOV	AL,CH
	SUB	AL,2
	CBW
	MOV	SI,AX
	PUSH	DX
	PUSH	BX
	PUSH	AX
	ADD	BX,204h
	ADD	BH,AL
	SUB	DL,10
	MOV	DH,1
	CALL	DoublMo
	POP	AX
	POP	BX
	POP	DX
	POP	CX
	JNC	Main58
	MOV	AH,1
	RET
Main58:	JE	Main56
	MOV	AL,0
	CALL	TypMous
	MOV	AH,1Ch
	RET
Main50	ENDP

;-------------------- Configuration --------------------
	.CONST
Cnf00_S	DB			' Configuration ',0
Cnf01_S	DB	' ⁄ ^bGeneral options^b ',0FFh,80h+10,'ƒø  ⁄ ^bExecute options^b '  ,0FFh,80h+15,'ƒø ',0
Cnf02_S	DB	' ≥  [ ] Auto menus'          ,0FFh,11,'≥  ≥  [ ] Memory allocation'     ,0FFh, 9,'≥ ',0
Cnf03_S	DB	' ≥  [ ] Path prompt'         ,0FFh,10,'≥  ≥  [ ] Quick execute commands',0FFh, 4,'≥ ',0
Cnf04_S	DB	' ≥  [ ] Key bar'             ,0FFh,14,'≥  ≥  [ ] TSR manager'           ,0FFh,15,'≥ ',0
Cnf05_S	DB	' ≥  [ ] Menu bar visible'    ,0FFh, 5,'≥  ¿'                       ,0FFh,80h+32,'ƒŸ ',0
Cnf06_S	DB	' ≥  [ ] Full screen'         ,0FFh,10,'≥  ⁄ ^bKeyboard options^b ' ,0FFh,80h+14,'ƒø ',0
Cnf07_S	DB	' ≥  [ ] Clock'               ,0FFh,16,'≥  ≥  [ ] Alt alone selects menu',0FFh, 4,'≥ ',0
Cnf08_S	DB	' ≥  [ ] Confirmation on quit '       ,'≥  ≥  [ ] Esc toggles screen on/off '    ,'≥ ',0
Cnf09_S	DB	' ≥  [ ] Error sound'         ,0FFh,10,'≥  ≥  [ ] Alternative viewer'    ,0FFh, 8,'≥ ',0
Cnf10_S	DB	' ≥  [ ] Auto save setup'     ,0FFh, 6,'≥  ≥  [ ] Alternative editor'    ,0FFh, 8,'≥ ',0
Cnf11_S	DB	' ≥  [ ] Show logical drive B:≥  ≥  [ ] Clear keyboard buffer' ,0FFh, 5,'≥ ',0
Cnf12_S	DB	' ¿'                     ,0FFh,80h+27,'ƒŸ  ¿'                       ,0FFh,80h+32,'ƒŸ ',0
Cnf13_S	DB	'Press ^r Space ^r to change an option, ^r  ^r and ^r  ^r',0
Cnf14_S	DB			'to move between options',0
OkCn_S  DB	'[ Ok ]',0FFh,3,'[ Cancel ]',0

	.DATA
OkCn_D	DB	 6, 0
	DB	10, 9
	DB	 0
Conf_P	DB	Conf_C, CnfRe_C, CnfBo_C, 0
	DB	17, 0
	DW	WinBuf
	DW	Cnf00_S
	DW	Cnf01_S
	DW	Cnf02_S
	DW	Cnf03_S
	DW	Cnf04_S
	DW	Cnf05_S
	DW	Cnf06_S
	DW	Cnf07_S
	DW	Cnf08_S
	DW	Cnf09_S
	DW	Cnf10_S
	DW	Cnf11_S
	DW	Cnf12_S
	DW	Cnf13_S
	DW	Cnf14_S
	DW	0
	DW	OkCn_S
Conf_B	BOXHD	< Conf_P, 228h, 0, 1, 4, 1, 0 >
	BOXMN	<3,18, 1, 0,10,  30Ah, TmpBuf+ 0, AutoMnu> ; Auto menus
	BOXMN	<3, 0, 2, 1,11,  40Ah, TmpBuf+ 1, Prompt > ; Path prompt
	BOXMN	<3, 1, 3, 2,12,  50Ah, TmpBuf+ 2, KeyBar > ; Key bar
	BOXMN	<3, 2, 4, 3,12,  60Ah, TmpBuf+ 3, MenuVs > ; Menu bar
	BOXMN	<3, 3, 5, 4,12,  70Ah, TmpBuf+ 4, FulScr > ; Full screen
	BOXMN	<3, 4, 6, 5,13,  80Ah, TmpBuf+ 5, Clock  > ; Clock
	BOXMN	<3, 5, 7, 6,14,  90Ah, TmpBuf+ 6, QuitAck> ; Confirm on quit
	BOXMN	<3, 6, 8, 7,15, 0A0Ah, TmpBuf+ 7, ErrSnd > ; Error sound
	BOXMN	<3, 7, 9, 8,16, 0B0Ah, TmpBuf+ 8, AutoSav> ; Auto save setup
	BOXMN	<3, 8,10, 9,17, 0C0Ah, TmpBuf+ 9, DriveB>  ; Show logical drive B:
	BOXMN	<3, 9,11, 0,10,  329h, TmpBuf+10, LoadFl > ; Memory allocation
	BOXMN	<3,10,12, 1,11,  429h, TmpBuf+11, ExecTyp> ; Quick execute
	BOXMN	<3,11,13, 2,12,  529h, TmpBuf+12, ChkTSR > ; TSR manager
	BOXMN	<3,12,14, 5,13,  829h, TmpBuf+13, AltBar > ; Alt selects menu
	BOXMN	<3,13,15, 6,14,  929h, TmpBuf+14, EscBar > ; Esc toggles on/off
	BOXMN	<3,14,16, 7,15, 0A29h, TmpBuf+15, AltView> ; Alternative viewer
	BOXMN	<3,15,17, 8,16, 0B29h, TmpBuf+16, AltEdit> ; Alternative editor
	BOXMN	<3,16,18, 9,17, 0C29h, TmpBuf+17, ClrBuf > ; Clear kbd buffer
	BOXMN	<4,17, 0,17,18, 111Eh, TmpBuf+18, OkCn_D > ; Ok / Cancel
	DB	0

	.CODE
F02_00:	MOV	BL,DS:KeyBar
	MOV	BP,OFFSET Conf_B
	CALL	Dialog
	CMP	DS:KeyBar,0
	JNZ	F02_01
	OR	BL,BL
	JZ	F02_01
	MOV	BH,DS:Lines
	DEC	BH
	MOV	BL,0
	MOV	AL,2*80
	MUL	BH
	MOV	SI,OFFSET UserScr
	MOV	DI,OFFSET ScrBuf
	ADD	SI,AX
	ADD	DI,AX
	MOV	CX,80
	REP	MOVSW
	MOV	DX,150h
	CALL	Window
	MOV	DS:BarPrev,0FFh
F02_01:	JMP	Main00

;------------------ Screen and Mouse -------------------
	.CONST
Scr00_S	DB			' Screen and Mouse ',0
Scr01_S	DB	' ⁄ ^bScreen colors^b ',0FFh,80h+6,'ƒø  ⁄ ^bScreen output^b '  ,0FFh,80h+14,'ƒø ',0
Scr02_S	DB	' ≥  ( ) Black & White  '          ,'≥  ≥  ( ) Synchronized access' ,0FFh, 4,'≥ ',0
Scr03_S	DB	' ≥  ( ) Color'            ,0FFh,10,'≥  ≥  ( ) Asynchronized access',0FFh, 3,'≥ ',0
Scr04_S	DB	' ≥  ( ) Laptop'            ,0FFh,9,'≥  ≥  ( ) Auto detect'         ,0FFh,12,'≥ ',0
Scr05_S	DB	' ¿'                  ,0FFh,80h+21,'ƒŸ  ¿'                     ,0FFh,80h+29,'ƒŸ ',0
Scr06_S	DB	' ⁄ ^bScreen blank delay^b ','ƒø  ⁄ ^bOther options^b ',0FFh,80h+14,'ƒø ',0
Scr07_S	DB	' ≥  ( ) 40 minutes' ,0FFh, 5,'≥  ≥  [ ] Zooming boxes'     ,0FFh,10,'≥ ',0
Scr08_S	DB	' ≥  ( ) 20 minutes' ,0FFh, 5,'≥  ≥  Screen saver chars:'   ,0FFh, 8,'≥ ',0
Scr09_S	DB	' ≥  ( )  5 minutes' ,0FFh, 5,'≥  ¿'                   ,0FFh,80h+29,'ƒŸ ',0
Scr10_S	DB	' ≥  ( )  3 minutes' ,0FFh, 5,'≥  ⁄ ^bMouse options^b ',0FFh,80h+14,'ƒø ',0
Scr11_S	DB	' ≥  ( )  1 minute'  ,0FFh, 6,'≥  ≥  [ ] Left-handed mouse' ,0FFh, 6,'≥ ',0
Scr12_S	DB	' ≥  ( ) Off'        ,0FFh,12,'≥  ¿'                   ,0FFh,80h+29,'ƒŸ ',0
Scr13_S	DB	' ¿'            ,0FFh,80h+21,'ƒŸ',0FFh,34,0
Scr14_S	DB	'Press ^r Space ^r to change an option, ^r  ^r and ^r  ^r',0
Scr15_S	DB			'to move between options',0

	.DATA
Scrn_P	DB	Conf_C, CnfRe_C, CnfBo_C, 0
	DB	18, 0
	DW	WinBuf
	DW	Scr00_S
	DW	Scr01_S
	DW	Scr02_S
	DW	Scr03_S
	DW	Scr04_S
	DW	Scr05_S
	DW	Scr06_S
	DW	Scr07_S
	DW	Scr08_S
	DW	Scr09_S
	DW	Scr10_S
	DW	Scr11_S
	DW	Scr12_S
	DW	Scr13_S
	DW	Scr14_S
	DW	Scr15_S
	DW	0
	DW	OkCn_S
Scrn_B	BOXHD	< Scrn_P, 228h, 0, 1, 4, 2, 0 >
	BOXMN	<2, 6,81h, 0,82h,  30Ah, TmpBuf+0, 3 >	   ; Screen colors
	BOXMN	<2,0C0h, 6, 1, 3,  80Ah, TmpBuf+2, 6 >	   ; Screen blank delay
	BOXMN	<2,0C1h,3,80h, 2,  323h, TmpBuf+4, 3 >	   ; Screen output
	BOXMN	<3,0C2h,4,81h, 3,  823h, TmpBuf+6, ZoomWin>; Zooming boxes
	BOXMN	<1, 3, 5, 81h, 4,  936h, StarTab , 5 >	   ; Menu bar visible
	BOXMN	<3, 4, 6, 81h, 5, 0C23h, TmpBuf+7, LftMous>; Left-handed mouse
	BOXMN	<4, 5, 80h, 6, 6, 121Eh, TmpBuf+9, OkCn_D> ; Ok / Cancel
	DB	0
Delay_T	DB	40, 20, 5, 3, 1, 0

	.CODE
F03_00:	PUSH	ES
	POP	DS
	MOV	DI,OFFSET TmpBuf
	MOV	AL,DS:ColTyp
	STOSB
	STOSB
	MOV	AH,DS:BlDelay
	MOV	SI,OFFSET Delay_T-1
	MOV	AL,0-1
F03_01:	INC	AL
	INC	SI
	CMP	AH,CS:[SI]
	JB	F03_01
	STOSB
	STOSB
	MOV	AL,DS:SnowMod
	STOSB
	STOSB
	MOV	BP,OFFSET Scrn_B
	CALL	Dialog
	JNE	F03_02
	MOV	SI,OFFSET TmpBuf
	LODSW
	MOV	DS:ColTyp,AL
	LODSW
	CBW
	MOV	BX,AX
	MOV	AL,CS:Delay_T.[BX]
	MOV	DS:BlDelay,AL
	LODSW
	MOV	DS:SnowMod,AL
F03_02:	CALL	SetSnow
	JMP	Main00

;-------------------- Panel options --------------------
	.CONST
Pan00_S	DB			' Panel options ',0
Pan01_S	DB	' ⁄ ^bFile panel options^b '   ,0FFh,80h+18,'ƒø ',0
Pan02_S	DB	' ≥  [ ] Mini status line'          ,0FFh,16,'≥ ',0
Pan03_S	DB	' ≥  [ ] Auto directory size'       ,0FFh,13,'≥ ',0
Pan04_S	DB	' ≥  [ ] Ins moves down'            ,0FFh,18,'≥ ',0
Pan05_S	DB	' ≥  [ ] Unselect during group operation '  ,'≥ ',0
Pan06_S	DB	' ¿'                           ,0FFh,80h+38,'ƒŸ ',0
Pan07_S	DB	' ⁄ ^bTree panel options^b '   ,0FFh,80h+18,'ƒø ',0
Pan08_S	DB	' ≥  [ ] Auto change directory'     ,0FFh,11,'≥ ',0
Pan09_S	DB	' ≥  [ ] Show hidden subdirectories',0FFh, 6,'≥ ',0
Pan10_S	DB	' ≥  [ ] Auto rescan tree'          ,0FFh,16,'≥ ',0
Pan11_S	DB	' ≥  Auto change dir time delay:'   ,0FFh, 9,'≥ ',0
Pan12_S	DB	' ¿'                           ,0FFh,80h+38,'ƒŸ ',0
Pan13_S	DB	'Press ^r Space ^r to change an option,',0
Pan14_S	DB	'arrows to move between options',0
NoTm1_S	DB	'Illegal format of',0
NoTm2_S	DB	'auto change dir time delay.',0
OkMn_S	DB	' Ok ',0

	.DATA
PanOp_P	DB	Conf_C, CnfRe_C, CnfBo_C, 0
	DB	17, 0
	DW	WinBuf
	DW	Pan00_S
	DW	Pan01_S
	DW	Pan02_S
	DW	Pan03_S
	DW	Pan04_S
	DW	Pan05_S
	DW	Pan06_S
	DW	Pan07_S
	DW	Pan08_S
	DW	Pan09_S
	DW	Pan10_S
	DW	Pan11_S
	DW	Pan12_S
	DW	Pan13_S
	DW	Pan14_S
	DW	0
	DW	OkCn_S
PanOp_B	BOXHD	< PanOp_P, 228h, F04_10, 1, 4, 3, 0 >
	BOXMN	<3, 8, 1, 0, 0,  30Ah, TmpBuf+0, MiniSta> ; Mini status
	BOXMN	<3, 0, 2, 1, 1,  40Ah, TmpBuf+1, AutoSiz> ; Auto dir size
	BOXMN	<3, 1, 3, 2, 2,  50Ah, TmpBuf+2, InsDwn > ; Ins moves down
	BOXMN	<3, 2, 4, 3, 3,  60Ah, TmpBuf+3, UnSlct > ; Unselect
	BOXMN	<3, 3, 5, 4, 4,  90Ah, TmpBuf+4, AutoCD > ; Auto change dir
	BOXMN	<3, 4, 6, 5, 5, 0A0Ah, TmpBuf+5, HidDirs> ; Show hidden dirs
	BOXMN	<3, 5, 7, 6, 6, 0B0Ah, TmpBuf+6, ChgTree> ; Auto rescan tree
	BOXMN	<1, 6, 8, 7, 7, 0C25h, TmpBuf+9, 5 >	  ; Delay time
	BOXMN	<4, 7, 0, 8, 8, 111Eh, TmpBuf+7, OkCn_D > ; Ok / Cancel
	DB	0
NoTim_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 0
	DW	BoxBuf
	DW	Pan00_S
	DW	NoTm1_S
	DW	NoTm2_S
	DW	OkMn_S
NoTim_B	BOXHD	< NoTim_P, 0C28h, 0, 1, 0FFh, 0FFh, 0 >
	BOXMN	< 4, 0, 0, 0, 0, 426h, TmpErr+240, OkMn_D >
	DB	0
OkMn_D	DB	4, 0
	DB	0

	.CODE
F04_00:	MOV	DI,OFFSET TmpBuf+9
	MOV	AL,DS:DelayTm[1]
	CMP	AL,99
	JBE	F04_01
	MOV	AL,99
F04_01:	CALL	HexDec
	CMP	AL,'0'
	JE	F04_02
	STOSB
F04_02:	MOV	AL,AH
	MOV	AH,DS:Country.DecSep
	STOSW
	MOV	AL,DS:DelayTm[0]
	CMP	AL,99
	JBE	F04_03
	MOV	AL,99
F04_03:	CALL	HexDec
	STOSW
	MOV	AL,0
	STOSB
	MOV	BP,OFFSET PanOp_B
	CALL	Dialog
	JMP	Main00

F04_10	PROC	NEAR			; Test delay time
	CMP	AL,0
	JNE	F04_15
	MOV	SI,OFFSET TmpErr+3
	CALL	FndLet
	CMP	AL,0
	JE	F04_15
	XOR	CX,CX
	CMP	AL,DS:Country.DecSep
	JE	F04_11
	CALL	TxtNum
	JC	F04_18
	CMP	AX,99
	JA	F04_18
	MOV	CH,AL
	PUSH	SI
	CALL	FndLet
	POP	SI
	CMP	AL,0
	JE	F04_17
	MOV	AL,DS:[SI]
	CMP	AL,DS:Country.DecSep
	JNE	F04_18
	PUSH	SI
	INC	SI
	CALL	FndLet
	POP	SI
	CMP	AL,0
	JE	F04_17
F04_11:	INC	SI
	PUSH	SI
	MOV	CL,0FFh
F04_12:	INC	CL
	LODSB
	CMP	AL,'0'
	JE	F04_12
	POP	SI
	CALL	TxtNum
	JC	F04_18
	PUSH	AX
	CALL	FndLet
	CMP	AL,0
	POP	AX
	JNE	F04_18
	CMP	AX,10
	JAE	F04_13
	MOV	AH,10
	MUL	AH
F04_13:	CMP	CL,1
	MOV	BX,100
	JB	F04_14
	MOV	BX,10
	JE	F04_14
	MOV	BL,1
F04_14:	CMP	AX,BX
	JB	F04_16
	XOR	DX,DX
	MOV	SI,10
	DIV	SI
	JMP	F04_14
F04_15:	RET
F04_16:	MOV	CL,AL
F04_17:	MOV	WORD PTR DS:DelayTm,CX
	MOV	AL,0
	CLC
	RET
F04_18:	MOV	BP,OFFSET NoTim_B
	CALL	Dialog
	MOV	DS:HlpPage,3
	STC
	RET
F04_10	ENDP

;------------- Set Black & White palette ---------------
	.CONST
Color_S	DB	' Color Setup ',0
Mode_S	DB	'Mode: ',0
Palet_S	DB	'Palette: ',0
BwPal_S	DB	'Black&White',0
LapTp_S	DB	'LapTop',0
ColMd_S	DB	'Color',0
MonMd_S	DB	'Monochrome',0
DfCo1_S	DB	'Do you wish to set default colors',0
DfCo2_S	DB	'for current palette?',0
Colrs_S	DB	'Panels',0
	DB	'Current pointer',0
	DB	'Selected files',0
	DB	'Selected pointer',0
	DB	'Title in file panel',0
	DB	'Key bar numbers',0
	DB	'Key bar text',0
	DB	'Pull-down menu bar',0
	DB	'Pull-down menu pointer',0
	DB	'Pull-down menu border',0
	DB	'Pull-down menu text',0
	DB	'Pull-down menu bright',0
	DB	'Pull-down menu current bar',0
	DB	'Pull-down menu current bright',0
	DB	'Pull-down menu unaccessable lines',0
	DB	'Pull-down menu minus',0
	DB	'Dialog box text',0
	DB	'Dialog box bright text',0
	DB	'Dialog box reverse text',0
	DB	'Configuration boxes text',0
	DB	'Configuration boxes reverse text',0
	DB	'Configuration boxes bright text',0
	DB	'Configuration boxes bright reverse text',0
	DB	'History box text',0
	DB	'History box pointer',0
	DB	'Mini help in .EXT file editor border',0
	DB	'Mini help in .EXT file editor text',0
	DB	'Help box text',0
	DB	'Help box bright text',0
	DB	'Help box reverse text',0
	DB	'Help box underline text',0
	DB	'Error box text',0
	DB	'Error box reverse text',0
	DB	'Error box bright text',0
	DB	'Clock',0
	DB	'Arrows in viewer and editor',0
	DB	'Stars',0
	DB	'Explosing stars',0
	DB	'Shadow',0
	DB	0
On_S	DB	'on ^I',0
ColNm_S	DB	'Black',0
	DB	'Blue',0
	DB	'Green',0
	DB	'Cyan',0
	DB	'Red',0
	DB	'Magenta',0
	DB	'Brown',0
	DB	'White',0
	DB	'Gray',0
	DB	'Bright Blue',0
	DB	'Bright Green',0
	DB	'Bright Cyan',0
	DB	'Bright Red',0
	DB	'Pink',0
	DB	'Yellow',0
	DB	'Bright White',0
	DB	'No shadow',0
	DB	'Inv brightness',0

	.DATA
ColNm_P	DB	Back_C
DfCol_P	DB	Box_C, BoxCu_C, BoxCu_C, 0
	DB	4, 0
	DW	BoxBuf
	DW	Color_S
	DW	DfCo1_S
	DW	DfCo2_S
	DW	YesNo_S
DfCol_B	BOXHD	< DfCol_P, 728h, 0, 1, 0FFh, 0FFh, 4>
	BOXMN	< 4, 0,0,0,0, 423h, TmpErr, YesNo_D>
	DB	0
Color_T	DW	4800h, F07_40
	DW	5000h, F07_41
	DW	4700h, F07_42
	DW	4F00h, F07_43
	DW	4900h, F07_44
	DW	5100h, F07_48
	DW	4200h, F07_53
	DW	001Bh, F07_57
	DW	4400h, F07_57
	DW	000Dh, Col00
	DW	0

	.CODE
F05_00:	MOV	SI,OFFSET BwPal_S
	XOR	BP,BP
	JMP	F07_01

;------------------ Set Color palette ------------------
F06_00:	MOV	SI,OFFSET ColMd_S
	MOV	BP,1
	JMP	F07_01

;----------------- Set Laptop palette ------------------
F07_00:	MOV	SI,OFFSET LapTp_S
	MOV	BP,2
F07_01:	MOV	DS:HlpPage,4
	PUSH	SI			; Color table
	MOV	SI,OFFSET ColMd_S
	CMP	DS:ScrMod,7
	JNE	F07_02
	MOV	SI,OFFSET MonMd_S
F07_02:	PUSH	SI
	MOV	BX,58
	MOV	DH,DS:Lines
	CMP	DS:KeyBar,0
	JZ	F07_03
	DEC	DH
F07_03:	MOV	DL,22
	CALL	BasAdr
	PUSH	DI
	MOV	CL,DH
	MOV	CH,0
	MOV	AH,Back_C
	CALL	Color
	MOV	DS:BackC,AH
	MOV	AL,' '
F07_04:	PUSH	DI
	PUSH	CX
	MOV	CL,22
	REP	STOSW
	POP	CX
	POP	DI
	ADD	DI,2*80
	LOOP	F07_04
	POP	DI
	POP	CX
	PUSH	DI
	ADD	DI,2*(80+4)
	MOV	SI,OFFSET Mode_S
	CALL	MovDz0
	MOV	SI,CX
	CALL	MovDz0
	POP	DI
	POP	CX
	PUSH	DI
	ADD	DI,2*(160+1)
	MOV	SI,OFFSET Palet_S
	CALL	MovDz0
	MOV	SI,CX
	CALL	MovDz0
	POP	DI
	ADD	DI,2*(320+3)
	MOV	CX,16
	MOV	AX,'x'
F07_05:	PUSH	DI
	PUSH	CX
	MOV	CX,16
F07_06:	STOSW
	INC	AH
	LOOP	F07_06
	POP	CX
	POP	DI
	ADD	DI,2*80
	LOOP	F07_05
	CALL	Window

	MOV	BX,203h			; Output window
	MOV	DH,DS:Lines
	SUB	DH,3
	MOV	DL,53
	MOV	AH,HstCu_C
	CALL	Color
	MOV	DS:HstCuC,AH
	MOV	AH,Hist_C
	CALL	Color
	MOV	DS:HistC,AH
	CALL	MinBox
	CALL	BasAdr
	ADD	DI,2*(80+19)
	MOV	SI,OFFSET Color_S
	MOV	CX,13
	CALL	MvsBwc
	CALL	Window
	XOR	AX,AX
	XOR	SI,SI
	MOV	DI,OFFSET Colrs_S
F07_07:	INC	SI
	MOV	CX,0FFFFh
	REPNE	SCASB
	CMP	AL,DS:[DI]
	JNE	F07_07
	MOV	DS:ColNum,SI
	MOV	DS:ColCur,AX
	MOV	DS:ColFrst,0FFF0h
	MOV	DS:WinMous,AL
	MOV	DS:MousEnt,AL
	MOV	DS:MousTim.Hi,0FFFFh
	XOR	SI,SI
	XOR	DI,DI
	CALL	ChgCur
	PUSH	CX

F07_10:	CMP	SI,DI			; Test parameters
	JAE	F07_11
	MOV	DI,SI
F07_11:	MOV	AL,DH
	SUB	AL,5
	CBW
	ADD	AX,DI
	CMP	SI,AX
	JB	F07_12
	SUB	AX,DI
	MOV	DI,SI
	SUB	DI,AX
	INC	DI
F07_12:	MOV	AX,DI
	XCHG	SI,DS:ColCur
	XCHG	DI,DS:ColFrst
	SUB	AX,DI
	JNE	F07_13
	CMP	SI,DS:ColCur
	JNE	F07_13
	JMP	F07_20
F07_13:	PUSH	AX			; Reset cursor
	CMP	DI,0FFF0h
	JE	F07_14
	PUSH	DX
	PUSH	BX
	MOV	AX,SI
	SUB	AX,DI
	ADD	BH,AL
	MOV	CX,SI
	CALL	F07_65
	ADD	BX,205h
	CALL	F07_70
	SUB	DL,12
	MOV	DH,1
	CALL	Window
	POP	BX
	POP	DX
F07_14:	POP	CX
	JCXZ	F07_18
	PUSH	CX			; Output lines
	PUSH	BX
	MOV	CX,DS:ColFrst
	CALL	F07_65
	MOV	CL,DH
	SUB	CL,5
	MOV	CH,0
	ADD	BX,205h
F07_15:	CALL	F07_70
	INC	BH
	CMP	BYTE PTR DS:[SI],0
	LOOPNE	F07_15
	POP	BX
	POP	CX
	PUSH	DX			; Output screen
	PUSH	BX
	ADD	BX,205h
	SUB	DX,50Ch
	MOV	AL,DH
	DEC	AX
	MOV	AH,6
	CMP	CX,1
	JE	F07_16
	CMP	CX,-1
	JNE	F07_17
	MOV	AX,700h
F07_16:	MOV	CX,BX
	DEC	DL
	DEC	DH
	ADD	DX,CX
	PUSH	AX
	MOV	AH,DS:HistC
	MOV	BH,AH
	POP	AX
	PUSH	AX
	MOV	AL,1
	CALL	Int10m
	POP	AX
	MOV	BX,CX
	ADD	BH,AL
	INC	DX
	MOV	DH,1
F07_17:	CALL	Window
	POP	BX
	POP	DX
F07_18:	PUSH	DX			; Set cursor
	PUSH	BX
	MOV	AX,DS:ColCur
	SUB	AX,DS:ColFrst
	ADD	BH,AL
	ADD	BX,205h
	CALL	BasAdr
	MOV	AH,DS:HstCuC
	MOV	AL,AH
	SUB	DL,12
	MOV	DH,1
	MOV	CL,DL
	MOV	CH,0
	CALL	MvsColr
	CALL	Window
	POP	BX
	POP	DX
	MOV	AX,DS:ColCur		; Cursor on color table
	MOV	AH,6
	MUL	AH
	LEA	SI,ColTab[BP]
	ADD	SI,BP
	ADD	SI,AX
	CMP	DS:ScrMod,7
	JNE	F07_19
	INC	SI
F07_19:	MOV	AL,DS:[SI]
	CALL	F07_75

F07_20:	CALL	GetMous			; Input
	JNZ	F07_30
	MOV	DS:WinMous,0
	MOV	DS:MousEnt,0
	MOV	AL,2
	CALL	Input
	CMP	AH,0FCh
	JE	F07_30
	MOV	SI,OFFSET Color_T
	CALL	Case
	JMP	F07_20

F07_30:	CALL	GetMous			; Mouse support
F07_31:	CALL	ClrKbd
	MOV	SI,CX
	TEST	AL,4
	JZ	F07_33
	MOV	AL,'x'
	CALL	TypMous
F07_32:	CALL	GetMous
	CALL	ClrKbd
	TEST	AL,4
	JNZ	F07_32
	PUSH	AX
	MOV	AL,0
	CALL	TypMous
	POP	AX
	TEST	AL,3
	JZ	F07_36
F07_33:	PUSH	DX
	PUSH	BX
	ADD	BX,204h
	SUB	DX,50Ah
	CALL	InWind
	POP	BX
	POP	DX
	JNC	F07_37
	CMP	DS:WinMous,0
	JZ	F07_35
	MOV	AL,BH
	ADD	AL,2
	CMP	CH,AL
	JB	F07_40
	MOV	AL,BH
	ADD	AL,DH
	SUB	AL,3
	CMP	CH,AL
	JAE	F07_41
F07_34:	JMP	F07_20
F07_35:	TEST	AL,1
	JZ	F07_34
	CALL	GetMous
	JNZ	F07_31
	MOV	CX,SI
	PUSH	DX
	SUB	DX,102h
	CALL	InWind
	POP	DX
	JNC	F07_34
F07_36:	JMP	F07_57
F07_37:	OR	DS:WinMous,1
	PUSH	CX
	SUB	CH,BH
	SUB	CH,2
	MOV	CL,CH
	MOV	CH,0
	MOV	DI,DS:ColFrst
	ADD	CX,DI
	MOV	AX,DS:ColNum
	DEC	AX
	POP	SI
	XCHG	AX,CX
	CMP	AX,CX
	JBE	F07_39
	MOV	SI,CX
F07_38:	JMP	F07_10
F07_39:	PUSH	DX
	PUSH	BX
	XCHG	AX,SI
	MOV	BH,AH
	ADD	BL,4
	SUB	DL,10
	MOV	DH,1
	CALL	DoublMo
	POP	BX
	POP	DX
	JC	F07_38
	JE	F07_34
	MOV	DS:MousEnt,0
	MOV	DS:MousTim.Hi,0FFFFh
	MOV	AL,0
	CALL	TypMous
	JMP	Col00

F07_40:	CALL	F07_60			; Up
	OR	SI,SI
	JE	F07_47
	DEC	SI
	JMP	F07_10

F07_41:	CALL	F07_60			; Down
	INC	SI
	CMP	SI,DS:ColNum
	JB	F07_47
	DEC	SI
	JMP	F07_10

F07_42:	XOR	SI,SI			; Home
	JMP	F07_10

F07_43:	XOR	DI,DI			; End
	MOV	SI,DS:ColNum
	DEC	SI
	JMP	F07_10

F07_44:	CALL	F07_60			; PgUp
	OR	DI,DI
	JNE	F07_45
	XOR	SI,SI
	JMP	F07_47
F07_45:	DEC	AX
	SUB	DI,AX
	JAE	F07_46
	XOR	DI,DI
F07_46:	ADD	AX,DI
	CMP	SI,AX
	JBE	F07_47
	MOV	SI,AX
F07_47:	JMP	F07_10

F07_48:	CALL	F07_60			; PgDn
	MOV	CX,DS:ColNum
	PUSH	SI
	MOV	SI,CX
	SUB	CX,AX
	JAE	F07_49
	XOR	CX,CX
F07_49:	DEC	AX
	CMP	DI,CX
	JB	F07_50
	POP	AX
	OR	SI,SI
	JE	F07_47
	DEC	SI
	JMP	F07_47
F07_50:	POP	SI
	ADD	DI,AX
	JC	F07_51
	CMP	DI,CX
	JBE	F07_52
F07_51:	MOV	DI,CX
F07_52:	CMP	SI,DI
	JAE	F07_47
	MOV	SI,DI
	JMP	F07_10

F07_53:	CALL	F07_60			; Default colors
	PUSH	BP
	MOV	BP,OFFSET DfCol_B
	CALL	Dialog
	POP	BP
	MOV	DS:HlpPage,4
	JNZ	F07_47
	MOV	DI,BP
	ADD	DI,DI
	CMP	DS:ScrMod,7
	JNE	F07_54
	INC	DI
F07_54:	LEA	SI,Palets[DI]
	LEA	DI,ColTab[DI]
	MOV	CX,LenColr
F07_55:	MOV	AL,DS:[SI]
	MOV	DS:[DI],AL
	ADD	SI,6
	ADD	DI,6
	LOOP	F07_55
	CALL	F07_60
	JMP	F07_10

F07_57:	CALL	HidCur			; Exit
	POP	CX
	MOV	AH,1
	INT	10h
	MOV	DL,80
	MOV	DH,DS:Lines
	MOV	AL,DL
	MUL	DH
	MOV	CX,AX
	MOV	SI,OFFSET UserScr
	MOV	DI,OFFSET ScrBuf
	REP	MOVSW
	XOR	BX,BX
	CALL	Window
	JMP	Main00

F07_60	PROC	NEAR			; Get parameters
	MOV	SI,DS:ColCur
	MOV	DI,DS:ColFrst
	MOV	AL,DH
	SUB	AL,5
	CBW
	RET
F07_60	ENDP

F07_65	PROC	NEAR			; Find line #CX
	MOV	DI,OFFSET Colrs_S
F07_66:	MOV	AL,0
	JCXZ	F07_68
F07_67:	PUSH	CX
	MOV	CX,0FFFFh
	REPNE	SCASB
	POP	CX
	LOOP	F07_67
F07_68:	MOV	SI,DI
	RET
F07_65	ENDP

F07_70	PROC	NEAR			; Put line from SI
	PUSH	CX
	PUSH	AX
	CALL	BasAdr
	MOV	CL,DL
	SUB	CL,12
	MOV	CH,0
	MOV	AH,DS:HistC
	JMP	F07_72
F07_71:	LODSB
	CMP	AL,0
	JNE	F07_73
	DEC	SI
F07_72:	MOV	AL,' '
F07_73:	STOSW
	LOOP	F07_71
	INC	SI
	POP	AX
	POP	CX
	RET
F07_70	ENDP

F07_75	PROC	NEAR			; Current color name
	PUSH	BP
	PUSH	DX
	PUSH	CX
	PUSH	BX
	MOV	AH,AL
	MOV	CL,4
	SHR	AH,CL
	AND	AX,0F0Fh
	PUSH	AX
	ADD	AX,43Dh
	CALL	Cursor
	POP	AX
	CMP	BYTE PTR DS:ColCur,ShadFl
	JNE	F07_76
	CMP	AX,1
	MOV	AX,0FF10h
	JB	F07_76
	MOV	AX,11h
	JE	F07_76
	MOV	AX,7
F07_76:	PUSH	AX
	CALL	F07_80
	MOV	BP,OFFSET ColNm_P
	MOV	BX,153Ah
	CALL	BasAdr
	MOV	AL,' '
	MOV	AH,CS:[BP]
	CALL	Color
	MOV	CX,22
	REP	STOSW
	POP	CX
	PUSH	CX
	MOV	CH,0
	MOV	DI,OFFSET ColNm_S
	PUSH	DI
	PUSH	AX
	CALL	F07_66
	PUSH	BX
	ADD	BL,11
	CALL	CntLine
	POP	BX
	MOV	DX,116h
	CALL	Window
	INC	BH
	CALL	BasAdr
	POP	AX
	MOV	CX,22
	REP	STOSW
	POP	DI
	POP	CX
	CMP	CH,0FFh
	JE	F07_77
	MOV	CL,CH
	MOV	CH,0
	CALL	F07_66
	PUSH	DX
	PUSH	BX
	MOV	DX,SI
	MOV	SI,OFFSET On_S
	ADD	BL,11
	CALL	CntLine
	POP	BX
	POP	DX
F07_77:	CALL	Window
	CALL	F07_80
	POP	BX
	POP	CX
	POP	DX
	POP	BP
	RET
F07_75	ENDP

F07_80	PROC	NEAR			; Change color Back_C
	MOV	SI,OFFSET BackC
	MOV	AH,Back_C
F07_80	ENDP

F07_85	PROC	NEAR			; Change color
	PUSH	DI
	CALL	ColAddr
	XCHG	AL,DS:[DI]
	XCHG	AL,DS:[SI]
	XCHG	AL,DS:[DI]
	POP	DI
	RET
F07_85	ENDP

;---------------------- Set color ----------------------
	.CONST
Page1_S	DB	'…ÕÕÕÕÕÕÕÕÕÕÕÕ—ÕÕ C:\DOS ÕÕÕÕÕÕÕÕ—Õ 4:55p                  '
	DB	'∫    Name    ≥   Size  ≥  Date  ≥ Time ∫                  '
	DB	'∫..          ≥UP--DIR≥10-29-91≥ 2:55p∫                  '
	DB	'∫command  com≥    54619…ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ Tree ÕÕÕÕÕÕÕÕÕÕÕ '
	DB	'∫append   exe≥     8170∫ \                                '
	DB	'∫as                    ∫ √ƒƒDOS                           '
	DB	'∫at   …ÕÕÕÕÕÕ Drive let∫ √ƒ›VC          ﬁ                 '
	DB	'∫ba   ∫    Choose left ∫ √ƒƒTOOLS                         '
	DB	'∫ch   ∫  A   B   C   D ∫ √ƒ›ARC         ﬁ                 '
	DB	'∫ch   »ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ∫ √ƒƒTEST                          '
	DB	'∫co                    ∫                                  '
	DB	'∫debug    exe≥    15718∫   …ÕÕÕÕÕÕÕ User Menu ÕÕÕÕÕÕÕª    '
	DB	'∫deltree  exe≥     7930∫   ∫ F1  Archives...         ∫    '
	DB	'∫diskcomp com≥     7249∫   ∫ F2  Format floppy disk  ∫    '
	DB	'∫diskcopy com≥     8763∫   ∫ F4  Edit file           ∫    '
	DB	'∫doskey   com≥     4729∫   »ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕº    '
	DB	'∫edlin    exe≥    10007∫                                  '
	DB	'∫exe2bin  exe≥     6755∫ √ƒƒDOCS                          '
	DB	'∫expand   exe≥    16129∫ ¿ƒ TMP                           '
	DB	'∫fasthelp exe≥     8199∫                                  '
	DB	'«ƒƒƒƒƒƒƒƒƒƒƒƒ¡ƒƒƒƒƒƒƒƒƒ∫                                  '
	DB	'∫   32,891 bytes in 3 s∫                                  '
	DB	'»ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ∫                                  '
	DB	'C:\DOS>                «ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ '
	DB	'1Help   2Menu   3View  ∫C:\DOS                            '
Page2_S	DB	'    Left    Files    Commands    Options    Right         '
	DB	'  ⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø                               '
	DB	'  ≥  Brief                ≥                ˙         ˙    '
	DB	'  ≥  Full                 ≥                               '
	DB	'  ≥  Info                 ≥       ˙                       '
	DB	'  ≥  Tree                 ≥                               '
	DB	'  ≥˚ On/Off       Ctrl-F1 ≥                      ˙       '
	DB	'  ≥ ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ ≥                               '
	DB	'  ≥  Name                 ≥                               '
	DB	'  ≥  eXtension            ≥                               '
	DB	'  ≥  tiMe                 ≥                               '
	DB	'  ≥  Size                 ≥            ˙                  '
	DB	'  ≥  Unsorted             ≥                        ˙      '
	DB	'  ≥ ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ ≥                               '
	DB	'  ≥  Re-read              ≥                               '
	DB	'  ≥ -fiLter...            ≥                              '
	DB	'  ≥  Drive...     Alt-F1  ≥                               '
	DB	'  ¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ          ˙                    '
	DB	'                                                   ˙      '
	DB	'                                             ˘            '
	DB	'                          ˙                               '
	DB	'       ˙                                                 '
	DB	'                                                          '
	DB	'                    ˙                    ˙                '
	DB	'                                                          '
Page3_S	DB	'Edit: C:\VC\                                              '
	DB	'asm:    tas   …ÕÕÕÕÕÕÕÕÕÕÕ Choose Directory ÕÕÕÕÕÕÕÕÕÕÕª '
	DB	'obj:    lin   ∫  √ƒƒTOOLS                              ∫ '
	DB	'sys:    dl    ∫  √ƒ ARC                               ∫ '
	DB	'zip:    pkun   ∫  √ƒƒTEST                               ∫ '
	DB	'arc:                                                      '
	DB	'ice:     …ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ Error ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕª    '
	DB	"lzh:     ∫      Can't read the disk in drive A:      ∫    "
	DB	'arj:     ∫  Press ENTER to try again, ESC to abort,  ∫    '
	DB	'         ∫ or enter a different drive letter here A: ∫    '
	DB	'         »ÕÕÕÕÕÕÕ                                         '
	DB	'                    …ÕÕÕÕÕÕÕÕÕÕÕ Delete ÕÕÕÕÕÕÕÕÕÕÕª      '
	DB	'               ∫    ∫       You are DELETING       ∫    ∫ '
	DB	'               ∫    ∫ 5 files and 1 directory from ∫    ∫ '
	DB	'‹‹‹‹‹‹‹‹‹‹‹‹   ∫    ∫            C:\TMP            ∫    ∫ '
	DB	' …ÕÕÕÕÕÕÕÕÕÕ   ∫    ∫     Delete   All   Cancel    ∫    ∫ '
	DB	' ∫ Format of   ∫    »ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕº    ∫ '
	DB	' ∫             «ƒ                                      ƒ∂ '
	DB	" ∫ ' comment   ∫ C:\ARC                                 ∫ "
	DB	' ∫ txt: edit   ∫ Speed search: ARC                      ∫ '
	DB	' ∫     cls    »ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ⁄ Screen colors ƒƒƒƒƒƒø '
	DB	' ∫  ¿ƒƒƒƒƒƒƒ                      ≥  ( ) Black & White  ≥ '
	DB	' »ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ≥  () Color          ≥ '
	DB	'ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ≥  ( ) Laptop         ≥ '
	DB	'1Help   2Save   3       4Hex    5 ¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ '
Page4_S	DB	'                                                          '
	DB	'                                                          '
	DB	'   …ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ Help ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ '
	DB	'   ∫ Configuration...                                     '
	DB	'   «ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ   …ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ '
	DB	'   ∫  This dialog box  allows yo   ∫               The Vo '
	DB	'   ∫  options.  Use the cursor k   «ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ '
	DB	'   ∫  to change, and use the Spa   ∫  About the Commander '
	DB	'   ∫  Enter to accept the dialog   ∫  Keyboard reference  '
	DB	'   ∫                               ∫  View -- Keyboard re '
	DB	'   ∫  ⁄ Screen colors ƒƒƒƒƒƒø  T   ∫  View -- Status line '
	DB	'   ∫  ≥  ( ) Black & White  ≥  w   ∫  Edit -- Keyboard re '
	DB	'   ∫  ≥  () Color          ≥  t   ∫  Edit -- Status line '
	DB	'   ∫  ≥  ( ) Laptop         ≥  l   ∫  Left/Right menu     '
	DB	'   ∫  ¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ  c   ∫       Brief/Full     '
	DB	'   ∫                               ∫       Info           '
	DB	'   ∫  ⁄ Screen blank delay ƒø  T   ∫       Tree           '
	DB	'   ∫  ≥  ( ) 40 minutes     ≥  b   ∫       On/Off         '
	DB	'   «ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ   ∫       Sorting order  '
	DB	'   ∫        [ Next ]   [ Previou   ∫       Re-read a pane '
	DB	'   »ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ   «ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ '
	DB	'                                   ∫                      '
	DB	'                                   »ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕ '
	DB	'                                                          '
	DB	'                                                          '

	.DATA
Colr1_T	DB	16,Back_C, 8,Cursr_C, 10,Back_C, 2,Clock_C, 1,Clock_C+80h, 3,Clock_C, 18,0FFh, 0
	DB	 1,Back_C, 12,Title_C, 1,Back_C, 9,Title_C, 1,Back_C, 8,Title_C, 1,Back_C, 6,Title_C, 1,Back_C, 18,0FFh, 0
	DB	40,Back_C, 18,0FFh, 0
	DB	57,Back_C, 1,0FFh, 0
	DB	57,Back_C, 1,0FFh, 0
	DB	 3,Back_C, 20,Box_C, 34,Back_C, 1,0FFh, 0
	DB	 3,Back_C, 20,Box_C, 4,Back_C, 14,Slct_C, 16,Back_C, 1,0FFh, 0
	DB	 3,Back_C, 15,Box_C, 4,BoxBr_C, 1,Box_C, 34,Back_C, 1,0FFh, 0
	DB	 3,Back_C, 13,Box_C, 3,BoxCu_C, 4,Box_C, 4,Back_C, 14,CurSl_C, 16,Back_C, 1,0FFh, 0
	DB	 3,Back_C, 20,Box_C, 34,Back_C, 1,0FFh, 0
	DB	 3,Back_C, 20,Box_C, 1,Back_C, 33,Hist_C, 1,0FFh, 0
	DB	 5,Back_C, 18,Back_C+40h, 1,Back_C, 33,Hist_C, 1,0FFh, 0
	DB	23,Back_C, 1,Back_C, 8,Hist_C, 20,HstCu_C, 5,Hist_C, 1,0FFh, 0
	DB	23,Back_C, 1,Back_C, 33,Hist_C, 1,0FFh, 0
	DB	 1,Back_C, 22,Cursr_C, 1,Back_C, 33,Hist_C, 1,0FFh, 0
	DB	23,Back_C, 1,Back_C, 33,Hist_C, 1,0FFh, 0
	DB	 1,Back_C, 22,CurSl_C, 1,Back_C, 33,Hist_C, 1,0FFh, 0
	DB	 1,Back_C, 22,Slct_C, 3,Back_C, 31,Back_C+40h, 1,0FFh, 0
	DB	 1,Back_C, 22,Slct_C, 4,Back_C, 14,Cursr_C, 16,Back_C, 1,0FFh, 0
	DB	57,Back_C, 1,0FFh, 0
	DB	57,Back_C, 1,0FFh, 0
	DB	4,Back_C, 19,Slct_C, 34,Back_C, 1,0FFh, 0
	DB	57,Back_C, 1,0FFh, 0
	DB	23,0FFh, 34,Back_C, 1,0FFh, 0
	DB	1,KeyNu_C, 6,KeyTi_C, 2,KeyNu_C, 6,KeyTi_C, 2,KeyNu_C, 6,KeyTi_C, 34,Back_C, 1,0FFh, 0
Colr2_T	DB	 2,Menu_C, 8,MnCur_C, 47,Menu_C, 1,0FFh, 0
	DB	 2,Sky1_C, 25,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 2,MnuTx_C, 1,MnuBr_C, 20,MnuTx_C, 1,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 2,MnuTx_C, 1,MnuBr_C, 20,MnuTx_C, 1,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 2,MnuTx_C, 1,MnuBr_C, 20,MnuTx_C, 1,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 2,MnuTx_C, 1,MnuBr_C, 20,MnuTx_C, 1,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 2,MnuCu_C, 1,MnuCB_C, 20,MnuCu_C, 1,MnuBx_C, 8,Sky1_C, 1,Sky2_C, 21,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 25,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 2,MnuTx_C, 1,MnuBr_C, 20,MnuTx_C, 1,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 3,MnuTx_C, 1,MnuBr_C, 19,MnuTx_C, 1,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 4,MnuTx_C, 1,MnuBr_C, 18,MnuTx_C, 1,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 2,MnuTx_C, 1,MnuBr_C, 20,MnuTx_C, 1,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 2,MnuTx_C, 1,MnuBr_C, 20,MnuTx_C, 1,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 25,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 2,MnuTx_C, 1,MnuBr_C, 20,MnuTx_C, 1,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 1,MnuDr_C, 1,MnuMi_C, 21,MnuDr_C, 1,MnuBx_C, 19,Sky1_C, 1,Sky2_C, 10,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 1,MnuBx_C, 2,MnuTx_C, 1,MnuBr_C, 20,MnuTx_C, 1,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	 2,Sky1_C, 25,MnuBx_C, 30,Sky1_C, 1,0FFh, 0
	DB	57,Sky1_C, 1,0FFh, 0
	DB	45,Sky1_C, 1,Sky2_C, 11,Sky1_C, 1,0FFh, 0
	DB	57,Sky1_C, 1,0FFh, 0
	DB	34,Sky1_C, 1,Sky2_C, 22,Sky1_C, 1,0FFh, 0
	DB	57,Sky1_C, 1,0FFh, 0
	DB	57,Sky1_C, 1,0FFh, 0
	DB	57,Sky1_C, 1,0FFh, 0
Colr3_T	DB	12,Menu_C, 45,Conf_C, 1,0FFh, 0
	DB	 1,Arrow_C, 11,Back_C, 45,Conf_C, 1,0FFh, 0
	DB	 1,Arrow_C, 11,Back_C, 45,Conf_C, 1,0FFh, 0
	DB	 1,Arrow_C, 11,Back_C, 8,Conf_C, 14,CnfUn_C, 20,Conf_C, 1,CnfBo_C, 2,Conf_C, 1,0FFh, 0
	DB	12,Back_C, 45,Conf_C, 1,0FFh, 0
	DB	 6,Back_C, 51,Err_C, 1,0FFh, 0
	DB	 6,Back_C, 51,Err_C, 1,0FFh, 0
	DB	 6,Back_C, 51,Err_C, 1,0FFh, 0
	DB	 6,Back_C, 51,Err_C, 1,0FFh, 0
	DB	 6,Back_C, 44,Err_C, 1,ErInv_C, 6,Err_C, 1,0FFh, 0
	DB	 6,Back_C, 51,Err_C, 1,0FFh, 0
	DB	 6,Back_C, 49,Err_C, 2,Err_C+40h, 1,0FFh, 0
	DB	 8,Back_C, 4,Back_C+40h, 5,Conf_C+40h, 19,Err_C, 8,ErBr_C, 11,Err_C, 2,Conf_C+40h, 1,0FFh, 0
	DB	12,Back_C, 5,Conf_C, 38,Err_C, 2,Conf_C+40h, 1,0FFh, 0
	DB	12,HlBrd_C, 5,Conf_C, 38,Err_C, 2,Conf_C+40h, 1,0FFh, 0
	DB	12,HlpMn_C, 5,Conf_C, 8,Err_C, 8,ErInv_C, 22,Err_C, 2,Conf_C+40h, 1,0FFh, 0
	DB	12,HlpMn_C, 5,Conf_C, 38,Err_C, 2,Conf_C+40h, 1,0FFh, 0
	DB	12,HlpMn_C, 5,Conf_C, 38,Err_C, 2,Conf_C+40h, 1,0FFh, 0
	DB	12,HlpMn_C, 7,Conf_C, 38,Conf_C+40h, 1,0FFh, 0
	DB	12,HlpMn_C, 19,Conf_C, 12,CnfRe_C, 14,Conf_C, 1,0FFh, 0
	DB	12,HlpMn_C, 24,Conf_C, 13,CnfBo_C, 8,Conf_C, 1,0FFh, 0
	DB	12,HlpMn_C, 45,Conf_C, 1,0FFh, 0
	DB	14,HlpMn_C, 20,HlpMn_C+40h, 23,Conf_C, 1,0FFh, 0
	DB	34,HlBrd_C, 23,Conf_C, 1,0FFh, 0
	DB	1,KeyNu_C, 6,KeyTi_C, 2,KeyNu_C, 6,KeyTi_C, 2,KeyNu_C, 6,KeyTi_C, 2,KeyNu_C, 6,KeyTi_C, 2,KeyNu_C, 1,KeyTi_C, 23,Conf_C, 1,0FFh, 0
Colr4_T	DB	58,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	 5,Help_C, 16,HlpBo_C, 36,Help_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	24,Help_C, 6,HlpUn_C, 27,Help_C, 1,0FFh, 0
	DB	29,Help_C, 3,HlpUn_C, 25,Help_C, 1,0FFh, 0
	DB	 6,Help_C, 5,HlpUn_C, 26,Help_C, 20,HlpRe_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	 8,Help_C, 13,HlpBo_C, 36,Help_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	 8,Help_C, 18,HlpBo_C, 31,Help_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	12,Help_C, 8,HlpRe_C, 37,Help_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	57,Help_C, 1,0FFh, 0
	DB	32,0FFh, 25,Help_C, 1,0FFh, 0
	DB	32,0FFh, 25,Help_C, 1,0FFh, 0
	DB	58,0FFh, 0

ColPage	DB	7 DUP(1)
	DB	9 DUP(2)
	DB	3 DUP(1)
	DB	4 DUP(3)
	DB	2 DUP(1)
	DB	2 DUP(3)
	DB	4 DUP(4)
	DB	3 DUP(3)
	DB	1 DUP(1)
	DB	1 DUP(3)
	DB	2 DUP(2)
	DB	1 DUP(1)
PagTab	DW	Page1_S, Colr1_T
	DW	Page2_S, Colr2_T
	DW	Page3_S, Colr3_T
	DW	Page4_S, Colr4_T
ColSt_T	DW	4B00h, Col20
	DW	4D00h, Col21
	DW	4800h, Col22
	DW	5000h, Col23
	DW	4700h, Col24
	DW	4F00h, Col25
	DW	4900h, Col26
	DW	5100h, Col27
	DW	4200h, Col28
	DW	000Dh, Col50
	DW	001Bh, Col55
	DW	4400h, Col55
	DW	0

	.CODE
Col00:	PUSH	DX
	PUSH	BX
	XOR	AX,AX
	XCHG	AL,DS:Clock
	XCHG	AH,DS:KeyBar
	PUSH	AX
	MOV	AL,DS:ColTyp
	PUSH	AX
	XOR	BX,BX
	MOV	DH,DS:Lines
	MOV	DL,58
	MOV	SI,OFFSET WinBuf
	CALL	SavWin
	PUSH	SI
	PUSH	DX
	PUSH	BX
	MOV	BH,DH
	DEC	BH
	MOV	BL,58
	MOV	DX,116h
	MOV	AH,DS:BackC
	MOV	AL,' '
	MOV	CL,DL
	MOV	CH,0
	CALL	BasAdr
	REP	STOSW
	CALL	Window
	MOV	CL,DS:Lines
	MOV	CH,0
	SUB	CL,25
	JBE	Col02
	MOV	BX,1900h
	CALL	BasAdr
Col01:	PUSH	DI
	PUSH	CX
	MOV	CX,58
	MOV	AX,720h
	REP	STOSW
	POP	CX
	POP	DI
	ADD	DI,2*80
	LOOP	Col01
Col02:	MOV	AX,BP
	MOV	DS:ColTyp,AL
	MOV	BX,DS:ColCur
	MOV	BL,DS:ColPage[BX]
	MOV	BH,0
	DEC	BX
	ADD	BX,BX
	ADD	BX,BX
	MOV	SI,DS:PagTab[BX][0]
	MOV	BX,DS:PagTab[BX][2]
	MOV	DI,OFFSET ScrBuf
	MOV	CX,25
Col03:	PUSH	DI
	PUSH	CX
Col04:	MOV	AX,DS:[BX]
	INC	BX
	CMP	AL,0
	JZ	Col07
	INC	BX
	MOV	CL,AL
	MOV	CH,0
	CMP	AH,0FFh
	JNE	Col05
	MOV	AL,7
	JMP	Col06
Col05:	MOV	AL,AH
	AND	AH,3Fh
	CALL	Color
	TEST	AL,40h
	PUSHF
	AND	AL,80h
	OR	AL,AH
	POPF
	JZ	Col06
	MOV	AH,ShadFl
	CALL	Color
	CALL	ShadCol
Col06:	MOV	AH,AL
	CALL	MvsBwc
	JMP	Col04
Col07:	POP	CX
	POP	DI
	ADD	DI,2*80
	LOOP	Col03
	POP	BX
	POP	DX
	CALL	Window

	MOV	AH,BYTE PTR DS:ColCur	; Set color
	CALL	Color
	MOV	CL,AH
Col10:	MOV	CH,CL
	PUSH	CX
	CALL	GetMous
	POP	CX
	JNZ	Col11
	MOV	AL,1
	CALL	Input
	CMP	AH,0FCh
	JE	Col11
	MOV	SI,OFFSET ColSt_T
	CALL	Case
	JMP	Col10
Col11:	PUSH	CX			; Mouse support
	CALL	GetMous
	CALL	ClrKbd
	TEST	AL,4
	JZ	Col13
	MOV	AL,'x'
	CALL	TypMous
Col12:	CALL	GetMous
	CALL	ClrKbd
	TEST	AL,4
	JNZ	Col12
	PUSH	AX
	MOV	AL,0
	CALL	TypMous
	POP	AX
	TEST	AL,3
	JZ	Col18
Col13:	TEST	AL,2
	JZ	Col15
	MOV	AL,'˚'
	CALL	TypMous
Col14:	CALL	GetMous
	CALL	ClrKbd
	TEST	AL,2
	JNZ	Col14
	PUSH	AX
	MOV	AL,0
	CALL	TypMous
	POP	AX
	TEST	AL,5
	JNZ	Col15
	POP	CX
	JMP	Col50
Col15:	TEST	AL,1
	JZ	Col16
	PUSH	DX
	PUSH	BX
	MOV	BX,43Dh
	MOV	DX,1010h
	CALL	InWind
	POP	BX
	POP	DX
	JNC	Col19
Col16:	POP	CX
Col17:	JMP	Col10
Col18:	POP	CX
	JMP	Col55
Col19:	MOV	AX,CX
	POP	CX
	SUB	AX,43Dh
	MOV	CL,4
	SHL	AH,CL
	OR	AL,AH
	MOV	CL,AL
	JMP	Col30

Col20:	TEST	CL,0Fh			; Left
	JZ	Col25
	DEC	CL
	JMP	Col30
Col21:	INC	CL			; Right
	TEST	CL,0Fh
	JNZ	Col30
	SUB	CL,10h
	JMP	Col30
Col22:	SUB	CL,10h			; Up
	JMP	Col30
Col23:	ADD	CL,10h			; Down
	JMP	Col30
Col24:	AND	CL,0F0h			; Home
	JMP	Col30
Col25:	OR	CL,0Fh			; End
	JMP	Col30
Col26:	AND	CL,0Fh			; PgUp
	JMP	Col30
Col27:	OR	CL,0F0h			; PgDn
	JMP	Col30
Col28:	MOV	AH,BYTE PTR DS:ColCur	; Default
	CALL	ColAddr
	SUB	DI,OFFSET ColTab
	MOV	CL,DS:Palets[DI]
Col30:	CMP	CL,CH			; Change color
	JE	Col17
	MOV	AL,CL
	CALL	F07_75
	PUSH	DX
	PUSH	CX
	PUSH	BX
	MOV	CH,CL
	MOV	SI,CX
	MOV	BX,DS:ColCur
	MOV	BL,DS:ColPage[BX]
	MOV	BH,0
	DEC	BX
	ADD	BX,BX
	ADD	BX,BX
	MOV	BX,DS:PagTab[BX][2]
	MOV	DI,OFFSET ScrBuf
	MOV	DX,0FF00h
	MOV	CX,25
Col31:	PUSH	DI
	PUSH	DX
	PUSH	CX
	MOV	DL,0
Col32:	MOV	AX,DS:[BX]
	INC	BX
	CMP	AL,0
	JZ	Col38
	INC	BX
	MOV	CL,AL
	MOV	CH,0
	MOV	DH,AH
	CMP	BYTE PTR DS:ColCur,ShadFl
	JNE	Col33
	CMP	AH,0FFh
	JE	Col34
	TEST	AH,40h
	JZ	Col34
	MOV	AX,SI
	MOV	AL,DH
	AND	AL,3Fh
	XCHG	AL,AH
	CALL	Color
	XCHG	AL,AH
	JMP	Col36
Col33:	AND	AH,3Fh
	CMP	AH,BYTE PTR DS:ColCur
	JE	Col35
Col34:	ADD	DI,CX
	ADD	DI,CX
	JMP	Col32
Col35:	MOV	AX,SI
	MOV	AL,DH
	TEST	AL,40h
	PUSHF
	AND	AL,80h
	OR	AL,AH
	POPF
	JZ	Col37
	MOV	AH,ShadFl
	CALL	Color
Col36:	CALL	ShadCol
Col37:	CALL	MvsColr
	MOV	DL,1
	JMP	Col32
Col38:	OR	DL,DL
	POP	CX
	POP	DX
	POP	DI
	JZ	Col39
	MOV	DL,CL
	CMP	DH,0FFh
	JNE	Col39
	MOV	DH,CL
Col39:	ADD	DI,2*80
	LOOP	Col31
	CMP	DH,0FFh
	JE	Col40
	MOV	BX,1900h
	SUB	BH,DH
	SUB	DH,DL
	INC	DH
	MOV	DL,58
	CALL	Window
Col40:	POP	BX
	POP	CX
	POP	DX
	JMP	Col10
Col50:	MOV	AH,BYTE PTR DS:ColCur	; Enter
	CALL	ColAddr
	MOV	DS:[DI],CL
Col55:	MOV	AH,BYTE PTR DS:ColCur	; Esc
	CALL	Color
	MOV	AL,AH
	CALL	F07_75
	POP	SI
	CALL	ResWin
	CALL	Window
	POP	AX
	MOV	DS:ColTyp,AL
	POP	AX
	MOV	DS:Clock,AL
	MOV	DS:KeyBar,AH
	POP	BX
	POP	DX
	JMP	F07_20

;-------------- Set default configuration --------------
	.CONST
Def1_S	DB	'Do you wish to reset all options',0
Def2_S	DB	'to default value?',0
YesNo_S	DB	'Yes',0FFh,3,'No',0

	.DATA
Deflt_P	DB	Box_C, BoxCu_C, BoxCu_C, 0
	DB	4, 0
	DW	WinBuf
	DW	SetUp_S
	DW	Def1_S
	DW	Def2_S
	DW	YesNo_S
Deflt_B	BOXHD	< Deflt_P, 728h, 0, 1, 0FFh, 5, 4>
	BOXMN	< 4, 0,0,0,0, 423h, TmpErr, YesNo_D>
	DB	0
YesNo_D	DB	5, 0
	DB	4, 6
	DB	0

	.CODE
F08_00:	MOV	BP,OFFSET Deflt_B
	CALL	Dialog
	JNZ	F08_01
	CALL	Default
F08_01:	JMP	Main00

;-------------------- Country info ---------------------
	.CONST
Inf00_S	DB				' Country info ',0
Inf01_S	DB	' ⁄ ^bTime format^b ƒø ⁄ ^bDate format^b ƒƒø ⁄ ^bNumbers format^b ƒø ',0
Inf02_S	DB	' ≥             ', ' ≥ ≥              ', ' ≥ ≥                ', ' ≥ ',0
Inf03_S	DB	' ¿',0FFh,80h+14,  'ƒŸ ¿',0FFh,80h+15,   'ƒŸ ¿',0FFh,80h+17,     'ƒŸ ',0
Inf04_S	DB	' ⁄ ^bCharacters table^b '                     ,0FFh,80h+34,       'ƒø ',0
Inf05_S	DB	' ≥',0FFh,6,   'ÄÅÇÉÑÖÜáàâäãåçéèêëíìîïñóòôöõúùûü†°¢£§•¶ß'   ,0FFh,6,'≥ ',0
Inf06_S	DB	' ≥',0FFh,3,'⁄ƒ                                        '   ,0FFh,6,'≥ ',0
Inf07_S	DB	' ≥',0FFh,3,'≥                                          ƒø',0FFh,3,'≥ ',0
Inf08_S	DB	' ≥',0FFh,3,'≥'                                 ,0FFh,44,'≥',0FFh,3,'≥ ',0
Inf09_S	DB	' ≥',0FFh,3,'≥  ®©™´¨≠ÆØ‡·‚„‰ÂÊÁËÈÍÎÏÌÓÔÒÚÛÙıˆ˜¯˘˙˚¸˝˛   ≥',0FFh,3,'≥ ',0
Inf10_S	DB	' ≥',0FFh,3,'√ƒ                                          ≥',0FFh,3,'≥ ',0
Inf11_S	DB	' ≥',0FFh,3,'≥                                          ƒ¥',0FFh,3,'≥ ',0
Inf12_S	DB	' ≥',0FFh,3,'¿ƒƒƒƒ ^bUpper case',0FFh,14,'Lower case^b ƒƒƒƒŸ',0FFh,3,'≥ ',0
Inf13_S	DB	' ¿',0FFh,80h+52,                                                   'ƒŸ ',0
Inf14_S	DB	'The country information is determined by',0
Inf15_S	DB	'the ^bcountry=^b statement in the ^bconfig.sys^b file.',0
Ok_S	DB	'[ Ok ]',0

	.DATA
Info_P	DB	Conf_C, CnfRe_C, CnfBo_C, 0
	DB	18, 0
	DW	WinBuf
	DW	Inf00_S
	DW	Inf01_S
	DW	Inf02_S
	DW	Inf03_S
	DW	Inf04_S
	DW	Inf05_S
	DW	Inf06_S
	DW	Inf07_S
	DW	Inf08_S
	DW	Inf09_S
	DW	Inf10_S
	DW	Inf11_S
	DW	Inf12_S
	DW	Inf13_S
	DW	Inf14_S
	DW	Inf15_S
	DW	0
	DW	Ok_S
Info_B	BOXHD	< Info_P, 228h, 0, 1, 4, 6, 0 >
	BOXMN	<4, 0, 0, 0, 0, 1225h, TmpBuf, Ok_D >
	DB	0
Ok_D	DB	6, 0
	DB	0

	.CODE
F09_00:	MOV	SI,OFFSET Inf05_S+4	; Case table
	MOV	DI,OFFSET Inf06_S+7
	PUSH	SI
	CALL	F09_10
	POP	SI
	MOV	DI,OFFSET Inf07_S+7
	CALL	F09_20
	MOV	SI,OFFSET Inf09_S+7
	MOV	DI,OFFSET Inf10_S+7
	PUSH	SI
	CALL	F09_10
	POP	SI
	MOV	DI,OFFSET Inf11_S+7
	CALL	F09_20
	MOV	AH,DS:Country.TimeSep	; Time format
	MOV	AL,'9'
	MOV	BX,'00'
	MOV	CL,'p'
	CMP	DS:Country.TimeFmt,0
	JZ	F09_01
	MOV	BL,AH
	MOV	AX,'12'
	MOV	CL,'0'
F09_01:	MOV	DI,OFFSET Inf02_S+6
	STOSW
	MOV	AX,BX
	STOSW
	MOV	AL,CL
	STOSB
	MOV	AH,DS:Country.DateSep	; Date format
	MOV	AL,'M'
	MOV	BX,'YD'
	CMP	DS:Country.DateFmt,1
	JB	F09_02
	XCHG	BL,AL
	JE	F09_02
	XCHG	BH,AL
F09_02:	MOV	DI,OFFSET Inf02_S+22
	STOSB
	STOSW
	MOV	AL,BL
	STOSB
	STOSW
	MOV	AL,BH
	STOSB
	STOSB
	MOV	AH,DS:Country.Sep1000	; Numbers format
	MOV	DI,OFFSET Inf02_S+39
	OR	AH,AH
	JNE	F09_03
	INC	DI
F09_03:	MOV	AL,'1'
	STOSB
	MOV	CX,3
	MOV	AL,'0'
F09_04:	XCHG	AL,AH
	STOSW
	XCHG	AL,AH
	STOSB
	STOSB
	LOOP	F09_04
	MOV	BP,OFFSET Info_B
	CALL	Dialog
	JMP	Main00

F09_10	PROC	NEAR
	MOV	CX,40
F09_11:	LODSB
	CALL	UpCase
	STOSB
	LOOP	F09_11
	RET
F09_10	ENDP

F09_20	PROC	NEAR
	MOV	CX,40
F09_21:	LODSB
	CALL	LowCase
	STOSB
	LOOP	F09_21
	RET
F09_20	ENDP

;------------------------ Exit -------------------------
	.CONST
SvSt0_S	DB	' Setup ',0
SvSt1_S	DB	'Do you wish to save',0
SvSt2_S	DB	'the current setup?',0
SvSt3_S	DB	'Save',0FFh,3,'Cancel',0
StErr_S	DB	'There was an error while saving the setup file',13,10,'$'

	.DATA
SvSt_P	DB	Box_C, BoxCu_C, BoxCu_C, 0
	DB	4, 0
	DW	WinBuf
	DW	SvSt0_S
	DW	SvSt1_S
	DW	SvSt2_S
	DW	SvSt3_S
SvSt_B	BOXHD	< SvSt_P, 528h, 0, 1, 4, 7, 0 >
	BOXMN	< 4, 0,0,0,0, 420h, TmpBuf, SvSt_D >
	DB	0
SvSt_D	DB	6, 0
	DB	8, 7
	DB	0

	.CODE
Exit:	MOV	SI,OFFSET SrcIni	; Compare VC.INI with source
	MOV	DI,OFFSET ColTab
	MOV	CX,CmnIni
	REPE	CMPSB
	MOV	BP,OFFSET SvSt_B
	JE	Exit1
	CALL	Dialog
	JNZ	Exit1
	XOR	BP,BP
Exit1:	PUSH	DS			; Restore vector 1Bh
	LDS	DX,DS:BrkVct
	MOV	AX,251Bh
	INT	21h
	POP	DS
	MOV	SI,OFFSET UserScr	; Restore screen
	MOV	DI,OFFSET ScrBuf
	MOV	DH,DS:Lines
	MOV	DL,80
	MOV	AL,DL
	MUL	DH
	MOV	CX,AX
	REP	MOVSW
	XOR	BX,BX
	CALL	Window
	MOV	AL,0			; Hide mouse
	CALL	CurMous
	MOV	CX,DS:CurType		; Restore cursor
	MOV	AH,1
	INT	10h
	MOV	AH,DS:DosLin
	MOV	AL,0
	CALL	Cursor
	MOV	DL,DS:BreakFl		; Restore BREAK status
	MOV	AX,3301h
	INT	21h
	OR	BP,BP
	JZ	Exit2
	JMP	Exit7

Exit2:	MOV	SI,OFFSET IniData	; Save VC.INI
	MOV	DI,SI
	MOV	CX,3
	MOV	AL,'V'
	REP	STOSB
	XOR	BX,BX
	MOV	CX,IniLen-2
	MOV	AH,0
Exit3:	LODSB
	ADD	BX,AX
	LOOP	Exit3
	MOV	DS:[SI],BX

	MOV	DX,OFFSET NcPath	; Open VC.INI
	MOV	AX,4300h
	INT	21h
	JNC	Exit4
	CMP	AX,2
	JNE	Exit5
	XOR	AX,AX
Exit4:	OR	CL,20h
	MOV	AH,3Ch
	INT	21h
	JC	Exit5
	MOV	BX,AX
	MOV	CX,IniLen
	MOV	DX,OFFSET IniData
	MOV	AH,40h
	INT	21h
	PUSHF
	PUSH	AX
	MOV	AH,3Eh
	INT	21h
	POP	BX
	POP	DX
	JC	Exit5
	PUSH	DX
	POPF
	JC	Exit5
	CMP	BX,CX
	JE	Exit6
Exit5:	MOV	DX,OFFSET StErr_S
	MOV	AH,9
	INT	21h

Exit6:	MOV	CX,'VC'			; Save in resident VC
	MOV	DX,'OM'
	MOV	AX,2B02h
	INT	21h
	CMP	AX,VcApiVr*100h
	JNE	Exit7
	MOV	DI,BX
	SUB	BX,OFFSET ColTab
	MOV	AL,ES:Active[BX]
	MOV	DS:Active,AL
	MOV	AL,ES:Ctrl_O[BX]
	MOV	DS:Ctrl_O,AL
	MOV	AL,ES:ChkTSR[BX]
	MOV	DS:ChkTSR,AL
	MOV	AL,ES:LoadFl[BX]
	XCHG	AL,DS:LoadFl
	MOV	ES:LoadRes[BX],AL
	MOV	SI,OFFSET ColTab
	MOV	CX,CmnIni
	REP	MOVSB

Exit7:	MOV	AX,4C00h		; Exit
	INT	21h

CtrlC:	IRET

;--------------------- Default init --------------------
Default	PROC	NEAR
	MOV	SI,OFFSET Palets
	MOV	DI,OFFSET ColTab
	MOV	CX,CmnIni
	REP	MOVSB
	MOV	SI,OFFSET DefWCB
	MOV	DI,OFFSET WCB1
	MOV	CX,WCBini
	PUSH	DI
	PUSH	CX
	REP	MOVSB
	MOV	AH,19h
	INT	21h
	ADD	DS:WCB1.WinPath[0],AL
	POP	CX
	POP	SI
	MOV	DI,OFFSET WCB2
	REP	MOVSB
	MOV	DS:WCB1.Visible,CL
	RET
Default	ENDP

	INCLUDE	VCSTHLP.INC
	INCLUDE	VCSTSUB.INC

	END
