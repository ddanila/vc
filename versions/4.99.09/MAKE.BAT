@ TASMX /z/m9/kh10000 %1;
@ IF errorlevel==1 GOTO End
@ IF "%1"=="vcovl" GOTO Ovl
@ LINK %1;
@ IF errorlevel==1 GOTO End
@ DEL %1.OBJ
@ EXE2BIN %1.EXE %1.COM
@ IF exist %1.COM DEL %1.EXE
@ GOTO End
:Ovl
@ LINK %1, VC.OVL;
@ IF errorlevel==1 GOTO End
@ DEL %1.OBJ
:End
