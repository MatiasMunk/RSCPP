#ifndef GAMESERVER_H
#define GAMESERVER_H

/* System Includes */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <array>

/* Program Includes */
#include "fwd/config.h"
#include "fwd/world.h"

#include "type.h"
#include "socket_impl.h"

class Stream;
class Gameserver
{
private:
	void Initialize(unsigned short port, unsigned short maxconnections, std::array<std::string, 6> dbinfo, const Config& server_config, const Config& admin_config);

public:
	World* world;

	Gameserver(unsigned short port, unsigned short maxconnections, std::array<std::string, 6> dbinfo, const Config& server_config, const Config& admin_config) {
		this->Initialize(port, maxconnections, dbinfo, server_config, admin_config);
	}

	void Tick();

	~Gameserver();

	bool serverRunning;
	int serverSpeed;
	int startTickCount;
	unsigned int maxconnections;

protected:
	void ClientHandler(const SOCKET& socket);


	void PlayerUpdateThreadFunc();

	void UpdatePlayerMovement(const Client_ptr& player);
	void UpdatePlayer(const Client_ptr& player);

	void AppendPlayerUpdateBlock(const Client_ptr& player, const Stream_ptr& str);
	void AppendPlayerChatText(const Client_ptr& player, const Stream_ptr& str);
	void AppendPlayerAppearance(const Client_ptr& player, const Stream_ptr& playerProps, const Stream_ptr& str);
private:
	/* Server Socket variables */
	SOCKET listener;
	SOCKET acceptor;
	sockaddr_in server;		//Server information structure
/* End of Socket Variables */

	ClientList  _playerList;
	Thread_ptr  _playerThread;

	Stream_ptr  _stream;
	Stream_ptr  _playerProps;
	Stream_ptr  _updateBlock;
};

#endif