#include "object.h"

int memSP;  // 内存堆栈指针
int *VA;
int *TA;

int curarg;
int curparam;
int vcount;
int tcount;

void initVsym(int v, int t)
{
    vcount = v;
    tcount = t;
    VA = (int *)malloc(sizeof(int) * v);
    int i;
    for (i = 0; i < v; i++)
    {
        *(VA + i) = i * 4;
    }
    TA = (int *)malloc(sizeof(int) * t);
    for (i = 0; i < t; i++)
    {
        *(TA + i) = (i + v) * 4;
    }
    curarg = 0;
    curparam = 0;
}


void tran_assign(Code *code, FILE *f)
{
    char a[10], b[10];    // a := b
    sscanf(code->sentense, "%s %*s %s", a, b);

    if (b[0] == '#') {
        fprintf(f, "  li $t0, %s\n", b + 1);
    } else {
        // 从内存加载
        int bk = atoi(b + 1) - 1;
        if (b[0] == 'v') {
            fprintf(f, "  lw $t1, %d($s0)\n", VA[bk]);
        } else {
            fprintf(f, "  lw $t1, %d($s0)\n", TA[bk]);
        }
        // 赋值运算
        fprintf(f, "  move $t0, $t1\n");
    }

    // 将结果送回内存
    int ak = atoi(a + 1) - 1;
    if (a[0] == 'v') {
        fprintf(f, "  sw $t0, %d($s0)\n", VA[ak]);
    } else if (a[0] == 't') {
        fprintf(f, "  sw $t0, %d($s0)\n", TA[ak]);
    }
}

void tran_plus(Code *code ,FILE *f)
{
    char a[10], b[10], c[10];
    sscanf(code->sentense, "%s %*s %s %*s %s", a, b, c);

    if (b[0] == '#') {
        fprintf(f, "  li $t1, %s\n", b + 1);
    } else {
        // 从内存加载
        int bk = atoi(b + 1) - 1;
        if (b[0] == 'v') {
            fprintf(f, "  lw $t1, %d($s0)\n", VA[bk]);
        } else {
            fprintf(f, "  lw $t1, %d($s0)\n", TA[bk]);
        }
    }

    if (c[0] == '#') {
        fprintf(f, "  addi $t0, $t1, %s\n", c + 1);
    } else {
        // 从内存加载
        int ck = atoi(c + 1) - 1;
        if (c[0] == 'v') {
            fprintf(f, "  lw $t2, %d($s0)\n", VA[ck]);
        } else {
            fprintf(f, "  lw $t2, %d($s0)\n", TA[ck]);
        }
        fprintf(f, "  add $t0, $t1, $t2\n");
    }

    // 将结果送回内存
    int ak = atoi(a + 1) - 1;
    if (a[0] == 'v') {
        fprintf(f, "  sw $t0, %d($s0)\n", VA[ak]);
    } else if (a[0] == 't') {
        fprintf(f, "  sw $t0, %d($s0)\n", TA[ak]);
    }
}

void tran_minus(Code *code ,FILE *f)
{
    char a[10], b[10], c[10];
    sscanf(code->sentense, "%s %*s %s %*s %s", a, b, c);

    if (b[0] == '#') {
        fprintf(f, "  li $t1, %s\n", b + 1);
    } else {
        // 从内存加载
        int bk = atoi(b + 1) - 1;
        if (b[0] == 'v') {
            fprintf(f, "  lw $t1, %d($s0)\n", VA[bk]);
        } else {
            fprintf(f, "  lw $t1, %d($s0)\n", TA[bk]);
        }
    }

    if (c[0] == '#') {
        if (c[1] == '-')
            fprintf(f, "  addi $t0, $t1, %s\n", c + 2);
        else
            fprintf(f, "  addi $t0, $t1, %s\n", c + 1);
    } else {
        // 从内存加载
        int ck = atoi(c + 1) - 1;
        if (c[0] == 'v') {
            fprintf(f, "  lw $t2, %d($s0)\n", VA[ck]);
        } else {
            fprintf(f, "  lw $t2, %d($s0)\n", TA[ck]);
        }
        fprintf(f, "  sub $t0, $t1, $t2\n");
    }

    // 将结果送回内存
    int ak = atoi(a + 1) - 1;
    if (a[0] == 'v') {
        fprintf(f, "  sw $t0, %d($s0)\n", VA[ak]);
    } else if (a[0] == 't') {
        fprintf(f, "  sw $t0, %d($s0)\n", TA[ak]);
    }
}

void tran_multi(Code *code ,FILE *f)
{
    char a[10], b[10], c[10];
    sscanf(code->sentense, "%s %*s %s %*s %s", a, b, c);

    if (b[0] == '#') {
        fprintf(f, "  li $t1, %s\n", b + 1);
    } else {
        // 从内存加载
        int bk = atoi(b + 1) - 1;
        if (b[0] == 'v') {
            fprintf(f, "  lw $t1, %d($s0)\n", VA[bk]);
        } else {
            fprintf(f, "  lw $t1, %d($s0)\n", TA[bk]);
        }
    }

    if (c[0] == '#') {
        fprintf(f, "  li $t2, %s\n", c + 1);
    } else {
        // 从内存加载
        int ck = atoi(c + 1) - 1;
        if (c[0] == 'v') {
            fprintf(f, "  lw $t2, %d($s0)\n", VA[ck]);
        } else {
            fprintf(f, "  lw $t2, %d($s0)\n", TA[ck]);
        }
    }
    fprintf(f, "  mul $t0, $t1, $t2\n");

    // 将结果送回内存
    int ak = atoi(a + 1) - 1;
    if (a[0] == 'v') {
        fprintf(f, "  sw $t0, %d($s0)\n", VA[ak]);
    } else if (a[0] == 't') {
        fprintf(f, "  sw $t0, %d($s0)\n", TA[ak]);
    }
}

void tran_div(Code *code ,FILE *f)
{
    char a[10], b[10], c[10];
    sscanf(code->sentense, "%s %*s %s %*s %s", a, b, c);

    if (b[0] == '#') {
        fprintf(f, "  li $t1, %s\n", b + 1);
    } else {
        // 从内存加载
        int bk = atoi(b + 1) - 1;
        if (b[0] == 'v') {
            fprintf(f, "  lw $t1, %d($s0)\n", VA[bk]);
        } else {
            fprintf(f, "  lw $t1, %d($s0)\n", TA[bk]);
        }
    }

    if (c[0] == '#') {
        fprintf(f, "  li $t2, %s\n", c + 1);
    } else {
        // 从内存加载
        int ck = atoi(c + 1) - 1;
        if (c[0] == 'v') {
            fprintf(f, "  lw $t2, %d($s0)\n", VA[ck]);
        } else {
            fprintf(f, "  lw $t2, %d($s0)\n", TA[ck]);
        }
    }
    fprintf(f, "  div $t1, $t2\n");
    fprintf(f, "  mflo $t0\n");

    // 将结果送回内存
    int ak = atoi(a + 1) - 1;
    if (a[0] == 'v') {
        fprintf(f, "  sw $t0, %d($s0)\n", VA[ak]);
    } else if (a[0] == 't') {
        fprintf(f, "  sw $t0, %d($s0)\n", TA[ak]);
    }
}

void tran_arg(Code *code, FILE *f)
{
    // 将参数送至寄存器$a(i)
    char a[10];
    sscanf(code->sentense, "%*s %s", a);
    if (a[0] == '#') {
        fprintf(f, "  li $a%d %s\n", curarg, a + 1);    
    } else {
        int ak = atoi(a + 1) - 1;
        if (a[0] == 'v') {
            fprintf(f, "  lw $a%d %d($s0)\n", curarg, VA[ak]);
        } else {
            fprintf(f, "  lw $a%d %d($s0)\n", curarg, TA[ak]);
        }
    }

    curarg++;
}

void tran_callassign(Code *code, FILE *f)
{
    char a[4], fun[12];
    sscanf(code->sentense, "%s %*s %*s %s", a, fun);
    curarg = 0;
    fprintf(f, "  addi $sp, $sp, -4\n");
    fprintf(f, "  sw $ra, 0($sp)\n");
    fprintf(f, "  jal %s\n", fun);
    fprintf(f, "  lw $ra, 0($sp)\n");
    fprintf(f, "  addi $sp, $sp, 4\n");
    int ak = atoi(a + 1) - 1;
    if (a[0] == 'v') {
        fprintf(f, "  sw $v0, %d($s0)\n", VA[ak]);
    } else if (a[0] == 't') {
        fprintf(f, "  sw $v0, %d($s0)\n", TA[ak]);
    }

}

void tran_return(Code *code, FILE *f)
{
    curparam = 0;
    if (strlen(code->sentense) == 6) {
        fprintf(f, "  jr $ra\n");
        return;
    }

    char ret[4];
    sscanf(code->sentense, "%*s %s", ret);
    int rk = atoi(ret + 1) - 1;
    if (ret[0] == 'v') {
        fprintf(f, "  lw $v0, %d($s0)\n", VA[rk]);
    } else {
        fprintf(f, "  lw $v0, %d($s0)\n", TA[rk]);
    }
    fprintf(f, "  jr $ra\n");
}

void tran_param(Code *code, FILE *f)
{
    char p[4];
    sscanf(code->sentense, "%*s %s", p);
    int pk = atoi(p + 1) - 1;
    if (p[0] == 'v') {
        fprintf(f, "  sw $a%d, %d($s0)\n", curparam, VA[pk]);
    } else {
        fprintf(f, "  sw $a%d, %d($s0)\n", curparam, TA[pk]);
    }
    curparam++;
}

void tran_if(Code *code, FILE *f)
{
    char x[10], y[10], z[10], op[4];
    sscanf(code->sentense, "%*s %s %s %s %*s %s", x, op, y, z);
    if (x[0] == '#') {
        fprintf(f, "  li $t0, %s\n", x + 1);
    } else {
        // 从内存加载
        int xk = atoi(x + 1) - 1;
        if (x[0] == 'v') {
            fprintf(f, "  lw $t0, %d($s0)\n", VA[xk]);
        } else {
            fprintf(f, "  lw $t0, %d($s0)\n", TA[xk]);
        }
    }
    if (y[0] == '#') {
        fprintf(f, "  li $t1, %s\n", y + 1);
    } else {
        // 从内存加载
        int yk = atoi(y + 1) - 1;
        if (y[0] == 'v') {
            fprintf(f, "  lw $t1, %d($s0)\n", VA[yk]);
        } else {
            fprintf(f, "  lw $t1, %d($s0)\n", TA[yk]);
        }
    }

    // 比较运算符转换为指令关键字
    if (!strcmp(op, "==")) {
        strcpy(op, "beq");
    } else if (!strcmp(op, "!=")) {
        strcpy(op, "bne");
    } else if (!strcmp(op, ">")) {
        strcpy(op, "bgt");
    } else if (!strcmp(op, "<")) {
        strcpy(op, "blt");
    } else if (!strcmp(op, ">=")) {
        strcpy(op, "bge");
    } else if (!strcmp(op, "<=")) {
        strcpy(op, "ble");
    }
    
    fprintf(f, "  %s $t0, $t1, %s\n", op, z);
}

void tran_write(Code *code, FILE *f)
{
    char p[4];
    sscanf(code->sentense, "%*s %s", p);
    int pk = atoi(p + 1) - 1;
    if (p[0] == 'v') {
        fprintf(f, "  lw $t0, %d($s0)\n", VA[pk]);
    } else {
        fprintf(f, "  lw $t0, %d($s0)\n", TA[pk]);
    }
    fprintf(f, "  move $a0, $t0\n");
    fprintf(f, "  addi $sp, $sp, -4\n");
    fprintf(f, "  sw $ra, 0($sp)\n");
    fprintf(f, "  jal write\n");
    fprintf(f, "  lw $ra, 0($sp)\n");
    fprintf(f, "  addi $sp, $sp, 4\n");
}

void initCode(FILE *f)
{
    fprintf(f, ".data\n");
    fprintf(f, "_ret: .asciiz \"\\n\"\n");
    fprintf(f, "var: .word");
    int i;
    for (i = 1; i < (vcount + tcount); i++)
    {
        fprintf(f, " 0,");
    }
    fprintf(f, " 0\n");
    fprintf(f, ".globl main\n");
    fprintf(f, ".text\n");
    fprintf(f, "write:\n");
    fprintf(f, "  li $v0, 1\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  li $v0, 4\n");
    fprintf(f, "  la $a0, _ret\n");
    fprintf(f, "  syscall\n");
    fprintf(f, "  move $v0, $0\n");
    fprintf(f, "  jr $ra\n");
}

void printObjectCode(Code *code)
{
    initVsym(curvar, curtemp);
    FILE *f = fopen("out.s", "w+");
    initCode(f);
    while (code != NULL)
    {
        char temp[16];
        switch (code->ctype) {
        case LABEL:
            sscanf(code->sentense, "%*s %s %*s", temp);
            fprintf(f, "%s :\n", temp);
            break;
        case ASSIGN:
            tran_assign(code, f);
            break;
        case PLUS:
            tran_plus(code, f);
            break;
        case MINUS:
            tran_minus(code, f);
            break;
        case MULTI:
            tran_multi(code, f);
            break;
        case DIV:
            tran_div(code, f);
            break;
        case GOTO:
            sscanf(code->sentense, "%*s %s", temp);
            fprintf(f, "  j %s\n", temp);
            break;
        case ARG:
            tran_arg(code, f);
            break;
        case CALL:
            sscanf(code->sentense, "%*s %s", temp);
            fprintf(f, "  addi $sp, $sp, -4\n");
            fprintf(f, "  sw $ra, 0($sp)\n");
            fprintf(f, "  jal %s\n", temp);
            fprintf(f, "  lw $ra, 0($sp)\n");
            fprintf(f, "  addi $sp, $sp, 4\n");
            curarg = 0;
            break;
        case CALLASSIGN:
            tran_callassign(code, f);
            break;
        case RETURN:
            tran_return(code, f);
            break;
        case FUNC:
            sscanf(code->sentense, "%*s %s %*s", temp);
            fprintf(f, "%s :\n", temp);
            if (!strcmp(temp, "main")) {
                fprintf(f, "la $s0, var\n");
            }
            break;
        case PARAM:
            tran_param(code, f);
            break;
        case IF:
            tran_if(code, f);
            break;
        case WRITE:
            tran_write(code, f);
            break;
        default:
            fprintf(f, "  * error\n");
            break;
        }
        code = code->next;
    }
    fclose(f);
}



