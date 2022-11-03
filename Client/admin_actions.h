#ifndef ADMIN_ACTIONS
#define ADMIN_ACTIONS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "structures.h"
#include <ctype.h>

void create_user(int type, int sd)
{
  user newUser;
  int id;

  if (type == 1)
  {
    printf("Please enter the name of the account holder: \n");
    scanf(" %[^\n]", newUser.name1);
    strcpy(newUser.name2, "");
  }
  else
  {
    printf("Please enter the name of the first user: \n");
    scanf(" %[^\n]", newUser.name1);
    printf("Please enter the name of the second user: \n");
    scanf(" %[^\n]", newUser.name2);
  }
  char *pass = getpass("Password: \n");
  memcpy(newUser.password, pass, sizeof(pass));
  newUser.user_type = type;
  write(sd, &newUser, sizeof(user));
  read(sd, &id, sizeof(id));
  printf("\e[1;1H\e[2J");
  if (id == -1)
  {
    printf("Error creating user \n");
  }
  else
  {
    printf("Your userID is : %d\n", id);
  }
  return;
}

void sign_up(int sd)
{
  printf("\e[1;1H\e[2J");
  int choice;
  printf("Enter your choice:\n1. Normal user\n2. Joint user\n3.Back \nchoice:\n");
  while (scanf("%d", &choice) != 1 || getchar() != '\n')
  {
    scanf("%*[^\n]%*c");
    printf("you must enter an integer: ");
  }
  switch (choice)
  {
  case 1:
    create_user(choice, sd);
    break;
  case 2:
    create_user(choice, sd);
    break;
  case 3:
    break;
  default:
    printf("\e[1;1H\e[2J");
    printf("Invalid Choice \n");
  }
}

void delete_account(int sd)
{
  int userID;
  bool result;
  printf("Enter User ID : \n");
  while (scanf("%d", &userID) != 1 || getchar() != '\n')
  {
    scanf("%*[^\n]%*c");
    printf("you must enter an integer: ");
  }
  write(sd, &userID, sizeof(int));

  read(sd, &result, sizeof(result)); // from the server

  if (!result)
  {
    printf("\e[1;1H\e[2J");
    printf("\n\tError deleting the account ,please re-check the User ID!!\n\n");
  }
  else
  {
    printf("\e[1;1H\e[2J");
    printf("\n\t***Succesfully deleted the account!!***\n\n");
  }
  return;
}

void modify_account(int sd)
{
  int type;
  bool result;
  user modUser;
  printf("User ID : ");

  while (scanf("%d", &modUser.userID) != 1 || getchar() != '\n')
  {
    scanf("%*[^\n]%*c");
    printf("you must enter an integer: ");
  }
  printf("\n  New account Type(1: Normal Account 2: Joint Account) : \n");
  while (scanf("%d", &type) != 1 || getchar() != '\n')
  {
    scanf("%*[^\n]%*c");
    printf("you must enter an integer: ");
  }
  if (type < 1 || type > 2)
  {
    printf("\e[1;1H\e[2J");
    printf("Invalid Choice \n");
    return;
  }
  modUser.user_type = type;

  printf("New Name of the account holder : \n");
  scanf(" %[^\n]", modUser.name1);

  if (type == 1)
  {
    strcpy(modUser.name2, "");
  }
  else if (type == 2)
  {
    printf("New Name of the other account holder : \n");
    scanf(" %[^\n]", modUser.name2);
  }

  char *pass = getpass("Password: \n");
  memcpy(modUser.password, pass, sizeof(pass));
  printf("New Balance : ");

  while (scanf("%d", &modUser.balance) != 1 || getchar() != '\n')
  {
    scanf("%*[^\n]%*c");
    printf("you must enter an integer: ");
  }

  write(sd, &modUser, sizeof(user));

  read(sd, &result, sizeof(result)); // from the server

  if (!result)
  {
    printf("\e[1;1H\e[2J");
    printf("\n\tError modifying the account ,please re-check the User ID!!\n\n");
  }
  else
  {
    printf("\e[1;1H\e[2J");
    printf("\n\t***Succesfully modified the account!!***\n\n");
  }
  return;
}

void search_account(int sd)
{
  int len;
  bool result;

  user search_user;
  int user_id;
  printf("User ID : \n");

  while (scanf("%d", &user_id) != 1 || getchar() != '\n')
  {
    scanf("%*[^\n]%*c");
    printf("you must enter an integer: ");
  }
  write(sd, &user_id, sizeof(int));

  len = read(sd, &search_user, sizeof(user));
  if (search_user.userID == -1)
  {
    printf("\e[1;1H\e[2J");
    printf("Please re-check the User ID!!\n\n");
  }
  else
  {
    printf("\e[1;1H\e[2J");
    printf("User ID : %d\n", search_user.userID);
    if (search_user.user_type == 1)
    {
      printf("Account type : Single \n");
      printf("Account Holder's Name : %s\n", search_user.name1);
    }
    else
    {
      printf("Account type : Joint \n");
      printf("First Account Holder's Name : %s\n", search_user.name1);
      printf("Second Account Holder's Name : %s\n", search_user.name2);
    }
    printf("Available Balance : Rs.%d\n", search_user.balance);
    printf("Status : %s\n\n", search_user.status);
  }
  return;
}

void show_admin_menu(int sd)
{
  int choice;
  while (1)
  {
    printf("1 : Add Account\n");
    printf("2 : Delete Account\n");
    printf("3 : Modify Account\n");
    printf("4 : Search Account\n");
    printf("5 : Exit\n");
    printf("Choose an option : \n");

    while (scanf("%d", &choice) != 1 || getchar() != '\n')
    {
      scanf("%*[^\n]%*c");
      printf("you must enter an integer: ");
    }

    write(sd, &choice, sizeof(choice));
    if (choice == 1)
    {
      sign_up(sd);
    }
    else if (choice == 2)
    {
      delete_account(sd);
    }
    else if (choice == 3)
    {
      modify_account(sd);
    }
    else if (choice == 4)
    {
      search_account(sd);
    }
    else if (choice == 5)
    {
      printf("\e[1;1H\e[2J");
      printf("\n\n\t\tThank You\n\n\n");
      exit(0);
    }
    else
    {
      printf("\e[1;1H\e[2J");
      printf("Invalid option!!\n\n");
    }
  }
}

#endif