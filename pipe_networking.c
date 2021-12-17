#include "pipe_networking.h"

/*=========================
	server_fork
	args: int *from_client, int *to_client 
	Preforms 3 way handshake, starting on the main server
    and continuing on a subserver. It alters the from_client
    and to_client pointers to direct towards the pipe
    and returns the value of the fork() call made.

    IMPORTANT: THIS FUCNTION CALLS fork()
	=========================*/
int server_fork(int *from_client, int *to_client) {
	int mk_err = mkfifo(WKP, 0644);
	if (mk_err)
	{
		printf("Couldn't create fifo: %s\n", strerror(errno));
		return -1;
	} 

	*from_client = open(WKP, O_RDONLY);
    if (remove(WKP))
    {
        printf("Couldn't open WKP fifo: %s\n", strerror(errno));
        return -1;
    }
    
    int c = fork();

    if (c)
    {
        return c;
    }

	if (*from_client == -1)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	char SKP[HANDSHAKE_BUFFER_SIZE];
	if (read(*from_client, SKP, HANDSHAKE_BUFFER_SIZE) == -1)
	{
		printf("Couldn't read from WKP fifo: %s\n", strerror(errno));
		return -1;
	}

	*to_client = open(SKP, O_WRONLY);
	if (*to_client == -1)
	{
		printf("Couldn't open SKP fifo: %s\n", strerror(errno));
		return -1;
	}

	if (write(*to_client, ACK, BUFFER_SIZE) == -1)
	{
		printf("Couldn't write to SKP fifo: %s\n", strerror(errno));
		return -1;
	}
    
    char recieved[BUFFER_SIZE];
	if (read(*from_client, recieved, BUFFER_SIZE) == -1 || strcmp(recieved, ACK))
	{
		printf("Couldn't read from WKP fifo: %s\n", strerror(errno));
		return -1;
	}

	return c;
}

/*=========================
	server_handshake
	args: int * to_client
	Performs the client side pipe 3 way handshake.
	Sets *to_client to the file descriptor to the downstream pipe.
	returns the file descriptor for the upstream pipe.
	=========================*/
int server_handshake(int *to_client) {
	int from_client = 0;

	int mk_err = mkfifo(WKP, 0644);
	if (mk_err)
	{
		printf("%s\n", strerror(errno));
		return -1;
	} 

	from_client = open(WKP, O_RDONLY);
    if (remove(WKP))
    {
        printf("%s\n", strerror(errno));
        return -1;
    }

	if (from_client == -1)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	char SKP[HANDSHAKE_BUFFER_SIZE];
	if (read(from_client, SKP, HANDSHAKE_BUFFER_SIZE) == -1)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	*to_client = open(SKP, O_WRONLY);
	if (*to_client == -1)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	if (write(*to_client, ACK, BUFFER_SIZE) == -1)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}
    
    char recieved[BUFFER_SIZE];
	if (read(from_client, recieved, BUFFER_SIZE) == -1 || strcmp(recieved, ACK))
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	return from_client;
}


/*=========================
	client_handshake
	args: int * to_server
	Performs the client side pipe 3 way handshake.
	Sets *to_server to the file descriptor for the upstream pipe.
	returns the file descriptor for the downstream pipe.
	=========================*/
int client_handshake(int *to_server) {
	int from_server = 0;

	*to_server = open(WKP, O_WRONLY);
	if (*to_server == -1)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	char SKP[HANDSHAKE_BUFFER_SIZE];
	sprintf(SKP, "%d", getpid());

	int mk_err = mkfifo(SKP, 0644);
	if (mk_err)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	if (write(*to_server, SKP, HANDSHAKE_BUFFER_SIZE) == -1)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	from_server = open(SKP, O_RDONLY);
    if (remove(SKP))
    {
        printf("%s\n", strerror(errno));
        return -1;
    }

	if (from_server == -1)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	char recieved[BUFFER_SIZE];
	if (read(from_server, recieved, BUFFER_SIZE) == -1 || strcmp(recieved, ACK))
	{
		printf("%s\n", strerror(errno));
		return -1;
	}

	if (write(*to_server, ACK, BUFFER_SIZE) == -1)
	{
		printf("%s\n", strerror(errno));
		return -1;
	}
    
	return from_server;
}