;---------------------------------------------------------------
;		Description
;
;		Version:	28.04.1998
;---------------------------------------------------------------

;---------------------------------------------------------------
;	Read description to WorkBuffer
;		ES:DX - path
;		   DS - buffer segment, 0-not loaded yet, 1-was error
;		   CX - reserve space
;	Return:	   CY - error
;		   DS - buffer segment, 1-was error
;		   BP - limit
;---------------------------------------------------------------
		.CODE	Subs
GetDescription	PROC	FAR
		USES	BX,CX,DI
		MOV	BP,DS:DscrSegSize
		MOV	AX,DS
		CMP	AX,1
		JA	@@Exit
		STC
		JE	@@Exit
		MOV	BX,CX
		MOV	DS,ES:WorkBuffer
		MOV	BP,ES
		ADD	BP,ES:SizeDataSeg
		MOV	CX,DS
		SUB	BP,CX
		CMP	BP,0FFFh
		JBE	@@RdDescr
		MOV	BP,0FFFh
  @@RdDescr:	MOV	CL,4
		SHL	BP,CL
		MOV	DI,OFFSET DscrBegin
		CALL	ReadDescription
		JC	@@Exit
		MOV	DS:DscrName,AX
		MOV	DS:DscrAttr,CL
		MOV	DS:DscrSize,DI
		ADD	DI,BX
		CMC
		JNC	@@Exit
		CMP	DI,BP
		JAE	@@Exit
		MOV	BP,DI
		XOR	AX,AX
  @@Exit:	MOV	DS:DscrSegSize,BP
		RET
GetDescription	ENDP

;---------------------------------------------------------------
;	Read description
;		ES:DX - path
;		DS:DI - buffer to read
;		   BP - limit
;	Return:	   CY - error, AX - error code
;		   NC - Ok, AX - description name offset
;		   CX - attributes
;		   DI - end of description in buffer
;---------------------------------------------------------------
ReadDescription	PROC	FAR
		USES	BX,DX,SI
		LOCAL	DescrFileName:BYTE:512
		MOV	SI,OFFSET DescrFiles
  @@DescrName:	LODS	BYTE PTR ES:[SI]
		CMP	AL,' '
		JE	@@DescrName
		CMP	AL,';'
		JE	@@DescrName
		CMP	AL,','
		JE	@@DescrName
		CMP	AL,0
		JE	@@Error
		DEC	SI
		PUSH	DS
		PUSH	ES
		PUSH	DI
		PUSH	DX
		PUSH	SI
		PUSH	ES
		POP	DS
		MOV	SI,DX
		CALL	CutPath
		MOV	CX,SI
		SUB	CX,DX
		MOV	SI,DX
		PUSH	SS
		POP	ES
		LEA	DI,DescrFileName
		MOV	DX,DI
		REP	MOVSB
		POP	SI
		MOV	CX,SI
  @@GetDescr:	LODSB
		CMP	AL,';'
		JE	@@EndList
		CMP	AL,','
		JE	@@EndList
		CMP	AL,0
		JE	@@EndList
		STOSB
		JMP	@@GetDescr
  @@EndList:	DEC	SI
		MOV	AL,0
		STOSB
		PUSH	SS
		POP	DS
		MOV	AL,40h
		CALL	OpenFil
		POP	DX
		POP	DI
		POP	ES
		POP	DS
		JNC	@@RdDescr
		CMP	AX,53h
		JNE	@@DescrName
  @@Error:	STC
		JMP	@@Exit
  @@RdDescr:	PUSH	CX
		MOV	BX,AX
		MOV	DX,DI
		MOV	CX,[BP]
		SUB	CX,DX
		DEC	CX
		MOV	AH,3Fh
		CALL	Intr21
		POP	CX
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	BX
		POP	SI
		JC	@@Exit
		MOV	AX,BX
		PUSH	SI
		POPF
		JC	@@Exit
		ADD	DI,AX
		PUSH	DS
		PUSH	CX
		PUSH	SS
		POP	DS
		LEA	DX,DescrFileName
		MOV	AX,4300h
		CALL	Intr21
		POP	DX
		POP	DS
		JC	@@Exit
		MOV	AX,DX
  @@Exit:	RET
ReadDescription	ENDP

;---------------------------------------------------------------
;	Find file in description
;		   DS - segment of description
;		ES:DX - path
;		   AX - file number
;	Return:    CY - not found
;		   NC - found, AX - length of name
;		DS:SI - entry point
;		   CX - length
;---------------------------------------------------------------
FindDescription	PROC	FAR
		USES	DX,DI
		PUSH	AX
		CALL	FindDescr1
		POP	DX
		JNC	@@Exit
		MOV	AX,DX
		MOV	DX,OFFSET DTA.FilName
		CALL	FindDescr1
  @@Exit:	RET
FindDescription	ENDP

FindDescr1	PROC	NEAR
		USES	DI,BP
		MOV	CX,AX
		INC	CX
		PUSH	DS
		PUSH	ES
		POP	DS
		MOV	SI,DX
		CALL	CutPath
		MOV	DI,SI
		POP	DS
		MOV	SI,OFFSET DscrBegin
  @@NewLine:	CMP	SI,DS:DscrSize
		JAE	@@Exit
		LODSB
		CMP	AL,0Dh
		JE	@@NewLine
		CMP	AL,0Ah
		JE	@@NewLine
		DEC	SI
		PUSH	SI
		PUSH	DI
		MOV	DL,0
		CMP	BYTE PTR DS:[SI],'"'
		JNE	@@CompName
		INC	SI
		MOV	DL,1
  @@CompName:	MOV	AL,ES:[DI]
		CMP	AL,0
		JE	@@ChkFound
		INC	DI
		CALL	UpCase
		MOV	AH,AL
		LODSB
		CALL	UpCase
		CMP	AL,AH
		JE	@@CompName
		POP	DI
		POP	SI
  @@SkipLine:	CMP	SI,DS:DscrSize
		JAE	@@Exit
		LODSB
		CMP	AL,0Dh
		JE	@@NewLine
		CMP	AL,0Ah
		JE	@@NewLine
		JMP	@@SkipLine
  @@ChkFound:	MOV	BP,SI
		CMP	SI,DS:DscrSize
		LODSB
		POP	DI
		POP	SI
		JA	@@Exit
		JE	@@ChkNoQuote
		CMP	AL,0Dh
		JE	@@ChkNoQuote
		CMP	AL,0Ah
		JE	@@ChkNoQuote
		CMP	AL,1Ah
		JE	@@ChkNoQuote
		OR	DL,DL
		JNE	@@ChkQuote
		CMP	AL,' '
		JE	@@ChkNumber
		CMP	AL,9
		JE	@@ChkNumber
		JMP	@@SkipLine
  @@ChkNoQuote:	OR	DL,DL
		JE	@@ChkNumber
		JMP	@@SkipLine
  @@ChkQuote:	CMP	AL,'"'
		JNE	@@SkipLine
		INC	BP
  @@ChkNumber:	DEC	CX
		JNE	@@SkipLine
		MOV	CX,SI
  @@SizeEntry1:	CMP	SI,DS:DscrSize
		JAE	@@SizeEntry4
		LODSW
		CMP	SI,DS:DscrSize
		JA	@@SizeEntry2
		CMP	AX,0A0Dh
		JE	@@SizeEntry3
		CMP	AX,0D0Ah
		JE	@@SizeEntry3
  @@SizeEntry2:	DEC	SI
		CMP	AL,0Dh
		JE	@@SizeEntry3
		CMP	AL,0Ah
		JNE	@@SizeEntry1
  @@SizeEntry3:	CMP	SI,DS:DscrSize
		JAE	@@SizeEntry4
		MOV	AL,DS:[SI]
		CMP	AL,' '
		JE	@@SizeEntry1
		CMP	AL,9
		JE	@@SizeEntry1
		CMP	AL,'+'
		JE	@@SizeEntry1
		JMP	@@SizeEntry5
  @@SizeEntry4:	CMP	BYTE PTR DS:[SI-1],1Ah
		JNE	@@SizeEntry5
		DEC	SI
  @@SizeEntry5:	XCHG	CX,SI
		SUB	CX,SI
		MOV	AX,BP
		SUB	AX,SI
		STC
  @@Exit:	CMC
		RET
FindDescr1	ENDP

;---------------------------------------------------------------
;	Delete file from description
;		DS:SI - entry point
;		   CX - length
;---------------------------------------------------------------
DelDescription	PROC	FAR
		USES	CX,SI,DI,ES
		PUSH	DS
		POP	ES
		MOV	DI,SI
		ADD	SI,CX
		MOV	CX,DS:DscrSize
		SUB	CX,SI
		REP	MOVSB
		MOV	DS:DscrSize,DI
		RET
DelDescription	ENDP

;---------------------------------------------------------------
;	Save description
;		ES:DX - path
;		   DS - segment
;	Return:	   CY - error, AX - error code
;---------------------------------------------------------------
SaveDescription	PROC	FAR
		USES	BX,CX,DX,SI,DI
		LOCAL	DescrFileName1:BYTE:512
		LOCAL	DescrFileName2:BYTE:512
		PUSH	DS			; Set name
		PUSH	ES
		PUSH	DS:DscrName
		PUSH	ES
		POP	DS
		MOV	SI,DX
		CALL	CutPath
		MOV	CX,SI
		SUB	CX,DX
		MOV	SI,DX
		LEA	DI,DescrFileName1
		MOV	DX,DI
		PUSH	SS
		POP	ES
		REP	MOVSB
		POP	SI
  @@GetDescr:	LODSB
		CMP	AL,';'
		JE	@@EndList
		CMP	AL,','
		JE	@@EndList
		CMP	AL,0
		JE	@@EndList
		STOSB
		JMP	@@GetDescr
  @@EndList:	MOV	AL,0
		STOSB
		POP	ES
		POP	DS
		CMP	ES:GroupFiles,0		; Get real name
		JE	@@Exist
		CALL	ChgDisk
		MOV	AX,53h
		JC	@@Exit
		MOV	SI,DX
		LEA	DI,DescrFileName2
		PUSH	DS
		PUSH	ES
		PUSH	SS
		PUSH	SS
		POP	DS
		POP	ES
		CALL	TrueName
		POP	ES
		POP	DS
		JNC	@@ChkInBuf0
		CMP	AX,53h
		JE	@@Error
		JMP	@@Exist
  @@ChkInBuf0:	PUSH	DS			; Check file in buffer
		PUSH	ES
		MOV	DS,ES:GroupFiles
		PUSH	SS
		POP	ES
		CALL	ChkInBuffer
		POP	ES
		POP	DS
		JC	@@Exit
  @@Exist:	TEST	DS:DscrAttr,80h		; Check file on disk
		JNZ	@@ChkDescr
		PUSH	DS
		PUSH	SS
		POP	DS
		MOV	AX,4300h
		CALL	Intr21
		POP	DS
		JNC	@@ChkDescr
		CMP	AX,53h
		JE	@@Error
		JMP	@@Exit
  @@ChkDescr:	MOV	SI,OFFSET DscrBegin	; Empty description
  @@GetChar:	CMP	SI,DS:DscrSize
		JAE	@@Delete
		LODSB
		CMP	AL,' '
		JE	@@GetChar
		CMP	AL,9
		JE	@@GetChar
		CMP	AL,13
		JE	@@GetChar
		CMP	AL,10
		JE	@@GetChar
		JMP	@@Save
  @@Delete:	CMP	ES:DescrDel,0		; Delete empty description
		JE	@@Save
		PUSH	DS
		PUSH	SS
		POP	DS
		MOV	AH,41h
		CALL	Intr21
		POP	DS
		JNC	@@Exit
  @@Error1:	CMP	AX,53h
		JE	@@Error
  @@Beep:	CALL	Beep
  @@Error:	STC
		JMP	@@Exit
  @@Save:	MOV	CL,DS:DscrAttr		; Save description
		MOV	CH,0
		AND	CL,7Fh
		OR	CL,20h
		PUSH	DS
		PUSH	SS
		POP	DS
		MOV	AH,3Ch
		CALL	Intr21
		POP	DS
		JC	@@Error1
		MOV	BX,AX
		MOV	DX,OFFSET DscrBegin
		MOV	CX,DS:DscrSize
		SUB	CX,DX
		MOV	AH,40h
		CALL	Intr21
		PUSHF
		PUSH	AX
		MOV	AH,3Eh
		CALL	Intr21
		POP	SI
		POP	DI
		JC	@@Exit
		MOV	AX,SI
		PUSH	DI
		POPF
		JC	@@Exit
		CMP	AX,CX
		MOV	AL,0
		JNE	@@Beep
  @@Exit:	RET
SaveDescription	ENDP

;---------------------------------------------------------------
;	Save both descriptions and deallocate memory
;	Return:	   CY - if error
;---------------------------------------------------------------
SaveDescrs	PROC	FAR
		USES	AX,BX,CX,DX,SI,DI,DS
		MOV	AX,ES:GroupDescr1
		SUB	AX,ES:GroupDescr2
		JNE	@@SaveDescr1
		MOV	ES:GroupDescr2,AX
		XCHG	ES:GroupDescrChg2,AL
		OR	ES:GroupDescrChg1,AL
  @@SaveDescr1:	MOV	AL,0
		XCHG	AL,ES:GroupDescrChg1
		CMP	AL,0
		JZ	@@SaveDescr2
		MOV	DS,ES:GroupDescr1
		MOV	DX,OFFSET LineBuf1
		CALL	SaveDescription
		JNC	@@SaveDescr2
		CMP	AX,53h
		JE	@@Exit
  @@SaveDescr2:	MOV	AL,0
		XCHG	AL,ES:GroupDescrChg2
		CMP	AL,0
		JZ	@@FreeMem
		MOV	DS,ES:GroupDescr2
		MOV	DX,OFFSET LineBuf2
		CALL	SaveDescription
		JNC	@@FreeMem
		CMP	AX,53h
		JE	@@Exit
  @@FreeMem:	MOV	SI,OFFSET AllSegs_L
		XOR	BX,BX
		XCHG	BX,ES:GroupDescr1
		CALL	CompressBlock
		XOR	BX,BX
		XCHG	BX,ES:GroupDescr2
		CALL	CompressBlock
		STC
  @@Exit:	CMC
		RET
SaveDescrs	ENDP
