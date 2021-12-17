#include "pipe_networking.h"

int main() {
	int to_server;
  	int from_server;

	from_server = client_handshake(&to_server);
	if (from_server == -1)
	{
		return -1;
	}

	char input[1000];
    input[0] = '\0';

	while (1)
	{
		printf("Input message to give to server (or type exit to quit): ");
		fgets(input, sizeof(input), stdin);

		if (strcmp(input, "exit") == 0 || strcmp(input, "exit\n") == 0)
		{
			break;
		}

		write(to_server, input, sizeof(input));
		read(from_server, input, sizeof(input));

		printf("\nMessage recieved from server: %s\n", input);
	}
}