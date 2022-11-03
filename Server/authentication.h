#ifndef AUTH
#define AUTH

#include "admin_actions.h"
#include "client_actions.h"
int ADMIN_ID = 9999;
int ADMIN_PASS = 9999;
int currUserID;
bool attempt_admin_login(int nsd)
{
  admin a;
  bool result;
  read(nsd, &a, sizeof(admin));
  result = false;
  if (a.userID == ADMIN_ID && a.password == ADMIN_PASS)
  {
    result = true;
  }
  write(nsd, &result, sizeof(result));
  return result;
}
bool check_user(user currUser)
{
  int i = currUser.userID - 1;
  int fd = open("Users", O_RDONLY, 0744);
  bool result;
  user temp;

  int fl1;
  struct flock lock;
  lock_record(lock, F_RDLCK, SEEK_SET, fd, i, sizeof(user));

  lseek(fd, (i) * sizeof(user), SEEK_SET);
  read(fd, &temp, sizeof(user));
  // printf("Password actual: %s\n", temp.password);
  // printf("id: %d\n", currUser.userID);
  if (!strcmp(temp.password, currUser.password) && currUser.userID == temp.userID && !strcmp(temp.status, "ACTIVE") && temp.user_type == currUser.user_type)
    result = true;
  else
    result = false;

  unlock_record(lock, fd);

  close(fd);
  return result;
}

bool attempt_user_login(int nsd)
{
  user currUser;
  bool result;
  read(nsd, &currUser, sizeof(user));
  result = check_user(currUser);
  currUserID = currUser.userID;
  write(nsd, &result, sizeof(result));
  return result;
}

void attempt_login(int nsd)
{
  int choice;
  bool result;
  read(nsd, &choice, sizeof(int));
  switch (choice)
  {
  case 1:
  case 2:
    result = attempt_user_login(nsd);
    if (!result)
    {
      // printf("\nInvalid Credintials\n");
      return;
    }
    else
    {
      show_client_menu(nsd, currUserID);
    }
    break;
  case 3:
    result = attempt_admin_login(nsd);
    if (!result)
    {
      // printf("\nInvalid Credintials\n");
      return;
    }
    else
    {
      show_admin_menu(nsd);
    }
  case 4:
    return;
  default:
    return;
  }
}

#endif