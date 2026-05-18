;---------------------------------------------------------------
;		Make directory
;
;		Version:	14.03.1998
;---------------------------------------------------------------

		.CONST
MkDir0_S	DB	"Make directory",0
MkDir1_S	DB	"Create the directory",0FFh,LenStr-20,0
MkDirError_S	DB	"Can't create the directory",0

		.DATA
MkDir_T		DW	MkDir0_S, LineBuf1
MkDir_P		DB	Box_C, BoxCu_C, 0, 0
		DW	IncSp_S
		DW	MkDir1_S
		DW	Empty_S
		DW	0
MkDir_B		BOXHD	< MkDir_P, 500h, 0, Dialog_K, Fls_M, MkDir_H, 0 >
		BOXMN	< 1, 0,0,0,0, 305h, LineBuf1, LenStrFull+LenStr >
		DB	0
MkDirError_P	DB	Err_C, ErInv_C, ErInv_C, 0
		DW	IncSp_S
		DW	MkDirError_S
		DW	Line1_S
		DW	Ok_S
		DW	0
MkDirError_B	BOXHD	< MkDirError_P, 900h, 0, Dialog_K, Def_M, Default_H, 6 >
		BOXMN	< 4, 0,0,0,0, 4FEh, OkMn_D, 0 >
		DB	0

		.CODE

;---------------------------------------------------------------
;
; F7 - make directory (path appended by the active panel path if not absolute)
;
;---------------------------------------------------------------
F37_00		PROC	NEAR
		CMP	DS:Visible,0		; no active panel, cannot build
		JZ	F47_00			; the full path from it
		MOV	CL,1			; f_mkdir_path = TRYCONSTRUCT
		CMP	DS:WinTyp,p_Tree	; Info or QuickView panel, cannot build
		JA	F47_00			; the full path from the active panel
		JE	MkDir_dialog_impl
		CMP	DS:ArcPnt,0		; File panel, but in the archive
		JNE	Func04			; cannot create directory there
F37_00		ENDP

;---------------------------------------------------------------
;
; Shift F7 - make directory (raw path), default when panels are off
;
;---------------------------------------------------------------
;
;
; // helper function, [[noreturn]]
; Error() {
;   if (errno == CRITERR_FAIL) goto Func04
;   Dialog(MkDir_T, MkDirError_B)
;   if (f_mkdir_success) goto @@UpdateTreePanelPath
;   else goto Func04
; }
;
; LineBuf1[0] = '\0'
; if (Dialog(MkDir_T, MkDir_B)) goto Func04 // failure exit without Error Dialog
; if (LineBuf1[0] == '\0') goto Func04 // empty directory path
;
; if (f_mkdir_path != RAW &&
;      && LineBuf1[0] != '\\' && LineBuf1[0] != '/' && LineBuf1[0] != ':') {
;     f_mkdir_path = CONSTRUCTED
;     sprintf(LineBuf3, "%s/%s", CrnPath, LineBuf1)
; } else {
;     strcpy(LineBuf3, LineBuf1)
; }
;
; f_mkdir_success = false
; if (NormPath(&LineBuf2, LineBuf3) == -1) Error()
; if (ChgDisk() == -1) goto Func04 // deal with single floppy PC
;
; cnt_mkdir_recursv = 0
; p = CutPath(LineBuf2) - &LineBuf2
; // last path's character is '/'
; if (LineBuf2[p] == '\0' && p > 0 && LineBuf2[p - 1] != ':') LineBuf2[p - 1] = '\0'
;
; // after failure try to create every directory in the path from the start
; while (mkdir(LineBuf2) == -1) {
;     if (errno == CRITERR_FAIL) goto Func04
;     p = CutPath(LineBuf2) - &LineBuf2
;     // cannot create any directory in the given path or path is rootdir
;     if (p <= 1 || LineBuf2[p - 1] == ':') Error()
;     cnt_mkdir_recursv++
;     LineBuf2[p - 1] = '\0'
; }
;
; // after first successfully created directory any failure is fatal
; for (; cnt_mkdir_recursv > 0; cnt_mkdir_recursv--) {
;      f_mkdir_success = true // can be partial
;      p = strlen(LineBuf2)
;      LineBuf2[p] = '\\' // restore path separator
;      if (mkdir(LineBuf2) == -1 ) {
;          LineBuf2[p] = '\0' // make asciiz path for Error()
;          Error()
;      }
; }
;
; if (PrepAddTree(LineBuf4/*shortname*/, LineBuf3/*longname*/, LineBuf2/*src*/) == -1)
;     goto FuncExit2
; if (f_mkdir_path == CONSTRUCTED) { // Tree panel
;     strncpy(MK_FP(WCBcrn, CrnPath), LineBuf4/*shortname*/, LenPath - 1)
; } else { // File panel
;     ActvPanPath_len = strlen(DosShortPath) - 1
;     if (!memcmp(LineBuf4/*shortname*/, DosShortPath, ActvPanPath_len)) {
;         // rootdir or path match active panel's path
;         if (ActvPanPath_len == 3 || LineBuf4[ActvPanPath_len] == '\\') {
;             ActvPanBuf_size = (FindNextSeg(GlobalSegs_L, WCBcrn + 1/*pActvPan*/) - &WCBcrn) * 16
;             DirName_len = strchr(LineBuf4[ActvPanPath_len + 1]/*shortname*/, '\\')
;                           - &LineBuf4[ActvPanPath_len]
;             // EndBuf is offset (see VCDATA.INC diagram), but here it used as
;             // panel's DiskBuf current size
;             if (sizeof(FilEnt) + DirName_len <= ActvPanBuf_size - EndBuf) { // have space
;                 FileCur = Addr2Numb(MK_FP(WCBcrn, EndBuf))
;                 FilEnt[EndBuf].AttrF = FA_DIR
;                 FilEnt[EndBuf].LenShortNamF = DirName_len
;                 FilEnt[EndBuf].LenLongNamF = 0
;                 strlcpy(FilEnt[EndBuf].NameF,
;                         LineBuf4[ActvPanPath_len + 1]/*short dirname*/, DirName_len)
;                 EndBuf += sizeof(FilEnt) + DirName_len // FilEnt is variadic
;             }
;         }
;     }
; }
;
; if ((pWrkBuf = GetTreeInfo(LineBuf4, -1)) != -1) {
;     FindTreeEntry(pWrkBuf, LineBuf4)
;     if (!AddTreeEntry(LineBuf4/*shortname*/, LineBuf3/*longname*/))
;         SaveTreeInfo(LineBuf4/*shortname*/)
; }
;
F47_00		PROC	NEAR			; Shift F7 - make directory (raw path)
		MOV	CL,0                    ; f_mkdir_path = RAW

MkDir_dialog_impl:
		PUSH	ES
		POP	DS
		MOV	SI,OFFSET LineBuf1
		MOV	WORD PTR [SI],0
		MOV	BP,OFFSET MkDir_B
		MOV	DX,OFFSET MkDir_T
		CALL	Dialog
		JNE	Func04			; Make Directory dialog canceled
		CMP	BYTE PTR [SI],0
		JE	Func04			; empty directory path
		MOV	DI,OFFSET LineBuf3	; construct path if LineBuf1 is a relative path
		PUSH	DI
		OR	CL,CL
		JZ	@@CanonicalizePath	; f_mkdir_path == RAW
		MOV	AX,[SI]
		CMP	AL,'\'
		JE	@@CanonicalizePath
		CMP	AL,'/'
		JE	@@CanonicalizePath
		CMP	AH,':'
		JE	@@CanonicalizePath
		INC	CX			; f_mkdir_path = CONSTRUCTED
		PUSH	DS
		PUSH	SI
		MOV	DS,DS:WCBcrn
		MOV	SI,OFFSET CrnPath
		CALL	MvzLine
		POP	SI
		POP	DS
		CALL	AddFile

  @@CanonicalizePath:
		CALL	MvzLine
		POP	SI
		MOV	DI,OFFSET LineBuf2
		MOV	DX,DI
		MOV	CH,0			; f_mkdir_success = false
		CALL	NormPath
		JC	@@Error
		CALL	ChgDisk			; deal with single floppy PC
		JC	Func04
		XOR	BX,BX			; cnt_mkdir_recursv = 0
		MOV	SI,DX
		CALL	CutPath
		CMP	BYTE PTR [SI],0
		JNE	@@MkDir1
		DEC	SI
		CMP	SI,DX
		JE	@@MkDir1
		CMP	BYTE PTR [SI],':'	; last path's character is '/'
		JE	@@MkDir1

  @@MkDirInPath_Next:
		MOV	BYTE PTR [SI],0

  @@MkDir1:	MOV	AH,39h
		CALL	Intr21			; mkdir()
		JNC	@@MkDir2
		CMP	AX,53h
		JE	Func04			; Critical Error(int 24h)

  @@DirAlreadyExist:
		MOV	SI,DX
		CALL	CutPath
		DEC	SI
		CMP	SI,DX
		JBE	@@DispErrorDialog	; cannot create any directory in the given path
		CMP	BYTE PTR [SI],':'
		JE	@@DispErrorDialog	; path is 'X:' - cannot create rootdir
		INC	BX                      ; cnt_mkdir_recursv++
		JMP	@@MkDirInPath_Next	; try mkdir() next path component

  @@MkDir2:
		MOV	CH,1			; f_mkdir_success = true (can be partial)
		OR	BX,BX
		JE	@@UpdateTreePanelPath
		DEC	BX			; cnt_mkdir_recursv--
		PUSH	CX
		MOV	DI,DX
		MOV	CH,0FFh
		MOV	AL,0
		REPNE	SCASB
		MOV	BYTE PTR [DI-1],'\'	; restore path separator (see @@MkDirInPath_Next)
		POP	CX
		MOV	AH,39h
		CALL	Intr21			; mkdir()
		JNC	@@MkDir2
		MOV	BYTE PTR [DI-1],0	; make asciiz path for Error Dialog

  @@Error:	CMP	AX,53h
		JE	Func04                  ; Critical Error(int 24h)

  @@DispErrorDialog:
		PUSH	DX
		MOV	DX,OFFSET MkDir_T
		MOV	BP,OFFSET MkDirError_B
		CALL	Dialog
		POP	DX
		OR	CH,CH
		JZ	Func04			; !f_mkdir_success

  @@UpdateTreePanelPath:
		MOV	SI,DX
		MOV	DI,OFFSET LineBuf4
		MOV	BX,OFFSET LineBuf3
		CALL	PrepAddTree
		JC	FuncExit2
		MOV	SI,DI
		CMP	CL,2
		JNE	@@UpdateFilePanelPath	; skip - f_mkdir_path != CONSTRUCTED
		MOV	ES,DS:WCBcrn
		MOV	DI,OFFSET CrnPath
		MOV	CX,LenPath-1
		REP	MOVSB
		MOV	AL,0
		STOSB
		JMP	@@UpdateTreeInfo

  @@UpdateFilePanelPath:
		MOV	DI,OFFSET DosShortPath
		CALL	StrLen
		DEC	CX
		MOV	AX,CX
		REPE	CMPSB
		JNE	@@UpdateTreeInfo
		CMP	AX,3
		JE	@@GetActivePanelBuf_Size ; rootdir
		LODSB
		CMP	AL,'\'
		JNE	@@UpdateTreeInfo	; path doesn't match active panel's path

  @@GetActivePanelBuf_Size:
		MOV	AX,ES:WCBcrn
		MOV	BX,AX
		INC	BX
		PUSH	SI
		MOV	SI,OFFSET GlobalSegs_L
		CALL	FindNextSeg
		POP	SI
		SUB	DX,AX
		MOV	CL,4
		SHL	DX,CL
		PUSH	AX			; WCBcrn - Active panel pointer
		MOV	DI,SI			; LineBuf4[ActvPanPath_len + 1]/*short dirname*/

  @@GetDirnameLen:
		LODSB
		CMP	AL,0
		JE	@@AddDirname2Panel
		CMP	AL,'\'
		JNE	@@GetDirnameLen

  @@AddDirname2Panel:
		MOV	CX,SI
		SUB	CX,DI
		POP	ES			; WCBcrn - Active panel pointer
		SUB	DX,ES:EndBuf
		MOV	AX,CX
		ADD	AX,SIZE FilEnt
		CMP	AX,DX
		JA	@@UpdateTreeInfo	; skip - too many files in the panel
		MOV	SI,ES:EndBuf
		PUSH	DS
		PUSH	SI
		PUSH	ES
		POP	DS
		CALL	Addr2Numb
		MOV	DS:FileCur,SI
		POP	SI
		POP	DS
		XCHG	SI,DI
		MOV	ES:[DI].AttrF,10h	; FA_DIR
		MOV	ES:[DI].LenShortNamF,CX
		MOV	ES:[DI].LenLongNamF,0
		ADD	DI,OFFSET NameF
		DEC	CX
		REP	MOVSB
		MOV	AL,0
		STOSB
		MOV	ES:EndBuf,DI

  @@UpdateTreeInfo:
		PUSH	DS
		POP	ES
		MOV	DX,OFFSET LineBuf4
		MOV	AX,0FFFFh
		CALL	GetTreeInfo
		JC	@@Exit
		CALL	FindTreeEntry
		MOV	BX,OFFSET LineBuf3
		CALL	AddTreeEntry
		JNC	@@Exit
		CALL	SaveTreeInfo

  @@Exit:	JMP	FuncExit2
F47_00		ENDP
