parser: parse.l grammartree.y grammartree.h symbol.h intermediate.h object.h
	bison -d grammartree.y
	flex parse.l
	gcc symbol.c grammartree.c intermediate.c object.c grammartree.tab.c -lfl -ly -o parser

.PHONY: clean
clean:
	rm parser grammartree.tab.c grammartree.tab.h lex.yy.c intercode.ir out.s