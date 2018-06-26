#include "symbol.h"

int hash(const char *name)
{
    int val = 0;
    for (; *name != '\0'; name++)
    {
        val = (val << 2) + *name;
        val = val % SIZE;
    }
    return val;
}

void initSymbolTable()
{
    int i;
    for (i = 0; i < SIZE; i++)
    {
        SymbolTable[i] = NULL;
    }
}

Varkind getVarkind(const char *type)
{
    if (!strcmp(type, "int")) {
        return INT;
    } else if (!strcmp(type, "float")) {
        return FLOAT;
    } else if (!strcmp(type, "char")) {
        return CHAR;
    } else if (!strcmp(type, "bool")) {
        return BOOL;
    } else if (!strcmp(type, "string")) {
        return STRING;
    } else {
        return VOID;
    }
}

int repeatName(Symbol *s, char *name, int level, int line)
{
    int repeat = 0;
    while (s != NULL && s->level == level)
    {
        if (s->kind == Variable)
        {
            if (!strcmp(s->pvar->name, name)) {
                repeat = 1;
                break;
            }
        }
        else
        {
            if (!strcmp(s->pfun->name, name)) {
                repeat = 1;
                break;
            }
        }
        s = s->next;
    }
    if (repeat) {
        serror++;
        printf("Error Type 3 at line %d: Redefined name '%s'\n", line, name);
    }
    return repeat;
}

void newVarSymbol(char *name, Varkind k, int level, int line)
{
    int h = hash(name);
    if (!repeatName(SymbolTable[h], name, level, line)) {
        Var *var = (Var *)malloc(sizeof(Var));
        var->name = (char *)malloc(strlen(name) + 1);
        var->vk = 0;
        strcpy(var->name, name);
        var->varkind = k;
        Symbol *varsym = (Symbol *)malloc(sizeof(Symbol));
        varsym->kind = Variable;
        varsym->level = level;
        varsym->pvar = var;
        varsym->next = SymbolTable[h];
        SymbolTable[h] = varsym;
    }
}

Varkind getSymbolType(const char *name)
{
    int h = hash(name);
    Symbol *p = SymbolTable[h];
    while (p != NULL)
    {
        if (p->kind == Variable) {
            if (!strcmp(p->pvar->name, name)) {
                return p->pvar->varkind;
            }
        }
        else {
            if (!strcmp(p->pfun->name, name)) {
                return p->pfun->retkind;
            }
        }
        p = p->next;
    }
    return VOID;
}

Varkind getExpType(GTNODE *gt)
{
    if (gt->type != VOID) {
        return gt->type;
    }

    if (!strcmp(gt->lchild->name, "IDENTIFIER"))
    {
        Varkind v = getSymbolType(gt->lchild->strval);
        gt->type = v;
        if (v == VOID) {
            serror++;
            printf("Error Type 1 at line %d: Undefined variable or function (%s)\n", gt->lchild->line, gt->lchild->strval);
            return VOID;
        }
        // SELFADD SELFMINUS
        if (gt->lchild->rchild != NULL) {
            if (!strcmp(gt->lchild->rchild->name, "OPSELFADD") || !strcmp(gt->lchild->rchild->name, "OPSELFMINUS")) {
                if (v != INT) {
                    serror++;
                    printf("Error Type 7 at line %d: Type dismatched for operand\n", gt->lchild->line);
                    gt->type = VOID;
                    return VOID;
                }
            }
            else if (!strcmp(gt->lchild->rchild->name, "LEFTBRAKET")) {
                int h = hash(gt->lchild->strval);
                Symbol *p = SymbolTable[h];
                while (p != NULL)
                {
                    if (p->kind == Variable) {
                        if (!strcmp(p->pvar->name, gt->lchild->strval)) {
                            // not a function
                            serror++;
                            printf("Error Type 11 at line %d: %s is not a function\n", gt->line, gt->lchild->strval);
                            return v;
                        }
                    }
                    else {
                        if (!strcmp(p->pfun->name, gt->lchild->strval)) {
                            break;
                        }
                    }
                    p = p->next;
                }

                // check paramlist
                if (p != NULL)
                {
                    Fun *f = p->pfun;
                    if (!strcmp(gt->lchild->rchild->rchild->name, "Args")) {
                        GTNODE *e = gt->lchild->rchild->rchild->lchild; // Exp
                        Varlist *vl = f->paramlist;
                        while (vl != NULL && e != NULL) {
                            if (vl->varkind != getExpType(e)) {
                                break;
                            }
                            vl = vl->next;
                            if ((e = e->rchild) != NULL) {
                                e = e->rchild->lchild;
                            }
                        }
                        if (vl != NULL || e != NULL) {
                            serror++;
                            printf("Error Type 9 at line %d: Param dismatched\n", gt->line);
                        }
                    } else {
                        if (f->paramlist != NULL) {
                            serror++;
                            printf("Error Type 9 at line %d: Param dismatched\n", gt->line);
                        }
                    }
                }

            }
        }

        return v;
    }
    else if (!strcmp(gt->lchild->name, "Exp"))
    {
        Varkind v1 = getExpType(gt->lchild);
        Varkind v2 = getExpType(gt->lchild->rchild->rchild);
        if (v1 == VOID || v2 == VOID) {
            return v1;
        }
        if (!strcmp(gt->lchild->rchild->name, "ROPAND")
        || !strcmp(gt->lchild->rchild->name, "ROPOR")) {
            if (v1 != BOOL || v2 != BOOL) {
                serror++;
                printf("Error Type 7 at line %d: Type dismatched for operand\n", gt->lchild->line);
                gt->type = VOID;
                return VOID;
            } else {
                gt->type = BOOL;
                return BOOL;
            }
        } else if (!strcmp(gt->lchild->rchild->name, "OPASSIGN")) {
            if (!strcmp(gt->lchild->lchild->name, "CONSTVALUE")) {
                serror++;
                printf("Error Type 6 at line %d: Const can't be left value\n", gt->line);
                return gt->lchild->lchild->type;
            }
            if (v1 != v2) {
                gt->type = VOID;
                serror++;
                printf("Error Type 5 at line %d: Type of operators between '=' dismatched\n", gt->lchild->line);
                return VOID;
            }
        } else {
            if (v1 != v2) {
                gt->type = VOID;
                serror++;
                printf("Error Type 5 at line %d: Type of operators dismatched\n", gt->lchild->line);
                return VOID;
            }
            if (v1 != INT && v1 != FLOAT) {
                serror++;
                printf("Error Type 7 at line %d: Type dismatched for operand\n", gt->lchild->line);
                gt->type = VOID;
                return VOID;
            }
            
            if (!strcmp(gt->lchild->rchild->name, "RELOP")) {
                gt->type = BOOL;
                return BOOL;
            } else {
                return v1;
            }
        }
    }
    else if (!strcmp(gt->lchild->name, "OPSELFADD") || !strcmp(gt->lchild->name, "OPSELFMINUS"))
    {
        Varkind v = getSymbolType(gt->lchild->rchild->strval);
        gt->type = v;
        if (v == VOID) {
            serror++;
            printf("Error Type 1 at line %d: Undefined variable or function (%s)\n", gt->lchild->line, gt->lchild->rchild->strval);
            return VOID;
        }
        if (v != INT) {
            serror++;
            printf("Error Type 7 at line %d: Type dismatched for operand\n", gt->lchild->line);
            gt->type = VOID;
            return VOID;
        }
        return v;
    }
    else if (!strcmp(gt->lchild->name, "LEFTBRAKET"))
    {
        Varkind v = getExpType(gt->lchild->rchild);
        return v;
    }
    else if (!strcmp(gt->lchild->name, "ROPNOT"))
    {
        Varkind v = getExpType(gt->lchild->rchild);
        if (v != VOID && v != BOOL) {
            serror++;
            printf("Error Type 7 at line %d: Type dismatched for operand\n", gt->lchild->line);
            gt->type = VOID;
            return VOID;
        }
    }
    else if (!strcmp(gt->lchild->name, "OPMINUS") || !strcmp(gt->lchild->name, "OPADD"))
    {
        Varkind v = getExpType(gt->lchild->rchild);
        if (v != VOID && v != INT && v != FLOAT) {
            serror++;
            printf("Error Type 7 at line %d: Type dismatched for operand\n", gt->lchild->line);
            gt->type = VOID;
            return VOID;
        }
    }
    else    // CONSTVALUE
    {
        gt->type = gt->lchild->type = gt->lchild->lchild->type;
        return gt->type;
    }
}

char *getNameOfVarkind(Varkind v)
{
    switch(v) {
        case INT:
            return "int";
        case FLOAT:
            return "float";
        case CHAR:
            return "char";
        case BOOL:
            return "bool";
        case STRING:
            return "string";
        default:
            return "void";
    }
}

void showSymbolTable()
{
    int i;
    printf("┌───────┐\n");
    for (i = 0; i < SIZE; i++)
    {
        printf("│  %2d   │", i);
        Symbol *s = SymbolTable[i];
        while (s != NULL)
        {
            char *name;
            Varkind v;
            if (s->kind == Variable) {
                name = s->pvar->name;
                v = s->pvar->varkind;
            } else {
                name = s->pfun->name;
                v = s->pfun->retkind;
            }
            printf(" ---> [%s, %s, %d]", name, getNameOfVarkind(v), s->level);
            s = s->next;
        }
        if (i < SIZE - 1)
            printf("\n├───────┤\n");
    }
    printf("\n└───────┘\n\n");
}

int getMaxLevel()
{
    int max = 0;
    int i;
    for (i = 0; i < SIZE; i++)
    {
        Symbol *s = SymbolTable[i];
        while (s != NULL)
        {
            if (s->level > max) {
                max = s->level;
            }
            s = s->next;
        }
    }
    return max;
}

Varkind findRetType(GTNODE *gt, int *line) // gt = CompSt
{
    GTNODE *stmtlist = gt->lchild->rchild->rchild;  // StmtList
    GTNODE *stmt = stmtlist->lchild;
    while (stmt != NULL)
    {
        if (!strcmp(stmt->lchild->name, "ReturnStmt")) {
            *line = stmt->lchild->line;
            if (stmt->lchild->lchild->rchild != NULL) {
                return getExpType(stmt->lchild->lchild->rchild);
            } else {
                return VOID;
            }

        }
        stmt = stmt->rchild->lchild;
    }
    *line = stmtlist->rchild->line;
    return VOID;
}

void createSymbolTable(GTNODE *gt, int level, int showTable)
{
    if (gt == NULL) {
        return;
    }

    if (!strcmp(gt->name, "ExtDef")) {
        GTNODE *type = gt->lchild;
        Varkind v = getVarkind(type->strval);
        GTNODE *p = type->rchild;
        if (!strcmp(p->name, "FunDec"))
        {
            int h = hash(p->lchild->strval);
            Symbol *s = SymbolTable[h];

            if (!repeatName(s, p->lchild->strval, level, p->lchild->line))
            {
                Fun *fun = (Fun *)malloc(sizeof(Fun));
                fun->name = (char *)malloc(sizeof(char) * (strlen(p->lchild->strval) + 1));
                strcpy(fun->name, p->lchild->strval);
                fun->retkind = v;
                
                Symbol *funsym = (Symbol *)malloc(sizeof(Symbol));
                funsym->kind = Function;
                funsym->level = level;
                funsym->pfun = fun;
                funsym->next = SymbolTable[h];
                SymbolTable[h] = funsym;

                Varlist funvar;         // temp head node
                Varlist *pv = &funvar;

                GTNODE *tmp = p->lchild->rchild->rchild;    // VarList
                if (tmp->rchild != NULL) {
                    tmp = tmp->lchild;      // ParamDec
                    while (tmp != NULL)
                    {
                        Varlist *v = (Varlist *)malloc(sizeof(Varlist));
                        Varkind vkind = getVarkind(tmp->lchild->strval);
                        v->varkind = vkind;
                        pv->next = v;
                        pv = v;
                        newVarSymbol(tmp->lchild->rchild->strval, vkind, level + 1, tmp->lchild->rchild->line);
                        if ((tmp = tmp->rchild) != NULL) {
                            tmp = tmp->rchild->lchild;
                        }
                    }
                }
                pv->next = NULL;
                fun->paramlist = funvar.next;
            }

        }
        else    // p == ExtDecList
        {
            while (p != NULL)
            {
                if (!strcmp(p->name, "ExtDecList"))
                {
                    p = p->lchild;  // IDENTIFIER
                    if (p->rchild != NULL && !strcmp(p->rchild->name, "OPASSIGN")) {
                        if (v != p->rchild->rchild->lchild->type) {
                            serror++;
                            printf("Error Type 5 at line %d: Type of operators between '=' dismatched\n", p->line);
                            p = p->rchild;
                            continue;
                        }
                    }
                    newVarSymbol(p->strval, v, level, p->line);
                }
                p = p->rchild;
            }
        }
    }
    else if (!strcmp(gt->name, "Def"))
    {
        GTNODE *type = gt->lchild;          // TYPE
        GTNODE *dec = type->rchild->lchild; // Dec
        Varkind v = getVarkind(type->strval);
        while (dec != NULL)
        {
            if (dec->lchild->rchild != NULL && v != getExpType(dec->lchild->rchild->rchild)) {
                serror++;
                printf("Error Type 5 at line %d: Type of operators between '=' dismatched\n", dec->line);                    
            } else {
                newVarSymbol(dec->lchild->strval, v, level, dec->lchild->line);
            }
            if (dec = dec->rchild) {
                dec = dec->rchild->lchild;
            }
        }
    }
    else if (!strcmp(gt->name, "DecList"))
    {
        return; // 已在Def中处理，防止重复检查其中的Exp
    }
    else if (!strcmp(gt->name, "LEFTBRACE"))
    {
        level++;
    }
    else if (!strcmp(gt->name, "RIGHTBRACE"))
    {
        if (showTable) {
            printf("\nAt Line %d:\n", gt->line);
            showSymbolTable();
        }
        removeSymbolOfLevel(level);
        level--;
    }
    else if (!strcmp(gt->name, "IfStmt") || !strcmp(gt->name, "WhileStmt"))   // if while condition
    {
        Varkind v = getExpType(gt->lchild->rchild->rchild);
        if (v != BOOL) {
            serror++;
            printf("Error Type 7 at line %d: Type dismatched\n", gt->lchild->line);
        }
    }
    else if (!strcmp(gt->name, "ForStmt"))   // for condition
    {
        Varkind v = getExpType(gt->lchild->rchild->rchild->rchild->rchild);
        if (v != BOOL) {
            serror++;
            printf("Error Type 7 at line %d: Type dismatched\n", gt->lchild->line);
        }
    }
    else if (!strcmp(gt->name, "Exp"))  // 类型检查
    {
        getExpType(gt);
        createSymbolTable(gt->rchild, level, showTable);
        return; // 不再递归查询子表达式
    }

    createSymbolTable(gt->lchild, level, showTable);
    if (!strcmp(gt->name, "ExtDef") && !strcmp(gt->lchild->rchild->name, "FunDec"))
    {
        // check return type
        Varkind v = getVarkind(gt->lchild->strval);
        int ln;
        Varkind retv = findRetType(gt->lchild->rchild->rchild, &ln);
        if (v != retv) {
            serror++;
            printf("Error Type 8 at line %d: Return Type dismatched with definition\n", ln);
        }
    }
    createSymbolTable(gt->rchild, level, showTable);

}


void removeSymbolOfLevel(int level)
{
    int i;
    for (i = 0; i < SIZE; i++)
    {
        Symbol *s = SymbolTable[i];
        Symbol *n = SymbolTable[i];
        while (s != NULL && s->level >= level)
        {
            n = s->next;
            if (s->kind == Variable)
            {
                free(s->pvar);
                free(s);
            }
            else
            {
                Varlist *p = s->pfun->paramlist;
                Varlist *pn;
                while (p != NULL)
                {
                    pn = p->next;
                    free(p);
                    p = pn;
                }
                free(s->pfun);
                free(s);
            }
            s = n;
        }
        SymbolTable[i] = n;
    }
}

