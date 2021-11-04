#include "misc.h"

#define MAX_SKILL_LEVEL 99

void changeToWhiteFont() {
	//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void printError(char* format, ...) { //red text (fatal)
  char buffer[256];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED | FOREGROUND_INTENSITY); //bright red
  printf(buffer);
  //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); //change back to white
  va_end (args);
}

void printWarning(char* format, ...) { //yellow text
  char buffer[256];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
	//			FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_INTENSITY); //red+green = orange + intensity = yellow
  printf(buffer);
  //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); //change back to white
  va_end (args);
}

void printDebug(char* format, ...) { //orange text & red background
  char buffer[256];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
	//			FOREGROUND_RED | FOREGROUND_INTENSITY |						//bright red
	//			BACKGROUND_RED | BACKGROUND_GREEN);							//red+green = orange
  printf(buffer);
  //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED   | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); //change back to white
  va_end (args);
}

void hexString(char* buff, char* out, int packetSize) {
	//simple string to hex function, by moparscape4lifez
	char appendByte[3] = {0}; 
	int newSize = (packetSize*2)+packetSize;
	int count = 0;
	for(int j = 0; j < newSize; j+=3){
		sprintf(appendByte, "%02X ", uint8_t(buff[count++]));
		memcpy(out+j, appendByte, 3);
	}
	out[newSize] = '\0'; //null terminate for %s format -moparscape4lifez
}

uint64_t playerNameToInt64(char s[]) {
	uint64_t l = 0;
	for(unsigned int i = 0; i < strlen(s) && i < 12; i++) {
		char c = s[i];
		l *= 37L;
		if(c >= 'A' && c <= 'Z') l += (1 + c) - 65;
		else if(c >= 'a' && c <= 'z') l += (1 + c) - 97;
		else if(c >= '0' && c <= '9') l += (27 + c) - 48;
	}
	while(l % 37L == 0L && l != 0L) l /= 37L;
	return l;
}

char* textUnpack(char encryptedChatText[], int size) {
	char* decodeBuf = new char[size*2];//4096];
	int idx = 0, highNibble = -1;
	for(int i = 0; i < size*2; i++) {
		int val = encryptedChatText[i/2] >> (4-4*(i%2)) & 0xf;
		if(highNibble == -1) {
			if(val < 13) decodeBuf[idx++] = xlateTable[val];
			else highNibble = val;
		} else {
			decodeBuf[idx++] = xlateTable[((highNibble<<4) + val) - 195];
			highNibble = -1;
		}
	}

	decodeBuf[idx] = 0; //null terminator
	return strdup(decodeBuf);
}

void optimizeText(char* buf) {
	bool endMarker = true;	// marks the end of a sentence to make the next char capital
	for(unsigned int i = 0; i < strlen(buf); i++) {
        char c = buf[i];
        if(endMarker && c >= 'a' && c <= 'z') {
			buf[i] -= 0x20;	// transform lower case into upper case
			endMarker = false;
		}
		if(c == '.' || c == '!' || c == '?') endMarker = true;
	}
	buf[strlen(buf)] = 0; //null terminator
}

void textPack(char packedData[], char* text) {
	int textlen = strlen(text);
	if(textlen > 80) 
		text[80] = 0; //null terminator
	int i = 0;
	while(text[i]) //while not null terminator
		text[i] = tolower(text[i++]);

	int carryOverNibble = -1;
	int ofs = 0;
	for(int idx = 0; idx < textlen; idx++) {
		char c = text[idx];
		int tableIdx = 0;
		for(int i = 0; i < sizeof(xlateTable); i++) {
			if(c == xlateTable[i]) {
				tableIdx = i;
				break;
			}
		}
		if(tableIdx > 12) tableIdx += 195;
		if(carryOverNibble == -1) {
			if(tableIdx < 13) carryOverNibble = tableIdx;
			else packedData[ofs++] = (unsigned char)(tableIdx);
		} else if(tableIdx < 13) {
			packedData[ofs++] = (unsigned char)((carryOverNibble << 4) + tableIdx);
			carryOverNibble = -1;
		} else {
			packedData[ofs++] = (unsigned char)((carryOverNibble << 4) + (tableIdx >> 4));
			carryOverNibble = tableIdx & 0xf;
		}
	}

	if(carryOverNibble != -1) packedData[ofs++] = (unsigned char)(carryOverNibble << 4);
}

unsigned char getLevelForXP(int exp) {
	double points = 0;
	int output = 0;

	for (unsigned char lvl = 1; lvl <= MAX_SKILL_LEVEL; lvl++) {
		points += floor((double) lvl + 300.0 * pow(2.0, (double) lvl / 7.0));
		output = (int)floor(points / 4);
		if (output >= exp)
			return lvl;
	}
	return 0;
}

int getDirection(Pos startPos, Pos endPos) {
    int deltaX = startPos.x - endPos.x;
    int deltaY = startPos.y - endPos.y;

    if(deltaX < 0) {
        if(deltaY > 0)
            return 7; // North-West
        if(deltaY == 0)
            return 6; // West
        if(deltaY < 0)
            return 5; // South-West
    }

    if(deltaX > 0) {
        if(deltaY < 0)
            return 3; // South-East
        if(deltaY == 0)
            return 2; // East
        if(deltaY > 0)
            return 1; // North-East
    }

    if(deltaX == 0) {
        if(deltaY > 0)
            return 0; // North
        if(deltaY < 0)
            return 4; // South
    }
    return -1;
}
