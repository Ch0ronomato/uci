#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int sigint_count;
int sigquit_count;
int sigtermstop_count;
const int STOP_COUNT=3;
struct sigaction act;

void sig_handler(int signum, siginfo_t *info, void *ptr)
{
	switch (signum) {
		case SIGINT:
			sigint_count++;
			printf("I");
			fflush(stdout);
			break;
		case SIGQUIT:
			sigquit_count++;
			printf("Q");
			fflush(stdout);
			break;
		case SIGTSTP:
			sigtermstop_count++;
			printf("S");
			fflush(stdout);
			break;
	}
	if (sigtermstop_count == STOP_COUNT) {
		printf("\nInterrupt: %d\nStop: %d\nQuit: %d\n", sigint_count, sigquit_count, sigtermstop_count);
		exit(0);
	}
}

int main() 
{
	// using the sigaction manner.
	memset(&act, 0, sizeof(act));

	act.sa_sigaction = sig_handler;
	act.sa_flags = SA_SIGINFO; // setting this will package an additional 
	// data package containing some neat info (pid, time, etc)

	// set our constants.
	sigint_count = 0;
	sigquit_count = 0;
	sigtermstop_count = 0;


	sigaction(SIGINT, &act, NULL);
	sigaction(SIGQUIT, &act, NULL);
	sigaction(SIGTSTP, &act, NULL);

	while (1) { sleep(1); }
	return 0;
}
