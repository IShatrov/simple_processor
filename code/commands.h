DEF_CMD(HALT, 0, 0,
{
    ip = max_ip;
})

DEF_CMD(push, 1, 1,
{
    if(CHECK_MASK(N_REG) && CHECK_MASK(N_RAM))
    {
        PUSH(RAM[(int)regs[(int)code[ip + 1]] + (int)code[ip + 2]]);

        ip += 2;
    }
    else if(CHECK_MASK(N_IMMED))//immediate argument
    {
        PUSH(code[++ip]);
    }
    else if(CHECK_MASK(N_RAM))//RAM
    {
        PUSH(RAM[(int)code[++ip]]);
    }
    else if(CHECK_MASK(N_REG))//register
    {
        PUSH(regs[(int)code[++ip]]);
    }
})

DEF_CMD(pop, 2, 1,
{
    if(CHECK_MASK(N_REG) && CHECK_MASK(N_RAM))
    {
        RAM[(int)regs[(int)code[ip + 1]] + (int)code[ip + 2]] = POP;

        ip += 2;
    }
    else if(CHECK_MASK(N_RAM))//RAM
    {
        RAM[(int)code[++ip]] = POP;
    }
    else if(CHECK_MASK(N_REG))//register
    {
        regs[(int)code[++ip]] = POP;
    }
})

DEF_CMD(add, 3, 0,
{
    PUSH(POP + POP);
})

DEF_CMD(sub, 4, 0,
{
    arg = POP;
    PUSH(POP - arg);
})

DEF_CMD(mul, 5, 0,
{
    PUSH(POP * POP);
})

DEF_CMD(div, 6, 0,
{
    arg = POP;
    if(arg) PUSH(POP / arg);
    else
    {
        printf("error: zero division");
        ip = max_ip;
    }
})

DEF_CMD(out, 7, 0,
{
    printf("%lf\n", POP);
})

DEF_CMD(in, 8, 0,
{
    if(scanf("%lf", &arg)) PUSH(arg);
})

DEF_CMD(dup, 9, 0,
{
    arg = POP;
    PUSH(arg);
    PUSH(arg);
})

DEF_CMD(call, 10, 1,
{
    stk_push(&ret_address_stk, ip + 2);
    ip = code[ip + 1] - 1; //for-loop will increase ip
})

DEF_CMD(ret, 11, 0,
{
    ip = stk_pop(&ret_address_stk) - 1;//for-loop will increase ip
})

DEF_CMD(sqr, 12, 0,
{
    PUSH(sqrt(POP));
})

DEF_CMD(jmp, 13, 1,
{
    DO_JUMP(1);
})

DEF_CMD(jb, 14, 1,
{
    arg = POP;
    DO_JUMP(POP < arg);
})

DEF_CMD(jbe, 15, 1,
{
    arg = POP;
    DO_JUMP(POP <= arg);
})

DEF_CMD(ja, 16, 1,
{
    arg = POP;
    DO_JUMP(POP > arg);
})

DEF_CMD(jae, 17, 1,
{
    arg = POP;
    DO_JUMP(POP >= arg);
})

DEF_CMD(je, 18, 1,
{
    arg = POP;
    DO_JUMP(are_doubles_equal(POP, arg));
})

DEF_CMD(jne, 19, 1,
{
    arg = POP;

    DO_JUMP(!are_doubles_equal(POP, arg));
})

DEF_CMD(sin, 20, 0,
{
    PUSH(sin(POP));
})

DEF_CMD(cos, 21, 0,
{
    PUSH(cos(POP));
})

DEF_CMD(vid, 22, 0,
{
    for(int i = 0; i < VID_HEIGHT; i++)
    {
        for(int j = 0; j < VID_WIDTH; j++)
        {
            if(RAM[RAM_SIZE + i*VID_HEIGHT + j + 1]) putchar('*');
            else putchar('_');
        }
    putchar('\n');
    }
    putchar('\n');
})

DEF_CMD(dump, -1, 0,
{
    stk_dump(stk.log, &stk, stk.info.line, stk.info.file_name, stk.info.func_name, 0);
    for(int i = 0; i < max_ip; i++) printf("%-5.2lf ", code[i]);
    putchar('\n');
    for(int i = 0; i < ip; i++) printf("      ");
    printf("^ ip = %d\n", ip);
})

DEF_CMD(cont, -2, 0,
{
    ;
})
