#ifndef WORLD_H
#define WORLD_H

#include "fwd/world.h"

#include "gameserver.h"
#include "config.h"
#include "database.h"
#include "i18n.h"
#include "timer.h"

#include <array>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct Home
{
	std::string id;
	std::string name;
	short map;
	unsigned char x;
	unsigned char y;
	int level;
	int innkeeper_vend;

	Home() : map(1), x(0), y(0), level(-1), innkeeper_vend(0) { }
};

/**
 * Object which holds and manages all maps and characters on the server, as well as timed events
 * Only one of these should exist per server
 */
class World
{
protected:
	int last_player_id;
	int last_character_id;

	void UpdateConfig();

public:
	Timer timer;

	Gameserver* server;
	Database db;

	Config config;
	Config admin_config;
	Config drops_config;
	Config shops_config;
	Config formulas_config;
	Config home_config;
	Config skills_config;
	Config speech_config;

	I18N i18n;

	/*std::unordered_map<unsigned int, std::unique_ptr<Player>> players;
	std::unordered_map<unsigned int, Character*> characters;*/

	std::vector<Home*> homes;

	int admin_count;

	World(std::array<std::string, 6> dbinfo, const Config& server_config, const Config& admin_config);

	void BeginDB();
	void CommitDB();

	void UpdateAdminCount(int admin_count);
	void IncAdminCount() { UpdateAdminCount(this->admin_count + 1); }
	void DecAdminCount() { UpdateAdminCount(this->admin_count - 1); }

	int GenerateCharacterID();
	int GeneratePlayerID();

	void Rehash();

	bool CharacterExists(std::string name);

	bool PlayerExists(std::string username);
	//bool PlayerOnline(std::string username);

	~World();
};

#endif // WORLD_H