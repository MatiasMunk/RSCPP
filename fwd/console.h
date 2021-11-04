#ifndef FWD_CONSOLE
#define FWD_CONSOLE

#include <string>

namespace Console
{

	void Out(const char* f, ...);
	void Wrn(const char* f, ...);
	void Err(const char* f, ...);
	void Dbg(const char* f, ...);

};

#endif // FWD_CONFIG
