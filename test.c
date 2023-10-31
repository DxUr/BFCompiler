#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/mman.h>
#include <signal.h>

#define INF(MSG) printf("\033[1m\033[94mInfo: %s at %s:%d\033[0m\n", MSG, __FILE__, __LINE__);

////////////////////////////
//
//   Testing MY JIT IDEA
//



/* Data bus error handler
 * (usually occurs when accessing unmapped addressing space) */
void sigbus_handler (int sig) {
    printf("SIGBUS(%d) recieved.\n", sig);
    exit(0);
}

static void sigsegv_handler (int sig, siginfo_t *si, void *unused) {
    //UNUSED(unused);
    printf("SIGSEGV(%d) recieved at address: 0x%lx.\n", sig, (long) si->si_addr);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
    /* Trap invalid memory related signals. */
    //signal(SIGSEGV, sigsegv_handler);
    //signal(SIGBUS,  sigbus_handler);
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sigsegv_handler;
    if (sigaction(SIGSEGV, &sa, NULL) == -1)
        perror("sigaction");



    char *ptr = mmap(NULL, 1024, PROT_READ | PROT_WRITE | PROT_EXEC
                                       , MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    goto _label;
    label:
    INF("Hello")
    goto exit;
    _label:
    
    printf("pointer:%p, label:%p, _label:%p, the size between the labels:%d\n", ptr, &&label, &&_label, &&_label - &&label);

    unsigned char code[] = {
    0x55,                           //    push   %rbp
    0x48, 0x89, 0xe5,               //    mov    %rsp,%rbp
    0xb8, 0x37, 0x00, 0x00, 0x00,   //    mov    $0x37,%eax
    0xc9,                           //    leaveq
    0xc3                            //    retq
    };

    //memcpy(ptr, code, sizeof(code));
    memcpy(ptr, &&label, &&_label - &&label);
    INF("EXEC...")
    //((unsigned long (*)()) ptr)();
    goto *(void*)ptr;



    exit:
    return 0;
}