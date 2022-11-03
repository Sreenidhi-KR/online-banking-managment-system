#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "admin_actions.h"
#include "authentication.h"
#include <ctype.h>

int main()
{
  struct sockaddr_in server;
  int sd;
  sd = socket(AF_INET, SOCK_STREAM, 0);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_port = htons(8083);
  connect(sd, (struct sockaddr *)&server, sizeof(server));
  printf("\e[1;1H\e[2J"); // clear screen
  while (1)
  {

    printf("\n\n\n\t\t\t************* WELCOME ***************\n");
    printf("1. Sign up\n2. Sign in\n3. Exit\n\nchoice:\n");
    int choice;
    while (scanf("%d", &choice) != 1 || getchar() != '\n')
    {
      scanf("%*[^\n]%*c"); // TO CLEAR INPUT BUFFER
      printf("Enter an integer: ");
    }
    write(sd, &choice, sizeof(choice));
    switch (choice)
    {
    case 1:
      sign_up(sd);
      break;
    case 2:
      attempt_login(sd);
      break;
    case 3:
      printf("--------------- THANK YOU ----------------\n");
      exit(0);
      break;
    default:
      printf("\e[1;1H\e[2J");
      printf("Please enter valid choice.\n");
    }
  }
  close(sd);
  return 0;
}
