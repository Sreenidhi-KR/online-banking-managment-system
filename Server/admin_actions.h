#ifndef ADMIN_ACTIONS
#define ADMIN_ACTIONS

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include "structures.h"

#define mode 0744
#define flags O_RDWR | O_CREAT

void lock_record(struct flock lock, short lock_type, short whence, int fd, int i, int size)
{
  int fl;
  lock.l_type = lock_type;
  lock.l_whence = whence;
  lock.l_start = (i)*size; // nth record
  lock.l_len = size;       // sizeof(record)
  lock.l_pid = getpid();
  fl = fcntl(fd, F_SETLKW, &lock);
}

void unlock_record(struct flock lock, int fd)
{
  lock.l_type = F_UNLCK;
  fcntl(fd, F_SETLK, &lock);
}

int get_latest_user()
{
  struct flock lock;
  int fd = open("users", flags, mode);
  int size = lseek(fd, 0, SEEK_END);

  if (!size)
  {
    close(fd);
    return 0;
  }
  else
  {
    lock_record(lock, F_WRLCK, SEEK_END, fd, -1, sizeof(user));
    user prev_user;
    lseek(fd, -sizeof(user), SEEK_END);
    read(fd, &prev_user, sizeof(user));
    unlock_record(lock, fd);
    close(fd);
    return prev_user.userID;
  }
}

user search_user(int ID)
{
  user currUser;
  printf("%d \n", get_latest_user());
  if (ID < 1000 || ID > get_latest_user())
  {
    currUser.userID = -1;
    return currUser;
  }
  int i = ID - 1000;
  int fd = open("users", O_RDONLY, 0744);
  struct flock lock;

  lock_record(lock, F_RDLCK, SEEK_SET, fd, i, sizeof(user));

  lseek(fd, (i) * sizeof(user), SEEK_SET); // changing the file pointer to the selected record
  read(fd, &currUser, sizeof(user));

  unlock_record(lock, fd);

  close(fd);
  return currUser;
}

void create_user(int nsd)
{
  user newUser;
  struct flock lock;
  read(nsd, &newUser, sizeof(user));
  int fd = open("users", flags, mode);
  int size = lseek(fd, 0, SEEK_END);
  int id, s;

  newUser.balance = 0;
  strcpy(newUser.status, "ACTIVE");
  if (!size)
  {
    lock_record(lock, F_WRLCK, SEEK_END, fd, 0, 0);
    newUser.userID = 1000;
    s = write(fd, &newUser, sizeof(user));
    unlock_record(lock, fd);
  }
  else
  {
    lock_record(lock, F_WRLCK, SEEK_END, fd, -1, sizeof(user));
    user prev_user;
    lseek(fd, -sizeof(user), SEEK_END);
    read(fd, &prev_user, sizeof(user));
    newUser.userID = prev_user.userID + 1;
    s = write(fd, &newUser, sizeof(user));
    unlock_record(lock, fd);
  }
  close(fd);
  if (s > 0)
  {
    id = newUser.userID;
  }
  else
  {
    id = -1;
  }
  write(nsd, &id, sizeof(id));
}

bool delete_user(int ID)
{
  if (ID < 1000 || ID > get_latest_user())
  {
    return false;
  }
  int i = ID - 1000;
  int fd = open("users", O_RDWR, 0744);
  bool result;

  int fl1;
  struct flock lock;
  lock_record(lock, F_WRLCK, SEEK_SET, fd, i, sizeof(user));
  user currUser;
  lseek(fd, (i) * sizeof(user), SEEK_SET); // changing the file pointer to the selected record
  read(fd, &currUser, sizeof(user));

  if (!strcmp(currUser.status, "ACTIVE"))
  {
    strcpy(currUser.status, "CLOSED");
    currUser.balance = 0;

    lseek(fd, (-1) * sizeof(user), SEEK_CUR);
    int j = write(fd, &currUser, sizeof(user));
    if (j != 0)
      result = true;
    else
      result = false;
  }

  unlock_record(lock, fd);

  close(fd);
  return result;
}

bool modify_user(user modUser)
{
  if (modUser.userID < 1000 || modUser.userID > get_latest_user())
  {
    return false;
  }
  int i = modUser.userID - 1000;
  int fd = open("users", O_RDWR, 0744);
  bool result = false;

  int fl1;
  struct flock lock;
  lock_record(lock, F_WRLCK, SEEK_SET, fd, i, sizeof(user));
  user currUser;
  lseek(fd, (i) * sizeof(user), SEEK_SET);
  read(fd, &currUser, sizeof(user));

  if (!strcmp(currUser.status, "ACTIVE"))
  {
    strcpy(modUser.status, "ACTIVE");
    lseek(fd, (-1) * sizeof(user), SEEK_CUR);
    int j = write(fd, &modUser, sizeof(user));
    if (j != 0)
      result = true;
    else
      result = false;
  }

  unlock_record(lock, fd);

  close(fd);
  return result;
}

void show_admin_menu(int nsd)
{

  int choice;
  bool result;
  while (1)
  {
    read(nsd, &choice, sizeof(choice));
    if (choice == 1)
    {
      create_user(nsd);
    }
    else if (choice == 2)
    {
      int id;
      read(nsd, &id, sizeof(int));
      result = delete_user(id);
      write(nsd, &result, sizeof(result));
    }
    else if (choice == 3)
    {
      user u;
      read(nsd, &u, sizeof(user));
      result = modify_user(u);
      write(nsd, &result, sizeof(result));
    }
    else if (choice == 4)
    {
      user u;
      int id;
      read(nsd, &id, sizeof(int));
      u = search_user(id);
      write(nsd, &u, sizeof(user));
    }
    else if (choice == 5)
    {
      close(nsd);
      printf("Ended client session.....\n");
      exit(0);
    }
    else
    {
    }
  }
}

#endif