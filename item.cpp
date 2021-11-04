#include "item.h"

#include "console.h"

#define MAX_ITEMS 7809

//Few item types for equipping
static int capes[] = {1007,1019,1021,1023,1027,1029,1031,1052,2412,2413,2414,4304,4315,4317,4319,4321,4323,4325,4327,4329,4331,4333,4335,4337,4339,4341,4343,4345,4347,4349,4351,4353,4355,4357,4359,4361,4363,4365,4367,4369,4371,4373,4375,4377,4379,4381,4383,4385,4387,4389,4391,4393,4395,4397,4399,4401,4403,4405,4407,4409,4411,4413,4514,4516,6070};
static int boots[] = {88,89,626,628,630,632,634,1061,1837,1846,2577,2579,2894,2904,2914,2924,2934,3061,3105,3107,3791,4097,4107,4117,4119,4121,4123,4125,4127,4129,4131,4310,5062,5063,5064,5345,5557,6069,6106,6143,6145,6147,6328};
static int gloves[] = {1059,1063,1065,1580,2487,2489,2491,2902,2912,2922,2932,2942,3060,3799,4095,4105,4115,4308,5556,6068,6110,6149,6151,6153};
static int shields[] = {1171,1173,1175,1177,1179,1181,1183,1185,1187,1189,1191,1193,1195,1197,1199,1201,1540,2589,2597,2603,2611,2621,2629,2659,2667,2675,2890,3122,3488,3758,3839,3840,3841,3842,3843,3844,4072,4156,4224,4225,4226,4227,4228,4229,4230,4231,4232,4233,4234,4302,4507,4512,6215,6217,6219,6221,6223,6225,6227,6229,6231,6233,6235,6237,6239,6241,6243,6245,6247,6249,6251,6253,6255,6257,6259,6261,6263,6265,6267,6269,6271,6273,6275,6277,6279,6524};
static int hats[] = {5525,5527,5529,5531,5533,5535,5537,5539,5541,5543,5545,5547,5549,5551,74,579,656,658,660,662,664,740,1017,1037,1038,1040,1042,1044,1046,1048,1050,1053,1055,1057,1137,1139,1141,1143,1145,1147,1149,1151,1153,1155,1157,1159,1161,1163,1165,1506,1949,2422,2581,2587,2595,2605,2613,2619,2627,2631,2651,2657,2673,2900,2910,2920,2930,2940,2978,2979,2980,2981,2982,2983,2984,2985,2986,2987,2988,2989,2990,2991,2992,2993,2994,2995,3057,3385,3486,3748,3749,3751,3753,3755,3797,4041,4042,4071,4089,4099,4109,4164,4302,4506,4511,4513,4515,4551,4567,4708,4716,4724,4745,4753,4856,4857,4858,4859,4880,4881,4882,4883,4904,4905,4906,4907,4952,4953,4954,4955,4976,4977,4978,4979,5013,5014,5554,5574,6109,6128,6131,6137,6182,6188,6335,6337,6339,6345,6355,6365,6375};
static int amulets[] = {86,87,295,421,552,589,1478,1692,1694,1696,1698,1700,1702,1704,1706,1708,1710,1712,1725,1727,1729,1731,4021,4081,4250,4677,6040,6041,6208};
static int arrows[] = {78,598,877,878,879,880,881,882,883,884,885,886,887,888,889,890,891,892,893,942,2532,2533,2534,2535,2536,2537,2538,2539,2540,2541,2866,4160,4172,4173,4174,4175,4740,5616,5617,5618,5619,5620,5621,5622,5623,5624,5625,5626,5627,6061,6062};
static int rings[] = {773,1635,1637,1639,1641,1643,1645,2550,2552,2554,2556,2558,2560,2562,2564,2566,2568,2570,2572,4202,4657,6465};
static int body[] = {3140,1101,1103,1105,1107,1109,1111,1113,1115,1117,1119,1121,1123,1125,1127,1129,1131,1133,1135,2499,2501,2503,2583,2591,2599,2607,2615,2623,2653,2669,3387,3481,4712,4720,4728,4749,4892,4893,4894,4895,4916,4917,4918,4919,4964,4965,4966,4967,6107,6133,6322};
static int legs[] = {538,542,548,1011,1013,1015,1067,1069,1071,1073,1075,1077,1079,1081,1083,1085,1087,1089,1091,1093,2585,2593,2601,2609,2617,2625,2655,2663,2671,3059,3389,3472,3473,3474,3475,3476,3477,3478,3479,3480,3483,3485,3795,4087,4585,4712,4714,4722,4730,4738,4751,4759,4874,4875,4876,4877,4898,4899,4900,4901,4922,4923,4924,4925,4946,4947,4948,4949,4970,4971,4972,4973,4994,4995,4996,4997,5048,5050,5052,5576,6107,6130,6187,6390};
static int platebody[] = {3140,1115,1117,1119,1121,1123,1125,1127,2583,2591,2599,2607,2615,2623,2653,2669,3481,4720,4728,4749};
static int fullHelm[] = {1153,1155,1157,1159,1161,1163,1165,2587,2595,2605,2613,2619,2627,2657,2673,3486};
static int weapons[] = {1,2};

static int fullMask[] = {1053,1055,1057};
//All other IDs are weapons (I hope)
static bool itemStackable[MAX_ITEMS];
static bool itemIsNote[MAX_ITEMS];
//item.cfg
static const char* itemBonusName[] = {"Stab", "Slash", "Crush", "Magic", "Range", "Stab", "Slash", "Crush", "Magic", "Range", "Strength", "Prayer"};
static ItemDef itemDef[MAX_ITEMS];
static ItemBonus itemBonuses[MAX_ITEMS]; //<- call frequently might as well make it solo -moparscap4lifez

Item::Item() {
	int counter = 0;
	int c;

	FILE* dataIn;
	dataIn = fopen("data/stackable.dat", "r");
	if(dataIn) {
		while(!feof(dataIn)) {
			c = (int)getc(dataIn);
			if(counter > MAX_ITEMS) break; //little safeguard
			if (c == 0) {
				itemStackable[counter] = false;
			} else {
				itemStackable[counter] = true;
			}
			counter++;
		}
		fclose(dataIn);
	} else {
		Console::Err("Critical error while loading stackabledata!");
	}

	counter = 0;
	Console::Dbg("Loading item noteable list");
	dataIn = fopen("data/notes.dat", "r");
	if(dataIn) {
		while(!feof(dataIn)) {
			c = (int)getc(dataIn);
			if(counter > MAX_ITEMS) break; //little safeguard
			if (c == 0)
				itemIsNote[counter] = true;
			else
				itemIsNote[counter] = false;
			counter++;
		}
		fclose(dataIn);
	} else {
		Console::Err("Critical error while loading notedata!");
	}

	char line[255];
	char* ptrOffset;
	int numTabs = 0;
	counter = 0;
	//itemdef
	int itemId;
	char name[50];
	char description[80];
	double shopValue = 0.0;
	double lowAlchemlyValue = 0.0;
	double highAlchemlyValue = 0.0;
	//bonuses
	int attackStab = 0;   //0
	int attackSlash = 0;  //1
	int attackCrush = 0;  //2
	int attackMagic = 0;  //3
	int attackRange = 0;  //4
	int defenseStab = 0;  //5
	int defenseSlash = 0; //6
	int defenseCrush = 0; //7
	int defenseMagic = 0; //8
	int defenseRange = 0; //9
	int otherStrength = 0; //10
	int otherPrayer = 0;   //11

	int retVal = -1;
	Console::Dbg("Loading item list");
	dataIn = fopen("data/item.cfg", "rb"); //read+binary
	if(dataIn) {
		while(!feof(dataIn)) {
			fgets(line, sizeof(line), dataIn); //get one line of data or 80 bytes whichever comes first -moparscape4lifez

			if((ptrOffset = strstr(line, "item =")) != NULL) { //found item = line
				numTabs = 0; //reset counter for this npc.

				for(int i = 0; i < strlen(line); i++)
					if(line[i] == '\t') numTabs++;
				

				if(numTabs == 17) { 
					////format: item = %d %s %s %lf %lf %lf %d %d %d %d %d %d %d %d %d %d %d %d -moparscape4lifez
					if(sscanf(line,"item = %d %s %s %lf %lf %lf %d %d %d %d %d %d %d %d %d %d %d %d", &itemId, name, description, &shopValue, &lowAlchemlyValue, &highAlchemlyValue, &attackStab, &attackSlash, &attackCrush, &attackMagic, &attackRange, &defenseStab, &defenseSlash, &defenseCrush, &defenseMagic, &defenseRange, &otherStrength, &otherPrayer) == 18) { //* = ignore
						strcpy(itemDef[itemId].name, name);
						strcpy(itemDef[itemId].description, description);
						itemDef[itemId].shopValue = shopValue;
						itemDef[itemId].lowAlchemlyValue = lowAlchemlyValue;
						itemDef[itemId].highAlchemlyValue = highAlchemlyValue;
						itemBonuses[itemId].attackStab = attackStab;		//1
						itemBonuses[itemId].attackSlash = attackSlash;		//2
						itemBonuses[itemId].attackCrush = attackCrush;		//3
						itemBonuses[itemId].attackMagic = attackMagic;		//4
						itemBonuses[itemId].attackRange = attackRange;		//5
						itemBonuses[itemId].defenseStab = defenseStab;		//6
						itemBonuses[itemId].defenseSlash = defenseSlash;	//7
						itemBonuses[itemId].defenseCrush = defenseCrush;	//8
						itemBonuses[itemId].defenseMagic = defenseMagic;	//9
						itemBonuses[itemId].defenseRange = defenseRange;	//10
						itemBonuses[itemId].otherStrength = otherStrength;	//11
						itemBonuses[itemId].otherPrayer = otherPrayer;		//12
					} else {
						Console::Err("Error @ item:[%d] sscanf failed while reading item.cfg", counter);
					}
				} else {
					Console::Err("Error @ item:[%d] while reading item.cfg", counter);
					Console::Err("Make sure when you add more items's you use tabbing not spaces in item.cfg");
				}
				counter++;
				if(counter > MAX_ITEMS) {
					Console::Err("Loading more items then maxItems: %d curItem: %d", MAX_ITEMS, counter);
					return;
				}
			}
		}
		fclose(dataIn);
	} else {
		Console::Err("Critical error while loading itemdata!");
	}
}

Item::~Item() {

}

bool Item::isItemStackable(int itemId) {
	return (itemStackable[itemId] == 1);
}

bool Item::isItemNoted(int itemId) {
	return (itemIsNote[itemId] == 1);
}

int Item::itemType(int itemId) {
	for (int i=0; i<sizeof(hats)/sizeof(int);i++) {
		if(itemId == hats[i])
		  return playerHat;
	}
	for (int i=0; i<sizeof(capes)/sizeof(int);i++) {
		if(itemId == capes[i])
		  return playerCape;
	}
	for (int i=0; i<sizeof(amulets)/sizeof(int);i++) {
		if(itemId == amulets[i])
		  return playerAmulet;
	}
	//Weapon goes here
	for (int i=0; i<sizeof(body)/sizeof(int);i++) {
		if(itemId == body[i])
		  return playerChest;
	}
	for (int i=0; i<sizeof(shields)/sizeof(int);i++) {
		if(itemId == shields[i])
		  return playerShield;
	}
	for (int i=0; i<sizeof(legs)/sizeof(int);i++) {
		if(itemId == legs[i])
		  return playerLegs;
	}
	for (int i=0; i<sizeof(gloves)/sizeof(int);i++) {
		if(itemId == gloves[i])
		  return playerHands;
	}
	for (int i=0; i<sizeof(boots)/sizeof(int);i++) {
		if(itemId == boots[i])
		  return playerFeet;
	}
	for (int i=0; i<sizeof(rings)/sizeof(int);i++) {
		if(itemId == rings[i])
		  return playerRing;
	}
	for (int i=0; i<sizeof(arrows)/sizeof(int);i++) {
		if(itemId == arrows[i])
		  return playerArrows;
	}

	//Default should return non-wearable need weapons array -___- moparscape4lifez
	return playerWeapon;
}

bool Item::isPlate(int itemID) {
	for (int i=0; i<sizeof(platebody)/sizeof(int); i++) {
		if (platebody[i] == itemID)
			return true;
	}
	return false;
}

bool Item::isFullHelm(int itemID) {
	for (int i=0; i<sizeof(fullHelm)/sizeof(int); i++) {
		if (fullHelm[i] == itemID)
			return true;
	}
	return false;
}

bool Item::isFullMask(int itemID) {
	for (int i=0; i<sizeof(fullMask)/sizeof(int); i++) {
		if (fullMask[i] == itemID)
			return true;
	}
	return false;
}

 const char* Item::getBonusName(int index) {
	return itemBonusName[index];
}

ItemBonus Item::getItemBonuses(int itemId) {
	return itemBonuses[itemId];
}

int Item::randomCape() {
	return capes[(int)rand() % sizeof(capes)/sizeof(int)];
}
int Item::randomBoots() {
	return boots[(int)rand() % sizeof(boots)/sizeof(int)];
}
int Item::randomGloves() {
	return gloves[(int)rand() % sizeof(gloves)/sizeof(int)];
}
int Item::randomShield() {
	return shields[(int)rand() % sizeof(shields)/sizeof(int)];
}
int Item::randomHat() {
	return hats[(int)rand() % sizeof(hats)/sizeof(int)];
}
int Item::randomAmulet() {
	return amulets[(int)rand() % sizeof(amulets)/sizeof(int)];
}
int Item::randomArrows() {
	return arrows[(int)rand() % sizeof(arrows)/sizeof(int)];
}
int Item::randomRing() {
	return rings[(int)rand() % sizeof(rings)/sizeof(int)];
}
int Item::randomBody() {
	return body[(int)rand() % sizeof(body)/sizeof(int)];
}
int Item::randomLegs() {
	return legs[(int)rand() % sizeof(legs)/sizeof(int)];
}
int Item::randomWeapon() {
	return weapons[(int)rand() % sizeof(weapons)/sizeof(int)];
}
