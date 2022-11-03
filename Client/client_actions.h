#ifndef CLIENT_ACTIONS
#define CLIENT_ACTIONS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/sem.h>
#include "structures.h"
#include "authentication.h"

int semIdentifier, semIdentifier1;
bool lock_critical_section(struct sembuf *semOp, int semIdentifier)
{
  semOp->sem_flg = SEM_UNDO;
  semOp->sem_op = -1;
  semOp->sem_num = 0;
  int semopStatus = semop(semIdentifier, semOp, 1);
  if (semopStatus == -1)
  {
    perror("Error while locking critical section");
    return false;
  }
  return true;
}

bool unlock_critical_section(struct sembuf *semOp, int semIdentifier)
{
  semOp->sem_op = 1;
  int semopStatus = semop(semIdentifier, semOp, 1);
  if (semopStatus == -1)
  {
    perror("Error while operating on semaphore!");
    _exit(1);
  }
  return true;
}

void deposit(int sd)
{
  int amt;
  bool result;
  struct sembuf semOp;
  printf("WAITING .............\n");
  lock_critical_section(&semOp, semIdentifier);
  printf("\e[1;1H\e[2J");
  printf("Amount to Deposit : Rs.\n");

  while (scanf("%d", &amt) != 1 || amt <= 0 || getchar() != '\n')
  {
    scanf("%*[^\n]%*c");
    printf("you must enter an integer: ");
  }

  write(sd, &amt, sizeof(int));

  read(sd, &result, sizeof(result));
  unlock_critical_section(&semOp, semIdentifier);
  if (!result)
  {
    printf("\n\t***Error depositing your money!!***\n\n");
  }
  else
  {
    printf("\n\t***Succesfully deposited!!***\n\n");
  }

  return;
}

void withdraw(int sd)
{
  int amt;
  bool result;
  struct sembuf semOp1;
  printf("WAITING .............\n");
  lock_critical_section(&semOp1, semIdentifier1);
  printf("\e[1;1H\e[2J");
  printf("\nAmount to Withdraw : Rs.");
  while (scanf("%d", &amt) != 1 || amt <= 0 || getchar() != '\n')
  {
    scanf("%*[^\n]%*c");
    printf("you must enter an integer: ");
  }

  fflush(stdin);
  write(sd, &amt, sizeof(int));

  read(sd, &result, sizeof(result));
  unlock_critical_section(&semOp1, semIdentifier1);
  if (!result)
  {
    printf("\n\t***Insufficient balance!!***\n\n");
  }
  else
  {
    printf("\n\t***Succesfully withdrew!!***\n\n");
  }

  return;
}

void balance_enquiry(int sd)
{
  int amt;
  read(sd, &amt, sizeof(int));

  printf("\nAvailable Balance :: Rs.");
  printf("%d\n", amt);

  return;
}

void change_password(int sd)
{
  char passwd[30];
  bool result;
  char *pass = getpass("\nEnter new password: ");
  memcpy(passwd, pass, sizeof(pass));
  write(sd, passwd, sizeof(passwd));
  read(sd, &result, sizeof(result));
  if (!result)
  {
    printf("\n\t+++Error changing your password+++\n\n");
  }
  else
  {
    printf("\n\t+++Succesfully changed your password+++\n\n");
  }

  return;
}

void view_user_details(int sd)
{

  user u;
  read(sd, &u, sizeof(user));

  if (u.user_type == 1)
  {
    printf("Account Holder's Name : %s\n", u.name1);
  }
  else
  {
    printf("First Account Holder's Name : %s\n", u.name1);
    printf("Second Account Holder's Name : %s\n", u.name1);
  }
  printf("User ID : %d\n", u.userID);
  printf("Available Balance : Rs.%d\n", u.balance);
  printf("Status : %s\n\n", u.status);

  return;
}
void view_transaction(int sd)
{
  int end;
  int index = 0;
  read(sd, &end, sizeof(end));
  if (end == -1)
  {
    printf("No Transactions \n");
  }
  else
  {
    printf("No \t Amount \t Balance \n");
  }
  while (end != -1)
  {
    transaction t;
    read(sd, &t, sizeof(t));
    if (t.amount > 0)
    {
      printf("%d \t +%d \t \t %d \n", ++index, t.amount, t.curr_bal);
    }
    else
    {
      printf("%d \t %d \t \t %d \n", ++index, t.amount, t.curr_bal);
    }
    read(sd, &end, sizeof(end));
  }
  printf("\n\n");
  return;
}

void show_client_menu(int sd)
{
  int choice;
  while (1)
  {
    printf("1 : Deposit\n");
    printf("2 : Withdraw\n");
    printf("3 : Balance Enquiry\n");
    printf("4 : Password Change\n");
    printf("5 : View Details\n");
    printf("6 : View Transaction\n");
    printf("7 : Exit\n");
    printf("Choose an option : \n");
    while (scanf("%d", &choice) != 1 || getchar() != '\n')
    {
      scanf("%*[^\n]%*c");
      printf("you must enter an integer: ");
    }
    printf("\e[1;1H\e[2J");
    write(sd, &choice, sizeof(choice));
    switch (choice)
    {
    case 1:
      deposit(sd);
      break;
    case 2:
      withdraw(sd);
      break;
    case 3:
      balance_enquiry(sd);
      break;
    case 4:
      change_password(sd);
      break;
    case 5:
      view_user_details(sd);
      break;
    case 6:
      view_transaction(sd);
      break;
    case 7:
      printf("\n\n\t\t*********Thank You*********\n\n\n");
      exit(0);
    default:
      printf("\e[1;1H\e[2J");
      printf("Invalid option!!\n\n");

      break;
    }
  }
}

#endif