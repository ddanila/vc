;---------------------------------------------------------------
;		   Idea:	15.05.1991
;		  Begin:	16.05.1991
;		Version:	16.07.2000
;---------------------------------------------------------------

		.MODEL	Tiny,C
		.CODE
Zero:		.STARTUP
		LOCALS	@@
		JUMPS
		DB	'RESIDENT',10,13
		JMP	Init00

HeaderBuffer	EQU	 WORD PTR [5Ch]+0
OvlOffset	EQU	DWORD PTR [5Ch]+0
Vect21EMS	EQU	DWORD PTR [5Ch]+0
Vect27EMS	EQU	DWORD PTR [5Ch]+4
DataHandleEMS	EQU	 WORD PTR [5Ch]+8
CodeHandleEMS	EQU	 WORD PTR [5Ch]+10
PageFrame	EQU	 WORD PTR [5Ch]+12
BreakProc	EQU	 BYTE PTR [5Ch]+14
ProcsEMS	EQU	 WORD PTR [5Ch]+15

		INCLUDE	VCDATA.INC

StackPoint	DW	0
StartMain	DD	0
RelocPoint	DW	0
CodeSegLength	DW	0
CodeLength	DW	0
DataLength	DW	0
DataPoint	DW	0
DataHandle	DW	0
CodeHandle	DW	0
ControlXMS	DD	0
Vect22		DD	0

rBatAdr		DW	0
rComSpec	DW	0		; Offset of COMSPEC= in env seg
rOvlName	DW	0		; Offset of this program name in env seg
rOvlLength	DW	0		; Length of this program name in env seg
rBreakFlag	DB	0
rExecType	DB	0
rPrompt		DB	0
rLoadFlag	DB	0
rChkTSR		DB	0
rLocation	DB	0FFh
rEPB		DW	0, 0, 0, 5Ch, 0, 6Ch, 0

CallTable	LABEL
Exec21		DW	0
Exec2E		DW	0
LoadNextCmd	DW	0
LoadOverlay	DW	0
DoExit		DW	EmptyProc
NumCalls	=	$ - CallTable

;---------------------------------------------------------------
		.DATA?
TmpMainPath	DB	LenPath+12 DUP(?)	; Path of the home directory
TmpOvlName	EQU	TmpMainPath		; Path and name of ovl file
TmpOvlBuffer	EQU	TmpMainPath		; Buffer for loading overlay
TmpPath		DB	LenPath+12 DUP(?)	; Path and name of swap file
TmpCode		DW	?			; Code segment for overlay
TmpData		DAT	?			; Initial copy of ComnIni, WCBs
TmpExt		LABEL
Last		LABEL

;---------------------------------------------------------------
		.CONST
		DB	13,10,13,10,'TEXT',13,10
Comspec		DB	'COMSPEC='
Comspec_L	=	$ - Comspec
VcEnv		DB	'VC='
VcEnv_L		=	$ - VcEnv
VcTmpEnv	DB	'VC'
TempEnv		DB	'TEMP='
VcTmpEnv_L	=	$ - VcTmpEnv
TempEnv_L	=	$ - TempEnv
Init_F		DB	'VC.INI',0
Ext_F		DB	'VC.EXT',0
Arch_F		DB	'VCARCH.EXT',0
Filter_F	DB	'VCFILTER.EXT',0
Swap_F		DB	'VCSWAP.000',0
Mask_S		DB	'\*.*',0,0
Title_S		DB	'The ',PrgName,', Version ',VcVersion,13,10
		DB	'Copyright (C) 1991-2000 Vsevolod V. Volkov'
		IFDEF	OFFICIAL
		DB	13,10
		DB	13,10, 'THIS IS ONLY A TEST VERSION'
		DB	13,10, 'AUTHOR DOES NOT WARRANTY ANYTHING'
		ENDIF
		DB	13,10, '$'
Version_S	DB	13,10, 'This program requires DOS 3.20 or later.'
		DB	13,10, 0
NoIni_S		DB	13,10, 'Can not read the setup file: ',0
BadIni_S	DB	13,10, 'Incorrect data in the setup file: ',0
ErrIni_S	DB	13,10, 'Use Shift-F9 to create a new setup file.'
		DB	13,10
Empty_S		DB	0
DefSwitches	DB	'/C',0,0			; Switches
DefDescr	DB	'descript.ion;files.bbs',0,0	; DescrFiles
DefDirInfo	DB	'dirinfo;file_id.diz;readme;'	; DirInfoFiles
		DB	'read.me;readme.txt;readme.*',0,0

;----------------------- Default settings ----------------------
		.DATA
		DB	13,10,13,10,'DATA',13,10
DefInit		DB	07h,07h,1Bh,07h,07h,07h	;  0 - Main windows
		DB	07h,07h,1Ah,07h,07h,07h	;  1 - Directories
		DB	70h,70h,30h,70h,70h,70h	;  2 - Cursor
		DB	0Fh,0Fh,1Eh,0Fh,0Fh,0Fh	;  3 - Selected files
		DB	7Fh,09h,3Eh,09h,70h,70h	;  4 - Cursor for selected files
		DB	0Fh,0Fh,1Eh,0Fh,0Fh,0Fh	;  5 - "Name   Size ..."
		DB	07h,07h,07h,07h,07h,07h	;  6 - Key bar numbers
		DB	70h,70h,30h,70h,70h,70h	;  7 - Key bar titles
		DB	70h,70h,30h,70h,70h,70h	;  8 - Menu bar
		DB	0Fh,0Fh,0Fh,0Fh,0Fh,0Fh	;  9 - Menu bar cursor
		DB	70h,70h,30h,70h,70h,70h	; 10 - Menu box border
		DB	70h,70h,3Fh,70h,70h,70h	; 11 - Menu box text
		DB	7Fh,70h,3Eh,70h,70h,70h	; 12 - Menu box high letters
		DB	07h,0Fh,0Fh,0Fh,0Fh,0Fh	; 13 - Menu box text cursor
		DB	0Fh,0Fh,0Eh,0Fh,0Fh,0Fh	; 14 - Menu box high letters cursor
		DB	70h,70h,30h,70h,70h,70h	; 15 - Menu box dark lines
		DB	70h,70h,33h,70h,70h,70h	; 16 - Menu box dark lines '-'
		DB	07h,07h,70h,07h,07h,07h	; 17 - Function box
		DB	0Fh,0Fh,7Eh,0Fh,0Fh,0Fh	; 18 - Function box bright
		DB	70h,70h,30h,70h,70h,70h	; 19 - Function box cursor
		DB	07h,07h,3Fh,07h,07h,07h	; 20 - Configuration window
		DB	70h,70h,0Fh,70h,70h,70h	; 21 - Configuration cursor
		DB	0Fh,0Fh,3Eh,0Fh,0Fh,0Fh	; 22 - Configuration window bright
		DB	7Fh,09h,0Eh,09h,70h,70h	; 23 - Configuration cursor bright
		DB	70h,70h,3Fh,70h,70h,70h	; 24 - History window
		DB	07h,07h,0Fh,07h,07h,07h	; 25 - History cursor
		DB	7Fh,70h,3Eh,70h,70h,70h	; 26 - History bright
		DB	07h,07h,13h,07h,07h,07h	; 27 - Menu help border
		DB	70h,70h,30h,70h,70h,70h	; 28 - Menu help window
		DB	07h,07h,30h,07h,07h,07h	; 29 - Help window
		DB	0Fh,0Fh,3Fh,0Fh,0Fh,0Fh	; 30 - Help window bold
		DB	70h,70h,0Fh,70h,70h,70h	; 31 - Help reversive
		DB	0Fh,09h,3Eh,09h,0Fh,0Fh	; 32 - Help underline
		DB	07h,07h,4Fh,07h,07h,07h	; 33 - Error windows
		DB	70h,70h,70h,70h,70h,70h	; 34 - Error window cursor
		DB	0Fh,0Fh,4Eh,0Fh,0Fh,0Fh	; 35 - Error window bright
		DB	70h,70h,30h,70h,70h,70h	; 36 - Clock
		DB	0Fh,0Fh,1Eh,0Fh,0Fh,0Fh	; 37 - Arrows in view/edit
		DB	07h,07h,0Bh,07h,07h,07h	; 38 - Screen blank
		DB	0Fh,0Fh,0Fh,0Fh,0Fh,0Fh	; 39 - Screen blank light stars
		DB	02h,02h,01h,02h,00h,00h	; 40 - Shadow flag
		DB	02h,02h,02h,02h,02h,02h	; 41 - Blink / High background / None
		DB	1			; FulScr
		DB	0A7h			; PanelSize
		DB	0			; MenuVs
		DB	1			; KeyBar
		DB	1			; Prompt
		DB	1			; InsDwn
		DB	1			; InsDirSize
		DB	0			; AutoMnu
		DB	1			; MiniSta
		DB	0			; Clock
		DB	1			; AutoCD
		DB	1			; AutoSav
		DB	1			; ColTyp
		DB	5			; BlDelay
		DB	1			; LoadFl
		DB	1			; UseSwap
		DB	1			; UseEMS
		DB	1			; UseXMS
		DB	1			; UnSlct
		DB	1			; ErrSnd
		DB	0			; LftMous
		DB	0			; MousRes
		DB	1			; ColorDirs
		DB	1			; ZoomWin
		DB	3			; ZoomStages
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
		DB	0			; ChkTSR
		DB	0			; IncDirs
		DB	1			; DescrProcess
		DB	1			; DescrCreate
		DB	1			; DescrEmpty
		DB	1			; DescrDel
		DB	1			; ChkTarget
		DB	1			; ClearRO
		DB	1			; CopyEAs
		DB	1			; ConvCase
		DB	1			; DriveB
		DB	1			; TreeInRoot
		DB	1			; TreeInHome
		DB	0			; SrchCase
		DB	0			; NumbSort
		DB	MiliSec, Sec		; DelayTm
		DB	0FAh, 0F9h, 07h, 04h, 0Fh		; StarTab = '·∙•♦︎☼'
		DB	0C9h, 0BBh, 0CDh, 0C8h, 0BCh, 0CDh, 0BAh, 0BAh, 0C7h, 0B6h, 0C4h, 0D1h, 0B3h, 0C1h, 0CFh	; Borders = '╔╗═╚╝═║║╟╢─╤│┴╧'
		DB	0DAh, 0BFh, 0C4h, 0C0h, 0D9h, 0C4h, 0B3h, 0B3h, 0C4h	; MenuBords = '┌┐─└┘─││─'
		DB	1Bh,1Ah			; Scrolls
		DB	18h, 19h, 0B0h, 0B2h	; ScrollBars = '↑↓░▓'
		DB	0B0h, 0DBh		; ProgressBars = '░█'
		DB	18h, 19h, 12h, 1Dh, 0FBh, 78h, 0FEh	; Pointers = '↑↓↕︎↔︎√x■'
		DB	0B0h, 0DEh, 0DBh	; FileMarks = '░▐█'
		DB	0FBh		; OnItem = '√'
		DW	LenBat			; LenCmdLn
		DW	LenHist			; LenHistory
		DW	MinFls			; MinFiles
		DB	MaxProg			; MaxTSRs
DefWCB		DB	0			; WinTyp
		DB	0			; BrfFul
		DB	0			; LongNames
		DB	12			; NameWidth
		DB	1			; Visible
		DB	37h			; FiltAttr
		DB	0			; MasTyp
		DB	4			; SortTyp
		DB	0			; SortRev
		DB	1			; ShowSize
		DB	1			; ShowRatio
		DB	1			; ShowDate
		DB	1			; ShowTime
		DB	0			; Ctrl_L
		DB	0, 0			; DskMas

;----------------------- Begin of program ----------------------
		.CODE
OvlPrm		DB	OvlPrm_L-2, 0, VcID
OvlPrmPoint	DW	StartMain, 0
		DB	13
OvlPrm_L	=	$ - OvlPrm
OvlExt_F	DB	'OVL',0
Command_S	DB	'command processor', 0
RunErr_S	DB	'There is not enough memory to run this command', 13,10,0
BadFile_S	DB	13,10, "Bad or missing ", 0
Retry_S		DB	13,10, "Press ENTER to try again or ESC to abort", 13,10,0
MemoryErr_S	DB	13,10, "Program too big to fit in memory"
CrLf_S		DB	13,10, 0

;---------------------- Execute commands -----------------------
Exec00:		MOV	DS:rExecType,BL
		MOV	DS:rPrompt,BH
		MOV	DS:rLoadFlag,DL
		MOV	DS:rBatAdr,SI
		MOV	DS:DataLength,CX
		OR	AH,AH			; Unload VC
		JZ	Exec01
		CALL	GetStat
		MOV	AH,49h
		INT	21h
		CALL	SetStat
Exec01:		OR	DH,DH			; Fixing DR-DOS INT 2Eh bug
		JE	Exec02
		MOV	SI,80h
		MOV	DL,2
		MOV	AX,0D3Ah
		MOV	DS:[SI+0],DX
		MOV	DS:[SI+2],AX
		CALL	CS:Exec2E
Exec02:		PUSH	DS
		POP	ES
		MOV	DS:rEPB.CmdLine.Offs,80h
Exec03:		CLD
		MOV	DI,81h			; Get next command
		MOV	CX,LenCmdLine+1
		PUSH	DI
		PUSH	CX
		CALL	DS:LoadNextCmd
		POP	CX
		POP	DI
		MOV	AL,0
		SCASB
		JE	Exec09
		REPNE	SCASB
		MOV	BYTE PTR DS:[DI-1],13
		LEA	AX,[DI-81h]
		ADD	DS:rBatAdr,AX
		DEC	AX
		MOV	SI,80h
		MOV	DS:[SI],AL
		MOV	DL,DS:rBreakFlag
		MOV	AX,3301h
		INT	21h
		CMP	DS:rExecType,0
		JNZ	Exec04
		CALL	CS:Exec2E		; Run INT 2Eh
		CALL	SetBreak
		PUSH	DS
		POP	ES
		JMP	Exec06
Exec04:		MOV	DI,OFFSET Command_S	; Run COMMAND /C ...
		MOV	DX,DS:rComSpec
		OR	DX,DX
		JE	Exec05
		MOV	DI,DX
		MOV	DS,DS:[2Ch]
		MOV	BX,OFFSET rEPB
		CALL	CS:Exec21
		CALL	SetBreak
		JNC	Exec06
		CMP	AX,3
		JBE	Exec05
		MOV	DX,OFFSET RunErr_S
		CALL	OutErr
		JMP	Exec09
Exec05:		CALL	NotFound
		PUSH	ES
		POP	DS
		JNC	Exec04
		JMP	Exec09
Exec06:		PUSH	ES			; Continue ?
		POP	DS
		MOV	DI,81h
		MOV	CX,1
		CALL	DS:LoadNextCmd
		MOV	AL,DS:[81h]
		CMP	AL,0
		JE	Exec09
		CMP	AL,'@'
		JNE	Exec07
		INC	DS:rBatAdr
		JMP	Exec03
Exec07:		MOV	DI,80h			; Output prompt
		PUSH	DI
		MOV	AX,0A0Dh
		STOSW
		MOV	AH,19h
		INT	21h
		MOV	DL,AL
		INC	DX
		ADD	AL,'A'
		STOSB
		CMP	DS:rPrompt,0
		JE	Exec08
		MOV	AX,'\:'
		STOSW
		MOV	SI,DI
		MOV	AH,47h
		INT	21h
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
Exec08:		MOV	AX,'>'
		STOSW
		POP	DX
		CALL	OutStd
		MOV	DI,81h
		MOV	CX,LenCmdLine+1
		PUSH	DI
		CALL	DS:LoadNextCmd
		POP	DX
		ADD	DL,DS:rExecType
		CALL	OutStd
		MOV	DX,OFFSET CrLf_S
		CALL	OutStd
		JMP	Exec03
Exec09:		PUSH	ES			; Load VC
		POP	DS
		CMP	DS:rLoadFlag,0
		JZ	Exec10
		MOV	DS:StartMain.Segm,0
		CALL	GetStat
		PUSH	BP
		CALL	DS:LoadOverlay
		POP	BP
		CALL	SetStat
		JC	ChkError
		CMP	DS:StartMain.Segm,0
		JE	OutError
Exec10:		MOV	AL,DS:rLocation
		MOV	SI,DS:DataPoint
		MOV	CX,DS:DataLength
		MOV	DX,DS:DataHandle
		CALL	DS:StartMain
		PUSH	CS
		POP	DS
		JNC	Exec00
ChkError:	CMP	AL,0FFh
		JNZ	Exit00
OutError:	MOV	DX,OFFSET MemoryErr_S
		CALL	OutErr
		MOV	AL,Alloc_E

Exit00:		MOV	CX,CS:Vect22.Offs	; Exit
		MOV	SS:[0Ah].Offs,CX
		XOR	CX,CX
		XCHG	CX,CS:Vect22.Segm
		MOV	SS:[0Ah].Segm,CX
Exit01:		PUSH	AX
		CMP	CS:rChkTSR,0
		JZ	Exit04
		MOV	CX,CS:Counter
		JCXZ	Exit03
Exit02:		MOV	SI,CS:Counter
		SHL	SI,1
		MOV	DS,CS:PrgList[SI-2]
		CALL	Release
		LOOP	Exit02
Exit03:		LDS	DX,CS:Vect21
		CALL	SetVect21
		LDS	DX,CS:Vect27
		MOV	AX,2527h
		INT	21h
Exit04:		CALL	CS:DoExit
		MOV	DL,CS:rBreakFlag
		MOV	AX,3301h
		INT	21h
		POP	AX
		CMP	CS:Vect22.Segm,0
		JNE	Exit05
		MOV	AH,4Ch
		INT	21h
Exit05:		JMP	CS:Vect22

;---------------------------------------------------------------
;	Output to error & check for retry
;		DS:DI - string
;	Return:	   CY - if abort
;---------------------------------------------------------------
NotFound	PROC	NEAR
		PUSH	DS
		PUSH	CS
		POP	DS
		MOV	DX,OFFSET BadFile_S
		CALL	OutErr
		POP	DS
		MOV	DX,DI
		CALL	OutErr
		PUSH	DS
		MOV	DX,OFFSET Retry_S
		PUSH	CS
		POP	DS
		CALL	OutErr
		POP	DS
  @@Retry:	MOV	AH,7
		INT	21h
		CMP	AL,0
		JNE	@@Enter
		MOV	AH,7
		INT	21h
		JMP	@@Retry
  @@Enter:	CMP	AL,0Dh
		JE	@@Exit
		CMP	AL,1Bh
		JNE	@@Retry
		STC
  @@Exit:	RET
NotFound	ENDP

;---------------------------------------------------------------
;	Output to StdOut
;		DS:DX - string
;---------------------------------------------------------------
OutStd		PROC	NEAR
		MOV	BX,1
		JMP	OutDev
OutStd		ENDP

;---------------------------------------------------------------
;	Output to StdError
;		DS:DX - string
;---------------------------------------------------------------
OutErr		PROC	NEAR
		MOV	BX,2
OutDev:		PUSH	CX
		PUSH	SI
		MOV	SI,DX
		MOV	CX,0FFFFh
  @@Length:	INC	CX
		LODSB
		CMP	AL,0
		JNE	@@Length
		POP	SI
		MOV	AH,40h
		INT	21h
		POP	CX
EmptyProc:	RET
OutErr		ENDP

;---------------------------------------------------------------
;	Save break flag & Disable break
;	Return:	   DL - break flag
;---------------------------------------------------------------
SetBreak	PROC	NEAR
		USES	AX,DS
		PUSHF
		MOV	AX,3300h
		INT	21h
		MOV	CS:rBreakFlag,DL
		MOV	DL,0
		MOV	AX,3301h
		INT	21h
		PUSH	SS
		POP	DS
		MOV	DX,OFFSET BreakProc
		MOV	AX,2523h
		INT	21h
		POPF
		RET
SetBreak	ENDP

;---------------------------------------------------------------
;	Get allocation strategy & link state
;	Return:	   BP - allocation strategy & link state
;---------------------------------------------------------------
GetStat		PROC	NEAR
		MOV	CX,0FFFFh
		MOV	AX,5802h
		INT	21h
		JC	@@Exit
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
  @@Exit:	MOV	BP,CX
		RET
GetStat		ENDP

;---------------------------------------------------------------
;	Set allocation strategy & link state
;		   BP - allocation strategy & link state
;---------------------------------------------------------------
SetStat		PROC	NEAR
		USES	AX
		PUSHF
		CMP	BP,0FFFFh
		JE	@@Exit
		MOV	BX,BP
		MOV	BL,BH
		MOV	BH,0
		MOV	AX,5801h
		INT	21h
		MOV	BX,BP
		MOV	BH,0
		MOV	AX,5803h
		INT	21h
  @@Exit:	POPF
		RET
SetStat		ENDP

;---------------------------------------------------------------
BegRelease:	DB	13,10,13,10,'RELEASE',13,10

SetVect21	PROC	NEAR			; Windows 95 shows incorrect
		USES	ES			; size of FAT-32 disks if
		XOR	AX,AX			; use INT 21h AH=25h
		MOV	ES,AX
		CLI
		MOV	ES:[4*21h].Offs,DX
		MOV	ES:[4*21h].Segm,DS
		STI
		RET
SetVect21	ENDP

New21		PROC	FAR			; INT 21h
		PUSHF
		CMP	AH,31h
		JE	@@Func31
		CMP	AH,49h
		JE	@@Func49
		CMP	AH,4Bh
		JNE	@@Old21
		CALL	ExeProg
		JMP	@@Old21
  @@Func31:	CALL	AddProg
		JNC	@@Old21
		MOV	AH,4Ch
		JMP	@@Old21
  @@Func49:	CALL	FreeBlk
  @@Old21:	POPF
  Old21:	JMP	CS:Vect21
New21		ENDP

New27		PROC	FAR			; INT 27h
		ADD	DX,0Fh
		SHR	DX,1
		SHR	DX,1
		SHR	DX,1
		SHR	DX,1
		CALL	AddProg
		JNC	@@Old27
		MOV	AH,0
		JMP	Old21
  @@Old27:	SHL	DX,1
		SHL	DX,1
		SHL	DX,1
		SHL	DX,1
		JMP	CS:Vect27
New27		ENDP

;---------------------------------------------------------------
;	Save vectors & program name
;		DS:DX - file name
;---------------------------------------------------------------
ExeProg		PROC	FAR
		USES	AX,CX,SI,DI,ES
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
  @@FndName1:	MOV	CX,SI
  @@FndName2:	LODSB
		CMP	AL,':'
		JE	@@FndName1
		CMP	AL,'\'
		JE	@@FndName1
		CMP	AL,'/'
		JE	@@FndName1
		CMP	AL,0
		JNE	@@FndName2
		MOV	SI,CX
		MOV	DI,OFFSET NameBuf
		MOV	CX,12
		REP	MOVSB
		RET
ExeProg		ENDP

;---------------------------------------------------------------
;	Add resident program to list
;		   DX - number of paragraphs
;---------------------------------------------------------------
AddProg		PROC	FAR
		USES	AX,BX,CX,SI,DI,BP,DS,ES
		STI
		CMP	CS:Counter,MaxProg
		JB	@@Realloc
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
		JMP	@@Exit
  @@Realloc:	MOV	AH,62h			; Get current PSP
		INT	21h
		MOV	ES,BX
		CALL	FreeBlk
		MOV	BX,DX
		ADD	BX,(2+12+1+5*256+4+2 +15) SHR 4
		MOV	AH,4Ah
		INT	21h
		CMC
		JNC	@@Exit
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
  @@CmpVect:	LODS	WORD PTR CS:[SI]
		MOV	CX,AX
		LODS	WORD PTR CS:[SI]
		CMP	CX,DS:[BX].Offs
		JNE	@@SaveVect
		CMP	AX,DS:[BX].Segm
		JE	@@NextVect
  @@SaveVect:	CMP	DL,22h
		JE	@@NextVect
		PUSH	AX			; Save vector
		MOV	AL,DL
		STOSB
		MOV	AX,CX
		STOSW
		POP	AX
		STOSW
		INC	DH
  @@NextVect:	ADD	BX,4
		INC	DL
		JNE	@@CmpVect
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
  @@ChkSum:	LODS	BYTE PTR ES:[SI]
		ADD	BX,AX
		LOOP	@@ChkSum
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
  @@Exit:	RET
AddProg		ENDP

;---------------------------------------------------------------
;	Free memory block
;		   ES - segment for deallocation
;---------------------------------------------------------------
FreeBlk		PROC	FAR
		USES	AX,CX,SI,DI,DS
		CLD
		MOV	SI,OFFSET PrgList
		MOV	DI,SI
		MOV	CX,CS:Counter
		JCXZ	@@Exit
  @@Check:	MOV	DS,CS:[SI]
		CALL	ChkBlk
		LODS	WORD PTR CS:[SI]
		JNE	@@Next
		PUSH	AX
		MOV	AX,ES
		CMP	AX,DS:[0]
		POP	AX
		JE	@@Next
		MOV	CS:[DI],AX
		INC	DI
		INC	DI
  @@Next:	LOOP	@@Check
		SUB	SI,DI
		SHR	SI,1
		SUB	CS:Counter,SI
  @@Exit:	RET
FreeBlk		ENDP

;---------------------------------------------------------------
;	Check sum
;		   DS - PrgSeg
;---------------------------------------------------------------
ChkBlk		PROC	NEAR
		USES	CX,SI,DI
		MOV	AL,DS:[0Eh]
		MOV	AH,5
		MUL	AH
		ADD	AX,2+12+1+4
		MOV	CX,AX
		XOR	SI,SI
		XOR	DI,DI
		MOV	AH,0
  @@ChkSum:	LODSB
		ADD	DI,AX
		LOOP	@@ChkSum
		CMP	DI,DS:[SI]
		RET
ChkBlk		ENDP

;---------------------------------------------------------------
;	Release programs
;		   DS - PrgSeg
;---------------------------------------------------------------
Release		PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,DS,ES
		PUSH	BP
		CALL	GetStat
		CLD
		CALL	ChkBlk			; Check sum
		JNE	@@Exit
		XOR	AX,AX			; Restore vectors
		MOV	ES,AX
		MOV	SI,0Eh
		LODSB
		MOV	CX,AX
		CLI
		JCXZ	@@RestPtr
  @@RestVect:	LODSB
		MOV	DI,AX
		SHL	DI,1
		SHL	DI,1
		MOVSW
		MOVSW
		LOOP	@@RestVect
  @@RestPtr:	MOV	DI,4A8h			; Restore SAVE PTR
		MOVSW
		MOVSW
		STI
		MOV	SI,DS:[0]		; Release blocks
		MOV	AH,52h
		INT	21h
		MOV	DS,ES:[BX-2]
  @@FreeBlk:	CMP	SI,DS:[1]
		JNE	@@NextBlk
		MOV	AX,DS
		INC	AX
		MOV	ES,AX
		MOV	AH,49h
		INT	21h
  @@NextBlk:	MOV	AX,DS
		INC	AX
		ADD	AX,DS:[3]
		CMP	BYTE PTR DS:[0],'M'
		MOV	DS,AX
		JE	@@FreeBlk
  @@Exit:	CALL	SetStat
		POP	BP
		RET
Release		ENDP

;---------------------------------------------------------------
;	Release data
;---------------------------------------------------------------
ReData		LABEL
IntTab		DD	256	DUP(?)
SavePtr		DD	?
Vect21		DD	?
Vect27		DD	?
NameBuf		DB	12 DUP(?)
ReleaseProc	DW	?
Counter		DW	?
PrgList		LABEL	WORD
		ORG	ReData

;---------------------------------------------------------------
SubsRAM:	DW	Exec21RAM
		DW	Exec2ERAM
		DW	LoadNextRAM
		DW	LoadOvlRAM
		DW	DoExitRAM

		DB	13,10,13,10,'STD',13,10

Exec21RAM	PROC	NEAR
		MOV	AX,4B00h
		INT	21h
		RET
Exec21RAM	ENDP

Exec2ERAM	PROC	NEAR
		MOV	DS:StackPoint,SP
		INT	2Eh
		PUSH	CS
		POP	DS
		PUSH	CS
		CLI
		POP	SS
		MOV	SP,DS:StackPoint
		STI
DoExitRAM:	RET
Exec2ERAM	ENDP

LoadNextRAM	PROC	NEAR
		MOV	SI,DS:DataPoint
		ADD	SI,DS:rBatAdr
		REP	MOVSB
		RET
LoadNextRAM	ENDP

LoadOvlRAM	PROC	NEAR			; Load overlay
  @@Retry:	MOV	BX,OFFSET rEPB
		MOV	DS:[BX].CmdLine.Offs,OFFSET OvlPrm
		MOV	DX,80h
		MOV	DI,DX
		MOV	SI,DS:rOvlName
		MOV	CX,DS:rOvlLength
		PUSH	DS
		MOV	DS,DS:[2Ch]
		REP	MOVSB
		POP	DS
		MOV	SI,OFFSET OvlExt_F
		MOVSW
		MOVSW
		MOV	DI,DX
		MOV	AX,4B00h
		INT	21h
		CLD
		JNC	@@Exit
		CMP	AL,3
		JBE	@@Error
		MOV	AL,0FFh
		STC
		JMP	@@Exit
  @@Error:	MOV	AX,OFFSET NotFound	; Overlay not found
		CALL	AX
		JNC	@@Retry
		MOV	AL,OvlDisk_E
  @@Exit:	RET
LoadOvlRAM	ENDP

LenSubsRAM	=	$ - SubsRAM

;---------------------------------------------------------------
SubsSwap:	DW	Exec21Swap
		DW	Exec2ESwap
		DW	LoadNextSwap
		DW	LoadOvlSwap
		DW	DoExitSwap

		DB	13,10,13,10,'SWAP',13,10

Exec21Swap	PROC	NEAR
		MOV	AX,4B00h
		INT	21h
		RET
Exec21Swap	ENDP

Exec2ESwap	PROC	NEAR
		MOV	DS:StackPoint,SP
		INT	2Eh
		PUSH	CS
		POP	DS
		PUSH	CS
		CLI
		POP	SS
		MOV	SP,DS:StackPoint
		STI
DoExitSwap:	RET
Exec2ESwap	ENDP

LoadNextSwap	PROC	NEAR
  @@Retry:	MOV	WORD PTR DS:[DI],0
		MOV	DX,DS:DataPoint
		MOV	AX,3D40h
		INT	21h
		JC	@@Error
		PUSH	CX
		XOR	CX,CX
		MOV	DX,DS:rBatAdr
		MOV	BX,AX
		MOV	AX,4201h
		INT	21h
		POP	CX
		JC	@@Close
		CMP	AX,DS:rBatAdr
		STC
		JNE	@@Close
		MOV	DX,DI
		MOV	AH,3Fh
		INT	21h
  @@Close:	PUSHF
		POP	DX
		MOV	AH,3Eh
		INT	21h
		JC	@@Error
		PUSH	DX
		POPF
		JNC	DoExitSwap
  @@Error:	PUSH	DI			; Swap file not found
		PUSH	CX
		MOV	DI,DS:DataPoint
		MOV	AX,OFFSET NotFound
		CALL	AX
		POP	CX
		POP	DI
		JNC	@@Retry
		MOV	AL,LoadSwap_E
		MOV	SI,OFFSET Exit00
		JMP	SI
LoadNextSwap	ENDP

LoadOvlSwap	PROC	NEAR			; Load overlay
  @@Retry:	MOV	BX,OFFSET rEPB
		MOV	DS:[BX].CmdLine.Offs,OFFSET OvlPrm
		MOV	DX,80h
		MOV	DI,DX
		MOV	SI,DS:rOvlName
		MOV	CX,DS:rOvlLength
		PUSH	DS
		MOV	DS,DS:[2Ch]
		REP	MOVSB
		POP	DS
		MOV	SI,OFFSET OvlExt_F
		MOVSW
		MOVSW
		MOV	DI,DX
		MOV	AX,4B00h
		INT	21h
		CLD
		JNC	@@Exit
		CMP	AL,3
		JBE	@@Error
		MOV	AL,0FFh
		STC
		JMP	@@Exit
  @@Error:	MOV	AX,OFFSET NotFound	; Overlay not found
		CALL	AX
		JNC	@@Retry
		MOV	DX,DS:DataPoint
		MOV	AH,41h
		INT	21h
		MOV	AL,OvlDisk_E
		STC
  @@Exit:	RET
LoadOvlSwap	ENDP

LenSubsSwap	=	$ - SubsSwap

;---------------------------------------------------------------
SubsXMS:	DW	Exec21XMS
		DW	Exec2EXMS
		DW	LoadNextXMS
		DW	LoadOvlXMS
		DW	DoExitXMS

		DB	13,10,13,10,'XMS',13,10

Exec21XMS	PROC	NEAR
		MOV	AX,4B00h
		INT	21h
		RET
Exec21XMS	ENDP

Exec2EXMS	PROC	NEAR
		MOV	DS:StackPoint,SP
		INT	2Eh
		PUSH	CS
		POP	DS
		PUSH	CS
		CLI
		POP	SS
		MOV	SP,DS:StackPoint
		STI
		RET
Exec2EXMS	ENDP

LoadNextXMS	PROC	NEAR
		MOV	SI,ExtMove-LoadNextXMS
		ADD	SI,DS:LoadNextCmd
		MOV	AX,DS:DataLength
		SUB	AX,DS:rBatAdr
		CMP	AX,CX
		JBE	@@Load
		MOV	AX,CX
  @@Load:	DEC	AX
		OR	AL,1
		INC	AX
		MOV	DS:[SI].LenBlk.Lo,AX
		MOV	AX,DS:DataHandle
		MOV	DS:[SI].SourceHandle,AX
		MOV	AX,DS:DataPoint
		ADD	AX,DS:rBatAdr
		MOV	DS:[SI].SourceOffset.Offs,AX
		XOR	AX,AX
		MOV	DS:[SI].SourceOffset.Segm,AX
		MOV	DS:[SI].DestHandle,AX
		MOV	DS:[SI].LenBlk.Hi,AX
		MOV	DS:[SI].DestOffset.Lo,DI
		MOV	DS:[SI].DestOffset.Hi,ES
		MOV	DS:[DI],AX
		MOV	AH,0Bh
		CALL	DS:ControlXMS
		RET
LoadNextXMS	ENDP

LoadOvlXMS	PROC	NEAR			; Load overlay
		MOV	BX,DS:CodeSegLength
		MOV	AH,48h
		INT	21h
		JC	@@Exit
		MOV	DI,AX
		PUSH	DI
		ADD	DI,10h
		MOV	SI,ExtMove-LoadOvlXMS
		ADD	SI,DS:LoadOverlay
		MOV	AX,DS:CodeLength
		MOV	DL,AH
		MOV	DH,0
		MOV	AH,AL
		MOV	AL,0
		MOV	DS:[SI].LenBlk.Lo,AX
		MOV	DS:[SI].LenBlk.Hi,DX
		MOV	AX,DS:CodeHandle
		MOV	DS:[SI].SourceHandle,AX
		XOR	AX,AX
		MOV	DS:[SI].SourceOffset.Offs,AX
		MOV	DS:[SI].SourceOffset.Segm,AX
		MOV	DS:[SI].DestHandle,AX
		MOV	DS:[SI].DestOffset.Lo,AX
		MOV	DS:[SI].DestOffset.Hi,DI
		MOV	AH,0Bh
		CALL	DS:ControlXMS
		MOV	SI,DI
		MOV	ES,SI
		ADD	SI,DS:RelocPoint
		MOV	DS,SI
		MOV	SI,DS:[18h]
		MOV	CX,DS:[6]
		JCXZ	@@SegSize
  @@Relocate:	LODSW
		MOV	BX,AX
		LODSW
		ADD	AX,DI
		MOV	ES,AX
		ADD	ES:[BX],DI
		LOOP	@@Relocate
  @@SegSize:	POP	ES
		MOV	BX,0FFFFh
		MOV	AH,4Ah
		INT	21h
		MOV	AH,4Ah
		INT	21h
		JC	@@Exit
		MOV	CS:StackPoint,SP
		MOV	AX,DS:[0Eh]
		ADD	AX,DI
		CLI
		MOV	SS,AX
		MOV	SP,DS:[10h]
		STI
		ADD	DI,DS:[16h]
		PUSH	DI
		PUSH	DS:[14h]
		PUSH	CS
		POP	DS
		MOV	DX,@@LoadOvlRet-LoadOvlXMS
		ADD	DX,DS:LoadOverlay
		MOV	AX,2522h
		INT	21h
		MOV	DX,ES
		MOV	SI,BX
		MOV	AH,55h
		INT	21h
		MOV	SI,OFFSET OvlPrm
		MOV	DI,80h
		MOV	CX,OvlPrm_L
		REP	MOVSB
		DEC	DX
		MOV	DS,DX
		MOV	DS:[1],ES
		PUSH	ES
		POP	DS
		RETF
  @@LoadOvlRet:	PUSH	CS
		CLI
		POP	SS
		MOV	SP,CS:StackPoint
		STI
		CLD
		CLC
  @@Exit:	MOV	AL,0FFh
		PUSH	CS
		POP	DS
		RET
LoadOvlXMS	ENDP

DoExitXMS	PROC	NEAR
		MOV	DX,CS:CodeHandle
		OR	DX,DX
		JE	@@Release
		MOV	AH,0Ah
		CALL	CS:ControlXMS
  @@Release:	MOV	DX,CS:DataHandle
		OR	DX,DX
		JE	@@Exit
		MOV	AH,0Ah
		CALL	CS:ControlXMS
  @@Exit:	RET
DoExitXMS	ENDP

ExtMove		EMOV	<>

LenSubsXMS	=	$ - SubsXMS

;---------------------------------------------------------------
SubsEMS:	DW	Exec21EMS
		DW	Exec2EEMS
		DW	LoadNextEMS
		DW	LoadOvlEMS
		DW	DoExitEMS

		DB	13,10,13,10,'EMS',13,10
ErrorEMS_S	DB	13,10, 'Expanded Memory status error.'
		DB	13,10, 0

Exec21EMS	PROC	NEAR
		MOV	CX,OFFSET NewExec21EMS-Load55+ProcsEMS
  Exec2XEMS:	POP	AX
		PUSH	CS
		PUSH	AX
		PUSH	SS
		PUSH	CX
		RETF
Exec21EMS	ENDP

Exec2EEMS	PROC	NEAR
		MOV	CX,OFFSET NewExec2EEMS-Load55+ProcsEMS
		JMP	Exec2XEMS
Exec2EEMS	ENDP

LoadNextEMS	PROC	NEAR
		MOV	AX,DS:DataPoint
		ADD	AX,DS:rBatAdr
		XOR	DX,DX
		MOV	BX,4000h
		DIV	BX
		MOV	SI,DX
		ADD	SI,BX
		MOV	BX,AX
		MOV	DX,DS:DataHandle
  @@MapData:	MOV	AX,4401h
		CALL	CallEMS
		JNE	@@Error
  @@MoveCmd:	LODSB
		STOSB
		CMP	AL,0
		JE	@@Exit
		CMP	SI,8000h
		JB	@@MoveCmd
		INC	BX
		MOV	SI,4000h
		JMP	@@MapData
  @@Error:	MOV	DX,ErrorEMS_S-LoadNextEMS
		ADD	DX,DS:LoadNextCmd
		MOV	AX,OFFSET OutErr
		CALL	AX
		MOV	AL,LoadEMS_E
		MOV	BP,OFFSET Exit00
		JMP	BP
  @@Exit:	RET
LoadNextEMS	ENDP

LoadOvlEMS	PROC	NEAR			; Load overlay
		MOV	BX,DS:CodeSegLength
		MOV	AH,48h
		INT	21h
		JC	@@Exit
		MOV	DI,AX
		PUSH	DI
		ADD	DI,10h
		PUSH	DI
		MOV	AX,DS:CodeLength
		MOV	DL,AH
		MOV	DH,0
		MOV	AH,AL
		MOV	AL,0
		MOV	BP,4000h
		DIV	BP
		MOV	CX,AX
		OR	DX,DX
		JE	@@WholePage
		INC	CX
		MOV	BP,DX
  @@WholePage:	SHR	BP,1
		MOV	DX,DS:CodeHandle
		MOV	AH,47h
		CALL	CallEMS
		JNE	@@ErrorEMS
		XOR	BX,BX
  @@MapCode:	MOV	AX,4401h
		CALL	CallEMS
		JNE	@@ErrorEMS
		PUSH	DI
		PUSH	CX
		CMP	CX,1
		MOV	CX,2000h
		JNE	@@Move
		MOV	CX,BP
		JCXZ	@@Skip
  @@Move:	MOV	SI,4000h
		MOV	ES,DI
		XOR	DI,DI
		REP	MOVSW
  @@Skip:	POP	CX
		POP	DI
		ADD	DI,400h
		INC	BX
		LOOP	@@MapCode
		MOV	AH,48h
		CALL	CallEMS
		JNE	@@ErrorEMS
		POP	DI
		MOV	SI,DI
		MOV	ES,SI
		ADD	SI,DS:RelocPoint
		MOV	DS,SI
		MOV	SI,DS:[18h]
		MOV	CX,DS:[6]
		JCXZ	@@SegSize
  @@Relocate:	LODSW
		MOV	BX,AX
		LODSW
		ADD	AX,DI
		MOV	ES,AX
		ADD	ES:[BX],DI
		LOOP	@@Relocate
  @@SegSize:	POP	ES
		MOV	BX,0FFFFh
		MOV	AH,4Ah
		INT	21h
		MOV	AH,4Ah
		INT	21h
		JC	@@Exit
		MOV	CS:StackPoint,SP
		MOV	AX,DS:[0Eh]
		ADD	AX,DI
		CLI
		MOV	SS,AX
		MOV	SP,DS:[10h]
		STI
		ADD	DI,DS:[16h]
		PUSH	DI
		PUSH	DS:[14h]
		PUSH	CS
		POP	DS
		MOV	DX,@@LoadOvlRet-LoadOvlEMS
		ADD	DX,DS:LoadOverlay
		MOV	AX,2522h
		INT	21h
		MOV	DX,ES
		MOV	SI,BX
		MOV	AH,55h
		INT	21h
		MOV	SI,OFFSET OvlPrm
		MOV	DI,80h
		MOV	CX,OvlPrm_L
		REP	MOVSB
		DEC	DX
		MOV	DS,DX
		MOV	DS:[1],ES
		PUSH	ES
		POP	DS
		RETF
  @@LoadOvlRet:	MOV	AH,62h
		INT	21h
		CLI
		MOV	SS,BX
		MOV	SP,CS:StackPoint
		STI
		CLD
		CLC
  @@Exit:	MOV	AL,0FFh
		PUSH	CS
		POP	DS
		RET
  @@ErrorEMS:	POP	AX
		POP	AX
		MOV	DX,ErrorEMS_S-LoadOvlEMS
		ADD	DX,DS:LoadOverlay
		MOV	AX,OFFSET OutErr
		CALL	AX
		MOV	AL,LoadEMS_E
		STC
		RET
LoadOvlEMS	ENDP

DoExitEMS	PROC	NEAR
		POP	AX
		MOV	DX,CS:CodeHandle
		MOV	AH,45h
		CALL	CallEMS
		PUSH	CS:Vect22.Segm
		PUSH	CS:Vect22.Offs
		PUSH	WORD PTR CS:rBreakFlag
		MOV	DX,CS:DataHandle
		MOV	AX,OFFSET NewDoExitEMS-Load55+ProcsEMS
		PUSH	CS:[0Ah].Segm
		PUSH	AX
		RETF
DoExitEMS	ENDP

CallEMS		PROC	NEAR
		PUSH	AX
		INT	67h
		CMP	AH,82h
		JNE	@@Exit
		POP	AX
		JMP	CallEMS
  @@Exit:	INC	SP
		INC	SP
		OR	AH,AH
		RET
CallEMS		ENDP

LenSubsEMS	=	$ - SubsEMS

;---------------------- Load as overlay ------------------------
		DB	13,10,13,10,'INIT',13,10

Init00:		CLD
		MOV	AX,DS
		DEC	AX
		MOV	DS,AX
		MOV	AX,DS:[3]
		CMP	AH,10h
		JB	Init01
		MOV	AX,0FFFh
Init01:		MOV	CL,4
		SHL	AX,CL
		MOV	SP,AX
		PUSH	ES
		POP	DS
		MOV	SI,80h
		MOV	DI,OFFSET OvlPrm
		MOV	CX,OvlPrm_L-4-1
		REPE	CMPSB
		JNE	Init20
		CMP	BYTE PTR DS:[SI+4],13
		JNE	Init20
		MOV	DS,DS:[2Ch]		; Name of this program in env
		XOR	SI,SI
Init02:		LODSW
		DEC	SI
		OR	AX,AX
		JNE	Init02
		LEA	DX,[SI+3]
		MOV	AX,3D40h		; Read header of overlay
		INT	21h
		JC	Init14
		PUSH	CS
		POP	DS
		MOV	BX,AX
		MOV	SI,OFFSET TmpOvlBuffer
		CALL	Init15
		JC	Init13
		MOV	AX,4200h
		INT	21h
		JC	Init13
		CALL	Init15
		JC	Init13
		CMP	DS:[SI+8],1000h		; Check size of this segment
		JAE	Init13
		CMP	CX,10h
		JAE	Init13
		MOV	AX,DX
		MOV	DX,CX
		MOV	CL,16
		DIV	CX
		PUSH	AX
		MOV	DI,OFFSET Init15-Init05-1
		MOV	CL,4
		SHR	DI,CL
		ADD	DI,LenStk/16+1
		ADD	AX,DI
		SHL	DI,CL
		MOV	BP,DS:[SI+10h]
		SHR	BP,CL
		ADD	BP,DS:[SI+0Eh]
		CMP	BP,AX
		JAE	Init03
		MOV	BP,AX
Init03:		ADD	BP,10h+1
		PUSH	BX
		MOV	BX,0FFFFh
		MOV	AH,4Ah
		INT	21h
		MOV	AH,4Ah
		INT	21h
		JC	Init13
		CMP	BP,BX
		POP	BX
		POP	AX
		JBE	Init04
		PUSH	AX			; Create new segment
		PUSH	BX
		MOV	BX,0FFFFh
		MOV	AH,48h
		INT	21h
		MOV	AH,48h
		INT	21h
		JC	Init13
		CMP	BP,BX
		JA	Init13
		POP	BX
		POP	BP
		DEC	AX			; Move code to new segment
		MOV	ES,AX
		INC	AX
		MOV	ES:[1],AX
		MOV	CX,DS:[2Ch]
		DEC	CX
		MOV	ES,CX
		MOV	ES:[1],AX
		MOV	CX,DS
		DEC	CX
		MOV	ES,CX
		MOV	ES:[1],AX
		MOV	ES,AX
		PUSH	DI
		PUSH	SI
		XOR	SI,SI
		XOR	DI,DI
		MOV	CX,OFFSET TmpOvlBuffer+20h
		REP	MOVSB
		POP	SI
		POP	DI
		MOV	DS,AX
		MOV	SS,AX
		PUSH	BX
		MOV	BX,AX
		MOV	AH,50h
		INT	21h
		POP	BX
		MOV	AX,BP
		MOV	CX,OFFSET Init04
		PUSH	DS
		PUSH	CX
		RETF
Init04:		MOV	CX,CS			; Move temporary code
		ADD	CX,AX
		ADD	CX,10h+1
		MOV	ES,CX
		CLI
		MOV	SS,CX
		MOV	SP,DI
		STI
		SUB	AX,DS:[SI+8]
		MOV	DI,DX
		MOV	CX,800h
		XOR	DX,DX
		DIV	CX
		MOV	CL,4
		SHL	DX,CL
		ADD	DX,DI
		XOR	DI,DI
		PUSH	DS
		PUSH	AX
		PUSH	ES
		PUSH	DI
		PUSH	SI
		MOV	SI,OFFSET Init05
		MOV	CX,OFFSET Init15-Init05
		REP	MOVSB
		POP	SI
		MOV	AX,ES
		SUB	AX,DS:[SI+8]
		MOV	ES,AX
		MOV	CX,20h
		XOR	DI,DI
		RETF
Init05:		REP	MOVSB			; Load overlay
		PUSH	ES
		POP	DS
		XCHG	DX,DI
		MOV	AX,DS:[8]
		MOV	CL,4
		SHL	AX,CL
		MOV	CX,AX
		SUB	CX,DX
		JBE	Init06
		MOV	AH,3Fh
		INT	21h
		JC	Init13
		CMP	AX,CX
		JNE	Init13
Init06:		POP	CX
		POP	AX
		PUSH	AX
		PUSH	DS
		ADD	AX,10h
		MOV	DS,AX
		PUSH	DS
		JCXZ	Init08
Init07:		PUSH	CX
		XOR	DX,DX
		MOV	CX,8000h
		MOV	AH,3Fh
		INT	21h
		JC	Init13
		CMP	AX,CX
		JNE	Init13
		MOV	AX,DS
		ADD	AX,800h
		MOV	DS,AX
		POP	CX
		LOOP	Init07
Init08:		MOV	CX,DI
		JCXZ	Init09
		XOR	DX,DX
		MOV	AH,3Fh
		INT	21h
		JC	Init13
		CMP	AX,CX
		JNE	Init13
Init09:		MOV	AH,3Eh
		INT	21h
		JC	Init14
		POP	ES
		POP	DS
		MOV	DI,ES			; Start overlay
		MOV	SI,DS:[18h]
		MOV	CX,DS:[6]
		JCXZ	Init11
Init10:		LODSW
		MOV	BX,AX
		LODSW
		ADD	AX,DI
		MOV	ES,AX
		ADD	ES:[BX],DI
		LOOP	Init10
Init11:		MOV	AX,DS:[0Eh]
		MOV	DX,DS:[10h]
		MOV	BX,DS:[16h]
		MOV	BP,DS:[14h]
		ADD	AX,DI
		ADD	BX,DI
		PUSH	CS
		PUSH	DS
		POP	ES
		POP	DS
		XOR	DI,DI
		PUSH	ES
		PUSH	DI
		MOV	SI,OFFSET Init12-Init05
		MOV	CX,OFFSET Init13-Init12
		REP	MOVSB
		RETF
Init12:		POP	DS
		PUSH	DS
		POP	ES
		CLI
		MOV	SS,AX
		MOV	SP,DX
		STI
		PUSH	BX
		PUSH	BP
		RETF

Init13:		MOV	AH,3Eh			; Error loading overlay
		INT	21h
Init14:		MOV	AX,4CFFh
		INT	21h

Init15		PROC	NEAR
		MOV	DX,SI
		MOV	CX,20h
		MOV	AH,3Fh
		INT	21h
		JC	Init18
		CMP	AX,CX
		JNE	Init18
		LODSW
		CMP	AX,'ZM'
		JE	Init16
		CMP	AX,'MZ'
		JNE	Init18
Init16:		LODSW
		MOV	CX,AX
		LODSW
		JCXZ	Init17
		DEC	AX
Init17:		SHL	AX,1
		JC	Init18
		MOV	SI,DX
		MOV	DL,0
		MOV	DH,AL
		ADD	DX,CX
		MOV	CL,AH
		MOV	CH,0
		RET
Init18:		STC
		RET
Init15		ENDP

;----------------------- Output Report -------------------------
Init20:		MOV	BX,0FF10h		; Check text mode
		MOV	AH,12h
		INT	10h
		MOV	DL,25
		CMP	BX,0FF10h
		JE	Init21
		PUSH	ES
		MOV	BH,2
		MOV	AX,1130h
		INT	10h
		POP	ES
		INC	DX
		CMP	DL,25
		JB	Init22
Init21:		MOV	AH,0Fh
		INT	10h
		CMP	AH,80
		JB	Init22
		MOV	AL,DL
		MUL	AH
		SHL	AX,1
		SHL	AX,1
		XOR	BP,BP
		MOV	DS,BP
		CMP	AX,DS:[44Ch]
		JA	Init23
Init22:		MOV	AX,3			; Init video mode
		INT	10h
Init23:		PUSH	CS			; Title message
		POP	DS
		MOV	DX,OFFSET Title_S
		MOV	AH,9
		INT	21h
		MOV	AH,30h			; Check DOS version
		INT	21h
		MOV	SI,OFFSET Version_S
		CMP	AL,2
		JB	Init24
		XCHG	AL,AH
		CMP	AX,314h
		JAE	Init28
		MOV	DX,SI
		CALL	OutErr
		MOV	AX,4C00h+Version_E
		INT	21h
Init24:		LODSB
		CMP	AL,0
		JE	Init25
		MOV	DL,AL
		MOV	AH,6
		INT	21h
		JMP	Init24
Init25:		INT	20h

Init26:		MOV	BP,OFFSET MemoryErr_S	; Allocation error message
Init27:		MOV	DX,BP
		CALL	OutErr
		MOV	AX,4C00h+Alloc_E
		INT	21h

Init28:		CMP	SP,OFFSET Last+LenStk	; Check free memory
		JB	Init26
		XOR	BP,BP
		CALL	SetStat

;---------------------- Check environment ----------------------
		MOV	DS,DS:[2Ch]		; Name of this program in env
		XOR	SI,SI
Init30:		LODSW
		DEC	SI
		OR	AX,AX
		JNE	Init30
		ADD	SI,3
		MOV	BP,OFFSET ErrorInOvl_S	; Check overlay in this file
		MOV	DX,SI
		MOV	AX,3D40h
		INT	21h
		PUSH	DS
		PUSH	CS
		POP	DS
		JC	Init27
		MOV	BX,AX
		MOV	CX,6
		MOV	DX,OFFSET HeaderBuffer
		MOV	AH,3Fh
		INT	21h
		JC	Init31
		CMP	AX,CX
		JB	Init31
		XOR	DX,DX
		XOR	CX,CX
		MOV	AX,4202h
		INT	21h
Init31:		PUSHF
		POP	DI
		PUSH	AX
		MOV	AH,3Eh
		INT	21h
		POP	BX
		JC	Init27
		PUSH	DI
		POPF
		JC	Init27
		MOV	AX,DS:HeaderBuffer[0]
		CMP	AX,'ZM'
		JE	Init32
		CMP	AX,'MZ'
		JNE	Init33
Init32:		XOR	CX,CX
		MOV	AX,DS:HeaderBuffer[4]
		CMP	DS:HeaderBuffer[2],1
		CMC
		SBB	AX,CX
		SHL	AX,1
		ADC	CH,CH
		MOV	CL,AH
		MOV	AH,AL
		MOV	AL,0
		ADD	AX,DS:HeaderBuffer[2]
		CMP	CX,DX
		JB	Init34
		JA	Init33
		CMP	AX,BX
		JB	Init34
Init33:		XOR	AX,AX
		XOR	CX,CX
Init34:		MOV	DS:OvlOffset.Lo,AX
		MOV	DS:OvlOffset.Hi,CX
		OR	CX,AX
		MOV	DS:rOvlName,SI
		POP	DS
Init35:		MOV	DI,SI			; Length of overlay name
Init36:		LODSB
		CMP	AL,'.'
		JE	Init35
		CMP	AL,0
		JNE	Init36
		JCXZ	Init37
		MOV	DI,SI
Init37:		SUB	DI,CS:rOvlName
		MOV	CS:rOvlLength,DI
		XOR	DX,DX			; Offset of COMSPEC=
		MOV	DI,OFFSET Comspec
		MOV	CX,Comspec_L
		CALL	EnvVar
		JC	Init38
		MOV	CS:rComSpec,SI
Init38:		MOV	DI,OFFSET VcEnv		; Path of the home directory
		MOV	DX,OFFSET TmpMainPath
		MOV	CX,VcEnv_L
		CALL	EnvVar
		JNC	Init41
		MOV	DI,DX
		MOV	SI,CS:rOvlName
		MOV	CX,LenPath-1
Init39:		MOV	BX,DI
Init40:		LODSB
		STOSB
		CMP	AL,':'
		JE	Init39
		CMP	AL,'\'
		JE	Init39
		CMP	AL,'/'
		JE	Init39
		CMP	AL,0
		LOOPNE	Init40
		MOV	DI,BX
		MOV	BYTE PTR ES:[DI],0
Init41:		PUSH	DX			; Path and name of swap file
		PUSH	DI
		MOV	DX,OFFSET TmpPath
		MOV	DI,OFFSET VcTmpEnv
		MOV	CX,VcTmpEnv_L
		CALL	EnvVar
		JNC	Init42
		MOV	DI,OFFSET TempEnv
		MOV	CX,TempEnv_L
		CALL	EnvVar
		JNC	Init42
		MOV	DI,DX
		MOV	SI,OFFSET TmpMainPath
		POP	CX
		PUSH	CX
		PUSH	DS
		PUSH	CS
		POP	DS
		SUB	CX,SI
		REP	MOVSB
		POP	DS
Init42:		PUSH	ES
		POP	DS
		MOV	SI,OFFSET Swap_F
		MOV	CX,13
		REP	MOVSB
		POP	DI
		POP	DX

;------------------------ Read VC.INI --------------------------
		PUSH	DI
		PUSH	DX
		MOV	CX,DI
		SUB	CX,DX
		MOV	SI,DX
		MOV	DX,OFFSET TmpData.ComnIni.IniFile
		MOV	DI,DX
		REP	MOVSB
		PUSH	DI
		MOV	SI,OFFSET Init_F
		MOV	CX,13
		REP	MOVSB
		POP	DI
		MOV	SI,81h			; Search /INI switch in cmd line
Init50:		LODSB
		CMP	AL,13
		JE	Init57
		CMP	AL,9
		JE	Init50
		CMP	AL,' '
		JE	Init50
		CMP	AL,'/'
		JNE	Init57
		PUSH	DI
		MOV	DI,OFFSET IniKey_S
		CALL	CmpLines
		POP	DI
		MOV	BP,SI
		JE	Init52
Init51:		LODSB
		CMP	AL,13
		JE	Init57
		CMP	AL,9
		JE	Init50
		CMP	AL,' '
		JE	Init50
		CMP	AL,'/'
		JNE	Init51
		DEC	SI
		JMP	Init50
Init52:		LODSB
		CMP	AL,13
		JE	Init54
		CMP	AL,' '
		JE	Init54
		CMP	AL,9
		JE	Init54
		CMP	AL,'/'
		JE	Init54
		CMP	AL,':'
		JE	Init53
		CMP	AL,'\'
		JNE	Init52
Init53:		MOV	DI,DX
Init54:		MOV	SI,BP
Init55:		LODSB
		CMP	AL,13
		JE	Init56
		CMP	AL,' '
		JE	Init56
		CMP	AL,9
		JE	Init56
		CMP	AL,'/'
		JE	Init56
		STOSB
		CMP	DI,OFFSET TmpData.ComnIni.IniFile+LenPath+12-1
		JB	Init55
Init56:		MOV	AL,0
		STOSB				; Read ini file
Init57:		MOV	BP,OFFSET TmpData.ComnIni-2*WCBini-3
		MOV	CX,3+2*WCBini+CmnIni+2+1
		CALL	ReadFile
		MOV	DX,OFFSET NoIni_S
		JC	Init60
		MOV	DX,OFFSET BadIni_S	; Check ini file
		DEC	CX
		SUB	AX,CX
		JNE	Init60
		DEC	CX
		DEC	CX
		MOV	SI,BP
		XOR	BX,BX
Init58:		LODSB
		ADD	BX,AX
		LOOP	Init58
		LODSW
		CMP	AX,BX
		JNE	Init60
		MOV	DI,BP
		MOV	AL,'V'
		MOV	CL,3
		REPE	SCASB
		JNE	Init60
		MOV	SI,DI
		MOV	DI,OFFSET TmpData.WCB1sav
		MOV	CX,WCBini
		PUSH	CX
		REP	MOVSB
		POP	CX
		MOV	DI,OFFSET TmpData.WCB2sav
		REP	MOVSB
		JMP	Init65

;------------------------ Default init -------------------------
Init60:		CALL	OutErr			; Error message
		MOV	DX,OFFSET TmpData.ComnIni.IniFile
		CALL	OutErr
		MOV	DX,OFFSET ErrIni_S
		CALL	OutErr
		MOV	SI,OFFSET DefInit	; Default init
		MOV	DI,OFFSET TmpData.ComnIni
		MOV	CX,DefBlock
		REP	MOVSB
		MOV	SI,OFFSET DefSwitches
		MOV	CX,LenSwtch+2
		REP	MOVSB
		MOV	SI,OFFSET DefDescr
		MOV	CX,LenStr+2
		PUSH	CX
		REP	MOVSB
		POP	CX
		MOV	SI,OFFSET DefDirInfo
		REP	MOVSB
		MOV	SI,OFFSET DefWCB
		MOV	DI,OFFSET TmpData.WCB1sav
		MOV	CX,WCBini
		PUSH	DI
		PUSH	CX
		REP	MOVSB
		MOV	AH,19h
		INT	21h
		MOV	DL,AL
		INC	DX
		ADD	AL,'A'
		MOV	DI,OFFSET TmpData.WCB1sav.WinShortPath
		STOSB
		MOV	AX,'\:'
		STOSW
		MOV	SI,DI
		MOV	AL,0
		STOSB
		MOV	AH,47h
		INT	21h
		POP	CX
		POP	SI
		MOV	DI,OFFSET TmpData.WCB2sav
		REP	MOVSB
		MOV	DS:TmpData.WCB1sav.Visible,CL

;------------------------- Read *.EXT --------------------------
Init65:		POP	DX
		POP	DI
		MOV	SI,OFFSET Ext_F		; Read VC.EXT
		MOV	BP,OFFSET TmpExt
		CALL	ReadExt
		MOV	AX,BP
		SUB	AX,OFFSET TmpData
		MOV	DS:TmpData.ComnIni.ArchPoint,AX
		MOV	SI,OFFSET Arch_F	; Read VCARCH.EXT
		CALL	ReadExt
		MOV	AX,BP
		SUB	AX,OFFSET TmpData
		MOV	DS:TmpData.ComnIni.FiltsPoint,AX
		MOV	SI,OFFSET Filter_F	; Read VCFILTER.EXT
		CALL	ReadExt
		MOV	AX,BP
		SUB	AX,OFFSET TmpData
		MOV	DS:TmpData.ComnIni.HistPoint,AX
		MOV	DI,BP			; Check size of memory
		MOV	AX,2*13
		MUL	DS:TmpData.ComnIni.MinFiles
		OR	DX,DX
		JNE	Init26
		ADD	AX,DI
		JC	Init26
		ADD	AX,DS:TmpData.ComnIni.LenHistory
		JC	Init26
		ADD	AX,DS:TmpData.ComnIni.LenCmdLn
		JC	Init26
		ADD	AX,LenStk
		JC	Init26
		CMP	AX,SP
		JAE	Init26
		XOR	AX,AX
		STOSW
		MOV	CX,DI
		SUB	CX,OFFSET TmpData
		MOV	DS:TmpData.ComnIni.FilesPoint,CX
		MOV	DS:TmpData.ComnIni.CmdLnPoint,CX

;-------------------- Command line switches --------------------
		.CONST
Help_S		DB	13,10,'Usage:     VC [/switches] [startup command]'
		DB	13,10
		DB	13,10,'Switches:  /?             - help screen'
		DB	13,10,'           /std           - force DOS memory mode'
		DB	13,10,'           /swap  /noswap - force or disable swap mode'
		DB	13,10,'           /EMS   /noEMS  - force or disable EMS mode'
		DB	13,10,'           /XMS   /noXMS  - force or disable XMS mode'
		DB	13,10,'           /small /big    - force small or big mode when EMS or XMS not used'
		DB	13,10,'           /TSR   /noTSR  - enable or disable TSR manager'
		DB	13,10,'           /2E    /no2E   - enable or disable using INT 2E to run commands'
		DB	13,10,'           /BW    /LCD    - set black & white or LCD palette'
		DB	13,10,'           /nozoom        - disable zooming boxes'
		DB	13,10,'           /ini:<file>    - use <file> instead of VC.INI'
		DB	13,10
		DB	13,10,'Press Shift to skip startup command'
		DB	13,10,0
BadKey_S	DB	13,10,'Invalid switches'
		DB	13,10,0
Sequence_S	DB	13,10,'Invalid sequence of switches'
		DB	13,10,0
Keys_S		DB	'?'	,0
		DB	'STD'	,0
		DB	'SWAP'	,0
		DB	'EMS'	,0
		DB	'XMS'	,0
		DB	'NOSWAP',0
		DB	'NOEMS'	,0
		DB	'NOXMS'	,0
		DB	'SMALL'	,0
		DB	'BIG'	,0
		DB	'TSR'	,0
		DB	'NOTSR'	,0
		DB	'2E'	,0
		DB	'NO2E'	,0
		DB	'BW'	,0
		DB	'LCD'	,0
		DB	'NOZOOM',0
IniKey_S	DB	'INI:'	,0
		DB	0

		.DATA
KeyTable	DW	0000h, 0183h,0FFFFh, Init77
		DW	0000h, 0200h, 001Fh, KeyStd
		DW	0000h, 0400h, 003Fh, KeySwap
		DW	0000h, 0800h, 035Fh, KeyEMS
		DW	0000h, 1000h, 039Fh, KeyXMS
		DW	0000h, 2000h, 0025h, KeyNoSwap
		DW	0000h, 4000h, 0049h, KeyNoEMS
		DW	0000h, 8000h, 0091h, KeyNoXMS
		DW	0001h, 0000h, 0319h, KeySmall
		DW	0002h, 0000h, 0319h, KeyBig
		DW	0010h, 0000h, 3001h, KeyTSR
		DW	0020h, 0000h, 3001h, KeyNoTSR
		DW	0040h, 0000h,0C001h, Key2E
		DW	0080h, 0000h,0C001h, KeyNo2E
		DW	0100h, 0003h, 0001h, KeyBW
		DW	0200h, 0003h, 0001h, KeyLCD
		DW	0400h, 0004h, 0001h, KeyNoZoom
		DW	0800h, 0008h, 0001h, KeyIni

		.CODE
		PUSH	DI
		XOR	BP,BP
		XOR	DL,DL
		MOV	SI,81h
Init71:		LODSB
		CMP	AL,13
		JE	Init78
		CMP	AL,9
		JE	Init71
		CMP	AL,' '
		JE	Init71
		CMP	AL,'/'
		JNE	Init78
		MOV	DI,OFFSET Keys_S
		MOV	BX,OFFSET KeyTable
Init72:		CALL	CmpLines
		JE	Init74
		ADD	BX,8
		MOV	AL,0
		MOV	CH,0FFh
		REPNE	SCASB
		CMP	AL,DS:[DI]
		JNE	Init72
Init73:		MOV	DX,OFFSET BadKey_S
		MOV	AL,InvalidKey_E
		JMP	Init76
Init74:		TEST	DL,DS:[BX+1]
		JNZ	Init75
		MOV	AL,DS:[BX+3]
		MOV	AH,DS:[BX+0]
		TEST	BP,AX
		JNZ	Init75
		OR	DL,DS:[BX+2]
		OR	BP,DS:[BX+4]
		JMP	DS:[BX+6]
Init75:		MOV	DX,OFFSET Sequence_S
		MOV	AL,KeySequence_E
Init76:		CALL	OutErr
		JMP	Init85
Init77:		LODSB
		CMP	AL,13
		JE	Init78
		CMP	AL,9
		JE	Init71
		CMP	AL,' '
		JE	Init71
		CMP	AL,'/'
		JNE	Init73
		DEC	SI
		JMP	Init71
Init78:		DEC	SI
		JMP	Init79
KeyStd:		MOV	DS:rLocation,BASEMEM
		JMP	Init77
KeySwap:	MOV	DS:rLocation,SWAP
		JMP	Init77
KeyEMS:		MOV	DS:rLocation,EMS
		JMP	Init77
KeyXMS:		MOV	DS:rLocation,XMS
		JMP	Init77
KeyNoSwap:	MOV	DS:TmpData.ComnIni.UseSwap,0
		JMP	Init77
KeyNoEMS:	MOV	DS:TmpData.ComnIni.UseEMS,0
		JMP	Init77
KeyNoXMS:	MOV	DS:TmpData.ComnIni.UseXMS,0
		JMP	Init77
KeySmall:	MOV	DS:TmpData.ComnIni.LoadFl,1
		JMP	Init77
KeyBig:		MOV	DS:TmpData.ComnIni.LoadFl,0
		JMP	Init77
KeyTSR:		MOV	DS:TmpData.ComnIni.ChkTSR,1
		JMP	Init77
KeyNoTSR:	MOV	DS:TmpData.ComnIni.ChkTSR,0
		JMP	Init77
Key2E:		MOV	DS:TmpData.ComnIni.ExecTyp,1
		JMP	Init77
KeyNo2E:	MOV	DS:TmpData.ComnIni.ExecTyp,0
		JMP	Init77
KeyBW:		MOV	DS:TmpData.ComnIni.ColTyp,0
		JMP	Init77
KeyLCD:		MOV	DS:TmpData.ComnIni.ColTyp,2
		JMP	Init77
KeyNoZoom:	MOV	DS:TmpData.ComnIni.ZoomWin,0
		JMP	Init77
KeyIni:		LODSB
		CMP	AL,13
		JE	Init78
		CMP	AL,9
		JE	Init71
		CMP	AL,' '
		JE	Init71
		CMP	AL,'/'
		JNE	KeyIni
		DEC	SI
		JMP	Init71
Init79:		POP	DI

;------------------------ Auto command -------------------------
		.CONST
SkipCmd_S	DB	13,10, 'Skipping startup command.'
		DB	13,10, 0

		.CODE
		MOV	BX,DI
		CMP	BYTE PTR DS:[SI],13
		JE	Init84
		MOV	AH,2
		INT	16h
		TEST	AL,3
		JZ	Init80
		PUSH	DX
		MOV	DX,OFFSET SkipCmd_S
		CALL	OutStd
		POP	DX
		JMP	Init84
Init80:		MOV	CX,LenCmdLine
		CMP	DS:TmpData.ComnIni.ExecTyp,0
		JNZ	Init83
		PUSH	SI
		MOV	SI,OFFSET TmpData.ComnIni.Switches
Init81:		LODSB
		CMP	AL,0
		JE	Init82
		STOSB
		DEC	CX
		JMP	Init81
Init82:		MOV	AL,' '
		STOSB
		DEC	CX
		POP	SI
Init83:		LODSB
		CMP	AL,13
		JE	Init84
		STOSB
		LOOP	Init83
Init84:		XOR	AX,AX
		STOSW
		SUB	DI,OFFSET TmpData
		MOV	DS:DataLength,DI
		TEST	DL,80h			; Output help screen
		JZ	Init86
		CMP	DS:[BX],AL
		JNE	Init75
		MOV	AL,0
Init85:		PUSH	AX
		MOV	DX,OFFSET Help_S
		CALL	OutStd
		POP	AX
		MOV	AH,4Ch
		INT	21h

;-------------------- Initialize main data ---------------------
Init86:		MOV	DS:OvlPrmPoint.Segm,CS
		MOV	DS:rEPB.CmdLine.Segm,DS
		MOV	DS:rEPB.FCB1.Segm,DS
		MOV	DS:rEPB.FCB2.Segm,DS
		MOV	BX,OFFSET TmpData.ComnIni
		MOV	SI,OFFSET TmpData.WCB1sav
		MOV	DI,OFFSET TmpData.WCB2sav
		MOV	AX,OFFSET ArcFiles
		MOV	DS:[SI].DiskBuf,AX
		MOV	DS:[DI].DiskBuf,AX
		MOV	DS:[SI].EndBuf,AX
		MOV	DS:[DI].EndBuf,AX
		XOR	AX,AX
		MOV	DS:[SI].FileCur,AX
		MOV	DS:[DI].FileCur,AX
		MOV	DS:[SI].FileFirst,AX
		MOV	DS:[DI].FileFirst,AX
		MOV	DS:[SI].CrnPath,AL
		MOV	DS:[DI].CrnPath,AL
		MOV	DS:[SI].ArcPnt,AX
		MOV	DS:[DI].ArcPnt,AX
		MOV	DS:[BX].BytesInSwap,AX
		MOV	DS:[BX].SwapWin,AL
		MOV	DS:[BX].TransTblNumb,AL
		MOV	WORD PTR DS:[BX].SourceFile,AX
		MOV	WORD PTR DS:[BX].TargetFile,AX
		MOV	WORD PTR DS:[BX].SrchStr,AX
		MOV	WORD PTR DS:[BX].GotoOffset,AX
		MOV	DS:[BX].RestoreMode,AX
		MOV	DS:[BX].ReleaseData,OFFSET ReleaseProc
		MOV	DS:[BX].LastMnu,1
		DEC	AX
		MOV	DS:[BX].TempDirNumb,AX
		MOV	DS:[BX].List1numb,AX
		MOV	DS:[BX].List2numb,AX
		MOV	DS:[BX].MouseX,AX
		MOV	DS:[BX].MouseY,AX
		MOV	AL,DS:[BX].AutoMnu
		MOV	DS:[BX].MenuFl,AL
		MOV	SI,OFFSET Mask_S+1
		LEA	DI,[BX].SelMas
		MOV	CX,5
		REP	MOVSB
		MOV	SI,OFFSET Mask_S
		LEA	DI,[BX].FindStr
		MOV	CL,6
		REP	MOVSB
		PUSH	BX
		MOV	AH,0Fh
		INT	10h
		POP	BX
		MOV	DS:[BX].ScrMod,AL
		MOV	AL,DS:[BX].ChkTSR
		MOV	DS:rChkTSR,AL
		LEA	DI,[BX].TempDir
		MOV	SI,OFFSET TmpPath
		MOV	CX,LenPath+12
		REP	MOVSB

;------------------- Choose memoty location --------------------
		.CONST
DrvName		DB	'EMMXXXX0'
NameCode_S	DB	'VC0 code'
NameData_S	DB	'VC0 data'
LoadEMS_S	DB	13,10, 0FEh, ' Using EMS',0
LoadXMS_S	DB	13,10, 0FEh, ' Using XMS',0
LoadSwap_S	DB	13,10, 0FEh, ' Using swap file',0
TsrManager_S	DB	13,10, 0FEh, ' TSR manager is enabled'
		DB	13,10, 0
EMSnotPresent_S	DB	13,10, 'Expanded Memory Manager not present.'
		DB	13,10, 0
VersionEMS_S	DB	13,10, 'This program requires EMS 4.0.'
		DB	13,10, 0
InsuffEMS_S	DB	13,10, 'Insufficient EMS space.'
		DB	13,10, 0
XMSnotPresent_S	DB	13,10, 'Extended Memory Manager not present.'
		DB	13,10, 0
VersionXMS_S	DB	13,10, 'This program requires XMS 2.0 or higher.'
		DB	13,10, 0
InsuffXMS_S	DB	13,10, 'Insufficient XMS space.'
		DB	13,10, 0
ErrorXMS_S	DB	13,10, 'Extended Memory status error.'
		DB	13,10, 0
SwapError_S	DB	13,10, 'Error writing swap file.'
		DB	13,10, 0
ErrorInOvl_S	DB	13,10, 'Error reading overlay file.'
		DB	13,10, 0

		.CODE
		MOV	DS:BreakProc,0CFh		; Disable break
		CALL	SetBreak
		MOV	BP,BX
		XOR	DI,DI
		MOV	DS:TmpCode,DI
		CMP	DS:rLocation,SWAP
		JB	Load40
		JE	Load31
		CMP	DS:rLocation,EMS
		JB	Load21
		JE	Load00

;---------------------- Check EMS mode -------------------------
		CMP	DS:[BP].UseEMS,0
		JZ	Load20
Load00:		PUSH	ES			; Check EMS manager
		PUSH	DI
		MOV	AX,3567h
		INT	21h
		MOV	DI,0Ah
		MOV	SI,OFFSET DrvName
		MOV	CX,8
		REPE	CMPSB
		POP	DI
		POP	ES
		MOV	DX,OFFSET EMSnotPresent_S
		JNE	Load11
		MOV	CX,8000h		; Get EMS manager status
Load01:		MOV	AH,40h
		INT	67h
		OR	AH,AH
		JE	Load03
		CMP	AH,82h
		LOOPE	Load01
Load02:		MOV	DX,OFFSET ErrorEMS_S
		JMP	Load11
Load03:		MOV	AH,46h			; Get EMS version
		CALL	CallEMS
		JNE	Load02
		MOV	DX,OFFSET VersionEMS_S
		CMP	AL,40h
		JB	Load11
		MOV	AH,41h			; Get page frame segment
		CALL	CallEMS
		JNE	Load02
		MOV	DS:PageFrame,BX
		CALL	Load80			; Load header of overlay
		JC	Load11
		MOV	BX,DI			; Allocate code handle
		DEC	BX
		MOV	CL,10
		SHR	BX,CL
		INC	BX
		MOV	AH,43h
		CALL	CallEMS
		JE	Load05
Load04:		CMP	AH,86h
		JBE	Load02
		MOV	DX,OFFSET InsuffEMS_S
		JMP	Load11
Load05:		MOV	DS:CodeHandle,DX
		MOV	SI,OFFSET NameCode_S
Load06:		MOV	AX,5301h
		CALL	CallEMS
		CMP	AH,0A1h
		JNE	Load07
		INC	BYTE PTR DS:[SI+2]
		CMP	BYTE PTR DS:[SI+2],'9'
		JBE	Load06
Load07:		PUSH	DI			; Allocate data handle
		CALL	Load82
		MOV	AX,2*13
		MUL	DS:[BP].MinFiles
		ADD	AX,DS:[BP].HistPoint
		ADD	AX,DS:[BP].LenHistory
		ADD	AX,DS:[BP].LenCmdLn
		ADD	AX,DI
		ADD	AX,LenSubsEMS-NumCalls
		POP	DI
		CMP	DS:rChkTSR,0
		JE	Load08
		ADD	AX,1024
Load08:		DEC	AX
		MOV	CL,14
		SHR	AX,CL
		INC	AX
		MOV	DS:[BP].NormalSize,AX
		MOV	BX,AX
		MOV	AH,43h
		CALL	CallEMS
		JNE	Load04
		MOV	DS:DataHandle,DX
		MOV	AL,DS:NameCode_S[2]
		CMP	AL,'9'
		JA	Load09
		MOV	SI,OFFSET NameData_S
		MOV	DS:[SI+2],AL
		MOV	AX,5301h
		CALL	CallEMS
Load09:		CALL	Load81			; Load and move code
		JC	Load11
		DEC	AX
		MOV	CL,6
		SHR	AX,CL
		INC	AX
		MOV	CX,AX
		MOV	DX,DS:CodeHandle
		MOV	AH,47h
		CALL	CallEMS
		JNE	Load02
		XOR	SI,SI
		XOR	BX,BX
		MOV	DI,DS:TmpCode
Load10:		MOV	AX,4400h
		CALL	CallEMS
		JNE	Load02
		PUSH	DS
		PUSH	ES
		PUSH	DI
		PUSH	CX
		MOV	ES,DS:PageFrame
		MOV	DS,DI
		XOR	SI,SI
		XOR	DI,DI
		MOV	CX,2000h
		REP	MOVSW
		POP	CX
		POP	DI
		POP	ES
		POP	DS
		ADD	DI,400h
		INC	BX
		LOOP	Load10
		MOV	AH,48h
		CALL	CallEMS
		JNE	Load02
		MOV	DX,OFFSET LoadEMS_S
		MOV	SI,OFFSET SubsEMS
		MOV	CX,LenSubsEMS
		MOV	AL,3
		JMP	Load41
Load11:		PUSH	DX			; Error
		MOV	DX,DS:CodeHandle
		OR	DX,DX
		JE	Load12
		MOV	AH,45h
		CALL	CallEMS
Load12:		MOV	DX,DS:DataHandle
		OR	DX,DX
		JE	Load13
		MOV	AH,45h
		CALL	CallEMS
Load13:		POP	DX
		CMP	DS:rLocation,EMS
		JNE	Load20
		MOV	AL,InitEMS_E
		JMP	Load37

;----------------------- Check XMS mode ------------------------
Load20:		CMP	DS:[BP].UseXMS,0
		JZ	Load30
		MOV	DS:CodeHandle,0
		MOV	DS:DataHandle,0
Load21:		MOV	AX,4300h		; Get XMS address
		INT	2Fh
		MOV	DX,OFFSET XMSnotPresent_S
		CMP	AL,80h
		JNE	Load27
		PUSH	ES
		MOV	AX,4310h
		INT	2Fh
		MOV	DS:ControlXMS.Offs,BX
		MOV	DS:ControlXMS.Segm,ES
		POP	ES
		MOV	AH,0			; Check XMS version
		CALL	DS:ControlXMS
		MOV	DX,OFFSET VersionXMS_S
		CMP	AH,2
		JB	Load27
		CALL	Load80			; Load overlay header
		JC	Load27
		MOV	DX,DI			; Allocate code handle
		DEC	DX
		MOV	CL,6
		SHR	DX,CL
		INC	DX
		MOV	AH,9
		CALL	DS:ControlXMS
		CMP	AX,1
		JNE	Load23
		MOV	DS:CodeHandle,DX
		MOV	AX,2*13			; Allocate data handle
		MUL	DS:[BP].MinFiles
		ADD	AX,DS:[BP].HistPoint
		ADD	AX,DS:[BP].LenHistory
		ADD	AX,DS:[BP].LenCmdLn
		CMP	DS:rChkTSR,0
		JE	Load22
		ADD	AX,1024
Load22:		DEC	AX
		MOV	CL,10
		SHR	AX,CL
		INC	AX
		MOV	DS:[BP].NormalSize,AX
		MOV	DX,AX
		MOV	AH,9
		CALL	DS:ControlXMS
		CMP	AX,1
		JE	Load25
Load23:		MOV	DX,OFFSET InsuffXMS_S
		CMP	BL,0A0h
		JE	Load27
Load24:		MOV	DX,OFFSET ErrorXMS_S
		JMP	Load27
Load25:		MOV	DS:DataHandle,DX
		MOV	SI,OFFSET ExtMove	; Move data
		MOV	AX,DS:DataLength
		DEC	AX
		OR	AL,1
		INC	AX
		MOV	DS:[SI].LenBlk.Lo,AX
		XOR	AX,AX
		MOV	DS:[SI].LenBlk.Hi,AX
		MOV	DS:[SI].SourceHandle,AX
		MOV	DS:[SI].SourceOffset.Offs,OFFSET TmpData
		MOV	DS:[SI].SourceOffset.Segm,DS
		MOV	DS:[SI].DestHandle,DX
		MOV	DS:[SI].DestOffset.Hi,AX
		CMP	DS:rChkTSR,AL
		JZ	Load26
		MOV	AX,1024
Load26:		MOV	DS:[SI].DestOffset.Lo,AX
		MOV	AH,0Bh
		CALL	DS:ControlXMS
		CMP	AX,1
		JNE	Load24
		CALL	Load81			; Load and move code
		JC	Load27
		MOV	BL,AH
		MOV	BH,0
		MOV	AH,AL
		MOV	AL,0
		MOV	SI,OFFSET ExtMove
		MOV	DS:[SI].LenBlk.Lo,AX
		MOV	DS:[SI].LenBlk.Hi,BX
		MOV	AX,DS:TmpCode
		MOV	DS:[SI].SourceOffset.Offs,0
		MOV	DS:[SI].SourceOffset.Segm,AX
		MOV	AX,DS:CodeHandle
		MOV	DS:[SI].DestHandle,AX
		XOR	AX,AX
		MOV	DS:[SI].DestOffset.Lo,AX
		MOV	DS:[SI].DestOffset.Hi,AX
		MOV	AH,0Bh
		CALL	DS:ControlXMS
		CMP	AX,1
		JNE	Load24
		MOV	DX,OFFSET LoadXMS_S
		MOV	SI,OFFSET SubsXMS
		MOV	CX,LenSubsXMS
		MOV	AL,2
		JMP	Load41
Load27:		PUSH	DX			; Error
		CALL	DoExitXMS
		POP	DX
		CMP	DS:rLocation,XMS
		JNE	Load30
		MOV	AL,InitXMS_E
		JMP	Load37

;---------------------- Check swap mode ------------------------
Load30:		CMP	DS:[BP].UseSwap,0
		JZ	Load40
Load31:		MOV	DX,OFFSET TmpPath	; Open swap file
		MOV	DI,DX
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
Load32:		MOV	CX,20h
		MOV	AH,5Bh
		INT	21h
		JNC	Load34
		CMP	AX,50h
		JNE	Load36
		MOV	SI,DI
		MOV	CX,3
Load33:		DEC	SI
		INC	BYTE PTR DS:[SI]
		CMP	BYTE PTR DS:[SI],'9'
		JBE	Load32
		MOV	BYTE PTR DS:[SI],'0'
		LOOP	Load33
		JMP	Load36
Load34:		MOV	BX,AX			; Write data to swap file
		MOV	CX,DS:DataLength
		MOV	DX,OFFSET TmpData
		MOV	AH,40h
		INT	21h
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		INT	21h
		POP	BX
		POP	DX
		JC	Load35
		MOV	AX,BX
		PUSH	DX
		POPF
		JC	Load35
		CMP	AX,CX
		JNE	Load35
		MOV	DX,OFFSET LoadSwap_S
		MOV	SI,OFFSET SubsSwap
		MOV	CX,LenSubsSwap
		MOV	AL,1
		JMP	Load41
Load35:		MOV	DX,OFFSET TmpPath	; Delete swap file
		MOV	AH,41h
		INT	21h
Load36:		CMP	DS:rLocation,SWAP
		JNE	Load40
		MOV	DX,OFFSET SwapError_S
		MOV	AL,InitSwap_E
Load37:		PUSH	AX			; Error
		CALL	OutErr
		JMP	Exit04

;------------------------ Standard mode ------------------------
Load40:		MOV	DX,OFFSET Empty_S
		MOV	SI,OFFSET SubsRAM
		MOV	CX,LenSubsRAM
		MOV	AL,BASEMEM

Load41:		MOV	DS:rLocation,AL
		CALL	OutStd			; Output status
		MOV	DX,OFFSET TsrManager_S
		CMP	DS:rChkTSR,0
		JNZ	Load42
		MOV	DX,OFFSET CrLf_S
		CMP	DS:rLocation,BASEMEM
		JNZ	Load42
		MOV	DX,OFFSET Empty_S
Load42:		CALL	OutStd
		MOV	AX,DS:TmpCode		; Deallocate TmpCode seg
		OR	AX,AX
		JE	Load43
		PUSH	ES
		MOV	ES,AX
		MOV	AH,49h
		INT	21h
		POP	ES
Load43:		MOV	BX,BP

;----------------------- Move subroutines ----------------------
		CALL	Load82
		PUSH	DI			; Init Call Table
		PUSH	CX
		MOV	CX,NumCalls SHR 1
		MOV	DX,DI
		SUB	DX,SI
		SUB	DX,CX
		SUB	DX,CX
		MOV	DI,OFFSET CallTable
Load44:		LODSW
		ADD	AX,DX
		STOSW
		LOOP	Load44
		POP	CX
		POP	DI
		SUB	CX,NumCalls		; Move subs
		MOV	AX,DI
		ADD	AX,CX
		MOV	DS:DataPoint,AX
		CMP	SI,DI
		JAE	Load45
		MOV	DI,AX
		ADD	SI,CX
		DEC	SI
		DEC	DI
		STD
Load45:		REP	MOVSB
		CLD

;----------------------- Init TSR manager ----------------------
		PUSH	AX
		MOV	AX,OFFSET Exit01
		XCHG	AX,DS:[0Ah].Offs
		MOV	DS:Vect22.Offs,AX
		MOV	AX,DS
		XCHG	AX,DS:[0Ah].Segm
		MOV	DS:Vect22.Segm,AX
		CMP	DS:rChkTSR,CL
		JZ	Load46
		MOV	DS:Counter,CX
		MOV	DS:ReleaseProc,OFFSET Release
		PUSH	ES
		PUSH	BX
		MOV	AX,3521h		; Hook INT 21h
		INT	21h
		MOV	DS:Vect21.Offs,BX
		MOV	DS:Vect21.Segm,ES
		MOV	DX,OFFSET New21
		CALL	SetVect21
		MOV	AX,3527h		; Hook INT 27h
		INT	21h
		MOV	DS:Vect27.Offs,BX
		MOV	DS:Vect27.Segm,ES
		MOV	DX,OFFSET New27
		MOV	AH,25h
		INT	21h
		POP	BX
		POP	ES
Load46:		POP	AX

;----------------------- Start EMS mode ------------------------
		CMP	DS:rLocation,XMS
		JBE	Load60
		PUSH	AX			; Init vars in conv memory
		MOV	DI,OFFSET Vect21EMS
		MOV	SI,OFFSET Vect21
		MOVSW
		MOVSW
		MOVSW
		MOVSW
		MOV	AX,DS:DataHandle
		STOSW
		MOV	AX,DS:CodeHandle
		STOSW
		MOV	DI,OFFSET ProcsEMS	; Move terminate code
		MOV	SI,OFFSET Load55
		MOV	CX,Load56-Load55
		REP	MOVSB
		POP	CX
		MOV	ES,DS:PageFrame
		MOV	DS:rEPB.CmdLine.Segm,ES
		MOV	DS:OvlPrmPoint.Segm,ES
		PUSH	DI			; Move code from RAM to EMS
		PUSH	SI
		MOV	DX,DS:DataHandle
		MOV	AH,47h
		CALL	CallEMS1
		JNE	Load51
		XOR	BX,BX
		MOV	AX,4400h
		CALL	CallEMS1
		JNE	Load51
		XOR	DI,DI
		XOR	SI,SI
		REP	MOVSB
		ADD	SI,DS:DataLength	; Move data from RAM to EMS
		MOV	CL,14
		SHR	SI,CL
		INC	SI
		MOV	CX,SI
		MOV	SI,OFFSET TmpData
		XOR	BX,BX
Load50:		MOV	AX,4400h
		CALL	CallEMS1
		JNE	Load51
		PUSH	CX
		MOV	CX,4000h
		SUB	CX,DI
		REP	MOVSB
		XOR	DI,DI
		POP	CX
		INC	BX
		LOOP	Load50
		XOR	BX,BX
		MOV	AX,4400h
		CALL	CallEMS1
		JNE	Load51
		MOV	AX,ES
		PUSH	DS
		POP	ES
		POP	SI
		POP	DI
		CMP	DS:rChkTSR,CL
		JZ	Load63
		PUSH	AX
		MOV	CX,Load60-Load56
		REP	MOVSB
		MOV	DX,OFFSET New21EMS-Load55+ProcsEMS
		CALL	SetVect21
		MOV	DX,OFFSET New27EMS-Load55+ProcsEMS
		MOV	AX,2527h
		INT	21h
		MOV	DS:[0Ah].Offs,OFFSET TerminateEMS-Load55+ProcsEMS
		POP	AX
		JMP	Load63
Load51:		MOV	DX,OFFSET ErrorEMS_S
		CALL	OutErr
		MOV	AL,InitEMS_E
		JMP	Exit00

Load55:

NewDoExitEMS	PROC	FAR
		MOV	AH,48h			; Terminate code
		CALL	CallEMS1
		MOV	AH,45h
		CALL	CallEMS1
		POP	DX
		MOV	AX,3301h
		INT	21h
		POP	BX
		POP	CX
		POP	AX
		OR	CX,CX
		JNE	@@Exit
		MOV	AH,4Ch
		INT	21h
  @@Exit:	PUSH	CX
		PUSH	BX
		RETF
NewDoExitEMS	ENDP

TerminateEMS	PROC	FAR			; Abnormal termination
		CALL	RestEMS
		MOV	AX,OFFSET Exit01
		PUSH	CS:PageFrame
		PUSH	AX
		RET
TerminateEMS	ENDP

NewExec21EMS	PROC	FAR			; Exec 21 and 2E code
		MOV	AX,4B00h
		INT	21h
		CALL	RestEMS
		RET
NewExec21EMS	ENDP

NewExec2EEMS	PROC	FAR
		MOV	DS:StackPoint,SP
		INT	2Eh
		CALL	RestEMS
		MOV	DS,CS:PageFrame
		PUSH	CS
		CLI
		POP	SS
		MOV	SP,DS:StackPoint
		STI
		RET
NewExec2EEMS	ENDP

RestEMS		PROC	NEAR
		USES	AX
		PUSHF
		XOR	BX,BX
		MOV	DX,CS:DataHandleEMS
		MOV	AX,4400h
		CALL	CallEMS1
		POPF
		RET
RestEMS		ENDP

CallEMS1	PROC	NEAR
		PUSH	AX
		INT	67h
		CMP	AH,82h
		JNE	@@Exit
		POP	AX
		JMP	CallEMS1
  @@Exit:	INC	SP
		INC	SP
		OR	AH,AH
		RET
CallEMS1	ENDP

Load56:

New21EMS	PROC	FAR			; INT 21h
		PUSHF
		PUSH	CX
		CMP	AH,31h
		JE	@@Func31
		CMP	AH,49h
		JE	@@Func49
		CMP	AH,4Bh
		JNE	Old21EMS
		MOV	CX,OFFSET ExeProg
		CALL	ProcEMS
		JMP	Old21EMS
  @@Func31:	MOV	CX,OFFSET AddProg
		CALL	ProcEMS
		JNC	Old21EMS
		MOV	AH,4Ch
		JMP	Old21EMS
  @@Func49:	MOV	CX,OFFSET FreeBlk
		CALL	ProcEMS
  Old21EMS:	POP	CX
		POPF
		JMP	CS:Vect21EMS
New21EMS	ENDP

New27EMS	PROC	FAR			; INT 27h
		PUSH	CX
		ADD	DX,0Fh
		MOV	CL,4
		SHR	DX,CL
		MOV	CX,OFFSET AddProg
		CALL	ProcEMS
		JNC	@@Old27
		MOV	AH,0
		JMP	Old21EMS
  @@Old27:	MOV	CL,4
		SHL	DX,CL
		POP	CX
		JMP	CS:Vect27EMS
New27EMS	ENDP

ProcEMS		PROC	NEAR
		USES	AX,BX
		PUSH	DX
		MOV	DX,CS:CodeHandleEMS
		MOV	AH,47h
		CALL	CallEMS1
		JNE	@@Exit
		XOR	BX,BX
		MOV	DX,CS:DataHandleEMS
		MOV	AX,4400h
		CALL	CallEMS1
		POP	DX
		JNE	@@Restore
		MOV	AX,OFFSET @@Restore-Load55+ProcsEMS
		PUSH	CS
		PUSH	AX
		PUSH	CS:PageFrame
		PUSH	CX
		RETF
  @@Restore:	PUSH	DX
		MOV	DX,CS:CodeHandleEMS
		MOV	AH,48h
		CALL	CallEMS1
  @@Exit:	POP	DX
		RET
ProcEMS		ENDP

;----------------------- Start XMS mode ------------------------
Load60:		JB	Load65
		MOV	DI,AX
		XOR	AX,AX
		CMP	DS:rChkTSR,AL
		JZ	Load61
		MOV	AX,1024
Load61:		MOV	DS:DataPoint,AX

Load62:		MOV	AX,DS
Load63:		MOV	BX,DI
		ADD	BX,LenStk-1
		OR	BL,0Fh
		INC	BX
		MOV	SP,BX
		MOV	CL,4
		SHR	BX,CL
		MOV	SI,OFFSET Load71
		MOV	CX,Load73-Load71
		PUSH	DI
		REP	MOVSB
		RETN

;----------------------- Start swap mode ------------------------
Load65:		CMP	DS:rLocation,SWAP
		JNE	Load70
		MOV	SI,OFFSET TmpPath
		MOV	DI,AX
Load66:		LODSB
		STOSB
		CMP	AL,0
		JNE	Load66
		JMP	Load62

;----------------------- Start RAM mode ------------------------
Load70:		MOV	DI,AX
		MOV	AX,2*13
		MUL	DS:[BX].MinFiles
		ADD	AX,DS:[BX].HistPoint
		ADD	AX,DS:[BX].LenHistory
		ADD	AX,DS:[BX].LenCmdLn
		MOV	BX,DI
		ADD	BX,LenStk
		MOV	DS:DataPoint,BX
		MOV	SP,BX
		ADD	BX,AX
		DEC	BX
		OR	BL,0Fh
		INC	BX
		MOV	DS:TmpData.ComnIni.NormalSize,BX
		MOV	CL,4
		SHR	BX,CL
		MOV	SI,OFFSET Load71
		MOV	CX,Load73-Load71
		PUSH	DI
		REP	MOVSB
		MOV	SI,OFFSET TmpData
		MOV	DI,DS:DataPoint
		MOV	CX,DS:DataLength
		MOV	AX,DS
		RETN
Load71:		MOV	BP,DS:TmpData.ComnIni.CmdLnPoint
		MOV	DH,DS:TmpData.ComnIni.Prompt
		MOV	DL,DS:TmpData.ComnIni.ExecTyp
		CMP	DL,1
		MOV	DL,0
		ADC	DL,DL
		JCXZ	Load72
		REP	MOVSB
Load72:		MOV	DS,AX
		MOV	CX,DS:DataLength
		MOV	AH,4Ah
		INT	21h
		MOV	SI,BP
		MOV	BX,DX
		MOV	AH,0
		MOV	DX,1
		MOV	BP,OFFSET Exec00
		PUSH	DS
		PUSH	BP
		PUSH	DS
		POP	ES
		CLC
		RETF
Load73:

;---------------------------------------------------------------
;	Read header of overlay and allocate memory for code seg
;		   DI - size of TmpCode
;	Return:	   DI - size of TmpCode
;---------------------------------------------------------------
Load80		PROC	NEAR
		OR	DI,DI
		JNE	@@ChkMem
		MOV	DX,OFFSET TmpOvlName
		MOV	DI,DX
		MOV	SI,DS:rOvlName
		MOV	CX,DS:rOvlLength
		CMP	CX,LenPath+12-4
		JA	@@Error
		MOV	DS,DS:[2Ch]
		REP	MOVSB
		PUSH	CS
		POP	DS
		MOV	SI,OFFSET OvlExt_F
		MOV	CX,4
		REP	MOVSB
		XOR	DI,DI
		MOV	AX,3D40h
		INT	21h
		JC	@@Error
		MOV	BX,AX
		MOV	DX,DS:OvlOffset.Lo
		MOV	CX,DS:OvlOffset.Hi
		MOV	AX,4200h
		INT	21h
		JC	@@ExitErr
		MOV	DX,OFFSET TmpData
		ADD	DX,DS:DataLength
		MOV	SI,DX
		MOV	CX,1Ch
		MOV	AH,3Fh
		INT	21h
  @@ExitErr:	PUSHF
		MOV	DX,AX
		MOV	AH,3Eh
		INT	21h
		POP	BX
		JC	@@Error
		PUSH	BX
		POPF
		JC	@@Error
		CMP	DX,CX
		JNE	@@Error
		CMP	WORD PTR DS:[SI],'ZM'
		JNE	@@Error
		MOV	DI,DS:[SI+4]
		MOV	CL,5
		SHL	DI,CL
  @@ChkMem:	CMP	DS:TmpCode,0
		JNE	@@Exit
		MOV	BX,SP
		DEC	BX
		MOV	CL,4
		SHR	BX,CL
		INC	BX
		INC	BX
		MOV	AH,4Ah
		INT	21h
		MOV	BX,DI
		MOV	AH,48h
		INT	21h
		MOV	DX,OFFSET MemoryErr_S
		JC	@@Exit
		MOV	DS:TmpCode,AX
  @@Exit:	RET
  @@Error:	MOV	DX,OFFSET ErrorInOvl_S
		STC
		RET
Load80		ENDP

;---------------------------------------------------------------
;	Read code from overlay
;---------------------------------------------------------------
Load81		PROC	NEAR
		PUSH	DS
		PUSH	DI
		MOV	DX,OFFSET TmpOvlName
		MOV	AX,3D40h
		INT	21h
		JC	@@Exit
		MOV	BX,AX
		MOV	DX,DS:OvlOffset.Lo
		MOV	CX,DS:OvlOffset.Hi
		MOV	AX,4200h
		INT	21h
		JC	@@ExitErr
		MOV	SI,OFFSET TmpData
		ADD	SI,DS:DataLength
		MOV	DX,DS:[SI+4]
		SHL	DX,1
		MOV	DS:CodeLength,DX
		MOV	CL,4
		SHL	DX,CL
		MOV	AX,DS:[SI+10h]
		DEC	AX
		SHR	AX,CL
		INC	AX
		ADD	AX,DS:[SI+0Eh]
		CMP	AX,DX
		JAE	@@BlockSize
		MOV	AX,DX
  @@BlockSize:	ADD	AX,10h
		MOV	DS:CodeSegLength,AX
		MOV	AX,DS:[SI+8]
		SUB	DX,AX
		MOV	DS:RelocPoint,DX
		SHL	AX,CL
		MOV	CX,AX
		MOV	SI,DX
		MOV	DI,DS:TmpCode
		ADD	DX,DI
		MOV	DS,DX
		XOR	DX,DX
		MOV	AH,3Fh
		INT	21h
		JC	@@ExitErr
  @@NextBlock:	MOV	CX,0FFF0h
		TEST	SI,0F000h
		JNZ	@@Read
		MOV	AX,SI
		MOV	CL,4
		SHL	AX,CL
		MOV	CX,AX
  @@Read:	MOV	DS,DI
		MOV	AH,3Fh
		INT	21h
		JC	@@ExitErr
		CMP	AX,CX
		JNE	@@EndRead
		MOV	CL,4
		SHR	AX,CL
		ADD	DI,AX
		SUB	SI,AX
		JA	@@NextBlock
  @@EndRead:	CLC
  @@ExitErr:	PUSHF
		MOV	AH,3Eh
		INT	21h
		POP	BX
		JC	@@Exit
		PUSH	BX
		POPF
  @@Exit:	POP	DI
		POP	DS
		MOV	DX,OFFSET ErrorInOvl_S
		MOV	AX,DS:CodeLength
		RET
Load81		ENDP

;---------------------------------------------------------------
;	Return:	   DI - address for move subs
;---------------------------------------------------------------
Load82		PROC	NEAR
		MOV	DI,OFFSET BegRelease
		CMP	DS:rChkTSR,0
		JZ	@@Exit
		MOV	DI,OFFSET PrgList
		MOV	AL,DS:[BX].MaxTSRs
		MOV	AH,0
		ADD	AX,AX
		ADD	DI,AX
  @@Exit:	RET
Load82		ENDP

;---------------------------------------------------------------
;	Read file
;		DS:BP - buffer
;		DS:DX - full name
;		   CX - length
;	Return:	   CY - if error
;---------------------------------------------------------------
ReadFile	PROC	NEAR
		MOV	AX,3D40h
		INT	21h
		JC	@@Exit
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
		JC	@@Exit
		PUSH	DX
		POPF
  @@Exit:	RET
ReadFile	ENDP

;---------------------------------------------------------------
;	Read extension file
;		DS:SI - filename
;		DS:DI - end of full name
;		DS:BP - buffer
;	Return:	   BP - end of buffer
;---------------------------------------------------------------
ReadExt		PROC	NEAR
		USES	DI
		MOV	CX,13
		REP	MOVSB
		MOV	CX,SP
		SUB	CX,LenStk
		SUB	CX,BP
		JBE	Init26
		MOV	DX,OFFSET TmpMainPath
		CALL	ReadFile
		MOV	DX,AX
		MOV	SI,BP
		MOV	DI,BP
		MOV	AL,1Ah
		JC	@@Exit
		CMP	DX,CX
		JAE	Init26
		ADD	BP,DX
		MOV	BYTE PTR DS:[BP],1Ah
  @@ChkLine:	LODSB
		CMP	AL,13
		JE	@@ChkLine
		CMP	AL,10
		JE	@@ChkLine
		CMP	AL,1Ah
		JE	@@Exit
		CMP	AL,"'"
		JE	@@Comment
		DEC	SI
		CMP	AL,' '
		JE	@@FindCmd
		CMP	AL,9
		JNE	@@CopyLine
  @@FindCmd:	LODSB
		CMP	AL,13
		JE	@@ChkLine
		CMP	AL,10
		JE	@@ChkLine
		CMP	AL,1Ah
		JE	@@Exit
		CMP	AL,' '
		JE	@@FindCmd
		CMP	AL,9
		JE	@@FindCmd
		MOV	AL,9
		STOSB
		DEC	SI
  @@CopyLine:	LODSB
		CMP	AL,1Ah
		JE	@@Exit
		STOSB
		CMP	AL,13
		JE	@@ChkLine
		CMP	AL,10
		JE	@@ChkLine
		JMP	@@CopyLine
  @@Comment:	LODSB
		CMP	AL,13
		JE	@@ChkLine
		CMP	AL,10
		JE	@@ChkLine
		CMP	AL,1Ah
		JNE	@@Comment
  @@Exit:	STOSB
		MOV	BP,DI
		RET
ReadExt		ENDP

;---------------------------------------------------------------
;	Get environment variable
;		   DS - environment segment
;		ES:DI - name of variable
;		ES:DX - buffer for variable or 0
;		   CX - length
;	Return:	   CY - if not found
;		DS:SI - variable if DX=0
;---------------------------------------------------------------
EnvVar		PROC	NEAR
		XOR	SI,SI
  @@Next:	CMP	BYTE PTR DS:[SI],1
		JB	@@Error
		PUSH	DI
		PUSH	SI
		PUSH	CX
		REPE	CMPSB
		POP	CX
		POP	SI
		POP	DI
		JE	@@Found
  @@FindNext:	LODSB
		CMP	AL,0
		JNE	@@FindNext
		JMP	@@Next
  @@Found:	ADD	SI,CX
		OR	DX,DX
		JZ	@@Exit
		MOV	DI,DX
		MOV	CX,LenPath-2
		PUSH	DI
		REP	MOVSB
		MOV	AL,0
		STOSB
		POP	DI
		MOV	CH,0FFh
		REPNE	SCASB
		DEC	DI
		MOV	AL,ES:[DI-1]
		CMP	AL,'\'
		JE	@@Exit
		CMP	AL,'/'
		JE	@@Exit
		MOV	AX,'\'
		STOSW
		DEC	DI
  @@Exit:	CLC
  @@Error:	RET
EnvVar		ENDP

;---------------------------------------------------------------
;	Compare lines
;		ES:DI - ASCIIZ line
;		DS:SI - buffer to compare
;	Return:	   ZR - equal
;---------------------------------------------------------------
CmpLines	PROC	NEAR
		PUSH	DI
		PUSH	SI
  @@Next:	CMP	BYTE PTR ES:[DI],0
		JE	@@Equal
		LODSB
		CMP	AL,'a'
		JB	@@Compare
		CMP	AL,'z'
		JA	@@Compare
		SUB	AL,20h
  @@Compare:	SCASB
		JE	@@Next
		POP	SI
		POP	DI
		RET
  @@Equal:	POP	DI
		POP	DI
		RET
CmpLines	ENDP

		END
