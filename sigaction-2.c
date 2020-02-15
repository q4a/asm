// linux build: gcc sigaction-2.c -o sigaction-2.out
// ./sigaction-2.out 13 3
#define _GNU_SOURCE /* Bring REG_XXX names from /usr/include/sys/ucontext.h */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ucontext.h>

static void sigaction_segv(int signal, siginfo_t *si, void *arg)
{
	ucontext_t *ctx = (ucontext_t *)arg;

	/* We are on linux x86, the returning IP is stored in RIP (64bit) or EIP (32bit).
	   In this example, the length of the offending instruction is 6 bytes.
	   So we skip the offender ! */
	#if __WORDSIZE == 64
		printf("Caught SIGSEGV, addr %p, RIP 0x%lx\n", si->si_addr, ctx->uc_mcontext.gregs[REG_RIP]);
		ctx->uc_mcontext.gregs[REG_RIP] += 6;
	#else
		printf("Caught SIGSEGV, addr %p, EIP 0x%x\n", si->si_addr, ctx->uc_mcontext.gregs[REG_EIP]);
		ctx->uc_mcontext.gregs[REG_EIP] += 6;
	#endif
}

void mbi_andn(unsigned int a, unsigned int b)
{
	unsigned int c = 0;
	printf("a=%d, b=%d\n", a, b);
#ifdef NOBMI
	c = ~a & b;
#else
	//c = __andn_u64(a, b);
	*(int *)NULL = 0;
#endif
	printf("__andn_u64(%d, %d)=%d\n", a, b, c);
}

void cause_calamity(unsigned int a, unsigned int b)
{
	mbi_andn(a, b);
}

int main(int argc, char * argv[])
{
	unsigned int a = strtol(argv[1], NULL, 10), b = strtol(argv[2], NULL, 10);

	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = sigaction_segv;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &sa, NULL);

	/* Generate a seg fault */
	//*(int *)NULL = 0;
	cause_calamity(a, b);

	printf("Back to normal execution.\n");

	return 0;
}
