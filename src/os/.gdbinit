#https://github.com/MrBad/cOSiris/blob/kdbg/.gdbinit
set architecture i386
set disassembly-flavor intel
symbol-file build/system
set prompt \033[31mgdb$ \033[0m
set remotetimeout 10
target remote localhost:8821
#set debug remote 1


