#include "intermediate.h"

int curtemp = 0;
int curvar = 0;
int curlabel = 0;

int newTemp()
{
    return ++curtemp;
}

char *newTempPlace()
{
    // 不超过99
    char *temp = (char *)malloc(4);
    sprintf(temp, "t%d", newTemp());
    return temp;
}

int newVar()
{
    return ++curvar;
}

Code *newCode(CodeType ctype)
{
    Code *s = (Code *)malloc(sizeof(Code));
    s->ctype = ctype;
    s->next = NULL;
    return s;
}

char *lookup(const char *id)
{
    int h = hash(id);
    Symbol *s = SymbolTable[h];
    while (s != NULL)
    {
        if (s->kind == Variable)
        {
            if (!strcmp(s->pvar->name, id))
            {
                char *res = (char *)malloc(4);
                if (s->pvar->vk) {
                    sprintf(res, "v%d", s->pvar->vk);
                } else {
                    int n = newVar();
                    sprintf(res, "v%d", n);
                    s->pvar->vk = n;
                }
                return res;
            }
        }
        s = s->next;
    }
    printf("Not Found the Variable (%s)\n", id);
    return NULL;
}

// gt should be a Exp node
char *getConstOrId(GTNODE *gt)
{
    char *res = NULL;
    if (!strcmp(gt->lchild->name, "CONSTVALUE"))
    {
        gt = gt->lchild->lchild;
        switch (gt->type) {
            case BOOL:
            case INT:
                res = (char *)malloc(10);
                sprintf(res, "#%d", gt->intval);
                break;
            case FLOAT:
                res = (char *)malloc(10);
                sprintf(res, "#%f", gt->floatval);
                break;
            default:
                break;
        }
    }
    else if (!strcmp(gt->name, "CONSTVALUE"))
    {
        gt = gt->lchild;
        switch (gt->type) {
            case BOOL:
            case INT:
                res = (char *)malloc(10);
                sprintf(res, "#%d", gt->intval);
                break;
            case FLOAT:
                res = (char *)malloc(10);
                sprintf(res, "#%f", gt->floatval);
                break;
            default:
                break;
        }
    }
    else if (!strcmp(gt->lchild->name, "IDENTIFIER") && gt->lchild->rchild == NULL)
    {
        res = lookup(gt->lchild->strval);
    }
    return res;
}

char *newLabel()
{
    char *l = (char *)malloc(sizeof(8));
    sprintf(l, "label%d", ++curlabel);
    return l;
}

Code* translate_Exp(GTNODE *gt, char *place)
{
    Code *c = NULL;
    if (!strcmp(gt->lchild->name, "IDENTIFIER"))
    {
        gt = gt->lchild;    // IDENTIFIER
        if (gt->rchild == NULL) // may not appear
        {
            char *v = lookup(gt->strval);
            if (v) {
                c = newCode(ASSIGN);
                sprintf(c->sentense, "%s := %s", place, v);
            }
        }
        else if (!strcmp(gt->rchild->name, "OPSELFADD"))
        {
            char *v = lookup(gt->strval);
            if (v) {
                if (place) {
                    c = newCode(ASSIGN);
                    sprintf(c->sentense, "%s := %s", place, v);
                    c->next = newCode(PLUS);
                    sprintf(c->next->sentense, "%s := %s + #1", v, v);
                } else {
                    c = newCode(PLUS);
                    sprintf(c->sentense, "%s := %s + #1", v, v);
                }
            }
        }
        else if (!strcmp(gt->rchild->name, "OPSELFMINUS"))
        {
            char *v = lookup(gt->strval);
            if (v) {
                Code *c2;
                if (place) {
                    c = newCode(ASSIGN);
                    sprintf(c->sentense, "%s := %s", place, v);
                    c->next = newCode(MINUS);
                    sprintf(c->next->sentense, "%s := %s - #1", v, v);
                } else {
                    c = newCode(MINUS);
                    sprintf(c->sentense, "%s := %s - #1", v, v);
                }
            }
        }
        else    // Call
        {
            c = newCode(CALL);
            if (gt->rchild->rchild->rchild == NULL)
            {
                if (place) {
                    sprintf(c->sentense, "%s := CALL %s", place, gt->strval);
                } else {
                    sprintf(c->sentense, "CALL %s", gt->strval);
                }
            }
            else
            {
                Arg *listhead = (Arg *)malloc(sizeof(Arg));
                listhead->next = NULL;
                c = translate_Args(gt->rchild->rchild, listhead);
                Arg *p = listhead->next, *q;
                Code head;
                head.next = NULL;
                Code *c2 = &head;
                while (p != NULL)
                {
                    q = p;
                    c2->next = newCode(ARG);
                    c2 = c2->next;
                    sprintf(c2->sentense, "ARG %s", p->a);
                    p = p->next;
                    free(q);
                }
                if (c) {
                    getCodeTail(c)->next = head.next;
                } else {
                    c = head.next;
                }
                free(listhead);
                if (place) {
                    Code *c3 = newCode(CALLASSIGN);
                    c2->next = c3;
                    sprintf(c3->sentense, "%s := CALL %s", place, gt->strval);
                } else {
                    Code *c3 = newCode(CALL);
                    c2->next = c3;
                    sprintf(c3->sentense, "CALL %s", gt->strval);
                }
            }
        }
    }
    else if (!strcmp(gt->lchild->name, "Exp"))
    {
        GTNODE *exp1 = gt->lchild;  // Exp1
        if (!strcmp(exp1->rchild->name, "OPASSIGN"))
        {
            char *v = getConstOrId(exp1);
            if (v)
            {
                char *con = getConstOrId(exp1->rchild->rchild);
                if (con) {
                    c = newCode(ASSIGN);
                    sprintf(c->sentense, "%s := %s", v, con);
                    if (place) {
                        c->next = newCode(ASSIGN);
                        sprintf(c->next->sentense, "%s := %s", place, v);
                    }
                } else {
                    char *t = newTempPlace();
                    c = translate_Exp(exp1->rchild->rchild, t);
                    Code *c2 = c;
                    while (c2->next != NULL) {
                        c2 = c2->next;
                    }
                    c2->next = newCode(ASSIGN);
                    c2 = c2->next;
                    sprintf(c2->sentense, "%s := %s", v, t);
                    if (place) {
                        c2->next = newCode(ASSIGN);
                        sprintf(c2->next->sentense, "%s := %s", place, v);
                    }
                }
            }
        }
        else if (!strcmp(exp1->rchild->name, "ROPAND") || !strcmp(exp1->rchild->name, "ROPOR")
            || !strcmp(exp1->rchild->name, "RELOP"))
        {
            char *l1 = newLabel();
            char *l2 = newLabel();
            c = newCode(ASSIGN);
            sprintf(c->sentense, "%s := #0", place);
            Code *c1 = translate_Cond(gt, l1, l2);
            c->next = c1;
            Code *c2 = c1;
            while (c2->next != NULL) {
                c2 = c2->next;
            }
            c2->next = newCode(LABEL);
            c2 = c2->next;
            sprintf(c2->sentense, "LABEL %s :", l1);
            c2->next = newCode(ASSIGN);
            Code *c3 = c2->next;
            sprintf(c3->sentense, "%s := #1", place);
            c3->next = newCode(LABEL);
            c3 = c3->next;
            sprintf(c3->sentense, "LABEL %s :", l2);    
        }
        else if (!strcmp(exp1->rchild->name, "OPADD") || !strcmp(exp1->rchild->name, "OPMINUS")
            || !strcmp(exp1->rchild->name, "OPMULTIPLY") || !strcmp(exp1->rchild->name, "OPDIVIDE"))
        {
            char op;
            CodeType ctype;
            if (!strcmp(exp1->rchild->name, "OPADD")) {
                op = '+';
                ctype = PLUS;
            } else if (!strcmp(exp1->rchild->name, "OPMINUS")) {
                op = '-';
                ctype = MINUS;
            } else if (!strcmp(exp1->rchild->name, "OPMULTIPLY")) {
                op = '*';
                ctype = MULTI;
            } else {
                op = '/';
                ctype = DIV;
            }
            char *var1 = getConstOrId(exp1);
            char *var2 = getConstOrId(exp1->rchild->rchild);
            if (var1) {
                if (var2) {
                    c = newCode(ctype);
                    sprintf(c->sentense, "%s := %s %c %s", place, var1, op, var2);
                } else {
                    var2 = newTempPlace();
                    c = translate_Exp(exp1->rchild->rchild, var2);
                    Code *c2 = c;
                    while (c2->next != NULL) {
                        c2 = c2->next;
                    }
                    c2->next = newCode(ctype);
                    c2 = c2->next;
                    sprintf(c2->sentense, "%s := %s %c %s", place, var1, op, var2);                    
                }
            } else {
                char *t1 = newTempPlace();
                c = translate_Exp(exp1, t1);
                Code *c2 = c;
                while (c2->next != NULL) {
                    c2 = c2->next;
                }
                if (var2) {
                    c2->next = newCode(ctype);
                    c2 = c2->next;
                    sprintf(c2->sentense, "%s := %s %c %s", place, t1, op, var2);
                } else {
                    var2 = newTempPlace();
                    c2->next = translate_Exp(exp1->rchild->rchild, var2);
                    while (c2->next != NULL) {
                        c2 = c2->next;
                    }
                    c2->next = newCode(ctype);
                    c2 = c2->next;
                    sprintf(c2->sentense, "%s := %s %c %s", place, t1, op, var2);
                }
            }
        }
        else    // += -= *= /=
        {
            char op;
            CodeType ctype;
            if (!strcmp(exp1->rchild->name, "OPADDASSIGN")) {
                op = '+';
                ctype = PLUS;
            } else if (!strcmp(exp1->rchild->name, "OPMINUSASSIGN")) {
                op = '-';
                ctype = MINUS;
            } else if (!strcmp(exp1->rchild->name, "OPMULASSIGN")) {
                op = '*';
                ctype = MULTI;
            } else {
                op = '/';
                ctype = DIV;
            }
            // exp1 should be identifier
            char *var1 = lookup(exp1->strval);
            if (var1) {
                char *var2 = getConstOrId(exp1->rchild->rchild);
                Code *c2;
                if (var2) {
                    c = newCode(ctype);
                    sprintf(c->sentense, "%s := %s %c %s", var1, var1, op, var2);
                    c2 = c;
                } else {
                    var2 = newTempPlace();
                    c = translate_Exp(exp1->rchild->rchild, var2);
                    c2 = c;
                    while (c2->next != NULL) {
                        c2 = c2->next;
                    }
                    c2->next = newCode(ctype);
                    c2 = c2->next;
                    sprintf(c2->sentense, "%s := %s %c %s", var1, var1, op, var2);
                }
                if (place) {
                    c2->next = newCode(ASSIGN);
                    c2 = c2->next;
                    sprintf(c2->sentense, "%s := %s", place, var1);
                }
            }
        }
    }   // end of first child is "Exp"
    else if (!strcmp(gt->lchild->name, "OPSELFADD"))
    {
        char *v = lookup(gt->lchild->rchild->strval);
        if (v) {
            Code *c2;
            c = newCode(PLUS);
            sprintf(c->sentense, "%s := %s + #1", v, v);
            if (place) {
                c->next = newCode(ASSIGN);
                sprintf(c->next->sentense, "%s := %s", place, v);
            }
        }
    }
    else if (!strcmp(gt->lchild->name, "OPSELFMINUS"))
    {
        char *v = lookup(gt->lchild->rchild->strval);
        if (v) {
            Code *c2;
            c = newCode(MINUS);
            sprintf(c->sentense, "%s := %s - #1", v, v);
            if (place) {
                c->next = newCode(ASSIGN);
                sprintf(c->next->sentense, "%s := %s", place, v);
            }
        }
    }
    else if (!strcmp(gt->lchild->name, "LEFTBRAKET"))
    {
        c = translate_Exp(gt->lchild->rchild, place);
    }
    else if (!strcmp(gt->lchild->name, "ROPNOT"))
    {
        char *l1 = newLabel();
        char *l2 = newLabel();
        c = newCode(ASSIGN);
        sprintf(c->sentense, "%s := #0", place);
        Code *c1 = translate_Cond(gt, l1, l2);
        c->next = c1;
        Code *c2 = c1;
        while (c2->next != NULL) {
            c2 = c2->next;
        }
        c2->next = newCode(LABEL);
        c2 = c2->next;
        sprintf(c2->sentense, "LABEL %s :", l1);
        c2->next = newCode(ASSIGN);
        Code *c3 = c2->next;
        sprintf(c3->sentense, "%s := #1", place);
        c3->next = newCode(LABEL);
        c3 = c3->next;
        sprintf(c3->sentense, "LABEL %s :", l2);
    }
    else if (!strcmp(gt->lchild->name, "OPMINUS"))
    {
        char *v = getConstOrId(gt->lchild->rchild);
        if (v) {
            c = newCode(MINUS);
            sprintf(c->sentense, "%s := #0 - %s", place, v);
        } else {
            char *t = newTempPlace();
            c = translate_Exp(gt->lchild->rchild, t);
            c->next = newCode(MINUS);
            sprintf(c->next->sentense, "%s := #0 - %s", place, t);
        }
    }
    else if (!strcmp(gt->lchild->name, "OPADD"))
    {
        return translate_Exp(gt->lchild->rchild, place);
    }
    else    // CONSTVALUE, this case should not appear
    {
        char *con = getConstOrId(gt);
        if (con) {
            c = newCode(ASSIGN);
            sprintf(c->sentense, "%s := %s", place, con);
        }
    }

    return c;
}


Code *translate_Args(GTNODE *gt, Arg *arglist)
{
    Code *c = NULL;
    GTNODE *exp1 = gt->lchild;
    char *v = getConstOrId(exp1);
    if (v == NULL) {
        v = newTempPlace();
        c = translate_Exp(exp1, v);
    }
    arglist->next = (Arg *)malloc(sizeof(Arg));
    strcpy(arglist->next->a, v);
    arglist->next->next = NULL;

    if (exp1->rchild != NULL) {
        if (c != NULL) {
            Code *c2 = c;
            while (c2->next != NULL) {
                c2 = c2->next;
            }
            c2->next = translate_Args(exp1->rchild->rchild, arglist->next);
        } else {
            c = translate_Args(exp1->rchild->rchild, arglist->next);
        }
    }
    return c;
}

Code *translate_Stmt(GTNODE *gt)
{
    Code *c = NULL;
    if (!strcmp(gt->lchild->name, "Exp"))
    {
        c = translate_Exp(gt->lchild, NULL);
    }
    else if (!strcmp(gt->lchild->name, "CompSt"))
    {
        // 临时重建符号表
        int l = getMaxLevel();
        createSymbolTable(gt->lchild->lchild->rchild->lchild, l + 1, 0);
        c = translate_CompSt(gt->lchild);
        removeSymbolOfLevel(l + 1);
    }
    else if (!strcmp(gt->lchild->name, "IfStmt"))
    {
        GTNODE *exp1 = gt->lchild->lchild->rchild->rchild;
        char *l1 = newLabel();
        char *l2 = newLabel();
        c = translate_Cond(exp1, l1, l2);
        Code *c2 = c;
        while (c2->next != NULL) {
            c2 = c2->next;
        }
        c2->next = newCode(LABEL);
        c2 = c2->next;
        sprintf(c2->sentense, "LABEL %s :", l1);
        c2->next = translate_Stmt(exp1->rchild->rchild);
        while (c2->next != NULL) {
            c2 = c2->next;
        }

        if (exp1->rchild->rchild->rchild == NULL) {
            c2->next = newCode(LABEL);
            c2 = c2->next;
            sprintf(c2->sentense, "LABEL %s :", l2);
        } else {
            GTNODE *stmt2 = exp1->rchild->rchild->rchild->rchild;
            char *l3 = newLabel();
            c2->next = newCode(GOTO);
            c2 = c2->next;
            sprintf(c2->sentense, "GOTO %s", l3);
            c2->next = newCode(LABEL);
            c2 = c2->next;
            sprintf(c2->sentense, "LABEL %s :", l2);
            c2->next = translate_Stmt(stmt2);
            while (c2->next != NULL) {
                c2 = c2->next;
            }
            c2->next = newCode(LABEL);
            c2 = c2->next;
            sprintf(c2->sentense, "LABEL %s :", l3);
        }
    }
    else if (!strcmp(gt->lchild->name, "ForStmt"))
    {
        GTNODE *exp1 = gt->lchild->lchild->rchild->rchild;
        GTNODE *exp2 = exp1->rchild->rchild;
        GTNODE *exp3 = exp2->rchild->rchild;
        GTNODE *stmt = exp3->rchild->rchild;
        char *l1 = newLabel();
        char *l2 = newLabel();
        char *l3 = newLabel();
        c = translate_Exp(exp1, NULL);
        Code *c2 = c;
        while (c2->next != NULL) {
            c2 = c2->next;
        }
        c2->next = newCode(LABEL);
        c2 = c2->next;
        sprintf(c2->sentense, "LABEL %s :", l1);
        c2->next = translate_Cond(exp2, l2, l3);
        while (c2->next != NULL) {
            c2 = c2->next;
        }
        c2->next = newCode(LABEL);
        c2 = c2->next;
        sprintf(c2->sentense, "LABEL %s :", l2);
        c2->next = translate_Stmt(stmt);
        while (c2->next != NULL) {
            c2 = c2->next;
        }
        c2->next = translate_Exp(exp3, NULL);
        while (c2->next != NULL) {
            c2 = c2->next;
        }
        c2->next = newCode(GOTO);
        c2 = c2->next;
        sprintf(c2->sentense, "GOTO %s", l1);
        c2->next = newCode(LABEL);
        sprintf(c2->next->sentense, "LABEL %s :", l3);
    }
    else if (!strcmp(gt->lchild->name, "WhileStmt"))
    {
        GTNODE *exp1 = gt->lchild->lchild->rchild->rchild;
        GTNODE *stmt = exp1->rchild->rchild;
        char *l1 = newLabel();
        char *l2 = newLabel();
        char *l3 = newLabel();
        c = newCode(LABEL);
        sprintf(c->sentense, "LABEL %s :", l1);
        c->next = translate_Cond(exp1, l2, l3);
        Code *c2 = c;
        while (c2->next != NULL) {
            c2 = c2->next;
        }
        c2->next = newCode(LABEL);
        c2 = c2->next;
        sprintf(c2->sentense, "LABEL %s :", l2);
        c2->next = translate_Stmt(stmt);
        while (c2->next != NULL) {
            c2 = c2->next;
        }
        c2->next = newCode(GOTO);
        c2 = c2->next;
        sprintf(c2->sentense, "GOTO %s", l1);
        c2->next = newCode(LABEL);
        sprintf(c2->next->sentense, "LABEL %s :", l3);
    }
    else if (!strcmp(gt->lchild->name, "ReturnStmt"))
    {
        if (gt->lchild->lchild->rchild == NULL) {
            c = newCode(RETURN);
            sprintf(c->sentense, "RETURN");
        } else {
            char *t = newTempPlace();
            c = translate_Exp(gt->lchild->lchild->rchild, t);
            Code *c2 = c;
            while (c2->next != NULL) {
                c2 = c2->next;
            }
            c2->next = newCode(RETURN);
            sprintf(c2->next->sentense, "RETURN %s", t);
        }
    }
    else if (!strcmp(gt->lchild->name, "PrintStmt"))
    {
        char *v = getConstOrId(gt->lchild->lchild->rchild->rchild);
        if (v) {
            c = newCode(WRITE);
            sprintf(c->sentense, "WRITE %s", v);
        }
    }

    return c;
}

Code *translate_Cond(GTNODE *gt, char *label_true, char *label_false)
{
    Code *c = NULL;
    if (!strcmp(gt->lchild->name, "ROPNOT"))
    {
        c = translate_Cond(gt, label_false, label_true);
    }
    else
    {
        // gt->lchild should be Exp
        GTNODE *exp1 = gt->lchild;
        if (!strcmp(exp1->rchild->name, "ROPAND"))
        {
            char *l1 = newLabel();
            c = translate_Cond(exp1, l1, label_false);
            Code *c2 = c;
            while (c2->next != NULL) {
                c2 = c2->next;
            }
            c2->next = newCode(LABEL);
            c2 = c2->next;
            sprintf(c2->sentense, "LABEL %s :", l1);
            c2->next = translate_Cond(exp1->rchild->rchild, label_true, label_false);
        }
        else if (!strcmp(exp1->rchild->name, "ROPOR"))
        {
            char *l1 = newLabel();
            c = translate_Cond(exp1, label_true, l1);
            Code *c2 = c;
            while (c2->next != NULL) {
                c2 = c2->next;
            }
            c2->next = newCode(LABEL);
            c2 = c2->next;
            sprintf(c2->sentense, "LABEL %s :", l1);
            c2->next = translate_Cond(exp1->rchild->rchild, label_true, label_false);
        }
        else
        {
            // should be Exp RELOP Exp
            char *op = exp1->rchild->strval;
            char *v1 = getConstOrId(exp1);
            char *v2 = getConstOrId(exp1->rchild->rchild);
            if (v1) {
                if (v2) {
                    c = newCode(IF);
                    sprintf(c->sentense, "IF %s %s %s GOTO %s", v1, op, v2, label_true);
                    c->next = newCode(GOTO);
                    sprintf(c->next->sentense, "GOTO %s", label_false);
                } else {
                    v2 = newTempPlace();
                    c = translate_Exp(exp1->rchild->rchild, v2);
                    Code *c2 = c;
                    while (c2->next != NULL) {
                        c2 = c2->next;
                    }
                    c2->next = newCode(IF);
                    c2 = c2->next;
                    sprintf(c2->sentense, "IF %s %s %s GOTO %s", v1, op, v2, label_true);
                    c2->next = newCode(GOTO);
                    sprintf(c2->next->sentense, "GOTO %s", label_false);
                }
            } else {
                v1 = newTempPlace();
                c = translate_Exp(exp1, v1);
                Code *c2 = c;
                while (c2->next != NULL) {
                    c2 = c2->next;
                }
                if (v2 == NULL) {
                    v2 = newTempPlace();
                    c2->next = translate_Exp(exp1->rchild->rchild, v2);
                    while (c2->next != NULL) {
                        c2 = c2->next;
                    }
                }
                c2->next = newCode(IF);
                c2 = c2->next;
                sprintf(c2->sentense, "IF %s %s %s GOTO %s", v1, op, v2, label_true);
                c2->next = newCode(GOTO);
                sprintf(c2->next->sentense, "GOTO %s", label_false);
            }
        }
    }
    return c;
}

Code *translate_CompSt(GTNODE *gt)
{
    GTNODE *deflist = gt->lchild->rchild;
    GTNODE *stmtlist = deflist->rchild;

    Code defHead;
    defHead.next = NULL;
    Code *c1 = &defHead;
    GTNODE *def = deflist->lchild;
    while (def != NULL)
    {
        GTNODE *dec = def->lchild->rchild->lchild;
        while (dec != NULL) {
            if (dec->lchild->rchild != NULL) {  // 声明定义
                GTNODE *exp = dec->lchild->rchild->rchild;
                char *v1 = lookup(dec->lchild->strval);
                char *v2 = getConstOrId(exp);
                if (v2 == NULL) {
                    v2 = newTempPlace();
                    c1->next = translate_Exp(exp, v2);
                    while (c1->next != NULL) {
                        c1 = c1->next;
                    }
                }
                c1->next = newCode(ASSIGN);
                c1 = c1->next;
                sprintf(c1->sentense, "%s := %s", v1, v2);
            }
            if ((dec = dec->rchild) != NULL) {
                dec = dec->rchild;
            }
        }
        def = def->rchild->lchild;
    }
    
    GTNODE *stmt = stmtlist->lchild;
    while (stmt != NULL) {
        c1->next = translate_Stmt(stmt);
        while (c1->next != NULL) {
            c1 = c1->next;
        }
        stmt = stmt->rchild->lchild;
    }

    return defHead.next;
}

Code *translate_ExtDecList(GTNODE *gt)
{
    Code *c = NULL;
    GTNODE *op = gt->lchild->rchild;
    if (op != NULL && !strcmp(op->name, "OPASSIGN"))
    {
        char *v = lookup(gt->lchild->strval);
        char *con = getConstOrId(op->rchild);
        if (v && con) {
            c = newCode(ASSIGN);
            sprintf(c->sentense, "%s := %s", v, con);
        }
    }
    return c;
}

Code *translate_Fundec(GTNODE *gt)
{
    Code *c = newCode(FUNC);
    sprintf(c->sentense, "FUNCTION %s :", gt->lchild->strval);
    GTNODE *varlist = gt->lchild->rchild->rchild;
    if (!strcmp(varlist->name, "VarList")) {
        Code *c2 = c;
        GTNODE *param = varlist->lchild;
        while (param != NULL) {
            c2->next = newCode(PARAM);
            c2 = c2->next;
            sprintf(c2->sentense, "PARAM %s", lookup(param->lchild->rchild->strval));
            if ((param = param->rchild) != NULL) {
                param = param->rchild->lchild;
            } 
        }
    }
    return c;
}

Code *translate_Dec(GTNODE *gt)
{
    Code *c = NULL;
    if (gt->lchild->rchild != NULL) {
        char *id = lookup(gt->lchild->strval);
        char *v = getConstOrId(gt->lchild->rchild->rchild);
        if (!v) {
            v = newTempPlace();
            c = translate_Exp(gt->lchild->rchild->rchild, v);
            Code *c2 = getCodeTail(c);
            c2->next = newCode(ASSIGN);
            c2 = c2->next;
            sprintf(c2->sentense, "%s := %s", id, v);
        } else {
            c = newCode(ASSIGN);
            sprintf(c->sentense, "%s := %s", id, v);
        }
    }
    return c;
}



void createInterCode(GTNODE *gt, int level, int trstmt, Code *ctail)
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
    else if (!strcmp(gt->name, "LEFTBRACE"))
    {
        level++;
    }
    else if (!strcmp(gt->name, "RIGHTBRACE"))
    {
        removeSymbolOfLevel(level);
        level--;
    }

    if (!strcmp(gt->name, "IfStmt") || !strcmp(gt->name, "ForStmt") || !strcmp(gt->name, "WhileStmt"))
    {
        trstmt = 0;
    }
    createInterCode(gt->lchild, level, trstmt, getCodeTail(ctail));

    if (!strcmp(gt->name, "ExtDecList")) {
        getCodeTail(ctail)->next = translate_ExtDecList(gt);
    } else if (!strcmp(gt->name, "FunDec")) {
        getCodeTail(ctail)->next = translate_Fundec(gt);
    } else if (trstmt && !strcmp(gt->name, "Dec")) {
        getCodeTail(ctail)->next = translate_Dec(gt);
    } else if (trstmt && !strcmp(gt->name, "Stmt") && strcmp(gt->lchild->name, "CompSt")) {
        getCodeTail(ctail)->next = translate_Stmt(gt);
    }

    createInterCode(gt->rchild, level, trstmt, getCodeTail(ctail));
}

Code *getCodeTail(Code *c)
{
    if (c == NULL) {
        return NULL;
    }
    while (c->next != NULL) {
        c = c->next;
    }
    return c;
}

void printCode(Code *c, int tofile)
{
    if (tofile)
    {
        FILE *f = fopen("intercode.ir", "w+");
        while (c != NULL)
        {
            fprintf(f, "%s\n", c->sentense);
            c = c->next;
        }
        fclose(f);
    }
    else
    {
        while (c != NULL)
        {
            printf("%s  %s\n", c->sentense, printCodeType(c->ctype));
            c = c->next;
        }
    }
}

// just for test
char *printCodeType(CodeType ctype)
{
    char *s = (char *)malloc(sizeof(12));
    switch (ctype) {
        case LABEL:
            sprintf(s, "LABEL");
            break;
        case ASSIGN:
            sprintf(s, "ASSIGN");
            break;
        case PLUS:
            sprintf(s, "PLUS");
            break;
        case MINUS:
            sprintf(s, "MINUS");
            break;
        case MULTI:
            sprintf(s, "MULTI");
            break;
        case DIV:
            sprintf(s, "DIV");
            break;
        case GOTO:
            sprintf(s, "GOTO");
            break;
        case ARG:
            sprintf(s, "ARG");
            break;
        case CALL:
            sprintf(s, "CALL");
            break;
        case CALLASSIGN:
            sprintf(s, "CALLASSIGN");
            break;
        case RETURN:
            sprintf(s, "RETURN");
            break;
        case FUNC:
            sprintf(s, "FUNC");
            break;
        case PARAM:
            sprintf(s, "PARAM");
            break;
        case IF:
            sprintf(s, "IF");
            break;
        case WRITE:
            sprintf(s, "WRITE");
            break;
        default:
            sprintf(s, "ERROR");
            break;
    }
    return s;
}