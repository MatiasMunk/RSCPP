//#include "isaac.h"
#include "client.h"
#include "console.h"
#include "constants.h"
#include <iostream>
#include <random>
#include <limits>
#include <time.h>

#include "cryption.h"
#include "buffer.h"
#include "item.h"

#include <cstdlib>
#include <climits>

static int packetSizes[256] = {
	0, 0, 0, 1, -1, 0, 0, 0, 0, 0, //0
	0, 0, 0, 0, 8, 0, 6, 2, 2, 0,  //10
	0, 2, 0, 6, 0, 12, 0, 0, 0, 0, //20
	0, 0, 0, 0, 0, 8, 4, 0, 0, 2,  //30
	2, 6, 0, 6, 0, -1, 0, 0, 0, 0, //40
	0, 0, 0, 12, 0, 0, 0, 0, 8, 0, //50
	0, 8, 0, 0, 0, 0, 0, 0, 0, 0,  //60
	6, 0, 2, 2, 8, 6, 0, -1, 0, 6, //70
	0, 0, 0, 0, 0, 1, 4, 6, 0, 0,  //80
	0, 0, 0, 0, 0, 3, 0, 0, -1, 0, //90
	0, 13, 0, -1, 0, 0, 0, 0, 0, 0,//100
	0, 0, 0, 0, 0, 0, 0, 6, 0, 0,  //110
	1, 0, 6, 0, 0, 0, -1, 0, 2, 6, //120
	0, 4, 6, 8, 0, 6, 0, 0, 0, 2,  //130
	0, 0, 0, 0, 0, 6, 0, 0, 0, 0,  //140
	0, 0, 1, 2, 0, 2, 6, 0, 0, 0,  //150
	0, 0, 0, 0, -1, -1, 0, 0, 0, 0,//160
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //170
	0, 8, 0, 3, 0, 2, 0, 0, 8, 1,  //180
	0, 0, 12, 0, 0, 0, 0, 0, 0, 0, //190
	2, 0, 0, 0, 0, 0, 0, 0, 4, 0,  //200
	4, 0, 0, 0, 7, 8, 0, 0, 10, 0, //210
	0, 0, 0, 0, 0, 0, -1, 0, 6, 0, //220
	1, 0, 0, 0, 6, 0, 6, 8, 1, 0,  //230
	0, 4, 0, 0, 0, 0, -1, 0, -1, 4,//240
	0, 0, 6, 6, 0, 0	           //250-255 (full byte of packetTypes)
};


#pragma pack(1)
struct RequestReplyServer {
  int8_t   unused[8];
  int8_t   responseCode;
  uint64_t  serverSessionKey;
};
#pragma pack()

#pragma pack(1)
struct RequestReplyServer2 {
	ResponseCode   responseCode;
	PlayerStatus   playerStatus;
	uint8_t   flagged;
};
#pragma pack()

#pragma pack(1)
struct RequestReplyClient {
  uint8_t  connectStatus;
  uint8_t  size;
  uint8_t  magicNumber;
  unsigned short   revision;
  int8_t  clientVersion;
  int     crcValues[9];
  uint8_t  size2;
  int8_t  byte10;
  uint64_t    clientSessionKey;
  uint64_t    serverSessionKey;
  int     userId;
  char    userName[MAX_NAME];
  char    password[MAX_NAME];
};
#pragma pack()

Item item;
Client::Client(const SOCKET& socket) :
	_isLoggedIn(false),
	_isConnected(true),
	_isInited(false),
	_socket(socket),
	_isMapRegionChanged(true){

	

	_equipment[playerHat].itemId = item.randomHat();
	_equipment[playerHat].amount = 1;
	_equipment[playerCape].itemId = item.randomCape();
	_equipment[playerCape].amount = 1;
	_equipment[playerAmulet].itemId = item.randomAmulet();
	_equipment[playerAmulet].amount = 1;
	_equipment[playerArrows].itemId = item.randomArrows();
	_equipment[playerArrows].amount = 1000;
	_equipment[playerChest].itemId = item.randomBody();
	_equipment[playerChest].amount = 1;
	_equipment[playerShield].itemId = item.randomShield();
	_equipment[playerShield].amount = 1;
	_equipment[playerLegs].itemId = item.randomLegs();
	_equipment[playerLegs].amount = 1;
	_equipment[playerHands].itemId = item.randomGloves();
	_equipment[playerHands].amount = 1;
	_equipment[playerFeet].itemId = item.randomBoots();
	_equipment[playerFeet].amount = 1;
	_equipment[playerRing].itemId = item.randomRing();
	_equipment[playerRing].amount = 1;
	_equipment[playerWeapon].itemId = item.randomWeapon();
	_equipment[playerWeapon].amount = 1;

	_absolutePos.x = _absolutePos.y = -1;
	_mapRegionPos.x = _mapRegionPos.y = -1;

	_updateRequired=true;
	_isRequireAppearenceUpdate = true;
	_isRequireChatUpdate = false;

	_isTeleporting = true;
	_teleportPos.x = 3254;
	_teleportPos.y = 3420;

	for (int i = 0; i < MAX_PLAYERS; i++)
		_playerInListBitmap[i >> 3] &= ~(1 << (i & 7));

	_dir1 = -1;
	_dir2 = -1;

	_gender = 0;
	_skulled = false;
	_combatLevel = 3;

	//Type of apperance look
	_apperence.head = 3;
	_apperence.body = 19;
	_apperence.arms = 29;
	_apperence.hands = 35;
	_apperence.legs = 39;
	_apperence.feet = 44;
	_apperence.beard = 17;

	//Apperance coloring
	_apperence.hairColor = 7;
	_apperence.bodyColor = 8;
	_apperence.legColor = 9;
	_apperence.feetColor = 5;
	_apperence.skinColor = 0;

	_isTeleported = false;
	_isNpc = false;
	_isRunning = false;

	_pathWaypoints.clear();
}

bool Client::IsPlat(const short& itemId) {
	return item.isPlate(itemId);
}

bool Client::IsFullHelm(const short& itemId) {
	return item.isFullHelm(itemId);
}
bool Client::IsFullMask(const short& itemId) {
	return item.isFullMask(itemId);
}

int Client::SendBytes(char* buffer, const size_t& len) {

	std::lock_guard<std::mutex> guard(_socketMutex);

	auto iResult = send(_socket, buffer, len, 0);
	if (iResult == SOCKET_ERROR) {
		#ifdef _WIN32
		Console::Err("send failed: %d\n", WSAGetLastError());
		closesocket(_socket);
		WSACleanup();
		#else
		Console::Err("send failed: %d\n", errno);
		close(_socket);
		#endif
		
		_isConnected = false;
		return FAILURE;
	}

	return SUCESS;
}
int Client::ReceiveBytes(char* buffer, const size_t& len)
{
	std::lock_guard<std::mutex> guard(_socketMutex);

	auto iResult = recv(_socket, buffer, len, 0);
	if (iResult > 0) {
		return SUCESS;
	}
	else if (iResult == 0) {
		Console::Err("Connection closed\n");
		_isConnected = false;
		return FAILURE;
	}
	else {
		Console::Err("recv failed: %d\n", errno);
		_isConnected = false;
		return FAILURE;
	}	

	return SUCESS;
}

int Client::SendCmd(const uint8_t& cmd) {

	uint8_t encCmd = cmd + _encryptor->getNextKey();
	return SendBytes((char*)(&encCmd), sizeof(uint8_t));
}

int Client::SendCmd(const uint8_t& cmdType, Buffer& buffer) {

	SendCmd(cmdType);
	return SendBytes(buffer.Data(), buffer.Size());
}

int Client::SendVarCmd(const uint8_t& cmdType, Buffer& buffer) {

	SendCmd(cmdType);

	uint8_t size = (uint8_t)buffer.Size();
	SendByte(size);
	return SendBytes(buffer.Data(), buffer.Size());
}

int Client::SendBigVarCmd(const uint8_t& cmdType, Buffer& buffer) {

	SendCmd(cmdType);

	short size = (short)buffer.Size();
	SendVariable(size);
	return SendBytes(buffer.Data(), buffer.Size());
}

int Client::SendWordA(const short& i) {

	char buffer[2];

	buffer[0] = (char)(i >> 8);
	buffer[1] = (char)(i + 128);

	return SendBytes(buffer, sizeof(buffer));
}

int Client::SendDWord_v2(const int& i) {

char buffer[4];

	buffer[0] = (char)(i >> 16);
	buffer[1] = (char)(i >> 24);
	buffer[2] = (char)i;
	buffer[3] = (char)(i >> 8);

	return SendBytes(buffer, sizeof(buffer));
}

int Client::SendByteA(const uint8_t& i) {

	uint8_t byteValue = i + 128;
	return SendBytes((char*)&byteValue, sizeof(byteValue));
}

int Client::SendNegByte(const uint8_t& i)
{
	uint8_t value = -1;
   return SendBytes((char*)&value, sizeof(value));
}
int Client::ReceiveString(char* buffer, const size_t& maxLen) {

	char nextCh;
	size_t i = 0;
	do {

		if (ReceiveBytes((char*)(&nextCh), 1)!= SUCESS) {

			return FAILURE;
		}

		buffer[i++] = nextCh;

	} while (nextCh != 10 && i< maxLen);

	buffer[i-1] = '\0';

	return SUCESS;
}

int Client::ClientLoop() {
  
	_thread = std::make_shared<std::thread>(&Client::ClientLoopThread, this);

	return SUCESS;
}

void Client::ClientLoopThread() {

	RequestType request;
	while (_isConnected) {

		//receive a request and then handle this request
		ReceiveBytes((char*)(&request), sizeof(RequestType));

		//the request may be a login request
		if ((!_isLoggedIn || !_isConnected) && request == REQUEST_TYPE_LOGIN_REQUEST) {

			if (HandleLogin() != SUCESS) {
				Console::Err("Login failed!\n");
			}
			else {
				PrepareEncryption();
				_isLoggedIn = true;

				Console::Out("%s (%d) has been logged in successfully with password: %s!",
					_userName.c_str(), _userId, _password.c_str());
			}
		}
		else {
			HandleRequest(request);
		}
	}
}

void Client::PrepareEncryption() {

	_sessionKey[0] = (int)(_clientKey >> 32);
	_sessionKey[1] = (int)_clientKey;
	_sessionKey[2] = (int)(_serverKey >> 32);
	_sessionKey[3] = (int)_serverKey;

	//create decryptor and encryptor
	_decryptor = std::make_shared<Cryption>(_sessionKey);
	for (int i = 0; i < 4; i++) {
		_sessionKey[i] += 50;
	}

	_encryptor = std::make_shared<Cryption>(_sessionKey);
}

int Client::HandleRequest(const RequestType& request) {

	uint8_t cmdType = request;
	int packetSize=0;
	uint8_t cmd[1024];

		//Console::Dbg("Received Command: %d", cmd);
		cmdType -= (unsigned char)_decryptor->getNextKey();
		packetSize = packetSizes[cmdType];

		if (packetSize==-1) {
			uint8_t byte;
			ReceiveByte(byte);
			packetSize=byte;
		}

		Console::Dbg("Packet (type, size): (%d, %d)", cmdType, packetSize);

		if (cmdType == 121) {
			Console::Out("loading finished!");
		}
		else if (cmdType == 185) {
			Console::Out("handle action button click!");
		}
		else if (cmdType==98 || cmdType == 164) {
			HandleMoving(packetSize);
		}
		else if (packetSize != 0) {
			ReceiveBytes((char*)(&cmd), packetSize);
		}


	return SUCESS;
}

int Client::SendServerReply(const RequestReplyServer_ptr& reply) {

	auto temp = reply->serverSessionKey;
	reply->serverSessionKey = ReverseBytes(reply->serverSessionKey);
	auto ret=SendBytes((char*)(reply.get()), sizeof(RequestReplyServer));
	reply->serverSessionKey= temp;
	
	return ret;
}

int Client::SendServerReply2(const RequestReplyServer2_ptr& reply){

	 SendBytes((char*)(&reply->responseCode), sizeof(uint8_t));
	 SendBytes((char*)(&reply->playerStatus), sizeof(uint8_t));
     return SendBytes((char*)(&reply->flagged), sizeof(uint8_t));
}

template<class T>
T Client::ReverseBytes(const T& num) {

	T newNum;
	const int byteCount = sizeof(T);

	char buffer[byteCount];
	memset(buffer, 0, byteCount);

	for (int i = 0; i < byteCount; i++) {
		buffer[byteCount - i-1] = num >> i * 8 & 0xFF;
	}

	memcpy(&newNum, &buffer[0], byteCount);
	return newNum;
}

template<class T>
int Client::SendVariable(const T& var) {

	T temp = ReverseBytes(var);

	return SendBytes((char*)(&temp), sizeof(T));
}

int Client::SendByte(const uint8_t& byte) {
	return SendBytes((char*)(&byte), sizeof(byte));
}

template<class T>
int Client::ReceiveVariable(T& var, const bool& reverse) {

	T temp;

	auto ret=ReceiveBytes((char*)(&temp), sizeof(T));

	if (reverse) {
		var = ReverseBytes(temp);
	}
	else {
		var = temp;
	}
	return ret;
}

int Client::ReceiveSignedWordBigEndianA(short& val) {

	char buffer[2];
	auto ret = ReceiveBytes(&buffer[0], 2);

	int temp=val = ((buffer[1] & 0xff) << 8)
		+ (buffer[0] - 128 & 0xff);
	if (temp > 32767)
		temp -= 0x10000;

	val = temp;

	return ret;
}

int Client::ReceiveByte(uint8_t& byte) {

	return ReceiveBytes((char*)(&byte), 1);
}

int Client::ReceiveByteUnsigned(int8_t& byte) {
	return ReceiveBytes((char*)(&byte), 1);
}

RequestReplyClient_ptr Client::ReceiveClientReply(){

	Console::Dbg("receiving client reply...\n");
	RequestReplyClient_ptr reply= std::make_shared<RequestReplyClient>();

	ReceiveBytes((char*)(&reply->connectStatus), sizeof(char));
	ReceiveBytes((char*)(&reply->size), sizeof(char));
	ReceiveBytes((char*)(&reply->magicNumber), sizeof(char));
	ReceiveBytes((char*)(&reply->revision), sizeof(short));
	reply->revision=ReverseBytes(reply->revision);
	ReceiveBytes((char*)(&reply->clientVersion), sizeof(char));
    
	//check if the received magic numbers are ok
	if (reply->magicNumber!=255 || reply->revision != 317) {
		Console::Err("Invalid magic numbers! Received %d, %d expected 255, 317\n",
			    reply->magicNumber, reply->revision);

		return nullptr;
	}

	//These bytes have no meanings right now
	for (int i = 0; i < 9; i++) {
		int32_t temp = 0;
		ReceiveBytes((char*)(&temp), sizeof(int32_t));
		reply->crcValues[i]=ReverseBytes(temp);
	}

	ReceiveBytes((char*)(&reply->size2), sizeof(char));
	ReceiveBytes((char*)(&reply->byte10), sizeof(char));

	if (reply->byte10 != 10) {
		Console::Err("Encrypted packet id is %d but expected 10\n", reply->byte10);
		return nullptr;
	}

	ReceiveBytes((char*)(&reply->clientSessionKey), sizeof(uint64_t));
	reply->clientSessionKey = ReverseBytes(reply->clientSessionKey);

	ReceiveBytes((char*)(&reply->serverSessionKey), sizeof(uint64_t));
	reply->serverSessionKey = ReverseBytes(reply->serverSessionKey);

	ReceiveBytes((char*)(&reply->userId), sizeof(int));
	reply->userId = ReverseBytes(reply->userId);

	ReceiveString(reply->userName, MAX_NAME);
	ReceiveString(reply->password, MAX_NAME);

	_serverKey = reply->serverSessionKey;
	_clientKey = reply->clientSessionKey;

	_userName = reply->userName;
	_password = reply->password;
	_userId = reply->userId;
	
	return reply;
}

int Client::HandleLogin() {

	//reecive name hash, has no use right now
	uint8_t nameHash;
	ReceiveBytes((char*)(&nameHash), sizeof(uint8_t));

	RequestReplyServer_ptr reply = std::make_shared<RequestReplyServer>();
	for (int i = 0; i < 8; i++) {
		reply->unused[i] = 0;
	}

	reply->responseCode = 0;
	reply->serverSessionKey  = GenerateSSAKey();
	Console::Dbg("SSA Key=%ld\n", reply->serverSessionKey);

	if (SendServerReply(reply)!= SUCESS) {
		return FAILURE;
	}
	if (ReceiveClientReply() == nullptr) {
		return FAILURE;
	}

	RequestReplyServer2_ptr reply2= std::make_shared<RequestReplyServer2>();

	reply2->responseCode = RESPONSE_CODE_SUCCESSFUL_LOGIN;
	reply2->playerStatus = PLAYER_STATUS_NORMAL;
	reply2->flagged = 0;

	return SendServerReply2(reply2);
}

uint64_t Client::GenerateSSAKey() {

	uint64_t  serverSessionKey;

	srand((unsigned int)time(0));
	serverSessionKey = ((uint64_t)(rand() * 99999999) << 32);
	srand((unsigned int)time(0));
	serverSessionKey += ((uint64_t)(rand() * 99999999));

	return 1;
}

int Client::Init() {

	for (int i = 0; i < 25; i++) {
		_playerSkills[i].level = i;
		_playerSkills[i].maxLevel = 99;
		_playerSkills[i].xp = 16000000;
	}

	SendPlayerId();

	for (int i = 0; i < 25; i++) 
		SendSkillLevel(i, _playerSkills[i].level, _playerSkills[i].xp);

	//resent the client
	SendCmd(107);

	SetSidebarInterface(1, 3917);
	SetSidebarInterface(2, 638);
	SetSidebarInterface(3, 3213);
	SetSidebarInterface(4, 1644);
	SetSidebarInterface(5, 5608);
	SetSidebarInterface(6, 1151);
	SetSidebarInterface(7, 1);		
	SetSidebarInterface(8, 5065);
	SetSidebarInterface(9, 5715);
	SetSidebarInterface(10, 2449);
	SetSidebarInterface(11, 4445);
	SetSidebarInterface(12, 147);
	SetSidebarInterface(13, 6299);
	SetSidebarInterface(0, 2423);


	Buffer buffer;
	buffer.AddNegativeByte(3);
	buffer.AddByteA(0);
	buffer.AddString("Trade with");
	SendVarCmd(104, buffer);

	OpenWelcomeScreen(201, false, 3, (127 << 24) + 1, (int)rand() % 10);

	SendStrMessage("Welcome to Runescape!");

	char szTitle[1024];
	sprintf(szTitle, "Runescape Server v%d (%d player(s) online!)\n", 1, 1);
	SendStrMessage(std::string(szTitle));
	sprintf(szTitle, "Server Uptime: %s\n", "5");
	SendStrMessage(std::string(szTitle));
	SendStrMessage("Base Created by Matias\n");

	LoadInventory();
	LoadBank();

	SendEquipment(_equipment[playerHat].itemId, _equipment[playerHat].amount, playerHat);
	SendEquipment(_equipment[playerCape].itemId, _equipment[playerCape].amount, playerCape);
	SendEquipment(_equipment[playerAmulet].itemId, _equipment[playerAmulet].amount, playerAmulet);
	SendEquipment(_equipment[playerArrows].itemId, _equipment[playerArrows].amount, playerArrows);
	SendEquipment(_equipment[playerChest].itemId, _equipment[playerChest].amount, playerChest);
	SendEquipment(_equipment[playerShield].itemId, _equipment[playerShield].amount, playerShield);
	SendEquipment(_equipment[playerLegs].itemId, _equipment[playerLegs].amount, playerLegs);
	SendEquipment(_equipment[playerHands].itemId, _equipment[playerHands].amount, playerHands);
	SendEquipment(_equipment[playerFeet].itemId, _equipment[playerFeet].amount, playerFeet);
	SendEquipment(_equipment[playerRing].itemId, _equipment[playerRing].amount, playerRing);
	SendEquipment(_equipment[playerWeapon].itemId, _equipment[playerWeapon].amount, playerWeapon);

	//UpdatePlayer();

	_isInited = true;
	return SUCESS;
}

void Client::SendSkillLevel(int skillNum, int currentLevel, int XP) {

	SendCmd(134);
	SendBytes((char*)(&skillNum), sizeof(uint8_t));

	int temp = ReverseBytes(XP);
	SendBytes((char*)(&XP), sizeof(XP));

	SendBytes((char*)(&currentLevel), sizeof(uint8_t));
}

void Client::SendPlayerId() {

	//send world
	SendCmd(249);
	uint8_t world = 1;//+_encryptor->getNextKey();
	SendBytes((char*)(&world), sizeof(uint8_t));

	short temp = ReverseBytes(_userId);
	SendBytes((char*)(&temp), sizeof(temp));
}

void Client::OpenWelcomeScreen(const uint8_t& recoveryChange, const bool& memberWarning,
	                           const short& unreadMessageCount,
	                           const int& lastLoginIP, const short& lastLogin) {

	SendCmd(176);

	SendNegByte(recoveryChange);
	SendWordA(unreadMessageCount);
	SendByte(memberWarning ? 1 : 0);

	SendDWord_v2(lastLoginIP);
	SendVariable(lastLogin);
}

int Client::SetSidebarInterface(const uint8_t& menuId, const short& form) {

	SendCmd(71);
	SendVariable(form);

	return SendByteA(menuId);
}

int Client::SendString(const std::string& str) {

	const int MAX_BUFFER = 1024;

	char buffer[MAX_BUFFER];

	const uint8_t strLen = (uint8_t)str.length();
	memcpy(&buffer[0] , str.c_str(), strLen);
	buffer[strLen] = 10;

	return SendBytes(buffer, strLen+1);
}

int Client::SendStrMessage(const std::string& str) {

	SendCmd(253);               //command
	SendByte((uint8_t)(str.length()+1));   //packet size
	return SendString(str);     //string
}

int Client::LoadInventory() {


	for (int i = 0; i < 28; i++) {
		_inventory[i].itemId = i;
		_inventory[i].amount = 5;
	}


	Buffer buffer;

	buffer.AddVariable((short)(3214));
	buffer.AddVariable((short)(28));

	for (int i = 0; i < 28; i++) {

		buffer.AddVariable(_inventory[i].itemId);
		buffer.AddVariable(_inventory[i].amount);
	}

	return SendBigVarCmd(53, buffer);
}

int Client::LoadBank() {

const int BANK_COUNT = 18;

	for (int i = 0; i < BANK_COUNT; i++) {
		_bank[i].itemId = i;
		_bank[i].amount = 5;
	}

	Buffer buffer;

	buffer.AddVariable((short)(3214));
	buffer.AddVariable((short)(18));

	for (int i = 0; i < BANK_COUNT; i++) {

		buffer.AddVariable(_bank[i].itemId);
		buffer.AddVariable(_bank[i].amount);
	}

	return SendBigVarCmd(53, buffer);

}

int Client::SendEquipment(int wearID, int amount, int targetSlot) {

	Buffer buffer;

	buffer.AddVariable((short)(1688));
	buffer.AddByte(targetSlot);
	buffer.AddVariable((short)(wearID + 1));
	if (amount > 254) {
		buffer.AddByte(255);
		buffer.AddVariable((int)(amount));
	}
	else {
		buffer.AddByte(amount);
	}

	return SendBigVarCmd(34, buffer);

	/*outStream->createFrameVarSizeWord(34);
	outStream->writeWord(1688);
	outStream->writeByte(targetSlot);
	outStream->writeWord(wearID + 1);
	if (amount > 254) {
		outStream->writeByte(255);
		outStream->writeDWord(amount);
	}
	else {
		outStream->writeByte(amount); //amount	
	}
	outStream->endFrameVarSizeWord();
	updateRequired = true;
	appearanceUpdateRequired = true;*/
}

int Client::UpdatePlayer() {

	Buffer buffer;

	buffer.writeWordA(55);
	buffer.AddVariable((short)60);

	return SendCmd(73, buffer);
}

bool Client::IsLocatedWithinDistance(const Client_ptr& otherClient) {

	if (_heightLevel != otherClient->_heightLevel) return false;
	int deltaX = otherClient->_absolutePos.x - _absolutePos.x, deltaY = otherClient->_absolutePos.y - _absolutePos.y;
	return deltaX <= 15 && deltaX >= -16 && deltaY <= 15 && deltaY >= -16;
}

void Client::GetNextPlayerMovement() {

	_dir1 = -1;
	_dir2 = -1;

	Console::Dbg("GetNextPlayerMovement");

	Console::Dbg("Here ... (%d,%d)", GetTelPortPos().x, GetTelPortPos().y);
	if (GetTelPortPos().x != -1 && GetTelPortPos().y != -1) {
		
		SetMapRegionChanged(true);

		if (GetMapRegionPos().x != -1 && GetMapRegionPos().y != -1) {
			// check, whether destination is within current map region
			int relX = GetTelPortPos().x - GetMapRegionPos().x * 8;
			int relY = GetTelPortPos().y - GetMapRegionPos().y * 8;
			if (relX >= 2 * 8 && relX < 11 * 8 && relY >= 2 * 8 && relY < 11 * 8)
				SetMapRegionChanged(false);
		}

		if (IsMapRegionChanged()) {

			_mapRegionPos.x = (GetTelPortPos().x >> 3) - 6;
			_mapRegionPos.y = (GetTelPortPos().y >> 3) - 6;


			// completely rebuild playerList after teleport AND map region change
			//TODO:
			//p->playerList.erase(p->playerList.begin(), p->playerList.end());
		}

		_absolutePos = GetTelPortPos();

		ResetWalkingQueue();

		_teleportPos.x = -1;
		_teleportPos.y = -1;
		
		_isTeleporting = false;
		_isTeleported = true;
	}
	else {
		
		_dir1 = GetNextWalkingDirection();
		if (_dir1 == -1)
			return;		// standing
		if (_isRunning && HasNextStep())
			_dir2 = GetNextWalkingDirection();

		Point mapRegion = GetMapRegionPos();
		Point absPos = GetAbsPos();

		// check, whether destination is within current map region
		int relX = absPos.x - mapRegion.x * 8;
		int relY = absPos.y - mapRegion.y * 8;

		if (relX >= 2 * 8 && relX < 11 * 8 && relY >= 2 * 8 && relY < 11 * 8) {
			SetMapRegionChanged(false);
		}
		else {
			Point mapRegionChange;
			
			mapRegionChange.x = (GetAbsPos().x >> 3) - 6;
			mapRegionChange.y = (GetAbsPos().y >> 3) - 6;
			_mapRegionPos = mapRegionChange;
			SetMapRegionChanged(true);
		}
	}
}

int Client::GetNextWalkingDirection() {
	// returns 0-7 for next walking direction or -1, if we're not moving

	Console::Dbg(" GetAbsPos(%d,%d)", GetAbsPos().x, GetAbsPos().y);
	Console::Dbg(" GetAbsPos(%d,%d)", _pathWaypoints.front().x, _pathWaypoints.front().y);
   // If referenced player is at the current waypoint, go to next waypoint
	if (cmpPos(_pathWaypoints.front(), GetAbsPos())) {
		_pathWaypoints.erase(_pathWaypoints.begin()); //remove first waypoint as we are standing right on it.

	}
	// Ends processing if there is no next step
	if (!HasNextStep())
		return -1;

	int nextX = GetAbsPos().x;
	int nextY = GetAbsPos().y;

	if (nextX > _pathWaypoints.front().x)
		--nextX;
	else if (nextX < _pathWaypoints.front().x)
		++nextX;

	if (nextY > _pathWaypoints.front().y)
		--nextY;
	else if (nextY < _pathWaypoints.front().y)
		++nextY;

	Point nextPos;
	nextPos.x = nextX;
	nextPos.y = nextY;

	if (!cmpPos(GetAbsPos(), nextPos)) {
		Point lastPos = GetAbsPos();
		_absolutePos = nextPos; 
		return GetDirection(GetAbsPos(), lastPos);
	}
	return -1;
}

bool Client::HasNextStep() {
	return !_pathWaypoints.empty();  
}

int Client::GetDirection(Point startPos, Point endPos) {
	int deltaX = startPos.x - endPos.x;
	int deltaY = startPos.y - endPos.y;

	if (deltaX < 0) {
		if (deltaY > 0)
			return 7; // North-West
		if (deltaY == 0)
			return 6; // West
		if (deltaY < 0)
			return 5; // South-West
	}

	if (deltaX > 0) {
		if (deltaY < 0)
			return 3; // South-East
		if (deltaY == 0)
			return 2; // East
		if (deltaY > 0)
			return 1; // North-East
	}

	if (deltaX == 0) {
		if (deltaY > 0)
			return 0; // North
		if (deltaY < 0)
			return 4; // South
	}
	return -1;
}

void Client::AddToWalkingQueue( int x, int y) {

	Point newWaypoint;

	newWaypoint.x = x;
	newWaypoint.y = y;

	if (_pathWaypoints.size() < WALKING_QUEUE_SIZE)
		_pathWaypoints.push_back(newWaypoint);
	else
		Console::Err("Walking queue is full!\n");
}

void Client::ResetWalkingQueue() {
	_pathWaypoints.clear();
}

void Client::HandleMoving(const int& packetSize) {

	Console::Out("handling movement for player: %s", _userName.c_str());

	int newWalkCmdSteps = (packetSize - 5) / 2;

	if (newWalkCmdSteps > WALKING_QUEUE_SIZE) {
		Console::Out("Warning: walkTo command contains too many steps (%d).",  newWalkCmdSteps);
		return;
	}

	ResetWalkingQueue();	
	Point temp;
	std::vector<Point> tempPathWaypoints;

	short  firstStepX;
	ReceiveSignedWordBigEndianA(firstStepX); //absPos

	for (int i = 0; i < newWalkCmdSteps; i++) {

		int8_t tempByte;
		ReceiveByteUnsigned(tempByte);
		temp.x = tempByte;

		ReceiveByteUnsigned(tempByte);
		temp.y = tempByte;
		tempPathWaypoints.push_back(temp);
	}

	short firstStepY;
	ReceiveVariable(firstStepY, false);

	uint8_t isRunning;
	ReceiveByte(isRunning);

	isRunning = isRunning == 1;

	Console::Out("(firstStepX, firstStepY)=(%d:%d)", firstStepX, firstStepY);


	AddToWalkingQueue(firstStepX, firstStepY);
	for (int i = 0; i < newWalkCmdSteps; i++) {
		tempPathWaypoints[i].x += firstStepX;
		tempPathWaypoints[i].y += firstStepY;
		AddToWalkingQueue(tempPathWaypoints[i].x, tempPathWaypoints[i].y);
	}
}




