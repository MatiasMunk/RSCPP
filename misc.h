#ifndef MISC_H
#define MISC_H

#include <cstring>
#include <stdint.h>

#include <math.h>	//floor() pow() xp to level support.

#include <stdio.h> // for standard input/output scanfs/printfs etc.. lots of useless shit
#include <ctype.h> //tolower()
#include <stdarg.h> //... unlimited arg[]'s support

#include "constants.h"

struct Pos {
	short x;
	short y;
};

struct Hit {
	int dmg;	   //amount of damage has been hit for.
	bool poisoned; //green hitting dmg.
};

struct GameItem {
	short itemId; //Change short to int if you pass 32,000 items.. but NEVER make it unsigned because -1 means no item exists.
	int amount; //2,147,000,000 +++ ;)
};

struct ItemDef {
	char name[50];
	char description[80];
	double shopValue;
	double lowAlchemlyValue;
	double highAlchemlyValue;
};

struct ItemBonus {
	int attackStab;   //0
	int attackSlash;  //1
	int attackCrush;  //2
	int attackMagic;  //3
	int attackRange;  //4

	int defenseStab;  //5
	int defenseSlash; //6
	int defenseCrush; //7
	int defenseMagic; //8
	int defenseRange; //9

	int otherStrength; //10
	int otherPrayer;   //11
};

struct Level {
	unsigned char level;	//this is the temp level which can get increased/decreased by potions/temp stat transfer system etc..
	unsigned char maxLevel; //calculated from getLevelForXP
	int xp; //2,147,000,000 +++ ;)
};

void changeToWhiteFont();
void printError(char * format, ...);
void printWarning(char * format, ...);
void printDebug(char * format, ...);
void hexString(char* buff, char* out, int packetSize);
uint64_t playerNameToInt64(char s[]);
char* textUnpack(char encryptedChatText[], int size);
void optimizeText(char* buf);
void textPack(char packedData[], char* text);
unsigned char getLevelForXP(int exp);
int getDirection(Pos startPos, Pos endPos);

// translates our direction convention to the one used in the protocol
static char xlateDirectionToClient[] = { 1, 2, 4, 7, 6, 5, 3, 0 };
static char directionDeltaX[] = { 0, 1, 1, 1, 0,-1,-1,-1 };
static char directionDeltaY[] = { 1, 1, 0,-1,-1,-1, 0, 1 };

static char xlateTable[] = {
	' ', 'e', 't', 'a', 'o', 'i', 'h', 'n', 's', 'r', 
	'd', 'l', 'u', 'm', 'w', 'c', 'y', 'f', 'g', 'p', 
	'b', 'v', 'k', 'x', 'j', 'q', 'z', '0', '1', '2', 
	'3', '4', '5', '6', '7', '8', '9', ' ', '!', '?', 
	'.', ',', ':', ';', '(', ')', '-', '&', '*', '\\', 
	'\'', '@', '#', '+', '=', '\243', '$', '%', '"', '[', 
	']'
};

#define cmpPos(startPos, endPos) ((startPos.x) == (endPos.x) && (startPos.y) == (endPos.y))  //macro l33tness but I think overloading would of been better. -moparscape4lifez
#endif
