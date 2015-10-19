#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

int sig_count;
struct sigaction act;

unsigned long getpid(siginfo_t *info) {
	return (unsigned long) info->si_pid;
}

void sig_handler(int signum, siginfo_t *info, void *ptr)
{
	unsigned long pid = getpid(info);
	switch (signum) {
		case SIGUSR1:
			sig_count++;
			kill(pid, SIGUSR2);
			break;
		case SIGINT:
			// bail.
			printf("Signals: %d\n", sig_count);
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
	sig_count = 0;

	sigaction(SIGINT, &act, NULL);
	sigaction(SIGUSR1, &act, NULL);

	while (1) { sleep(1); }
	return 0;
}
