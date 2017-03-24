all: cd Lexer Parser VirtualMachine

cd: CompilerDriver.c
	gcc -o cd CompilerDriver.c

Lexer: Lexer.c
	gcc Lexer.c -o Lexer.exe

Parser: Parser.c
	gcc Parser.c -o Parser.exe

VirtualMachine: VirtualMachine.c
	gcc VirtualMachine.c -o VirtualMachine.exe
