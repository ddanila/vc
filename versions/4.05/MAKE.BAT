@ TASMX /z/m9/kh10000 %1;
@ IF errorlevel==1 GOTO End
@ TLINK /t/x %1;
@ IF errorlevel==1 GOTO End
@ DEL %1.OBJ
:End
