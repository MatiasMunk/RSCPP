#include "server_config.h"

#include "config.h"

#include "console.h"

#include <string>

template <typename T> static void server_config_default(Config& config, const char* key, T value)
{
	if (config.find(key) == config.end())
	{
		config[key] = util::variant(value);
		Console::Wrn("Could not load config value '%s' - using default (%s)", key, std::string(config[key]).c_str());
	}
}

void server_config_validate_config(Config& config)
{
	//Console
	server_config_default(config, "LogOut", "-");
	server_config_default(config, "LogErr", "error.log");
	server_config_default(config, "StyleConsole", true);
	server_config_default(config, "LogCommands", true);

	//Database
	server_config_default(config, "DBType", "mysql");
	server_config_default(config, "DBHost", "localhost");
	server_config_default(config, "DBUser", "root");
	server_config_default(config, "DBPass", "");
	server_config_default(config, "DBName", "");
	server_config_default(config, "DBPort", 0);

	//Server
	server_config_default(config, "ServerName", "RSC++");
	server_config_default(config, "Host", "0.0.0.0");
	server_config_default(config, "Port", 43594);
	server_config_default(config, "MaxConnections", 500);
	server_config_default(config, "MaxPlayers", 500);
	server_config_default(config, "MaxConnectionsPerIP", 3);
	server_config_default(config, "IPReconnectLimit", 10);
	server_config_default(config, "MaxLoginAttempts", 3);
	server_config_default(config, "PasswordSalt", "ChangeMe");
	server_config_default(config, "TimedSave", "5m");
	server_config_default(config, "ServerLanguage", "./lang/en.ini");

	//Rates
	server_config_default(config, "ExpRate", 1.0);
	server_config_default(config, "DropRate", 1.0);
	server_config_default(config, "MobRate", 1.0);
	server_config_default(config, "CriticalRate", 0.08);
	server_config_default(config, "SpawnRate", 1.0);

	//Files
	server_config_default(config, "NewsFile", "./data/news.txt");
	server_config_default(config, "DropsFile", "./data/drops.ini");
	server_config_default(config, "ShopsFile", "./data/shops.ini");
	server_config_default(config, "QuestDir", "./data/quests/");
	server_config_default(config, "Quests", 0);

	//PK
	server_config_default(config, "GlobalWilderness", false);

	//Misc
	server_config_default(config, "FirstCharacterAdmin", true);
	server_config_default(config, "AdminPrefix", "$");
	server_config_default(config, "ProtectPlayerDrop", 60);
	server_config_default(config, "ProtectNPCDrop", 30);
	server_config_default(config, "ProtectPKLoot", 60);
	server_config_default(config, "ProtectDeathDrop", 300);
	server_config_default(config, "ItemDespawn", true);
	server_config_default(config, "ItemDespawnCheck", 60);
	server_config_default(config, "ItemDespawnRate", 600);
	server_config_default(config, "ChatLength", 128);
	server_config_default(config, "StartItems", "");
	server_config_default(config, "StartEquipMale", "0,0,0,0,0,0,0,0,0,0,0,");
	server_config_default(config, "StartEquipFemale", "0,0,0,0,0,0,0,0,0,0,0,");
	server_config_default(config, "DefaultBanLength", "2h");
	server_config_default(config, "Deadly", false);
	server_config_default(config, "NoInteractDefault", 0);
	server_config_default(config, "NoInteractDefaultAdmin", 2);

	//Limits
	server_config_default(config, "MaxSkillLevel", 99);
	server_config_default(config, "MaxExp", 2000000000);

	//Extra
	server_config_default(config, "AccountMinLength", 4);
	server_config_default(config, "AccountMaxLength", 16);
	server_config_default(config, "PasswordMinLength", 6);
	server_config_default(config, "PasswordMaxLength", 12);
	server_config_default(config, "MuteLength", 90);
	server_config_default(config, "BaseBankSize", 100);
	server_config_default(config, "PacketRateWalk", 0.46);
	server_config_default(config, "PacketRateAttack", 0.58);
}

void server_config_validate_admin(Config& config)
{
	//Info commands
	server_config_default(config, "uptime", 1);

	//Player control commands
	server_config_default(config, "kick", 2);
	server_config_default(config, "hide", 2);
	server_config_default(config, "show", 2);
	server_config_default(config, "strip", 3);

	//Server control commands
	server_config_default(config, "rehash", 3);
	server_config_default(config, "request", 4);
	server_config_default(config, "shutdown", 4);

	//Debug commands
	server_config_default(config, "sitem", 3);
	server_config_default(config, "ditem", 3);
	server_config_default(config, "snpc", 3);
	server_config_default(config, "dnpc", 3);
	
	server_config_default(config, "setgender", 2);
	server_config_default(config, "sethairstyle", 2);
	server_config_default(config, "sethaircolor", 2);
	server_config_default(config, "setskincolor", 2);
	server_config_default(config, "setadmin", 4);

	//Other commands
	server_config_default(config, "noclip", 2);
}
