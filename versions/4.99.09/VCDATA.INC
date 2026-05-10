;---------------------------------------------------------------
;		Global variables
;
;		Version:	16.07.2000
;---------------------------------------------------------------

PrgName		EQU	'Volkov Commander'
VcVersion	EQU	'4.99.09 alpha'
HelpVer		EQU	'VC 4.99.09 Help'
VcID		EQU	'VVV009'

MoDelay		EQU	40		; Mouse double click delay
MiliSec		EQU	60		; auto CD time : sec x 0.01
Sec		EQU	0		;                sec x 1
LenStk		EQU	100h		; Stack size
LenPath		EQU	3+256+1+1	; Length of path name
LenCmdLine	EQU	126		; Length of command line
LenColr		EQU	42		; Length of color table
LenHist		EQU	512		; Length of hisory buffer
LenStr		EQU	64		; Visible length of path & file string
LenStrFull	EQU	512		; Full length of path & file string
LenFilt		EQU	25		; Length of custom filter
LenSearch	EQU	24		; Length of Alt-search line
LenBat		EQU	1000		; Length of full command line
LenHelp		EQU	3000		; Maximal length of help page
LenMask		EQU	32		; Length of mask
LenMaskFull	EQU	256		; Full length of mask
LenSwtch	EQU	20		; Length of COMMAND.COM switches
MinFls		EQU	256		; Minimum files in window
MinTree		EQU	512		; Minimum tree size
MinView		EQU	8000		; Minimum view buffer size
MaxProg		EQU	18		; Maximal resident quantaty
MaxFilts	EQU	10		; Maximal number of filters
NmbStrs		EQU	32		; Number of stars
ExpandSize	EQU	2000		; Expand size of segments
InpModePoint	EQU	7		; Max size of input line when alt edit
TblWidth	EQU	60		; Max length of table titles

KbdShiftMask	EQU	03h		; Right/left Shift status bits
KbdCtrlMask	EQU	04h
KbdAltMask	EQU	08h

;---------------------- Global structures ----------------------
; Far address structure definition.
FARAD		STRUC
  Offs		DW	?		; Offset
  Segm		DW	?		; Segment
FARAD		ENDS

; Long type structure definition.
LONG		STRUC
  Lo		DW	?		; Low word
  Hi		DW	?		; High word
LONG		ENDS

; Coordinates structure definition.
COORD		STRUC
  X		DB	?		; Column
  Y		DB	?		; Row
COORD		ENDS

; DTA structure definition.
DTAs		STRUC
		DB	15h DUP(?)
  FilAttr	DB	?
  FilTime	DW	?
  FilDate	DW	?
  FilSize	DD	?
  FilName	DB	13 DUP(?)
		DB	?
DTAs		ENDS

; Find Data Record structure definition.
FDR		STRUC
  WinAttr	DD	?		; Attributes
		DQ	?		; Creation time
		DQ	?		; Last access time
  WinTime	DQ	?		; Last modification time
		DD	?		; Size (high 32 bits)
  WinSize	DD	?		; Size (low 32 bits)
		DB	  8 DUP(?)	; Reserved
  WinLongName	DB	260 DUP(?)	; Full name
  WinName	DB	 14 DUP(?)	; Short name
FDR		ENDS

; Execute program block structure definition.
EPBs		STRUC
  EnvSeg	DW	?		; Segment of new environment
  CmdLine	DD	?		; Pointer to command line
  FCB1		DD	?		; Pointer to FCB1
  FCB2		DD	?		; Pointer to FCB2
EPBs		ENDS

; Country information structure definition.
CNTRY		STRUC
  DateFmt	DW	?		; Date format
		DB	5 DUP(?)	; Currency
  Sep1000	DB	2 DUP(?)	; Thousand separator
  DecSep	DB	2 DUP(?)	; Decimal separator
  DateSep	DB	2 DUP(?)	; Date separator
  TimeSep	DB	2 DUP(?)	; Time separator
		DB	?		; Currency format
		DB	?		; Number of meaning digits in currency
  TimeFmt	DB	?		; Time format
  CaseMap	DD	?		; Addres of case map
		DB	2 DUP(?)	; Data separator
		DB	8 DUP(?)	; Reserved
CNTRY		ENDS

; Extended memory move structure definition.
EMOV		STRUC
  LenBlk	DD	?		; 32-bit number of bytes to transfer
  SourceHandle	DW	?		; Handle of source block
  SourceOffset	DD	?		; 32-bit offset into source
  DestHandle	DW	?		; Handle of destination block
  DestOffset	DD	?		; 32-bit offset into destination block
EMOV		ENDS

; File entry in DiskBuf.
FILENT		STRUC
  AttrF		DB	?
  TimeF		DW	?
  DateF		DW	?
  SizeF		DD	?
  CompF		DD	?
  NumbF		DW	?
  DescrF	DW	?
  LenShortNamF	DW	?
  LenLongNamF	DW	?
  NameF		LABEL	BYTE
FILENT		ENDS

; Subdirectory entry of TREEINFO file.
DIRENT		STRUC
  DirLevel	DB	?		; Subdirectory level
  DirAttr	DB	?		; Attributes
  DirShortLen	DB	?		; Length of short subdirectory name
  DirWinLen	DW	?		; Length of Win95 subdirectory name
  DirOS2Len	DW	?		; Length of OS/2 subdirectory name
  DirName	LABEL	BYTE		; Subdirectory name in ASCIIZ
DIRENT		ENDS

; Structure of TREEINFO file.
DIRTREE		STRUC
  TreeHead	DB	9  DUP(?)	; Tree file header 'VC TREE'
  TreeLabel	DB	11 DUP(?)	; Volume label
  TreeCrLf	DB	2  DUP(?)	; CrLf
  TreeSerNum	DB	4  DUP(?)	; Serial number
  TreeList	LABEL	BYTE		; List of subdirectories
		DW	?		; Check sum of the list
DIRTREE		ENDS

; Structure of description segment.
DESCRSEG	STRUC
  DscrSegSize	DW	?		; Segment size
  DscrSize	DW	?		; File size
  DscrName	DW	?		; Name offset
  DscrAttr	DB	?		; Attributes
  DscrBegin	LABEL	BYTE		; Begin of file
DESCRSEG	ENDS

; The dialog box header structure definition.
BOXHD		STRUC
  pDiBox	DW	?		; Pointer to DialBox structure
  BoxBase	DW	?		; The dialog box begin-center
  SubFunc	DW	?		; Subfunction
  bKeyBar	DB	?		; KeyBar
  bMenu		DB	0FFh		; PutMnu
  bHelp		DB	0		; HlpPage
  bFlags	DB	0		; Flags
BOXHD	ENDS

; The dialog box header structure definition.
BOXMN		STRUC
  mType		DB	?		; Menu type
  mUp		DB	0		; Up link
  mDown		DB	0		; Down link
  mLeft		DB	0		; Left link
  mRight	DB	0		; Right link
  mBase		DW	?		; Menu base
  mPoint	DW	?
  mSelTot	LABEL	BYTE
  mCtrl		DW	?
BOXMN		ENDS

; The star structure definition.
STAR		STRUC
  StarClk	LABEL	BYTE		; Time counter
  StarMod	DW	?		; Low byte - counter, High - explosion
  StarPos	DW	?		; Coordinates of star
STAR		ENDS

; Search data structure definition.
SrData		STRUC
  SrLenText	DW	?			; Length of text pattern
  SrLenTextFull	DW	?			; Length of text pattern
  SrLenHex	DW	?			; Length of hex pattern
  SrTransProc	DW	?			; Search translation procedure
  SrTblDecode	DD	?			; Decode table
  SrText	DB	LenStrFull+1 DUP(?)	; Search string
  SrHex		DB	LenStrFull/2+2 DUP(?)	; Search hex bytes
SrData		ENDS

; Translation table structure definition.
TblData		STRUC
  Decode	DB	256 DUP (?)	; Decoding table
  Encode	DB	256 DUP (?)	; Encoding table
  HiDBCS	DB	?		; High byte of DBCS
  TblNumb	DB	?		; Table number
  Translate	DW	?		; Translation procedure
TblData		ENDS

; ZIP file header structure definition.
ZIPFILEHD	STRUC
  ZipSignFile	DB	4 DUP(?)	; central file header signature
  ZipOS		DW	?		; version made by
		DW	?		; version needed to extract
		DW	?		; general purpose bit flag
		DW	?		; compression method
  ZipTime	DW	?		; last mod file time
  ZipDate	DW	?		; last mod file date
		DD	?		; crc-32
  ZipComp	DD	?		; compressed size
  ZipSize	DD	?		; uncompressed size
  ZipNameLen	DW	?		; filename length
  ZipExtraLen	DW	?		; extra field length
  ZipCommLen	DW	?		; file comment length
		DW	?		; disk number start
		DW	?		; internal file attributes
  ZipAttr	DD	?		; external file attributes
		DD	?		; relative offset of local header
  ZipName	LABEL	BYTE		; filename (variable size)
					; extra field (variable size)
					; file comment (variable size)
ZIPFILEHD	ENDS

; End of ZIP central dir record structure definition.
ZIPENDHD	STRUC
  ZipEndSign	DB	4 DUP(?)	; end of central dir signature
  ZipLastDsk	DW	?		; number of this disk
  ZipStartDsk	DW	?		; number of the disk with the
					; start of the central directory
  ZipFlsDsk	DW	?		; total number of entries in
					; the central dir on this disk
  ZipFlsTot	DW	?		; total number of entries in
					; the central dir
  ZipDirSize	DD	?		; size of the central directory
  ZipDirOffset	DD	?		; offset of start of central
					; directory with respect to
					; the starting disk number
		DW	?		; zipfile comment length
ZIPENDHD	ENDS

;----------------------- WCB definition ------------------------
;		      0 +-------+
;			|	| Vars saved in VC.INI
;		 WCBini +-------+
;			|	| Vars saved in memory when run command
;		WCBsave +-------+
;			|	| Other vars
;	WCBdef=ArcFiles +-------+
;			|	| Archive files
;		DiskBuf +-------+
;			|	| Visible files
;		 EndBuf +-------+
;			|	| Hidden files
;		 EndFul +-------+
;			|	| Local VC.EXT
;			+-------+
;			|	| Description
;		SizeWCB +-------+
;
WCBdef		STRUC
WinTyp		DB	?			; File/Tree/Info/View panel
BrfFul		DB	?			; Brief/Full/Description panel
LongNames	DB	?			; Long names
NameWidth	DB	?			; Long names width
Visible		DB	?			; Panel is off/on
FiltAttr	DB	?			; Filter attributes
FiltTyp		DB	?			; Filter number
SortTyp		DB	?			; Sort order
SortRev		DB	?			; Reverced sort
ShowSize	DB	?			; Show size in full panel
ShowRatio	DB	?			; Show ratio in full panel
ShowDate	DB	?			; Show date in full panel
ShowTime	DB	?			; Show time in full panel
Ctrl_L		DB	?			; Previous panel type
DskMas		DB	LenFilt+2 DUP(?)	; Custom filter
WinShortPath	DB	LenPath DUP(?)		; Short path of panel
WCBini		=	$			; End of WCB part saved in VC.INI

DiskBuf		DW	?			; Start address of disk buffer
FileCur		DW	?			; Pointer position
FileFirst	DW	?			; First file number
EndBuf		DW	?			; End of file buffer
ArcPnt		DW	?			; Archive pointer
SerNumb		DD	?			; Serial number
SerNumErr	DB	?			; Error reading serial number
ArcPath		DB	LenPath DUP(?)		; Path in archive
CrnPath		DB	LenPath DUP(?)		; Current directory in the tree panel
WCBsave		=	$			; End of WCB part saved in memory

LongNamFlag	DB	?			; Long names were loadad
EndFul		DW	?			; End of file buffer including all files
ExtFile		DW	?			; Address of extension file
SizeWCB		DW	?			; Size of WCB segment
TreeCur		DW	?			; Pointer in tree panel
TreeFirst	DW	?			; First line in tree panel
TreeCrnt	DW	?			; Current path enrty in tree
TreeOffset	DB	?			; Offset in tree panel
TreeTyp		DB	?			; Offset in tree levels
ChgFlg		DB	?			; Flag to force reread the panel
WinLongPath	DB	LenPath DUP(?)		; Long path of panel
ArcExt		DB	4 DUP(?)		; Type of archive file
ArcFiles	LABEL	BYTE
WCBdef		ENDS

;------------------------- Data segment ------------------------
MainDat		STRUC
ColTab		DB	6*LenColr DUP(?)	; Color table (6 pallets)
FulScr		DB	?			; Full screen
PanelSize	DB	?			; Size of panels in non full screen mode
MenuVs		DB	?			; Menu always visible
KeyBar		DB	?			; Key bar
Prompt		DB	?			; Path prompt
InsDwn		DB	?			; Ins moves down
InsDirSize	DB	?			; Ins calculates directory size
AutoMnu		DB	?			; Auto user menus
MiniSta		DB	?			; Mini status
Clock		DB	?			; Clock
AutoCD		DB	?			; Auto change dir
AutoSav		DB	?			; Auto save setup
ColTyp		DB	?			; Monochrome / Color / LapTop
BlDelay		DB	?			; Screen blank delay
LoadFl		DB	?			; Small mode
UseSwap		DB	?			; Enable swap file
UseEMS		DB	?			; Enable EMS
UseXMS		DB	?			; Enable XMS
UnSlct		DB	?			; Unselect files when group operation
ErrSnd		DB	?			; Beep when error
LftMous		DB	?			; Left handed mouse
MousRes		DB	?			; Fast mouse reset
ColorDirs	DB	?			; Show dirs by different color
ZoomWin		DB	?			; Zooming windows
ZoomStages	DB	?			; Number of stages of zooming windows
AltBar		DB	?			; Alt alone selects menu
EscBar		DB	?			; Esc toggles panels on/off
ExecTyp		DB	?			; Exec command without COMMAND.COM
AutoSiz		DB	?			; Auto calculate directory sizes
HidDirs		DB	?			; Show hidden subdirs in tree panel
QuitAck		DB	?			; Confirmation on Quit
SnowMod		DB	?			; Snow mode (2=auto)
AltView		DB	?			; Alt-F3 - external viewer
AltEdit		DB	?			; Alt-F4 - external editor
ClrBuf		DB	?			; Clear buffer flag
Active		DB	?			; Active panel
Ctrl_O		DB	?			; Visible status of unactive panel
ChkTSR		DB	?			; TSR manager
IncDirs		DB	?			; Include subdirectories
DescrProcess	DB	?			; Process description
DescrCreate	DB	?			; Create target description file if not exists
DescrEmpty	DB	?			; Delete target description if source empty
DescrDel	DB	?			; Delete empty description file
ChkTarget	DB	?			; Check target when copy
ClearRO		DB	?			; Clear read-only attribute when copy from CD-ROM
CopyEAs		DB	?			; Copy OS/2 extended attributes
ConvCase	DB	?			; Lowercase short names
DriveB		DB	?			; Show logic drive B:
TreeInRoot	DB	?			; Save TREEINFO in root
TreeInHome	DB	?			; Save TREEINFO in home
SrchCase	DB	?			; Case sensitive search
NumbSort	DB	?			; Numeric file sort
DelayTm		DB	?, ?			; Auto change dir delay time
StarTab		DB	 5 DUP (?)		; Star table
Borders		DB	15 DUP (?)		; Border characters
MenuBords	DB	 9 DUP (?)		; Menu border characters
Scrolls		DB	 2 DUP (?)		; Scroll characters
ScrollBars	DB	 4 DUP (?)		; Scroll bar characters
ProgressBars	DB	 2 DUP (?)		; Progress bar characters
Pointers	DB	 7 DUP (?)		; Mouse pointers characters
FileMarks	DB	 3 DUP (?)		; File mark characters
ItemOn		DB	?			; Item switched on
LenCmdLn	DW	?			; Length of command lines
LenHistory	DW	?			; Lenght of history buffer
MinFiles	DW	?			; Reserved memory for files
MaxTSRs		DB	?			; Maximal numbers of TSR
DefBlock	=	$
Switches	DB	LenSwtch+2 DUP(?)	; COMMAND.COM switches
DescrFiles	DB	LenStr+2 DUP(?)		; Description files
DirInfoFiles	DB	LenStr+2 DUP(?)		; Directory information files
CmnIni		=	$			; End of main data' part saved in VC.INI

ScrMod		DB	?			; Current screen mode
Columns		DB	?			; Screen columns
Lines		DB	?			; Number of lines on screen
CharSize	DB	?			; Scan lines per char
Mouse		DB	?			; 1 if Mouse was found
MenuFl		DB	?			; 1 to bring user menu automatically
SwapWin		DB	?			; Swap WCB1 and WCB2
TransTblNumb	DB	?			; Translation table number
RestoreMode	DW	?			; Restore video mode
MouseX		DW	?			; Mouse cursor X
MouseY		DW	?			; Mouse cursor Y
LastMnu		DW	?			; The last pull down menu item
FreeMem		DW	?			; Free memory in small mode
ArchPoint	DW	?			; Pointer to main extension file
FiltsPoint	DW	?			; Pointer to filters
HistPoint	DW	?			; Pointer to History
FilesPoint	DW	?			; Pointer to files buffer
CmdLnPoint	DW	?			; Pointer to command line
BytesInSwap	DW	?			; Bytes in swap file
NormalSize	DW	?			; Normal size of data in EMS/XMS
List1numb	DW	?			; File list 1 number
List2numb	DW	?			; File list 2 number
TempDirNumb	DW	?			; Temporary directory number
ReleaseData	DW	?			; Release date pointer
SelMas		DB	LenMaskFull+2 DUP(?)	; Wildcard for selection
SourceFile	DB	LenStrFull+2 DUP(?)	; Source filename
TargetFile	DB	LenStrFull+2 DUP(?)	; Target filename
FindStr		DB	LenStrFull+2 DUP(?)	; FileFind wildcard
SrchStr		DB	LenStrFull+2 DUP(?)	; String for search
GotoOffset	DB	8+2 DUP(?)		; Go to offset
IniFile		DB	LenPath+12 DUP(?)	; Pathname of ini file
TempDir		DB	LenPath+12 DUP(?)	; Path from TEMP= variable
CmnSave	=	$				; End of main data part saved in memory

DosLin		DB	?			; Line number of DOS's command line
DosCol		DB	?			; Current column of DOS's command line
BegCol		DB	?			; Start position of DOS's command line
OutCol		DB	?			; Start position for output com. line
ScrPage		DB	?			; Current screen page number
TstEGA		DB	?			; 1 if EGA or VGA present
ModeFlag	DB	?			; Error setting video mode
ColorMono	DB	?			; Color/Monochrome mode
Snow		DB	?			; Snow on/off
ExtKeyb		DB	?			; Extended keyboard
StdIn		DB	?			; Input through INT 21h
StdFile		DB	?			; Input from file
TopView		DB	?			; TopView output
DesqVie		DB	?			; Running under DESQVIEW
OS2Box		DB	?			; Running under OS/2
WinLeave	DB	?			; Leave time under Windows
CrntCol		DB	?			; Color which DOS currently uses
Quote		DB	?			; Quote next character
BarPrev		DB	?			; Previous Key Bar
DosChg		DB	?			; Current drive when enter
SftErr		DB	?			; Shift error boxes down
BlocEr		DB	?			; Disable critical error messages
PathEr		DB	?			; Error when reading current path
RescanTree	DB	?			; Rescan tree
MoveCurFlag	DB	?			; Cursor moved (Auto CD, Quick View)
MouseWin	DB	?			; Mouse in some window
MouseEnter	DB	?			; Mouse double click
MouseRep	DB	?			; Mouse autorepeat command
HlpPage		DB	?			; Current page of help
OneFile		DB	?			; Do one file and exit
ErrDat		DB	?			; Don't pop up error box
ProcLongNams	DB	?			; Process long names in subdirs
PanelSize1	DB	?			; Temporary panel size
DescrUpdate	DB	?			; Update description
ZoomOne		DB	?			; Zoom group flag
MinMnuFlag	DB	?			; MinMnu Home and End process
MinMnuDefault	DB	?			; MinMnu default position
WinClose	DB	?			; Enable WinClose
VerDOS		DW	?			; Version of DOS
WCBcrn		DW	?			; Active panel pointer
WCBsec		DW	?			; Non active panel pointer
WCB1seg		DW	?			; Left panel control block
WCB2seg		DW	?			; Right panel control block
TreeSeg		DW	?			; Treeinfo block
DirinfoSeg	DW	?			; Dirinfo block
ViewSeg		DW	?			; Quick view block
SizeDataSeg	DW	?			; Length of data segment
ScreenBuf	DW	?			; Output screen buffer segment
UserScreen	DW	?			; User screen buffer segment
WindowsPoint	DW	?			; Pointer to save windows area
WorkBuffer	DW	?			; Buffer for copy, view, edit...
Memorys		DW	?			; Total memory
HstAdr		DW	?			; History pointer
DataLocation	DB	?			; Locatoin: RAM/Swap/EMS/XMS
DatPoint	DW	?			; Pointer to data save buffer
DatHandle	DW	?			; Data handle
ClusterSize	DD	?			; Size of cluster
TotalClusters	DD	?			; Total clusters on disk
FreeClusters	DD	?			; Free clusters on disk
Rand1		DW	?			; Random number 1
Rand2		DW	?			; Random number 2
ArcCmdPoint	DW	?			; Pointer to command in VCARCH.EXT
ArcPathName	DW	?			; Pointer to pathname of archive
ArcPathIn	DW	?			; Pointer to path in archive
FncAddr		DW	?			; Pointer to group function
FindHandle	DW	?			; DOS 7.0 find handle
DirInfoNumb	DW	?			; Number of dir of dirinfo
KeyStroke	DW	?			; Key stroke
MousePos	DW	?			; Last mouse position
MouseTime	DD	?			; Last mouse press time
MouseCmd	DW	?			; Last mouse command
TaskStack	DW	?			; Task switsher stack
TaskAddr	DD	?			; Task switsher return address
AutoTim		DD	?			; Auto press key time
Screen		DD	?			; Address of screen
ErrVct		DD	?			; Original vector 24h
BrkVct		DD	?			; Original vector 1Bh
QuickViewComp	DB	?			; Compressed size supported
QuickViewFiles	DD	?			; Quick View files
QuickViewDirs	DD	?			; Quick View directories
QuickViewFlSize	DQ	?			; Quick View files size
QuickViewClSize	DQ	?			; Quick View cluster size
UserInLine	DW	2*10 DUP(?)		; User menu in line pointers buffer
Volume		DB	13 DUP(?)		; Volume label
MouseCur	DB	128 DUP(?)		; List of mouse cursor types
GroupMask	DB	LenStr+1 DUP(?)		; Mask for group operations
DosPath		DB	LenPath DUP(?)		; Current path
DosShortPath	DB	LenPath DUP(?)		; Current short path
VcDir		DB	LenPath DUP(?)		; Path of VC directory
VcSet		DB	LenPath DUP(?)		; Path from VC= variable
Country		CNTRY	?			; Country information
DTA		DTAs	?			; Data tramsnir area
FindData	FDR	?			; Find data record
OS2AttrBuf	DB	22 DUP(?)		; Buffer for OS/2 file attributes
DosBuf		DB	LenCmdLine+1 DUP(?)	; Command line
LowTab		DB	128 DUP(?)		; Lower case table
DirInfoFile	DB	514 DUP(?)		; DirInfo filename
TmpBuf		DB	514 DUP(?)		; Temporary buffer
LineBuf1	DB	514 DUP(?)		; Line buffer 1
LineBuf2	DB	514 DUP(?)		; Line buffer 2
LineBuf3	DB	514 DUP(?)		; Line buffer 3
LineBuf4	DB	514 DUP(?)		; Line buffer 4
ArchiveExt	DB	4 DUP(?)		; Archive extension
ExecArcArcPath	DB	LenPath DUP(?)		; Path in archive for ExecArc
IniBuffer	DB	3+2*WCBini+CmnIni+2 DUP(?)
TmpErr		DB	256 DUP(?)		; Temporary buffer for errors
Stars		STAR	NmbStrs DUP(?)		; Stars buffer
HelpBuf		DB	LenHelp	DUP(?)		; Help buffer
MainExt		LABEL	BYTE			; Buffer for the control files
MainDat		ENDS

; Saved data structure definition.
Dat		STRUC
  WCB1sav	DB	WCBsave DUP(?)
  WCB2sav	DB	WCBsave DUP(?)
  ComnIni	DB	CmnSave DUP(?)
Dat		ENDS

Back_C	EQU	0
Dirs_C	EQU	1
Cursr_C	EQU	2
Slct_C	EQU	3
CurSl_C	EQU	4
Title_C	EQU	5
KeyNu_C	EQU	6
KeyTi_C	EQU	7
Menu_C	EQU	8
MnCur_C	EQU	9
MnuBx_C	EQU	10
MnuTx_C	EQU	11
MnuBr_C	EQU	12
MnuCu_C	EQU	13
MnuCB_C	EQU	14
MnuDr_C	EQU	15
MnuMi_C	EQU	16
Box_C	EQU	17
BoxBr_C	EQU	18
BoxCu_C	EQU	19
Conf_C	EQU	20
CnfRe_C	EQU	21
CnfBo_C	EQU	22
CnfUn_C	EQU	23
Hist_C	EQU	24
HstCu_C	EQU	25
HstBr_C	EQU	26
HlBrd_C	EQU	27
HlpMn_C	EQU	28
Help_C	EQU	29
HlpBo_C	EQU	30
HlpRe_C	EQU	31
HlpUn_C	EQU	32
Err_C	EQU	33
ErInv_C	EQU	34
ErBr_C	EQU	35
Clock_C	EQU	36
Arrow_C	EQU	37
Sky1_C	EQU	38
Sky2_C	EQU	39
ShadFl	EQU	40

None_H		EQU	255
Default_H	EQU	 0
Index_H		EQU	 0
Main_H		EQU	 1
Keys_H		EQU	 2
ViewKeys_H	EQU	 3
ViewStat_H	EQU	 4
EditKeys_H	EQU	 5
EditStat_H	EQU	 6
LeftRight_H	EQU	 7
BriefFull_H	EQU	 8
Info_H		EQU	 9
Tree_H		EQU	10
QuickView_H	EQU	11
OnOff_H		EQU	12
LongNames_H	EQU	13
Sort_H		EQU	14
ReRead_H	EQU	15
Filter_H	EQU	16
Drive_H		EQU	17
Files_H		EQU	18
Menu_H		EQU	19
View_H		EQU	20
Edit_H		EQU	21
Copy_H		EQU	22
Move_H		EQU	23
MkDir_H		EQU	24
Delete_H	EQU	25
Attrib_H	EQU	26
Select_H	EQU	27
Quit_H		EQU	28
Commands_H	EQU	29
ChgTree_H	EQU	30
Label_H		EQU	31
Memory_H	EQU	32
DirSize_H	EQU	33
FileFind_H	EQU	34
History_H	EQU	35
EgaLines_H	EQU	36
Mode_H		EQU	37
SwapPan_H	EQU	38
OnOffPan_H	EQU	39
CompDirs_H	EQU	40
EditMenu_H	EQU	41
Options_H	EQU	42
General_H	EQU	43
Interface_H	EQU	44
Chars_H		EQU	45
PanelOpt_H	EQU	46
Descript_H	EQU	47
ScrSavOpt_H	EQU	48
CopyOpt_H	EQU	49
EditExt_H	EQU	50
EditView_H	EQU	51
EditEdit_H	EQU	52
AutoMenu_H	EQU	53
KeyBar_H	EQU	54
FullScr_H	EQU	55
Status_H	EQU	56
Alloc_H		EQU	57
SaveSet_H	EQU	58

Version_E	EQU	1
Alloc_E		EQU	2
OvlDisk_E	EQU	3
InvalidKey_E	EQU	4
KeySequence_E	EQU	5
InitSwap_E	EQU	6
LoadSwap_E	EQU	7
InitEMS_E	EQU	8
InitXMS_E	EQU	9
LoadEMS_E	EQU	10

