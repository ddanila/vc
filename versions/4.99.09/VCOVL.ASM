;---------------------------------------------------------------
;		   Idea:	15.05.1991
;		  Begin:	16.05.1991
;		Version:	16.07.2000
;---------------------------------------------------------------

		.MODEL	Medium,C
;		.STACK	3072 ; 3K
		.STACK	2304 ; 2.2K
		LOCALS	@@
		JUMPS
Dgroup		GROUP	Subs,_DATA,CONST

		INCLUDE	VCDATA.INC

		.CODE	Subs2
		DB	13,10,13,10,'SUBROUTINES',13,10

		.CODE	Subs
		DB	13,10,13,10,'SUBROUTINES2',13,10
DataSegment	DW	0

		.DATA
		DB	13,10,13,10,'DATA',13,10
ResidentPoint	DD	0
ResidentStack	DD	0
MainStack	DD	0
CallXMS		DD	0
FncTab		DW	4800h, F00_00		; Up
		DW	5000h, F01_00		; Down
		DW	4B00h, F02_00		; Left
		DW	4D00h, F03_00		; Right
		DW	4900h, F04_00		; PgUp
		DW	5100h, F05_00		; PgDn
		DW	4700h, F06_00		; Home
		DW	4F00h, F07_00		; End
		DW	0E08h, F73_00		; BackSpace
		DW	5200h, F08_00		; Ins     - select / unselect
		DW	4E2Bh, F09_00		; Grey +  - select group
		DW	9000h, F09_02		; ^Grey + - select group
		DW	4A2Dh, F10_00		; Grey -  - unselect group
		DW	8E00h, F10_02		; ^Grey - - unselect group
		DW	372Ah, F76_00		; Grey *  - invert group
		DW	9600h, F76_02		; ^Grey * - invert group
		DW	0F09h, F20_00		; Tab     - change active panel
		DW	1C0Dh, F21_00		; Enter   - run / change directory
		DW	000Ah, F27_00		; ^Enter  - history command
		DW	011Bh, F74_00		; Esc     - clear command line / on/off
		DW	000Dh, F78_00		; ^M      - restore marks
		DW	000Fh, F11_00		; ^O      - on/off panels
		DW	0010h, F12_00		; ^P      - on/off unactive panel
		DW	0015h, F15_00		; ^U      - swap panels
		DW	0002h, F16_00		; ^B      - on/off key bar
		DW	0012h, F17_00		; ^R      - re-read
		DW	000Ch, F18_00		; ^L      - info
		DW	001Ah, F79_00		; ^Z      - tree
		DW	0011h, F83_00		; ^Q      - quick view
		DW	8400h, F22_00		; ^PgUp   - CD ..
		DW	7600h, F23_00		; ^PgDn   - change directory
		DW	001Ch, F24_00		; ^\      - CD \
		DW	0005h, F25_00		; ^E      - previous history command
		DW	0018h, F26_00		; ^X      - next history command
		DW	0009h, F80_00		; ^I	  - insert files
		DW	001Bh, F81_00		; ^[	  - insert left path
		DW	001Dh, F82_00		; ^]	  - insert right path
		DW	000Eh, M08_00		; ^N      - long names
		DW	0006h, M18_00		; ^F      - filter
		DW	0003h, M52_00		; ^C      - compare directories
		DW	0008h, F86_00		; ^H      - hidden files
		DW	0001h, F87_00		; ^A      - attributes
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
		DW	5500h, F42_00		; Shift F2  - main menu
		DW	5600h, F43_00		; Shift F3  - view...
		DW	5700h, F44_00		; Shift F4  - edit...
		DW	5800h, F45_00		; Shift F5  - copy...
		DW	5900h, F46_00		; Shift F6  - rename/move...
		DW	5A00h, F47_00		; Shift F7  - make directory...
		DW	5B00h, F48_00		; Shift F8  - delete...
		DW	5C00h, F49_00		; Shift F9  - save setup
		DW	5E00h, F13_00		; ^F1     - on/off left panel
		DW	5F00h, F14_00		; ^F2     - on/off right panel
		DW	6100h, F54_00		; ^F4     - change label
		DW	6200h, F55_00		; ^F5     - copy file
		DW	6300h, F56_00		; ^F6     - rename/move file
		DW	6500h, F58_00		; ^F8     - delete file
		DW	6600h, F59_00		; ^F9     - video mode
		DW	6800h, F61_00		; Alt F1  - change left drive
		DW	6900h, F62_00		; Alt F2  - change right drive
		DW	6A00h, F63_00		; Alt F3  - internal viewer
		DW	6B00h, F64_00		; Alt F4  - internal editer
		DW	6C00h, F65_00		; Alt F5  - memory info
		DW	6D00h, F66_00		; Alt F6  - directory sizes
		DW	6E00h, F67_00		; Alt F7  - file find
		DW	6F00h, F68_00		; Alt F8  - history
		DW	7000h, F69_00		; Alt F9  - EGA lines
		DW	7100h, F60_00		; Alt F10 - Tree
		DW     0FD00h, F39_00		; Alt     - pull down menu
		DW     0FE00h, F75_00		;         - auto CD / quick view
		DW	9B00h, F85_00		; Alt Left  - change name width
		DW	9D00h, F85_01		; Alt Right - change name width
FncTab1		DW	9800h, F84_00		; Alt Up    - change panel height
		DW     0A000h, F84_01		; Alt Down  - change panel height
		DW	0
LineBufs_T	DW	LineBuf1, LineBuf2, LineBuf3, LineBuf4

		.CONST
		DB	13,10,13,10,'TEXT',13,10
OvlPrm		DB	11, 0, VcID
VcEnv		DB	'VC='
ErrorXMS_S	DB	13,10, 'Extended Memory status error.'
		DB	13,10, 0
ErrorEMS_S	DB	13,10, 'Expanded Memory status error.'
		DB	13,10, 0
WrSwapErr_S	DB	13,10, 'Error writing swap file.'
CrLf_S		DB	13,10, 0
BadFile_S	DB	13,10, 'Bad or missing ', 0
Retry_S		DB	13,10, 'Press ENTER to try again or ESC to abort', 13,10,0

Line0_S		DB	'^0E64',0
Line1_S		DB	'^1E64',0
Line2_S		DB	'^2E64',0
Line0sl_S	DB	'^0E32<',0
Line1sl_S	DB	'^1E32<',0
Line2sl_S	DB	'^2E32<',0
Quote0_S	DB	'"^0E62"',0
Quote1_S	DB	'"^1E62"',0
Quote2_S	DB	'"^2E62"',0
Inc0_S		DB	'^0C',0
IncSp_S		DB	' ^0C ',0
Inc0l_S		DB	'^0C64<',0
Inc1l_S		DB	'^1C64<',0
Inc2l_S		DB	'^2C64<',0
Inc3l_S		DB	'^3C64<',0
Ok_S		DB	' Ok '
Empty_S		DB	0

;------------------------- Init overlay ------------------------
		.CODE
		DB	13,10,13,10,'MAIN',13,10
SubsSeg		DW	Dgroup

Start:		CLD
		MOV	AX,DS
		DEC	AX
		MOV	DS,AX
		MOV	ES,CS:SubsSeg		; Check command line
		MOV	SI,90h
		MOV	DI,OFFSET OvlPrm
		MOV	CL,ES:[DI]
		MOV	CH,0
		SUB	CL,4-1
		REPE	CMPSB
		MOV	AL,0FFh
		JNE	Terminate
		LES	DI,DS:[SI]
		CMP	BYTE PTR DS:[SI+4],13
		JNE	Terminate
		MOV	ES:[DI].Offs,OFFSET Init00
		MOV	ES:[DI].Segm,CS
		MOV	DS:[101h],ES		; Split into 2 MCB
		MOV	AL,'M'
		MOV	BX,0Fh
		CLI
		XCHG	AL,DS:[0]
		XCHG	BX,DS:[3]
		SUB	BX,10h
		MOV	DS:[100h],AL
		MOV	DS:[103h],BX
		STI
		MOV	DS,CS:SubsSeg
		MOV	DS:MainStack.Offs,SP
		MOV	DS:MainStack.Segm,SS
		MOV	AL,0
Terminate:	MOV	AH,4Ch
		INT	21h

;------------------- Return to resident part -------------------
QuitVC:		STC
GoResident:	PUSH	CS
		POP	ES
		MOV	DS,CS:SubsSeg
		CLI
		MOV	SP,DS:ResidentStack.Offs
		MOV	SS,DS:ResidentStack.Segm
		STI
		JMP	DS:ResidentPoint

;---------------------------------------------------------------
;	Main entry point
;		   AL - location: RAM/Swap/EMS/XMS
;		   SI - address of data
;		   CX - length of data
;		   DX - data handle
;---------------------------------------------------------------
Init00:		MOV	DS,CS:SubsSeg
		POP	DS:ResidentPoint.Offs
		POP	DS:ResidentPoint.Segm
		MOV	DS:ResidentStack.Offs,SP
		MOV	DS:ResidentStack.Segm,SS
		CLI
		MOV	SP,DS:MainStack.Offs
		MOV	SS,DS:MainStack.Segm
		STI

;------------------- Allocate data segment ---------------------
		CBW
		MOV	BP,AX
		PUSH	CS
		POP	ES
		MOV	BX,0FFFFh		; Check max free space
		MOV	AH,4Ah
		INT	21h
		CMP	AX,8
		JNE	Init01
		MOV	DI,BX
		MOV	AX,SP			; Compress PSP
		DEC	AX
		PUSH	CX
		MOV	CL,4
		SHR	AX,CL
		POP	CX
		INC	AX
		MOV	BX,SS
		ADD	BX,AX
		MOV	AX,CS
		SUB	BX,AX
		MOV	AH,4Ah
		INT	21h
		JC	Init01
		MOV	BX,0FFFFh		; Allocate data segment
		MOV	AH,48h
		INT	21h
		CMP	AX,8
		JNE	Init01
		MOV	AH,48h
		INT	21h
		JNC	Init02
Init01:		MOV	AL,0FFh
		JMP	QuitVC
Init02:		MOV	DS:DataSegment,AX
		MOV	ES,AX
		PUSH	CX			; Check space for restore
		MOV	AX,CX
		DEC	AX
		MOV	CL,4
		SHR	AX,CL
		INC	AX
		POP	CX
		CMP	AX,BX
		JA	Init01

;------------------------ Restore data -------------------------
		PUSH	DI
		PUSH	SI
		PUSH	BP
		PUSH	DX
		PUSH	BX
		MOV	DS,DS:ResidentPoint.Segm
		XOR	DI,DI
		CMP	BP,1			; Standard mode
		JAE	Init05
		REP	MOVSB
		JMP	Init20

Init05:		JA	Init10			; Swap mode
		MOV	DX,SI
		MOV	AX,3D00h
		INT	21h
		JC	Init06
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	BX,AX
		MOV	DX,DI
		MOV	AH,3Fh
		INT	21h
		PUSHF
		POP	DX
		POP	DS
		MOV	AH,3Eh
		INT	21h
		JC	Init06
		PUSH	DX
		POPF
		JC	Init06
		MOV	DX,SI
		MOV	AH,41h
		INT	21h
		JNC	Init20
Init06:		CALL	BadSwapFile
		JMP	Init05

BadSwapFile	PROC	NEAR
		PUSH	DS
		MOV	DS,CS:SubsSeg
		MOV	DX,OFFSET BadFile_S
		CALL	OutErr
		POP	DS
		MOV	DX,SI
		CALL	OutErr
		PUSH	DS
		MOV	DS,CS:SubsSeg
		MOV	DX,OFFSET Retry_S
		CALL	OutErr
		POP	DS
  @@InKey:	MOV	AH,7
		INT	21h
		CMP	AL,0
		JE	@@InExtKey
		CMP	AL,0Dh
		JE	@@Exit
		CMP	AL,1Bh
		JNE	@@InKey
		MOV	AL,LoadSwap_E
		JMP	QuitVC
  @@InExtKey:	MOV	AH,7
		INT	21h
		JMP	@@InKey
  @@Exit:	RET
BadSwapFile	ENDP

Init10:		CMP	BP,3			; XMS mode
		JAE	Init16
		PUSH	ES
		MOV	AX,4310h
		INT	2Fh
		MOV	AX,ES
		MOV	ES,CS:SubsSeg
		MOV	ES:CallXMS.Offs,BX
		MOV	ES:CallXMS.Segm,AX
		POP	DS
		MOV	BX,SI
		XOR	SI,SI
		XOR	AX,AX
		DEC	CX
		OR	CL,1
		INC	CX
		MOV	DS:[SI].LenBlk.Lo,CX
		MOV	DS:[SI].LenBlk.Hi,AX
		MOV	DS:[SI].SourceHandle,DX
		MOV	DS:[SI].SourceOffset.Lo,BX
		MOV	DS:[SI].SourceOffset.Hi,AX
		MOV	DS:[SI].DestHandle,AX
		MOV	DS:[SI].DestOffset.Offs,DI
		MOV	DS:[SI].DestOffset.Segm,DS
		MOV	AH,0Bh
		CALL	ES:CallXMS
		CMP	AX,1
		JNE	Init11
		MOV	BX,DS:ComnIni.NormalSize
		MOV	AH,0Fh
		CALL	ES:CallXMS
		PUSH	DS
		POP	ES
		JMP	Init20
Init11:		MOV	DX,OFFSET ErrorXMS_S
		MOV	AL,InitXMS_E
Init12:		PUSH	AX
		MOV	DS,CS:SubsSeg
		CALL	OutErr
		POP	AX
		JMP	QuitVC

Init15:		MOV	DX,OFFSET ErrorEMS_S	; EMS mode
		MOV	AL,InitEMS_E
		JMP	Init12
Init16:		XOR	BX,BX
Init17:		MOV	AX,4401h
		CALL	CallEMS
		JNE	Init15
		PUSH	CX
		MOV	AX,4000h
		SUB	AX,SI
		CMP	AX,CX
		JAE	Init18
		MOV	CX,AX
Init18:		MOV	AX,CX
		ADD	SI,4000h
		REP	MOVSB
		POP	CX
		XOR	SI,SI
		INC	BX
		SUB	CX,AX
		JA	Init17
		MOV	BX,ES:ComnIni.NormalSize
		MOV	AH,51h
		CALL	CallEMS

Init20:		PUSH	ES			; Load rest of data from swap
		POP	DS
		MOV	SI,OFFSET ComnIni.TempDir
		MOV	CX,DS:ComnIni.BytesInSwap
		JCXZ	Init23
		MOV	DX,SI
		MOV	AX,DS:ComnIni.CmdLnPoint
		ADD	AX,CX
		PUSH	CX
		MOV	CL,4
		SHR	AX,CL
		INC	AX
		POP	CX
		POP	BX
		CMP	AX,BX
		JBE	Init21
		MOV	AH,41h
		INT	21h
		JMP	Init01
Init21:		PUSH	BX
		MOV	AX,3D00h
		INT	21h
		JC	Init22
		INC	CX
		MOV	BX,AX
		MOV	DX,DS:ComnIni.CmdLnPoint
		MOV	AH,3Fh
		INT	21h
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		INT	21h
		POP	AX
		POP	BX
		JC	Init22
		PUSH	BX
		POPF
		JC	Init22
		DEC	CX
		CMP	AX,CX
		JNE	Init22
		ADD	DS:ComnIni.CmdLnPoint,AX
		MOV	DX,SI
		MOV	AH,41h
		INT	21h
		JMP	Init23
Init22:		CALL	BadSwapFile
		JMP	Init20
Init23:		POP	BX

		MOV	CL,4			; Check size of data segment
		MOV	DI,DS:ComnIni.HistPoint
		ADD	DI,DS:ComnIni.LenHistory
		ADD	DI,SIZE MainDat - SIZE Dat - 1
		SHR	DI,CL
		INC	DI
		MOV	AX,DS:WCB1sav.EndBuf
		DEC	AX
		SHR	AX,CL
		INC	AX
		ADD	DI,AX
		MOV	AX,DS:WCB2sav.EndBuf
		DEC	AX
		SHR	AX,CL
		INC	AX
		ADD	DI,AX
		MOV	AX,BX
		SUB	AX,MinView/16
		JBE	Init01
		SUB	AX,DI
		JB	Init01
		MOV	SI,DS:ComnIni.CmdLnPoint ; Move WCB1, WCB2 and files
		MOV	DX,SI
		SUB	DX,DS:ComnIni.FilesPoint
		MOV	AX,2 * WCBsave - 1
		ADD	AX,DX
		PUSH	AX
		MOV	AX,DX
		SHR	AX,CL
		SUB	DI,AX
		INC	DI
		INC	DI
		MOV	CX,DS
		ADD	CX,DI
		INC	CX
		MOV	ES,CX
		POP	DI
		DEC	SI
		PUSH	SI
		MOV	SI,OFFSET ComnIni-1
		MOV	CX,2 * WCBsave
		STD
		REP	MOVSB
		POP	SI
		MOV	CX,DX
		JCXZ	Init24
		REP	MOVSB
Init24:		POP	DX
		POP	AX
		POP	BP

		PUSH	ES			; Restore *.EXT, History
		PUSH	DS
		POP	ES
		MOV	DI,SI
		ADD	DI,SIZE MainDat - SIZE Dat
		MOV	CX,SI
		SUB	CX,SIZE Dat - 1
		REP	MOVSB
		CLD

		XOR	DI,DI			; Restore global vars
		MOV	SI,OFFSET ComnIni
		MOV	CX,CmnSave
		REP	MOVSB
		MOV	DS:DataLocation,AL	; Init global vars
		MOV	DS:DatPoint,BP
		MOV	DS:DatHandle,DX
		MOV	DS:SizeDataSeg,BX
		POP	DS
		POP	AX
		CMP	AX,BX
		JAE	Init25
		MOV	AX,BX
Init25:		MOV	ES:FreeMem,AX
		MOV	AX,SIZE MainDat - SIZE Dat
		ADD	ES:ArchPoint,AX
		ADD	ES:FiltsPoint,AX
		ADD	ES:HistPoint,AX

		XOR	SI,SI			; Restore WCB1 & WCB2
		MOV	DX,ES:CmdLnPoint
		SUB	DX,ES:FilesPoint
		MOV	DI,ES:HistPoint
		ADD	DI,ES:LenHistory
		MOV	BL,3
		JMP	Init31
Init26:		MOV	ES,AX
		XCHG	SI,DX
		XOR	DI,DI
		MOV	CX,WCBsave
		REP	MOVSB
		XCHG	SI,DX
		MOV	DI,OFFSET ArcFiles
		MOV	CX,ES:DiskBuf
		SUB	CX,DI
		JBE	Init27
		REP	MOVSB
Init27:		CMP	DI,ES:EndBuf
		JAE	Init30
		MOV	BP,DI
		ADD	DI,OFFSET NameF
Init28:		LODSB
		CMP	AL,20h
		JB	Init29
		STOSB
		JMP	Init28
Init29:		MOV	CL,3
		SHL	AL,CL
		MOV	ES:[BP].AttrF,AL
		MOV	AL,0
		STOSB
		MOV	AX,DI
		SUB	AX,BP
		SUB	AX,OFFSET NameF
		MOV	ES:[BP].LenShortNamF,AX
		MOV	ES:[BP].LenLongNamF,0
		JMP	Init27
Init30:		MOV	AX,ES:DiskBuf
		ADD	AX,SIZE FilEnt+3
		CMP	AX,DI
		JBE	Init31
		MOV	DI,AX
Init31:		PUSH	ES
		MOV	AX,DI
		DEC	AX
		MOV	CL,4
		SHR	AX,CL
		INC	AX
		MOV	CX,ES
		ADD	AX,CX
		DEC	BL
		JNZ	Init26
		POP	CX
		POP	BX
		POP	DS
		CMP	DS:SwapWin,0
		JZ	Init32
		XCHG	BX,CX
Init32:		MOV	DS:WCB1seg,BX
		MOV	DS:WCB2seg,CX
		MOV	DS:WorkBuffer,AX
		MOV	DS:TreeSeg,0
		MOV	DS:DirinfoSeg,0
		MOV	DS:ViewSeg,0

;------------------------- Set vectors -------------------------
		CALL	HookVct
		PUSH	DS
		POP	ES

;------------------- Get display parameters --------------------
		.CONST
Return_S	DB	13,10, 'Press ENTER to return ', 0
		DB	13,10, 'to the ',PrgName, 0

		.CODE
		PUSH	DS
		MOV	DS:TstEGA,0
		MOV	BX,0FF10h
		MOV	AH,12h
		INT	10h
		MOV	DL,25
		CMP	BX,0FF10h
		JE	Init40
		MOV	DS:TstEGA,1
		PUSH	ES
		MOV	BH,2
		MOV	AX,1130h
		INT	10h
		POP	ES
		INC	DX
		CMP	DL,25
		JB	Init41
Init40:		MOV	AH,0Fh
		INT	10h
		CMP	AH,80
		JB	Init41
		MOV	AL,DL
		MUL	AH
		SHL	AX,1
		SHL	AX,1
		XOR	BP,BP
		MOV	DS,BP
		CMP	AX,DS:[44Ch]
		JBE	Init41
		CMP	ES:RestoreMode,0
		JZ	Init45
		MOV	AL,ES:ScrMod
		MOV	AH,ES:CharSize
		MOV	ES:RestoreMode,AX
		MOV	AH,0Fh
		INT	10h
		CMP	AL,ES:ScrMod
		JNE	Init45
		CMP	AH,ES:Columns
		JNE	Init45
		CMP	DL,ES:Lines
		JNE	Init45
		MOV	ES:RestoreMode,0
		JMP	Init45
Init41:		MOV	DS,CS:SubsSeg
		MOV	CX,2000h
		MOV	AH,1
		INT	10h
		MOV	SI,OFFSET Return_S
		CALL	PrintStr
		MOV	AH,0Fh
		INT	10h
		CMP	AH,80
		JB	Init42
		INC	SI
		INC	SI
Init42:		CALL	PrintStr
Init43:		MOV	AH,7
		INT	21h
		CMP	AL,0
		JNE	Init44
		MOV	AH,7
		INT	21h
		JMP	Init43
Init44:		CMP	AL,0Dh
		JNE	Init43
		PUSH	ES
		POP	DS
		MOV	DS:RestoreMode,0
		CALL	F69_40
Init45:		POP	DS
		MOV	AH,0Fh
		INT	10h
		MOV	DS:ScrMod,AL
		MOV	DS:Columns,AH
		MOV	DS:ScrPage,BH

;-------------------- Correct cursor size ----------------------
		MOV	BL,8
		CMP	DS:ScrMod,7
		JNE	Init46
		MOV	BL,14
Init46:		CMP	DS:TstEGA,0
		JE	Init47
		PUSH	ES
		MOV	BL,2
		MOV	AX,1130h
		INT	10h
		POP	ES
		MOV	BL,CL
Init47:		MOV	BH,0
		MOV	AH,3
		INT	10h
		CMP	CH,BL
		JAE	Init48
		CMP	CL,BL
		JAE	Init48
		CMP	CL,CH
		JAE	Init49
Init48:		MOV	CL,BL
		CMP	BL,11
		CMC
		SBB	CL,1
		MOV	CH,CL
		DEC	CH
		MOV	AH,1
		INT	10h

;---------------------- Initialize data ------------------------
Init49:		MOV	DX,OFFSET Country	; Country
		MOV	AX,3800h
		INT	21h
		MOV	DI,OFFSET TmpBuf	; Lower case table
		MOV	SI,OFFSET LowTab
		MOV	CX,80h
		PUSH	DI
		PUSH	CX
Init50:		MOV	AL,CL
		NEG	AL
		MOV	DS:[SI],AL
		INC	SI
		CALL	DS:Country.CaseMap
		STOSB
		LOOP	Init50
		POP	CX
		POP	SI
		MOV	BH,0
		MOV	AH,80h
Init51:		LODSB
		CMP	AL,AH
		JE	Init52
		CMP	AL,80h
		JB	Init52
		MOV	BL,AL
		MOV	DS:LowTab[BX-128],AH
Init52:		INC	AH
		LOOP	Init51
		CALL	MakeIni			; IniBuffer
		MOV	DS,CS:SubsSeg		; Memorys
		MOV	DS,DS:ResidentPoint.Segm
		MOV	AX,DS:[2]
		MOV	ES:Memorys,AX
		MOV	DS,DS:[2Ch]		; VcDir
		XOR	SI,SI
Init53:		LODSW
		DEC	SI
		OR	AX,AX
		JNE	Init53
		ADD	SI,3
		MOV	CX,LenPath-1
		MOV	DI,OFFSET VcDir
		PUSH	DI
		REP	MOVSB
		STOSB
		POP	SI
		PUSH	DS
		PUSH	ES
		POP	DS
		CALL	CutPath
		MOV	DS:[SI],AL
		POP	DS
		PUSH	ES			; VcSet
		MOV	ES,CS:SubsSeg
		MOV	DI,OFFSET VcEnv
		MOV	CX,3
		XOR	SI,SI
		MOV	AL,0
Init54:		CMP	AL,DS:[SI]
		JE	Init57
		PUSH	DI
		PUSH	SI
		PUSH	CX
		REPE	CMPSB
		POP	CX
		POP	SI
		POP	DI
		JE	Init56
Init55:		LODSB
		CMP	AL,0
		JNE	Init55
		JMP	Init54
Init56:		ADD	SI,CX
		JMP	Init58
Init57:		POP	DS
		PUSH	DS
		MOV	SI,OFFSET VcDir
Init58:		POP	ES
		MOV	DI,OFFSET VcSet
		MOV	CX,LenPath-2
		PUSH	DI
		REP	MOVSB
		MOV	AL,0
		STOSB
		POP	DI
		MOV	CH,0FFh
		REPNE	SCASB
		DEC	DI
		CALL	AddPathSep
		PUSH	ES
		POP	DS
		XOR	AX,AX
		MOV	DS:[DI],AL

		MOV	SI,OFFSET TempDir	; TempDir
		CALL	CutPath
		MOV	DS:[SI],AL
		MOV	DS:DosBuf,AL		; DosBuf
		MOV	DS:DosCol,AL		; DosCol
		MOV	DS:BegCol,AL		; BegCol
		MOV	DS:OutCol,AL		; OutCol
		MOV	DS:ModeFlag,AL		; ModeFlag
		MOV	DS:Quote,AL		; Quote
		MOV	DS:SftErr,AL		; SftErr
		MOV	DS:BlocEr,AL		; BlocEr
		MOV	DS:RescanTree,AL	; RescanTree
		MOV	DS:WinLeave,AL		; WinLeave
		MOV	DS:PanelSize1,AL	; PanelSize1
		MOV	DS:GroupMask[0],AL	; GroupMask
		MOV	DS:WinClose,AL		; WinClose
		MOV	DS:MouseWin,AL		; MouseWin
		MOV	DS:MouseCmd,AX		; MouseCmd
		MOV	DS:TaskStack,AX		; TaskStack
		MOV	DS:KeyStroke,AX		; KeyStroke
		MOV	DS:HstAdr,AX		; HstAdr
		MOV	DS:FindHandle,AX	; FindHandle
		MOV	DS:DirInfoNumb,0FFFFh	; DirInfoNumb
		MOV	DS:BarPrev,0FFh		; BarPrev
		MOV	DS:MouseCur,0FFh	; MouseCur
		MOV	DS:ProcLongNams,1	; ProcLongNams
		XOR	BX,BX			; StdIn, StdFile
		MOV	DL,81h
		MOV	AX,4400h
		INT	21h
		MOV	AL,DL
		AND	AL,80h
		MOV	DS:StdFile,AL
		MOV	AX,100h
		AND	DL,81h
		CMP	DL,81h
		JNE	Init59
		MOV	AX,11FFh		; ExtKeyb
		INT	16h
		SUB	AX,11FFh
		JE	Init59
		MOV	AX,10h
Init59:		MOV	DS:ExtKeyb,AL
		MOV	DS:StdIn,AH
		MOV	AH,19h			; DosChg
		INT	21h
		MOV	DS:DosChg,AL
		MOV	DL,25			; Lines
		MOV	CL,8
		CMP	DS:TstEGA,0
		JZ	Init60
		PUSH	ES
		MOV	BH,0
		MOV	AX,1130h
		INT	10h
		POP	ES
		INC	DX
Init60:		MOV	DS:Lines,DL
		MOV	DS:CharSize,CL
		CALL	ScreenParms
		MOV	CX,'DE'			; Check DESQWIEV
		MOV	DX,'SQ'
		MOV	AX,2B01h
		INT	21h
		INC	AX
		MOV	DS:DesqVie,AL
		MOV	DS:OS2Box,0		; VerDOS
		MOV	BX,0FFFFh
		MOV	AX,3306h
		INT	21h
		CMP	AL,0FFh
		JE	Init61                  ; true DOS version < 5.0
		MOV	AX,BX
		CMP	AX,0FFFFh
		JE	Init61                  ; DOS version -1
		CMP	AH,100
		JAE	Init61                  ; DOS minor version >= 100
		CMP	AL,5
		JB	Init61                  ; DOS major version <= 5
		PUSH	AX
		MOV	AH,0
		MOV	BL,10
		DIV	BL
		OR	AH,AH
		POP	AX
		JNE	Init62                  ; DOS.major != 10 && DOS.major != 20
		MOV	DS:OS2Box,1
		JMP	Init62
Init61:		MOV	AH,30h
		INT	21h
		CMP	AX,4
		JNE	Init62
		CMP	BH,0FFh
		JNE	Init62
		MOV	AH,1
Init62:		MOV	DS:VerDOS,AX
		MOV	SI,DS:WCB1seg		; WCBseg
		MOV	DI,DS:WCB2seg
		PUSH	DS
		MOV	DS,SI
		MOV	DS:ChgFlg,1
		MOV	DS:LongNamFlag,0
		MOV	DS,DI
		MOV	DS:ChgFlg,1
		MOV	DS:LongNamFlag,0
		POP	DS
		CMP	DS:Active,0
		JE	Init63
		XCHG	DI,SI
Init63:		MOV	DS:WCBcrn,SI
		MOV	DS:WCBsec,DI

;------------------------ Set DOS color ------------------------
		MOV	AH,0Fh
		INT	10h
		MOV	AH,3
		INT	10h
		MOV	AH,DS:Lines
		CMP	DH,AH
		JB	Init64
		DEC	AH
		MOV	AL,0
		CALL	Cursor
Init64:		MOV	AX,0E0Dh
		INT	10h
		MOV	AX,0E0Ah
		INT	10h
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

;------------------------- Set cursor --------------------------
Init70:		PUSH	ES
		POP	DS
		MOV	AH,0Fh
		INT	10h
		MOV	AH,3
		INT	10h
		CMP	DS:KeyBar,0
		JZ	Init71
		MOV	AL,DS:Lines
		DEC	AX
		CMP	DH,AL
		JB	Init71
		MOV	DH,AL
		DEC	DH
		MOV	AX,0E0Ah
		INT	10h
Init71:		MOV	AL,DH
		PUSH	DS
		MOV	DS,DS:WCBcrn
		CMP	DS:Visible,0
		POP	DS
		JZ	Init72
		CALL	BegBX
		CALL	SizDX
		ADD	DH,BH
		CMP	AL,DH
		JAE	Init72
		MOV	AL,DH
Init72:		MOV	DS:DosLin,AL
		CALL	HidCur

;------------------------- Save screen -------------------------
		PUSH	DS
		PUSH	ES
		MOV	AL,DS:Columns
		MUL	DS:Lines
		PUSH	AX
		ADD	AX,AX
		DEC	AX
		MOV	CL,4
		SHR	AX,CL
		INC	AX
		MOV	CX,DS:WorkBuffer
		PUSH	CX
		MOV	DS:ScreenBuf,CX
		ADD	CX,AX
		PUSH	CX
		MOV	DS:UserScreen,CX
		ADD	CX,AX
		ADD	CX,AX
		ADD	CX,AX
		ADD	CX,AX
		MOV	DS:WindowsPoint,CX
		MOV	DS:WorkBuffer,CX
		MOV	AX,DS
		ADD	AX,DS:SizeDataSeg
		SUB	AX,MinView/16
		CMP	CX,AX
		JBE	Init73
		CALL	RestVct
		JMP	Init01
Init73:		CALL	SavScr
		POP	ES
		POP	DS
		POP	CX
		XOR	SI,SI
		XOR	DI,DI
		REP	MOVSW
		POP	ES
		POP	DS

;-------------------- Output menu & key bar --------------------
		CALL	IniMous
		MOV	AL,None_M
		CALL	PutMnu
		MOV	AL,Empty_K
		CALL	PutKey

;--------------------- Restore video mode ----------------------
		XOR	AX,AX
		XCHG	AX,DS:RestoreMode
		OR	AX,AX
		JZ	Panels00
		MOV	DS:ScrMod,AL
		MOV	DS:CharSize,AH
		CALL	SavMous
		MOV	AL,0
		CALL	CurMous
		MOV	SI,1
		JMP	F69_10

;----------------------- Output panels -------------------------
Panels00:	CALL	HidCur
		PUSH	ES			; Delete temp files
		POP	DS
		MOV	AX,0FFFFh
		XCHG	AX,DS:TempDirNumb
		MOV	SI,OFFSET SourceFile
		CALL	DelTempArcFile
		MOV	SI,OFFSET List1Numb
		CALL	Panels50
		MOV	SI,OFFSET List2Numb
		CALL	Panels50
		MOV	DS,ES:WCBsec		; Remove Tree/Dirinfo/View segs
		MOV	CL,DS:WinTyp
		MOV	DS,ES:WCBcrn
		MOV	CH,DS:WinTyp
		MOV	BL,1
		CALL	Panels55
		JE	Panels01
		MOV	DI,OFFSET TreeSeg
		CALL	Panels56
Panels01:	MOV	BL,2
		CALL	Panels55
		JE	Panels02
		PUSH	ES
		POP	DS
		MOV	AX,0FFFFh
		XCHG	AX,DS:DirInfoNumb
		MOV	SI,OFFSET DirInfoFile
		CALL	DelTempArcFile
		MOV	DI,OFFSET DirinfoSeg
		CALL	Panels56
Panels02:	MOV	BL,3
		CALL	Panels55
		JE	Panels03
		CALL	DelTempQuickVw
		MOV	DI,OFFSET ViewSeg
		CALL	Panels56
Panels03:	MOV	SI,ES:WCBcrn
		MOV	DI,ES:WCBsec
		MOV	DS,SI
		CMP	CL,CH
		JAE	Panels06
		MOV	DS,DI
		XCHG	SI,DI
Panels06:	CALL	BegBX			; Active panel
		CALL	SizDX
		MOV	BL,0
		MOV	DL,ES:Columns
		MOV	CL,0
		CMP	DS:ChgFlg,CL
		JE	Panels08
Panels07:	MOV	ES:BlocEr,1
		CALL	GetPath
		MOV	ES:BlocEr,0
Panels08:	CALL	PutPrompt
		XOR	AX,AX
		CMP	DS:Visible,AL
		JZ	Panels20
		CALL	Panel
		CMP	DS:ChgFlg,AL
		JZ	Panels14
		CALL	Window
		MOV	CH,0
		CMP	DS:WinTyp,AL
		JNE	Panels09
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		PUSH	SI
		PUSH	CX
		MOV	SI,OFFSET DosShortPath
		MOV	DI,OFFSET WinShortPath
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
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	CH,1
		JE	Panels09
		PUSH	AX
		CALL	DelAllTempArcs
		POP	AX
		MOV	DS:DiskBuf,OFFSET ArcFiles
		MOV	DS:ArcPnt,0
		MOV	CH,0
Panels09:	CMP	ES:PathEr,AL
		JZ	Panels10
		CALL	Panels40
		JMP	Panels13
Panels10:	OR	CL,CL
		JNE	Panels11
		OR	CH,CH
		JZ	Panels11
		MOV	AL,1
Panels11:	CMP	DS:WinTyp,p_File
		JNE	Panels12
		MOV	ES:BlocEr,1
Panels12:	CALL	ReadDir
		MOV	ES:BlocEr,0
		JNC	Panels14
Panels13:	MOV	AH,1
		OR	CL,CL
		JNE	Panels14
		MOV	CL,DS:WinShortPath[0]
Panels14:	CALL	PutPath
		CALL	PutFls
		OR	AH,AH
		JE	Panels20
		CALL	Window
		CMP	DS:WinTyp,p_File
		JNE	Panels20
		MOV	AL,DS:WinShortPath[0]
		CALL	ErrorDrive
		JE	Panels20
		OR	CL,CL
		JNE	Panels15
		MOV	CL,AL
Panels15:	SUB	AL,'A'
		MOV	DL,AL
		MOV	AH,0Eh
		INT	21h
		JMP	Panels07
Panels20:	CALL	InvWCB			; Inactive panel
		CMP	DS:Visible,0
		JZ	Panels30
		MOV	AL,1
		MOV	CL,0
Panels21:	CALL	Panel
		MOV	AH,0
		CMP	DS:ChgFlg,AH
		JE	Panels26
		CALL	Window
		OR	CL,CL
		JNZ	Panels22
		CMP	DS:WinTyp,AH
		JE	Panels23
		CMP	ES:PathEr,AH
		JE	Panels24
Panels22:	CALL	Panels40
		JMP	Panels25
Panels23:	MOV	ES:BlocEr,1
Panels24:	CALL	ReadDir
		MOV	ES:BlocEr,0
		JNC	Panels26
Panels25:	MOV	AH,1
Panels26:	CALL	PutPath
		CALL	PutFls
		CMP	DS:WinTyp,p_File
		JNE	Panels30
		OR	AH,AH
		JE	Panels30
		CALL	Window
		MOV	AL,DS:WinShortPath[0]
		CALL	ErrorDrive
		JE	Panels30
		MOV	SI,OFFSET WinShortPath[3]
		MOV	BYTE PTR DS:[SI],0
		MOV	CL,AL
		MOV	DS:WinShortPath[0],AL
		CALL	Phantom
		JC	Panels27
		SUB	AL,'A'-1
		MOV	DL,AL
		MOV	AH,47h
		MOV	ES:BlocEr,1
		CALL	Intr21
		MOV	ES:BlocEr,0
		JC	Panels27
		MOV	CL,0
Panels27:	MOV	AL,0
		JMP	Panels21
Panels30:	PUSH	ES			; Mode error
		POP	DS
		CMP	DS:ModeFlag,0
		JE	Panels31
		MOV	DS:ModeFlag,0
		XOR	BX,BX
		MOV	DH,DS:Lines
		MOV	DL,DS:Columns
		CALL	Window
		MOV	BP,OFFSET ModeError_B
		CALL	Dialog
Panels31:	MOV	DS,ES:WCBcrn
		JMP	Func02

Panels40	PROC	NEAR
		USES	AX,BX,SI,BP
		XOR	AX,AX
		XOR	BX,BX
		MOV	SI,OFFSET GlobalSegs_L
		CMP	DS:WinTyp,AL		; File panel
		JNE	Panels42
		MOV	DS:FileCur,AX
		MOV	DS:FileFirst,AX
		MOV	AX,DS:DiskBuf
		MOV	DS:EndBuf,AX
		MOV	DS:EndFul,AX
		PUSH	ES
		PUSH	DS
		POP	ES
		MOV	SI,OFFSET WinShortPath
		MOV	DI,OFFSET WinLongPath
		CALL	strcpy
		CMP	DS:WinShortPath[3],0
		JE	Panels41
		CALL	SetParentDir
		MOV	DS:EndBuf,DI
		MOV	DS:EndFul,DI
Panels41:	POP	ES
		JMP	Panels45
Panels42:	CMP	DS:WinTyp,p_Info	; Tree panel
		JAE	Panels43
		MOV	DS:TreeFirst,AX
		MOV	DS:TreeCur,AX
		XCHG	BX,ES:TreeSeg
		CALL	CompressBlock
		PUSH	DS
		MOV	DS,ES:WorkBuffer
		MOV	ES:TreeSeg,DS
		MOV	BP,1
		CALL	AllocateBlock
		CALL	TreeRoot
		POP	DS
		JMP	Panels45
Panels43:	JA	Panels44		; Info panel
		MOV	ES:ClusterSize.Lo,1
		MOV	ES:ClusterSize.Hi,AX
		MOV	ES:TotalClusters.Lo,AX
		MOV	ES:TotalClusters.Hi,AX
		MOV	ES:FreeClusters.Lo,AX
		MOV	ES:FreeClusters.Hi,AX
		MOV	ES:Volume[0],AL
		DEC	AX
		MOV	ES:DirInfoNumb,AX
		XCHG	BX,ES:DirinfoSeg
		CALL	CompressBlock
		JMP	Panels45
Panels44:	XCHG	BX,ES:ViewSeg		; QuickView panel
		CALL	CompressBlock
Panels45:	RET
Panels40	ENDP

Panels50	PROC	NEAR
		MOV	AX,0FFFFh
		XCHG	AX,DS:[SI]
		CMP	AX,0FFFFh
		JE	@@Exit
		MOV	SI,OFFSET List_F
		MOV	DI,OFFSET TmpBuf
		MOV	DX,DI
		CALL	MakePathName
		CALL	ChgDisk
		JC	@@Exit
		MOV	AH,41h
		CALL	Intr21
  @@Exit:	RET
Panels50	ENDP

Panels55	PROC	NEAR			; Check type of panel
		CMP	BL,CL
		JE	@@Exit
		CMP	BL,CH
  @@Exit:	RET
Panels55	ENDP

Panels56	PROC	NEAR			; Remove unused segment
		XOR	BX,BX
		XCHG	BX,ES:[DI]
		MOV	SI,OFFSET GlobalSegs_L
		CALL	CompressBlock
		RET
Panels56	ENDP

;---------------------------------------------------------------
Func01:		CALL	Panel
		CALL	PutPath
		CALL	PutFls
Func02:		XOR	BX,BX
		MOV	DH,ES:Lines
		MOV	DL,ES:Columns
Func03:		CALL	Window
Func04:		MOV	AL,None_M
		CALL	PutMnu
		PUSH	ES
		POP	DS
		MOV	DS:OneFile,0
		MOV	DS:MoveCurFlag,0
		MOV	DS:MouseEnter,0
		MOV	DS:MouseTime.Hi,0FFFFh
Func05:		PUSH	ES			; Auto save setup
		POP	DS
		MOV	CX,CmnIni+2*WCBini+3
		MOV	SI,OFFSET IniBuffer
		MOV	ES,DS:WorkBuffer
		XOR	DI,DI
		PUSH	SI
		PUSH	DI
		REP	MOVSB
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		POP	DI
		POP	SI
		CALL	MakeIni
		CMP	ES:AutoSav,0
		JZ	Func10
		PUSH	SI
		PUSH	DI
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		MOV	CX,LenPath
		MOV	AX,OFFSET WinShortPath[3]
		CMP	DS:Active,0
		JZ	Func06
		ADD	AX,WCBini
Func06:		ADD	SI,AX
		ADD	DI,AX
		REP	MOVSB
		PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		POP	SI
		POP	DI
		MOV	BX,OFFSET [3+2*WCBini].Active
		MOV	AL,ES:[DI+BX]
		MOV	DS:[SI+BX],AL
		MOV	BX,OFFSET [3+2*WCBini].Ctrl_O
		MOV	AL,ES:[DI+BX]
		MOV	DS:[SI+BX],AL
		MOV	CX,CmnIni+2*WCBini+3
		REPE	CMPSB
		JE	Func10
		CALL	HidCur
		MOV	ES:BlocEr,1
		MOV	DX,OFFSET IniFile
		CALL	SaveSet
		MOV	ES:BlocEr,0
Func10:		CALL	Cursor1			; Set cursor
		MOV	DS,ES:WCBcrn
		CMP	ES:MoveCurFlag,0	; Auto CD / Quick View
		JZ	Func19
		CMP	DS:Visible,0
		JZ	Func19
		PUSH	DS
		CALL	InvWCB
		MOV	BH,DS:WinTyp
		POP	DS
		CMP	DS:WinTyp,p_Tree
		JB	Func11			; File panel
		JA	Func19			; Info or QuickView panel
		MOV	SI,OFFSET CrnPath
		CMP	ES:AutoCD,0
		JZ	Func15
		MOV	DI,OFFSET DosShortPath
		CALL	StrLen
		PUSH	SI
		REPE	CMPSB
		POP	SI
		JE	Func15
		JMP	Func17
Func11:		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	Func19
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		PUSH	SI
		CMP	DS:ArcPnt,0
		JNZ	Func12
		MOV	SI,OFFSET WinShortPath
		CALL	strcpy
		JMP	Func13
Func12:		CALL	PutDescriptor
Func13:		POP	SI
		MOV	BL,DS:[SI].AttrF
		ADD	SI,OFFSET NameF
		CALL	ChkUpDir
		JE	Func14
		CALL	AddPathSep
		CALL	strcpy
Func14:		POP	SI
		PUSH	ES
		POP	DS
		AND	BL,18h
		TEST	BL,10h
		JZ	Func16
Func15:		MOV	BL,10h
Func16:		MOV	ES:MoveCurFlag,2
		CMP	BH,3
		JNE	Func19
		MOV	AX,ES:ViewSeg
		OR	AX,AX
		JE	Func17
		PUSH	ES
		MOV	ES,AX
		MOV	BH,ES:ViewAttr
		MOV	DI,OFFSET ViewName1
		CALL	StrLen
		REPE	CMPSB
		POP	ES
		JNE	Func17
		MOV	AL,BH
		AND	BH,18h
		CMP	BL,BH
		JNE	Func17
		AND	AL,11h
		CMP	AL,11h
		JNE	Func19
Func17:		MOV	AH,2Ch
		INT	21h
		ADD	DL,ES:DelayTm[0]
		CMP	DL,100
		JB	Func18
		SUB	DL,100
		INC	DH
Func18:		ADD	DH,ES:DelayTm[1]
		CMP	DH,60
		JB	Func20
		SUB	DH,60
		INC	CX
		CMP	CL,60
		JB	Func20
		SUB	CL,60
		INC	CH
		CMP	CH,24
		JB	Func20
		MOV	CH,0
		JMP	Func20
Func19:		MOV	CX,0FFFFh
		MOV	ES:MoveCurFlag,0
Func20:		MOV	ES:AutoTim.Lo,CX
		MOV	ES:AutoTim.Hi,DX
		MOV	DS,ES:WCBcrn		; Init variables
		MOV	DI,OFFSET TmpBuf
		MOV	CX,512
		MOV	AL,0
		REP	STOSB
		MOV	ES:ZoomOne,AL
		MOV	ES:GroupMask[0],AL
		MOV	ES:HlpPage,Index_H
		XCHG	CH,ES:MenuFl
		MOV	AX,3C00h
		OR	CH,CH
		JNE	Func40
		MOV	DI,OFFSET MouseCur	; Mouse cursor
		MOV	SI,2
		CALL	SizDX
Func21:		CMP	DS:Visible,0
		JZ	Func27
		CALL	BegBX
		MOV	AX,BX
		ADD	AH,DH
		DEC	AH
		STOSW
		MOV	AL,DL
		MOV	AH,1
		STOSW
		MOV	AL,ES:Pointers[2]
		STOSB
		CMP	DS:WinTyp,p_Tree
		JA	Func27			; Info or QuickView panel
		MOV	CX,2
		JE	Func22
		MOV	CH,1
		CMP	ES:MiniSta,0
		JNZ	Func22
		MOV	CL,0
Func22:		ADD	CH,BH
		JE	Func23
		MOV	AL,BL
		MOV	AH,1
		STOSW
		MOV	AL,DL
		MOV	AH,CH
		STOSW
		MOV	AL,ES:Pointers[0]
		STOSB
Func23:		OR	CL,CL
		JE	Func24
		MOV	AX,BX
		ADD	AH,DH
		SUB	AH,CL
		DEC	AH
		STOSW
		MOV	AL,DL
		MOV	AH,CL
		STOSW
		MOV	AL,ES:Pointers[1]
		STOSB
Func24:		CMP	DS:WinTyp,p_File
		JNE	Func27
		CMP	DS:LongNames,0
		JZ	Func27
		DEC	DL
		INC	CH
		MOV	AH,0
Func25:		CALL	NameWidthAL
		INC	AX
		ADD	AH,AL
		CMP	AH,DL
		JB	Func26
		JA	Func27
		CMP	AL,13
		JE	Func27
Func26:		PUSH	AX
		MOV	AL,AH
		ADD	AL,BL
		MOV	AH,CH
		STOSW
		MOV	AH,BH
		ADD	AH,DH
		SUB	AH,CL
		SUB	AH,CH
		DEC	AH
		MOV	AL,1
		STOSW
		MOV	AL,ES:Pointers[3]
		STOSB
		POP	AX
		CMP	DS:BrfFul,0
		JE	Func25
Func27:		CALL	InvWCB
		DEC	SI
		JNZ	Func21
		XOR	AX,AX
		STOSW
		MOV	AX,102h
		STOSW
		MOV	AL,ES:Pointers[6]
		MOV	AH,0FFh
		STOSW
		CALL	GetMous
		JNZ	Mouse01
		XOR	AX,AX
		MOV	ES:MouseWin,AL
		MOV	ES:MouseEnter,AL
		CALL	ResMouseCmd
		MOV	DX,1			; Input
		MOV	AX,168Fh
		INT	2Fh
		MOV	ES:WinClose,1
		PUSH	DS
		MOV	AL,MainNone_K
		CMP	DS:Visible,0
		JZ	Func30
		MOV	AL,MainFile_K
		CMP	DS:WinTyp,p_File
		JE	Func30
		MOV	AL,MainNone_K
		CMP	DS:WinTyp,p_Info
		JB	Func30                  ; File or Tree panel
		JA	Func28			; QuickView panel
		MOV	BX,ES:DirInfoSeg
		OR	BX,BX
		JE	Func30
		MOV	AL,MainDirInfo_K
		MOV	DS,BX
		JMP	Func29
Func28:		MOV	BX,ES:ViewSeg
		OR	BX,BX
		JE	Func30
		MOV	DS,BX
		TEST	DS:ViewAttr,19h
		JNZ	Func30
		MOV	AL,MainView_K
		ADD	BX,(SIZE QuickViewData-1)/16+1
		MOV	DS,BX
		CMP	DS:VwTextHex,0
		JE	Func29
		ADD	AL,4
Func29:		CMP	DS:VwTbl.Translate,OFFSET TransDBCS
		JNE	Func30
		ADD	AL,2
Func30:		CMP	ES:TstEGA,0
		JZ	Func31
		INC	AX
Func31:		POP	DS
		CALL	Input
		PUSH	AX
		XOR	DX,DX
		MOV	AX,168Fh
		INT	2Fh
		MOV	ES:WinClose,0
		POP	AX
		MOV	ES:MouseCur,0FFh
		CMP	AH,0FCh
		JE	Mouse00
		CMP	DS:Visible,0
		JZ	Func40
		CMP	DS:WinTyp,p_Info
		JE	Func32
		JA	Func33			; QuickView panel
		CMP	AL,0			; File/Tree panel
		JNE	Func40
		XCHG	AL,AH
		PUSH	ES
		MOV	ES,CS:SubsSeg
		MOV	DI,OFFSET AltSr_T
		MOV	CX,AltSrLen
		REPNE	SCASB
		POP	ES
		XCHG	AL,AH
		JE	F77_00
		JMP	Func40
Func32:		CMP	AX,3E00h		; Info panel
		JE	Func40
		MOV	BX,ES:DirInfoSeg
		OR	BX,BX
		JE	Func40
		PUSH	DS
		JMP	Func34
Func33:		MOV	BX,ES:ViewSeg		; Quick View panel
		OR	BX,BX
		JE	Func40
		PUSH	DS
		MOV	DS,BX
		TEST	DS:ViewAttr,19h
		JNZ	Func35
		ADD	BX,(SIZE QuickViewData-1)/16+1
Func34:		MOV	DS,BX
		CALL	ViewMain
Func35:		POP	DS
Func40:		CMP	AH,0FCh			; Jump to function
		JE	Mouse00
		MOV	SI,OFFSET FncTab1
		CALL	Case
		CALL	F84_20
		CMP	ES:Quote,0
		JNZ	Func41
		MOV	SI,OFFSET FncTab
		CALL	Case
Func41:		JMP	F73_00

Mouse00:	CALL	GetMous			; Mouse support
Mouse01:	CALL	F84_20
		CMP	ES:MouseWin,0
		JNZ	Mouse25
		CMP	CX,2			; Panels on/off
		JAE	Mouse02
		CALL	MouseCtrlO
		JNC	Func10
		JMP	F11_00

Mouse02:	OR	CH,CH			; Pull down menu
		JE	F39_00
		CALL	BegBX
		CALL	SizDX
		ADD	DH,BH
		DEC	DH
		CMP	CH,DH
		JA	Mouse47

		JNE	Mouse20
		CMP	DS:Visible,0
		JZ	Mouse47
		CALL	InvWCB
		CMP	DS:Visible,0
		JNZ	Mouse10
		CMP	CL,BL
		JB	Mouse47
		ADD	BL,DL
		CMP	CL,BL
		JAE	Mouse47

Mouse10:	MOV	AL,ES:Pointers[2]	; Change size of panels
		CALL	TypMous
		MOV	AL,ES:PanelSize
		MOV	ES:PanelSize1,AL
Mouse11:	CALL	GetMous
		JZ	Mouse12
		CALL	F84_10
		JC	Mouse11
		CALL	LeaveTm
		JMP	Mouse11
Mouse12:	MOV	AL,0
		CALL	TypMous
		CALL	F84_20
		JMP	Func10

Mouse20:	CMP	CL,DL			; Change panel
		JAE	Mouse21
		CMP	ES:Active,0
		JE	Mouse25
		JMP	Mouse22
Mouse21:	CMP	ES:Active,0
		JNE	Mouse25
Mouse22:	PUSH	DS
		CALL	InvWCB
		CMP	DS:Visible,0
		POP	DS
		JZ	Mouse46
		JMP	F20_00

Mouse25:	TEST	AL,3
		JZ	Mouse47
		CMP	DS:Visible,0
		JZ	Mouse47
		CMP	DS:WinTyp,p_Info
		JB	Mouse30			; File/Tree panel
		MOV	AH,0FCh
		JA	Func33
		CALL	SizDX			; Edit dirinfo
		SUB	DH,11
		JBE	Mouse47
		CALL	BegBX
		ADD	BX,0A01h
		DEC	DX
		DEC	DX
		CMP	ES:DirinfoSeg,0
		JE	Mouse26
		MOV	DH,1
Mouse26:	CALL	InWind
		JC	Func32
		MOV	AX,1
		JMP	F34_10

Mouse30:	MOV	BP,AX			; Change width of long name
		CMP	ES:MouseWin,0
		JNZ	Mouse40
		CMP	DS:WinTyp,p_File
		JNE	Mouse40
		CMP	DS:LongNames,0
		JZ	Mouse40
		CALL	BegBX
		MOV	DX,CX
		SUB	DX,BX
		SUB	DH,2
		JB	Mouse40
		CALL	LnsAX
		CMP	DH,AL
		JAE	Mouse40
		CALL	NameWidthAL
		CMP	AL,12
		JA	Mouse31
		PUSH	DX
		PUSH	AX
		MOV	AL,DL
		CALL	SizDX
		DEC	DX
		CMP	AL,DL
		POP	AX
		POP	DX
		JE	Mouse40
Mouse31:	XCHG	AL,DL
		INC	DX
		DIV	DL
		CMP	AL,0
		JE	Mouse40
		OR	AH,AH
		JNE	Mouse40
		CMP	DS:BrfFul,0
		JE	Mouse32
		CMP	AL,1
		JNE	Mouse40
Mouse32:	PUSH	AX
		MOV	AL,ES:Pointers[3]
		CALL	TypMous
		POP	AX
Mouse33:	CALL	BegBX
		SUB	CL,BL
		JAE	Mouse34
		MOV	CL,0
Mouse34:	CALL	SizDX
		DEC	DX
		CMP	CL,DL
		JBE	Mouse35
		MOV	CL,DL
Mouse35:	XCHG	AL,CL
		MOV	AH,0
		DIV	CL
		CMP	AL,13
		JAE	Mouse36
		MOV	AL,13
Mouse36:	DEC	AX
		CMP	AL,DS:NameWidth
		JE	Mouse37
		MOV	DS:NameWidth,AL
		CALL	Panel
		CALL	PutPath
		CALL	PutFls
		CALL	Window
		JMP	Mouse38
Mouse37:	CALL	LeaveTm
Mouse38:	PUSH	CX
		CALL	GetMous
		POP	AX
		JNZ	Mouse33
		MOV	AL,0
		CALL	TypMous
		JMP	Func10

TmpMousePos	EQU	WORD PTR TmpBuf[0]

Mouse40:	OR	ES:MouseWin,1		; Move the cursor
		MOV	ES:TmpMousePos,CX
		CALL	AdrNum
		CALL	BegBX
		SUB	CL,BL
		JB	Mouse46
		MOV	BL,ES:Columns
		SHR	BL,1
		CMP	CL,BL
		JAE	Mouse46
		INC	BH
		CMP	DS:WinTyp,p_File
		JNE	Mouse41
		INC	BH
Mouse41:	DEC	BX
		SUB	CH,BH
		JB	Mouse50
		CMP	CH,AL
		JAE	Mouse51
		CMP	CL,BL
		JAE	Mouse46
		MOV	BH,DS:WinTyp		; Set cutsor
		OR	BH,DS:BrfFul
		JNE	Mouse44
		PUSH	AX
		CALL	NameWidthAL
		INC	AX
		XCHG	AL,BL
		CMP	DS:LongNames,0
		JZ	Mouse42
		PUSH	AX
		PUSH	BX
		DEC	AX
		DIV	BL
		DEC	BX
		DEC	BX
		SUB	BL,AL
		CMP	BL,AH
		POP	BX
		POP	AX
		JB	Mouse43
Mouse42:	DIV	BL
		MUL	BL
Mouse43:	CMP	CL,AL
		POP	AX
		JAE	Mouse46
		XCHG	AL,CL
		DIV	BL
		XCHG	AH,CL
		MUL	AH
		ADD	CH,AL
Mouse44:	OR	CL,CL
		JE	Mouse46
		CALL	ResMouseCmd
		MOV	AL,CH
		MOV	SI,DI
		ADD	SI,AX
		CMP	SI,DX
		JA	Mouse46
		TEST	BP,1
		JNZ	Mouse60
Mouse45:	PUSH	SI
		CALL	AdrNum
		MOV	AX,SI
		POP	SI
		CMP	AX,SI
		JNE	F00_70
		TEST	BP,2
		JZ	Mouse46
		TEST	ES:MouseWin,2
		JZ	F00_70
Mouse46:	CALL	ResMouseCmd
Mouse47:	CALL	LeaveTm
		JMP	Func10

Mouse50:	MOV	SI,4800h		; Scroll up
		MOV	BH,ES:Pointers[0]
		JMP	Mouse52

Mouse51:	MOV	SI,5000h		; Scroll down
		MOV	BH,ES:Pointers[1]
Mouse52:	CMP	ES:MouseCmd,0
		JNZ	Mouse53
		PUSH	AX
		MOV	AL,BH
		CALL	TypMous
		POP	AX
Mouse53:	CALL	MouseRepeat
		JNC	Func10
		CMP	SI,4800h
		MOV	SI,DI
		JE	F00_05
		MOV	BH,DS:WinTyp
		OR	BH,DS:BrfFul
		JNE	Mouse54
		MOV	BX,AX
		CALL	BriefNumFiles
Mouse54:	DEC	AX
		ADD	SI,AX
		CMP	SI,DX
		JBE	Mouse55
		MOV	SI,DX
Mouse55:	JMP	F01_05

Mouse60:	MOV	CX,ES:TmpMousePos	; Double click
		CALL	BegBX
		CALL	SizDX
		INC	BX
		DEC	DX
		DEC	DX
		MOV	BH,CH
		MOV	DH,1
		MOV	AL,DS:WinTyp
		OR	AL,DS:BrfFul
		JNE	Mouse61
		MOV	AL,CL
		SUB	AL,BL
		CBW
		MOV	CL,13
		DIV	CL
		MUL	CL
		ADD	BL,AL
		MOV	DL,12
Mouse61:	CALL	MouseDblClick
		JC	Mouse45
		JNE	F21_10
		JMP	Func04

EmptyProc:	IRET

;---------------------------------------------------------------
;	Run commans from User Menu, Extensions files
;		DS:SI - commands
;		   AL - 1 external viewer/editor, 2 - from archive
;---------------------------------------------------------------
		.CONST
List_F		DB	'VCLIST',0
UserInLine_S	DB	'Input parameter',0

		.DATA
UserInLine_P	DB	Box_C, BoxCu_C, 0, 0
		DW	User_S
		DW	Line0_S
		DW	Empty_S
		DW	0
UserInLine_B	BOXHD	< UserInLine_P, 500h, 0, Dialog_K, Fls_M, None_H, 0 >
		BOXMN	< 1, 0,0,0,0, 305h, LineBuf1, LenStr >
		DB	0
UserInLine_T	DW	TmpBuf

		.CODE
Run_00:		MOV	BX,ES
		MOV	DL,AL
		MOV	DH,0
		MOV	DI,OFFSET UserInLine
		MOV	CX,2*10
		MOV	AX,0FFFFh
		REP	STOSW
		XOR	DI,DI
		MOV	AX,DS
		CMP	AX,ES:WorkBuffer
		JNE	Run_01
		PUSH	ES
		PUSH	DS
		POP	ES
		MOV	DI,SI
		MOV	CH,0FFh
		MOV	AL,1Ah
		REPNE	SCASB
		POP	ES
		MOV	CL,4
		SHR	DI,CL
		INC	DI
Run_01:		ADD	DI,ES:WorkBuffer
		MOV	AX,ES
		ADD	AX,ES:SizeDataSeg
		SUB	AX,DI
		CMP	AX,0FFFh
		JBE	Run_02
		MOV	AX,0FFFh
Run_02:		MOV	CL,4
		SHL	AX,CL
		MOV	BP,AX
		MOV	ES,DI
		XOR	DI,DI
		JMP	Run_05
Run_03:		MOV	AL,DS:[SI]		; Check line start
		CMP	AL,13
		JE	Run_04
		CMP	AL,10
		JE	Run_04
		CMP	AL,' '
		JE	Run_05
		CMP	AL,9
		JE	Run_05
		CMP	AL,"'"
		JNE	Run_72
Run_04:		LODSB				; Skip line
		CMP	AL,13
		JE	Run_03
		CMP	AL,10
		JE	Run_03
		CMP	AL,1Ah
		JNE	Run_04
		JMP	Run_72
Run_05:		CALL	FndLet			; Find begin of command
		CMP	AL,13
		JE	Run_04
		CMP	AL,10
		JE	Run_04
		CMP	AL,1Ah
		JE	Run_72
		MOV	CX,DI			; Check @
		CMP	AL,'@'
		JNE	Run_07
		INC	SI
		CALL	FndLet
		CMP	AL,13
		JE	Run_04
		CMP	AL,10
		JE	Run_04
		CMP	AL,1Ah
		JE	Run_72
		OR	DI,DI
		JE	Run_06
		MOV	AL,'@'
		STOSB
		CMP	DI,BP
		JB	Run_07
		DEC	DI
		JMP	Run_72
Run_06:		OR	DH,1
Run_07:		PUSH	DS
		PUSH	SI
		PUSH	BX
		MOV	DS,BX
		MOV	BX,CX
		MOV	CX,LenCmdLine
		CMP	DS:ExecTyp,0
		JNE	Run_11
		MOV	SI,OFFSET Switches	; Add /C
Run_08:		LODSB
		CMP	AL,0
		JE	Run_10
		STOSB
		DEC	CX
		CMP	DI,BP
		JB	Run_08
Run_09:		MOV	DI,BX
		JMP	Run_71
Run_10:		MOV	AL,' '
		STOSB
		DEC	CX
		CMP	DI,BP
		JAE	Run_09
Run_11:		POP	BX
		POP	SI
		POP	DS
Run_12:		LODSB				; Parse line
		CMP	AL,13
		JE	Run_15
		CMP	AL,10
		JE	Run_15
		CMP	AL,1Ah
		JE	Run_15
		JCXZ	Run_12
		CMP	AL,'!'
		JE	Run_20
		CMP	AL,'%'
		JE	Run_20
		CMP	AL,9
		JNE	Run_14
		MOV	AL,' '
		JMP	Run_14
Run_13:		POP	DS
Run_14:		STOSB
		CMP	DI,BP
		JAE	Run_72
		DEC	CX
		JMP	Run_12
Run_15:		MOV	BYTE PTR ES:[DI],0	; End of command
		INC	DI
		CMP	DI,BP
		JAE	Run_72
		CMP	AL,1Ah
		JNE	Run_03
		JMP	Run_72

Run_20:		MOV	AH,AL
		MOV	AL,DS:[SI]
		CMP	AL,AH			; !!, %%
		JNE	Run_21
		INC	SI
		JMP	Run_14

Run_21:		PUSH	DS
		CMP	AL,'['			; ![...]
		JNE	Run_30
		PUSH	ES
		PUSH	DI
		MOV	ES,BX
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		PUSH	CX
		MOV	CX,64
		MOV	AL,' '
		REP	STOSB
		POP	CX
		POP	DI
		INC	SI
Run_22:		LODSB
		CMP	AL,']'
		JE	Run_24
		CMP	AL,13
		JE	Run_23
		CMP	AL,10
		JE	Run_23
		CMP	AL,1Ah
		JE	Run_23
		STOSB
		JMP	Run_22
Run_23:		DEC	SI
Run_24:		CMP	DI,OFFSET TmpBuf
		JA	Run_25
		PUSH	SI
		MOV	DS,CS:SubsSeg
		MOV	SI,OFFSET UserInLine_S
		CALL	strcpy
		MOV	AL,' '
		STOSB
		POP	SI
Run_25:		MOV	ES:TmpBuf[64],0
		POP	DI
		POP	ES
		PUSH	SI
		PUSH	BX
		PUSH	ES
		PUSH	DX
		MOV	DS,BX
		MOV	ES,BX
		MOV	SI,OFFSET LineBuf1
		MOV	WORD PTR DS:[SI],0
		MOV	DX,OFFSET UserInLine_T
		MOV	BP,OFFSET UserInLine_B
		CALL	Dialog
		MOV	BX,OFFSET UserInLine-4
		JE	Run_26
		POP	AX
		POP	AX
		POP	AX
		POP	AX
		POP	AX
		MOV	DS,ES:WCBcrn
		MOV	AX,ES:List1numb
		CALL	Run_29
		CALL	InvWCB
		MOV	AX,ES:List2numb
		CALL	Run_29
		JMP	Panels00

Run_26:		ADD	BX,4
		CMP	BX,OFFSET UserInLine+10*4
		JAE	Run_27
		CMP	WORD PTR DS:[BX],0FFFFh
		JNE	Run_26
		PUSH	DI
		PUSH	CX
		MOV	DI,SI
		CALL	StrLen
		DEC	CX
		MOV	DS:[BX+2],CX
		POP	CX
		POP	DI
		MOV	DS:[BX+0],DI
Run_27:		POP	DX
		POP	ES
		JMP	Run_70

Run_29		PROC	NEAR
		INC	AX
		JE	@@Exit
		CMP	DS:WinTyp,p_File
		JNE	@@Exit
		CMP	DS:Visible,0
		JE	@@Exit
		CMP	DS:ArcPnt,0
		JNE	@@Exit
		MOV	SI,DS:DiskBuf
  @@Restore:	CMP	SI,DS:EndBuf
		JAE	@@Exit
		TEST	DS:[SI].AttrF,80h
		JZ	@@Next
		OR	DS:[SI].AttrF,40h
  @@Next:	CALL	NextFilEnt
		JMP	@@Restore
  @@Exit:	RET
Run_29		ENDP

Run_30:		CMP	AL,'0'			; !n
		JB	Run_35
		CMP	AL,'9'
		JA	Run_35
		INC	SI
		PUSH	SI
		PUSH	BX
		PUSH	ES
		PUSH	DI
		PUSH	CX
		PUSH	ES
		POP	DS
		MOV	ES,BX
		MOV	DI,OFFSET TmpBuf
		MOV	BYTE PTR ES:[DI],0
		PUSH	DI
		SUB	AL,'0'
		MOV	AH,0
		SHL	AX,1
		SHL	AX,1
		MOV	SI,OFFSET UserInLine
		ADD	SI,AX
		MOV	CX,ES:[SI+2]
		MOV	SI,ES:[SI+0]
		CMP	SI,0FFFFh
		JE	Run_33
		JCXZ	Run_32
Run_31:		LODSB
		CMP	AL,0
		JE	Run_32
		STOSB
		LOOP	Run_31
Run_32:		MOV	AL,0
		STOSB
Run_33:		MOV	DS,BX
		POP	SI
		POP	CX
		POP	DI
		POP	ES
		JMP	Run_70

Run_35:		MOV	DS,BX			; Choose panel
		MOV	DS,DS:WCBcrn
		CMP	AH,'!'
		JE	Run_36
		MOV	DS,BX
		MOV	DS,DS:WCBsec

Run_36:		CMP	AL,':'			; !:
		JNE	Run_40
		MOV	AL,DS:WinShortPath[0]
		CMP	DS:Visible,0
		JZ	Run_37
		CMP	DS:WinTyp,p_File
		JE	Run_13
Run_37:		MOV	DS,BX
		MOV	AL,DS:DosPath[0]
		JMP	Run_13

Run_40:		CMP	AL,'~'			; ~
		JNE	Run_41
		OR	AH,80h
		INC	SI
		PUSH	DS
		PUSH	BP
		MOV	BP,SP
		MOV	DS,[BP+4]
		MOV	AL,DS:[SI]
		POP	BP
		POP	DS
Run_41:		CMP	AL,'\'			; !\
		JE	Run_42
		CMP	AL,'/'			; !/
		JNE	Run_50
Run_42:		AND	AH,80h
		INC	SI
		PUSH	SI
		PUSH	BX
		CMP	DS:Visible,0
		JZ	Run_45
		CMP	DS:WinTyp,p_Tree
		JA	Run_45			; Info or QuickView panel
		JE	Run_44
		CMP	DS:ArcPnt,0		; File panel
		JNE	Run_43
		MOV	SI,OFFSET WinShortPath
		PUSH	DS
		MOV	DS,BX
		CMP	DS:OS2box,0
		POP	DS
		JNZ	Run_46
		TEST	AH,80h
		JNZ	Run_46
		CMP	DS:LongNames,0
		JZ	Run_46
		MOV	SI,OFFSET WinLongPath
		JMP	Run_46
Run_43:		OR	AH,40h
		MOV	SI,OFFSET ArcPath
		JMP	Run_46
Run_44:		MOV	SI,OFFSET CrnPath
		OR	AH,20h
		JMP	Run_46
Run_45:		MOV	DS,BX
		MOV	SI,OFFSET DosPath
		TEST	AH,80h
		JZ	Run_46
		MOV	SI,OFFSET DosShortPath
Run_46:		PUSH	ES
		PUSH	DI
		MOV	ES,BX
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		PUSH	SI
		CALL	strcpy
		POP	SI
		POP	DI
		PUSH	AX
		CMP	BYTE PTR DS:[SI],0
		JE	Run_48
		TEST	AH,40h
		JNZ	Run_47
		CMP	BYTE PTR DS:[SI+3],0
		JE	Run_48
		TEST	AH,20h
		JZ	Run_47
		TEST	AH,80h
		JNZ	Run_47
		CALL	TreePath
Run_47:		PUSH	DI
		PUSH	CX
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		MOV	AX,'\'
		STOSW
		POP	CX
		POP	DI
Run_48:		POP	AX
		MOV	SI,DI
		TEST	AH,40h
		JNZ	Run_49
		INC	SI
		INC	SI
Run_49:		MOV	DS,BX
		POP	DI
		POP	ES
		JMP	Run_70

Run_50:		CMP	AL,'@'			; !@
		JNE	Run_60
		INC	SI
		PUSH	SI
		PUSH	BX
		PUSH	ES
		PUSH	BP
		PUSH	DI
		PUSH	DX
		PUSH	CX
		MOV	ES,BX
		MOV	CL,AH
		MOV	BP,OFFSET List1numb
		AND	AH,7Fh
		CMP	AH,'!'
		JE	Run_51
		MOV	BP,OFFSET List2numb
Run_51:		MOV	DX,OFFSET TmpBuf
		MOV	SI,OFFSET List_F
		MOV	AX,ES:[BP]
		CMP	AX,0FFFFh
		JNE	Run_54
		MOV	DI,DX
		CALL	MoveTempPath
		CALL	OpenTempFile
		JNC	Run_52
		MOV	ES:TmpBuf[0],0
		JMP	Run_55
Run_52:		MOV	ES:[BP],AX
		PUSH	AX
		MOV	AL,0
		TEST	CL,80h
		JNZ	Run_53
		MOV	AL,DS:LongNames
Run_53:		CALL	FileList
		POP	AX
Run_54:		MOV	DI,DX
		CALL	MakePathName
Run_55:		MOV	SI,DX
		PUSH	ES
		POP	DS
		POP	CX
		POP	DX
		POP	DI
		POP	BP
		POP	ES
		JMP	Run_70

Run_60:		MOV	AL,AH			; !, !.!
		AND	AX,7F80h
		PUSH	DS
		PUSH	BP
		MOV	BP,SP
		MOV	DS,[BP+4]
		PUSH	AX
		MOV	AL,'.'
		CMP	AX,DS:[SI]
		POP	AX
		POP	BP
		POP	DS
		JNE	Run_61
		INC	SI
		INC	SI
		OR	AL,1
Run_61:		PUSH	SI
		PUSH	BX
		XCHG	AL,AH
		CMP	AL,'!'
		JNE	Run_62
		CMP	DL,1
		JB	Run_62
		PUSH	ES
		PUSH	DI
		MOV	DS,BX
		MOV	ES,BX
		MOV	DI,OFFSET LineBuf1
		MOV	SI,OFFSET SourceFile
		JE	Run_64
		PUSH	SI
		PUSH	AX
		MOV	SI,OFFSET VcTempArc_S
		MOV	AX,DS:TempDirNumb
		CALL	MakePathName
		CALL	AddPathSep
		POP	AX
		POP	SI
		CALL	CutPath
		JMP	Run_64
Run_62:		CMP	DS:Visible,0
		JZ	Run_11
		CMP	DS:WinTyp,p_File
		JNE	Run_11
		MOV	SI,DS:FileCur
		CALL	Numb2Addr
		CMP	SI,DS:EndBuf
		JAE	Run_11
		PUSH	ES
		PUSH	DI
		MOV	ES,BX
		MOV	DI,OFFSET LineBuf1
		TEST	AH,80h
		JNZ	Run_63
		CMP	ES:OS2box,0
		JNE	Run_63
		CMP	DS:[SI].LenLongNamF,0
		JE	Run_63
		ADD	SI,DS:[SI].LenShortNamF
Run_63:		ADD	SI,OFFSET NameF
		AND	AH,7Fh
Run_64:		CALL	strcpy
		MOV	DS,BX
		MOV	SI,OFFSET LineBuf1
		MOV	DI,OFFSET TmpBuf
		PUSH	DI
		PUSH	SI
		CALL	strcpy
		POP	SI
		POP	DI
		TEST	AH,80h
		JE	Run_65
		PUSH	AX
		CALL	ShortName
		POP	AX
Run_65:		MOV	SI,DI
		POP	DI
		POP	ES
		OR	DL,DL
		JNE	Run_66
		CALL	CutPath
Run_66:		TEST	AH,1
		JNE	Run_70
		PUSH	SI
		CALL	CutPath
		CALL	ChkUpDir
		POP	SI
		JE	Run_70
		PUSH	SI
		CALL	FindFileExt
		CMP	BYTE PTR DS:[SI-1],'.'
		JNE	Run_67
		MOV	BYTE PTR DS:[SI-1],0
Run_67:		POP	SI

Run_70:		LODSB
		CMP	AL,0
		JE	Run_11
		STOSB
		CMP	DI,BP
		JAE	Run_71
		LOOP	Run_70
		JMP	Run_11
Run_71:		POP	BX
		POP	SI
		POP	DS
Run_72:		XOR	AX,AX			; End of batch
		STOSW
		MOV	AX,ES
		MOV	DS,BX
		CMP	AX,DS:WorkBuffer
		JBE	Run_73
		PUSH	ES
		MOV	ES,DS:WorkBuffer
		MOV	CX,DI
		XOR	SI,SI
		XOR	DI,DI
		POP	DS
		REP	MOVSB
Run_73:		PUSH	ES
		POP	DS
		MOV	ES,BX
		XOR	SI,SI
		CMP	ES:ExecTyp,0
		JNE	Run_74
		MOV	DI,OFFSET Switches
		CALL	StrLen
		MOV	SI,CX
Run_74:		MOV	DI,OFFSET DosBuf
		PUSH	DI
		CALL	strcpy
		POP	DI
		CALL	StrLen
		MOV	AX,CX
		MOV	CL,DH
		AND	CL,1
		DEC	AX
		MOV	ES:DosCol,AL
		JNE	RunCmd05
		CALL	DosCom
		CALL	Cursor1
		JMP	Func04

		.CONST
Swap_F		DB	'VCSWAP',0
ManyData1_S	DB	"Too many data to save state.",0
ManyData2_S	DB	"Can't run this command.",0

		.DATA
ManyData_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Error_S
		DW	ManyData1_S
		DW	ManyData2_S
		DW	Ok_S
		DW	0
ManyData_B	BOXHD	< ManyData_P, 900h, 0, Error_K, Def_M, None_H, 2 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0>
		DB	0
OkMn_D		DB	4, 0
		DB	0

		.CODE
RunCmd00:	PUSH	ES			; Run command line
		POP	DS
		XOR	DI,DI
		MOV	ES,DS:WorkBuffer
		CMP	DS:ExecTyp,0
		JNZ	RunCmd01
		MOV	SI,OFFSET Switches
		CALL	strcpy
		MOV	AL,' '
		STOSB
RunCmd01:	MOV	SI,OFFSET DosBuf
		MOV	CX,LenCmdLine
		SUB	CX,DI
RunCmd02:	LODSB
		STOSB
		CMP	AL,0
		LOOPNE	RunCmd02
		XOR	AX,AX
		STOSW
		MOV	CL,0
		PUSH	DS
		POP	ES
RunCmd05:	CALL	DelTempQuickVw		; Delete temp arc files
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET DirInfoFile
		MOV	AX,DS:DirInfoNumb
		CALL	DelTempArcFile
		MOV	SI,DS:HistPoint		; Calculate size of data
RunCmd06:	LODSW
		DEC	SI
		OR	AX,AX
		JNE	RunCmd06
		ADD	SI,OFFSET CmnSave - SIZE MainDat + 2
		MOV	BX,SI
		MOV	DS,ES:WCB1seg
		MOV	AL,2
RunCmd07:	MOV	SI,DS:DiskBuf
		ADD	BX,SI
		JC	RunCmd11
RunCmd08:	CMP	SI,DS:EndBuf
		JAE	RunCmd09
		ADD	BX,DS:[SI].LenShortNamF
		JC	RunCmd11
		CALL	NextFilEnt
		JMP	RunCmd08
RunCmd09:	MOV	DS,ES:WCB2seg
		DEC	AL
		JNE	RunCmd07

		MOV	DS,ES:WorkBuffer
		XOR	SI,SI
RunCmd10:	LODSW
		DEC	SI
		OR	AX,AX
		JNE	RunCmd10
		ADD	SI,BX
		JC	RunCmd11
		MOV	AX,SI
		ADD	AX,ES:DatPoint
		JNC	RunCmd12
RunCmd11:	MOV	BP,OFFSET ManyData_B
		CALL	Dialog
		MOV	ES:DosBuf,0
		MOV	ES:DosCol,0
 		CALL	PutPrompt
		JMP	Func04

RunCmd12:	PUSH	ES			; Restore screen
		POP	DS
		XOR	BX,BX
		MOV	DH,DS:Lines
		MOV	DL,DS:Columns
		CALL	ResUser
		MOV	BH,DS:DosLin
		CALL	ClsLine
		OR	CL,CL
		JNZ	RunCmd13
		CALL	PutPrompt
RunCmd13:	XOR	BX,BX
		CALL	Window
		CALL	SavMous
		MOV	AL,0
		CALL	CurMous
		MOV	AH,DS:DosLin
		CALL	Cursor
		OR	CL,CL
		JNZ	RunCmd14
		MOV	AX,0E0Ah
		INT	10h
RunCmd14:	CALL	RestVct

		PUSH	SI			; Compare free space
		PUSH	ES
		POP	DS
		MOV	DI,SI
		MOV	AL,DS:DataLocation
		CMP	AL,0			; Std mode
		JNE	RunCmd20
		MOV	DI,DS:NormalSize
		SUB	DI,DS:DatPoint
		JMP	RunCmd24
RunCmd20:	CMP	AL,2			; XMS mode
		JNE	RunCmd23
		MOV	DX,DS:DatHandle
		MOV	DS,CS:SubsSeg
		MOV	AH,0Eh
		CALL	DS:CallXMS
		CMP	AX,1
		JNE	RunCmd24
		MOV	CL,10
		MOV	BX,ES:DatPoint
		ADD	BX,SI
		ADD	BX,7Fh
		SHR	BX,CL
		INC	BX
		CMP	BX,DX
		JBE	RunCmd24
		MOV	CX,BX
		SUB	CX,DX
RunCmd21:	PUSH	BX
		MOV	DX,ES:DatHandle
		MOV	AH,0Fh
		CALL	DS:CallXMS
		POP	DX
		CMP	AX,1
		JE	RunCmd22
		DEC	DX
		CMP	BL,0A0h
		MOV	BX,DX
		LOOPE	RunCmd21
RunCmd22:	MOV	CL,10
		SHL	DX,CL
		MOV	DI,DX
		SUB	DI,ES:DatPoint
		SUB	DI,80h
		JMP	RunCmd24
RunCmd23:	JB	RunCmd24		; EMS mode
		MOV	DX,DS:DatHandle
		MOV	AH,4Ch
		CALL	CallEMS
		JNE	RunCmd24
		MOV	CL,14
		MOV	DX,BX
		SHL	DX,CL
		MOV	BX,DS:DatPoint
		ADD	BX,SI
		CMP	BX,DX
		JBE	RunCmd24
		DEC	BX
		SHR	BX,CL
		INC	BX
		MOV	DX,DS:DatHandle
		MOV	AH,51h
		CALL	CallEMS
		SHL	BX,CL
		MOV	DI,BX
		SUB	DI,DS:DatPoint
RunCmd24:	PUSH	ES
		POP	DS
		POP	SI
		MOV	AX,SI
		SUB	AX,DI
		JAE	RunCmd25
		XOR	AX,AX
RunCmd25:	MOV	DS:BytesInSwap,AX

		PUSH	ES			; Prepare data to save
		POP	DS
		MOV	AH,DS:DosChg
		MOV	AL,DS:DataLocation
		PUSH	AX
		PUSH	DS:DatPoint
		PUSH	DS:DatHandle
		PUSH	DS:WorkBuffer
		PUSH	DS:HistPoint
		MOV	BX,DS:WCB1seg
		MOV	DX,DS:WCB2seg
		MOV	AX,SIZE Dat - SIZE MainDat
		ADD	DS:ArchPoint,AX
		ADD	DS:FiltsPoint,AX
		ADD	DS:HistPoint,AX
		MOV	SI,CmnSave-1		; Move CmnSave
		MOV	DI,CmnSave+2*WCBsave-1
		MOV	CX,CmnSave
		STD
		REP	MOVSB
		CLD
		MOV	AL,0			; Move WCB1save
		CMP	BX,DX
		JB	RunCmd30
		XCHG	BX,DX
		MOV	AL,1
RunCmd30:	MOV	DS:ComnIni.SwapWin,AL
		MOV	DS,BX
		XOR	SI,SI
		XOR	DI,DI
		MOV	CX,WCBsave
		PUSH	CX
		REP	MOVSB
		POP	CX
		MOV	DS,DX			; Move WCB2save
		XOR	SI,SI
		REP	MOVSB
		PUSH	ES			; Move *.EXT, History
		POP	DS
		POP	SI
RunCmd31:	LODSW
		DEC	SI
		OR	AX,AX
		JNE	RunCmd31
		MOV	CX,SI
		MOV	SI,OFFSET MainExt
		MOV	DI,CmnSave+2*WCBsave
		SUB	CX,SI
		INC	CX
		REP	MOVSB
		MOV	DS:ComnIni.FilesPoint,DI ; Compress files
		XOR	BP,BP
		MOV	AH,2
RunCmd32:	MOV	DS,BX
		MOV	SI,OFFSET ArcFiles
		MOV	CX,ES:[BP].DiskBuf
		SUB	CX,SI
		JBE	RunCmd33
		REP	MOVSB
RunCmd33:	PUSH	DX
		PUSH	BX
		MOV	SI,ES:[BP].DiskBuf
		MOV	DX,ES:[BP].EndBuf
RunCmd34:	CMP	SI,DX
		JAE	RunCmd35
		MOV	BX,DS:[SI].LenLongNamF
		MOV	AL,DS:[SI].AttrF
		MOV	CL,3
		SHR	AL,CL
		MOV	CX,DS:[SI].LenShortNamF
		ADD	SI,OFFSET NameF
		REP	MOVSB
		DEC	DI
		STOSB
		ADD	SI,BX
		SUB	ES:[BP].EndBuf,BX
		JMP	RunCmd34
RunCmd35:	POP	DX
		POP	BX
		ADD	BP,WCBsave
		DEC	AH
		JNE	RunCmd32

		MOV	ES:ComnIni.CmdLnPoint,DI
		MOV	CX,ES:ComnIni.BytesInSwap ; Save rest of data
		JCXZ	RunCmd37
		MOV	DX,DI
		SUB	DX,CX
		JC	RunCmd36
		CMP	DX,SIZE Dat
		JB	RunCmd36
		CALL	RunCmd70
		JC	RunCmd36
		MOV	AH,40h
		INT	21h
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		INT	21h
		POP	AX
		POP	BX
		JC	RunCmd36
		PUSH	BX
		POPF
		JC	RunCmd36
		CMP	AX,CX
		JE	RunCmd37
		MOV	DX,OFFSET ComnIni.TempDir
		MOV	AH,41h
		INT	21h
RunCmd36:	MOV	ES:ComnIni.BytesInSwap,0 ; Error writing swap file
		MOV	DX,OFFSET WrSwapErr_S
		POP	DS
		JMP	RunCmd46
RunCmd37:	SUB	DI,CX			; Move command line
		MOV	ES:ComnIni.CmdLnPoint,DI
		POP	DS
		XOR	SI,SI
RunCmd38:	LODSW
		DEC	SI
		OR	AX,AX
		JNE	RunCmd38
		MOV	CX,SI
		INC	CX
		POP	DX
		POP	BX
		POP	AX
		PUSH	DI
		XOR	SI,SI
		REP	MOVSB

		PUSH	ES			; Save data
		POP	DS
		MOV	CX,DI
		XOR	SI,SI
		MOV	DI,BX
		PUSH	DS
		PUSH	CX
		PUSH	AX
		PUSH	DI
		PUSH	DX
		CMP	AL,1			; Std mode
		JAE	RunCmd40
		MOV	ES,CS:SubsSeg
		MOV	ES,ES:ResidentPoint.Segm
		REP	MOVSB
		JMP	RunCmd60

RunCmd40:	JA	RunCmd45		; Swap mode
		CALL	RunCmd70
		JC	RunCmd41
		MOV	DX,SI
		MOV	AH,40h
		INT	21h
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		INT	21h
		POP	AX
		POP	DX
		JC	RunCmd41
		PUSH	DX
		POPF
		JC	RunCmd41
		CMP	AX,CX
		JNE	RunCmd41
		PUSH	ES
		MOV	ES,CS:SubsSeg
		MOV	ES,ES:ResidentPoint.Segm
		MOV	SI,OFFSET ComnIni.TempDir
		CALL	strcpy
		POP	ES
		JMP	RunCmd60
RunCmd41:	MOV	DX,OFFSET WrSwapErr_S
		JMP	RunCmd46

RunCmd45:	CMP	AL,2			; XMS mode
		JA	RunCmd50
		PUSH	DS
		MOV	BP,SP
		SUB	SP,SIZE EMOV + 4
		PUSH	ES
		MOV	AX,4310h
		INT	2Fh
		MOV	[BP-4].Offs,BX
		MOV	[BP-4].Segm,ES
		XOR	AX,AX
		PUSH	SS
		POP	DS
		POP	ES
		LEA	SI,[BP-4-SIZE EMOV]
		DEC	CX
		OR	CL,1
		INC	CX
		MOV	DS:[SI].LenBlk.Lo,CX
		MOV	DS:[SI].LenBlk.Hi,AX
		MOV	DS:[SI].SourceHandle,AX
		MOV	DS:[SI].SourceOffset.Offs,AX
		MOV	DS:[SI].SourceOffset.Segm,ES
		MOV	DS:[SI].DestHandle,DX
		MOV	DS:[SI].DestOffset.Lo,DI
		MOV	DS:[SI].DestOffset.Hi,AX
		MOV	AH,0Bh
		CALL	DWORD PTR [BP-4]
		MOV	SP,BP
		POP	DS
		CMP	AX,1
		JE	RunCmd60
		MOV	DX,OFFSET ErrorXMS_S
RunCmd46:	MOV	DS,CS:SubsSeg
		CALL	OutErr
		POP	DX
		POP	SI
		POP	AX
		CBW
		MOV	BP,AX
		MOV	DS,CS:SubsSeg
		MOV	SP,DS:MainStack.Offs
		MOV	ES,DS:DataSegment
		MOV	BX,0FFFFh
		MOV	AH,4Ah
		INT	21h
		CMP	AX,8
		JNE	Init01
		PUSH	ES:ComnIni.FreeMem
		PUSH	SI
		PUSH	BP
		PUSH	DX
		PUSH	BX
		JMP	Init20
RunCmd47:	MOV	DX,OFFSET ErrorEMS_S
		JMP	RunCmd46

RunCmd50:	MOV	ES,CS:SubsSeg		; EMS mode
		MOV	ES,ES:ResidentPoint.Segm
		XOR	BX,BX
		MOV	AX,4400h
		CALL	CallEMS
		JNE	RunCmd47
RunCmd51:	MOV	AX,4401h
		CALL	CallEMS
		JNE	RunCmd47
		PUSH	CX
		MOV	AX,4000h
		SUB	AX,DI
		CMP	AX,CX
		JAE	RunCmd52
		MOV	CX,AX
RunCmd52:	MOV	AX,CX
		ADD	DI,4000h
		REP	MOVSB
		POP	CX
		XOR	DI,DI
		INC	BX
		SUB	CX,AX
		JA	RunCmd51

RunCmd60:	POP	DX
		POP	DX
		POP	DX
		POP	CX
		POP	DS
		POP	SI
		PUSH	DS
		POP	ES
		CMP	DS:ComnIni.ExecTyp,0	; Set parameters
		JNE	RunCmd61
		PUSH	CX
		MOV	DI,OFFSET ComnIni.Switches
		CALL	StrLen
		MOV	BL,CL
		MOV	DH,0
		POP	CX
		JMP	RunCmd62
RunCmd61:	MOV	BL,0
		MOV	AH,19h
		INT	21h
		SUB	DH,AL
		JE	RunCmd62
		ADD	AL,'A'
		MOV	DH,AL
RunCmd62:	MOV	BH,DS:ComnIni.Prompt
		CMP	DL,2
		JAE	RunCmd63
		MOV	DL,DS:ComnIni.LoadFl
RunCmd63:	MOV	AH,49h
		INT	21h
		MOV	AH,DL
		CLC
		JMP	GoResident

RunCmd70	PROC	NEAR			; Open swap file
		USES	CX,DX,SI,DI
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET ComnIni.TempDir
		MOV	DI,DX
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		MOV	SI,OFFSET Swap_F
		CALL	OpenTempFile
		RET
RunCmd70	ENDP

;---------------------------- Exit -----------------------------
ExitVC:		CMP	ES:DataLocation,3
		JB	Exit1
		XOR	BX,BX
		MOV	DX,ES:DatHandle
		MOV	AX,4400h
		CALL	CallEMS
Exit1:		CALL	RstVc1
		PUSH	ES
		POP	DS
		XOR	BX,BX
		MOV	DH,DS:Lines
		MOV	DL,DS:Columns
		CALL	ResUser
		MOV	BH,DS:DosLin
		CALL	ClsLine
		XOR	BX,BX
		CALL	Window
		MOV	AL,0
		CALL	CurMous
		MOV	AH,DS:DosLin
		DEC	AH
		CALL	Cursor
		MOV	AL,0
		JMP	QuitVC

;------------------ Restore and Hook vectots -------------------
RestVct		PROC	NEAR
		LDS	DX,ES:ErrVct
		MOV	AX,2524h
		INT	21h
RstVc1:		LDS	DX,ES:BrkVct
		MOV	AX,251Bh
		INT	21h
		RET
RestVct		ENDP

HookVct		PROC	NEAR
		USES	DS
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
		MOV	DX,OFFSET CriticalError
		MOV	AX,2524h
		INT	21h
		MOV	DX,OFFSET EmptyProc
		MOV	AX,251Bh
		INT	21h
		RET
HookVct		ENDP

;---------------------------------------------------------------
;	Output to StdError
;		DS:DX - string
;---------------------------------------------------------------
OutErr		PROC	NEAR
		USES	BX,CX,DI,ES
		PUSH	DS
		POP	ES
		MOV	BX,2
		PUSH	DI
		MOV	DI,DX
		MOV	CX,0FFFFh
		MOV	AX,4000h
		REPNE	SCASB
		POP	DI
		NOT	CX
		DEC	CX
		INT	21h
		RET
OutErr		ENDP

;---------------------------------------------------------------
;	Call EMS function
;---------------------------------------------------------------
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

		INCLUDE	VCFUNC.INC
		INCLUDE	VCMENU.INC
		INCLUDE	VCGROUP.INC
		INCLUDE	VCMKDIR.INC
		INCLUDE	VCDELETE.INC
		INCLUDE	VCCOPY.INC
		INCLUDE	VCCOPY2.INC
		INCLUDE	VCATTRIB.INC
		INCLUDE	VCLABEL.INC
		INCLUDE	VCFIND.INC
		INCLUDE	VCEXTMNU.INC
		INCLUDE	VCVIEW.INC
		INCLUDE	VCEDIT.INC
		INCLUDE	VCMEM.INC

		INCLUDE	VCERROR.INC
		INCLUDE	VCSCREEN.INC
		INCLUDE	VCKEYB.INC
		INCLUDE	VCMOUSE.INC
		INCLUDE	VCDIALOG.INC
		INCLUDE	VCTREE.INC
		INCLUDE	VCDESCR.INC
		INCLUDE	VCARC.INC
		INCLUDE	VCARC2.INC
		INCLUDE	VCCOMMON.INC
		INCLUDE	VCSUBS.INC
		INCLUDE	VCPANELS.INC

		.CODE
		IFNDEF	OFFICIAL
		DB	3000 DUP(' Main')
		ENDIF

		END	Start
