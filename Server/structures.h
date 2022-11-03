#ifndef STRUCTURES
#define STRUCTURES

struct user
{
	int userID;
	char name1[30];
	char name2[30];
	char password[30];
	int balance;
	char status[20];
	int user_type;
};

struct admin
{
	int userID;
	int password;
};

struct transaction
{
	int amount;
	int curr_bal;
};

#endif