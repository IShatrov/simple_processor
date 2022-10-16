DEF_CMD(HALT, 0, 0,
{
    ip = max_ip;
})

DEF_CMD(PUSH, 1, 1,
{
    if(*((int*)(code + ip) + 1) == N_IMMED)//immediate argument
    {
        stk_push(&stk, code[++ip]);
    }
    else if(*((int*)(code + ip) + 1) == N_RAM)//RAM
    {
        stk_push(&stk, RAM[(int)code[++ip]]);
    }
    else if(*((int*)(code + ip) + 1) == N_REG)//register
    {
        stk_push(&stk, regs[(int)code[++ip]]);
    }
})

DEF_CMD(POP, 2, 1,
{
    if(*((int*)(code + ip) + 1) == N_RAM)//RAM
    {
        RAM[(int)code[++ip]] = stk_pop(&stk);
    }
    else if(*((int*)(code + ip) + 1) == N_REG)//register
    {
        regs[(int)code[++ip]] = stk_pop(&stk);
    }
})

DEF_CMD(ADD, 3, 0,
{
    stk_push(&stk, stk_pop(&stk) + stk_pop(&stk));
})

DEF_CMD(SUB, 4, 0,
{
    arg = stk_pop(&stk);
    stk_push(&stk, stk_pop(&stk) - arg);
})

DEF_CMD(MUL, 5, 0,
{
    stk_push(&stk, stk_pop(&stk) * stk_pop(&stk));
})

DEF_CMD(DIV, 6, 0,
{
    arg = stk_pop(&stk);
    if(arg) stk_push(&stk, stk_pop(&stk) / arg);
    else
    {
        printf("error: zero division");
        ip = max_ip;
    }
})

DEF_CMD(OUT, 7, 0,
{
    printf("%lf\n", stk_pop(&stk));
})

DEF_CMD(IN, 8, 0,
{
    if(scanf("%lf", &arg)) stk_push(&stk, arg);
})

DEF_CMD(DUP, 9, 0,
{
    arg = stk_pop(&stk);
    stk_push(&stk, arg);
    stk_push(&stk, arg);
})

DEF_CMD(CALL, 10, 1,
{
    stk_push(&ret_address_stk, ip + 2);
    ip = code[ip + 1] - 1; //for-loop will increase ip
})

DEF_CMD(RET, 11, 0,
{
    ip = stk_pop(&ret_address_stk) - 1;//for-loop will increase ip
})

DEF_CMD(SQR, 12, 0,
{
    stk_push(&stk, sqrt(stk_pop(&stk)));
})

DEF_CMD(JMP, 13, 1,
{
    ip = code[ip + 1] - 1; //for-loop will increase ip
})

DEF_CMD(JB, 14, 1,
{
    arg = stk_pop(&stk);
    if(stk_pop(&stk) < arg)
    {
        ip = code[ip + 1] - 1; //for-loop will increase ip
    }
    else ip++;
})

DEF_CMD(JBE, 15, 1,
{
    arg = stk_pop(&stk);
    if(stk_pop(&stk) <= arg)
    {
        ip = code[ip + 1] - 1; //for-loop will increase ip
    }
    else ip++;
})

DEF_CMD(JA, 16, 1,
{
    arg = stk_pop(&stk);
    if(stk_pop(&stk) > arg)
    {
        ip = code[ip + 1] - 1; //for-loop will increase ip
    }
    else ip++;
})

DEF_CMD(JAE, 17, 1,
{
    arg = stk_pop(&stk);
    if(stk_pop(&stk) >= arg)
    {
        ip = code[ip + 1] - 1; //for-loop will increase ip
    }
    else ip++;
})

DEF_CMD(JE, 18, 1,
{
    arg = stk_pop(&stk);
    if(are_doubles_equal(stk_pop(&stk), arg))
    {
        ip = code[ip + 1] - 1; //for-loop will increase ip
    }
    else ip++;
})

DEF_CMD(JNE, 19, 1,
{
    arg = stk_pop(&stk);

    if(!are_doubles_equal(stk_pop(&stk), arg))
    {
        ip = code[ip + 1] - 1; //for-loop will increase ip
    }
    else ip++;
})

DEF_CMD(DUMP, -1, 0,
{
    stk_dump(stk.log, &stk, stk.info.line, stk.info.file_name, stk.info.func_name, 0);
    for(int i = 0; i < max_ip; i++) printf("%-5.2lf ", code[i]);
    putchar('\n');
    for(int i = 0; i < ip; i++) printf("      ");
    printf("^ ip = %d\n", ip);
})

DEF_CMD(CONT, -2, 0,
{
    ;
})
