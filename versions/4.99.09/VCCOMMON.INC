;---------------------------------------------------------------
;		Subroutines
;
;		Version:	10.08.1998
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Timer
;	Return:	CX DX - clock
;---------------------------------------------------------------
		.CODE	Subs
Timer		PROC	FAR	USES	DS
		XOR	CX,CX
		MOV	DS,CX
		CLI
		MOV	DX,DS:[46Ch].Lo
		MOV	CX,DS:[46Ch].Hi
		STI
		RET
Timer		ENDP

;---------------------------------------------------------------
;	Go to CASE
;		CS:SI - address of case table
;		   AX - key code
;---------------------------------------------------------------
Case		PROC	FAR
		PUSH	SI
		PUSH	CX
		OR	AH,AH
		JE	@@Exit
  @@Next:	MOV	CX,CS:[SI]
		ADD	SI,4
		JCXZ	@@Exit
		CMP	AL,CL
		JNE	@@Next
		OR	CH,CH
		JE	@@GoCase
		CMP	AH,CH
		JNE	@@Next
  @@GoCase:	POP	CX
		ADD	SP,4
		PUSH	CS:[SI-2]
		RET
  @@Exit:	POP	CX
		POP	SI
		RET
Case		ENDP

;---------------------------------------------------------------
;	Test current dir '.' and up dir '..'
;		DS:SI - name
;	Return:	   ZR - yes
;		   NZ - no
;---------------------------------------------------------------
ChkUpDir	PROC	FAR
		CMP	BYTE PTR DS:[SI],'.'
		JNE	@@Exit
		CMP	BYTE PTR DS:[SI+1],0
		JE	@@Exit
		CMP	WORD PTR DS:[SI+1],'.'
  @@Exit:	RET
ChkUpDir	ENDP

;---------------------------------------------------------------
;	Add backslash to ES:DI if there is not
;---------------------------------------------------------------
AddPathSep	PROC	FAR
		MOV	AL,ES:[DI-1]
		CMP	AL,'\'
		JE	@@Exit
		CMP	AL,'/'
		JE	@@Exit
		CMP	AL,':'
		JE	@@Exit
		MOV	AL,'\'
		STOSB
  @@Exit:	RET
AddPathSep		ENDP

;---------------------------------------------------------------
;	Check short name
;		DS:SI - name
;		   AL - check case: 0-lower, 1-upper
;	Return:    CY - not short
;		   NC - short
;---------------------------------------------------------------
ChkShortNam	PROC	FAR	USES	AX BX CX SI DI
		MOV	DI,OFFSET FileSymb2_S
		CALL	ChkForSymb
		JC	@@Exit
		MOV	BL,AL
		MOV	CX,8+1
		CALL	ChkShort1
		JC	@@Exit
		OR	AL,AL
		JE	@@Exit
		MOV	CX,3+1
		CALL	ChkShort1
		JC	@@Exit
		CMP	AL,1
		CMC
  @@Exit:	RET
ChkShortNam	ENDP

ChkShort1	PROC	NEAR
		CMP	BYTE PTR DS:[SI],'.'
		JE	@@Error
  @@GetChar:	LODSB
		CMP	AL,0
		JE	@@Exit
		CMP	AL,'.'
		JE	@@Exit
		MOV	AH,AL
		OR	BL,BL
		JNZ	@@UpCase
		CALL	LowCase
		JMP	@@ChkCase
  @@UpCase:	CALL	UpCase
  @@ChkCase:	CMP	AL,AH
		LOOPE	@@GetChar
  @@Error:	STC
  @@Exit:	RET
ChkShort1	ENDP

;---------------------------------------------------------------
;	Copy ASCIIZ string from DS:SI to ES:DI
;---------------------------------------------------------------
strcpy		PROC	FAR
		LODSB
		STOSB
		CMP	AL,0
		JNE	strcpy
		DEC	DI
		RET
strcpy		ENDP

;---------------------------------------------------------------
;	Check name for special symbols
;		DS:SI - name
;		ES:DI - table of symbols
;	Return:	   CY - found
;---------------------------------------------------------------
		.CONST
FileSymb1_S	DB	'*?/\<>|:"',0
FileSymb2_S	DB	'[]=+,; ',0

		.CODE	Subs
ChkForSymb	PROC	FAR	USES	AX CX SI ES
		PUSH	CS
		POP	ES
		CALL	StrLen
		DEC	CX
  @@Char:	LODSB
		CMP	AL,0
		JE	@@Exit
		CMP	AL,' '
		JB	@@Exit
		PUSH	DI
		PUSH	CX
		REPNE	SCASB
		POP	CX
		POP	DI
		JNE	@@Char
		STC
  @@Exit:	RET
ChkForSymb	ENDP

;---------------------------------------------------------------
;	Move history lines from DS:SI to ES:DI (ASCIIZ,...,ASCIIZ,0)
;---------------------------------------------------------------
MvzHist		PROC	FAR
		LODSW
		STOSW
		DEC	SI
		DEC	DI
		OR	AX,AX
		JNE	MvzHist
		RET
MvzHist		ENDP

;---------------------------------------------------------------
;	Clear rest of line DS:SI, length CX
;---------------------------------------------------------------
FilZero		PROC	FAR	USES	DI ES
		PUSH	DS
		POP	ES
		MOV	DI,SI
		MOV	AL,0
		REPNE	SCASB
		JCXZ	@@Exit
		REP	STOSB
  @@Exit:	RET
FilZero		ENDP

;---------------------------------------------------------------
;	Clear rest of history DS:SI, length CX
;---------------------------------------------------------------
FilHist		PROC	FAR	USES	SI DI ES
		PUSH	DS
		POP	ES
		MOV	DI,SI
  @@FindEnd:	LODSW
		DEC	SI
		OR	AX,AX
		JNE	@@FindEnd
		PUSH	SI
		SUB	SI,DI
		SUB	CX,SI
		POP	DI
		JBE	@@Exit
		REP	STOSB
  @@Exit:	RET
FilHist		ENDP

;---------------------------------------------------------------
;	String length
;		ES:DI - string
;	Return:	   CX - length
;---------------------------------------------------------------
StrLen		PROC	FAR	USES	DI
		MOV	CX,0FFFFh
		MOV	AL,0
		REPNE	SCASB
		NOT	CX
		RET
StrLen		ENDP

;---------------------------------------------------------------
;	Compare two substrings
;		DS:SI - line 1
;		ES:DI - line 2
;		   CX - counter
;	Return:	   ZR - equal
;		   NZ - not equal
;---------------------------------------------------------------
CompNormSubStr	PROC	FAR	USES	AX
  @@Next:	LODSB
		CALL	UpCase
		MOV	AH,AL
		MOV	AL,ES:[DI]
		INC	DI
		CALL	UpCase
		CMP	AL,AH
		LOOPE	@@Next
		RET
CompNormSubStr	ENDP

;---------------------------------------------------------------
;	Compare two lines
;		DS:SI - line 1
;		ES:DI - line 2
;	Return:	   ZR - equal
;		   NZ - not equal
;---------------------------------------------------------------
CompNormLines	PROC	FAR	USES	AX SI DI
  @@Next:	LODSB
		CALL	UpCase
		MOV	AH,AL
		MOV	AL,ES:[DI]
		INC	DI
		CALL	UpCase
		CMP	AL,AH
		JNE	@@Exit
		CMP	AL,0
		JNE	@@Next
  @@Exit:	RET
CompNormLines	ENDP

;---------------------------------------------------------------
;	Uppercase letter
;		   AL - letter
;	Return:	   AL - letter
;---------------------------------------------------------------
UpCase		PROC	FAR
		CMP	AL,'a'
		JB	@@Exit
		CMP	AL,'z'
		JA	@@CaseMap
		SUB	AL,20h
		RET
  @@CaseMap:	CMP	AL,80h
		JB	@@Exit
		PUSH	DS
		MOV	DS,CS:DataSegment
		CALL	DS:Country.CaseMap
		POP	DS
  @@Exit:	RET
UpCase		ENDP

;---------------------------------------------------------------
;	Lowercase letter
;		   AL - letter
;	Return:	   AL - letter
;---------------------------------------------------------------
LowCase		PROC	FAR
		CMP	AL,'A'
		JB	@@Exit
		CMP	AL,'Z'
		JA	@@Decode
		ADD	AL,20h
		RET
  @@Decode:	CMP	AL,80h
		JB	@@Exit
		PUSH	DS
		PUSH	BX
		MOV	BL,AL
		MOV	BH,0
		MOV	DS,CS:DataSegment
		MOV	AL,DS:LowTab[BX-128]
		POP	BX
		POP	DS
  @@Exit:	RET
LowCase		ENDP

;---------------------------------------------------------------
;	Skip leading spaces
;		DS:SI - string
;---------------------------------------------------------------
;
; for (i = 0; str[i] == ' ' || str[i] == '\t'; i++) ;
; return &str[i]
;
SkipSpace	PROC	FAR
		LODSB
		CMP	AL,' '
		JE	SkipSpace
		CMP	AL,9
		JE	SkipSpace
		DEC	SI
		RET
SkipSpace	ENDP

;---------------------------------------------------------------
;	Check long name
;		DS:SI - name
;	Return:  CY=1 - long name
;		   ZF - last char = ZERO, else '\'
;---------------------------------------------------------------
ChkLongName	PROC	NEAR	USES	AX CX
		MOV	CX,9
  @@CheckName:	LODSB
		CMP	AL,0
		JE	@@ExitOk
		CMP	AL,'\'
		JE	@@ExitOk
		CMP	AL,'.'
		JE	@@NameOk
		LOOP	@@CheckName
		JMP	@@Error
  @@NameOk:	MOV	CX,4
  @@CheckExt:	LODSB
		CMP	AL,0
		JE	@@ExitOk
		CMP	AL,'\'
		JE	@@ExitOk
		CMP	AL,'.'
		LOOPNE	@@CheckExt
  @@Error:	STC
		JMP	@@Exit
  @@ExitOk:	CMP	AL,0
  @@Exit:	RET
ChkLongName	ENDP

;---------------------------------------------------------------
;	Return the last path element
;		DS:SI - pathname
;	Return:	DS:SI - end of path
;---------------------------------------------------------------
;
; for (i = p = 0; path[i] != '\0'; i++)
;      if (path[i] == ':' || path[i] == '\\' || path[i] == '/')
;          p = i + 1
; return &path[p]
;
CutPath		PROC	FAR	USES	AX BX
  @@Point:	MOV	BX,SI
  @@Next:	LODSB
		CMP	AL,':'
		JE	@@Point
		CMP	AL,'\'
		JE	@@Point
		CMP	AL,'/'
		JE	@@Point
		CMP	AL,0
		JNE	@@Next
		MOV	SI,BX
		RET
CutPath		ENDP

;---------------------------------------------------------------
;	Find extension of the file
;		DS:SI - file
;	Return:	DS:SI - extension
;---------------------------------------------------------------
FindFileExt	PROC	FAR	USES	AX BX DI
		CMP	BYTE PTR DS:[SI],0
		JE	@@Exit
		INC	SI
		MOV	BX,SI
  @@SetPoint:	MOV	DI,SI
  @@GetChar:	LODSB
		CMP	AL,'.'
		JE	@@SetPoint
		CMP	AL,0
		JNE	@@GetChar
		DEC	SI
		CMP	DI,BX
		JBE	@@Exit
		MOV	SI,DI
  @@Exit:	RET
FindFileExt	ENDP

;---------------------------------------------------------------
;	True name
;		DS:SI - name
;		ES:DI - buffer for true name
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
TrueName	PROC	FAR	USES	CX
		MOV	CX,2
		MOV	AX,7160h
		CALL	Intr21
		JNC	@@Exit
		CMP	AX,7100h
		JNE	@@Error
		XOR	CX,CX
		MOV	AH,60h
		CALL	Intr21
		JNC	@@Exit
  @@Error:	STC
		MOV	CX,AX
  @@Exit:	MOV	AX,CX
		RET
TrueName	ENDP

;---------------------------------------------------------------
;	Short name
;		DS:SI - name
;		ES:DI - buffer for short name
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
ShortName	PROC	FAR	USES	CX DX SI DI DS
		PUSH	ES
		MOV	ES,CS:DataSegment
		MOV	AX,DS:[SI]
		CMP	AH,':'
		JE	@@Phantom
		MOV	AL,ES:DosPath[0]

  @@Phantom:
		CALL	Phantom
		MOV	AX,ErrFail
		POP	ES
		JC	@@Error
		MOV	CX,8001h
		MOV	AX,7160h
		CALL	Intr21
		JC	@@Error
		PUSH	DS
		MOV	DS,CS:DataSegment
		CMP	DS:ConvCase,0
		POP	DS
		JE	@@Exit
		MOV	DX,SI
		MOV	AX,4300h
		CALL	Intr21			; stat()
		JC	@@Error
		TEST	CL,fa_LABEL OR fa_DIR
		JNZ	@@Exit
		PUSH	ES
		POP	DS
		MOV	SI,DI
		CALL	UpPathLowFname
		CLC
		JMP	@@Exit

  @@Error:
		PUSH	AX
		CALL	strcpy
		POP	AX
		STC

  @@Exit:
		RET
ShortName	ENDP

;---------------------------------------------------------------
;	Canonize name
;		DS:SI - name
;		ES:DI - buffer
;		   CX - length of name, 0 - short names
;	Return:	DS:SI - end of name
;		ES:DI - end of converted name
;---------------------------------------------------------------
NormName	PROC	FAR	USES	AX BX CX
		CMP	CX,0
		JNZ	@@LongName1
		PUSH	DI
		MOV	DI,OFFSET TmpErr
		CALL	Convert
		MOV	AX,SI
		MOV	SI,DI
		POP	DI
		PUSH	DS
		PUSH	ES
		POP	DS
		CALL	UnConv
		POP	DS
		MOV	SI,AX
		JMP	@@Exit
  @@LongName1:	MOV	BX,DI			; Long name
  @@LongName2:	LODSB
		CMP	AL,'\'
		JE	@@End
		CMP	AL,'/'
		JE	@@End
		CMP	AL,0
		JE	@@End
		JCXZ	@@LongName2
		DEC	CX
		STOSB
		CMP	AL,' '
		JE	@@LongName2
		JMP	@@LongName1
  @@End:	DEC	SI
		MOV	DI,BX
		MOV	BYTE PTR ES:[DI],0
  @@Exit:	RET
NormName	ENDP

;---------------------------------------------------------------
;	Full pathname
;		DS:SI - pathname
;		ES:DI - buffer
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
NormPath	PROC	FAR	USES	BX CX DX SI DI
		LEA	BX,[DI+2]
		CALL	SkipSpace
		LODSW
		CMP	AX,'\\'
		JNE	@@GetDisk
		STOSW
		DEC	BX
		XOR	CX,CX
		JMP	@@NormDir
  @@GetDisk:	CMP	AH,':'
		JE	@@SaveDisk
		DEC	SI
		DEC	SI
		MOV	AX,WORD PTR ES:DosPath
  @@SaveDisk:	CALL	UpCase
		STOSW
		PUSH	AX
		CALL	GetFS
		MOV	CX,AX
		MOV	AL,'\'
		STOSB
		POP	AX
		CMP	BYTE PTR DS:[SI],'\'
		JE	@@NormDir
		CMP	BYTE PTR DS:[SI],'/'
		JE	@@NormDir
		CMP	AL,ES:DosPath[0]
		JNE	@@CurDir
		PUSH	DS
		PUSH	SI
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET DosPath[3]
		CALL	strcpy
		POP	SI
		POP	DS
		JMP	@@AddSlash
  @@CurDir:	MOV	DL,AL
		CALL	Phantom
		MOV	AX,ErrFail
		JC	@@Exit
		PUSH	DS
		PUSH	SI
		PUSH	ES
		POP	DS
		MOV	SI,DI
		SUB	DL,'A'-1
		MOV	AH,47h
		CALL	Intr21
		POP	SI
		POP	DS
		JC	@@Exit
		PUSH	DS
		PUSH	SI
		PUSH	ES
		POP	DS
		LEA	SI,[DI-3]
		MOV	AL,1
		CALL	RealPath
		POP	SI
		POP	DS
		JC	@@Exit
		PUSH	CX
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		DEC	DI
		POP	CX
  @@AddSlash:	CALL	AddPathSep
  @@NormDir:	LODSB
		CMP	AL,0
		JE	@@Exit
		CMP	AL,'\'
		JE	@@AddSlash
		CMP	AL,'/'
		JE	@@AddSlash
		DEC	SI
		PUSH	SI
  @@ChkPoints:	LODSB
		CMP	AL,'.'
		JE	@@ChkPoints
		CMP	AL,0
		JE	@@Points
		CMP	AL,'\'
		JE	@@Points
		CMP	AL,'/'
		JE	@@Points
		POP	SI
		CALL	NormName
		JMP	@@NormDir
  @@Points:	POP	AX
		DEC	SI
		PUSH	CX
		MOV	CX,SI
		SUB	CX,AX
		POP	AX
		PUSH	AX
		OR	AX,AX
		MOV	AL,'\'
		JNE	@@CutEntry
		CMP	CX,2
		JA	@@ExitPnt
  @@CutEntry:	PUSH	CX
		MOV	CH,0FFh
		DEC	DI
		MOV	AL,'\'
		STD
		REPNE	SCASB
		CLD
		INC	DI
		POP	CX
		CMP	DI,BX
		JBE	@@Limit
		LOOP	@@CutEntry
		JMP	@@ExitPnt
  @@Limit:	MOV	DI,BX
		INC	DI
		DEC	CX
  @@ExitPnt:	OR	CX,CX
		POP	CX
		JE	@@NormDir
		MOV	AX,3
		STC
  @@Exit:	MOV	BYTE PTR ES:[DI],0
		RET
NormPath	ENDP

;---------------------------------------------------------------
;	Cut spaces
;		ES:SI - address of string
;---------------------------------------------------------------
NormStr		PROC	FAR	USES	AX BX SI DI
		MOV	DI,SI
  @@FndChar:	LODS	BYTE PTR ES:[SI]
		CMP	AL,' '
		JE	@@FndChar
		DEC	SI
  @@SaveEnd:	MOV	BX,DI
  @@Next:	LODS	BYTE PTR ES:[SI]
		STOSB
		CMP	AL,' '
		JE	@@Next
		CMP	AL,0
		JNE	@@SaveEnd
		MOV	ES:[BX],AL
		RET
NormStr		ENDP

;---------------------------------------------------------------
;	Uppercase whole string
;		ES:SI - address of string
;---------------------------------------------------------------
;
; for (i = 0; str[i] != '\0'; i++)
;      str[i] = UpCase(str[i])
;
UpCaseStr	PROC	FAR	USES	AX DI
		MOV	DI,SI

  @@Next:
		MOV	AL,ES:[DI]
		CALL	UpCase
		STOSB
		CMP	AL,0
		JNE	@@Next
		RET
UpCaseStr	ENDP

;---------------------------------------------------------------
;	Uppercase path & lowercase filename
;		ES:SI - address of string
;---------------------------------------------------------------
;
; UpCaseStr(&path)
; fname = CutPath(path)
; for (i = 0; fname[i] != '\0'; i++)
;      fname[i] = LowCase(fname[i])
;
UpPathLowFname	PROC	FAR	USES	AX SI DI DS
		CALL	UpCaseStr
		PUSH	ES
		POP	DS
		CALL	CutPath
		MOV	DI,SI

  @@Next:
		LODSB
		CALL	LowCase
		STOSB
		CMP	AL,0
		JNE	@@Next
		RET
UpPathLowFname	ENDP

;---------------------------------------------------------------
;	Shorter string
;		ES:SI - address of string
;		   AX - limit
;---------------------------------------------------------------
ShrtLin		PROC	FAR	USES	AX BX CX SI DI DS
		PUSH	AX
		MOV	DI,SI
		CALL	StrLen
		DEC	CX
		MOV	DI,CX
		POP	CX
		SUB	DI,CX
		JBE	@@Exit
		PUSH	ES
		POP	DS
		CMP	BYTE PTR DS:[SI+1],':'
		JNE	@@ChkPath
		INC	SI
		INC	SI
		DEC	CX
		DEC	CX
		JMP	@@ChkSlash
  @@ChkPath:	PUSH	SI
		MOV	AH,0
  @@FindColon:	LODSB
		CMP	AL,0
		JE	@@ExitColon1
		CMP	AL,'\'
		JE	@@ExitColon
		CMP	AL,':'
		JNE	@@FindColon
		MOV	AH,1
		JMP	@@FindColon
  @@ExitColon:	OR	AH,AH
		MOV	BX,SI
  @@ExitColon1:	POP	SI
		JE	@@ChkSlash
		MOV	AX,SI
		SUB	AX,BX
		ADD	AX,CX
		JNC	@@Short
		CMP	AX,WORD PTR 3
		JB	@@Short
		MOV	CX,AX
		MOV	SI,BX
  @@ChkSlash:	CMP	BYTE PTR DS:[SI],'\'
		JNE	@@Short
		INC	SI
		DEC	CX
  @@Short:	XCHG	SI,DI
		MOV	AL,'.'
		STOSB
		STOSB
		STOSB
		DEC	CX
		DEC	CX
		ADD	SI,DI
		REP	MOVSB
  @@Exit:	RET
ShrtLin		ENDP

;---------------------------------------------------------------
;	Main file
;		VcSet + CS:SI -> ES:DI
;---------------------------------------------------------------
MainFil		PROC	FAR	USES	DS
		PUSH	SI
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET VcSet
		CALL	strcpy
		POP	SI
		PUSH	CS
		POP	DS
		CALL	strcpy
		RET
MainFil		ENDP

;---------------------------------------------------------------
;	Output number into buffer
;			ES:DI - address of buffer
;			BX CX - number
;			   AL - mode: b0-coma, b1-spaces
;---------------------------------------------------------------
Number		PROC	FAR	USES	AX BX CX DX SI BP DS
		MOV	DS,CS:DataSegment
		MOV	DH,AL
		CMP	DS:Country.Sep1000,0
		JNE	@@Calculate
		TEST	AL,1
		JZ	@@Calculate
		AND	DH,0FEh
		JZ	@@Calculate
		MOV	AL,' '
		STOSB
		STOSB
		STOSB
  @@Calculate:	MOV	DL,0
		MOV	BP,0CA00h	; 1.000.000.000
		MOV	SI,3B9Ah
		CALL	Numb1
		CALL	Numb2
		MOV	BP,0E100h	;   100.000.000
		MOV	SI,5F5h
		CALL	Numb1
		MOV	BP,9680h	;    10.000.000
		MOV	SI,98h
		CALL	Numb1
		MOV	BP,4240h	;     1.000.000
		MOV	SI,0Fh
		CALL	Numb1
		CALL	Numb2
		MOV	BP,86A0h	;       100.000
		MOV	SI,1
		CALL	Numb1
		MOV	BP,10000	;        10.000
		XOR	SI,SI
		CALL	Numb1
		MOV	BP,1000		;         1.000
		CALL	Numb1
		CALL	Numb2
		MOV	BP,100		;           100
		CALL	Numb1
		MOV	BP,10		;            10
		CALL	Numb1
		MOV	AL,'0'		;             1
		ADD	AL,CL
		MOV	AH,0
		STOSW
		DEC	DI
		RET
Number		ENDP

Numb1		PROC	NEAR
		MOV	AL,'0'-1
  @@Divide:	INC	AX
		SUB	CX,BP
		SBB	BX,SI
		JNC	@@Divide
		ADD	CX,BP
		ADC	BX,SI
		CMP	AL,'0'
		JE	ChkSpace
		MOV	DL,1
		STOSB
		RET
Numb1		ENDP

Numb2		PROC	NEAR
		TEST	DH,1
		JZ	@@Exit
		MOV	AL,DS:Country.Sep1000
IFDEF	__JWASM__
ChkSpace::
ELSE
ChkSpace:
ENDIF
		OR	DL,DL
		JNZ	@@Write
		TEST	DH,2
		JZ	@@Exit
		MOV	AL,' '
  @@Write:	STOSB
  @@Exit:	RET
Numb2		ENDP

;---------------------------------------------------------------
;	Convert QWORD to DWORD
;		ES:DI - QWORD
;	Return:	   AX - letter
;---------------------------------------------------------------
		.CONST
Letters_S	DB	0,'KMGT'

		.CODE	Subs
ReduceNumb	PROC	FAR	USES	CX SI DS
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET Letters_S
  @@Reduce:	MOV	AX,DS:[DI+4]
		OR	AX,DS:[DI+6]
		JE	@@Letter
		INC	SI
		MOV	CX,10
		MOV	AX,DS:[DI+0]
  @@Rotate:	SHR	WORD PTR DS:[DI+6],1
		RCR	WORD PTR DS:[DI+4],1
		RCR	WORD PTR DS:[DI+2],1
		RCR	WORD PTR DS:[DI+0],1
		LOOP	@@Rotate
		TEST	AX,3FFh
		JE	@@Reduce
		ADD	WORD PTR DS:[DI+0],1
		ADC	WORD PTR DS:[DI+2],CX
		ADC	WORD PTR DS:[DI+4],CX
		ADC	WORD PTR DS:[DI+6],CX
		JMP	@@Reduce
  @@Letter:	MOV	AL,CS:[SI]
		MOV	AH,0
		RET
ReduceNumb	ENDP

;---------------------------------------------------------------
;	Output hex byte
;		   AL - code
;	Return:	AL,AH - high and low digits
;---------------------------------------------------------------
HexByt		PROC	FAR	USES	CX
		PUSH	AX
		CALL	HexCod
		MOV	CH,AL
		POP	AX
		MOV	CL,4
		SHR	AL,CL
		CALL	HexCod
		MOV	AH,CH
		RET
HexByt		ENDP

HexCod		PROC	NEAR
		AND	AL,0Fh
		ADD	AL,'0'
		CMP	AL,'9'
		JBE	@@Exit
		ADD	AL,7
  @@Exit:	RET
HexCod		ENDP

;---------------------------------------------------------------
;	Text hex number
;		   AL - ASCII byte
;	Return:	   AL - hex number, CY=0
;			     error, CY=1
;---------------------------------------------------------------
TstHex		PROC	FAR
		CMP	AL,'0'
		JB	@@Error
		CMP	AL,'9'
		JA	@@ChkLetter
		SUB	AL,'0'
		RET
  @@ChkLetter:	CALL	UpCase
		CMP	AL,'A'
		JB	@@Error
		CMP	AL,'F'
		JA	@@Error
		SUB	AL,'A'-10
		RET
  @@Error:	STC
		RET
TstHex		ENDP

;---------------------------------------------------------------
;	Number input
;		DS:SI - string
;	Return:	   AX - number
;		 CY=1 - error
;---------------------------------------------------------------
TxtNum		PROC	FAR	USES	BX DX
		PUSH	SI
		XOR	BX,BX
  @@NextChar:	LODSB
		SUB	AL,'0'
		JB	@@Exit
		CMP	AL,9
		JA	@@Exit
		PUSH	AX
		MOV	AX,10
		MUL	BX
		MOV	BX,AX
		POP	AX
		OR	DX,DX
		JNE	@@Overflow
		CBW
		ADD	BX,AX
		JNC	@@NextChar
  @@Overflow:	MOV	BX,0FFFFh
		JMP	@@NextChar
  @@Exit:	DEC	SI
		POP	AX
		CMP	AX,SI
		MOV	AX,BX
		CMC
		RET
TxtNum		ENDP

;---------------------------------------------------------------
;	Convert name & extension
;		DS:SI - address of file
;		ES:DI - convert buffer
;	Return:	DS:SI - end of file
;---------------------------------------------------------------
Convert		PROC	FAR	USES	AX CX DI
		MOV	CX,8
		MOV	AH,'.'
		CALL	Conv1
		MOV	CL,3
		MOV	AH,0
		CALL	Conv1
		RET
Convert		ENDP

Conv1		PROC	NEAR
  @@Next:	LODSB
		CMP	AL,0
		JE	@@FillSpace
		CMP	AL,'\'
		JE	@@FillSpace
		CMP	AL,'/'
		JE	@@FillSpace
		CMP	AL,AH
		JE	@@FillSpace
		CMP	AL,'*'
		JE	@@FillAll
		CALL	UpCase
		STOSB
		LOOP	@@Next
		JMP	@@FindEnd
  @@FillAll:	MOV	AL,'?'
		JMP	@@Fill
  @@FillSpace:	MOV	AL,' '
  @@Fill:	REP	STOSB
		DEC	SI
  @@FindEnd:	LODSB
		CMP	AL,0
		JE	@@Exit1
		CMP	AL,AH
		JE	@@Exit
		CMP	AL,'\'
		JE	@@Exit1
		CMP	AL,'/'
		JNE	@@FindEnd
  @@Exit1:	DEC	SI
  @@Exit:	RET
Conv1		ENDP

;---------------------------------------------------------------
;	Unconvert name & extension
;		DS:SI - convert buffer
;		ES:DI - address of file
;---------------------------------------------------------------
UnConv		PROC	FAR	USES	AX BX CX
		MOV	BX,DI
		MOV	CX,8
		CALL	UnCnv1
		MOV	AL,'.'
		STOSB
		MOV	CL,3
		CALL	UnCnv1
		MOV	BYTE PTR ES:[DI],0
		RET
UnConv		ENDP

UnCnv1		PROC	NEAR
		LODSB
		STOSB
		CMP	AL,' '
		JE	@@Next
		MOV	BX,DI
  @@Next:	LOOP	UnCnv1
		MOV	DI,BX
		RET
UnCnv1		ENDP

;---------------------------------------------------------------
;	Test mask file
;		DS:SI - address of file
;		ES:DI - address of masks
;	Return:	   ZR - yes
;		   NZ - no
;---------------------------------------------------------------
TestMask	PROC	FAR	USES	AX BX DX SI DI DS ES
		PUSH	SI
  @@FindPoint:	LODSB				; Add . to file without ext
		CMP	AL,'.'
		JE	@@TestMask
		CMP	AL,0
		JNE	@@FindPoint
		POP	SI
		PUSH	DI
		MOV	DI,OFFSET TmpErr
		PUSH	DI
		CALL	strcpy
		MOV	AX,'.'
		STOSW
		PUSH	ES
		POP	DS
		POP	SI
		POP	DI
		PUSH	SI
  @@TestMask:	PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		POP	SI
		MOV	DL,1
		XCHG	SI,DI
  @@ParseMask:	CALL	SkipSpace		; Parse one mask
		MOV	DH,0
		CMP	AL,'|'
		JNE	@@CutEndSpc0
		INC	SI
		CALL	SkipSpace
		MOV	DH,1
  @@CutEndSpc0:	PUSH	SI
  @@CutEndSpc1:	MOV	BX,SI
  @@CutEndSpc2: LODSB
		CMP	AL,' '
		JE	@@CutEndSpc2
		CMP	AL,9
		JE	@@CutEndSpc2
		CMP	AL,';'
		JE	@@CutEndSpc3
		CMP	AL,','
		JE	@@CutEndSpc3
		CMP	AL,0
		JNE	@@CutEndSpc1
  @@CutEndSpc3:	POP	SI
		MOV	AX,BX
		SUB	AX,SI
		JBE	@@SkipEndSpc
		OR	DH,DH			; Compare
		JE	@@ChkInclude
		CALL	CompMask
		JNE	@@SkipEndSpc
  @@NotFound:	OR	SI,SI
		JMP	@@Exit
  @@ChkInclude:	AND	DL,0FEh
		TEST	DL,2
		JNE	@@SkipEndSpc
		CALL	CompMask
		JNE	@@SkipEndSpc
		OR	DL,2
  @@SkipEndSpc:	MOV	SI,BX			; Skip end spaces
		CALL	SkipSpace
		INC	SI
		CMP	AL,';'
		JE	@@ParseMask
		CMP	AL,','
		JE	@@ParseMask
		OR	DL,DL
		JE	@@NotFound
		XOR	DL,DL
  @@Exit:	RET
TestMask	ENDP

CompMask	PROC	NEAR	USES	SI DI
  @@CompChar:	CMP	SI,BX
		JAE	@@EndComp
		LODSB
		CMP	AL,'?'
		JE	@@SkipChar
		CMP	AL,'*'
		JE	@@SkipChars
		CALL	UpCase
		MOV	AH,AL
		MOV	AL,ES:[DI]
		INC	DI
		CALL	UpCase
		CMP	AL,AH
		JE	@@CompChar
		JMP	@@NoEqu
  @@SkipChar:	MOV	AL,ES:[DI]
		CMP	AL,0
		JE	@@CompChar
		CMP	AL,'.'
		JE	@@CompChar
		INC	DI
		JMP	@@CompChar
  @@SkipChars:	CMP	SI,BX
		JAE	@@CompMask
		LODSB
		CMP	AL,'*'
		JE	@@SkipChars
		DEC	SI
  @@CompMask:	CALL	CompMask
		JE	@@Exit
		CMP	BYTE PTR ES:[DI],0
		JE	@@NoEqu
		INC	DI
		JMP	@@CompMask
  @@EndComp:	CMP	BYTE PTR ES:[DI],0
		JE	@@Exit
		CMP	WORD PTR ES:[DI],'.'
		JE	@@Exit
  @@NoEqu:	OR	SI,SI
  @@Exit:	RET
CompMask	ENDP

;---------------------------------------------------------------
;	Test drive letter
;		   AL - drive letter
;	Return:	   AL - type, NC
;		   CY - no drive
;---------------------------------------------------------------
None		EQU	0
Floppy		EQU	1
Logical		EQU	2
Fixed		EQU	3
Substed		EQU	4
Network		EQU	5
CDROM		EQU	6

TstDrv		PROC	FAR	USES	BX CX DX
		SUB	AL,'A'
		MOV	CL,AL			; Check drive
		MOV	AH,19h
		INT	21h
		PUSH	AX
		MOV	DL,CL
		MOV	AH,0Eh
		INT	21h
		MOV	AH,19h
		INT	21h
		MOV	CH,AL
		POP	DX
		MOV	AH,0Eh
		INT	21h
		MOV	AL,None
		SUB	CH,CL
		JNE	@@Exit
		XOR	BX,BX			; Check CD-ROM
		MOV	AX,150Bh
		INT	2Fh
		CMP	BX,0ADADh
		JNE	@@Floppy
		OR	AX,AX
		MOV	AL,CDROM
		JNZ	@@Exit
  @@Floppy:	MOV	BL,CL			; Check floppy
		INC	BX
		MOV	AX,4408h
		INT	21h
		CMP	AL,0
		JNE	@@GetFlags
		MOV	AX,440Eh
		INT	21h
		MOV	AH,AL
		MOV	AL,Floppy
		JC	@@Exit
		OR	AH,AH
		JE	@@Exit
		CMP	AH,BL
		JE	@@Exit
		MOV	AL,Logical
		JMP	@@Exit
  @@GetFlags:	XOR	DX,DX			; Analyze flags
		MOV	AX,4409h
		INT	21h
		MOV	AL,Network
		TEST	DX,1000h
		JNZ	@@Exit
		MOV	AL,Substed
		TEST	DX,8000h
		JNZ	@@Exit
		MOV	AL,Fixed
  @@Exit:	CMP	AL,1
		RET
TstDrv		ENDP

;---------------------------------------------------------------
;	Get File System attributes
;		   AL - drive letter
;	Return:	   NC - Ok, AX - length of names, 0 - short names
;		   CY - error, AX - error code
;---------------------------------------------------------------
GetFS		PROC	FAR	USES	BX CX DX SI DI DS
		PUSH	ES
		POP	DS
		MOV	DX,OFFSET TmpErr
		MOV	DI,DX
		MOV	AH,':'
		STOSW
		MOV	AX,'\'
		STOSW
		XOR	CX,CX
		XOR	BX,BX
		MOV	AX,71A0h
		INT	21h
		JNC	@@ChkLen
		CMP	AX,WORD PTR ErrFail
		JNE	@@Zero
		STC
		JMP	@@Exit
  @@ChkLen:	MOV	AX,CX
		TEST	BH,40h
		JNZ	@@Exit
		CMP	AX,WORD PTR 12
		JA	@@Exit
  @@Zero:	XOR	AX,AX
  @@Exit:	RET
GetFS		ENDP

;---------------------------------------------------------------
;	Int 21h with extended error
;---------------------------------------------------------------
;
;#define SUCCESS 0
;#define ERR -1
;
; func = regs.AH
; subfunc = regs.AL
; saveregs = regs
; err = SUCCESS
; ErrDat = 0
;
; if (func < 0x36) { // DOS 1.x error reporting (no Carry flag on error)
;     (void)intdos(&regs, &regs)
;     if (regs.AL == 0xff) err = ERR
; } else if (func == 0x36) {
;     (void)intdos(&regs, &regs)
;     if (regs.AX == 0xffff) err = ERR
; } else { // DOS 2.x+ error reporting (Carry flag set)
;     switch (func) { // try LFN variant for file functions first
;       case 0x39: // mkdir()
;       case 0x3a: // rmdir()
;       case 0x3b: // chdir()
;       case 0x3c: // creat()
;       case 0x3d: // open()
;       case 0x41: // unlink()
;       case 0x43: // stat()/chmod()
;       case 0x47: // getcwd()
;       case 0x4e: // FindFirst()
;       case 0x4f: // FindNext()
;       case 0x56: // rename()
;       case 0x5b: // CreatNew()
;       case 0x60: // truename()
;       case 0x6c: // xcreat()
;         regs.AH = 0x71
;         regs.AL = func
;
;         // replace creat(), open(), CreatNew() with LFN_xcreat()
;         if (func == 0x3c) { // creat()
;             regs.SI = regs.DX // MK_FP(DS,SI) - asciiz filename
;             regs.DX = 0x12 // flags = O_CREAT | O_TRUNC
;             regs.BX = 2 // mode = RW
;             regs.AL = 0x6c // LFN_xcreat()
;         } else if (func == 0x3d) { // open()
;             regs.SI = regs.DX // MK_FP(DS,SI) - asciiz filename
;             regs.DX = 1 // open file
;             regs.BX = regs.AL // mode
;             regs.AL = 0x6c // LFN_xcreat()
;         } else if (func == 0x5b) { // CreatNew()
;             regs.SI = regs.DX // MK_FP(DS,SI) - asciiz filename
;             regs.DX = 0x10 // flags = O_CREAT
;             regs.BX = 2 // mode = RW
;             regs.AL = 0x6c // LFN_xcreat()
;         } else if (func == 0x41) { // unlink()
;             regs.SI = 0 // no wildcards allowed
;         } else if (func == 0x43) { // stat()/chmod()
;             regs.BL = subfunc
;         } else if (func == 0x4e) { // FindFirst()
;             CloseFindFile()
;         } else if (func == 0x4f) { // FindNext()
;             regs.ES = MK_SEG(FindData)
;             regs.DI = MK_OFF(FindData)
;             regs.BX = FindHandle
;             regs.SI = 1 // use MS-DOS (old) date/time format
;         } else if (func == 0x60) { // truename()
;             regs.CX = 0 // truename subfunc
;         }
;         if ((err = intdos(&regs, &regs)) != ERR || regs.AX != 0x7100) { // success
;             if (func == 0x43) { // stat()/chmod()
;                 saveregs.SI = regs.SI
;                 saveregs.DI = regs.DI
;             } else if (func == 0x4e || func == 0x4f) { // FindFirst/Next
;                 if (func == 0x4e) FindHandle = regs.AX
;                 DTA.FilTime = FindData.WinTime.Lo
;                 DTA.FilDate = FindData.WinTime.Hi
;                 FilSize.Lo = FindData.WinSize.Lo
;                 FilSize.Hi = FindData.WinSize.Hi
;                 // clear Temporary file(0x80) bit and reserved bit 6
;                 DTA.FilAttr = FindData.WinAttr.Lo &
;                   (fa_RO | fa_HIDDEN | fa_SYSTEM | fa_LABEL | fa_DIR | fa_ARCHIVE)
;
;                 // no LFN for this entry, use WinLongName
;                 // (see RBIL 21/714E, 'Under Windows95B' note)
;                 if (FindData.WinShortName[0] == '\0') {
;                     for (i = 0; FindData.WinLongName[i] != '\0'; i++)
;                          // don't upcase volume label
;                          FindData.WinShortName[i] = (DTA.FilAttr & fa_LABEL) ?
;                           FindData.WinLongName[i] : UpCase(FindData.WinLongName[i])
;                 }
;
;                 // use LFN only if LFN != SFN
;                 if (strncmp(FindData.WinLongName, FindData.WinShortName,
;                       StrLen(FindData.WinShortName))) {
;                     strcpy(DTA.FilName, FindData.WinLongName)
;                     FindData.WinLongName[0] = '\0'
;                 } else {
;                     strcpy(DTA.FilName, FindData.WinShortName)
;                 }
;
;                 if (ConvCase &&
;                      // don't lowercase volume label
;                      // ignore Find calls used to calculate directory size
;                      (DTA.FilAttr & (fa_LABEL | fa_DIR)) == 0)
;                     UpPathLowFname(DTA.FilName)
;             }
;             saveregs.FLAGS = regs.FLAGS
;             saveregs.AX = regs.AX
;             saveregs.CX = regs.CX
;             regs = saveregs
;             break
;         }
;
;       default: // fallback to SFN functions
;         regs = saveregs
;         if (func == 0x4e || func == 0x4f) { // FindFirst/Next
;             f_label = (func == 0x4e && regs.CL & fa_LABEL) ? true : false
;             SetDTA(DTA)
;             err = intdos(&regs, &regs)
;             FindData.WinLongName[0] = '\0'
;             if (ConvCase && !f_label && // don't lowercase volume label
;                  // ignore Find calls used to calculate directory size
;                  (DTA.FilAttr & (fa_LABEL | fa_DIR)) == 0) {
;                 for(i = 0; i < 13; i++) DTA.FilName[i] = LowCase(DTA.FilName[i])
;             }
;         } else { // other DOS functions
;             err = intdos(&regs, &regs)
;         }
;     }
; }
;
; if (ErrDat) {
;     err = CRITERR_FAIL
;     regs.FLAGS |= F_CARRY
; } else if (err == ERR) {
;     err = GetExtendedError()
;     regs.FLAGS |= F_CARRY
; }
;
; return err
;
		.DATA
FuncVFAT_T	DB	39h, 3Ah, 3Bh, 3Ch, 3Dh, 41h, 43h
		DB	47h, 4Eh, 4Fh, 56h, 5Bh, 60h, 6Ch

		.CODE	Subs
Intr21		PROC	FAR
		PUSH	DS
		MOV	DS,CS:DataSegment
		MOV	DS:ErrDat,0
		POP	DS
		CMP	AH,36h			; DOS 2.X style functions
		JE	@@Func36
		JA	@@NewFunc
		INT	21h
		CMP	AL,0FFh			; DOS 1.X style error
		CMC				; set CARRY flag
		JMP	@@CheckErr

  @@Func36:
		INT	21h			; GetFreeSpace()
		CMP	AX,WORD PTR 0FFFFh	; invalid drive error
		CMC				; set CARRY flag
		JMP	@@CheckErr

  @@NewFunc:
		PUSH	ES			; LFN functions
		PUSH	DI
		PUSH	CX
		PUSH	AX
		PUSH	CS
		POP	ES
		MOV	DI,OFFSET FuncVFAT_T
		MOV	CX,14
		MOV	AL,AH
		CLD
		REPNE	SCASB
		POP	AX
		POP	CX
		POP	DI
		POP	ES
		JNE	@@OldStyle
		PUSH	ES
		PUSH	DI
		PUSH	SI
		PUSH	DX
		PUSH	BX
		PUSH	AX
		CMP	AH,3Ch			; creat()
		JNE	@@Func3D
		MOV	AX,1202h
		JMP	@@LFN_xcreat

  @@Func3D:
		CMP	AH,3Dh			; open()
		JNE	@@Func41
		MOV	AH,1
		JMP	@@LFN_xcreat

  @@Func41:
		CMP	AH,41h			; unlink()
		JNE	@@Func43
		XOR	SI,SI

  @@Func43:
		CMP	AH,43h			; stat()/chmod()
		JNE	@@Func4E
		MOV	BL,AL

  @@Func4E:
		CMP	AH,4Eh			; FindFirst()
		JNE	@@Func4F
		CALL	CloseFindFile
		JMP	@@FuncFind

  @@Func4F:
		CMP	AH,4Fh			; FindNext()
		JNE	@@Func60

  @@FuncFind:
		MOV	ES,CS:DataSegment
		MOV	DI,OFFSET FindData
		MOV	BX,ES:FindHandle
		MOV	SI,1

  @@Func60:
		CMP	AH,60h			; truename()
		JNE	@@Func5B
		XOR	CX,CX

  @@Func5B:
		CMP	AH,5Bh			; CreatNew()
		JNE	@@LFN
		MOV	AX,1002h		; flags = O_CREAT, mode = RW

  @@LFN_xcreat:
		MOV	SI,DX
		MOV	DL,AH
		MOV	DH,0			; flags
		MOV	BL,AL
		MOV	BH,0			; mode
		MOV	AH,6Ch

  @@LFN:
		MOV	AL,AH			; LFN call
		MOV	AH,71h
		STC
		INT	21h
		PUSHF
IF	__TASMIDENT__
		JC	@F
		JMP	NEAR PTR @@LFNOk
  @@:
ELSE
		JNC	@@LFNOk
ENDIF
		CMP	AX,7100h
		JNE	@@LFNOk
		POPF
		POP	AX
		POP	BX
		POP	DX
		POP	SI
		POP	DI
		POP	ES

  @@OldStyle:
		CMP	AH,4Eh			; DOS 3.X style function
		JE	@@OldFind
		CMP	AH,4Fh
		JE	@@OldFind
		STC
		INT	21h
		JMP	@@CheckErr

  @@OldFind:
		PUSH	DS			; DOS 3.X find
		PUSH	DX
		PUSH	AX
		MOV	DS,CS:DataSegment
		MOV	AL,0
		CMP	AH,4Eh
		JNE	@@LabelFlag
		TEST	CL,fa_LABEL
		JZ	@@LabelFlag
		MOV	AL,1

  @@LabelFlag:
		MOV	BYTE PTR DS:FindData[0],AL
		MOV	DX,OFFSET DTA
		MOV	AH,1Ah
		INT	21h			; SetDTA()
		POP	AX
		POP	DX
		POP	DS
		STC
		INT	21h
		PUSHF
		PUSH	DS
		PUSH	SI
		PUSH	CX
		PUSH	AX
		MOV	DS,CS:DataSegment
		MOV	DS:FindData.WinLongName,0
		CMP	BYTE PTR DS:FindData[0],0
		JNZ	@@OldFindExit
		TEST	DS:DTA.FilAttr,fa_LABEL OR fa_DIR
		JNZ	@@OldFindExit
		CMP	DS:ConvCase,0
		JE	@@OldFindExit
		MOV	SI,OFFSET DTA.FilName
		MOV	CX,13

  @@OldFindCnv:
		MOV	AL,DS:[SI]
		CALL	LowCase
		MOV	DS:[SI],AL
		INC	SI
		LOOP	@@OldFindCnv

  @@OldFindExit:
		POP	AX
		POP	CX
		POP	SI
		POP	DS
		POPF
		JMP	@@CheckErr

  @@LFNOk:
		POPF				; LFN analyse results
		POP	BX			; regs.AX
		PUSHF
		JC	@@LFNEpilogue
		CMP	BH,43h			; LFN get/set attr/date/time
		JNE	@@ChkLFNFind
		POPF
		POP	BX
		POP	DX
		POP	ES			; keep regs.SI, regs.DI
		POP	ES
		POP	ES
		JMP	@@CheckErr

  @@ChkLFNFind:
		CMP	BH,4Fh			; LFN find convert
		JE	@@FindResult
		CMP	BH,4Eh
		JNE	@@LFNEpilogue
		MOV	ES:FindHandle,AX

  @@FindResult:
		PUSH	DS
		PUSH	AX
		PUSH	ES
		POP	DS
		CLD
		MOV	AX,DS:FindData.WinTime.Lo
		MOV	DS:DTA.FilTime,AX
		MOV	AX,DS:FindData.WinTime.Hi
		MOV	DS:DTA.FilDate,AX
		MOV	AX,DS:FindData.WinSize.Lo
		MOV	DS:DTA.FilSize.Lo,AX
		MOV	AX,DS:FindData.WinSize.Hi
		MOV	DS:DTA.FilSize.Hi,AX
		MOV	AX,DS:FindData.WinAttr.Lo
		AND	AL,fa_RO OR fa_HIDDEN OR fa_SYSTEM OR fa_LABEL OR fa_DIR OR fa_ARCHIVE
		MOV	DS:DTA.FilAttr,AL
		MOV	BL,AL
		MOV	SI,OFFSET FindData.WinLongName
		MOV	DI,OFFSET FindData.WinShortName
		CMP	BYTE PTR DS:[DI],0
		JNE	@@CompNames
		PUSH	SI
		PUSH	DI

  @@MoveShort:
		LODSB
		TEST	BL,fa_LABEL
		JNZ	@@SaveChar
		CALL	UpCase

  @@SaveChar:
		STOSB
		CMP	AL,0
		JNE	@@MoveShort
		POP	DI
		POP	SI

  @@CompNames:
		PUSH	DI
		PUSH	SI
		PUSH	CX
		CALL	StrLen
		REPE	CMPSB
		POP	CX
		POP	SI
		POP	DI
		JE	@@MoveName
		MOV	SI,DI

  @@MoveName:
		MOV	DI,OFFSET DTA.FilName
		PUSH	SI
		PUSH	DI
		CALL	strcpy
		POP	SI
		TEST	BL,fa_LABEL OR fa_DIR
		JNZ	@@SkipConv
		CMP	DS:ConvCase,0
		JE	@@SkipConv
		CALL	UpPathLowFname

  @@SkipConv:
		POP	SI
		POP	AX
		POP	DS
		CMP	SI,OFFSET FindData.WinLongName
		JNE	@@LFNEpilogue
		MOV	BYTE PTR ES:[SI],0

  @@LFNEpilogue:
		POPF
		POP	BX
		POP	DX
		POP	SI
		POP	DI
		POP	ES

  @@CheckErr:
		PUSHF				; Get error code
		PUSH	DS
		MOV	DS,CS:DataSegment
		CMP	DS:ErrDat,0
		POP	DS
		JNE	@@CritError
		POPF
		JNC	@@Exit
		CMP	AL,1
		JB	@@Exit
		PUSH	DS
		PUSH	ES
		PUSH	BP
		PUSH	SI
		PUSH	DI
		PUSH	DX
		PUSH	CX
		PUSH	BX
		XOR	BX,BX
		MOV	AH,59h
		INT	21h			; GetExtendedError()
		POP	BX
		POP	CX
		POP	DX
		POP	DI
		POP	SI
		POP	BP
		POP	ES
		POP	DS
		STC
		RET

  @@CritError:
		POPF
		MOV	AX,ErrFail
		STC
  @@Exit:	RET
Intr21		ENDP

;---------------------------------------------------------------
;	Close Find File Handle
;---------------------------------------------------------------
CloseFindFile	PROC	FAR	USES	AX BX DS
		PUSHF
		MOV	DS,CS:DataSegment
		MOV	BX,DS:FindHandle
		OR	BX,BX
		JE	@@Exit
		MOV	AX,71A1h
		INT	21h
		MOV	DS:FindHandle,0
  @@Exit:	POPF
		RET
CloseFindFile	ENDP

;---------------------------------------------------------------
;	Get Long Name from EA
;		DS:SI - file name
;		DS:BP - limit
;		ES:DI - buffer for long name
;	Return:	   CY - error,	AX = error code
;		   NC - Ok,	AX = length of name
;---------------------------------------------------------------
		.CONST
LongName_S	DD	15
		DB	9
		DB	'.LONGNAME',0

		.CODE	Subs
GetLongNameEA	PROC	FAR	USES	BX CX DX SI DI DS ES
		LOCAL	EAInfoBuffer[12]:BYTE
		MOV	DX,DS
		MOV	DS,CS:DataSegment
		CMP	DS:OS2Box,0
		JZ	@@Error
		MOV	EAInfoBuffer[0].Offs,OFFSET LongName_S
		MOV	EAInfoBuffer[0].Segm,CS
		MOV	EAInfoBuffer[4].Offs,DI
		MOV	EAInfoBuffer[4].Segm,ES
		MOV	AX,[BP]
		SUB	AX,DI
		JBE	@@Error
		CMP	AX,WORD PTR 4
IF	__TASMIDENT__
		JA	@F
		JMP	NEAR PTR @@Error
  @@:
ELSE
		JBE	@@Error
ENDIF
		MOV	ES:[DI].Lo,AX
		MOV	ES:[DI].Hi,0
		MOV	DS:BlockCritErrBox,1
		PUSH	DS
		PUSH	ES
		PUSH	DI
		PUSH	SS
		POP	ES
		LEA	DI,EAInfoBuffer
		MOV	DS,DX
		MOV	BX,0FFFFh
		MOV	CX,12
		MOV	DX,3
		MOV	AX,5702h
		CALL	Intr21
		POP	DI
		POP	ES
		POP	DS
		MOV	DS:BlockCritErrBox,0
		JC	@@Exit
		CMP	ES:[DI].Lo,4
		JBE	@@Error
		MOV	CX,ES:[DI+6]
		SUB	CX,3
		JBE	@@Error
		MOV	SI,DI
		MOV	AL,ES:[DI+5]
		MOV	AH,0
		ADD	AX,WORD PTR 13
		ADD	SI,AX
		PUSH	DI
		DEC	CX
  @@EAMoveName:	LODS	WORD PTR ES:[SI]
		CMP	CX,1
		JBE	@@OneByte
		DEC	CX
		CMP	AX,0A0Dh
		JE	@@Space
		CMP	AX,0D0Ah
		JE	@@Space
		INC	CX
  @@OneByte:	DEC	SI
		CMP	AL,13
		JE	@@Space
		CMP	AL,10
		JE	@@Space
		CMP	AL,'^'
		JNE	@@EAMoveNext
  @@Space:	MOV	AL,' '
  @@EAMoveNext:	STOSB
		LOOP	@@EAMoveName
		MOV	AL,0
		STOSB
		POP	SI
		MOV	AX,DI
		SUB	AX,SI
		JMP	@@Exit
  @@Error:	XOR	AX,AX
		MOV	ES:[DI],AL
  @@Exit:	RET
GetLongNameEA	ENDP

;---------------------------------------------------------------
;	OS/2 long name
;		DS:SI - source name
;		ES:DI - target name
;---------------------------------------------------------------
LongNameOS2	PROC	FAR	USES	BX CX DX SI DI
		LOCAL	NameOS2[300]:BYTE
		MOV	BX,SI
		LEA	CX,[DI+LenPath-1]
		LODSW
		CMP	AL,0
		JE	@@Exit
		STOSW
		CMP	AH,':'
		JE	@@ChkSlash
		OR	AH,AH
		JE	@@Exit
		DEC	SI
		DEC	SI
		DEC	DI
		DEC	DI
  @@ChkSlash:	MOV	DX,SI
		LODSB
		CMP	AL,0
		JE	@@Exit
		CMP	AL,'\'
		JNE	@@EndName
		STOSB
		CMP	DI,CX
		JB	@@ChkSlash
		JMP	@@Exit
  @@EndName:	LODSB
		CMP	AL,0
		JE	@@SetEnd
		CMP	AL,'\'
		JNE	@@EndName
  @@SetEnd:	DEC	SI
		PUSH	ES
		PUSH	DI
		PUSH	DX
		PUSH	SS
		POP	ES
		LEA	DI,NameOS2
		PUSH	BP
		LEA	BP,[DI+300]
		MOV	DL,0
		XCHG	DL,DS:[SI]
		XCHG	SI,BX
		CALL	GetLongNameEA
		XCHG	SI,BX
		MOV	DS:[SI],DL
		POP	BP
		POP	DX
		POP	DI
		POP	ES
		JC	@@Error
		PUSH	DS
		PUSH	SI
		OR	AX,AX
		JNE	@@Move1
		MOV	AX,SI
		SUB	AX,DX
		MOV	SI,DX
		JMP	@@Move2
  @@Move1:	PUSH	SS
		POP	DS
		LEA	SI,NameOS2
		DEC	AX
  @@Move2:	MOV	DX,CX
		SUB	DX,DI
		JB	@@Move4
		PUSH	CX
		MOV	CX,AX
		MOV	AL,1
		CMP	CX,DX
		JBE	@@Move3
		MOV	CX,DX
		MOV	AL,0
  @@Move3:	REP	MOVSB
		CMP	AL,1
		POP	CX
  @@Move4:	POP	SI
		POP	DS
		JNC	@@ChkSlash
		CLC
		JMP	@@Exit
  @@Error:	PUSH	AX
		MOV	SI,DX
  @@AddName:	LODSB
		STOSB
		CMP	DI,CX
		JAE	@@ExitName
		CMP	AL,0
		JNE	@@AddName
  @@ExitName:	POP	AX
		STC
  @@Exit:	MOV	BYTE PTR ES:[DI],0
		RET
LongNameOS2	ENDP

;---------------------------------------------------------------
;	Real path
;		DS:SI - buffer for path
;		   AL - set current dir
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
RealPath	PROC	FAR	USES	CX DX SI DI DS ES
		LOCAL	PathBuf[LenPath]:BYTE
		MOV	DL,AL
		PUSH	SS
		POP	ES
		LEA	DI,PathBuf
		MOV	CX,8002h
		MOV	AX,7160h
		CALL	Intr21
		JNC	@@ChDir
		XOR	AX,WORD PTR ErrFail
		JNE	@@Exit
		STC
		JMP	@@Exit
  @@ChDir:	PUSH	DS
		PUSH	ES
		POP	DS
		POP	ES
		OR	DL,DL
		JZ	@@Move
		MOV	DX,DI
		MOV	AH,3Bh
		CALL	Intr21
		JC	@@Exit
  @@Move:	XCHG	SI,DI
		CALL	strcpy
		CLC
  @@Exit:	RET
RealPath	ENDP

;---------------------------------------------------------------
;	Open file
;		DS:DX - filename
;		   AL - share mode
;	Return:    AX - handle, if CY=0
;		   AX - error,  if CY=1
;---------------------------------------------------------------
OpenFil		PROC	FAR	USES	BX CX
		XOR	CX,CX
		MOV	AH,3Dh
		MOV	BX,AX
		CALL	Intr21
		JNC	@@Exit
		CMP	AX,WORD PTR 20h
		STC
		JNE	@@Exit
		MOV	AX,BX
		AND	AL,0Fh
		CALL	Intr21
  @@Exit:	RET
OpenFil		ENDP

;---------------------------------------------------------------
;	Open temporary file
;		CS:SI - filename
;		ES:DX - buffer for opened file
;		ES:DI - end of buffer
;	Return:	   BX - file handle
;		   NC - Ok,	AX - number of file
;		   CY - error,	AX - error code
;---------------------------------------------------------------
OpenTempFile	PROC	FAR	USES	CX SI DS
		XOR	AX,AX
		CALL	MakeFileName
		PUSH	ES
		POP	DS
		CALL	ChgDisk
		MOV	AX,ErrFail
		JC	@@Error
		XOR	BX,BX
  @@OpenFile:	MOV	CX,20h
		MOV	AH,5Bh
		CALL	Intr21			; CreatNew()
		JNC	@@Exit
		CMP	AX,WORD PTR 3
		JBE	@@Error
		MOV	SI,DI
		MOV	CX,3
  @@NextFile:	INC	BX
		DEC	SI
		INC	BYTE PTR DS:[SI]
		CMP	BYTE PTR DS:[SI],'9'
		JBE	@@OpenFile
		MOV	BYTE PTR DS:[SI],'0'
		LOOP	@@NextFile
  @@Error:	STC
		XCHG	AX,BX
  @@Exit:	XCHG	AX,BX
		RET
OpenTempFile	ENDP

;---------------------------------------------------------------
;	Make temporary file name
;		   AX - file number
;		CS:SI - file name
;		ES:DI - target buffer
;	Return:	ES:DI - end of buffer
;---------------------------------------------------------------
MoveTempPath	PROC	FAR	USES	AX SI DS
		MOV	DS,CS:DataSegment
		MOV	SI,OFFSET TempDir
		CALL	strcpy
		RET
MoveTempPath	ENDP

MakePathName	PROC	FAR
		CALL	MoveTempPath
MakePathName	ENDP

MakeFileName	PROC	FAR	USES	AX CX DX SI DS
		MOV	CX,AX
		PUSH	CS
		POP	DS
		CALL	strcpy
		MOV	AL,'.'
		STOSB
		MOV	AX,CX
		MOV	CX,10
		XOR	DX,DX
		DIV	CX
		PUSH	DX
		XOR	DX,DX
		DIV	CX
		ADD	AL,'0'
		STOSB
		MOV	AL,DL
		ADD	AL,'0'
		STOSB
		POP	AX
		ADD	AL,'0'
		STOSW
		DEC	DI
		RET
MakeFileName	ENDP

;---------------------------------------------------------------
;	Get file date and time
;		DS:DX - file
;	Return:    CX - time
;		   DI - date
;---------------------------------------------------------------
GetFileTime	PROC	FAR	USES	BX DX SI DS
		PUSH	ES
		POP	DS
		CMP	DS:OS2box,0
		JE	@@WinDate
		MOV	SI,DX
		MOV	DI,OFFSET OS2AttrBuf
		MOV	DX,1
		MOV	CX,22
		MOV	BX,0FFFFh
		MOV	AX,5702h
		CALL	Intr21			; DosQFileInfo() [OS/2]
		MOV	CX,DS:[DI+8].Hi
		MOV	DI,DS:[DI+8].Lo
		JMP	@@Exit
  @@WinDate:	MOV	AX,4304h
		CALL	Intr21
  @@Exit:	RET
GetFileTime	ENDP

;---------------------------------------------------------------
;	Set file date and time
;		DS:DX - file
;		   CX - time
;		   DI - date
;---------------------------------------------------------------
SetFileTime	PROC	FAR	USES	BX CX DX SI DI DS
		PUSH	ES
		POP	DS
		MOV	AX,4303h
		CMP	DS:OS2box,0
		JE	@@Exec
		MOV	SI,DX
		MOV	AX,DI
		MOV	DI,OFFSET OS2AttrBuf
		MOV	DS:[DI+8].Lo,AX
		MOV	DS:[DI+8].Hi,CX
		MOV	DX,1
		MOV	CX,22
		MOV	BX,0FFFFh
		MOV	AX,5703h
  @@Exec:	CALL	Intr21			; DosSetFileInfo() [OS/2]
		RET
SetFileTime	ENDP

;---------------------------------------------------------------
;	Invert global variable
;		ES:BX - address of variable
;---------------------------------------------------------------
InvVar		PROC	FAR	USES	DS
		PUSH	ES
		POP	DS
		CALL	InvPanelVar
		RET
InvVar		ENDP

;---------------------------------------------------------------
;	Invert panel variable
;		DS:BX - address of variable
;---------------------------------------------------------------
InvPanelVar	PROC	FAR
		CMP	BYTE PTR DS:[BX],1
		MOV	AL,0
		ADC	AL,0
		MOV	DS:[BX],AL
		RET
InvPanelVar	ENDP

;---------------------------------------------------------------
;	Check sum
;		DS:SI - address
;		   CX - length
;	Return:	   AX - check sum
;---------------------------------------------------------------
ChkSum		PROC	FAR	USES	BX CX SI
		XOR	BX,BX
		MOV	AH,0
  @@GetChar:	LODSB
		ADD	BX,AX
		LOOP	@@GetChar
		LODSW
		CMP	AX,BX
		MOV	AX,BX
		RET
ChkSum		ENDP

;---------------------------------------------------------------
;	File Number -> Address
;		   SI - Number
;	Return:	DS:SI - Address
;---------------------------------------------------------------
Numb2Addr	PROC	FAR	USES	AX CX
		MOV	CX,SI
		MOV	SI,DS:DiskBuf
		JCXZ	@@Exit
  @@Next:	CALL	NextFilEnt
		LOOP	@@Next
  @@Exit:	RET
Numb2Addr	ENDP

;---------------------------------------------------------------
;	File Address -> Number
;		DS:SI - Address
;	Return:	   SI - Number
;---------------------------------------------------------------
Addr2Numb	PROC	FAR	USES	AX BX
		MOV	AX,SI
		MOV	SI,DS:DiskBuf
		XOR	BX,BX
  @@Next:	CMP	AX,SI
		JBE	@@Exit
		CALL	NextFilEnt
		INC	BX
		JMP	@@Next
  @@Exit:	MOV	SI,BX
		RET
Addr2Numb	ENDP

;---------------------------------------------------------------
;	Tree Number -> Address
;		   SI - Number
;	Return:	   SI - Address
;---------------------------------------------------------------
TreeNumb2Adr	PROC	FAR	USES	DS
		MOV	DS,ES:TreeSeg
		CALL	TreeNumb2Adr1
		RET
TreeNumb2Adr	ENDP

TreeNumb2Adr1	PROC	FAR	USES	AX CX
		MOV	CX,SI
		MOV	SI,OFFSET TreeList
		JCXZ	@@Exit
  @@Next:	CALL	NextDirEnt
		LOOP	@@Next
  @@Exit:	RET
TreeNumb2Adr1	ENDP

TreeNumb2AdrH	PROC	FAR	USES	DS
		MOV	DS,ES:TreeSeg
		CALL	TreeNumb2AdrH1
		RET
TreeNumb2AdrH	ENDP

TreeNumb2AdrH1	PROC	FAR	USES	AX CX
		MOV	CX,SI
		MOV	SI,OFFSET TreeList
		JCXZ	@@Exit
  @@Next:	CALL	NextDirEntH
		LOOP	@@Next
  @@Exit:	RET
TreeNumb2AdrH1	ENDP

;---------------------------------------------------------------
;	Tree Address -> Number
;		   SI - Address
;	Return:	   SI - Number
;---------------------------------------------------------------
TreeAdr2Numb	PROC	FAR	USES	CX
		MOV	CX,OFFSET NextDirEnt
		CALL	TreeAdr2Numb0
		RET
TreeAdr2Numb	ENDP

TreeAdr2NumbH	PROC	FAR	USES	CX
		MOV	CX,OFFSET NextDirEntH
		CALL	TreeAdr2Numb0
		RET
TreeAdr2NumbH	ENDP

TreeAdr2Numb0	PROC	NEAR	USES	AX BX DS
		MOV	DS,ES:TreeSeg
		MOV	AX,SI
		MOV	SI,OFFSET TreeList
		XOR	BX,BX
  @@Next:	CMP	AX,SI
		JBE	@@Exit
		PUSH	CS
		CALL	CX
		INC	BX
		JMP	@@Next
  @@Exit:	MOV	SI,BX
		RET
TreeAdr2Numb0	ENDP

;---------------------------------------------------------------
;	Allocate block in WorkBuffer
;		DS - segment of block
;		BP - size of block
;---------------------------------------------------------------
AllocateBlock	PROC	FAR	USES	AX CX
		MOV	CL,4
		MOV	AX,BP
		DEC	AX
		SHR	AX,CL
		INC	AX
		MOV	CX,DS
		ADD	AX,CX
		MOV	ES:WorkBuffer,AX
		RET
AllocateBlock	ENDP

;---------------------------------------------------------------
;	Find end of segment
;		   BX - segment address
;		CS:SI - table of segments
;	Return:	   DX - address of next segment
;---------------------------------------------------------------
		.DATA
AllSegs_L	DW	GroupTree1, GroupTree2, GroupDescr1, GroupDescr2, GroupFiles
GlobalSegs_L	DW	WCB1seg, WCB2seg, TreeSeg, DirinfoSeg, ViewSeg
		DW	ScreenBuf, WorkBuffer, 0
		DW	WCBcrn, WCBsec, UserScreen, WindowsPoint, 0

		.CODE	Subs
FindNextSeg	PROC	FAR	USES	AX SI DI
		MOV	DX,0FFFFh
  @@Compare:	MOV	DI,CS:[SI]
		OR	DI,DI
		JE	@@Exit
		INC	SI
		INC	SI
		MOV	AX,ES:[DI]
		CMP	AX,BX
		JB	@@Compare
		CMP	AX,DX
		JAE	@@Compare
		MOV	DX,AX
		JMP	@@Compare
  @@Exit:	RET
FindNextSeg	ENDP

;---------------------------------------------------------------
;	Compress block
;		   BX - segment address
;		CS:SI - table of segments
;---------------------------------------------------------------
CompressBlock	PROC	FAR	USES	AX CX DX SI DI
		CMP	BX,1
		JBE	@@Exit
		CALL	FindNextSeg
		CMP	DX,BX
		JBE	@@Exit
		PUSH	SI
		PUSH	BX
		MOV	CX,ES:WorkBuffer
		SUB	CX,DX
		JBE	@@CorSegs
		PUSH	DS
		PUSH	ES
  @@Move:	PUSH	CX
		CMP	CX,0FFFh
		JBE	@@Length
		MOV	CX,0FFFh
  @@Length:	PUSH	CX
		SHL	CX,1
		SHL	CX,1
		SHL	CX,1
		MOV	DS,DX
		MOV	ES,BX
		XOR	SI,SI
		XOR	DI,DI
		REP	MOVSW
		POP	SI
		POP	CX
		ADD	BX,SI
		ADD	DX,SI
		SUB	CX,SI
		JA	@@Move
		POP	ES
		POP	DS
  @@CorSegs:	SUB	DX,BX
		POP	BX
		POP	DI
		MOV	CX,2
  @@NextSeg:	MOV	SI,CS:[DI]
		INC	DI
		INC	DI
		OR	SI,SI
		JE	@@ChkEnd
		CMP	ES:[SI],BX
		JBE	@@NextSeg
		SUB	ES:[SI],DX
		JMP	@@NextSeg
  @@ChkEnd:	DEC	CX
		JNE	@@NextSeg
		MOV	CX,DS
		CMP	CX,BX
		JBE	@@Exit
		SUB	CX,DX
		MOV	DS,CX
  @@Exit:	RET
CompressBlock	ENDP

;---------------------------------------------------------------
;	Expand block
;		   DS - segment of block
;		   AX - expand size
;		CS:SI - table of segments
;	Return:	   AX - new segment size
;---------------------------------------------------------------
ExpandBlock	PROC	FAR	USES	BX CX DX SI DI
		MOV	BX,DS
		INC	BX
		CALL	FindNextSeg
		MOV	DI,DX
		SUB	DX,BX
		MOV	BX,0FFFh
		SUB	BX,DX
		JBE	@@Exit
		MOV	CX,ES
		ADD	CX,ES:SizeDataSeg
		SUB	CX,ES:WorkBuffer
		SUB	CX,MinView/16
		JBE	@@Exit
		CMP	BX,CX
		JBE	@@SetSize1
		MOV	BX,CX
  @@SetSize1:	MOV	CL,4
		DEC	AX
		SHR	AX,CL
		INC	AX
		CMP	AX,BX
		JBE	@@SetSize2
		MOV	AX,BX
  @@SetSize2:	PUSH	SI
		PUSH	AX
		ADD	DX,AX
		MOV	SI,ES:WorkBuffer
		MOV	BX,SI
		SUB	BX,DI
		JBE	@@Correct
		MOV	DI,SI
		ADD	DI,AX
  @@Move1:	MOV	AX,0FFFh
		CMP	AX,BX
		JBE	@@Move2
		MOV	AX,BX
  @@Move2:	SUB	SI,AX
		SUB	DI,AX
		SUB	BX,AX
		PUSH	ES
		PUSH	DS
		PUSH	DI
		PUSH	SI
		MOV	DS,SI
		MOV	ES,DI
		MOV	CL,4
		SHL	AX,CL
		MOV	CX,AX
		MOV	SI,AX
		DEC	SI
		MOV	DI,SI
		STD
		REP	MOVSB
		CLD
		POP	SI
		POP	DI
		POP	DS
		POP	ES
		OR	BX,BX
		JNE	@@Move1
  @@Correct:	POP	AX
		POP	DI
		MOV	BX,DS
		MOV	CX,2
  @@NextSeg:	MOV	SI,CS:[DI]
		INC	DI
		INC	DI
		OR	SI,SI
		JE	@@ChkEnd
		CMP	ES:[SI],BX
		JBE	@@NextSeg
		ADD	ES:[SI],AX
		JMP	@@NextSeg
  @@ChkEnd:	DEC	CX
		JNE	@@NextSeg
  @@Exit:	MOV	CL,4
		MOV	AX,DX
		SHL	AX,CL
		RET
ExpandBlock	ENDP

;---------------------------------------------------------------
;	DWORD * DWORD
;		DS:SI - DWORD 1
;		DS:BX - DWORD 2
;		ES:DI - 8 bytes buffer
;---------------------------------------------------------------
LongMul		PROC	FAR	USES	AX CX DX
		XOR	CX,CX
		MOV	AX,DS:[SI].Lo
		MUL	DS:[BX].Lo
		MOV	ES:[DI+0],AX
		MOV	ES:[DI+2],DX
		MOV	AX,DS:[SI].Lo
		MUL	DS:[BX].Hi
		ADD	ES:[DI+2],AX
		ADC	DX,CX
		MOV	ES:[DI+4],DX
		MOV	AX,DS:[SI].Hi
		MUL	DS:[BX].Hi
		ADD	ES:[DI+4],AX
		ADC	DX,CX
		MOV	ES:[DI+6],DX
		MOV	AX,DS:[SI].Hi
		MUL	DS:[BX].Lo
		ADD	ES:[DI+2],AX
		ADC	ES:[DI+4],DX
		ADC	ES:[DI+6],CX
		RET
LongMul		ENDP

;---------------------------------------------------------------
;	Cluster size of file
;		DX AX - real size
;	Return:	DI DX AX - cluster size
;---------------------------------------------------------------
FileClustSize	PROC	FAR	USES	BX CX SI BP
		XOR	SI,SI
		XOR	DI,DI
		MOV	CX,32
  @@Div:	SHL	AX,1
		RCL	DX,1
		RCL	SI,1
		RCL	DI,1
		SUB	SI,ES:ClusterSize.Lo
		SBB	DI,ES:ClusterSize.Hi
		JNC	@@DivRotate
		ADD	SI,ES:ClusterSize.Lo
		ADC	DI,ES:ClusterSize.Hi
  @@DivRotate:	CMC
		RCL	BX,1
		RCL	BP,1
		LOOP	@@Div
		MOV	CL,32
		OR	SI,DI
		MOV	DI,0
		JE	@@Mul
		ADD	BX,1
		ADC	BP,0
  @@Mul:	SHL	AX,1
		RCL	DX,1
		RCL	DI,1
		RCL	BX,1
		RCL	BP,1
		JNC	@@MulNext
		ADD	AX,ES:ClusterSize.Lo
		ADC	DX,ES:ClusterSize.Hi
		ADC	DI,0
  @@MulNext:	LOOP	@@Mul
		RET
FileClustSize	ENDP

;---------------------------------------------------------------
;	Next File Entry
;		DS:SI - current file entry
;	Return:	DS:SI - next file entry
;---------------------------------------------------------------
NextFilEnt	PROC	FAR	USES	AX
		MOV	AX,SIZE FilEnt
		ADD	AX,DS:[SI].LenShortNamF
		ADD	AX,DS:[SI].LenLongNamF
		ADD	SI,AX
		RET
NextFilEnt	ENDP

;---------------------------------------------------------------
;	Next Directory Entry
;		DS:SI - current directory entry
;	Return:	DS:SI - next directory entry
;---------------------------------------------------------------
NextDirEnt	PROC	FAR	USES	AX
		MOV	AL,DS:[SI].DirShortLen
		MOV	AH,0
		ADD	AX,WORD PTR SIZE DirEnt
		ADD	AX,DS:[SI].DirWinLen
		ADD	AX,DS:[SI].DirOS2Len
		ADD	SI,AX
		RET
NextDirEnt	ENDP

NextDirEntH	PROC	FAR	USES	AX
		CALL	NextDirEnt
		PUSH	DS
		MOV	DS,CS:DataSegment
		CMP	DS:HidDirs,0
		POP	DS
		JNZ	@@Exit
		CMP	SI,DS:TreeSize
		JAE	@@Exit
  @@ChkEntry:	TEST	DS:[SI].DirAttr,fa_HIDDEN OR fa_SYSTEM
		JZ	@@Exit
		MOV	AL,DS:[SI].DirLevel
  @@FindNext:	CALL	NextDirEnt
		CMP	SI,DS:TreeSize
		JAE	@@Exit
		CMP	AL,DS:[SI].DirLevel
		JB	@@FindNext
		JMP	@@ChkEntry
  @@Exit:	RET
NextDirEntH	ENDP

;---------------------------------------------------------------
;	Name Offset
;		DS:SI - current file entry
;	Return:	DS:SI - offset of name
;---------------------------------------------------------------
NameOffset	PROC	FAR			; Short/Long name under Win95
		PUSH	DS			; Short name under OS/2
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JNZ	NameOffsExit
NameOffset	ENDP

NameOffset1	PROC	FAR			; Short/Long name
		CMP	DS:LongNames,0
		JZ	NameOffsExit
		JMP	LongNameOffset1
NameOffset1	ENDP

LongNameOffset	PROC	FAR			; Long name under Win95
		PUSH	DS			; Short name under OS/2
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JNZ	NameOffsExit
LongNameOffset	ENDP

LongNameOffset1	PROC	FAR			; Long name
		CMP	DS:[SI].LenLongNamF,0
		JE	NameOffsExit
		ADD	SI,DS:[SI].LenShortNamF
IFDEF	__JWASM__
NameOffsExit::
ELSE
NameOffsExit:
ENDIF
		ADD	SI,OFFSET NameF
		RET
LongNameOffset1	ENDP

;---------------------------------------------------------------
;	Panel window path
;		   DS - WCB
;	Return:	DS:SI - path
;---------------------------------------------------------------
WinPath		PROC	FAR			; Short/Long path under Win95
		PUSH	DS			; Short path under OS/2
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JNZ	WinPathShort
WinPath		ENDP

WinPath1	PROC	FAR			; Short/Long path
		MOV	SI,OFFSET WinLongPath
		CMP	DS:LongNames,0
		JNZ	@@Exit
IFDEF	__JWASM__
WinPathShort::
ELSE
WinPathShort:
ENDIF
		MOV	SI,OFFSET WinShortPath
  @@Exit:	RET
WinPath1	ENDP

WinPathLong	PROC	FAR			; Long path under Win95
		PUSH	DS			; Short path under OS/2
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JNZ	WinPathShort
		MOV	SI,OFFSET WinLongPath
		RET
WinPathLong	ENDP

;---------------------------------------------------------------
;	Directory name offset
;		   AL - short/long
;		DS:SI - directory entry
;	Return:	DS:SI - name offset
;---------------------------------------------------------------
DirOffset	PROC	FAR			; Short/Long name under Win95
		PUSH	DS			; Short name under OS/2
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JNZ	DirOffsetExit
DirOffset	ENDP

DirOffset1	PROC	FAR			; Short/Long name
		CMP	AL,0
		JZ	DirOffsetExit
		JMP	LongDirOffset1
DirOffset1	ENDP

LongDirOffset	PROC	FAR			; Long name under Win95
		PUSH	DS			; Short name under OS/2
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JNZ	DirOffsetExit
LongDirOffset	ENDP

LongDirOffset1	PROC	FAR			; Long name
		CMP	DS:TreeLFN,0
		JE	@@ChkOS2
		CMP	DS:[SI].DirWinLen,0
		JE	DirOffsetExit
		PUSH	AX
		MOV	AL,DS:[SI].DirShortLen
		MOV	AH,0
		JMP	@@Offset
  @@ChkOS2:	PUSH	DS
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JZ	DirOffsetExit
		CMP	DS:[SI].DirOS2Len,0
		JE	DirOffsetExit
		PUSH	AX
		MOV	AL,DS:[SI].DirShortLen
		MOV	AH,0
		ADD	AX,DS:[SI].DirWinLen
  @@Offset:	ADD	SI,AX
		POP	AX
IFDEF	__JWASM__
DirOffsetExit::
ELSE
DirOffsetExit:
ENDIF
		ADD	SI,OFFSET DirName
		RET
LongDirOffset1	ENDP

;---------------------------------------------------------------
;	Directory name length
;		   AL - short/long
;		DS:SI - directory entry
;	Return:	   AX - name length
;---------------------------------------------------------------
DirLen		PROC	FAR			; Short/Long length under Win95
		PUSH	DS			; Short length under OS/2
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JNZ	ShortDirLen
DirLen		ENDP

DirLen1		PROC	FAR			; Short/Long length
		CMP	AL,0
		JZ	ShortDirLen
		JMP	LongDirLen1
DirLen1		ENDP

LongDirLen	PROC	FAR			; Long length under Win95
		PUSH	DS			; Short length under OS/2
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JNZ	ShortDirLen
LongDirLen	ENDP

LongDirLen1	PROC	FAR			; Long length
		MOV	AX,DS:[SI].DirWinLen
		CMP	DS:TreeLFN,0
		JNE	@@ChkOffs
		PUSH	DS
		MOV	DS,CS:DataSegment
		CMP	DS:OS2box,0
		POP	DS
		JZ	ShortDirLen
		MOV	AX,DS:[SI].DirOS2Len
  @@ChkOffs:	OR	AX,AX
		JNE	@@Exit
IFDEF	__JWASM__
ShortDirLen::
ELSE
ShortDirLen:
ENDIF
		MOV	AL,DS:[SI].DirShortLen
		MOV	AH,0
  @@Exit:	RET
LongDirLen1	ENDP
