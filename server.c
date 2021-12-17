#include <signal.h>

#include "pipe_networking.h"

void process(char *);
void quick(char *, int, int);
int partition(char *, int, int);
void swap(char *, int, int);

static void sighandler(int signo)
{
	if (signo == SIGINT)
	{
		remove(WKP);
		exit(0);
	}
}

int main() {
	signal(SIGINT, sighandler);

	int to_client;
	int from_client;

	while (1)
	{
		from_client = server_handshake(&to_client);
		if (from_client == -1)
		{
			return -1;
		}

		printf("New client entered\n");
		
		char input[BUFFER_SIZE];
		while (read(from_client, input, BUFFER_SIZE) > 0)
		{
			process(input);
			
			if (write(to_client, input, BUFFER_SIZE) == -1)
			{
				printf("%s\n", strerror(errno));
			}
		}

		printf("Client exited\n");
	}

	return 0;
}

void process(char *input)
{
	while (strlen(input) > 1 && input[strlen(input) - 1] == '\n')
	{
		input[strlen(input) - 1] = '\0';
	}

	quick(input, 0, strlen(input));
}

void quick(char *input, int l, int r)
{
	if (l >= r)
	{
		return;
	}

	int ind = partition(input, l, r);
	quick(input, l, ind);
	quick(input, ind + 1, r);
}

int partition(char *input, int l, int r)
{
	int index = l;
	for (int i = l; i < r - 1; i++)
	{
		if (input[i] < input[r - 1])
		{
			swap(input, i, index);
			index++;
		}
	}

	swap(input, index, r - 1);
	return index;
}

void swap(char *input, int a, int b)
{
	char temp = input[a];
	input[a] = input[b];
	input[b] = temp;
}