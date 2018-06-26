parser: parse.l grammartree.y grammartree.h
	bison -d -v grammartree.y
	flex parse.l
	gcc grammartree.c grammartree.tab.c -lfl -ly -o parser

.PHONY: clean
clean:
	rm parser grammartree.tab.c grammartree.tab.h lex.yy.c