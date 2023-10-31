#if 0
THIS IS JUST A SIMPLE BF* INTERPITER, I'LL EXTEND IT LATER TO A JIT COMPILER
How Does it work?
First of all the bytecode compiler compile the bf* code to an optmized bytecode // well it will be a binary code
I want to implement a very fast compiler and runner
the generated code actualy will be a serialized code that make the language exterimely fast
I will make some labels for parts of the code that will be the basic of this
on my version you need to specify the lenght of the array with:
#__version__,__size__!
just the first # character will be maintained as a version definition
note the ! mean the start of the program!!


>   62
<   60
+   43
-   45
[   91
]   93
,   44
.   46

because we have just 8 opcodes I'll use native 64 bit comparision to optmize the code


#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>



#ifdef DEBUG
#define ERR(MSG) printf("\033[1m\033[91mError: %s at %s:%d\033[0m\n", MSG, __FILE__, __LINE__);
#define OK(MSG) printf("\033[1m\033[92mOk: %s at %s:%d\033[0m\n", MSG, __FILE__, __LINE__);
#define WRN(MSG) printf("\033[1m\033[93mWarning: %s at %s:%d\033[0m\n", MSG, __FILE__, __LINE__);
#define INF(MSG) printf("\033[1m\033[94mInfo: %s at %s:%d\033[0m\n", MSG, __FILE__, __LINE__);
#define SIG(MSG) printf("\033[1m\033[95mSignal: %s at %s:%d\033[0m\n", MSG, __FILE__, __LINE__);
#define LINE() printf("\033[1m\033[96mLine at %s:%d\033[0m\n", __FILE__, __LINE__);
#define LN LINE();
#else
#define ERR(MSG) ((void)NULL);
#define OK(MSG) ((void)NULL);
#define WRN(MSG) ((void)NULL);
#define INF(MSG) ((void)NULL);
#define SIG(MSG) ((void)NULL);
#define LINE() ((void)NULL);
#define LN  ((void)NULL);
#endif


#define MAX_INS_SIZE 64

char bytecodes[256] = {
    [62] = 0,
    [60] = 1,
    [43] = 2,
    [45] = 3,
    [91] = 4,
    [93] = 5,
    [44] = 6,
    [46] = 7,
};


char bfcode[] = "#100?30000!\
+[-->-[>>+>-----<<]<--<---]>-.>>>+.>>..+++[.>]<<<<.+++.------.<<-.>>>>+.\
\
\
";


int str_to_int(const char* string, const int digits) { // 0 digits means untill the non number character
    int i = 0;
    uint8_t pointer = 0;
    while (digits > 0 ? pointer < digits : 1)
    {
        char byte = string[pointer];
        if (47 < byte && byte < 58)
        {
            uint8_t digit = byte - 48;
            i = i * 10 + digit;
        } else break;
        pointer++;
    }
    return i;
}

int shrink_bfc(char bfc[]) {
    unsigned int r_pointer = 0;
    unsigned int w_pointer = 0;
    char byte = bfc[r_pointer];
    while (byte != 10)
    {
        if (byte == 43 ||
            byte == 44 ||
            byte == 45 ||
            byte == 46 ||
            byte == 60 ||
            byte == 62 ||
            byte == 91 ||
            byte == 93)
        {
            bfc[w_pointer] = bytecodes[byte];
            w_pointer++;
        }
        r_pointer++;
        byte = bfc[r_pointer];
    }

    bfc[w_pointer] = 8;
    return w_pointer;
}

int main(int arg, char** argv) {
    // array, pointer
    char* arr;
    unsigned int p = 0;
    unsigned int pc = 0;
    uint64_t* lp_rip;
    void* ins[] = {
        &&incp,
        &&decp,
        &&incv,
        &&decv,
        &&lpst,
        &&lped,
        &&scan,
        &&prnt,
        &&exit,
    };

    // labels of operations:
    if(0) {
        incp://LN
            p++;
            pc++;
            goto *ins[bfcode[pc]];
        _incp:
        decp://LN
            p--;
            pc++;
            goto *ins[bfcode[pc]];
        _decp:
        incv://LN
            arr[p]++;
            pc++;
            goto *ins[bfcode[pc]];
        _incv:
        decv://LN
            arr[p]--;
            pc++;
            goto *ins[bfcode[pc]];
        _decv:
        lpst://LN
            asm volatile("1: lea 1b(%%rip), %0;": "=a"(lp_rip));
            pc++;
            goto *ins[bfcode[pc]];
        _lpst:
        lped://LN
            goto *lp_rip;
            pc++;
            goto *ins[bfcode[pc]];
        _lped:
        scan://LN
            arr[p] = getchar();
            pc++;
            goto *ins[bfcode[pc]];
        _scan:
        prnt://LN
            putchar(arr[p]);
            pc++;
            goto *ins[bfcode[pc]];
        _prnt:
    }
    // curently i'll skip analyzing
    // parsing the code
    // getting version and size.
    if(bfcode[0] != 35) {
        ERR("No specified version")
        return 1;
    }
    short version = str_to_int(bfcode + 1, 3);
    printf("version:%d\n", version);
    if(version != 100) {
        ERR("Version not supported")
        return 1;
    }
    int arr_size = bfcode[4] == 63? str_to_int(bfcode+5, 0) : 30000;
    arr = malloc(arr_size * sizeof(char));
    // loop on all bytes
    int pointer = 0;
    bool compiling = true;
    #if 0
    // [exec_chain] is an array of instructions  that will putted togather in a dense INS array then jump to this array
    int ins_len = shrink_bfc(bfcode);
    void* exec_chain = malloc(
        ins_len * MAX_INS_SIZE
    );
    memset(exec_chain, 0, ins_len * MAX_INS_SIZE);
    void* exec_end_pointer = exec_chain;

    INF("--BF* code--")
    OK(bfcode)
    printf("fc:%d\n", bfcode[0]);
    while (compiling)
    {
        char byte = bfcode[pointer];
        switch (byte)
        {
        case 10:
            memcpy(exec_end_pointer, &&exit, &&_exit - &&exit);
            compiling = false;
            break;
        case 43:
            memcpy(exec_end_pointer, &&incv, &&_incv - &&incv);
            exec_end_pointer += &&_incv - &&incv;
            break;
        case 44:
            memcpy(exec_end_pointer, &&scan, &&_scan - &&scan);
            exec_end_pointer += &&_scan - &&scan;
            break;
        case 45:
            memcpy(exec_end_pointer, &&decv, &&_decv - &&decv);
            exec_end_pointer += &&_decv - &&decv;
            break;
        case 46:
            memcpy(exec_end_pointer, &&prnt, &&_prnt - &&prnt);
            exec_end_pointer += &&_prnt - &&prnt;
            break;
        case 60:
            memcpy(exec_end_pointer, &&decp, &&_decp - &&decp);
            exec_end_pointer += &&_decp - &&decp;
            break;
        case 62:
            memcpy(exec_end_pointer, &&incp, &&_incp - &&incp);
            exec_end_pointer += &&_incp - &&incp;
            break;
        case 91:
            memcpy(exec_end_pointer, &&lpst, &&_lpst - &&lpst);
            exec_end_pointer += &&_lpst - &&lpst;
            break;
        case 93:
            memcpy(exec_end_pointer, &&lped, &&_lped - &&lped);
            exec_end_pointer += &&_lped - &&lped;
            break;
        default:
            break;
        }
        printf("exec_end_p:%p\n", exec_end_pointer);
        pointer++;
    }
    #endif
    int ins_len = shrink_bfc(bfcode);

    goto *ins[bfcode[pc]];




    INF("Compiled")

    //goto *exec_chain;
    exit:
        INF("Exit...")
        return 0;
    _exit:
}