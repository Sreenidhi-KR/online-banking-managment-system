
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "admin_actions.h"
#include "authentication.h"

void handle_client(int nsd)
{
  while (1)
  {
    int choice;
    read(nsd, &choice, sizeof(choice));
    switch (choice)
    {
    case 1:
      create_user(nsd);
      break;
    case 2:
      attempt_login(nsd);
      break;
    case 3:
      close(nsd);
      printf("Ended client session.....\n");
      return;
    }
  }
}

int main()
{
  struct sockaddr_in server, client;
  int sd, nsd;
  socklen_t clientLen;
  bool result;
  sd = socket(AF_INET, SOCK_STREAM, 0);

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(8083);

  bind(sd, (struct sockaddr *)&server, sizeof(server));
  listen(sd, 10);

  printf("Waiting for the client.....\n");
  while (1)
  {
    clientLen = sizeof(client);
    nsd = accept(sd, (struct sockaddr *)&client, &clientLen);

    printf("Connected to the client.....\n");
    if (!fork())
    {
      close(sd);
      handle_client(nsd);
      exit(0);
    }
    else
    {
      close(nsd);
    }
  }
  return 0;
}