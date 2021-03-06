#include "console.h"

#include <cstdarg>
#include <cstdio>
#include <string>

namespace Console
{

	bool Styled[2] = { true, true };

	static const int ansi_color_map[] = { 0, 4, 2, 6, 1, 5, 3, 7, 0 };
	
	void SetTextColor(Stream stream, Color color, bool bold)
	{
		char command[8] = { 27, '[', '1', ';', '3', '0', 'm', 0 };

		if (bold)
		{
			command[5] += ansi_color_map[static_cast<int>(color)];
		}
		else
		{
			for (int i = 2; i < 6; ++i)
				command[i] = command[i + 2];

			command[3] += ansi_color_map[static_cast<int>(color)];
		}

		std::fputs(command, (stream == STREAM_OUT) ? stdout : stderr);
	}

	void ResetTextColor(Stream stream)
	{
		char command[5] = { 27, '[', '0', 'm', 0 };
		std::fputs(command, (stream == STREAM_OUT) ? stdout : stderr);
	}

#define CONSOLE_GENERIC_OUT(prefix, stream, color, bold) \
do { \
	if (Styled[stream]) SetTextColor(stream, color, bold); \
	va_list args; \
	va_start(args, f); \
	std::vfprintf((stream == STREAM_OUT) ? stdout : stderr, (std::string("[" prefix "] ") + f + "\n").c_str(), args); \
	va_end(args); \
	if (Styled[stream]) ResetTextColor(stream); \
} while (false)

	void Out(const char* f, ...)
	{
		CONSOLE_GENERIC_OUT("   ", STREAM_OUT, COLOR_GREY, true);
	}

	void Wrn(const char* f, ...)
	{
		CONSOLE_GENERIC_OUT("WRN", STREAM_OUT, COLOR_YELLOW, true);
	}

	void Err(const char* f, ...)
	{
		if (!Styled[STREAM_ERR])
		{
			CONSOLE_GENERIC_OUT("ERR", STREAM_OUT, COLOR_RED, true);
		}

		CONSOLE_GENERIC_OUT("ERR", STREAM_ERR, COLOR_RED, true);
	}

	void Dbg(const char* f, ...)
	{
		CONSOLE_GENERIC_OUT("DBG", STREAM_OUT, COLOR_GREY, false);
	}

}

