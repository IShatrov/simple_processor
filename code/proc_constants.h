#ifndef PROC_CONSTANTS
#define PROC_CONSTANTS

#define SIGNATURE 0xDED
#define VERSION 2

//enum commands
//{
//    CMD_HALT = 0,
//    CMD_PUSH = 1,
//    CMD_POP  = 2,
//    CMD_ADD  = 3,
//    CMD_SUB  = 4,
//    CMD_MUL  = 5,
//    CMD_DIV  = 6,
//    CMD_OUT  = 7,
//    CMD_IN   = 8,
//    CMD_DUP  = 9,
//    CMD_CALL = 10,
//    CMD_RET  = 11,
//    CMD_SQR  = 12,
//
//    CMD_JMP  = 13,
//    CMD_JB   = 14,
//    CMD_JBE  = 15,
//    CMD_JA   = 16,
//    CMD_JAE  = 17,
//    CMD_JE   = 18,
//    CMD_JNE  = 19,
//
//    CMD_DUMP = -1,
//    CMD_CONT = -2,
//};

enum reg_n
{
    REG_RAX = 1,
    REG_RBX = 2,
    REG_RCX = 3,
    REG_RDX = 4,
};

enum arg_types
{
    N_IMMED = 1,
    N_REG   = 2,
    N_RAM   = 3,
};

#endif
