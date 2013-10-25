@echo off
rm *.PBP
make
copy EBOOT.PBP Z:\Descargas\jpcsp-r3095-windows-x86-32\jpcsp-windows-x86\ms0\PSP\GAME\138CPP\EBOOT.PBP
rm *.o *.elf *.sfo graphics/*.o gb138/*.o other/*.o
pause > nul