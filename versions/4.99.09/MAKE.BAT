@ECHO OFF
REM tasm / tlink should be in PATH
TASMX /z/m9/kh10000 vc.asm
TLINK /t vc, VC.COM
TASMX /z/m9/kh10000 vcovl.asm
TLINK vcovl, VC.OVL
DEL *.OBJ
DEL *.MAP
