#include "gameserver.h"

#include "world.h"
#include "config.h"

#include "console.h"
#include "util.h"

#include "client.h"
#include "stream.h"

#include <array>
#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include "constants.h"

void Gameserver::Initialize(unsigned short port, unsigned short maxconnections, std::array<std::string, 6> dbinfo, const Config& server_config, const Config& admin_config)
{
	this->maxconnections = maxconnections;

	this->world = new World(dbinfo, server_config, admin_config);

	serverRunning = true;

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	/* create our listner socket */
	listener = socket(AF_INET, SOCK_STREAM, 0);

	if (listener == INVALID_SOCKET)
		return;

	/* bind our listener socket to a port(port 43594) */
	if (bind(listener, (sockaddr*)&server, sizeof(server)) != 0) {
		Console::Err("Failed to bind on port %i (Already in use?)", port);
		return;
	}

	/* listen for a connection(s) */
	if (listen(listener, 5) != 0)
		return;

	_playerList.clear();

	_playerThread = std::make_shared<std::thread>(&Gameserver::PlayerUpdateThreadFunc, this);

	_stream = std::make_shared<Stream>();
	_playerProps = std::make_shared<Stream>();
	_updateBlock = std::make_shared<Stream>();
}

void Gameserver::Tick() {
	/* Server loop */
	sockaddr_in connection;
	//memset(&connection, 0, sizeof(connection));
	//int connectionSize = sizeof(connection);

	sockaddr_in sin;
	socklen_t connectionSize = sizeof(sockaddr_in);

	/* Game loop */
	while (serverRunning)
	{
		auto socket = accept(listener, (struct sockaddr*) & connection, &connectionSize);
		Console::Out("New connection from %s (%i/%i connections)", inet_ntoa(connection.sin_addr), 1, maxconnections);

		ClientHandler(socket);
	}
}

void Gameserver::ClientHandler(const SOCKET& socket) {

	Client_ptr newPlayer = std::make_shared<Client>(socket);
	_playerList.emplace_back(newPlayer);
	newPlayer->ClientLoop();
}

Gameserver::~Gameserver()
{
	serverRunning = false;
	close(listener);
}

void Gameserver::PlayerUpdateThreadFunc() {

	while (true) {

		//initialize players if they have not been initialized
		for (const auto& player: _playerList) {

			if (!player->IsInited()) {
				player->Init();
			}
		}

		//update players
		for (const auto& player : _playerList) {
			if (player->HasNextStep() || player->IsTeleporting()) {
				
				player->GetNextPlayerMovement();
			}
			
			UpdatePlayer(player);
		}

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
	}
}

void Gameserver::UpdatePlayer(const Client_ptr& player) {

	_updateBlock->currentOffset = 0;
	
	UpdatePlayerMovement(player);

	// do NOT send chat text back to thisPlayer!
	bool saveChatTextUpdate = player->IsChatRequireUpdate();
	player->SetChatReqireUpdate(false);
	AppendPlayerUpdateBlock(player, _updateBlock);
	player->SetChatReqireUpdate(saveChatTextUpdate);

	//TODO
	//_stream->writeBits(8,(_playerList.size()-1));
	_stream->writeBits(8,0);
	
	//player's in my seen list updates
	/*for (const auto& p : _playerList) {

		
		if (!p->HasTeleported() && !p->IsTeleporting() && player->IsLocatedWithinDistance(p)) {
			UpdatePlayerMovement(p);

			//TODO
			AppendPlayerUpdateBlock(p, _updateBlock);
		}
		else {

			//int id = p->GetId();
			//player->_playerInListBitmap[id >> 3] &= ~(1 << (id & 7));		// clear the flag
			//_stream->writeBits(1, 1);
			//_stream->writeBits(2, 3);		
			////plr->playerList.erase(iPlr);
			//continue;
		}
	}

	// iterate through all players to check whether there's new players to add
	for (const auto& p : _playerList) {

		if (!p->IsConnected() || !p->IsLoggedIn() || p == player) continue;

		int id = p->GetId();
		if ((player->_playerInListBitmap[id >> 3] & (1 << (id & 7))) != 0) continue;	// player already in playerList
		if (!player->IsLocatedWithinDistance(p)) continue;		// out of sight

		//addNewPlayer(plr, (*iter), str, updateBlock);
	}*/

	if (_updateBlock->currentOffset > 0) {
		_stream->writeBits(11, 2047);	// magic EOF - needed only when player updateblock follows
		_stream->finishBitAccess();

		// append update block
		_stream->writeBytes(_updateBlock->buffer, _updateBlock->currentOffset, 0);
	}
	else {
		_stream->finishBitAccess();
	}

	_stream->endFrameVarSizeWord();

	if (_stream->currentOffset == 0) {
		printf("Can't flush emtpy stream o.O\n");
		return;
	}

	//send this frame
	player->SendBytes(_stream->buffer, _stream->currentOffset);
	_stream->currentOffset = 0;		// reset
}

void Gameserver::AppendPlayerUpdateBlock(const Client_ptr& player, const Stream_ptr& str) {

	if (!player->IsUpdateRequired() && !player->IsChatRequireUpdate()) return;		// nothing required
	int updateMask = 0;
	if (player->IsChatRequireUpdate()) updateMask |= 0x80;
	if (player->IsRequireAppearenceUpdate()) updateMask |= 0x10;

	if (updateMask >= 0x100) {
		// byte isn't sufficient
		updateMask |= 0x40;			// indication for the client that updateMask is stored in a word
		str->writeByte(updateMask & 0xFF);
		str->writeByte(updateMask >> 8);
	}
	else {
		str->writeByte(updateMask);
	}

	// now writing the various update blocks itself - note that their order crucial
	if (player->IsChatRequireUpdate()) AppendPlayerChatText(player, str);
	if (player->IsRequireAppearenceUpdate()) AppendPlayerAppearance(player, _playerProps, str);

	// TODO: add the various other update blocks

}

uint64_t playerNameToInt64(const char s[]) {
	uint64_t l = 0;
	for (unsigned int i = 0; i < strlen(s) && i < 12; i++) {
		char c = s[i];
		l *= 37L;
		if (c >= 'A' && c <= 'Z') l += (1 + c) - 65;
		else if (c >= 'a' && c <= 'z') l += (1 + c) - 97;
		else if (c >= '0' && c <= '9') l += (27 + c) - 48;
	}
	while (l % 37L == 0L && l != 0L) l /= 37L;
	return l;
}
void Gameserver::AppendPlayerAppearance(const Client_ptr& player, const Stream_ptr& playerProps, const Stream_ptr& str) {

	playerProps->currentOffset = 0;

	// TODO: yet some things to figure out on this block + properly implement this
	playerProps->writeByte(player->GetGender());		// player sex. 0=Male and 1=Female
	playerProps->writeByte(player->IsSkulled());		// playerStatusMask - skull, prayers etc alkup 0

	// defining the character shape - 12 slots following - 0 denotes a null entry and just a byte is used
	// slot 0,8,11,1 is head part - missing additional things are beard and eyepatch like things either 11 or 1
	// cape, apron, amulet... the remaining things...

	if (!player->IsNpc()) {
		if (player->GetEquipmemt(playerHat).itemId > 1)
			playerProps->writeWord(0x200 + player->GetEquipmemt(playerHat).itemId);
		else
			playerProps->writeByte(0);

		if (player->GetEquipmemt(playerCape).itemId > 1)
			playerProps->writeWord(0x200 + player->GetEquipmemt(playerCape).itemId);
		else
			playerProps->writeByte(0);

		if (player->GetEquipmemt(playerAmulet).itemId > 1)
			playerProps->writeWord(0x200 + player->GetEquipmemt(playerAmulet).itemId);
		else
			playerProps->writeByte(0);

		if (player->GetEquipmemt(playerWeapon).itemId > 1)
			playerProps->writeWord(0x200 + player->GetEquipmemt(playerWeapon).itemId);
		else
			playerProps->writeByte(0);

		if (player->GetEquipmemt(playerChest).itemId > 1)
			playerProps->writeWord(0x200 + player->GetEquipmemt(playerChest).itemId);
		else
			playerProps->writeWord(0x100 + player->GetApperance().body);

		if (player->GetEquipmemt(playerShield).itemId > 1)
			playerProps->writeWord(0x200 + player->GetEquipmemt(playerShield).itemId);
		else
			playerProps->writeByte(0);


		if (!player->IsPlat(player->GetEquipmemt(playerChest).itemId))
			playerProps->writeWord(0x100 + player->GetApperance().arms);
		else
			playerProps->writeByte(0);

		if (player->GetEquipmemt(playerLegs).itemId > 1)
			playerProps->writeWord(0x200 + player->GetEquipmemt(playerLegs).itemId);
		else
			playerProps->writeWord(0x100 + player->GetApperance().legs);

		if (!player->IsFullHelm(player->GetEquipmemt(playerHat).itemId) &&
			    !player->IsFullMask(player->GetEquipmemt(playerHat).itemId))
			playerProps->writeWord(0x100 + player->GetApperance().head);		// head
		else
			playerProps->writeByte(0);

		if (player->GetEquipmemt(playerHands).itemId > 1)
			playerProps->writeWord(0x200 + player->GetEquipmemt(playerHands).itemId);
		else
			playerProps->writeWord(0x100 + player->GetApperance().hands);

		if (player->GetEquipmemt(playerFeet).itemId > 1)
			playerProps->writeWord(0x200 + player->GetEquipmemt(playerFeet).itemId);
		else
			playerProps->writeWord(0x100 + player->GetApperance().feet);

		if (!player->IsFullHelm(player->GetEquipmemt(playerHat).itemId) &&
			     !player->IsFullMask(player->GetEquipmemt(playerHat).itemId) &&
			       player->GetGender() == 0)
			playerProps->writeWord(0x100 + player->GetApperance().beard);
		else
			playerProps->writeByte(0); // 0 = nothing on and girl don't have beard
	}
	else {
		playerProps->writeWord(-1);
		playerProps->writeWord(player->GetNpc());
	}

	// array of 5 bytes defining the colors
	playerProps->writeByte(player->GetApperance().hairColor);	// hair color
	playerProps->writeByte(player->GetApperance().bodyColor);	// torso color.
	playerProps->writeByte(player->GetApperance().legColor);	// leg color
	playerProps->writeByte(player->GetApperance().feetColor);	// feet color
	playerProps->writeByte(player->GetApperance().skinColor);	// skin color (0-6)

	// player animation indices
	playerProps->writeWord(0x328);		// standAnimIndex
	playerProps->writeWord(0x337);		// standTurnAnimIndex
	playerProps->writeWord(0x333);		// walkAnimIndex
	playerProps->writeWord(0x334);		// turn180AnimIndex
	playerProps->writeWord(0x335);		// turn90CWAnimIndex
	playerProps->writeWord(0x336);		// turn90CCWAnimIndex
	playerProps->writeWord(0x338);		// runAnimIndex



	playerProps->writeQWord(playerNameToInt64(player->GetName().c_str()));
	//player->SetCombatLevel((int)((double)player->GetLevel(0).level * 0.32707 + (double)player->getLevel(1).level * 0.249 + (double)p->getLevel(2).level * 0.324 + (double)p->getLevel(3).level * 0.25 + (double)p->getLevel(5).level * 0.124));

	playerProps->writeByte(player->GetCombatLevel());		// combat level
	playerProps->writeWord(0);		// incase != 0, writes skill-%d

	str->writeByteC(playerProps->currentOffset);		// size of player appearance block
	str->writeBytes(playerProps->buffer, playerProps->currentOffset, 0);
}


void Gameserver::AppendPlayerChatText(const Client_ptr& player, const Stream_ptr& str) {

	/*str->writeWordBigEndian(((p->chatTextColor & 0xFF) << 8) + (p->chatTextEffects & 0xFF));
	str->writeByte(p->getPlayerRights());
	str->writeByteC(p->chatTextSize);		// no more than 256 bytes!!!
	str->writeBytes_reverse(p->chatText, p->chatTextSize, 0);*/
}



static char xlateDirectionToClient[] = { 1, 2, 4, 7, 6, 5, 3, 0 };
static char directionDeltaX[] = { 0, 1, 1, 1, 0,-1,-1,-1 };
static char directionDeltaY[] = { 1, 1, 0,-1,-1,-1, 0, 1 };

void Gameserver::UpdatePlayerMovement(const Client_ptr& player) {
	
	_stream->packetEncryption =player->GetEncrypor();
	// handles anything related to character position, i.e. walking,running and teleportation
	// applies only to the char and the client which is playing it
	if (player->IsMapRegionChanged()) {
		
		_stream->createFrame(73);
		_stream->writeWordA(player->GetMapRegionPos().x + 6);	// for some reason the client substracts 6 from those values
		_stream->writeWord(player->GetMapRegionPos().y + 6);
		 player->SetMapRegionChanged(false);

		 Console::Out("MapRegionChanged: %d, %d\n", player->GetMapRegionPos().x, player->GetMapRegionPos().y);
	}
	

	if (player->HasTeleported()) { //if teleport is completed.
		_stream->createFrameVarSizeWord(81);
		_stream->initBitAccess();
		_stream->writeBits(1, 1);
		_stream->writeBits(2, 3);			// updateType
		_stream->writeBits(2, player->GetHeightLevel());
		_stream->writeBits(1, 1);			// set to true, if discarding (clientside) walking queue
		_stream->writeBits(1, (player->IsUpdateRequired()) ? 1 : 0);

		_stream->writeBits(7, (player->GetAbsPos().y - 8 * player->GetMapRegionPos().y));
		_stream->writeBits(7, (player->GetAbsPos().x - 8 * player->GetMapRegionPos().x));

		Console::Dbg(" GetMapRegionPos(%d,%d)", player->GetMapRegionPos().x, player->GetMapRegionPos().y);
		Console::Dbg(" UpdatePlayerMovement(%d,%d)", (player->GetAbsPos().y - 8 * player->GetMapRegionPos().y),
			(player->GetAbsPos().x - 8 * player->GetMapRegionPos().x));


		player->SetHasTeleported(false); //finished teleporting.
		return;
	}

	if (player->GetDir1() == -1) {
		// don't have to update the character position, because we're just standing
		_stream->createFrameVarSizeWord(81);
		_stream->initBitAccess();
		if (player->IsUpdateRequired()) {
			// tell client there's an update block appended at the end
			_stream->writeBits(1, 1);
			_stream->writeBits(2, 0);
		}
		else {
			_stream->writeBits(1, 0);
		}
	}
	else {
		if (player->GetIsCreateItem()) { //Creating new items randomly 1038-1048 every update process.
			_stream->createFrame(85);
			_stream->writeByteC((player->GetAbsPos().y - 8 * player->GetMapRegionPos().y));
			_stream->writeByteC((player->GetAbsPos().x - 8 * player->GetMapRegionPos().x));
			_stream->createFrame(44);
			int newItemID = (int)((rand() % 10) + 1038);
			if (newItemID % 2 == 1)
				newItemID++;
			_stream->writeWordBigEndianA(newItemID); //itemId
			_stream->writeWord(5); //amount
			_stream->writeByte(0);  // x(4 MSB) y(LSB) coords
		}

		_stream->createFrameVarSizeWord(81);
		_stream->initBitAccess();
		_stream->writeBits(1, 1);

		if (player->GetDir2() == -1) {
			// send "walking packet"
			_stream->writeBits(2, 1);		// updateType
			_stream->writeBits(3, xlateDirectionToClient[player->GetDir1()]);
			if (player->IsUpdateRequired()) _stream->writeBits(1, 1);		// tell client there's an update block appended at the end
			else _stream->writeBits(1, 0);
		}
		else {
			// send "running packet"
			_stream->writeBits(2, 2);		// updateType
			_stream->writeBits(3, xlateDirectionToClient[player->GetDir1()]);
			_stream->writeBits(3, xlateDirectionToClient[player->GetDir2()]);
			if (player->IsUpdateRequired()) {
				_stream->writeBits(1, 1);
			}
			else _stream->writeBits(1, 0);
		}
	}

}