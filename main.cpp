#include <iostream>
#include <string>
#include <array>

#include "config.h"
#include "database.h"
#include "server_config.h"
#include "gameserver.h"
#include "world.h"

#include "console.h"
#include "socket_exception.h"

#include "version.h"

#include <ctime>

int main(int argc, char* argv[])
{
	try
	{
		Config config, aconfig;

		try
		{
			config.Read("config.ini");
		}
		catch (std::runtime_error)
		{
			Console::Wrn("Could not load config.ini - using defaults");
		}

		try
		{
			aconfig.Read("admin.ini");
		}
		catch (std::runtime_error)
		{
			Console::Err("Could not load admin.ini - using defaults");
		}

		server_config_validate_config(config);
		server_config_validate_admin(aconfig);

		Console::Styled[1] = Console::Styled[0] = config["StyleConsole"];

		puts("\
                            ___ ___ _____   _____ ___   \n\
   RSC++ V " SERVER_VERSION_STRING "\t   / __| __| _ \\ \\ / / __| _ \\     http://rscpp.net/\n\
=========================| \\__ \\ _||   /\\ V /| _||   / |===========================\n\
                           |___/___|_|_\\ \\_/ |___|_|_\\    \t\tCopyright (c)\n\
                                                         Software:\tMatias Persson\n");
#ifdef DEBUG
		Console::Wrn("This is a debug build and shouldn't be used for live servers.");
#endif

		{
			time_t rawtime;
			char timestr[256];
			time(&rawtime);
			std::strftime(timestr, 256, "%c", std::localtime(&rawtime));

			std::string logerr = config["LogErr"];
			if (!logerr.empty() && logerr.compare("-") != 0)
			{
				Console::Out("Redirecting errors to '%s'...", logerr.c_str());

				FILE* reopenedFile = freopen(logerr.c_str(), "a", stderr);
				if (!reopenedFile)
				{
					Console::Err("Failed to redirect errors.");
				}
				else
				{
					Console::Styled[Console::STREAM_ERR] = false;
					fprintf(stderr, "\n\n--- %s ---\n\n", timestr);
				}

				if (setvbuf(stderr, nullptr, _IONBF, 0) != 0)
				{
					Console::Wrn("Failed to change stderr buffer settings");
				}
			}

			std::string logout = config["LogOut"];
			if (!logout.empty() && logout.compare("-") != 0)
			{
				FILE* reopenedFile = freopen(logout.c_str(), "a", stdout);
				Console::Out("Redirecting output to '%s'...", logout.c_str());

				if (!reopenedFile)
				{
					Console::Err("Failed to redirect output.");
				}
				else
				{
					Console::Styled[Console::STREAM_OUT] = false;
					printf("\n\n--- %s ---\n\n", timestr);
				}

				if (setvbuf(stdout, nullptr, _IONBF, 0) != 0)
				{
					Console::Wrn("Failed to change stdout buffer settings");
				}
			}
		}

		std::array<std::string, 6> dbinfo;
		dbinfo[0] = std::string(config["DBType"]);
		dbinfo[1] = std::string(config["DBHost"]);
		dbinfo[2] = std::string(config["DBUser"]);
		dbinfo[3] = std::string(config["DBPass"]);
		dbinfo[4] = std::string(config["DBName"]);
		dbinfo[5] = std::string(config["DBPort"]);

		Gameserver server(static_cast<int>(config["Port"]), static_cast<int>(config["MaxConnections"]), dbinfo, config, aconfig);
		//server.Listen(int(config["MaxConnections"]), int(config["ListenBacklog"]));
		Console::Out("Listening on %s:%i (0/%i connections)", std::string(config["Host"]).c_str(), int(config["Port"]), int(config["MaxConnections"]));

		bool tables_exist = false;
		bool tried_install = false;

		while (!tables_exist)
		{
			bool try_install = false;

			try
			{
				/*Database_Result acc_count = server.world->db.Query("SELECT COUNT(1) AS `count` FROM `accounts`");
				Database_Result admin_character_count = server.world->db.Query("SELECT COUNT(1) AS `count` FROM `accounts` WHERE `admin` > 0");
				Database_Result guild_count = server.world->db.Query("SELECT COUNT(1) AS `count` FROM `clans`");

				Console::Out("Database info:");
				Console::Out("  Accounts:   %i (%i staff)", int(acc_count.front()["count"]), int(admin_character_count.front()["count"]));
				Console::Out("  Clans:     %i", int(guild_count.front()["count"]));

				server.world->UpdateAdminCount(int(admin_character_count.front()["count"]));*/

				tables_exist = true;
			}
			catch (Database_Exception & e)
			{
#ifdef DEBUG
				Console::Dbg("Install check: %s: %s", e.what(), e.error());
#endif // DEBUG

				if (tried_install)
				{
					Console::Err("Could not find or install tables.");
					Console::Err(e.error());
					exit(1);
				}

				try_install = true;
			}

			if (try_install)
			{
				tried_install = true;
				Console::Wrn("A required table is missing. Attempting to execute install.sql");

				try
				{
					server.world->CommitDB();
					server.world->db.ExecuteFile(config["InstallSQL"]);
					server.world->BeginDB();
				}
				catch (Database_Exception & e)
				{
					Console::Err("Could not install tables.");
					Console::Err(e.error());
					exit(1);
				}
			}
		}

		server.Tick();
	}
	catch (Socket_Exception & e)
	{
		Console::Err("%s: %s", e.what(), e.error());
		return 1;
	}
	catch (Database_Exception & e)
	{
		Console::Err("%s: %s", e.what(), e.error());
		return 1;
	}
	catch (std::runtime_error & e)
	{
		Console::Err("Runtime Error: %s", e.what());
		return 1;
	}
	catch (std::logic_error & e)
	{
		Console::Err("Logic Error: %s", e.what());
		return 1;
	}
	catch (std::exception & e)
	{
		Console::Err("Uncaught Exception: %s", e.what());
		return 1;
	}
	catch (...)
	{
		Console::Err("Uncaught Exception");
		return 1;
	}

	return 0;
}
