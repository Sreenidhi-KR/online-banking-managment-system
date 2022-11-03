#ifndef AUTH
#define AUTH

#include "admin_actions.h"
#include "client_actions.h"
#include <sys/sem.h>

bool attemptUserLogin(int sd, int user_type)
{
  bool result;
  user currUser;

  printf("User ID : ");

  while (scanf("%d", &currUser.userID) != 1 || getchar() != '\n')
  {
    scanf("%*[^\n]%*c");
    printf("you must enter an integer: ");
  }
  char *pass = getpass("Password: ");
  memcpy(currUser.password, pass, sizeof(pass));
  // to the server
  currUser.user_type = user_type;
  write(sd, &currUser, sizeof(user));

  read(sd, &result, sizeof(result)); // from the server
  if (!result)
  {
    return result;
  }
  else
  {
    printf("\n\t***Succesfully logged in!!***\n\n");
    key_t semKey = ftok("Users", currUser.userID); // Generate a key based on the account number hence, different customers will have different semaphores
    union semun
    {
      int val; // Value of the semaphore
    } semSet, semSet1, semSet2;

    int semctlStatus;
    semIdentifier = semget(semKey, 1, 0); // Get the semaphore if it exists
    if (semIdentifier == -1)
    {
      semIdentifier = semget(semKey, 1, IPC_CREAT | 0700); // Create a new semaphore
      if (semIdentifier == -1)
      {
        perror("Error while creating semaphore!");
        _exit(1);
      }

      semSet.val = 1; // Set a binary semaphore
      semctlStatus = semctl(semIdentifier, 0, SETVAL, semSet);
      if (semctlStatus == -1)
      {
        perror("Error while initializing a binary sempahore!");
        _exit(1);
      }
    }
    // Semaphore - Withdraw
    int semctlStatus1;
    semIdentifier1 = semget(semKey, 1, 0); // Get the semaphore if it exists
    if (semIdentifier1 == -1)
    {
      semIdentifier1 = semget(semKey, 1, IPC_CREAT | 0700); // Create a new semaphore
      if (semIdentifier1 == -1)
      {
        perror("Error while creating semaphore!");
        _exit(1);
      }

      semSet1.val = 1; // Set a binary semaphore
      semctlStatus1 = semctl(semIdentifier1, 0, SETVAL, semSet1);
      if (semctlStatus1 == -1)
      {
        perror("Error while initializing a binary sempahore!");
        _exit(1);
      }
    }
    return true;
  }
}

bool attempt_admin_login(int sd)
{
  bool result;
  admin a;
  printf("ADMIN ID : \n");

  while (scanf("%d", &a.userID) != 1 || getchar() != '\n')
  {
    scanf("%*[^\n]%*c");
    printf("you must enter an integer: ");
  }
  printf("ADMIN Password : \n");
  while (scanf("%d", &a.password) != 1 || getchar() != '\n')
  {
    scanf("%*[^\n]%*c");
    printf("you must enter an integer: ");
  }
  write(sd, &a, sizeof(admin));

  read(sd, &result, sizeof(result)); // from the server

  return result;
}

void attempt_login(int sd)
{
  int choice;
  bool result;
  printf("\e[1;1H\e[2J");
  printf("1 : Normal-User Login\n");
  printf("2 : Joint Account-User Login\n");
  printf("3 : Admin Login\n");
  printf("4 : Back\n");
  printf("Choose an option : \n");
  while (scanf("%d", &choice) != 1 || getchar() != '\n')
  {
    scanf("%*[^\n]%*c");
    printf("you must enter an integer: ");
  }
  write(sd, &choice, sizeof(int));
  switch (choice)
  {
  case 1:
  case 2:
    result = attemptUserLogin(sd, choice);
    if (!result)
    {
      printf("\e[1;1H\e[2J");
      printf("\n\t\t\t+++Invalid Credintials!!+++\n\n");
      return;
    }
    else
    {
      printf("\e[1;1H\e[2J");
      printf("\n\t\t\t+++Succesfully logged in!!+++\n\n");
      show_client_menu(sd);
    }
    break;
  case 3:
    result = attempt_admin_login(sd);
    if (!result)
    {
      printf("\e[1;1H\e[2J");
      printf("\n\t\t\t+++Invalid Credintials!!+++\n\n");
      return;
    }
    else
    {
      printf("\e[1;1H\e[2J");
      printf("\n\t\t\t+++Succesfully logged in!!+++\n\n");
      show_admin_menu(sd);
    }
  case 4:
    printf("\e[1;1H\e[2J");
    return;
  default:
    printf("\e[1;1H\e[2J");
    printf("Invalid option!!\n\n");
    return;
  }
  return;
}

#endif