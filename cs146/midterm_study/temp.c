#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
int main( void )
{
	int pid;
	printf( "ORIGINAL: PID=%d PPID=%d\n", getpid(), getppid() );
	pid = fork();
	if( pid != 0 )
		printf( "PARENT: PID=%d PPID=%d child=%d\n",
			getpid(), getppid(), pid );
	else
		printf( "CHILD: PID=%d PPID=%d\n", getpid(), getppid() );
	printf( "PID %d terminates.\n\n", getpid() );
	return( 0 );
}
