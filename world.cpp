#include "world.h"

#include "config.h"
#include "database.h"
#include "gameserver.h"
#include "i18n.h"
#include "console.h"
#include "hash.h"
#include "util.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <limits>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

void world_timed_save(void* world_void)
{
	World* world = static_cast<World*>(world_void);

	if (!world->config["TimedSave"])
		return;

	/*for (auto& [key, character] : world->characters)
		character->Save();*/

	try
	{
		world->CommitDB();
	}
	catch (Database_Exception&)
	{
		Console::Wrn("Database commit failed - no data was saved!");
		world->db.Rollback();
	}

	world->BeginDB();
}

void World::UpdateConfig()
{
	this->timer.SetMaxDelta(this->config["ClockMaxDelta"]);

	double rate_walk = this->config["PacketRateWalk"];
	double rate_attack = this->config["PacketRateAttack"];

	std::array<double, 7> npc_speed_table;

	std::vector<std::string> rate_list = util::explode(',', this->config["NPCMovementRate"]);

	for (std::size_t i = 0; i < std::min<std::size_t>(7, rate_list.size()); ++i)
	{
		if (i < rate_list.size())
			npc_speed_table[i] = util::tdparse(rate_list[i]);
		else
			npc_speed_table[i] = 1.0;
	}

	this->i18n.SetLangFile(this->config["ServerLanguage"]);

	if (this->db.Pending() && !this->config["TimedSave"])
	{
		try
		{
			this->CommitDB();
		}
		catch (Database_Exception&)
		{
			Console::Wrn("Database commit failed - no data was saved!");
			this->db.Rollback();
		}
	}
}

World::World(std::array<std::string, 6> dbinfo, const Config& server_config, const Config& admin_config)
	: i18n(server_config.find("ServerLanguage")->second)
	, admin_count(0)
{
	if (int(this->timer.resolution * 1000.0) > 1)
	{
		Console::Out("Timers set at approx. %i ms resolution", int(this->timer.resolution * 1000.0));
	}
	else
	{
		Console::Out("Timers set at < 1 ms resolution");
	}

	this->config = server_config;
	this->admin_config = admin_config;

	Database::Engine engine;

	std::string dbdesc;

	if (util::lowercase(dbinfo[0]).compare("mysql") == 0)
	{
		engine = Database::MySQL;
		dbdesc = std::string("MySQL: ")
			+ dbinfo[2] + "@"
			+ dbinfo[1];

		if (dbinfo[5] != "0" && dbinfo[5] != "3306")
			dbdesc += ":" + dbinfo[5];

		dbdesc += "/" + dbinfo[4];
	}

	Console::Out("Connecting to database (%s)...", dbdesc.c_str());
	this->db.Connect(engine, dbinfo[1], util::to_int(dbinfo[5]), dbinfo[2], dbinfo[3], dbinfo[4]);
	this->BeginDB();

	try
	{
		this->drops_config.Read(this->config["DropsFile"]);
		this->shops_config.Read(this->config["ShopsFile"]);
		this->formulas_config.Read(this->config["FormulasFile"]);
		this->home_config.Read(this->config["HomeFile"]);
		this->skills_config.Read(this->config["SkillsFile"]);
		this->speech_config.Read(this->config["SpeechFile"]);
	}
	catch (std::runtime_error & e)
	{
		Console::Wrn(e.what());
	}

	this->UpdateConfig();

	//Load world
	//Console::Out("%i/%i maps loaded.", loaded, static_cast<int>(this->maps.size()));

	short max_quest = static_cast<int>(this->config["Quests"]);

	this->last_player_id = 0;
	this->last_character_id = 0;

	//Timed npc spawns
	/*TimeEvent* event = new TimeEvent(world_spawn_npcs, this, 1.0, Timer::FOREVER);
	this->timer.Register(event);*/

	//Item despawn
	/*if (this->config["ItemDespawn"])
	{
		event = new TimeEvent(world_despawn_items, this, static_cast<double>(this->config["ItemDespawnCheck"]), Timer::FOREVER);
		this->timer.Register(event);
	}*/

	//Timed saves
	/*if (this->config["TimedSave"])
	{
		event = new TimeEvent(world_timed_save, this, static_cast<double>(this->config["TimedSave"]), Timer::FOREVER);
		this->timer.Register(event);
	}*/
}

void World::BeginDB()
{
	if (this->config["TimedSave"])
		this->db.BeginTransaction();
}

void World::CommitDB()
{
	if (this->db.Pending())
		this->db.Commit();
}

void World::UpdateAdminCount(int admin_count)
{
	this->admin_count = admin_count;

	if (admin_count == 0 && this->config["FirstCharacterAdmin"])
	{
		Console::Out("There are no admin accounts!");
		Console::Out("The next account created will be given Owner status!");
	}
}

int World::GenerateCharacterID()
{
	return this->last_character_id++;
}

int World::GeneratePlayerID()
{
	return this->last_player_id++;
}

//void World::Login(Character* character)
//{
//	this->characters.insert({ character->id, character });
//
//	if (this->GetMap(character->mapid)->relog_x || this->GetMap(character->mapid)->relog_y)
//	{
//		character->x = this->GetMap(character->mapid)->relog_x;
//		character->y = this->GetMap(character->mapid)->relog_y;
//	}
//
//	Map* map = this->GetMap(character->mapid);
//
//	character->Login();
//}

//void World::Logout(Character* character)
//{
//	if (character) {
//		characters.erase(character->player->id);
//		Console::Dbg("Logged out attached character");
//	}
//	else {
//		Console::Dbg("No character attached to player");
//	}
//}

//void World::Logout(Player* player)
//{
//	if (PlayerOnline(player->username)) {
//		//Console::Out("Logging out player: %s", player->username.c_str());
//		Console::Out("Logging out player %i, %s", player->id, player->username.c_str());
//
//		this->Logout(player->character);
//
//		if (players.find(player->id) != players.end()) {
//			Console::Out("Success logout");
//			players.erase(player->id);
//		}
//	}
//}

void World::Rehash()
{
	try
	{
		this->config.Read("config.ini");
		this->admin_config.Read("admin.ini");
		this->drops_config.Read(this->config["DropsFile"]);
		this->shops_config.Read(this->config["ShopsFile"]);
		this->formulas_config.Read(this->config["FormulasFile"]);
		this->home_config.Read(this->config["HomeFile"]);
		this->skills_config.Read(this->config["SkillsFile"]);
		this->speech_config.Read(this->config["SpeechFile"]);
	}
	catch (std::runtime_error & e)
	{
		Console::Err(e.what());
	}

	this->UpdateConfig();
}

bool World::CharacterExists(std::string name)
{
	Database_Result res = this->db.Query("SELECT 1 FROM `characters` WHERE `name` = '$'", name.c_str());
	return !res.empty();
}

//Player* World::Login(const std::string& username, util::secure_string&& password)
//{
//	if (LoginCheck(username, std::move(password)) == LOGIN_WRONG_USERPASS)
//		return 0;
//
//	return new Player(username, this);
//}

//Player* World::Login(std::string username)
//{
//	return new Player(username, this);
//}

//LoginReply World::LoginCheck(const std::string& username, util::secure_string&& password)
//{
//	{
//		util::secure_string password_buffer(std::move(std::string(this->config["PasswordSalt"]) + username + password.str()));
//		password = sha256(password_buffer.str());
//	}
//
//	Database_Result res = this->db.Query("SELECT 1 FROM `accounts` WHERE `username` = '$' AND `password` = '$'", username.c_str(), password.str().c_str());
//
//	if (res.empty())
//	{
//		return LOGIN_WRONG_USERPASS;
//	}
//	else if (this->PlayerOnline(username))
//	{
//		return LOGIN_LOGGEDIN;
//	}
//	else
//	{
//		return LOGIN_OK;
//	}
//}

//bool World::CreatePlayer(const std::string& username, util::secure_string&& password,
//	const std::string& fullname, const std::string& location, const std::string& email,
//	const std::string& computer, const std::string& hdid, const std::string& ip)
//{
//	{
//		util::secure_string password_buffer(std::move(std::string(this->config["PasswordSalt"]) + username + password.str()));
//		password = sha256(password_buffer.str());
//	}
//
//	Database_Result result = this->db.Query("INSERT INTO `accounts` (`username`, `password`, `fullname`, `location`, `email`, `computer`, `hdid`, `regip`, `created`) VALUES ('$','$','$','$','$','$','$','$',#)",
//		username.c_str(), password.str().c_str(), fullname.c_str(), location.c_str(), email.c_str(), computer.c_str(), hdid.c_str(), ip.c_str(), int(std::time(0)));
//
//	return !result.Error();
//}

bool World::PlayerExists(std::string username)
{
	Database_Result res = this->db.Query("SELECT 1 FROM `accounts` WHERE `username` = '$'", username.c_str());
	return !res.empty();
}

//bool World::PlayerOnline(std::string username)
//{
//	if (!Player::ValidName(username))
//	{
//		return false;
//	}
//
//	for (auto& [key, player] : this->players)
//	{
//		if (player->username.compare(username) == 0) {
//			return true;
//		}
//	}
//
//	return false;
//}

World::~World()
{
	UTIL_FOREACH(this->homes, home)
	{
		delete home;
	}

	if (this->config["TimedSave"])
	{
		this->db.Commit();
	}
}
