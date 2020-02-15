// linux build: gcc -mbmi -O3 catcher.c -o catcher.out
#include <x86intrin.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <err.h>

void posix_signal_handler(int sig, siginfo_t *siginfo, void *context)
{
	(void)context;
	switch(sig)
	{
		case SIGSEGV:
			fputs("Caught SIGSEGV: Segmentation Fault\n", stderr);
			break;
		case SIGINT:
			fputs("Caught SIGINT: Interactive attention signal, (usually ctrl+c)\n", stderr);
			break;
		case SIGFPE:
			switch(siginfo->si_code)
			{
				case FPE_INTDIV:
					fputs("Caught SIGFPE: (integer divide by zero)\n", stderr);
					break;
				case FPE_INTOVF:
					fputs("Caught SIGFPE: (integer overflow)\n", stderr);
					break;
				case FPE_FLTDIV:
					fputs("Caught SIGFPE: (floating-point divide by zero)\n", stderr);
					break;
				case FPE_FLTOVF:
					fputs("Caught SIGFPE: (floating-point overflow)\n", stderr);
					break;
				case FPE_FLTUND:
					fputs("Caught SIGFPE: (floating-point underflow)\n", stderr);
					break;
				case FPE_FLTRES:
					fputs("Caught SIGFPE: (floating-point inexact result)\n", stderr);
					break;
				case FPE_FLTINV:
					fputs("Caught SIGFPE: (floating-point invalid operation)\n", stderr);
					break;
				case FPE_FLTSUB:
					fputs("Caught SIGFPE: (subscript out of range)\n", stderr);
					break;
				default:
					fputs("Caught SIGFPE: Arithmetic Exception\n", stderr);
					break;
			}
		case SIGILL:
			switch(siginfo->si_code)
			{
				case ILL_ILLOPC:
					fputs("Caught SIGILL: (illegal opcode)\n", stderr);
					break;
				case ILL_ILLOPN:
					fputs("Caught SIGILL: (illegal operand)\n", stderr);
					fputs("Check if it's MBI - replace it!\n", stderr);
					break;
				case ILL_ILLADR:
					fputs("Caught SIGILL: (illegal addressing mode)\n", stderr);
					break;
				case ILL_ILLTRP:
					fputs("Caught SIGILL: (illegal trap)\n", stderr);
					break;
				case ILL_PRVOPC:
					fputs("Caught SIGILL: (privileged opcode)\n", stderr);
					break;
				case ILL_PRVREG:
					fputs("Caught SIGILL: (privileged register)\n", stderr);
					break;
				case ILL_COPROC:
					fputs("Caught SIGILL: (coprocessor error)\n", stderr);
					break;
				case ILL_BADSTK:
					fputs("Caught SIGILL: (internal stack error)\n", stderr);
					break;
				default:
					fputs("Caught SIGILL: Illegal Instruction\n", stderr);
					break;
			}
			break;
		case SIGTERM:
			fputs("Caught SIGTERM: a termination request was sent to the program\n", stderr);
			break;
		case SIGABRT:
			fputs("Caught SIGABRT: usually caused by an abort() or assert()\n", stderr);
			break;
		default:
			break;
	}
	_Exit(1);
}

static uint8_t alternate_stack[SIGSTKSZ];
void set_signal_handler()
{
	/* setup alternate stack */
	{
		stack_t ss = {};
		/* malloc is usually used here, I'm not 100% sure my static allocation
		   is valid but it seems to work just fine. */
		ss.ss_sp = (void*)alternate_stack;
		ss.ss_size = SIGSTKSZ;
		ss.ss_flags = 0;

		if (sigaltstack(&ss, NULL) != 0) { err(1, "sigaltstack"); }
	}

	/* register our signal handlers */
	{
		struct sigaction sig_action = {};
		sig_action.sa_sigaction = posix_signal_handler;
		sigemptyset(&sig_action.sa_mask);

		#ifdef __APPLE__
			/* for some reason we backtrace() doesn't work on osx
			   when we use an alternate stack */
			sig_action.sa_flags = SA_SIGINFO;
		#else
			sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;
		#endif

		if (sigaction(SIGSEGV, &sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGFPE,  &sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGINT,  &sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGILL,  &sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGTERM, &sig_action, NULL) != 0) { err(1, "sigaction"); }
		if (sigaction(SIGABRT, &sig_action, NULL) != 0) { err(1, "sigaction"); }
	}
}

void mbi_andn(unsigned int a, unsigned int b)
{
	unsigned int c;
	printf("a=%d, b=%d\n", a, b);
#ifdef NOBMI
	c = ~a & b;
#else
	c = __andn_u64(a, b);
#endif
	printf("__andn_u64(%d, %d)=%d\n", a, b, c);
}

void cause_calamity(unsigned int a, unsigned int b)
{
	mbi_andn(a, b);
}

int main(int argc, char * argv[]){
	(void)argc;
	unsigned int a = strtol(argv[1], NULL, 10), b = strtol(argv[2], NULL, 10);

	set_signal_handler();
	cause_calamity(a, b);
	puts("OMG! Nothing bad happend!");
	
	return 0;
}
