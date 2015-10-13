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
		case SIGUSR2:
			printf("I got message! %ld\n", pid);
			sig_count++;
			break;
	}
	if (sig_count >= 10) {
		printf("Sending sigint to %ld\n", pid);
		kill(pid, SIGINT);
	}
}

int main(int argc, char** argv) 
{
	sig_count = 0;
	// using the sigaction manner.
	memset(&act, 0, sizeof(act));

	act.sa_sigaction = sig_handler;
	act.sa_flags = SA_SIGINFO; // setting this will package an additional 
	// data package containing some neat info (pid, time, etc)

	// set our constants.
	sigaction(SIGUSR2, &act, NULL);

	while (1) { 
		long pid = strtol(argv[argc - 1], NULL, 10);
		kill(pid, SIGUSR1); 
		sleep(1); 
	}
	return 0;
}
