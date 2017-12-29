@echo off
cl /W3 /GS /Gd /Zi /Od /Ob0 /MDd /TC /D_CRT_SECURE_NO_WARNINGS /Fe: json_test.exe /Fd: json_test.pdb json.c json_test.c
del *.ilk *.obj
