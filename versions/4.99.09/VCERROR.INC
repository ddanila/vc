;---------------------------------------------------------------
;		Error messages
;
;		Version:	06.08.1998
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Critical error
;---------------------------------------------------------------
		.CONST
Error_S		DB	' Error ',0
ErrorOn_S	DB	'Error on ^2C ^1E',0
Drive_S		DB	'drive',0
Device_S	DB	'device',0
Errors_S	DB	'Attempt to write on write-protected disk.',0
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
		DB	'Error ^0I.',0
FatErr_S	DB	'File allocation table bad.',0
RtAb_S		DB	' Retry   Abort ',0
RtIgAb_S	DB	' Retry   Ignore   Abort ',0
RtSkAb_S	DB	' Retry   Skip   Abort ',0

		.DATA
CritError_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Error_S
		DW	ErrorOn_S
		DW	Empty_S
		DW	Empty_S
		DW	0
CritError_B	BOXHD	< CritError_P, 900h, 0, Error_K, Def_M, None_H, 7>
		BOXMN	< 4, 0,0,0,0, 400h, 0, 0>
		DB	0
RtAb_D		DB	7, 0
		DB	7, 8
		DB	0
RtIgAb_D	DB	7, 0
		DB	8, 8
		DB	7,17
		DB	0
RtSkAb_D	DB	7, 0
		DB	6, 8
		DB	7,15
		DB	0
Error_T		DW	TmpErr, TmpErr+2, 0

		.CODE
CriticalError	PROC	FAR
		PUSH	DS
		PUSH	ES
		PUSH	BP
		PUSH	SI
		PUSH	DI
		PUSH	DX
		PUSH	CX
		PUSH	BX
		CLD
		MOV	DS,CS:SubsSeg
		MOV	ES,DS:DataSegment
		CMP	ES:ErrDat,0
		JNZ	@@Abort1
		CMP	ES:BlocEr,1
		JZ	@@Abort
		MOV	DS,BP
		MOV	BX,SI
		XOR	CX,CX
		PUSH	AX
		TEST	BYTE PTR DS:[BX+5],80h
		JNZ	@@Device
		ADD	AL,'A'
		MOV	CL,AL
		MOV	WORD PTR ES:TmpErr+2,CX
		MOV	BP,OFFSET Drive_S
		TEST	AH,80h
		JZ	@@Message
		PUSH	ES
		MOV	ES,CS:SubsSeg
		MOV	DI,OFFSET FatErr_S
		JMP	@@SetData
  @@Device:	PUSH	DI
		LEA	SI,[BX+10]
		MOV	DI,OFFSET TmpErr+2
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
		POP	DI
		MOV	BP,OFFSET Device_S
  @@Message:	PUSH	ES
		PUSH	BP
		PUSH	DI
		MOV	AH,59h
		INT	21h
		POP	CX
		POP	BP
		MOV	CH,0
		MOV	DX,AX
		SUB	AX,13h
		JB	@@FindMsg
		CMP	AX,15h
		JAE	@@FindMsg
		MOV	CX,AX
  @@FindMsg:	MOV	ES,CS:SubsSeg
		MOV	DI,OFFSET Errors_S
		JCXZ	@@SetData
		CMP	CX,15h
		JB	@@NextMsg
		MOV	CX,15h
  @@NextMsg:	PUSH	CX
		MOV	CH,1
		MOV	AL,0
		REPNE	SCASB
		POP	CX
		LOOP	@@NextMsg
  @@SetData:	POP	DS
		POP	CX
		MOV	ES:Error_T[4],BP
		MOV	WORD PTR DS:TmpErr,DX
		MOV	BX,OFFSET RtIgAb_S
		MOV	DX,OFFSET RtIgAb_D
		MOV	CL,-12
		TEST	CH,20h
		JNZ	@@Dialog
		MOV	BX,OFFSET Ok_S
		MOV	DX,OFFSET OkMn_D
		MOV	CL,-2
		TEST	CH,10h
		JZ	@@Dialog
		MOV	BX,OFFSET RtAb_S
		MOV	DX,OFFSET RtAb_D
		MOV	CL,-8
		CMP	DS:BlocEr,1
		JBE	@@Dialog
		MOV	BX,OFFSET RtSkAb_S
		MOV	DX,OFFSET RtSkAb_D
		MOV	CL,-11
  @@Dialog:	MOV	BP,OFFSET CritError_B
		MOV	SI,OFFSET CritError_P
		MOV	ES:[SI+8],DI
		MOV	ES:[SI+10],BX
		MOV	ES:[BP+SIZE BoxHd].mPoint,DX
		MOV	ES:[BP+SIZE BoxHd].mBase.X,CL
		PUSH	DS
		POP	ES
		XOR	BX,BX
		XOR	SI,SI
		XCHG	SI,DS:TaskStack
		XCHG	BL,DS:Clock
		XCHG	BH,DS:BlDelay
		PUSH	BX
		MOV	BL,DS:HlpPage
		MOV	BH,DS:MinMnuFlag
		MOV	DS:AutoTim.Lo,0FFFFh
		MOV	DX,OFFSET Error_T
		CALL	Dialog
		MOV	DS:TaskStack,SI
		MOV	DS:HlpPage,BL
		MOV	DS:MinMnuFlag,BH
		POP	BX
		MOV	DS:Clock,BL
		MOV	DS:BlDelay,BH
		TEST	CH,30h
		JZ	@@Abort
		MOV	AH,1
		CMP	AL,0
		JE	@@Exit
		TEST	CH,20h
		JZ	@@Skip
		CMP	AL,1
		JNE	@@Abort
		MOV	AH,0
		JMP	@@Exit
  @@Skip:	CMP	DS:BlocEr,1
		JBE	@@Abort
		MOV	DS:ErrDat,2
		CMP	AL,1
		JE	@@Abort1
  @@Abort:	MOV	ES:ErrDat,1
  @@Abort1:	MOV	AH,3
  @@Exit:	MOV	AL,AH
		CBW
		POP	BX
		POP	CX
		POP	DX
		POP	DI
		POP	SI
		POP	BP
		POP	ES
		POP	DS
		IRET
CriticalError	ENDP

;---------------------------------------------------------------
;	Error reading file panel
;		   AL - Drive letter
;	Return:	   AL - Drive letter (0 - Esc, ZR)
;---------------------------------------------------------------
		.CONST
ErrDrv1_S	DB	"Can't read the disk in drive ^0E:",0
ErrDrv2_S	DB	"Press ENTER to try again, ESC to abort,",0
ErrDrv3_S	DB	"or enter a different drive letter here ^r^0E^r:",0

		.DATA
ErrDrv_P	DB	Err_C, ErInv_C, 0, 0
		DW	Error_S
		DW	ErrDrv1_S
		DW	ErrDrv2_S
		DW	ErrDrv3_S
		DW	0
ErrDrv_B	BOXHD	< ErrDrv_P, 500h, ErrDrvSub, Error_K, Def_M, None_H, 7>
		DB	0

		.CODE
ErrorDrive	PROC	NEAR
		USES	BX,DX,BP
		PUSH	AX
		CBW
		MOV	WORD PTR ES:TmpErr,AX
		MOV	DX,OFFSET Error_T
		MOV	BP,OFFSET ErrDrv_B
		CALL	Dialog
		MOV	DL,0
		JNZ	@@Exit
		MOV	DL,ES:TmpErr
		MOV	BL,DL
		SUB	BL,'A'-1
		MOV	AX,440Fh
		INT	21h
  @@Exit:	POP	AX
		MOV	AL,DL
		CMP	AL,0
		RET
ErrorDrive	ENDP

		.CODE	Subs
ErrDrvSub	PROC	NEAR
		CMP	AX,0FFFFh
		JNE	@@Drive
		LEA	AX,[BX+42Ch]
		CALL	Cursor
		JMP	@@Exit
  @@Drive:	CMP	AH,0FCh
		JAE	@@Exit
		CMP	AL,80h
		JAE	@@Exit
		CALL	UpCase
		CMP	AL,'A'
		JB	@@Exit
		CMP	AL,'Z'
		JA	@@Exit
		PUSH	AX
		CALL	TstDrv
		POP	AX
		JC	@@Exit
		MOV	ES:TmpErr,AL
		ADD	BX,42Ch
		MOV	DX,101h
		CALL	BasAdr
		MOV	ES:[DI],AL
		CALL	Window
  @@Exit:	RET
ErrDrvSub	ENDP

;---------------------------------------------------------------
;	Phantom Diskette
;		   AL - Disk ID
;	Return:    CY - can not change
;		   NC - Ok
;---------------------------------------------------------------
		.CONST
Phant0_S	DB	' Change drive ',0
Phant1_S	DB	'Insert diskette for drive ^0E:',0
OkCan_S		DB	' Ok',0FFh,3,'Cancel',0

		.DATA
Phant_P		DB	Err_C, ErInv_C, ErInv_C, 0
		DW	Phant0_S
		DW	Phant1_S
		DW	OkCan_S
		DW	0
Phant_B		BOXHD	< Phant_P, 900h, 0, Error_K, Def_M, None_H, 5 >
		BOXMN	< 4, 0,0,0,0, 3FAh, OkCan_D, 0 >
		DB	0
OkCan_D		DB	4, 0
		DB	8, 5
		DB	0

		.CODE	Subs
Phantom		PROC	FAR
		USES	AX,BX,DX,BP
		CALL	UpCase
		SUB	AL,'A'-1
		MOV	BL,AL
		MOV	AX,440Eh
		INT	21h
		JC	@@Skip
		CMP	AL,0
		JE	@@Skip
		CMP	AL,BL
		JE	@@Skip
		MOV	BH,AL
		MOV	AX,440Fh
		INT	21h
		JC	@@Skip
		CMP	AL,BH
		JE	@@Skip
		XCHG	BL,BH
		MOV	AX,440Fh
		INT	21h
		JC	@@Skip
		XCHG	BL,BH
		MOV	AL,BL
		CBW
		ADD	AL,'A'-1
		MOV	WORD PTR ES:TmpErr,AX
		MOV	BH,ES:HlpPage
		MOV	DX,OFFSET Error_T
		MOV	BP,OFFSET Phant_B
		CALL	Dialog
		MOV	ES:HlpPage,BH
		STC
		JNE	@@Exit
		MOV	AX,440Fh
		INT	21h
  @@Skip:	CLC
  @@Exit:	RET
Phantom		ENDP

;---------------------------------------------------------------
;	Change logical drive map
;		ES:DX - path
;	Return:    CY - can not change
;		   NC - Ok
;---------------------------------------------------------------
ChgDisk		PROC	FAR
		USES	AX,BX
		MOV	BX,DX
		MOV	AX,ES:[BX]
		CMP	AH,':'
		JE	@@Phantom
		MOV	AL,ES:DosPath[0]
  @@Phantom:	CALL	Phantom
		RET
ChgDisk		ENDP
