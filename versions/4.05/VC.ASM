;-------------------------------------------------------
;		   Idea :	15.05.1991
;		  Begin :	16.05.1991
;		Version :	16.06.2000
;-------------------------------------------------------

	JUMPS
	.MODEL	Tiny
	.CODE
Zero:	.STARTUP
	JMP	Alloc0

PrgName	EQU	'Volkov Commander'
HelpVer	EQU	'VC 4.05 Help'
VcVers	EQU	'4.05'
VcSuff	EQU	'freeware'
VcApiVr	EQU	42

MoDelay	EQU	40			; Mouse double click delay
MiliSec	EQU	60			; auto CD time : sec x 0.01
Sec	EQU	0			;                sec x 1
LenStk	EQU	100h			; Stack size
LenPath	EQU	68			; Length of path name
LenColr	EQU	40			; Length of color table
LenHist	EQU	512			; Length of hisory buffer
LenExt	EQU	512			; Length of extension file buffer
LenMenu	EQU	4000			; Length of menu buffer
LenStr	EQU	64			; Length of path & file string
LenCmd	EQU	124			; Length of command line
LenBat	EQU	1000			; Length of full command line
LenHelp	EQU	3000			; Maximal length of help page
MinFls	EQU	256			; Minimum files in window
MinTree	EQU	768			; Minimum tree size
MinView	EQU	8000			; Minimum view buffer size
MaxLins	EQU	50			; Maximum screen lines
MaxProg	EQU	18			; Maximal resident quantaty
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

; Coordinates structure definition.
COORD	STRUC
  X	DB	?			; Column
  Y	DB	?			; Row
COORD	ENDS

; DTA structure definition.
DTAs	STRUC
	  DB	15h DUP(?)
  FilAttr DB	?
  FilTime DW	?
  FilDate DW	?
  FilSize DD	?
  FilName DB	13 DUP(?)
	  DB	?
DTAs	ENDS

; Execute program block structure definition.
EPBs	STRUC
  EnvSeg  DW	?			; Segment of new environment
  CmdLine DD	?			; Pointer to command line
  FCB1	  DD	?			; Pointer to FCB1
  FCB2	  DD	?			; Pointer to FCB2
EPBs	ENDS

; Country information structure definition.
CNTRY	STRUC
  DateFmt DW	?			; Date format
	  DB	5 DUP(?)		; Currency
  Sep1000 DB	2 DUP(?)		; Thousand separator
	  DB	2 DUP(?)		; Decimal separator
  DateSep DB	2 DUP(?)		; Date separator
  TimeSep DB	2 DUP(?)		; Time separator
	  DB	?			; Currency format
	  DB	?			; Number of meaning digits in currency
  TimeFmt DB	?			; Time format
  CaseMap DD	?			; Addres of case map
	  DB	2 DUP(?)		; Data separator
	  DB	8 DUP(?)		; Reserved
CNTRY	ENDS

; File entry in DskBuf.
FILENT	STRUC
  NameF   DB	13 DUP(?)
  AttrF   DB	?
  TimeF   DW	?
  DateF   DW	?
  SizeF   DD	?
  NumbF   DW	?
FILENT	ENDS

; Subdirectory entry of NCD file.
DIRENT	STRUC
  DirName  DB   13 DUP(?)		; Subdirectory name in ASCIIZ
  DirLevel DB	?			; Subdirectory level
  DirShift DB	?			; 1 in next entry - there is branch
  DirCont  DB	?			; 0 - end of branch 'А', 1 - 'Г'
DIRENT	ENDS

; Structure of NCD file.
DIRTREE	STRUC
  TreHead  DB	   5 DUP(?)		; Tree file header 'PNCI',0
  TreeLen  DW	   ?			; Number of subdirectories
  TreSum1  DW	   ?			; Check sum of the header
  TreList  LABEL   BYTE			; List of subdirectories
  TreSum2  DW	   ?			; Check sum of the list
DIRTREE	ENDS

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

;--------------------- Data segment --------------------
MainDat	STRUC
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
DriveB	DB	?			; Show logic drive B: in 1 floppy system
DelayTm	DB	?, ?			; Auto change dir delay time
StarTab	DB	5  DUP (?)		; Star table
ResIni	DB	51 DUP (?)		; Reserved
CmnIni	=	$			; End of main data' part saved in VC.INI

LoadRes	DB	?			; Original LoadFl
ScrMod	DB	?			; Current screen mode
Mouse	DB	?			; 1 if Mouse was found
BreakFl	DB	?			; Break DOS's status
MenuFl	DB	?			; 1 to bring user menu automatically
SelMas	DB	13 DUP(?)		; Wildcard for selection
HstBuf	DB	LenHist DUP(?)		; History buffer
ExtMain	DB	LenExt DUP(?)		; Buffer for the main extension file
EPB	EPBs	?			; Execute program block
MouseX	DW	?			; Mouse cursor X
MouseY	DW	?			; Mouse cursor Y
BatAdr	DW	?			; Pointer to command in buffer
BatPoin	DW	?			; Pointer to command
LastMnu	DW	?			; The last pull down menu item
OvlPth	DW	?			; Pointer to file name in environment
ComSpec	DW	?			; Pointer to COMSPEC= in environment
CrnPath	DB	LenPath DUP(?)		; Current directory in the tree panel
ViewFil	DB	LenStr+1 DUP(?)		; Source filename
ToFile	DB	LenStr+1 DUP(?)		; Target filename
FindStr	DB	LenStr+1 DUP(?)		; FileFind wildcard
SrchStr	DB	LenStr+1 DUP(?)		; String for search
DosChg	DB	5 DUP(?)		; Reserved for change drive or /C_ 
DosBuf	DB	LenBat DUP(?)		; Command line
CmnSave	=	$			; End of main data' part saved in memory

DosLin	DB	?			; Line number of DOS's command line
DosCol	DB	?			; Current column of DOS's command line
BegCol	DB	?			; Start position of DOS's command line
OutCol	DB	?			; Start position for output com. line
ScrPage	DB	?			; Current screen page number
TstEGA	DB	?			; 1 if EGA or VGA present
Lines	DB	?			; Number of lines on screen
Snow	DB	?			; Snow on/off
TopView	DB	?			; TopView output
DesqVie	DB	?			; Running under DESQVIEW
WinLeav	DB	?			; Leave time under Windows
CrntCol	DB	?			; Color which DOS currently uses
Quote	DB	?			; Quote next character
BarPrev	DB	?			; Previous Key Bar
SftErr	DB	?			; Shift error boxes down
BlocEr	DB	?			; Disable critical error box
PathEr	DB	?			; Error when reading current path
FlagCD	DB	?			; 1 when needs auto CD
TreeTyp	DB	?
WinMous	DB	?
MousEnt	DB	?
OldFlg	DB	?
HlpPage	DB	?
ZoomOne	DB	?
OneFile	DB	?			; Do one file and exit
ErrDat	DB	?
VerDOS	DW	?
WCBcrn	DW	?
WCBsec	DW	?
WCB1seg	DW	?
WCB2seg	DW	?
ViewSeg	DW	?
FreMem1	DW	?
FreMem2	DW	?
Memorys	DW	?
HstAdr	DW	?
TreAdr	DW	?
TreFrst	DW	?
TreCrnt	DW	?
ClstSiz	DW	?
Total	DW	?
FreeDsk	DW	?
InfoLen	DW	?
Rand1	DW	?
Rand2	DW	?
FncAddr	DW	?
MousPos	DW	?
MousTim	DD	?
AutoTim	DD	?
Screen	DD	?			; Address of screen
ErrVct	DD	?			; Original vector 24h
BrkVct	DD	?			; Original vector 1Bh
Volume	DB	12 DUP(?)		; Volume label
DosPth	DB	LenPath DUP(?)		; The current path
NcPath	DB	LenPath DUP(?)		; The path from VC= variable
TempDir	DB	LenPath DUP(?)		; The path from TEMP= variable
Country	CNTRY	?			; Country information
DTA	DTAs	?			; Data tramsnir area
LowTab	DB	128 DUP(?)		; Lower case table
TmpBuf	DB	256 DUP(?)
TmpErr	DB	256 DUP(?)
Stars	STAR	NmbStrs DUP(?)
MenuBuf	DB	LenMenu DUP(?)
InfBuf	DB	(MaxLins-13)* 38 DUP(?)
	EVEN
ScrBuf	DW	 MaxLins    * 80 DUP(?)
UserScr	DW	(MaxLins-1) * 80 DUP(?)
WinBuf	DW	(MaxLins-2) * 77 DUP(?)
ErrBuf	DW	(MaxLins-2) * 77 DUP(?)
ErrBuf1	DW	(MaxLins-2) * 77 DUP(?)
HelpBuf	DB	LenHelp	DUP(?)
TreeBuf	DB	MinTree*SIZE DirEnt+SIZE DirTree DUP(?)
MainDat	ENDS

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

CurAdr	DW	?
First	DW	?
EndBuf	DW	?
WCBsave	=	$			; End of WCB part saved in memory

ExtBuf	DB	LenExt	DUP(?)
EndFul	DW	?
ChgFlg	DB	?
SizDir	DB	?
WCBsize	=	$			; End of WCB

DskBuf	DB	MinFls * SIZE FilEnt DUP(?)
WCBdef	ENDS

;------------------- Begin of program ------------------
RunPar	DW	0
ChkFile	DW	0
Command	DB	'COMMAND.COM', 0
OvlPrm	DB	10, 'VVV', 0, 0, 0, 0, 0, 0, 0, 0Dh
NoRun_S	DB	'There is not enough memory to run this command', 13,10,0
FnFil_S	DB	13,10, "Can't find the file: ", 0
BadFl_S	DB	13,10, "Bad or missing ", 0
Retr_S	DB	13,10, "Press ENTER to try again or ESC to abort", 13,10,0
Aloc_S	DB	'Program too big to fit in memory', 13,10,0

;------------------- Execute commands ------------------
LoadSiz	EQU	WORD PTR DosBuf+0

Exec:	TEST	DS:ComnIni.LoadFl,7Fh
	JZ	Exec01
	CALL	GetStat
	MOV	AH,49h
	INT	21h
	CALL	SetStat
Exec01:	PUSH	DS
	POP	ES
	MOV	AX,OFFSET ComnIni.DosBuf
	MOV	DS:ComnIni.BatAdr,AX
Exec02:	CLD
	MOV	DL,DS:ComnIni.BreakFl
	MOV	AX,3301h
	INT	21h
	MOV	DI,DS:ComnIni.BatAdr
	LEA	SI,[DI-4]
	PUSH	DI
	MOV	CX,0FFFFh
	MOV	AL,0
	REPNE	SCASB
	MOV	DS:ComnIni.BatAdr,DI
	POP	DI
	MOV	CX,LenCmd
	REPNE	SCASB
	DEC	DI
	MOV	BYTE PTR DS:[DI],0Dh
	LEA	CX,[DI-1]
	CMP	AL,DS:ComnIni.ExecTyp
	JE	Exec04
	INC	SI
	INC	SI
	INC	SI
	SUB	CX,SI
	MOV	DS:[SI],CL
	CMP	DS:ComnIni.DosChg[1],0
	JE	Exec03
	MOV	DS:ComnIni.BatPoin,SI
	MOV	SI,OFFSET ComnIni.DosChg
	INT	2Eh
	CLI
	MOV	SP,CS
	MOV	SS,SP
	MOV	SP,OFFSET VcStack
	STI
	PUSH	CS
	POP	DS
	MOV	SI,DS:ComnIni.BatPoin
	MOV	DS:ComnIni.DosChg[1],0
Exec03:	INT	2Eh
	CLI
	MOV	SP,CS
	MOV	SS,SP
	MOV	SP,OFFSET VcStack
	STI
	CALL	SetBrk0
	JMP	Exec07
Exec04:	SUB	CX,SI
	MOV	CH,'/'
	MOV	DS:[SI],CX
	MOV	WORD PTR DS:[SI+2],' C'
	LEA	BX,ComnIni.EPB
	MOV	DS:[BX].CmdLine.Offs,SI
	MOV	DS:[BX].CmdLine.Segm,DS
Exec05:	MOV	DI,OFFSET Command
	MOV	DX,DS:ComnIni.ComSpec
	OR	DX,DX
	JE	Exec06
	MOV	DS,DS:[2Ch]
	MOV	AX,4B00h
	INT	21h
	CALL	SetBrk0
	JNC	Exec07
	MOV	DS,ES:[2Ch]
	MOV	DI,ES:ComnIni.ComSpec
	CMP	AX,3
	JBE	Exec06
	MOV	SI,OFFSET NoRun_S
	PUSH	CS
	CALL	OutStr0
	JMP	Exec10
Exec06:	MOV	SI,OFFSET FnFil_S
	CALL	NotFind
	PUSH	ES
	POP	DS
	JNC	Exec05
	JMP	Exec10
Exec07:	PUSH	ES
	POP	DS
	MOV	BX,DS:ComnIni.BatAdr
	CMP	BYTE PTR DS:[BX],0
	JE	Exec10
	CMP	BYTE PTR DS:[BX],'@'
	JNE	Exec08
	INC	BX
	MOV	DS:ComnIni.BatAdr,BX
	JMP	Exec02
Exec08:	MOV	DI,OFFSET IntTab
	PUSH	DI
	MOV	AX,0A0Dh
	STOSW
	MOV	AH,19h
	INT	21h
	MOV	DL,AL
	INC	DX
	ADD	AL,'A'
	STOSB
	CMP	DS:ComnIni.Prompt,0
	JE	Exec09
	MOV	AX,'\:'
	STOSW
	MOV	SI,DI
	MOV	AH,47h
	INT	21h
	MOV	CX,0FFFFh
	MOV	AL,0
	REPNE	SCASB
	DEC	DI
Exec09:	MOV	AX,'>'
	STOSW
	POP	SI
	PUSH	CS
	CALL	OutStr
	MOV	SI,DS:ComnIni.BatAdr
	PUSH	CS
	CALL	OutStr
	MOV	AX,0E0Dh
	INT	10h
	MOV	AX,0E0Ah
	INT	10h
	JMP	Exec02
Exec10:	MOV	AL,DS:ComnIni.LoadRes
	XCHG	AL,DS:ComnIni.LoadFl
	TEST	AL,7Fh
	JNZ	Exec11
	JMP	Exec15
Exec11:	CALL	GetStat			; Load VC.COM
	MOV	CL,4
	MOV	BX,OFFSET Last-1
	SHR	BX,CL
	SUB	BX,((Init00-Zero) SHR 4)-1
	MOV	AH,48h
	INT	21h
	CALL	SetStat
	SUB	AX,((Init00-Zero) SHR 4)
	MOV	CS:RunPar,AX
	JNC	Exec12
	JMP	AlocEr
Exec12:	XOR	AX,AX
	MOV	DS:ComnIni.LoadSiz,OFFSET Last
	MOV	SI,OFFSET OvlPrm
	MOV	BL,CS:[SI]
	MOV	BH,0
	MOV	CS:[SI+BX-1],CS
	LEA	BX,ComnIni.EPB
	MOV	ES:[BX].CmdLine.Offs,SI
	MOV	ES:[BX].CmdLine.Segm,CS
	MOV	DX,ES:ComnIni.OvlPth
	MOV	DS,CS:[2Ch]
	MOV	AX,4B00h
	INT	21h
	PUSH	CS
	POP	ES
	CLD
	JNC	Exec14
	CMP	AL,3
	JA	AlocEr
Exec13:	MOV	DI,ES:ComnIni.OvlPth	; Error: VC.COM not found
	MOV	DS,CS:[2Ch]
	MOV	SI,OFFSET BadFl_S
	CALL	NotFind
	JNC	Exec12
	MOV	AL,1
	JMP	Exit00
Exec14:	MOV	AH,4Dh
	INT	21h
	OR	AX,AX
	JNE	Exec13
	PUSH	ES
	POP	DS
	MOV	AX,DS:ComnIni.LoadSiz
	CMP	AX,DS:ChkFile
	JNE	Exec13
Exec15:	PUSH	CS:RunPar
	MOV	AX,OFFSET Init11
	PUSH	AX
	RETF

AlocEr:	MOV	SI,OFFSET Aloc_S
	PUSH	CS
	CALL	OutStr0
	MOV	AL,1
Exit00:	MOV	BX,CS:Vect22.Offs
	MOV	CS:[0Ah].Offs,BX
	XOR	BX,BX
	XCHG	BX,CS:Vect22.Segm
	MOV	CS:[0Ah].Segm,BX
Exit01:	PUSH	AX
	PUSH	CS
	POP	ES
	MOV	DL,ES:ComnIni.BreakFl
	MOV	AX,3301h
	INT	21h
	MOV	CX,ES:Counter
	JCXZ	Exit03
Exit02:	MOV	SI,ES:Counter
	SHL	SI,1
	MOV	DS,ES:PrgList[SI-2]
	PUSH	CS
	CALL	Release
	LOOP	Exit02
Exit03:	CMP	ES:ComnIni.ChkTSR,0
	JZ	Exit04
	LDS	DX,ES:Vect21
	MOV	AX,2521h
	INT	21h
	LDS	DX,ES:Vect27
	MOV	AX,2527h
	INT	21h
Exit04:	POP	AX
	CMP	ES:Vect22.Segm,0
	JNE	Exit05
	MOV	AH,4Ch
	INT	21h
Exit05:	JMP	ES:Vect22

NotFind	PROC	NEAR
	PUSH	DS
	PUSH	CS
	CALL	OutStr0
	POP	DS
	MOV	SI,DI
	PUSH	CS
	CALL	OutStr
	PUSH	DS
	MOV	SI,OFFSET Retr_S
	PUSH	CS
	CALL	OutStr0
	POP	DS
NotFn1:	MOV	AH,0
	INT	16h
	CMP	AL,0Dh
	JE	NotFn2
	CMP	AL,1Bh
	JNE	NotFn1
	STC
NotFn2:	RET
NotFind	ENDP

OutStr0	PROC	NEAR
	PUSH	CS
	POP	DS
OutStr:	PUSH	BP
	PUSH	BX
	MOV	BL,7
	LODSB
	MOV	AH,0Eh
	INT	10h
	POP	BX
	POP	BP
	CMP	BYTE PTR DS:[SI],0
	JNE	OutStr
	RETF
OutStr0	ENDP

SetBrk0	PROC	NEAR
	CLD
	PUSH	SS
	POP	ES
	POP	BP
	PUSH	CS
	PUSH	BP
SetBrk0	ENDP

SetBrk	PROC	NEAR
	PUSH	DS
	PUSH	AX
	PUSHF
	PUSH	SS
	POP	DS
	MOV	AX,3300h
	INT	21h
	MOV	DS:ComnIni.BreakFl,DL
	MOV	DL,0
	MOV	AX,3301h
	INT	21h
	MOV	DX,OFFSET CtrlC
	MOV	AX,2523h
	INT	21h
	POPF
	POP	AX
	POP	DS
	RETF
SetBrk	ENDP

GetStat	PROC	NEAR
	MOV	CX,0FFFFh
	MOV	AX,5802h
	INT	21h
	JC	GetSt1
	MOV	CL,AL
	MOV	AX,5800h
	INT	21h
	MOV	CH,AL
	MOV	BX,1
	MOV	AX,5803h
	INT	21h
	MOV	BX,80h
	MOV	AX,5801h
	INT	21h
GetSt1:	MOV	BP,CX
	RET
GetStat	ENDP

SetStat	PROC	NEAR
	PUSH	AX
	PUSHF
	CMP	BP,0FFFFh
	JE	SetSt1
	MOV	BX,BP
	MOV	BL,BH
	MOV	BH,0
	MOV	AX,5801h
	INT	21h
	MOV	BX,BP
	MOV	BH,0
	MOV	AX,5803h
	INT	21h
SetSt1:	POPF
	POP	AX
	RET
SetStat	ENDP

CtrlC:	IRET

New21	PROC	FAR			; INT 21h
	CMP	AH,4Bh
	JE	Fnc4B
	CMP	AH,31h
	JE	Fnc31
	CMP	AH,49h
	JE	Fnc49
	CMP	AH,2Bh
	JE	VCapi
Old21:	JMP	CS:Vect21
Fnc4B:	CALL	ExeProg
	JMP	Old21
Fnc31:	CALL	AddProg
	JNC	Old21
	MOV	AH,4Ch
	JMP	Old21
Fnc49:	CALL	FreeBlk
	JMP	Old21
VCapi:	CMP	CX,'VC'
	JNE	Old21
	CMP	DX,'OM'
	JNE	Old21
	PUSH	CS
	POP	ES
	CMP	AL,1
	MOV	BX,OFFSET WCB1sav
	JB	VCapi1
	MOV	BX,OFFSET WCB2sav
	JE	VCapi1
	MOV	BX,OFFSET ComnIni
VCapi1:	MOV	AX,VcApiVr*100h
	CLC
	RET	2
New21	ENDP

New27	PROC	FAR			; INT 27h
	ADD	DX,0Fh
	SHR	DX,1
	SHR	DX,1
	SHR	DX,1
	SHR	DX,1
	CALL	AddProg
	JNC	New27a
	MOV	AH,0
	JMP	Old21
New27a:	SHL	DX,1
	SHL	DX,1
	SHL	DX,1
	SHL	DX,1
	JMP	CS:Vect27
New27	ENDP

;------------- Save vectors & program name -------------
;			DS:DX - file name
ExeProg	PROC	C NEAR
	PUSH	ES
	PUSH	SI
	PUSH	DI
	PUSH	CX
	PUSH	AX
	CLD
	PUSH	CS
	POP	ES
	PUSH	DS			; Copy vectors & SAVE PTR
	XOR	SI,SI
	MOV	DS,SI
	MOV	DI,OFFSET IntTab
	MOV	CX,256*2
	CLI
	REP	MOVSW
	MOV	SI,4A8h
	MOVSW
	MOVSW
	STI
	POP	DS
	MOV	SI,DX			; Save file name
ExPrg1:	MOV	CX,SI
ExPrg2:	LODSB
	CMP	AL,':'
	JE	ExPrg1
	CMP	AL,'\'
	JE	ExPrg1
	CMP	AL,0
	JNE	ExPrg2
	MOV	SI,CX
	MOV	DI,OFFSET NameBuf
	MOV	CX,12
	REP	MOVSB
	POP	AX
	POP	CX
	POP	DI
	POP	SI
	POP	ES
	RET
ExeProg	ENDP

;------------- Add resident program to list ------------
;			   DX - number of paragraphs
AddProg	PROC	NEAR
	PUSH	DS
	PUSH	ES
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	CX
	PUSH	BX
	PUSH	AX
	STI
	CMP	CS:Counter,MaxProg
	JB	AdPrg1
	MOV	AX,0E07h
	INT	10h
	PUSH	CS
	POP	DS
	MOV	SI,OFFSET IntTab
	XOR	DI,DI
	MOV	ES,DI
	MOV	CX,2*256
	CLD
	CLI
	REP	MOVSW
	MOV	DI,4A8h
	MOVSW
	MOVSW
	STI
	STC
	JMP	AdPrg6
AdPrg1:	MOV	AH,62h			; Get current PSP
	INT	21h
	MOV	ES,BX
	CALL	FreeBlk
	MOV	BX,DX
	ADD	BX,(2+12+1+5*256+4+2 +15) SHR 4
	MOV	AH,4Ah
	INT	21h
	CMC
	JNC	AdPrg6
	MOV	AX,ES
	MOV	CX,ES
	ADD	CX,DX
	MOV	ES,CX
	PUSH	CS
	POP	DS
	CLD
	CLI
	XOR	DI,DI			; PSP -> PrgSeg
	STOSW
	MOV	SI,OFFSET NameBuf	; File name -> PrgSeg
	MOV	CX,12
	REP	MOVSB
	PUSH	DX			; Changed vectors -> PrgSeg
	PUSH	DI
	INC	DI
	XOR	BX,BX
	XOR	DX,DX
	MOV	DS,BX
	MOV	SI,OFFSET IntTab
AdPrg2:	LODS	CS:Wrd
	MOV	CX,AX
	LODS	CS:Wrd
	CMP	CX,DS:[BX].Offs
	JNE	AdPrg3
	CMP	AX,DS:[BX].Segm
	JE	AdPrg4
AdPrg3:	CMP	DL,22h
	JE	AdPrg4
	PUSH	AX			; Save vector
	MOV	AL,DL
	STOSB
	MOV	AX,CX
	STOSW
	POP	AX
	STOSW
	INC	DH
AdPrg4:	ADD	BX,4
	INC	DL
	JNE	AdPrg2
	POP	BX
	MOV	ES:[BX],DH
	PUSH	CS
	POP	DS
	POP	DX
	MOVSW				; SAVE PTR -> PrgSeg
	MOVSW
	MOV	CX,DI			; check sum -> PrgSeg
	XOR	SI,SI
	XOR	BX,BX
	MOV	AH,0
AdPrg5:	LODS	ES:Byt
	ADD	BX,AX
	LOOP	AdPrg5
	MOV	AX,BX
	STOSW
	MOV	BX,DS:Counter		; Save PrgSeg
	SHL	BX,1
	MOV	DS:PrgList[BX],ES
	INC	DS:Counter
	STI				; Add PrgSeg size
	DEC	DI
	MOV	CL,4
	SHR	DI,CL
	INC	DI
	ADD	DX,DI
AdPrg6:	POP	AX
	POP	BX
	POP	CX
	POP	DI
	POP	SI
	POP	BP
	POP	ES
	POP	DS
	RET
AddProg	ENDP

;------------------ Free memory block ------------------
;			   ES - segment for diallocation
FreeBlk	PROC	NEAR
	PUSH	DS
	PUSH	SI
	PUSH	DI
	PUSH	CX
	PUSH	AX
	CLD
	MOV	SI,OFFSET PrgList
	MOV	DI,SI
	MOV	CX,CS:Counter
	JCXZ	FreBl3
FreBl1:	MOV	DS,CS:[SI]
	CALL	ChkBlk
	LODS	CS:Wrd
	JNE	FreBl2
	PUSH	AX
	MOV	AX,ES
	CMP	AX,DS:[0]
	POP	AX
	JE	FreBl2
	MOV	CS:[DI],AX
	INC	DI
	INC	DI
FreBl2:	LOOP	FreBl1
	SUB	SI,DI
	SHR	SI,1
	SUB	CS:Counter,SI
FreBl3:	POP	AX
	POP	CX
	POP	DI
	POP	SI
	POP	DS
	RET
FreeBlk	ENDP

;---------------------- Check sum ----------------------
;			   DS - PrgSeg
ChkBlk	PROC	NEAR
	PUSH	SI
	PUSH	DI
	PUSH	CX
	MOV	AL,DS:[0Eh]
	MOV	AH,5
	MUL	AH
	ADD	AX,2+12+1+4
	MOV	CX,AX
	XOR	SI,SI
	XOR	DI,DI
	MOV	AH,0
ChkBl1:	LODSB
	ADD	DI,AX
	LOOP	ChkBl1
	CMP	DI,DS:[SI]
	POP	CX
	POP	DI
	POP	SI
	RET
ChkBlk	ENDP

;------------------- Release programs ------------------
;			   DS - PrgSeg
Release	PROC	NEAR
	PUSH	DS
	PUSH	ES
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	CALL	GetStat
	CLD
	CALL	ChkBlk			; Check sum
	JNE	Relea5
	XOR	AX,AX			; Restore vectors
	MOV	ES,AX
	MOV	SI,0Eh
	LODSB
	MOV	CX,AX
	CLI
	JCXZ	Relea2
Relea1:	LODSB
	MOV	DI,AX
	SHL	DI,1
	SHL	DI,1
	MOVSW
	MOVSW
	LOOP	Relea1
Relea2:	MOV	DI,4A8h			; Restore SAVE PTR
	MOVSW
	MOVSW
	STI
	MOV	SI,DS:[0]		; Release blocks
	MOV	AH,52h
	INT	21h
	MOV	DS,ES:[BX-2]
Relea3:	CMP	SI,DS:[1]
	JNE	Relea4
	MOV	AX,DS
	INC	AX
	MOV	ES,AX
	MOV	AH,49h
	INT	21h
Relea4:	MOV	AX,DS
	INC	AX
	ADD	AX,DS:[3]
	CMP	BYTE PTR DS:[0],'M'
	MOV	DS,AX
	JE	Relea3
Relea5:	CALL	SetStat
	POP	AX
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	POP	ES
	POP	DS
	RETF
Release	ENDP

LastRes:

;--------------------- Release data --------------------
IntTab	DD	256	DUP(?)
SavePtr	DD	?
Vect21	DD	?
Vect22	DD	?
Vect27	DD	?
Counter	DW	?
PrgList	DW	MaxProg	DUP(?)
NameBuf	DB	12	DUP(?)

;------------------ Data save segment ------------------
WCB1sav	DB	WCBsave + 13 * MinFls	DUP(?)	; WCB1
WCB2sav	DB	WCBsave + 13 * MinFls	DUP(?)	; WCB2
ComnIni	DB	CmnSave			DUP(?)	; Common data
DataSav	DB	LenStk			DUP(?)	; Local stack
VcStack:
LastDat:

;-------------------------- Data -----------------------
	.CONST
Title_S	DB	0FFh,14,'The ',PrgName,', Version ',VcVers,0
Suff_S	DB	' ',VcSuff,0
Copyr_S	DB	13,10, 'Copyright (C) 1991-2000 Vsevolod V. Volkov',0
CrLf_S	DB	13,10, '$'
Retrn_S	DB	13,10, 'Press ENTER to return ', 0
	DB	13,10, 'to the ',PrgName, 0
NoIni_S	DB	    'VC.INI is not correct.'
	DB	13,10, 0
Vers_S	DB	13,10, 'This program requires DOS 3.20 or later.'
	DB	13,10, 0
Comsp	DB	'COMSPEC='
NcEnv	DB	'VC='
TempEnv	DB	'TEMP='

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
	DB	'ъщ'			; StarTab
DefWCB	DB	0			; WinTyp
	DB	0			; BrfFul
	DB	1			; Visible
	DB	1			; Hidden
	DB	0			; MasTyp
	DB	8			; SortTyp
	DB	0			; Ctrl_L
	DB	0			; DskMas
FncTab	DW	4800h, F00_00		; Up
	DW	5000h, F01_00		; Down
	DW	4B00h, F02_00		; Left
	DW	4D00h, F03_00		; Right
	DW	4900h, F04_00		; PgUp
	DW	5100h, F05_00		; PgDn
	DW	4700h, F06_00		; Home
	DW	4F00h, F07_00		; End
	DW	5200h, F08_00		; Ins     - select / unselect
	DW	4E2Bh, F09_00		; Grey +  - select group
	DW	4A2Dh, F10_00		; Grey -  - unselect group
	DW	372Ah, F76_00		; Grey *  - invert group
	DW	0F09h, F20_00		; Tab     - change active panel
	DW	1C0Dh, F21_00		; Enter   - run / change directory
	DW	000Ah, F27_00		; ^Enter  - history command
	DW	011Bh, F74_00		; Esc     - clear command line / on/off
	DW	000Dh, F78_00		; ^M      - restore marks
	DW	000Fh, F11_00		; ^O      - on/off panels
	DW	0010h, F12_00		; ^P      - on/off unactive panel
	DW	5E00h, F13_00		; ^F1     - on/off left panel
	DW	5F00h, F14_00		; ^F2     - on/off right panel
	DW	0015h, F15_00		; ^U      - swap panels
	DW	0002h, F16_00		; ^B      - on/off key bar
	DW	0012h, F17_00		; ^R      - re-read
	DW	0006h, M16_00		; ^F      - filter
	DW	2308h, M16_10		; ^H      - hidden files
	DW	0001h, M30_00		; ^A      - attributes
	DW	0003h, M49_00		; ^C      - compare directories
	DW	000Ch, F18_00		; ^L      - info
	DW	001Ah, F79_00		; ^Z      - tree
	DW	8400h, F22_00		; ^PgUp   - CD ..
	DW	7600h, F23_00		; ^PgDn   - change directory
	DW	001Ch, F24_00		; ^\      - CD \
	DW	0005h, F25_00		; ^E      - previous history command
	DW	0018h, F26_00		; ^X      - next history command
	DW	0009h, F80_00		; ^I	  - insert files
	DW	001Bh, F81_00		; ^[	  - insert left path
	DW	001Dh, F82_00		; ^]	  - insert right path
	DW	4400h, F30_00		; F10     - quit
	DW	3C00h, F32_00		; F2      - user menu
	DW	3D00h, F33_00		; F3      - view
	DW	3E00h, F34_00		; F4      - edit
	DW	3F00h, F35_00		; F5      - copy
	DW	4000h, F36_00		; F6      - rename / move
	DW	4100h, F37_00		; F7      - make directory
	DW	4200h, F38_00		; F8      - delete
	DW	4300h, F39_00		; F9      - pull down menu
	DW	5D00h, F40_00		; Shift F10 - pull down menu
	DW	5500h, F42_00		; Shift F2  - main menu...
	DW	5600h, F43_00		; Shift F3  - view...
	DW	5700h, F44_00		; Shift F4  - edit...
	DW	5800h, F45_00		; Shift F5  - copy...
	DW	5900h, F46_00		; Shift F6  - rename/move...
	DW	5A00h, F47_00		; Shift F7  - make directory...
	DW	5B00h, F48_00		; Shift F8  - delete...
	DW	5C00h, M75_00		; Shift F9  - save setup
	DW	6100h, F54_00		; ^F4     - change label
	DW	6200h, F55_00		; ^F5     - copy file
	DW	6300h, F56_00		; ^F6     - rename/move file
	DW	6500h, F58_00		; ^F8     - delete file
	DW	6800h, F61_00		; Alt F1  - change left drive
	DW	6900h, F62_00		; Alt F2  - change right drive
	DW	6A00h, F63_00		; Alt F3  - internal viewer
	DW	6B00h, F64_00		; Alt F4  - internal editer
	DW	6C00h, F65_00		; Alt F5  - memory info
	DW	6D00h, M42_00		; Alt F6  - directory sizes
	DW	6E00h, F67_00		; Alt F7  - file find
	DW	6F00h, F68_00		; Alt F8  - history
	DW	7000h, F69_00		; Alt F9  - EGA lines
	DW	7100h, M41_00		; Alt F10 - Tree
	DW     0FD00h, F39_00		; Alt     - pull down menu
	DW     0FE00h, F75_00		;         - auto CD / quick view
	DW	0

;--------------------- Main module ---------------------
	.CODE
	ORG	LastRes

Alloc0:	CLD
	MOV	SI,100h
	MOV	CX,OFFSET Last-100h
	XOR	BX,BX
	XOR	DX,DX
	MOV	AH,0
Alloc1:	LODSB
	ADD	BX,AX
	LOOP	Alloc1
	MOV	SI,80h
	MOV	DI,OFFSET OvlPrm
	MOV	CL,DS:[DI]
	DEC	CX
	REPE	CMPSB
	JNE	Alloc3
	LODSW
	CMP	BYTE PTR DS:[SI],0Dh
	JNE	Alloc3
	MOV	ES,AX
	XCHG	BX,ES:ComnIni.LoadSiz
	MOV	ES,ES:RunPar
	MOV	SI,OFFSET Init00
	MOV	DI,SI
	MOV	CX,BX
	SUB	CX,SI
	REP	MOVSB
	MOV	AX,4C00h
	SUB	BX,OFFSET Last
	JE	Alloc2
	INC	AX
Alloc2:	INT	21h
Alloc3:	MOV	DS:ChkFile,BX
	MOV	AH,30h			; Check DOS version
	INT	21h
	CMP	AL,3
	JB	Alloc4
	XCHG	AL,AH
	CMP	AX,314h
	JAE	Alloc5
	MOV	SI,OFFSET Vers_S
	PUSH	CS
	CALL	OutStr0
	MOV	AX,4C02h
	INT	21h
Alloc4:	RETN
Alloc5:	XOR	BP,BP			; Allocate segments
	MOV	BX,((LastDat-Zero-1) SHR 4)+1
	MOV	DX,DS
	ADD	DX,BX
	INC	DX
	PUSH	DX
	MOV	DX,OFFSET Last-1
	MOV	SI,DX
	SUB	DX,(Init00-Zero) AND 0FFF0h
	MOV	DI,DX
	PUSH	DX
	MOV	CL,4
	SHR	DX,CL
	INC	DX
	POP	CX
	INC	CX
	PUSH	BX
	ADD	BX,DX
	INC	BX
	MOV	AH,4Ah
	INT	21h
	POP	BX
	POP	ES
	JNC	Alloc6
	XCHG	BX,DX			; Allocate transit block
	MOV	AH,48h
	INT	21h
	XCHG	BX,DX
	JC	Alloc9
	MOV	ES,AX
	STD
	REP	MOVSB
	CLD
	MOV	SP,OFFSET VcStack
	PUSH	ES
	PUSH	CS			; Compress resident block
	POP	ES
	MOV	AH,4Ah
	INT	21h
	POP	ES
	JMP	Alloc7
Alloc6:	STD
	CLI
	PUSH	CS
	POP	SS
	MOV	SP,OFFSET VcStack
	REP	MOVSB
	CLD

;-------- Split into resident and transit parts --------
	MOV	AX,ES
	DEC	AX
	PUSH	AX
	MOV	AX,CS
	DEC	AX
	MOV	DS,AX
	MOV	AL,'M'
	XCHG	AL,DS:[0]
	XCHG	BX,DS:[3]
	SUB	BX,DS:[3]
	DEC	BX
	POP	DS
	MOV	DS:[0],AL
	MOV	DS:[1],CS
	MOV	DS:[3],BX
	STI
	PUSH	CS
	POP	DS
Alloc7:	MOV	AX,ES
	SUB	AX,(Init00-Zero) SHR 4
	MOV	DS:RunPar,AX
	PUSH	AX
	CALL	SetStat
	MOV	AX,OFFSET Init00
	PUSH	AX
	RETF

Alloc9:	MOV	SI,OFFSET Aloc_S	; Error
	PUSH	CS
	CALL	OutStr0
	CALL	SetStat
	MOV	AX,4C01h
	INT	21h

;------------------ Read configuration -----------------
Init00:	MOV	ES,DS:[2Ch]
	MOV	DI,OFFSET WCB1sav + WCBsave
	MOV	DX,DI
	CALL	MainPth
	PUSH	DI
	PUSH	DX
	MOV	SI,OFFSET Init_F
	MOV	BP,OFFSET ComnIni - (3 + 2*WCBini)
	MOV	CX, 3 + 2*WCBini + CmnIni + 2 + 1
	CALL	ReadOne
	MOV	BX,0
	JC	Init03
	DEC	CX
	SUB	AX,CX
	JNE	Init02
	DEC	CX
	DEC	CX
	MOV	SI,BP
	XOR	DX,DX
Init01:	LODSB
	ADD	DX,AX
	LOOP	Init01
	LODSW
	CMP	AX,DX
	JNE	Init02
	MOV	DI,BP
	MOV	AL,'V'
	MOV	CL,3
	REPE	SCASB
	JNE	Init02
	MOV	SI,DI
	MOV	DI,OFFSET WCB1sav
	MOV	CL,WCBini
	PUSH	CX
	REP	MOVSB
	POP	CX
	MOV	DI,OFFSET WCB2sav
	REP	MOVSB
	JMP	Init04
Init02:	MOV	BL,1

;--------------------- Default init --------------------
Init03:	PUSH	DS
	PUSH	CS
	POP	DS
	MOV	SI,OFFSET Palets
	MOV	DI,OFFSET ComnIni
	MOV	CX,OFFSET ResIni
	REP	MOVSB
	MOV	CL,CmnIni-ResIni
	MOV	AL,0
	REP	STOSB
	MOV	SI,OFFSET DefWCB
	MOV	DI,OFFSET WCB1sav
	MOV	CL,WCBini
	REP	MOVSB
	POP	DS
	MOV	AH,19h
	INT	21h
	MOV	DL,AL
	INC	DX
	ADD	AL,'A'
	MOV	DI,OFFSET WCB1sav.WinPath
	STOSB
	MOV	AX,'\:'
	STOSW
	MOV	SI,DI
	MOV	AL,0
	STOSB
	MOV	AH,47h
	INT	21h
	MOV	SI,OFFSET WCB1sav
	MOV	DI,OFFSET WCB2sav
	MOV	CX,WCBini
	REP	MOVSB
	MOV	DS:WCB1sav.Visible,CL

;--------------------- Output Report -------------------
Init04:	CALL	ChkMode
	JZ	Init05
	MOV	AX,3
	INT	10h
Init05:	PUSH	CS			; Output message
	POP	DS
	MOV	DX,OFFSET ComnIni.ExtMain
	MOV	DI,DX
	MOV	SI,OFFSET Title_S+2
	CALL	MvzLin0
	MOV	SI,OFFSET Suff_S
	CALL	MvzLin0
	MOV	SI,OFFSET Copyr_S
	CALL	MvzLin0
	MOV	SI,OFFSET CrLf_S
	CALL	MvzLin0
	PUSH	ES
	POP	DS
	MOV	AH,9
	INT	21h
	OR	BL,BL			; Error in INI file
	JE	Init09
	MOV	SI,OFFSET NoIni_S
	CALL	OutStr1

;--------------- Read main extension file --------------
Init09:	POP	DX
	POP	DI
	MOV	SI,OFFSET Ext_F
	MOV	BP,OFFSET ComnIni.ExtMain
	MOV	CX,LenExt-1
	CALL	ReadOne
	JC	Init10
	ADD	BP,AX
Init10:	MOV	BYTE PTR DS:[BP],1Ah

;---------------- Initialize main data -----------------
	MOV	BX,OFFSET ComnIni
	MOV	SI,OFFSET WCB1sav
	MOV	DI,OFFSET WCB2sav
	MOV	AX,OFFSET DskBuf
	MOV	DS:[SI].CurAdr,AX
	MOV	DS:[DI].CurAdr,AX
	MOV	DS:[SI].First,AX
	MOV	DS:[DI].First,AX
	MOV	DS:[SI].EndBuf,AX
	MOV	DS:[DI].EndBuf,AX
	XOR	AX,AX
	MOV	DS:Counter,AX
	MOV	DS:[BX].CrnPath,AL
	MOV	DS:[BX].ViewFil,AL
	MOV	DS:[BX].ToFile,AL
	MOV	DS:[BX].SrchStr,AL
	MOV	DS:[BX].HstBuf.Wrd,AX
	MOV	DS:[BX].LastMnu,1
	DEC	AX
	MOV	DS:[BX].MouseX,AX
	MOV	DS:[BX].MouseY,AX
	MOV	DS:[BX].LoadRes,AL
	MOV	AL,DS:[BX].AutoMnu
	MOV	DS:[BX].MenuFl,AL
	PUSH	DS
	PUSH	CS
	POP	DS
	MOV	SI,OFFSET Mask_S+1
	LEA	DI,[BX].SelMas
	MOV	CX,4
	REP	MOVSB
	MOV	SI,OFFSET Mask_S
	LEA	DI,[BX].FindStr
	MOV	CL,5
	REP	MOVSB
	POP	DS
	MOV	AX,OFFSET SetBrk
	CALL	CallPSP
	MOV	AX,OFFSET Exit01
	XCHG	AX,DS:[0Ah].Offs
	MOV	DS:Vect22.Offs,AX
	MOV	AX,DS
	XCHG	AX,DS:[0Ah].Segm
	MOV	DS:Vect22.Segm,AX
	CMP	DS:[BX].ChkTSR,0
	JZ	Init11
	MOV	AX,3521h
	INT	21h
	MOV	DS:Vect21.Offs,BX
	MOV	DS:Vect21.Segm,ES
	XOR	AX,AX
	MOV	ES,AX
	CLI
	MOV	ES:[4*21h].Offs,OFFSET New21
	MOV	ES:[4*21h].Segm,DS
	STI
	MOV	AX,3527h
	INT	21h
	MOV	DS:Vect27.Offs,BX
	MOV	DS:Vect27.Segm,ES
	MOV	DX,OFFSET New27
	MOV	AH,25h
	INT	21h

;------------ Allocate Secondary Data Segment ----------
Init11:	MOV	AX,CS
	ADD	AX,(Init00-Zero) SHR 4
	MOV	ES,AX
	MOV	BX,0FFFFh
	MOV	AH,4Ah
	INT	21h
	MOV	SI,BX
	CMP	AX,8
	JE	Init12
	XOR	SI,SI
Init12:	MOV	CL,4
	MOV	BX,OFFSET Last-1
	SHR	BX,CL
	SUB	BX,((Init00-Zero) SHR 4)-1
	MOV	AH,4Ah
	INT	21h
	PUSH	DS
	POP	ES
	MOV	BX,0FFFFh
	MOV	AH,48h
	INT	21h
	MOV	AH,48h
	INT	21h
	MOV	ES,AX
	MOV	DX,((SIZE MainDat-1) SHR 4)+1
	MOV	DI,((SIZE WCBdef -1) SHR 4)+1
	MOV	AX,((   MinView  -1) SHR 4)+1
	ADD	AX,DX
	ADD	AX,DI
	ADD	AX,DI
	CMP	AX,BX
	JBE	Init13
	MOV	AX,OFFSET AlocEr
	PUSH	SS
	PUSH	AX
	RETF
Init13:	MOV	ES:FreMem2,BX
	CMP	SI,BX
	JAE	Init14
	MOV	SI,BX
Init14:	MOV	ES:FreMem1,SI
	MOV	AX,ES
	ADD	AX,DX
	MOV	ES:WCB1seg,AX
	ADD	AX,DI
	MOV	ES:WCB2seg,AX
	ADD	AX,DI
	MOV	ES:ViewSeg,AX
	MOV	CS:SecDat,ES

;------------------ Restore data segment ---------------
	CLD
	XOR	DI,DI
	MOV	SI,OFFSET ComnIni
	MOV	CX,CmnSave
	REP	MOVSB
	PUSH	ES
	MOV	ES,ES:WCB1seg
	MOV	SI,OFFSET WCB1sav
	CALL	Expand
	POP	ES
	PUSH	ES
	MOV	ES,ES:WCB2seg
	MOV	SI,OFFSET WCB2sav
	CALL	Expand
	POP	DS

;------------------- Set Error Vectors -----------------
	CALL	HookVct
	PUSH	ES
	POP	DS

;------------------ Set Display Segment ----------------
	CALL	ChkMode
	JZ	Init17
	MOV	BL,AH
	MOV	CX,2000h
	MOV	AH,1
	INT	10h
	PUSH	DS
	MOV	SI,OFFSET Retrn_S
	CALL	OutStr1
	INC	SI
	CMP	BL,80
	JB	Init15
	INC	SI
	INC	SI
Init15:	CALL	OutStr1
	POP	DS
Init16:	MOV	AH,0
	INT	16h
	CMP	AL,0Dh
	JNE	Init16
	MOV	AL,DS:ScrMod
	MOV	AH,0
	INT	10h
	MOV	AH,0Fh
	INT	10h
	AND	AL,7Fh
Init17:	MOV	DS:ScrMod,AL
	MOV	DS:ScrPage,BH

;------------------ Correct cursor size ----------------
	MOV	AH,3
	INT	10h
	CMP	CH,0Fh
	JBE	Init18
	MOV	CX,607h
Init18:	CMP	CX,67h
	JNE	Init19
	MOV	CX,607h
Init19:	CMP	DS:ScrMod,7
	JNE	Init20
	CMP	CX,607h
	JNE	Init20
	MOV	CX,0B0Ch
Init20:	MOV	AH,1
	INT	10h

;---------------- Initialize data segment --------------
IniSave	EQU	WCB1sav

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
	AND	DS:LoadFl,7Fh
	MOV	AX,SS:[2]		; Memorys
	MOV	DS:Memorys,AX
	XOR	AX,AX
	MOV	SS:IniSave,AL
	MOV	DS:DosBuf,AL		; DosBuf
	MOV	BX,OFFSET EPB		; EPB
	MOV	DS:[BX].EnvSeg,AX
	MOV	DS:[BX].FCB1.Offs,5Ch
	MOV	DS:[BX].FCB1.Segm,ES
	MOV	DS:[BX].FCB2.Offs,6Ch
	MOV	DS:[BX].FCB2.Segm,ES
	MOV	ES,SS:[2Ch]		; ComSpec
	PUSH	DS
	PUSH	CS
	POP	DS
	XOR	DI,DI
Init24:	CMP	AL,ES:[DI]
	JE	Init25
	MOV	SI,OFFSET Comsp
	MOV	CX,8
	REPE	CMPSB
	JE	Init26
	DEC	DI
	MOV	CH,0FFh
	REPNE	SCASB
	JMP	Init24
Init25:	XOR	DI,DI
Init26:	POP	DS
	MOV	DS:ComSpec,DI
	XOR	DI,DI			; OvlPth
Init27:	CMP	AL,ES:[DI]
	JE	Init28
	MOV	CH,0FFh
	REPNE	SCASB
	JMP	Init27
Init28:	ADD	DI,3
	MOV	DS:OvlPth,DI
	MOV	DI,OFFSET NcPath	; NcPath
	PUSH	ES
	PUSH	DI
	CALL	MainPth
	POP	SI
	CALL	PathLin
	POP	ES
	MOV	DI,OFFSET TempDir	; TempDir
	MOV	SI,OFFSET TempEnv
	MOV	CX,5
	CALL	MnPth0
	XOR	AX,AX
	MOV	DS:DosCol,AL		; DosCol
	MOV	DS:BegCol,AL		; BegCol
	MOV	DS:OutCol,AL		; OutCol
	MOV	DS:Quote,AL		; Quote
	MOV	DS:SftErr,AL		; SftErr
	MOV	DS:BlocEr,AL		; BlocEr
	MOV	DS:TstEGA,AL		; TstEGA
	MOV	DS:WinMous,AL		; WinMous
	MOV	DS:WinLeav,AL		; WinLeav
	MOV	DS:HstAdr,OFFSET HstBuf	; HstAdr
	MOV	DS:BarPrev,0FFh		; BarPrev
	MOV	AH,19h			; DosChg
	INT	21h
	MOV	DS:DosChg[0],AL
	MOV	AH,12h			; TstEGA
	MOV	BX,0FF10h
	INT	10h
	CMP	BX,0FF10h
	JE	Init29
	MOV	DS:TstEGA,1
Init29:	MOV	AL,25			; Lines
	MOV	DL,0
	CMP	DS:TstEGA,DL
	JE	Init30
	PUSH	ES
	PUSH	AX
	MOV	BH,0
	MOV	AX,1130h
	INT	10h
	POP	AX
	POP	ES
	OR	DL,DL
	JE	Init30
	INC	DX
	MOV	AL,MaxLins
	CMP	AL,DL
	JBE	Init30
	MOV	AL,DL
Init30:	MOV	DS:Lines,AL
	MOV	AH,1			; Snow
	CMP	DS:ScrMod,7
	JE	Init33
	CMP	DS:SnowMod,1
	JE	Init33
	MOV	DX,03DAh
	MOV	BX,8000h
	JB	Init31
	CMP	DS:TstEGA,BL
	JNE	Init33
Init31:	STI
	DEC	BX
	JE	Init33
	MOV	CX,7			; Const2_1 ???
	CLI
Init32:	IN	AL,DX
	TEST	AL,1
	JZ	Init31
	LOOP	Init32
	MOV	AH,0
Init33:	STI
	MOV	DS:Snow,AH
	CALL	ScrOffs			; Screen Offset
	MOV	BX,0B800h		; Screen Segment
	CMP	DS:ScrMod,7
	JNE	Init34
	MOV	BH,0B0h
Init34:	MOV	ES,BX			; Virtual screen buffer
	MOV	AH,0FEh
	INT	10h
	MOV	CX,ES
	PUSH	DS
	POP	ES
	MOV	AL,0
	CMP	CX,BX
	JE	Init35
	MOV	AL,1
	MOV	DS:Snow,AL
Init35:	MOV	DS:Screen.Segm,CX
	MOV	DS:TopView,AL
	MOV	CX,'DE'			; Check DESQWIEV
	MOV	DX,'SQ'
	MOV	AX,2B01h
	INT	21h
	INC	AX
	MOV	DS:DesqVie,AL
	XOR	BX,BX			; VerDOS
	MOV	AX,3306h
	INT	21h
	XCHG	AX,BX
	OR	AX,AX
	JNE	Init36
	MOV	AH,30h
	INT	21h
Init36:	CMP	AX,4
	JNE	Init37
	CMP	BH,0FFh
	JNE	Init37
	MOV	AH,1
Init37:	MOV	DS:VerDOS,AX
	MOV	SI,DS:WCB1seg		; WCBseg
	MOV	DI,DS:WCB2seg
	PUSH	DS
	MOV	DS,SI
	MOV	DS:ChgFlg,1
	MOV	DS,DI
	MOV	DS:ChgFlg,1
	POP	DS
	CMP	DS:Active,0
	JE	Init38
	XCHG	DI,SI
Init38:	MOV	DS:WCBcrn,SI
	MOV	DS:WCBsec,DI

;---------------------- Set DOS color ------------------
	CALL	CurNl
	MOV	AX,0E0Dh
	INT	10h
	MOV	AX,0E0Ah
	INT	10h
	CALL	CurNl
	MOV	AH,8
	INT	10h
	PUSH	AX
	MOV	DL,' '
	MOV	AH,6
	INT	21h
	MOV	AX,0E0Dh
	INT	10h
	MOV	AH,8
	INT	10h
	MOV	DS:CrntCol,AH
	POP	AX
	MOV	BL,AH
	MOV	CX,1
	MOV	AH,9
	INT	10h

;----------------------- Set cursor --------------------
Init40:	PUSH	ES
	POP	DS
	MOV	AH,0Fh
	INT	10h
	MOV	AH,3
	INT	10h
	CMP	DS:KeyBar,0
	JE	Init41
	MOV	AL,DS:Lines
	DEC	AX
	CMP	DH,AL
	JB	Init41
	MOV	DH,AL
	DEC	DH
	MOV	AX,0E0Ah
	INT	10h
Init41:	MOV	AL,DH
	MOV	DS:MenuBuf[0],AL
	PUSH	DS
	MOV	DS,DS:WCBcrn
	CMP	DS:Visible,0
	POP	DS
	JE	Init42
	CALL	BegBX
	CALL	SizDX
	ADD	DH,BH
	CMP	AL,DH
	JAE	Init42
	MOV	AL,DH
Init42:	MOV	DS:DosLin,AL
	CALL	HidCur

;--------------------- Save Screen ---------------------
	CALL	SavScr
	CALL	IniMous

;------------------ Save Main Windows ------------------
	MOV	SI,OFFSET ScrBuf
	MOV	DI,OFFSET UserScr
	MOV	AL,DS:Lines
	DEC	AX
	MOV	AH,80
	MUL	AH
	MOV	CX,AX
	REP	MOVSW

;----------------------- Auto file ---------------------
	CMP	DS:LoadRes,0FFh
	JNE	Init49
	MOV	DS:LoadRes,0
	PUSH	SS
	POP	DS
	MOV	SI,81h
	CALL	FndLet
	CALL	Init46
	JE	Init49
	MOV	DI,OFFSET DosBuf
Init43:	LODSB
	CALL	Init46
	JE	Init45
	CMP	AL,9
	JNE	Init44
	MOV	AL,' '
Init44:	STOSB
	JMP	Init43
Init45:	PUSH	ES
	POP	DS
	XOR	AX,AX
	STOSW
	MOV	AL,DS:MenuBuf[0]
	DEC	AX
	MOV	DS:DosLin,AL
	MOV	AL,81h
	XCHG	AL,DS:LoadFl
	MOV	DS:LoadRes,AL
	JMP	F21_31

Init46	PROC	NEAR
	CMP	AL,0
	JE	Init47
	CMP	AL,0Dh
	JE	Init47
	CMP	AL,0Ah
Init47:	RET
Init46	ENDP

Mvz2lin	PROC	NEAR
	PUSH	DS
	CALL	MvzLin0
	PUSH	ES
	POP	DS
	MOV	SI,BP
	CALL	MvzLin0
	POP	DS
	RET
Mvz2lin	ENDP

CurNl	PROC	NEAR
	MOV	AH,0Fh
	INT	10h
	MOV	AH,3
	INT	10h
	MOV	AH,DS:Lines
	CMP	DH,AH
	JB	Init47
	DEC	AH
	MOV	AL,0
	JMP	Cursor
CurNl	ENDP

Init49:

;---------------- Output menu & key bar ----------------
	MOV	AL,0
	CALL	PutMnu
	MOV	AL,0FFh
	CALL	PutKey

;--------------------- Main windows --------------------
Init50:	CALL	HidCur
	MOV	SI,ES:WCBcrn
	MOV	DI,ES:WCBsec
	MOV	DS,DI
	MOV	AL,DS:WinTyp
	MOV	DS,SI
	CMP	AL,DS:WinTyp
	JAE	Init51
	MOV	DS,DI
	XCHG	SI,DI
Init51:	XOR	BX,BX
	MOV	DH,ES:Lines
	MOV	DL,80
	MOV	CL,0
	CMP	DS:ChgFlg,CL
	JE	Init53
Init52:	MOV	ES:BlocEr,1
	CALL	SetPth
	MOV	ES:BlocEr,0
Init53:	CALL	PutPrm
	XOR	AX,AX
	CMP	DS:Visible,AL
	JNZ	Init54
	JMP	Init70
Init54:	CALL	Panel
	CMP	DS:ChgFlg,AL
	JE	Init60
	CALL	Window
	CMP	DS:WinTyp,AL
	PUSH	DS
	PUSH	ES
	POP	DS
	POP	ES
	JNE	Init55
	PUSH	SI
	PUSH	CX
	MOV	SI,OFFSET DosPth
	MOV	DI,OFFSET WinPath
	CALL	StrLen
	PUSH	SI
	PUSH	DI
	REPE	CMPSB
	POP	DI
	POP	SI
	MOV	CX,LenPath
	REP	MOVSB
	POP	CX
	POP	SI
	MOV	DS:OldFlg,AL
	JNE	Init55
	MOV	DS:OldFlg,1
Init55:	CMP	DS:PathEr,AL
	PUSH	DS
	PUSH	ES
	POP	DS
	POP	ES
	JE	Init56
	CALL	Init90
	JMP	Init59
Init56:	OR	CL,CL
	JNE	Init57
	CMP	ES:OldFlg,AL
	JE	Init57
	MOV	AL,1
Init57:	CMP	DS:WinTyp,0
	JNE	Init58
	MOV	ES:BlocEr,1
Init58:	CALL	ReDir
	MOV	ES:BlocEr,0
	JNC	Init60
Init59:	MOV	AH,1
	OR	CL,CL
	JNE	Init60
	MOV	CL,DS:WinPath[0]
Init60:	CALL	PutPath
	CALL	PutFls
	OR	AH,AH
	JE	Init62
	CALL	Window
	CMP	DS:WinTyp,0
	JNE	Init70
	MOV	AL,DS:WinPath[0]
	CALL	ErrRdy
	CMP	AL,0
	JE	Init70
	OR	CL,CL
	JNE	Init61
	MOV	CL,AL
Init61:	SUB	AL,'A'
	MOV	DL,AL
	MOV	AH,0Eh
	INT	21h
	JMP	Init52
Init62:	CMP	CL,DS:WinPath[0]
	JNE	Init70
	MOV	CL,0
Init70:	CALL	InvWCB
	MOV	AL,1
	CMP	DS:Visible,0
	JNE	Init71
	JMP	Init80
Init71:	CALL	Panel
	MOV	AH,0
	CMP	DS:ChgFlg,AH
	JE	Init77
	CALL	Window
	CMP	DS:WinTyp,AH
	JNE	Init72
	CMP	CL,DS:WinPath[0]
	JNE	Init74
	MOV	DS:WinPath[3],AH
	JMP	Init73
Init72:	CMP	ES:PathEr,AH
	JE	Init75
Init73:	CALL	Init90
	JMP	Init76
Init74:	MOV	ES:BlocEr,1
Init75:	CALL	ReDir
	MOV	ES:BlocEr,0
	JNC	Init77
Init76:	MOV	AH,1
Init77:	CALL	PutPath
	CALL	PutFls
	CMP	DS:WinTyp,0
	JNE	Init80
	OR	AH,AH
	JE	Init80
	CALL	Window
	MOV	AL,DS:WinPath[0]
	CALL	ErrRdy
	CMP	AL,0
	JE	Init80
	MOV	SI,OFFSET WinPath[3]
	MOV	BYTE PTR DS:[SI],0
	MOV	CL,AL
	MOV	DS:WinPath[0],AL
	SUB	AL,'A'-1
	MOV	DL,AL
	CALL	Fantom
	JC	Init78
	MOV	AH,47h
	MOV	ES:BlocEr,1
	CALL	Intr21
	MOV	ES:BlocEr,0
	JC	Init78
	MOV	CL,0
Init78:	MOV	AL,0
	JMP	Init71
Init80:	MOV	DS,ES:WCBcrn
	JMP	Func02

Init90	PROC	NEAR
	PUSH	DS
	PUSH	AX
	XOR	AX,AX
	CMP	DS:WinTyp,AL
	JNE	Init91
	MOV	AX,OFFSET DskBuf
	MOV	DS:EndBuf,AX
	MOV	DS:EndFul,AX
	MOV	DS:CurAdr,AX
	MOV	DS:First,AX
	JMP	Init93
Init91:	CMP	DS:WinTyp,2
	PUSH	ES
	POP	DS
	JNE	Init92
	MOV	DS:Total,AX
	MOV	DS:FreeDsk,AX
	MOV	DS:ClstSiz,1
	MOV	DS:Volume[0],AL
	DEC	AX
	MOV	DS:InfoLen,AX
	JMP	Init93
Init92:	JA	Init93			; ???
	MOV	DI,OFFSET TreeBuf.TreList
	PUSH	DI
	PUSH	CX
	MOV	CX,16
	REP	STOSB
	POP	CX
	POP	DI
	MOV	DS:[DI].DirName,'\'
	MOV	DS:[DI].DirShift,1
	MOV	DS:TreeBuf.TreeLen,1
	MOV	AX,OFFSET TreeBuf.TreList
	MOV	DS:TreFrst,AX
	MOV	DS:TreAdr,AX
Init93:	POP	AX
	POP	DS
	RET
Init90	ENDP

OutStr1	PROC	NEAR
	PUSH	CS
	POP	DS
	MOV	AX,OFFSET OutStr
	JMP	CallPSP
OutStr1	ENDP

;-------------------------------------------------------
Func01:	CALL	Panel
	CALL	PutPath
	CALL	PutFls
Func02:	XOR	BX,BX
	MOV	DH,ES:Lines
	MOV	DL,80
Func03:	CALL	Window
Func04:	XOR	AX,AX
	CALL	PutMnu
	PUSH	ES
	POP	DS
	MOV	DS:OneFile,AL
	MOV	DS:FlagCD,AL
	MOV	DS:MousEnt,AL
	DEC	AX
	MOV	DS:MousTim.Hi,AX
Func05:	CALL	MakeIni
	PUSH	SS
	POP	ES
	MOV	DI,OFFSET IniSave
	CMP	WORD PTR ES:[DI],'VV'
	JNE	Func06
	PUSH	CX
	MOV	AX,OFFSET [3].WinPath
	MOV	CX,LenPath
	PUSH	CX
	CALL	Func15
	POP	CX
	MOV	AX,OFFSET [3+WCBini].WinPath
	CALL	Func15
	POP	CX
	MOV	BX,OFFSET [3+2*WCBini].Active
	MOV	AL,DS:[SI+BX]
	MOV	ES:[DI+BX],AL
	MOV	BX,OFFSET [3+2*WCBini].Ctrl_O
	MOV	AL,DS:[SI+BX]
	MOV	ES:[DI+BX],AL
	PUSH	SI
	PUSH	DI
	PUSH	CX
	DEC	CX
	DEC	CX
	REPE	CMPSB
	POP	CX
	POP	DI
	POP	SI
	JE	Func06
	CMP	DS:AutoSav,0
	JZ	Func06
	PUSH	DS
	POP	ES
	CALL	HidCur
	MOV	DS:BlocEr,1
	CALL	SaveSet
	MOV	DS:BlocEr,0
Func06:	PUSH	SS
	POP	ES
	REP	MOVSB
	PUSH	DS
	POP	ES
Func07:	CALL	Cursor1
	MOV	DS,ES:WCBcrn
	PUSH	DS
	XOR	AX,AX
	CMP	ES:AutoCD,AL
	JZ	Func09
	CMP	DS:WinTyp,1
	JNE	Func09
	CMP	DS:Visible,AL
	JZ	Func09
	PUSH	ES
	POP	DS
	MOV	DI,OFFSET DosPth
	CALL	StrLen
	MOV	SI,OFFSET CrnPath
	REPE	CMPSB
	JE	Func09
	CMP	DS:FlagCD,AL
	JZ	Func09
	MOV	AH,2Ch
	INT	21h
	ADD	DL,DS:DelayTm[0]
	CMP	DL,100
	JB	Func08
	SUB	DL,100
	INC	DH
Func08:	ADD	DH,DS:DelayTm[1]
	CMP	DH,60
	JB	Func10
	SUB	DH,60
	INC	CX
	CMP	CL,60
	JB	Func10
	SUB	CL,60
	INC	CH
	CMP	CH,24
	JB	Func10
	MOV	CH,0
	JMP	Func10
Func09:	MOV	CX,0FFFFh
Func10:	POP	DS
	MOV	ES:AutoTim.Lo,CX
	MOV	ES:AutoTim.Hi,DX
	XOR	CX,CX
	MOV	ES:HlpPage,CL
	XCHG	CH,ES:MenuFl
	MOV	AX,3C00h
	OR	CH,CH
	JNE	Func12
	CALL	GetMous
	JNZ	Func21
	MOV	AL,0
	MOV	ES:WinMous,AL
	MOV	ES:MousEnt,AL
	CMP	ES:TstEGA,AL
	JNE	Func11
	MOV	AL,9
Func11:	CALL	Input
	CMP	AH,0FCh
	JE	Func20
	CMP	DS:Visible,0
	JZ	Func12
	CMP	DS:WinTyp,1
	JA	Func12
	CMP	AL,0
	JNE	Func12
	XCHG	AL,AH
	PUSH	ES
	PUSH	CS
	POP	ES
	MOV	DI,OFFSET AltSr_T
	MOV	CX,12+10+9+7
	REPNE	SCASB
	POP	ES
	XCHG	AL,AH
	JNE	Func12
	JMP	F77_00
Func12:	CMP	AH,0FCh
	JE	Func20
	CMP	ES:Quote,0
	JNZ	Func14
	MOV	SI,OFFSET FncTab
	CALL	Case
Func14:	JMP	F73_00

Func15	PROC	NEAR
	PUSH	SI
	PUSH	DI
	ADD	SI,AX
	ADD	DI,AX
	REP	MOVSB
	POP	DI
	POP	SI
	RET
Func15	ENDP

Func20:	CALL	GetMous			; Mouse support
Func21:	CALL	ClrKbd
	CMP	ES:WinMous,0
	JNZ	Func30
	CMP	CX,2			; Panels on/off
	JAE	Func25
	MOV	BL,0
Func22:	CALL	GetMous
	JZ	Func24
	MOV	AL,0
	CMP	CX,2
	JAE	Func23
	MOV	AL,'ю'
Func23:	CMP	AL,BL
	JE	Func22
	MOV	BL,AL
	CALL	TypMous
	JMP	Func22
Func24:	MOV	AL,0
	CALL	TypMous
	OR	BL,BL
	JE	Func35
	JMP	F11_00

Func25:	OR	CH,CH			; Pull down menu
	JNE	Func26
	JMP	F39_00
Func26:	CALL	BegBX
	CALL	SizDX
	ADD	DH,BH
	CMP	CH,DH
	JAE	Func35

	CMP	CL,40			; Change panel
	JAE	Func27
	MOV	DS,ES:WCB1seg
	CMP	ES:Active,0
	JE	Func30
	JMP	Func28
Func27:	MOV	DS,ES:WCB2seg
	CMP	ES:Active,0
	JNE	Func30
Func28:	MOV	DS,ES:WCBcrn
	JMP	F20_00

Func30:	TEST	AL,3
	JZ	Func35
	CMP	DS:Visible,0
	JZ	Func35
	CMP	DS:WinTyp,2
	JB	Func40
	JNE	Func35
	CALL	BegBX			; Edit 'dirinfo'
	CALL	SizDX
	ADD	BX,0B01h
	SUB	DX,0C02h
	CALL	InWind
	JC	Func35
	JMP	F34_10
Func35:	JMP	Func07

TmpMous	EQU	WORD PTR TmpBuf[0]

Func40:	OR	ES:WinMous,1		; Move the cursor
	MOV	ES:TmpMous,CX
	PUSH	AX
	CALL	AdrNum
	POP	BP
	CALL	BegBX
	CMP	CL,BL
	JB	Func35
	PUSH	BX
	ADD	BL,40
	CMP	CL,BL
	POP	BX
	JAE	Func35
	INC	BH
	CMP	DS:WinTyp,0
	JNE	Func41
	INC	BH
Func41:	SUB	CH,BH
	JB	Func50
	CMP	CH,AL
	JAE	Func55
	SUB	CL,BL			; Set cursor position
	CMP	CL,39
	JAE	Func35
	MOV	BL,DS:WinTyp
	OR	BL,DS:BrfFul
	JNE	Func42
	XCHG	AL,CL
	CBW
	MOV	BL,13
	DIV	BL
	XCHG	AH,CL
	MUL	AH
	ADD	CH,AL
Func42:	OR	CL,CL
	JE	Func35
	MOV	AL,CH
	CBW
	ADD	AX,DI
	MOV	SI,AX
	CMP	SI,DX
	JBE	Func43
	MOV	SI,DX
	JMP	Func45
Func43:	TEST	BP,1
	JNZ	Func60
Func45:	JMP	F00_60

Func50:	MOV	SI,DI			; Scroll up
	JMP	F00_05

Func55:	MOV	BL,DS:WinTyp		; Scroll down
	OR	BL,DS:BrfFul
	JNE	Func56
	MOV	AH,3
	MUL	AH
Func56:	DEC	AX
	MOV	SI,DI
	ADD	SI,AX
	CMP	SI,DX
	JBE	Func57
	MOV	SI,DX
Func57:	JMP	F01_05

Func60:	MOV	CX,ES:TmpMous		; Double click
	CALL	BegBX
	INC	BX
	MOV	BH,CH
	MOV	DX,126h
	MOV	CH,0
	MOV	AL,DS:WinTyp
	OR	AL,DS:BrfFul
	JNE	Func61
	MOV	AL,CL
	SUB	AL,BL
	CBW
	MOV	CL,13
	DIV	CL
	MUL	CL
	ADD	BL,AL
	MOV	DL,12
Func61:	CALL	DoublMo
	JC	Func45
	JNE	Func62
	JMP	Func04
Func62:	MOV	AL,0
	CALL	TypMous
	JMP	F21_10

	INCLUDE	VCFUNC.INC
	INCLUDE	VCMENU.INC
	INCLUDE	VCF1.INC
	INCLUDE	VCCOPY.INC
	INCLUDE	VCVIEW.INC
	INCLUDE	VCEDIT.INC
	INCLUDE	VCHELP.INC
	INCLUDE	VCMEM.INC
	INCLUDE	VCFIND.INC

;------------------------ Exit -------------------------
Exit:	CALL	RstVc1
	CALL	Quit
	MOV	BH,DS:DosLin
	CALL	ClsLine
	XOR	BX,BX
	MOV	DH,DS:Lines
	MOV	DL,80
	CALL	Window
	MOV	AL,0
	CALL	CurMous
	MOV	AH,DS:DosLin
	DEC	AH
	CALL	Cursor
	MOV	AX,OFFSET Exit00
	PUSH	SS
	PUSH	AX
	MOV	AL,0
	RETF

;------------------------ Quit -------------------------
Quit	PROC	NEAR
	PUSH	ES
	POP	DS
	MOV	BH,DS:Lines
	DEC	BH
	CMP	DS:KeyBar,0
	JE	Quit01
	CALL	ClsLine
Quit01:	MOV	DH,BH
	XOR	BX,BX
	MOV	DL,80
	CALL	ResUser
	RET
Quit	ENDP

;--------------- Restore and Hook vectots --------------
RestVct	PROC	NEAR
	LDS	DX,ES:ErrVct
	MOV	AX,2524h
	INT	21h
RstVc1:	LDS	DX,ES:BrkVct
	MOV	AX,251Bh
	INT	21h
	RET
RestVct	ENDP

HookVct	PROC	NEAR
	PUSH	DS
	MOV	AX,3524h
	INT	21h
	MOV	DS:ErrVct.Offs,BX
	MOV	DS:ErrVct.Segm,ES
	MOV	AX,351Bh
	INT	21h
	MOV	DS:BrkVct.Offs,BX
	MOV	DS:BrkVct.Segm,ES
	PUSH	CS
	POP	DS
	MOV	DX,OFFSET FatErr
	MOV	AX,2524h
	INT	21h
	PUSH	SS
	POP	DS
	MOV	DX,OFFSET CtrlC
	MOV	AX,251Bh
	INT	21h
	POP	ES
	RET
HookVct	ENDP

;--------------------- Fatal error ---------------------
	.CONST
Err_S	DB	' Error ',0
OnDr_S	DB	'on drive A',0
OnDv_S	DB	'on device '
Errs_S	DB	'Attempt to write on write-protected disk.',0
	DB	'Unknown disk drive.',0
	DB	'Drive or device not ready.',0
	DB	'Unknown command.',0
	DB	'Data error.',0
	DB	'Bad request.',0
	DB	'Seek error.',0
	DB	'Non-DOS disk.',0
	DB	'Sector not found.',0
	DB	'Printer out of paper.',0
	DB	'Write fault.',0
	DB	'Read fault.',0
	DB	'General failure.',0
	DB	'Sharing violation.',0
	DB	'Lock violation.',0
	DB	'Invalid disk change.',0
	DB	'FCB unavailable.',0
	DB	'Sharing buffer overflow.',0
	DB	'Code page mismatch.',0
	DB	'Out of input.',0
	DB	'Insufficient disk space.',0
	DB	'Unknown error.',0
FatEr_S	DB	'File allocation table bad.',0
RtAb_S	DB	' Retry   Abort ',0
Ok_S	DB	' Ok ',0
Var_S	DB	'^V',0
Inc_S	DB	'^I',0

	.DATA
RtAb_D	DB	7, 0
	DB	7, 8
	DB	0
FtEr1_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 1
	DW	ErrBuf1
	DW	TmpErr+1
	DB	0, 70
	DW	Err_S
	DW	Var_S
	DW	Inc_S
	DW	RtAb_S
FtEr2_P	DB	Err_C, ErInv_C, ErInv_C, 0
	DB	4, 1
	DW	ErrBuf1
	DW	TmpErr+1
	DB	0, 70
	DW	Err_S
	DW	Var_S
	DW	Inc_S
	DW	Ok_S
FtEr1_B	BOXHD	< FtEr1_P, 928h, 0, 2, 0FFh, 0FFh, 7>
	BOXMN	< 4, 0,0,0,0, 420h, TmpErr, RtAb_D>
	DB	0
FtEr2_B	BOXHD	< FtEr2_P, 928h, 0, 2, 0FFh, 0FFh, 7>
	BOXMN	< 4, 0,0,0,0, 426h, TmpErr, OkMn_D>
	DB	0
OkMn_D	DB	4, 0
	DB	0
SecDat	DW	0

	.CODE
FatErr:	PUSH	DS
	PUSH	ES
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	CLD
	MOV	DS,CS:SecDat
	PUSH	DS
	POP	ES
	MOV	BL,DS:ErrDat
	OR	BL,DS:BlocEr
	JZ	FtErr1
	MOV	AL,3
	JMP	FtErr9
FtErr1:	PUSH	AX
	PUSH	ES
	PUSH	DI
	MOV	DS,BP
	MOV	BX,SI
	TEST	BYTE PTR DS:[BX+5],80h
	PUSH	CS
	POP	DS
	MOV	DI,OFFSET TmpErr+1
	MOV	SI,OFFSET Err_S+1
	MOV	CX,6
	REP	MOVSB
	JNZ	FtErr2
	MOV	SI,OFFSET OnDr_S
	MOV	CL,11
	REP	MOVSB
	ADD	ES:[DI-2],AL
	TEST	AH,80h
	JZ	FtErr3
	POP	DI
	MOV	DI,OFFSET FatEr_S
	XOR	AX,AX
	JMP	FtErr5
FtErr2:	MOV	SI,OFFSET OnDv_S
	MOV	CL,10
	REP	MOVSB
	MOV	DS,BP
	LEA	SI,[BX+10]
	MOV	CL,8
	PUSH	DI
	PUSH	CX
	REP	MOVSB
	MOV	AL,' '
	STOSB
	POP	CX
	POP	DI
	INC	CX
	REPNE	SCASB
	DEC	DI
	MOV	AX,':'
	STOSW
FtErr3:	PUSH	CS
	POP	ES
	POP	CX
	XOR	AX,AX
	MOV	DI,OFFSET Errs_S
	JCXZ	FtErr5
	CMP	CX,15h
	JB	FtErr4
	MOV	CX,15h
FtErr4:	PUSH	CX
	MOV	CL,80
	REPNE	SCASB
	POP	CX
	LOOP	FtErr4
FtErr5:	POP	ES
	POP	CX
	PUSH	ES
	POP	DS
	XOR	BX,BX
	XCHG	BL,DS:Clock
	XCHG	BH,DS:BlDelay
	MOV	CL,DS:HlpPage
	DEC	AX
	MOV	DS:AutoTim.Lo,AX
	MOV	BP,OFFSET FtEr1_B
	TEST	CH,10h
	JNZ	FtErr6
	MOV	BP,OFFSET FtEr2_B
FtErr6:	MOV	DX,DI
	CALL	Dialog
	TEST	CH,10h
	JZ	FtErr7
	CMP	AL,0
	MOV	AL,1
	JE	FtErr8
FtErr7:	MOV	AL,3
FtErr8:	MOV	DS:Clock,BL
	MOV	DS:BlDelay,BH
	MOV	DS:HlpPage,CL
	CMP	AL,1
	JE	FtErr9
	MOV	DS:ErrDat,1
FtErr9:	CBW
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	POP	ES
	POP	DS
	IRET

;-------------------- Not ready error ------------------
;			   AL - Drive letter
;	Return:		   AL - Drive letter
	.CONST
NoRd1_S	DB	"Can't read the disk in drive ^V:",0
NoRd2_S	DB	"Press ENTER to try again, ESC to abort,",0
NoRd3_S	DB	"or enter a different drive letter here ^r^V^r:",0
	.DATA
NoRd_P	DB	Err_C, ErInv_C, 0, 0
	DB	4, 1
	DW	ErrBuf
	DW	TmpErr+1
	DB	0, 55
	DW	Err_S
	DW	NoRd1_S
	DW	NoRd2_S
	DW	NoRd3_S
Ready_T	DW	001Bh, ErRdy6
	DW	4400h, ErRdy6
	DW	000Dh, ErRdy7
	DW	0

	.CODE
ErrRdy	PROC	NEAR
	PUSH	DS
	PUSH	BP
	PUSH	SI
	PUSH	DI
	PUSH	DX
	PUSH	CX
	PUSH	BX
	PUSH	AX
	PUSH	ES
	POP	DS
	CBW
	MOV	WORD PTR DS:[TmpErr+1],AX
	MOV	CL,AL
	MOV	AL,2
	CALL	PutKey
	MOV	BP,OFFSET NoRd_P
	MOV	BX,528h
	CALL	DialBox
	CALL	Window
	PUSH	SI
	LEA	AX,[BX+42Ch]
	CALL	Cursor
	CALL	ClrKbd
	CALL	Beep
	MOV	DS:HlpPage,0FFh
	CALL	BasAdr
	ADD	DI,2*364
ErRdy1:	MOV	AL,2
	CALL	Input
	CMP	AH,0FCh
	JE	ErRdy2
	MOV	SI,OFFSET Ready_T
	CALL	Case
	CMP	AL,80h
	JAE	ErRdy1
	CALL	UpCase
	CMP	AL,'A'
	JB	ErRdy1
	CMP	AL,'Z'
	JA	ErRdy1
	CALL	TstDrv
	JNZ	ErRdy1
	MOV	DS:[DI],AL
	MOV	CL,AL
	PUSH	DX
	PUSH	BX
	ADD	BX,42Ch
	MOV	DX,101h
	CALL	Window
	POP	BX
	POP	DX
	JMP	ErRdy1
ErRdy2:	PUSH	CX
	XOR	SI,SI
ErRdy3:	CALL	GetMous
	JZ	ErRdy5
	MOV	BP,CX
	MOV	AH,0
	CMP	AL,1
	JE	ErRdy4
	MOV	AH,'ы'
	CMP	AL,2
	JE	ErRdy4
	MOV	AH,'x'
ErRdy4:	MOV	CX,SI
	MOV	SI,AX
	CMP	AH,CH
	JE	ErRdy3
	MOV	AL,AH
	CALL	TypMous
	JMP	ErRdy3
ErRdy5:	POP	CX
	MOV	AL,0
	CALL	TypMous
	MOV	AX,SI
	CMP	AL,2
	JE	ErRdy7
	CMP	AL,1
	JNE	ErRdy6
	PUSH	DX
	PUSH	CX
	MOV	CX,BP
	SUB	DX,102h
	CALL	InWind
	POP	CX
	POP	DX
	JNC	ErRdy1
ErRdy6:	MOV	CL,0
ErRdy7:	CALL	HidCur
	POP	SI
	CALL	ResWin
	CALL	Window
	MOV	BL,CL
	SUB	BL,'A'-1
	MOV	AX,440Fh
	INT	21h
	POP	AX
	MOV	AL,CL
	POP	BX
	POP	CX
	POP	DX
	POP	DI
	POP	SI
	POP	BP
	POP	DS
	RET
ErrRdy	ENDP

MainPth	PROC	NEAR
	MOV	SI,OFFSET NcEnv
	MOV	CX,3
MnPth0:	PUSH	DS
	PUSH	DI
	PUSH	CS
	POP	DS
	XOR	DI,DI
	MOV	AL,0
MnPth1:	CMP	AL,ES:[DI]
	JE	MnPth2
	PUSH	DI
	PUSH	SI
	PUSH	CX
	REPE	CMPSB
	POP	CX
	POP	SI
	POP	DI
	JE	MnPth3
	PUSH	CX
	MOV	CH,0FFh
	REPNE	SCASB
	POP	CX
	JMP	MnPth1
MnPth2:	LEA	SI,[DI+3]
	PUSH	ES
	POP	DS
	POP	DI
	POP	ES
	CMP	DI,OFFSET TempDir
	JNE	MnPth9
	PUSH	ES
	POP	DS
	MOV	SI,OFFSET NcPath
MnPth9:	MOV	CX,LenPath
	PUSH	DI
	PUSH	CX
	REP	MOVSB
	POP	CX
	POP	SI
	PUSH	ES
	POP	DS
	CALL	CutPath
	MOV	DI,SI
	JMP	MnPth4
MnPth3:	MOV	SI,DI
	ADD	SI,CX
	PUSH	ES
	POP	DS
	POP	DI
	POP	ES
	MOV	CX,LenPath-2
	CALL	MvzLine
	CALL	AddFile
MnPth4:	PUSH	ES
	POP	DS
	MOV	BYTE PTR DS:[DI],0
	RET
MainPth	ENDP

ReadOne	PROC	NEAR
	PUSH	CX
	PUSH	CS
	POP	DS
	MOV	CX,13
	REP	MOVSB
	PUSH	ES
	POP	DS
	MOV	AX,3D20h
	INT	21h
	POP	CX
	JC	RdOne1
	MOV	BX,AX
	MOV	DX,BP
	MOV	AH,3Fh
	INT	21h
	PUSHF
	PUSH	AX
	MOV	AH,3Eh
	INT	21h
	POP	AX
	POP	DX
	JC	RdOne1
	PUSH	DX
	POPF
RdOne1:	RET
ReadOne	ENDP

Expand	PROC	NEAR
	XOR	DI,DI
	MOV	CX,WCBsave
	REP	MOVSB
	MOV	DI,OFFSET DskBuf
	MOV	AX,ES:EndBuf
	SUB	AX,DI
	XOR	DX,DX
	MOV	CX,SIZE FilEnt
	DIV	CX
	MOV	CX,AX
	JCXZ	Expnd2
Expnd1:	PUSH	DI
	PUSH	CX
	MOV	CX,12
	REP	MOVSB
	MOV	AL,0
	STOSB
	MOVSB
	POP	CX
	POP	DI
	ADD	DI,SIZE FilEnt
	LOOP	Expnd1
Expnd2:	RET
Expand	ENDP

	INCLUDE	VCSUB1.INC
	INCLUDE	VCSUB2.INC
	INCLUDE	VCSUB3.INC

	.CONST
Last:

	END
