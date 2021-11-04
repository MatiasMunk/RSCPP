#ifndef FWD_WORLD
#define FWD_WORLD

class World;

enum AccountReply : short
{
	ACCOUNT_EXISTS = 1,
	ACCOUNT_NOT_APPROVED = 2,
	ACCOUNT_CREATED = 3,
	ACCOUNT_CHANGE_FAILED = 5,
	ACCOUNT_CHANGED = 6,
	ACCOUNT_CONTINUE = 1000 // TODO: Check this for the real value
};

enum LoginReply : short
{
	LOGIN_WRONG_USER = 1,
	LOGIN_WRONG_USERPASS = 2,
	LOGIN_OK = 3,
	LOGIN_LOGGEDIN = 5,
	LOGIN_BUSY = 6
};

#endif // FWD_WORLD
