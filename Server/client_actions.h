#ifndef CLIENT_ACTIONS
#define CLIENT_ACTIONS
#include "admin_actions.h"
user view_user_details(int ID)
{
  int i = ID - 1;
  user currUser;
  int fd = open("Users", O_RDONLY, 0744);
  struct flock lock;

  lock_record(lock, F_RDLCK, SEEK_SET, fd, i, sizeof(user));

  lseek(fd, (i) * sizeof(user), SEEK_SET); // changing the file pointer to the selected record
  read(fd, &currUser, sizeof(user));

  unlock_record(lock, fd);

  close(fd);
  return currUser;
}

void view_transaction(int acc, int nsd)
{
  char acc_no[5];
  sprintf(acc_no, "%d", acc);
  // printf("%s", acc_no);
  int fd = open(acc_no, O_RDONLY, 0744);
  int index = -1;
  if (fd == -1)
  {
    write(nsd, &index, sizeof(index));
    return;
  }
  else
  {
    transaction t;
    int read_bytes;
    index = 1;
    while (read(fd, &t, sizeof(t)) > 0)
    {
      write(nsd, &index, sizeof(index));
      write(nsd, &t, sizeof(t));
    }
    index = -1;
    write(nsd, &index, sizeof(index));
  }
  close(fd);
  return;
}

bool deposit(int ID, int amt)
{
  int i = ID - 1;
  int fd = open("Users", O_RDWR, 0744);
  bool result;
  int fl1;
  struct flock lock;

  lock_record(lock, F_WRLCK, SEEK_SET, fd, i, sizeof(user));
  user currUser;
  lseek(fd, (i) * sizeof(user), SEEK_SET); // changing the file pointer to the selected record
  read(fd, &currUser, sizeof(user));

  if (!strcmp(currUser.status, "ACTIVE"))
  {
    currUser.balance += amt;
    lseek(fd, sizeof(user) * (-1), SEEK_CUR);
    write(fd, &currUser, sizeof(user));
    result = true;
  }
  else
    result = false;
  unlock_record(lock, fd);

  close(fd);

  //---ADD transaction
  char acc_no[4];
  sprintf(acc_no, "%d", currUser.userID);
  int fd2 = open(acc_no, O_CREAT | O_RDWR, 0744);
  lseek(fd2, 0, SEEK_END);
  transaction t;
  t.amount = amt;
  t.curr_bal = currUser.balance;
  write(fd2, &t, sizeof(t));
  close(fd2);

  return result;
}

bool withdraw(int ID, int amt)
{
  int i = ID - 1;
  int fd = open("Users", O_RDWR, 0744);
  bool result;
  int fl1;
  struct flock lock;
  lock_record(lock, F_WRLCK, SEEK_SET, fd, i, sizeof(user));

  user currUser;
  lseek(fd, (i) * sizeof(user), SEEK_SET);
  read(fd, &currUser, sizeof(user));

  if (!strcmp(currUser.status, "ACTIVE") && currUser.balance >= amt)
  {
    currUser.balance -= amt;
    lseek(fd, sizeof(user) * (-1), SEEK_CUR);
    write(fd, &currUser, sizeof(user));
    result = true;
  }
  else
    result = false;
  unlock_record(lock, fd);

  close(fd);

  //---ADD transaction
  char acc_no[4];
  sprintf(acc_no, "%d", currUser.userID);
  int fd2 = open(acc_no, O_CREAT | O_RDWR, 0744);
  lseek(fd2, 0, SEEK_END);
  transaction t;
  t.amount = -1 * amt;
  t.curr_bal = currUser.balance;
  write(fd2, &t, sizeof(t));
  close(fd2);

  return result;
}

int balance_enquiry(int ID)
{
  int i = ID - 1;
  int result;
  int fd = open("Users", O_RDONLY, 0744);
  user temp;

  int fl1;
  struct flock lock;
  lock_record(lock, F_RDLCK, SEEK_SET, fd, i, sizeof(user));

  lseek(fd, (i) * sizeof(user), SEEK_SET); // changing the file pointer to the selected record
  read(fd, &temp, sizeof(user));
  if (!strcmp(temp.status, "ACTIVE"))
    result = temp.balance;
  else
    result = 0;

  unlock_record(lock, fd);

  close(fd);
  return result;
}

bool change_password(int ID, char newPwd[10])
{
  int i = ID - 1;

  int fd = open("Users", O_RDWR, 0744);
  bool result;
  int fl1;
  struct flock lock;
  lock_record(lock, F_WRLCK, SEEK_SET, fd, i, sizeof(user));

  user currUser;
  lseek(fd, (i) * sizeof(user), SEEK_SET);
  read(fd, &currUser, sizeof(user));

  if (!strcmp(currUser.status, "ACTIVE"))
  {
    strcpy(currUser.password, newPwd);
    lseek(fd, sizeof(user) * (-1), SEEK_CUR);
    write(fd, &currUser, sizeof(user));
    result = true;
  }
  else
    result = false;
  unlock_record(lock, fd);

  close(fd);
  return result;
}

void show_client_menu(int nsd, int currUserID)
{

  int choice;
  bool result;
  while (1)
  {
    read(nsd, &choice, sizeof(choice));

    if (choice == 1)
    {
      int amt;
      read(nsd, &amt, sizeof(int));
      // printf("%d  AMOUnt \n", amt);
      result = deposit(currUserID, amt);
      write(nsd, &result, sizeof(result));
    }
    else if (choice == 2)
    {
      int amt;
      read(nsd, &amt, sizeof(int));
      result = withdraw(currUserID, amt);
      write(nsd, &result, sizeof(result));
    }
    else if (choice == 3)
    {
      int amt;
      amt = balance_enquiry(currUserID);
      write(nsd, &amt, sizeof(int));
    }
    else if (choice == 4)
    {
      char pwd[10];
      read(nsd, pwd, sizeof(pwd));
      result = change_password(currUserID, pwd);
      write(nsd, &result, sizeof(result));
    }
    else if (choice == 5)
    {
      user user1 = view_user_details(currUserID);
      write(nsd, &user1, sizeof(user));
    }
    else if (choice == 6)
    {
      view_transaction(currUserID, nsd);
    }
    else if (choice == 7)
    {
      close(nsd);
      printf("Ended client session.....\n");
      exit(0);
    }
  }
}

#endif