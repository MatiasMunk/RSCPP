#pragma once

#include "socket_impl.h"

#include "statuscodes.h"
#include "type.h"
#include <cstring>
#include <vector>

#include "type.h"
#include <mutex>

/* System Includes */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_PLAYERS 100

//a class which represent a game client
class Buffer;
struct Point;
class Client {
public:

	struct Point {
		short x;
		short y;
	};

	struct Hit {
		int dmg;
		bool poisoned;
	};

	struct Apperance { //Player apperance (makes life easier).
				//#Male#	#Female#
		int head;	//0-9		42-55
		int beard;	//10-17		Not Exist
		int body;	//18-25		56-60
		int arms;	//26-32		61-66
		int hands;	//33-35		67-69
		int legs;	//36-41		70-78
		int feet;	//42-44		79-81
		//colors
		int hairColor;
		int bodyColor;
		int legColor;
		int feetColor;
		int skinColor; //skin color (0-6)
	};

	struct GameItem {
		short itemId;
		int amount; //2,147,000,000 +++ ;)
	};

	Client(const SOCKET& socket);

	//client main loop
	int ClientLoop();

	bool IsInited() { return _isInited; }

	bool IsMapRegionChanged() {
		return _isMapRegionChanged;
	}
	void SetMapRegionChanged(const bool& val) {
		_isMapRegionChanged = val;
	}

	bool HasTeleported() {
		return _isTeleported;
	}

	void SetHasTeleported(const bool& flag) {
		_isTeleported = flag;
	}

	bool IsUpdateRequired() {

		return _updateRequired;
	}

	bool IsChatRequireUpdate(){
		return _isRequireChatUpdate;
	}

	void SetChatReqireUpdate(const bool& flag) {
		_isRequireChatUpdate = flag;
	}

	bool IsTeleporting() { return _isTeleporting; }

	void SetIsUpdateRequired(const bool& flag) {
		_updateRequired = flag;
	}

	bool GetIsCreateItem() {
		return _isCreateItems;
	}

	bool IsConnected() {
		return _isConnected;
	}

	bool IsLoggedIn() {
		return _isLoggedIn;
	}

	bool IsRequireAppearenceUpdate() {
		return _isRequireAppearenceUpdate;
	}

	bool IsNpc() {
		return _isNpc;
	}

	bool IsPlat(const short& itemId);
	bool IsFullHelm(const short& itemId);
	bool IsFullMask(const short& itemId);

	int GetDir1() { return _dir1; }
	int GetDir2() { return _dir2; }

	uint8_t GetHeightLevel() { return _heightLevel; }

	int Init();

    Point GetAbsPos() { return _absolutePos; }
	Point GetMapRegionPos() { return _mapRegionPos; };

	bool IsLocatedWithinDistance(const Client_ptr& otherClient);

	int GetId() { return _userId; }
	std::string GetName() { return _userName; }

	SOCKET GetSocket() { return _socket;}

	Cryption* GetEncrypor() { return _encryptor.get(); }

	//TODO
	unsigned char _playerInListBitmap[(MAX_PLAYERS + 7) >> 3];


	uint8_t GetGender() { return _gender; }
	bool    IsSkulled() { return _skulled; }
	int  GetCombatLevel() { return _combatLevel; }

	Apperance GetApperance() { return _apperence; }
	GameItem GetEquipmemt(const uint8_t& i) {
		return _equipment[i];
	}

	int GetNpc() { return _playerNpc; }

	Point GetTelPortPos() {
		return _teleportPos;
	}

	void GetNextPlayerMovement();
	bool HasNextStep();

	void AddToWalkingQueue(int x, int y);
	void ResetWalkingQueue();

	void HandleMoving(const int& packetSize);

protected:

	void ClientLoopThread();

	int HandleLogin();
	int HandleRequest(const RequestType& request);

	uint64_t GenerateSSAKey();

	//create encryption objects
	void PrepareEncryption();

	void SendSkillLevel(int skillNum, int currentLevel, int XP);
	void SendPlayerId();
	void OpenWelcomeScreen(const uint8_t& recoveryChange, const bool& memberWarning,
							const short& unreadMessageCount,
							const int& lastLoginIP, const short& lastLogin);

	int SetSidebarInterface(const uint8_t& menuId, const short& form);

public:
	int SendBytes(char* buffer, const size_t& len);
	int ReceiveBytes(char* buffer, const size_t& len);
protected:

	int SendServerReply(const RequestReplyServer_ptr& reply);
	RequestReplyClient_ptr ReceiveClientReply();
	int SendServerReply2(const RequestReplyServer2_ptr& reply);

	
	int ReceiveString(char* buffer, const size_t& maxLen);

	template<class T>
	int SendVariable(const T& var);
	int SendByte(const uint8_t& byte);

	int SendCmd(const uint8_t& cmdType);

	//a known command size, we do not add size
	int SendCmd(const uint8_t& cmdType, Buffer& buffer);

	//unknown size to the client, we ass the size
	int SendVarCmd(const uint8_t& cmdType, Buffer& buffer);

	//same as SendVarCmd but the size is a bit big for uint8_t
	int SendBigVarCmd(const uint8_t& cmdType, Buffer& buffer);

	template<class T>
	int ReceiveVariable( T& var, const bool& reverse);
	int ReceiveByte(uint8_t& byte);
	int ReceiveByteUnsigned(int8_t& byte);

	int ReceiveSignedWordBigEndianA(short &val);

	//reverse the bits of 64 bit long
	template<class T>
	T ReverseBytes(const T& num);

	int SendWordA(const short& i);
	int SendDWord_v2(const int& i);

	int SendByteA(const uint8_t& i);
	int SendNegByte(const uint8_t& i);
	int SendString(const std::string& str);
	int SendStrMessage(const std::string& str);
	int LoadInventory();
	int LoadBank();

	int SendEquipment(int wearID, int amount, int targetSlot);

	int UpdatePlayer();

	int GetNextWalkingDirection();
	int GetDirection(Point startPos, Point endPos);

private:

	SOCKET _socket;

	//server and client keys
	uint64_t _serverKey;
	uint64_t _clientKey;

	//session key array
	unsigned int _sessionKey[4];

	//encryption and decryption objects
	Cryption_ptr  _decryptor;
	Cryption_ptr  _encryptor;

	//flags to check the client status
	bool _isLoggedIn;
	bool _isConnected;
	bool _isInited;
	bool _isTeleporting;
	bool _isRequireAppearenceUpdate;
	bool _isRunning;

	bool _isRequireChatUpdate;
	bool _isNpc;
	

	std::string _userName;
	int    _userId;
	std::string _password;

	struct Level {
		unsigned char level;	//this is the temp level which can get increased/decreased by potions/temp stat transfer system etc..
		unsigned char maxLevel; //calculated from getLevelForXP
		int xp; //2,147,000,000 +++ ;)
	};
	Level _playerSkills[25];

	GameItem _inventory[28];
	GameItem _bank[800];
	GameItem _equipment[14];


	Point  _absolutePos;
	Point  _mapRegionPos;
	Point  _teleportPos;
	std::vector<Point> _pathWaypoints;


	Thread_ptr  _thread;

	bool _isMapRegionChanged;
	bool _isTeleported;

	uint8_t _heightLevel;

	bool _updateRequired;

	// direction char is going in this cycle
	int _dir1;		
	int _dir2;

	bool _isCreateItems;

	std::mutex  _socketMutex;

	//info
	uint8_t _gender;
	bool    _skulled;
	int     _combatLevel;

	Apperance  _apperence;

	int _playerNpc;
};
